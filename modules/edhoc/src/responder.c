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
 * @brief   Parses message 1
 * @param   msg1 buffer containing message 1
 * @param   msg1_len length of msg1
 * @param   method_corr 4*method+corr
 * @param   suites_i
 * @param   suites_i_len length of suites_i
 * @param   g_x Public ephemeral key of the initiator
 * @param   g_x_len length of g_x
 * @param   c_i connection identifier of the initiator
 * @param   c_i_len length of c_i
 * @param   ad1 axillary data 1
 * @param   ad1_len length of ad1
 * @retval an EdhocError code
 */
static inline EdhocError msg1_parse(uint8_t *msg1, uint32_t msg1_len,
				    uint8_t *method_corr, uint8_t *suites_i,
				    uint64_t *suites_i_len, uint8_t *g_x,
				    uint64_t *g_x_len, uint8_t *c_i,
				    uint64_t *c_i_len, uint8_t *ad1,
				    uint64_t *ad1_len)
{
	EdhocError r;
	uint8_t *temp_ptr = msg1;
	uint8_t *next_temp_ptr;
	uint32_t temp_len = msg1_len;
	uint64_t method_len;
	CborType ignore;

	int tmp;
	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, &tmp, &method_len,
			 &ignore);
	*method_corr = tmp;
	if (r != EdhocNoError)
		return r;
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;
	PRINT_ARRAY("msg1 METHOD_CORR", method_corr, method_len);

	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, suites_i,
			 suites_i_len, &ignore);
	if (r != EdhocNoError)
		return r;
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;

	PRINT_ARRAY("msg1 SUITES_I", suites_i, *suites_i_len);

	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, g_x, g_x_len,
			 &ignore);
	if (r != EdhocNoError)
		return r;
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;
	PRINT_ARRAY("msg1 G_X", g_x, *g_x_len);

	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, c_i, c_i_len,
			 &ignore);
	if (r != EdhocNoError)
		return r;

	if (c_i_len[0] == 1) {
		/*C_I is encoded as bstr_identifier*/
		int t;
		uint64_t t_len = sizeof(t);
		r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, &t, &t_len,
				 &ignore);
		if (r != EdhocNoError)
			return r;
		*c_i = t + 24;
	}
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;
	PRINT_ARRAY("msg1 C_I", c_i, *c_i_len);

	if (temp_len) {
		r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, ad1,
				 ad1_len, &ignore);
		if (r != EdhocNoError)
			return r;
		temp_len -= (next_temp_ptr - temp_ptr);
		temp_ptr = next_temp_ptr;
		PRINT_ARRAY("msg1 AD_1", ad1, *ad1_len);
	}
	return EdhocNoError;
}

/**
 * @brief   checks if the selected (the first in the list received from the 
 *          initiator) ciphersute is supported
 * @param   selected the selected suite
 * @param   suites_r the list of suported ciphersuites
 * @retval  true if supported
 */
static inline bool selected_suite_is_supported(uint8_t selected,
					       struct byte_array *suites_r)
{
	for (uint8_t i = 0; i < suites_r->len; i++) {
		if ((suites_r->ptr[i] == selected))
			return true;
	}
	return false;
}

/**
 * @brief   Parses message 3
 * @param   corr correlation parameter
 * @param   msg3 buffer containing message 3
 * @param   msg3_len length of msg3
 * @param   c_r connection identifier of the responder
 * @param   c_r_len length of c_r
 * @param   ciphertext_3 ciphertext 3
 * @param   ciphertext_3_len length of ciphertext_3
 */
