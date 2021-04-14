/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include <cbor.h>

#include "../edhoc.h"
#include "../inc/a_Xae_encode.h"
#include "../inc/cbor_decoder.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/err_msg.h"
#include "../inc/error.h"
#include "../inc/hkdf_info.h"
#include "../inc/memcpy_s.h"
#include "../inc/messages.h"
#include "../inc/okm.h"
#include "../inc/plaintext.h"
#include "../inc/print_util.h"
#include "../inc/prk.h"
#include "../inc/retrieve_cred.h"
#include "../inc/signature_or_mac_msg.h"
#include "../inc/suites.h"
#include "../inc/th.h"
#include "../inc/txrx_wrapper.h"

/**
 * @brief   Encodes a connection identifier C_x of length one byte to 
 *          bstr_identifier
 * @param   c_x connection identifier
 * @param   pointer to the output buffer
 */
static inline EdhocError c_x_bstr_identifier_encode(uint8_t c_x, uint8_t *out)
{
	CborEncoder enc;
	cbor_encoder_init(&enc, out, 1, 0);
	int64_t t = c_x - 24;
	CborError r = cbor_encode_int(&enc, t);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;
	return EdhocNoError;
}

/**
 * @brief   Parses message 2
 * @param   c initiator context
 * @param   msg2 pointer to a buffer containign message 2 
 * @param   msg2_len the length of the raw message
 * @param   msg2_struct pointer to a structure for the parsed message
 * @param   err_msg pointer to an error message structure
 */
static inline EdhocError
msg2_parse(const struct edhoc_initiator_context *c, uint8_t *msg2,
	   uint32_t msg2_len, uint8_t *c_i, uint64_t *c_i_len, uint8_t *g_y,
	   uint64_t *g_y_len, uint8_t *c_r, uint64_t *c_r_len,
	   uint8_t *ciphertext2, uint64_t *ciphertext2_len)
{
	uint8_t *temp_ptr = msg2;
	uint8_t *next_temp_ptr;
	uint32_t temp_len = msg2_len;
	EdhocError r;
	CborType cbor_type;

	if (!(c->corr == 1 || c->corr == 3)) {
		/*C_I (the connection identifier of the initiator) is present*/
		r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, c_i,
				 c_i_len, &cbor_type);
		if (r != EdhocNoError)
			return r;
		temp_len -= (next_temp_ptr - temp_ptr);
		temp_ptr = next_temp_ptr;
		PRINT_ARRAY("msg2 C_I", c_i, *c_i_len);
	} else {
		*c_i_len = 0;
	}

	/*get the ephemeral public key of the responder (G_Y) or ERR_MSG*/
	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, g_y, g_y_len,
			 &cbor_type);
	if (r != EdhocNoError)
		return r;
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;
	PRINT_ARRAY("msg2 G_Y", g_y, *g_y_len);

	if (cbor_type == CborTextStringType) {
		/*the error message and message 2 differ in the type of the second 
        element. In the error message the second element is DIAG_MSG : tstr and 
        in message 2 it is G_Y : bstr*/
		return ErrorMessageReceived;
	} else {
		/*get the connection identifier of the responder C_R */
		/*c_r is encoded as bstr_identifier, i.e., byte string of length 1 is
		 * encoded as integer -24, else c_r is a byte string.
		 * On different platforms int has different size so we dump c_r first
		 * in a int buffer and then we check if it is string or int*/

		if (*c_r_len > 1) {
			r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
					 c_r, c_r_len, &cbor_type);
			if (r != EdhocNoError)
				return r;
		} else {
			int buf;
			r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
					 &buf, c_r_len, &cbor_type);
			if (r != EdhocNoError)
				return r;
			if (*c_r_len > 1)
				return C_R_lengthtosmall;
			*c_r = buf + 24;
		}

		PRINT_ARRAY("msg2 C_R", c_r, *c_r_len);
		temp_len -= (next_temp_ptr - temp_ptr);
		temp_ptr = next_temp_ptr;

		/*get ciphertext_2*/
		r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
				 ciphertext2, ciphertext2_len, &cbor_type);
		if (r != EdhocNoError)
			return r;
		PRINT_ARRAY("msg2 CIPHERTEXT_2", ciphertext2, *ciphertext2_len);
		return EdhocNoError;
	}
}
/**
 * @brief   Encodes message 1
 * @param   c initiator context
 * @param   msg1 pointer to a buffer for holding the encoded message
 * @param   msg1_len length of the encoded message
 */
