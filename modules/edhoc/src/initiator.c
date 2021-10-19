/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include "../edhoc.h"
#include "../inc/associated_data_encode.h"
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
#include "../inc/c_x.h"
#include "../inc/ciphertext.h"
#include "../cbor/encode_message_1.h"
#include "../cbor/decode_message_2.h"
#include "../cbor/encode_message_3.h"
//#include "../cbor/decode_message_2_c_i.h"

/**
 * @brief   Parses message 2
 * @param   c initiator context
 * @param   msg2 pointer to a buffer containign message 2 
 * @param   msg2_len the length of the raw message
 * @param   msg2_struct pointer to a structure for the parsed message
 * @param   err_msg pointer to an error message structure
 */
static inline enum edhoc_error
msg2_parse(const struct edhoc_initiator_context *c, uint8_t *msg2,
	   uint32_t msg2_len, uint8_t *g_y, uint64_t g_y_len, struct c_x *c_r,
	   uint8_t *ciphertext2, uint64_t *ciphertext2_len)
{
	bool ok;
	size_t decode_len = 0;
	enum edhoc_error r;
	struct m2 m;

	ok = cbor_decode_m2(msg2, msg2_len, &m, &decode_len);
	if (!ok) {
		return cbor_decoding_error;
	}

	r = _memcpy_s(g_y, g_y_len, m._m2_G_Y_CIPHERTEXT_2.value, g_y_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("g_y", g_y, g_y_len);

	r = _memcpy_s(ciphertext2, *ciphertext2_len,
		      m._m2_G_Y_CIPHERTEXT_2.value + g_y_len,
		      m._m2_G_Y_CIPHERTEXT_2.len - g_y_len);
	if (r != edhoc_no_error) {
		return r;
	}
	*ciphertext2_len = m._m2_G_Y_CIPHERTEXT_2.len - g_y_len;
	PRINT_ARRAY("ciphertext2", ciphertext2, *ciphertext2_len);

	if (m._m2_C_R_choice == _m2_C_R_int) {
		r = c_x_set(INT, NULL, 0, m._m2_C_R_int, c_r);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINTF("C_R is an int: %d", c_r->mem.c_x_int);
	} else {
		r = c_x_set(BSTR, m._m2_C_R_bstr.value, m._m2_C_R_bstr.len, 0,
			    c_r);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("C_R_raw bstr", c_r->mem.c_x_bstr.ptr,
			    c_r->mem.c_x_bstr.len);
	}

	return edhoc_no_error;
}
/**
 * @brief   Encodes message 1
 * @param   c initiator context
 * @param   msg1 pointer to a buffer for holding the encoded message
 * @param   msg1_len length of the encoded message
 */
static inline enum edhoc_error
msg1_encode(const struct edhoc_initiator_context *c, uint8_t *msg1,
	    uint32_t *msg1_len)
{
	bool ok;
	struct message_1 m1;

	/*METHOD_CORR*/
	m1._message_1_METHOD = c->method;

	/*SUITES_I*/
	if (c->suites_i.len == 1) {
		/* only one suite, encode into int */
		m1._message_1_SUITES_I_choice = _message_1_SUITES_I_int;
		m1._message_1_SUITES_I_int = c->suites_i.ptr[0];
	} else if (c->suites_i.len > 1) {
		/* more than one suites, encode into array */
		m1._message_1_SUITES_I_choice = _SUITES_I__suite;
		m1._SUITES_I__suite_suite_count = c->suites_i.len;
		for (uint32_t i = 0; i < c->suites_i.len; i++) {
			m1._SUITES_I__suite_suite[i] = c->suites_i.ptr[i];
		}
	}

	/* G_X ephemeral public key */
	m1._message_1_G_X.value = c->g_x.ptr;
	m1._message_1_G_X.len = c->g_x.len;

	/* C_I connection id, encoded as  bstr_identifier */
	if (c->c_i.type == INT) {
		m1._message_1_C_I_choice = _message_1_C_I_int;
		m1._message_1_C_I_int = c->c_i.mem.c_x_int;
	} else {
		m1._message_1_C_I_choice = _message_1_C_I_bstr;
		m1._message_1_C_I_bstr.value = c->c_i.mem.c_x_bstr.ptr;
		m1._message_1_C_I_bstr.len = c->c_i.mem.c_x_bstr.len;
	}

	if (c->ead_1.len != 0) {
		/* ead_1 unprotected opaque auxiliary data */
		m1._message_1_ead_1.value = c->ead_1.ptr;
		m1._message_1_ead_1.len = c->ead_1.len;
		m1._message_1_ead_1_present = c->ead_1.len;
	} else {
		m1._message_1_ead_1_present = 0;
	}

	size_t payload_len_out;
	ok = cbor_encode_message_1(msg1, *msg1_len, &m1, &payload_len_out);

	if (!ok) {
		return cbor_encoding_error;
	}
	*msg1_len = payload_len_out;

	PRINT_ARRAY("message_1 (CBOR Sequence)", msg1, *msg1_len);
	return edhoc_no_error;
}

enum edhoc_error edhoc_initiator_run(const struct edhoc_initiator_context *c,
				     struct other_party_cred *cred_r_array,
				     uint16_t num_cred_r, uint8_t *err_msg,
				     uint32_t *err_msg_len, uint8_t *ead_2,
				     uint64_t *ead_2_len, uint8_t *ead_4,
				     uint64_t *ead_4_len, uint8_t *prk_4x3m,
				     uint8_t prk_4x3m_len, uint8_t *th4,
				     uint8_t th4_len)
{
	enum edhoc_error r;

	struct suite suite;
	bool auth_method_static_dh_i = false, static_dh_r = false;
	r = get_suite((enum suite_label)c->suites_i.ptr[0], &suite);
	if (r != edhoc_no_error) {
		return r;
	}

	authentication_type_get(c->method, &auth_method_static_dh_i,
				&static_dh_r);

	uint8_t msg1[MSG_1_DEFAULT_SIZE];
	uint32_t msg1_len = sizeof(msg1);
	uint8_t msg2[MSG_2_DEFAULT_SIZE];
	uint32_t msg2_len = sizeof(msg2);
	uint8_t msg4[MSG_4_DEFAULT_SIZE];
	uint32_t msg4_len = sizeof(msg2);

	//uint8_t c_i[c->c_i.len];
	//uint64_t c_i_len = sizeof(c_i);

	/*in a given selected cipher suite the length of G_X and G_Y is equal*/
	uint8_t g_y[c->g_x.len];
	uint64_t g_y_len = sizeof(g_y);

	uint8_t c_r_buf[C_R_DEFAULT_SIZE];
	struct c_x c_r;
	c_x_init(&c_r, c_r_buf, sizeof(c_r_buf));

	uint8_t ciphertext2[CIPHERTEXT2_DEFAULT_SIZE];
	uint64_t ciphertext2_len = sizeof(ciphertext2);

	/********************* encode and send message 1***********************/

	r = msg1_encode(c, (uint8_t *)&msg1, &msg1_len);
	if (r != edhoc_no_error) {
		return r;
	}

	r = tx(msg1, msg1_len);
	if (r != edhoc_no_error) {
		return r;
	}

	/*******************receive and process message 2**********************/

	r = rx(msg2, &msg2_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("message_2 (CBOR Sequence)", msg2, msg2_len);

	/* If an error message is received msg2_parse will return 
    error_message_received. If this hapens edhoc_initiator_run will return. Then
    the caller needs to examine SUITES_R in err_msg re-initialize the initiator 
    and call edhoc_initiator_run again*/
	r = msg2_parse(c, msg2, msg2_len, g_y, g_y_len, &c_r,
		       (uint8_t *)&ciphertext2, &ciphertext2_len);
	// if (r == error_message_received) {
	// 	/*provide the error message to the caller*/
	// 	r = _memcpy_s(err_msg, *err_msg_len, msg2, msg2_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	*err_msg_len = msg2_len;
	// 	return error_message_received;
	// }
	if (r != edhoc_no_error) {
		return r;
	}

	/*calculate the DH shared secret*/
	uint8_t g_xy[ECDH_SECRET_DEFAULT_SIZE];
	r = shared_secret_derive(suite.edhoc_ecdh_curve, c->x.ptr, c->x.len,
				 g_y, g_y_len, g_xy);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("G_XY (ECDH shared secret) ", g_xy, sizeof(g_xy));

	/*calculate th2*/
	uint8_t th2[SHA_DEFAULT_SIZE];
	r = th2_calculate(suite.edhoc_hash, msg1, msg1_len, g_y, sizeof(g_y),
			  &c_r, th2);
	if (r != edhoc_no_error) {
		return r;
	}

	/*calculate PRK_2e*/
	uint8_t PRK_2e[PRK_DEFAULT_SIZE];
	r = hkdf_extract(suite.edhoc_hash, NULL, 0, g_xy, sizeof(g_xy), PRK_2e);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("PRK_2e", PRK_2e, sizeof(PRK_2e));

	/*Derive KEYSTREAM_2*/
	//todo move this to a separate function
	uint64_t KEYSTREAM_2_len = ciphertext2_len;
	uint8_t KEYSTREAM_2[KEYSTREAM_2_len];
	r = okm_calc(suite.edhoc_hash, (uint8_t *)&PRK_2e, sizeof(PRK_2e),
		     (uint8_t *)&th2, sizeof(th2), "KEYSTREAM_2", NULL, 0,
		     KEYSTREAM_2, KEYSTREAM_2_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("KEYSTREAM_2", KEYSTREAM_2, sizeof(KEYSTREAM_2));

	uint8_t PLAINTEXT_2[ciphertext2_len];
	for (uint16_t i = 0; i < KEYSTREAM_2_len; i++) {
		PLAINTEXT_2[i] = ciphertext2[i] ^ KEYSTREAM_2[i];
	}
	PRINT_ARRAY("PLAINTEXT_2", PLAINTEXT_2, sizeof(PLAINTEXT_2));

	uint8_t sign_or_mac[SGN_OR_MAC_DEFAULT_SIZE];
	uint64_t sign_or_mac_len = sizeof(sign_or_mac);
	uint8_t id_cred_r[ID_CRED_DEFAULT_SIZE];
	uint64_t id_cred_r_len = sizeof(id_cred_r);

	r = plaintext_split(PLAINTEXT_2, sizeof(PLAINTEXT_2), id_cred_r,
			    &id_cred_r_len, sign_or_mac, &sign_or_mac_len,
			    ead_2, ead_2_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("ID_CRED_R", id_cred_r, id_cred_r_len);
	PRINT_ARRAY("sign_or_mac", sign_or_mac, sign_or_mac_len);
	if (*ead_2_len) {
		PRINT_ARRAY("ead_2", ead_2, *ead_2_len);
	}

	/*check the authenticity of the responder*/
	uint8_t cred_r[CRED_DEFAULT_SIZE];
	uint16_t cred_r_len = sizeof(cred_r);
	uint8_t pk[PK_DEFAULT_SIZE];
	uint16_t pk_len = sizeof(pk);
	uint8_t g_r[G_R_DEFAULT_SIZE];
	uint16_t g_r_len = sizeof(g_r);

	r = retrieve_cred(static_dh_r, cred_r_array, num_cred_r, id_cred_r,
			  id_cred_r_len, cred_r, &cred_r_len, pk, &pk_len, g_r,
			  &g_r_len);
	if (r != edhoc_no_error) {
		return r;
	}

	PRINT_ARRAY("CRED_R", cred_r, cred_r_len);
	PRINT_ARRAY("pk", pk, pk_len);
	PRINT_ARRAY("g_r", g_r, g_r_len);

	/*derive prk_3e2m*/
	uint8_t PRK_3e2m[PRK_DEFAULT_SIZE];
	r = prk_derive(static_dh_r, suite, PRK_2e, sizeof(PRK_2e), g_r, g_r_len,
		       c->x.ptr, c->x.len, PRK_3e2m);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("prk_3e2m", PRK_3e2m, sizeof(PRK_3e2m));

	// uint8_t m_2[A_2M_DEFAULT_SIZE];
	// uint16_t m_2_len = sizeof(m_2);
	// uint8_t mac_2[16];
	// uint8_t mac_2_len = sizeof(mac_2);
	// r = signature_or_mac_msg_create(static_dh_r, suite, "K_2m",
	// 				"IV_2m", (uint8_t *)&PRK_3e2m,
	// 				sizeof(PRK_3e2m), (uint8_t *)&th2,
	// 				sizeof(th2), id_cred_r, id_cred_r_len,
	// 				cred_r, cred_r_len, ead_2, *ead_2_len,
	// 				m_2, &m_2_len, mac_2, &mac_2_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }

	/*calculate MAC_2*/
	uint32_t mac_2_len;
	uint8_t mac_2[SHA_DEFAULT_SIZE];
	r = mac(PRK_3e2m, sizeof(PRK_3e2m), th2, sizeof(th2), id_cred_r,
		id_cred_r_len, cred_r, cred_r_len, ead_2, *ead_2_len, "MAC_2",
		static_dh_r, &suite, mac_2, &mac_2_len);
	if (r != edhoc_no_error) {
		return r;
	}

	// uint8_t *sign_or_mac_2 = mac_2;
	// uint32_t sign_or_mac_2_len = mac_2_len;

	// /*check the authenticity of message 2*/
	// uint8_t diag_msg[] = { "Responder authentication FIALED!\n" };
	if (static_dh_r) {
		if (!memcmp(mac_2, sign_or_mac, mac_2_len)) {
			PRINT_MSG("Responder authentication ok!\n");
		} else {
			// r = tx_err_msg(INITIATOR, c->corr, c_r, c_r_len,
			// 	       diag_msg, strlen((char *)diag_msg), NULL,
			// 	       0);
			// if (r != edhoc_no_error) {
			// 	return r;
			// }
			return responder_authentication_failed;
		}
	} else {
		/*the responder authenticates with a signature*/
		// bool verified = false;
		// r = verify(suite.edhoc_sign_curve, pk, pk_len, (uint8_t *)&m_2,
		// 	   m_2_len, (uint8_t *)&sign_or_mac, sign_or_mac_len,
		// 	   &verified);
		// if (verified) {
		// 	PRINT_MSG("Responder authentication okful!\n");
		// } else {
		// 	r = tx_err_msg(INITIATOR, c->corr, c_r, c_r_len,
		// 		       diag_msg, strlen((char *)diag_msg), NULL,
		// 		       0);
		// 	if (r != edhoc_no_error) {
		// 		return r;
		// 	}
		// 	return responder_authentication_failed;
		//}
	}

	/********msg3 create and send**************************************/

	// uint8_t *data_3;
	// uint8_t data_3_len;
	// if (c->corr == 2 || c->corr == 3) {
	// 	data_3 = NULL;
	// 	data_3_len = 0;
	// } else {
	// 	data_3 = c_r;
	// 	data_3_len = c_r_len;
	// }
	uint8_t th3[32];
	r = th3_calculate(suite.edhoc_hash, (uint8_t *)&th2, sizeof(th2),
			  ciphertext2, ciphertext2_len, th3);
	if (r != edhoc_no_error) {
		return r;
	}

	/*derive prk_4x3m*/
	r = prk_derive(static_dh_r, suite, (uint8_t *)&PRK_3e2m,
		       sizeof(PRK_3e2m), g_y, sizeof(g_y), c->i.ptr, c->i.len,
		       prk_4x3m);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("prk_4x3m", prk_4x3m, prk_4x3m_len);

	/*calculate MAC_3*/
	uint32_t mac_3_len;
	uint8_t mac_3[SHA_DEFAULT_SIZE];
	r = mac(prk_4x3m, prk_4x3m_len, th3, sizeof(th3), c->id_cred_i.ptr,
		c->id_cred_i.len, c->cred_i.ptr, c->cred_i.len, c->ead_3.ptr,
		c->ead_3.len, "MAC_3", static_dh_r, &suite, mac_3, &mac_3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	uint8_t *sign_or_mac_3 = mac_3;
	uint32_t sign_or_mac_3_len = mac_3_len;

	// uint8_t m_3[M_3_DEFAULT_SIZE];
	// uint16_t m_3_len = sizeof(m_3);
	// uint8_t sign_or_mac_3[64];
	// uint32_t sign_or_mac_3_len = sizeof(sign_or_mac_3);
	// r = signature_or_mac_msg_create(
	// 	auth_method_static_dh_i, suite, "K_3m", "IV_3m", prk_4x3m,
	// 	prk_4x3m_len, (uint8_t *)&th3, sizeof(th3), c->id_cred_i.ptr,
	// 	c->id_cred_i.len, c->cred_i.ptr, c->cred_i.len, c->ead_3.ptr,
	// 	c->ead_3.len, m_3, &m_3_len, sign_or_mac_3,
	// 	(uint8_t *)&sign_or_mac_3_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }

	// /*Calculate signature if the initiator authenticates with signature*/
	// if (!auth_method_static_dh_i) {
	// 	/*Calculate a signature*/
	// 	sign_or_mac_3_len = sizeof(sign_or_mac_3);
	// 	r = sign(suite.edhoc_sign_curve, c->sk_i.ptr, c->sk_i.len,
	// 		 c->pk_i.ptr, c->pk_i.len, m_3, m_3_len, sign_or_mac_3,
	// 		 &sign_or_mac_3_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	PRINT_ARRAY("Signature_or_MAC_3", sign_or_mac_3,
	// 		    sign_or_mac_3_len);
	// }

	/*Calculate P_3ae*/
	// uint8_t sign_or_mac_3_enc[sign_or_mac_3_len + 2];
	// uint16_t sign_or_mac_3_enc_len = sizeof(sign_or_mac_3_enc);
	// r = encode_byte_string(sign_or_mac_3, sign_or_mac_3_len,
	// 		       sign_or_mac_3_enc, &sign_or_mac_3_enc_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }

	// uint8_t kid_buf[KID_DEFAULT_SIZE];
	// uint32_t kid_len = sizeof(kid_buf);
	// r = id_cred2kid(c->id_cred_i.ptr, c->id_cred_i.len, kid_buf, &kid_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }
	// PRINT_ARRAY("kid", kid_buf, kid_len);

	// uint8_t P_3ae[PRK_3AE_DEFAULT_SIZE];
	// uint32_t P_3ae_len = sizeof(P_3ae);

	// if (kid_len != 0) {
	// 	r = _memcpy_s(P_3ae, P_3ae_len, kid_buf, kid_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	r = _memcpy_s(P_3ae + kid_len, P_3ae_len - kid_len,
	// 		      sign_or_mac_3_enc, sign_or_mac_3_enc_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	P_3ae_len = sign_or_mac_3_enc_len + kid_len;
	// } else {
	// 	r = _memcpy_s(P_3ae, P_3ae_len, c->id_cred_i.ptr,
	// 		      c->id_cred_i.len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	r = _memcpy_s(P_3ae + c->id_cred_i.len,
	// 		      P_3ae_len - c->id_cred_i.len, sign_or_mac_3_enc,
	// 		      sign_or_mac_3_enc_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	P_3ae_len = c->id_cred_i.len + sign_or_mac_3_enc_len;
	// }
	// if (c->ead_3.len > 0) {
	// 	r = _memcpy_s(P_3ae + P_3ae_len, sizeof(P_3ae), c->ead_3.ptr,
	// 		      c->ead_3.len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	P_3ae_len += c->ead_3.len;
	// }

	// PRINT_ARRAY("P_3ae", P_3ae, P_3ae_len);

	// /*Calculate K_3*/
	// uint8_t K_3[16];
	// r = okm_calc(suite.edhoc_hash, PRK_3e2m, sizeof(PRK_3e2m),
	// 	     (uint8_t *)&th3, sizeof(th3), "K_3", NULL, 0, K_3,
	// 	     sizeof(K_3));
	// if (r != edhoc_no_error) {
	// 	return r;
	// }
	// PRINT_ARRAY("K_3", K_3, sizeof(K_3));

	// /*Calculate IV_3*/
	// uint8_t IV_3[13];
	// r = okm_calc(suite.edhoc_hash, PRK_3e2m, sizeof(PRK_3e2m),
	// 	     (uint8_t *)&th3, sizeof(th3), "IV_3", NULL, 0, IV_3,
	// 	     sizeof(IV_3));
	// if (r != edhoc_no_error) {
	// 	return r;
	// }
	// PRINT_ARRAY("IV_3", IV_3, sizeof(IV_3));

	// /*Associated data A_3ae*/
	// uint8_t A_3ae[A_3AE_DEFAULT_SIZE];
	// uint16_t A_3ae_len = sizeof(A_3ae);
	// r = associated_data_encode(th3, sizeof(th3), (uint8_t *)&A_3ae, &A_3ae_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }
	// PRINT_ARRAY("A_3ae", A_3ae, A_3ae_len);

	// /*Ciphertext 3 calculate*/
	// uint8_t mac_len = 8;
	// if (suite.edhoc_aead == AES_CCM_16_128_128) {
	// 	mac_len = 16;
	// }
	// uint8_t tag[mac_len];
	// uint32_t ciphertext_3_len = P_3ae_len;
	// uint8_t ciphertext_3[ciphertext_3_len + mac_len];

	// r = aead(ENCRYPT, P_3ae, P_3ae_len, K_3, sizeof(K_3), IV_3,
	// 	 sizeof(IV_3), A_3ae, A_3ae_len, ciphertext_3, ciphertext_3_len,
	// 	 tag, mac_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }

	// PRINT_ARRAY("ciphertext_3", ciphertext_3, sizeof(ciphertext_3));

	uint8_t ciphertext_3[CIPHERTEXT3_DEFAULT_SIZE];
	uint32_t ciphertext_3_len = sizeof(ciphertext_3);
	r = ciphertext_gen(CIPHERTEXT3, suite.edhoc_hash, c->id_cred_i.ptr,
			   c->id_cred_i.len, sign_or_mac_3, sign_or_mac_3_len,
			   c->ead_3.ptr, c->ead_3.len, PRK_3e2m,
			   sizeof(PRK_3e2m), th3, sizeof(th3), ciphertext_3,
			   &ciphertext_3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	/*massage 3 create and send*/
	uint8_t msg3[ciphertext_3_len + 2];
	uint64_t msg3_len = sizeof(msg3);

	r = encode_byte_string(ciphertext_3, ciphertext_3_len, msg3, &msg3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	PRINT_ARRAY("msg3", msg3, msg3_len);

	r = tx(msg3, msg3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	/*TH4*/
	r = th4_calculate(suite.edhoc_hash, th3, sizeof(th3), ciphertext_3,
			  ciphertext_3_len, th4);
	if (r != edhoc_no_error) {
		return r;
	}

	/*******************receive and process message 4**********************/
	if (c->msg4) {
		r = rx(msg4, &msg4_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("message_4 (CBOR Sequence)", msg4, msg4_len);
	}

	uint8_t ciphertext_4[CIPHERTEXT4_DEFAULT_SIZE];
	uint32_t ciphertext_4_len = sizeof(ciphertext_4);
	r = decode_byte_string(msg4, msg4_len, ciphertext_4, &ciphertext_4_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("ciphertext_4", ciphertext_4, ciphertext_4_len);

	r = ciphertext_decrypt_split(CIPHERTEXT4, &suite, prk_4x3m,
				     prk_4x3m_len, th4, th4_len, ciphertext_4,
				     ciphertext_4_len, NULL, 0, NULL, 0, ead_4,
				     ead_4_len);
	if (r != edhoc_no_error) {
		return r;
	}
	return edhoc_no_error;
}