static inline EdhocError msg3_parse(uint8_t corr, uint8_t *msg3,
				    uint16_t msg3_len, uint8_t *c_r,
				    uint64_t *c_r_len, uint8_t *ciphertext_3,
				    uint64_t *ciphertext_3_len)
{
	uint8_t *temp_ptr = msg3;
	uint8_t *next_temp_ptr;
	uint32_t temp_len = msg3_len;
	CborType cbor_type;
	EdhocError r;

	if (corr != 2 && corr != 3) {
		/*C_R is present*/
		if (*c_r_len == 1) {
			/*C_R is encoded as int*/
			int buf;
			r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
					 &buf, c_r_len, &cbor_type);
			c_r[0] = (uint8_t)buf;
		} else {
			r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
					 c_r, c_r_len, &cbor_type);
		}
		if (r != EdhocNoError)
			return r;
		temp_len -= (next_temp_ptr - temp_ptr);
		temp_ptr = next_temp_ptr;
		PRINT_ARRAY("msg3 C_R", c_r, *c_r_len);
	}

	/*get cyphertext_2 or DIAG_MSG*/
	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, ciphertext_3,
			 ciphertext_3_len, &cbor_type);
	if (r != EdhocNoError)
		return r;
	if (cbor_type == CborTextStringType) {
		/*the error message and message 3 differ in the type of the second 
        element. In the error message the second element is DIAG_MSG : tstr and 
        in message 2 it is cyphertext_2 : bstr*/
		return ErrorMessageReceived;
	}
	PRINT_ARRAY("msg3 CIPHERTEXT_3", ciphertext_3, *ciphertext_3_len);
	return EdhocNoError;
};

/**
 * @brief   Encodes message 2
 * @param   corr corelation parameter
 * @param   c_i Connection identifier of the initiator
 * @param   c_i_len length of c_i
 * @param   g_y public ephemeral DH key of the responder 
 * @param   g_y_len length of g_y
 * @param   c_r connection identifier of the responder
 * @param   c_r_len length of c_r
 * @param   ciphertext_2 the ciphertext
 * @param   ciphertext_2_len length of ciphertext_2
 * @param   msg2 the encoded message
 * @param   msg2_len length of msg2
 * @retval  an EdhocError error code
 */
static inline EdhocError
msg2_encode(uint8_t corr, const uint8_t *c_i, uint8_t c_i_len,
	    const uint8_t *g_y, uint8_t g_y_len, const uint8_t *c_r,
	    uint8_t c_r_len, const uint8_t *ciphertext_2,
	    uint32_t ciphertext_2_len, uint8_t *msg2, uint32_t *msg2_len)
{
	CborEncoder enc;
	CborError r;
	cbor_encoder_init(&enc, msg2, *msg2_len, 0);

	if (corr != 1 && corr != 3) {
		if (c_i_len == 1) {
			r = cbor_encode_int(&enc, (*c_i - 24));
			if (r == CborErrorOutOfMemory)
				return CborEncodingBufferToSmall;
		} else {
			r = cbor_encode_byte_string(&enc, c_i, c_i_len);
			if (r == CborErrorOutOfMemory)
				return CborEncodingBufferToSmall;
		}
	}

	r = cbor_encode_byte_string(&enc, g_y, g_y_len);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;

	if (c_r_len == 1) {
		r = cbor_encode_int(&enc, (*c_r - 24));
		if (r == CborErrorOutOfMemory)
			return CborEncodingBufferToSmall;
	} else {
		r = cbor_encode_byte_string(&enc, c_r, c_i_len);
		if (r == CborErrorOutOfMemory)
			return CborEncodingBufferToSmall;
	}

	r = cbor_encode_byte_string(&enc, ciphertext_2, ciphertext_2_len);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;

	/* Get the CBOR length */
	*msg2_len = cbor_encoder_get_buffer_size(&enc, msg2);

	PRINT_ARRAY("message_2 (CBOR Sequence)", msg2, *msg2_len);
	return EdhocNoError;
}