static inline EdhocError msg1_encode(const struct edhoc_initiator_context *c,
				     uint8_t *msg1, uint32_t *msg1_len)
{
	CborEncoder msg1_enc, enc;
	CborError r;
	uint32_t tmp_len = *msg1_len;
	cbor_encoder_init(&msg1_enc, msg1, *msg1_len, 0);

	/*METHOD_CORR*/
	r = cbor_encode_int(&msg1_enc, (4 * c->method_type + c->corr));
	if (r != CborNoError)
		return CborEncodingError;

	/*SUITES_I*/
	if (c->suites_i.len == 1) {
		/* only one suite, encode into int */
		r = cbor_encode_int(&msg1_enc, c->suites_i.ptr[0]);
		if (r != CborNoError)
			return CborEncodingError;

	} else {
		/* more than one suites, encode into array */
		CborEncoder suites_array_enc;
		r = cbor_encoder_create_array(&msg1_enc, &suites_array_enc,
					      c->suites_i.len);
		if (r != CborNoError)
			return CborEncodingError;
		for (uint8_t i = 0; i < c->suites_i.len; i++) {
			r = cbor_encode_int(&suites_array_enc,
					    c->suites_i.ptr[i]);
			if (r != CborNoError)
				return CborEncodingError;
		}
		cbor_encoder_close_container(&msg1_enc, &suites_array_enc);
		if (r != CborNoError)
			return CborEncodingError;
	}

	/* G_X ephemeral public key, bstr */
	r = cbor_encode_byte_string(&msg1_enc, c->g_x.ptr, c->g_x.len);
	if (r != CborNoError)
		return CborEncodingError;

	/* C_I connection id, encoded as  bstr_identifier */
	if (c->c_i.len == 1) {
		r = cbor_encode_int(&msg1_enc, (*c->c_i.ptr - 24));
		if (r != CborNoError)
			return CborEncodingError;
	} else {
		r = cbor_encode_byte_string(&msg1_enc, c->c_i.ptr, c->c_i.len);
		if (r != CborNoError)
			return CborEncodingError;
	}

	*msg1_len = cbor_encoder_get_buffer_size(&msg1_enc, msg1);
	if (c->ad_1.len != 0) {
		/* AD_1 unprotected opaque auxiliary data */
		tmp_len -= *msg1_len;
		cbor_encoder_init(&enc, (msg1 + *msg1_len), tmp_len, 0);
		r = cbor_encode_byte_string(&enc, c->ad_1.ptr, c->ad_1.len);
		if (r != CborNoError)
			return CborEncodingError;
		*msg1_len += cbor_encoder_get_buffer_size(&enc, msg1);
	} else {
		/* Get the CBOR length */
		*msg1_len = cbor_encoder_get_buffer_size(&msg1_enc, msg1);
	}

	PRINT_ARRAY("message_1 (CBOR Sequence)", msg1, *msg1_len);
	return EdhocNoError;
}