EdhocError edhoc_responder_run(struct edhoc_responder_context *c,
			       struct other_party_cred *cred_i_array,
			       uint16_t num_cred_i, uint8_t *err_msg,
			       uint32_t *err_msg_len, uint8_t *ad_1,
			       uint64_t *ad_1_len, uint8_t *ad_3,
			       uint64_t *ad_3_len, uint8_t *prk_4x3m,
			       uint16_t prk_4x3m_len, uint8_t *th4,
			       uint16_t th4_len)
{
	EdhocError r;
	/******************** receive and process message 1 ***********************/
	uint8_t msg1[MSG_1_DEFAULT_SIZE];
	uint32_t msg1_len = sizeof(msg1);

	r = rx(msg1, &msg1_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("message_1 (CBOR Sequence)", msg1, msg1_len);

	uint8_t method_corr;
	uint8_t suites_i[5];
	uint64_t suites_i_len = sizeof(suites_i);
	uint8_t g_x[G_X_DEFAULT_SIZE];
	uint64_t g_x_len = sizeof(g_x);
	uint8_t c_i[C_I_DEFAULT_SIZE];
	uint64_t c_i_len = sizeof(c_i);

	r = msg1_parse(msg1, msg1_len, &method_corr, suites_i, &suites_i_len,
		       g_x, &g_x_len, c_i, &c_i_len, ad_1, ad_1_len);
	if (r != EdhocNoError)
		return r;

	if (!(selected_suite_is_supported(suites_i[0], &c->suites_r))) {
		r = tx_err_msg(RESPONDER, method_corr, c_i, c_i_len, NULL, 0,
			       c->suites_r.ptr, c->suites_r.len);
		if (r != EdhocNoError)
			return r;

		/*After an error message is sent the protocol must be discontinued*/
		return ErrorMessageSent;
	}

	/*get the method*/
	enum method_type method = method_corr >> 2;
	/*get corr*/
	uint8_t corr = method_corr - 4 * method;
	/*get cipher suite*/
	struct suite suite;
	r = get_suite((enum suite_label)suites_i[0], &suite);
	if (r != EdhocNoError)
		return r;

	bool static_dh_i, static_dh_r;
	authentication_type_get(method, &static_dh_i, &static_dh_r);

	/*********************** create and send message 2*************************/

	uint8_t th2[SHA_DEFAULT_SIZE];
	uint64_t tmp_c_i_len;
	if ((corr == 1) || (corr = 3)) {
		tmp_c_i_len = 0;
	} else {
		tmp_c_i_len = c_i_len;
	}

	r = th2_calculate(suite.edhoc_hash, msg1, msg1_len, c_i, tmp_c_i_len,
			  c->g_y.ptr, c->g_y.len, c->c_r.ptr, c->c_r.len, th2);
	if (r != EdhocNoError)
		return r;

	/*calculate the DH shared secret*/
	uint8_t g_xy[ECDH_SECRET_DEFAULT_SIZE];
	r = shared_secret_derive(suite.edhoc_ecdh_curve, c->y.ptr, c->y.len,
				 g_x, g_x_len, g_xy);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("G_XY (ECDH shared secret) ", g_xy, sizeof(g_xy));

	uint8_t PRK_3e2m[PRK_DEFAULT_SIZE];
	uint8_t ciphertext_2[CIPHERTEXT2_DEFAULT_SIZE];
	uint32_t ciphertext_2_len = sizeof(ciphertext_2);

	uint8_t PRK_2e[PRK_DEFAULT_SIZE];
	r = hkdf_extract(suite.edhoc_hash, NULL, 0, g_xy, sizeof(g_xy), PRK_2e);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("PRK_2e", PRK_2e, sizeof(PRK_2e));

	/*derive prk_3e2m*/
	r = prk_derive(static_dh_r, suite, PRK_2e, sizeof(PRK_2e), g_x, g_x_len,
		       c->r.ptr, c->r.len, PRK_3e2m);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("prk_3e2m", PRK_3e2m, sizeof(PRK_3e2m));

	uint8_t m_2[A_2M_DEFAULT_SIZE];
	uint16_t m_2_len = sizeof(m_2);
	uint8_t sign_or_mac_2[64];
	uint32_t sign_or_mac_2_len = sizeof(sign_or_mac_2);

	PRINT_ARRAY("CRED_R", c->cred_r.ptr, c->cred_r.len);

	r = signature_or_mac_msg_create(
		static_dh_r, suite, "K_2m", "IV_2m", (uint8_t *)&PRK_3e2m,
		sizeof(PRK_3e2m), (uint8_t *)&th2, sizeof(th2),
		c->id_cred_r.ptr, c->id_cred_r.len, c->cred_r.ptr,
		c->cred_r.len, c->ad_2.ptr, c->ad_2.len, m_2, &m_2_len,
		sign_or_mac_2, (uint8_t *)&sign_or_mac_2_len);
	if (r != EdhocNoError)
		return r;

	/*Signature_or_mac_2*/
	if (!static_dh_r) {
		/*Calculate a signature*/
		sign_or_mac_2_len = 64;
		r = sign(suite.edhoc_sign_curve, c->sk_r.ptr, c->sk_r.len,
			 c->pk_r.ptr, c->pk_r.len, m_2, m_2_len, sign_or_mac_2,
			 &sign_or_mac_2_len);
		if (r != EdhocNoError)
			return r;
		PRINT_ARRAY("Signature_or_MAC_2", sign_or_mac_2,
			    sign_or_mac_2_len);
	}

	/*Calculate P_2e*/
	uint16_t P_2e_len =
		c->id_cred_r.len + sign_or_mac_2_len + 2 + c->ad_2.len;
	uint8_t P_2e[P_2e_len];

	r = plaintext_encode(c->id_cred_r.ptr, c->id_cred_r.len, sign_or_mac_2,
			     sign_or_mac_2_len, c->ad_2.ptr, c->ad_2.len, P_2e,
			     &P_2e_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("P_2e", P_2e, P_2e_len);

	/*Calculate KEYSTREAM_2*/
	uint8_t KEYSTREAM_2[P_2e_len];
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, "KEYSTREAM_2",
		     (uint8_t *)&PRK_2e, sizeof(PRK_2e), (uint8_t *)&th2,
		     sizeof(th2), KEYSTREAM_2, sizeof(KEYSTREAM_2));
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("KEYSTREAM_2", KEYSTREAM_2, sizeof(KEYSTREAM_2));

	/*Ciphertext 2 calculate*/
	ciphertext_2_len = P_2e_len;
	for (uint16_t i = 0; i < P_2e_len; i++) {
		ciphertext_2[i] = P_2e[i] ^ KEYSTREAM_2[i];
	}
	PRINT_ARRAY("ciphertext_2", ciphertext_2, ciphertext_2_len);

	/*message 2 create and send*/
	uint8_t msg2[MSG_2_DEFAULT_SIZE];
	uint32_t msg2_len = sizeof(msg2);
	r = msg2_encode(corr, c_i, c_i_len, c->g_y.ptr, c->g_y.len, c->c_r.ptr,
			c->c_r.len, ciphertext_2, ciphertext_2_len, msg2,
			&msg2_len);
	if (r != EdhocNoError)
		return r;
	r = tx(msg2, msg2_len);
	if (r != EdhocNoError)
		return r;

	/********message 3 receive and process*********************************/
	uint8_t msg3[MSG_3_DEFAULT_SIZE];
	uint32_t msg3_len = sizeof(msg3);
	r = rx(msg3, &msg3_len);
	if (r != EdhocNoError)
		return r;

	uint8_t c_r[c->c_r.len];
	uint64_t c_r_len = sizeof(c_r);
	uint8_t ciphertext_3[CIPHERTEXT3_DEFAULT_SIZE];
	uint64_t ciphertext_3_len = sizeof(ciphertext_3);

	r = msg3_parse(corr, msg3, msg3_len, c_r, &c_r_len, ciphertext_3,
		       &ciphertext_3_len);
	if (r == ErrorMessageReceived) {
		/*provide the error message to the caller*/;
		r = _memcpy_s(err_msg, *err_msg_len, msg3, msg3_len);
		if (r != EdhocNoError)
			return r;
		*err_msg_len = msg3_len;
		return ErrorMessageReceived;
	}
	if (r != EdhocNoError)
		return r;

	uint8_t th3[32];
	r = th3_calculate(suite.edhoc_hash, (uint8_t *)&th2, sizeof(th2),
			  ciphertext_2, ciphertext_2_len, c->c_r.ptr,
			  c->c_r.len, th3);
	if (r != EdhocNoError)
		return r;

	uint8_t K_3ae[AEAD_KEY_DEFAULT_SIZE];
	uint8_t IV_3ae[AEAD_IV_DEFAULT_SIZE];

	/*Calculate K_3ae*/
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, "K_3ae", PRK_3e2m,
		     sizeof(PRK_3e2m), (uint8_t *)&th3, sizeof(th3), K_3ae,
		     sizeof(K_3ae));
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("K_3ae", K_3ae, sizeof(K_3ae));

	/*Calculate IV_3ae*/
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

	uint8_t tag[16];
	uint8_t mac_len;
	if (suite.edhoc_aead == AES_CCM_16_64_128) {
		mac_len = 8;
	} else {
		mac_len = 16;
	}
	uint8_t P_3ae[ciphertext_3_len - mac_len];
	//memcpy(tag, &ciphertext_3[ciphertext_3_len - mac_len], mac_len);
	r = _memcpy_s(tag, sizeof(tag),
		      &ciphertext_3[ciphertext_3_len - mac_len], mac_len);
	if (r != EdhocNoError)
		return r;
	r = aead(DECRYPT, ciphertext_3, ciphertext_3_len, K_3ae, sizeof(K_3ae),
		 IV_3ae, sizeof(IV_3ae), A_3ae, A_3ae_len, P_3ae, sizeof(P_3ae),
		 tag, mac_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("P_3ae", P_3ae, sizeof(P_3ae));

	uint8_t id_cred_i[ID_CRED_DEFAULT_SIZE];
	uint64_t id_cred_i_len = sizeof(id_cred_i);
	uint8_t sign_or_mac[SGN_OR_MAC_DEFAULT_SIZE];
	uint64_t sign_or_mac_len = sizeof(sign_or_mac);
	r = plaintext_split(P_3ae, sizeof(P_3ae), id_cred_i, &id_cred_i_len,
			    sign_or_mac, &sign_or_mac_len, ad_3, ad_3_len);
	if (r != EdhocNoError)
		return r;

	PRINT_ARRAY("ID_CRED_I", id_cred_i, id_cred_i_len);
	PRINT_ARRAY("sign_or_mac", sign_or_mac, sign_or_mac_len);
	if (*ad_3_len) {
		PRINT_ARRAY("AD_3", ad_3, *ad_3_len);
	}

	/*check the authenticity of the responder*/
	uint8_t *cred_i; //[CRED_DEFAULT_SIZE];
	uint16_t cred_i_len;
	uint8_t *pk;
	uint16_t pk_len = 0;
	uint8_t *g_i;
	uint16_t g_i_len = 0;

	r = retrieve_cred(static_dh_i, cred_i_array, num_cred_i, id_cred_i,
			  id_cred_i_len, &cred_i, &cred_i_len, &pk, &pk_len,
			  &g_i, &g_i_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("CRED_I", cred_i, cred_i_len);
	PRINT_ARRAY("pk", pk, pk_len);
	PRINT_ARRAY("g_i", g_i, g_i_len);

	/*derive prk_4x3m*/
	r = prk_derive(static_dh_i, suite, (uint8_t *)&PRK_3e2m,
		       sizeof(PRK_3e2m), g_i, g_i_len, c->y.ptr, c->y.len,
		       prk_4x3m);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("prk_4x3m", prk_4x3m, prk_4x3m_len);

	uint8_t m_3[A_2M_DEFAULT_SIZE];
	uint16_t m_3_len = sizeof(m_3);
	uint8_t mac_3[16];
	uint8_t mac_3_len = sizeof(mac_3);
	r = signature_or_mac_msg_create(static_dh_i, suite, "K_3m", "IV_3m",
					prk_4x3m, prk_4x3m_len, (uint8_t *)&th3,
					sizeof(th3), id_cred_i, id_cred_i_len,
					cred_i, cred_i_len, ad_3, *ad_3_len,
					m_3, &m_3_len, mac_3, &mac_3_len);
	if (r != EdhocNoError)
		return r;

	if (static_dh_i) {
		/*check inner mac MAC_3*/
		if (0 != memcmp(mac_3, sign_or_mac, mac_3_len)) {
			PRINT_MSG("Initiator authentication failed!");
			r = tx_err_msg(RESPONDER, corr, c_i, c_i_len, NULL, 0,
				       NULL, 0);
			if (r != EdhocNoError)
				return r;
			return ResponderAuthenticationFailed;
		} else {
			PRINT_MSG("Initiator authentication successful!\n");
		}
	} else {
		/*the initiator authenticates with a signature*/
		bool verified = false;
		r = verify(suite.edhoc_sign_curve, pk, pk_len, (uint8_t *)&m_3,
			   m_3_len, (uint8_t *)&sign_or_mac, sign_or_mac_len,
			   &verified);
		if (verified) {
			PRINT_MSG("Initiator authentication successful!\n");
		} else {
			PRINT_MSG("Initiator authentication failed!\n");
			r = tx_err_msg(RESPONDER, corr, c_i, c_i_len, NULL, 0,
				       NULL, 0);
			if (r != EdhocNoError)
				return r;
			return ResponderAuthenticationFailed;
		}
	}

	/*TH4*/
	return th4_calculate(suite.edhoc_hash, th3, sizeof(th3), ciphertext_3,
			     ciphertext_3_len, th4);
}