EdhocError edhoc_initiator_run(const struct edhoc_initiator_context *c,
			       struct other_party_cred *cred_r_array,
			       uint16_t num_cred_r, uint8_t *err_msg,
			       uint32_t *err_msg_len, uint8_t *ad_2,
			       uint64_t *ad_2_len, uint8_t *prk_4x3m,
			       uint8_t prk_4x3m_len, uint8_t *th4,
			       uint8_t th4_len)
{
	EdhocError r;

	struct suite suite;
	bool auth_method_static_dh_i = false, auth_method_static_dh_r = false;
	r = get_suite((enum suite_label)c->suites_i.ptr[0], &suite);
	if (r != EdhocNoError)
		return r;

	authentication_type_get(c->method_type, &auth_method_static_dh_i,
				&auth_method_static_dh_r);

	uint8_t msg1[MSG_1_DEFAULT_SIZE];
	uint32_t msg1_len = sizeof(msg1);
	uint8_t msg2[MSG_2_DEFAULT_SIZE];
	uint32_t msg2_len = sizeof(msg2);

	uint8_t c_i[c->c_i.len];
	uint64_t c_i_len = sizeof(c_i);

	/*in a given selected cipher suite the length of G_X and G_Y is equal*/
	uint8_t g_y[c->g_x.len];
	uint64_t g_y_len = sizeof(g_y);

	uint8_t c_r[C_R_DEFAULT_SIZE];
	uint64_t c_r_len = sizeof(c_r);

	uint8_t ciphertext2[CIPHERTEXT2_DEFAULT_SIZE];
	uint64_t ciphertext2_len = sizeof(ciphertext2);

	r = msg1_encode(c, (uint8_t *)&msg1, &msg1_len);
	if (r != EdhocNoError)
		return r;

	r = tx(msg1, msg1_len);
	if (r != EdhocNoError)
		return r;

	/**********************receive and process msg2 ***************************/

	r = rx(msg2, &msg2_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("message_2 (CBOR Sequence)", msg2, msg2_len);

	/* If an error message is received msg2_parse will return 
    ErrorMessageReceived. If this hapens edhoc_initiator_run will return. Then
    the caller needs to examine SUITES_R in err_msg re-initialize the initiator 
    and call edhoc_initiator_run again*/
	r = msg2_parse(c, msg2, msg2_len, c_i, &c_i_len, g_y, &g_y_len, c_r,
		       &c_r_len, ciphertext2, &ciphertext2_len);
	if (r == ErrorMessageReceived) {
		/*provide the error message to the caller*/;
		r = _memcpy_s(err_msg, *err_msg_len, msg2, msg2_len);
		if (r != EdhocNoError)
			return r;
		*err_msg_len = msg2_len;
		return ErrorMessageReceived;
	}
	if (r != EdhocNoError)
		return r;

	/*calculate the DH shared secret*/
	uint8_t g_xy[ECDH_SECRET_DEFAULT_SIZE];
	r = shared_secret_derive(suite.edhoc_ecdh_curve, c->x.ptr, c->x.len,
				 g_y, g_y_len, g_xy);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("G_XY (ECDH shared secret) ", g_xy, sizeof(g_xy));

	/*calculate th2*/
	uint8_t th2[SHA_DEFAULT_SIZE];
	r = th2_calculate(suite.edhoc_hash, msg1, msg1_len, c_i, c_i_len, g_y,
			  sizeof(g_y), c_r, c_r_len, th2);
	if (r != 0)
		return r;

	/*calculate PRK_2e*/
	uint8_t PRK_2e[PRK_DEFAULT_SIZE];
	r = hkdf_extract(suite.edhoc_hash, NULL, 0, g_xy, sizeof(g_xy), PRK_2e);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("PRK_2e", PRK_2e, sizeof(PRK_2e));

	/*Derive KEYSTREAM_2*/
	uint64_t KEYSTREAM_2_len = ciphertext2_len;
	uint8_t KEYSTREAM_2[KEYSTREAM_2_len];
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, "KEYSTREAM_2",
		     (uint8_t *)&PRK_2e, sizeof(PRK_2e), (uint8_t *)&th2,
		     sizeof(th2), KEYSTREAM_2, KEYSTREAM_2_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("KEYSTREAM_2", KEYSTREAM_2, sizeof(KEYSTREAM_2));

	uint8_t P_2e[ciphertext2_len];
	for (uint16_t i = 0; i < KEYSTREAM_2_len; i++) {
		P_2e[i] = ciphertext2[i] ^ KEYSTREAM_2[i];
	}

	PRINT_ARRAY("P_2e", P_2e, sizeof(P_2e));

	uint8_t sign_or_mac[SGN_OR_MAC_DEFAULT_SIZE];
	uint64_t sign_or_mac_len = sizeof(sign_or_mac);
	uint8_t id_cred_r[ID_CRED_DEFAULT_SIZE];
	uint64_t id_cred_r_len = sizeof(id_cred_r);

	r = plaintext_split(P_2e, sizeof(P_2e), id_cred_r, &id_cred_r_len,
			    sign_or_mac, &sign_or_mac_len, ad_2, ad_2_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("ID_CRED_R", id_cred_r, id_cred_r_len);
	PRINT_ARRAY("sign_or_mac", sign_or_mac, sign_or_mac_len);
	if (*ad_2_len) {
		PRINT_ARRAY("AD_2", ad_2, *ad_2_len);
	}

	/*check the authenticity of the responder*/
	uint8_t *cred_r;
	uint16_t cred_r_len = 0;
	uint8_t *pk;
	uint16_t pk_len = 0;
	uint8_t *g_r;
	uint16_t g_r_len = 0;

	r = retrieve_cred(auth_method_static_dh_r, cred_r_array, num_cred_r,
			  id_cred_r, id_cred_r_len, &cred_r, &cred_r_len, &pk,
			  &pk_len, &g_r, &g_r_len);
	if (r != EdhocNoError)
		return r;

	PRINT_ARRAY("CRED_R", cred_r, cred_r_len);
	PRINT_ARRAY("pk", pk, pk_len);
	PRINT_ARRAY("g_r", g_r, g_r_len);

	uint8_t PRK_3e2m[PRK_DEFAULT_SIZE];
	/*derive prk_3e2m*/
	r = prk_derive(auth_method_static_dh_r, suite, PRK_2e, sizeof(PRK_2e),
		       g_r, g_r_len, c->x.ptr, c->x.len, PRK_3e2m);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("prk_3e2m", PRK_3e2m, sizeof(PRK_3e2m));

	uint8_t m_2[A_2M_DEFAULT_SIZE];
	uint16_t m_2_len = sizeof(m_2);
	uint8_t mac_2[16];
	uint8_t mac_2_len = sizeof(mac_2);
	r = signature_or_mac_msg_create(auth_method_static_dh_r, suite, "K_2m",
					"IV_2m", (uint8_t *)&PRK_3e2m,
					sizeof(PRK_3e2m), (uint8_t *)&th2,
					sizeof(th2), id_cred_r, id_cred_r_len,
					cred_r, cred_r_len, ad_2, *ad_2_len,
					m_2, &m_2_len, mac_2, &mac_2_len);
	if (r != EdhocNoError)
		return r;

	if (auth_method_static_dh_r) {
		if (!memcmp(mac_2, sign_or_mac, mac_2_len)) {
			PRINT_MSG("Responder authentication successful!\n");
		} else {
			r = tx_err_msg(INITIATOR, c->corr, c_r, c_r_len, NULL,
				       0, NULL, 0);
			if (r != EdhocNoError)
				return r;
			return ResponderAuthenticationFailed;
		}
	} else {
		/*the responder authenticates with a signature*/
		bool verified = false;
		r = verify(suite.edhoc_sign_curve, pk, pk_len, (uint8_t *)&m_2,
			   m_2_len, (uint8_t *)&sign_or_mac, sign_or_mac_len,
			   &verified);
		if (verified) {
			PRINT_MSG("Responder authentication successful!\n");
		} else {
			r = tx_err_msg(INITIATOR, c->corr, c_r, c_r_len, NULL,
				       0, NULL, 0);
			if (r != EdhocNoError)
				return r;
			return ResponderAuthenticationFailed;
		}
	}

	/********msg3 create and send**********************************************/

	uint8_t *data_3;
	uint8_t data_3_len;
	if (c->corr == 2 || c->corr == 3) {
		data_3 = NULL;
		data_3_len = 0;
	} else {
		data_3 = c_r;
		data_3_len = c_r_len;
	}
	uint8_t th3[32];
	r = th3_calculate(suite.edhoc_hash, (uint8_t *)&th2, sizeof(th2),
			  ciphertext2, ciphertext2_len, data_3, data_3_len,
			  th3);
	if (r != EdhocNoError)
		return r;

	/*derive prk_4x3m*/
	r = prk_derive(auth_method_static_dh_i, suite, (uint8_t *)&PRK_3e2m,
		       sizeof(PRK_3e2m), g_y, sizeof(g_y), c->i.ptr, c->i.len,
		       prk_4x3m);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("prk_4x3m", prk_4x3m, prk_4x3m_len);

	uint8_t m_3[M_3_DEFAULT_SIZE];
	uint16_t m_3_len = sizeof(m_3);
	uint8_t sign_or_mac_3[64];
	uint32_t sign_or_mac_3_len = sizeof(sign_or_mac_3);
	r = signature_or_mac_msg_create(
		auth_method_static_dh_i, suite, "K_3m", "IV_3m", prk_4x3m,
		prk_4x3m_len, (uint8_t *)&th3, sizeof(th3), c->id_cred_i.ptr,
		c->id_cred_i.len, c->cred_i.ptr, c->cred_i.len, c->ad_3.ptr,
		c->ad_3.len, m_3, &m_3_len, sign_or_mac_3,
		(uint8_t *)&sign_or_mac_3_len);
	if (r != EdhocNoError)
		return r;

	/*Calculate signature if the initiator authenticates with signature*/
	if (!auth_method_static_dh_i) {
		/*Calculate a signature*/
		sign_or_mac_3_len = sizeof(sign_or_mac_3);
		r = sign(suite.edhoc_sign_curve, c->sk_i.ptr, c->sk_i.len,
			 c->pk_i.ptr, c->pk_i.len, m_3, m_3_len, sign_or_mac_3,
			 &sign_or_mac_3_len);
		if (r != EdhocNoError)
			return r;
		PRINT_ARRAY("Signature_or_MAC_3", sign_or_mac_3,
			    sign_or_mac_3_len);
	}

	/*Calculate P_3ae*/
	uint8_t sign_or_mac_3_enc[sign_or_mac_3_len + 2];
	uint16_t sign_or_mac_3_enc_len = sizeof(sign_or_mac_3_enc);
	r = encode_byte_string(sign_or_mac_3, sign_or_mac_3_len,
			       sign_or_mac_3_enc, &sign_or_mac_3_enc_len);
	if (r != EdhocNoError)
		return r;

	uint8_t kid_buf[KID_DEFAULT_SIZE];
	uint32_t kid_len = sizeof(kid_buf);
	id_cred2kid(c->id_cred_i.ptr, c->id_cred_i.len, kid_buf, &kid_len);

	uint8_t P_3ae[PRK_3AE_DEFAULT_SIZE];
	uint32_t P_3ae_len = sizeof(P_3ae);

	if (kid_len != 0) {
		r = _memcpy_s(P_3ae, P_3ae_len, kid_buf, kid_len);
		if (r != EdhocNoError)
			return r;
		r = _memcpy_s(P_3ae + kid_len, P_3ae_len - kid_len,
			      sign_or_mac_3_enc, sign_or_mac_3_enc_len);
		if (r != EdhocNoError)
			return r;
		P_3ae_len = sign_or_mac_3_enc_len + kid_len;
	} else {
		r = _memcpy_s(P_3ae, P_3ae_len, c->id_cred_i.ptr,
			      c->id_cred_i.len);
		if (r != EdhocNoError)
			return r;
		r = _memcpy_s(P_3ae + c->id_cred_i.len,
			      P_3ae_len - c->id_cred_i.len, sign_or_mac_3_enc,
			      sign_or_mac_3_enc_len);
		if (r != EdhocNoError)
			return r;
		P_3ae_len = c->id_cred_i.len + sign_or_mac_3_enc_len;
	}
	PRINT_ARRAY("P_3ae", P_3ae, P_3ae_len);

	/*Calculate K_3ae*/
	uint8_t K_3ae[16];
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, "K_3ae", PRK_3e2m,
		     sizeof(PRK_3e2m), (uint8_t *)&th3, sizeof(th3), K_3ae,
		     sizeof(K_3ae));
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("K_3ae", K_3ae, sizeof(K_3ae));

	/*Calculate IV_3ae*/
	uint8_t IV_3ae[13];
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, "IV_3ae", PRK_3e2m,
		     sizeof(PRK_3e2m), (uint8_t *)&th3, sizeof(th3), IV_3ae,
		     sizeof(IV_3ae));
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("IV_3ae", IV_3ae, sizeof(IV_3ae));

	/*Associated data A_3ae*/
	uint8_t A_3ae[A_3AE_DEFAULT_SIZE];
	uint32_t A_3ae_len = sizeof(A_3ae);
	r = a_Xae_encode(th3, sizeof(th3), (uint8_t *)&A_3ae, &A_3ae_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("A_3ae", A_3ae, A_3ae_len);

	/*Ciphertext 3 calculate*/
	uint8_t mac_len = 8;
	if (suite.edhoc_aead == AES_CCM_16_128_128) {
		mac_len = 16;
	}
	uint8_t tag[mac_len];
	uint32_t ciphertext_3_len = P_3ae_len;
	uint8_t ciphertext_3[ciphertext_3_len + mac_len];
	
	r = aead(ENCRYPT, P_3ae, P_3ae_len, K_3ae, sizeof(K_3ae), IV_3ae,
		 sizeof(IV_3ae), A_3ae, A_3ae_len, ciphertext_3,
		 ciphertext_3_len, tag, mac_len);
	if (r != EdhocNoError)
		return r;

	PRINT_ARRAY("ciphertext_3", ciphertext_3, sizeof(ciphertext_3));

	/*massage 3 create and send*/
	uint8_t ciphertext_3_enc[sizeof(ciphertext_3) + 2];
	uint16_t ciphertext_3_enc_len = sizeof(ciphertext_3_enc);
	r = encode_byte_string(ciphertext_3, sizeof(ciphertext_3),
			       ciphertext_3_enc, &ciphertext_3_enc_len);
	if (r != EdhocNoError)
		return r;

	uint16_t msg3_len = ciphertext_3_enc_len + c_r_len;
	uint8_t msg3[msg3_len];
	if (c_r_len == 1) {
		uint8_t c_r_bstr_ident;
		r = c_x_bstr_identifier_encode(c_r[0], &c_r_bstr_ident);
		if (r != EdhocNoError)
			return r;
		memcpy(msg3, &c_r_bstr_ident, 1);
		memcpy(msg3 + 1, ciphertext_3_enc, ciphertext_3_enc_len);
		msg3_len = ciphertext_3_enc_len + 1;
	} else {
		memcpy(msg3, c_r, c_r_len);
		memcpy(msg3 + c_r_len, ciphertext_3_enc, ciphertext_3_enc_len);
	}

	PRINT_ARRAY("msg3", msg3, msg3_len);

	r = tx(msg3, msg3_len);
	if (r != EdhocNoError)
		return r;

	/*TH4*/
	r = th4_calculate(suite.edhoc_hash, th3, sizeof(th3), ciphertext_3,
			  sizeof(ciphertext_3), th4);
	if (r != EdhocNoError)
		return r;

	return EdhocNoError;
}
