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
#include "../inc/ciphertext.h"
#include "../cbor/decode_message_1.h"
#include "../cbor/encode_message_2.h"
#include "../cbor/decode_bstr_type.h"
#include "../cbor/decode_message_3.h"

/**
 * @brief   Parses message 1
 * @param   msg1 buffer containing message 1
 * @param   msg1_len length of msg1
 * @param   method method
 * @param   suites_i
 * @param   suites_i_len length of suites_i
 * @param   g_x Public ephemeral key of the initiator
 * @param   g_x_len length of g_x
 * @param   c_i connection identifier of the initiator
 * @param   c_i_len length of c_i
 * @param   ad1 axillary data 1
 * @param   ad1_len length of ad1
 * @retval an edhoc_error code
 */
static inline enum edhoc_error
msg1_parse(uint8_t *msg1, uint32_t msg1_len, enum method_type *method,
	   uint8_t *suites_i, uint64_t *suites_i_len, uint8_t *g_x,
	   uint64_t *g_x_len, struct c_x *c_i, uint8_t *ad1, uint64_t *ad1_len)
{
	uint32_t i;
	bool ok;
	struct message_1 m;
	size_t decode_len = 0;
	enum edhoc_error r;

	ok = cbor_decode_message_1(msg1, msg1_len, &m, &decode_len);
	if (!ok) {
		return cbor_decoding_error;
	}

	/*METHOD*/
	*method = m._message_1_METHOD;
	PRINTF("msg1 METHOD: %d\n", (int)*method);

	/*SUITES_I*/
	if (m._message_1_SUITES_I_choice == _message_1_SUITES_I_int) {
		/*the initiator supports only one suite*/
		suites_i[0] = m._message_1_SUITES_I_int;
		*suites_i_len = 1;
	} else {
		/*the initiator supports more than one suite*/
		if (m._SUITES_I__suite_suite_count > *suites_i_len) {
			return suites_i_list_to_long;
		}

		for (i = 0; i < m._SUITES_I__suite_suite_count; i++) {
			suites_i[i] = m._SUITES_I__suite_suite[i];
		}
		*suites_i_len = m._SUITES_I__suite_suite_count;
	}
	PRINT_ARRAY("msg1 SUITES_I", suites_i, *suites_i_len);

	/*G_X*/
	_memcpy_s(g_x, *g_x_len, m._message_1_G_X.value, m._message_1_G_X.len);
	*g_x_len = m._message_1_G_X.len;
	PRINT_ARRAY("msg1 G_X", g_x, *g_x_len);

	/*C_I*/
	if (m._message_1_C_I_choice == _message_1_C_I_int) {
		r = c_x_set(INT, NULL, 0, m._message_1_C_I_int, c_i);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINTF("msg1 C_I_raw (int): %d\n", c_i->mem.c_x_int);
	} else {
		r = c_x_set(BSTR, m._message_1_C_I_bstr.value, 0,
			    m._message_1_C_I_int, c_i);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("msg1 C_I_raw (bstr)", c_i->mem.c_x_bstr.ptr,
			    c_i->mem.c_x_bstr.len);
	}

	/*ead_1*/
	if (m._message_1_ead_1_present) {
		_memcpy_s(ad1, *ad1_len, m._message_1_ead_1.value,
			  m._message_1_ead_1.len);
		*ad1_len = m._message_1_ead_1.len;
		PRINT_ARRAY("msg1 ead_1", ad1, *ad1_len);
	}
	return edhoc_no_error;
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
// static inline enum edhoc_error
// msg3_parse(uint8_t corr, uint8_t *msg3, uint16_t msg3_len, uint8_t *c_r,
// 	   uint64_t *c_r_len, uint8_t *ciphertext_3, uint64_t *ciphertext_3_len)
// {
// 	enum edhoc_error r;
// 	bool ok;
// 	struct m3 m;
// 	size_t decode_len = 0;

// 	ok = cbor_decode_m3(msg3, msg3_len, &m, &decode_len);
// 	if (!ok) {
// 		return cbor_decoding_error;
// 	}

// 	if (corr != 2 && corr != 3) {
// 		/*C_R is present*/
// 		if (*c_r_len == 1) {
// 			/*C_R is encoded as int*/
// 			c_r[0] = m._m3_C_R._m3_C_R_int;
// 		} else {
// 			r = _memcpy_s(c_r, *c_r_len,
// 				      m._m3_C_R._m3_C_R_bstr.value,
// 				      m._m3_C_R._m3_C_R_bstr.len);
// 			if (r != edhoc_no_error) {
// 				return r;
// 			}

// 			*c_r_len = m._m3_C_R._m3_C_R_bstr.len;
// 		}
// 		PRINT_ARRAY("msg3 C_R", c_r, *c_r_len);
// 	}

// 	r = _memcpy_s(ciphertext_3, *ciphertext_3_len, m._m3_CIPHERTEXT_3.value,
// 		      m._m3_CIPHERTEXT_3.len);
// 	if (r != edhoc_no_error) {
// 		return r;
// 	}

// 	*ciphertext_3_len = m._m3_CIPHERTEXT_3.len;

// 	PRINT_ARRAY("msg3 CIPHERTEXT_3", ciphertext_3, *ciphertext_3_len);
// 	return edhoc_no_error;
// };

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
 * @retval  an edhoc_error error code
 */
static inline enum edhoc_error msg2_encode(const uint8_t *g_y, uint8_t g_y_len,
					   struct c_x *c_r,
					   const uint8_t *ciphertext_2,
					   uint32_t ciphertext_2_len,
					   uint8_t *msg2, uint32_t *msg2_len)
{
	bool ok;
	size_t payload_len_out;
	struct m2 m;

	uint8_t G_Y_CIPHERTEXT_2[g_y_len + ciphertext_2_len];

	memcpy(G_Y_CIPHERTEXT_2, g_y, g_y_len);
	memcpy(G_Y_CIPHERTEXT_2 + g_y_len, ciphertext_2, ciphertext_2_len);

	/*Encode G_Y_CIPHERTEXT_2*/
	m._m2_G_Y_CIPHERTEXT_2.value = G_Y_CIPHERTEXT_2;
	m._m2_G_Y_CIPHERTEXT_2.len = sizeof(G_Y_CIPHERTEXT_2);

	/*Encode C_R*/
	if (c_r->type == INT) {
		m._m2_C_R_choice = _m2_C_R_int;
		m._m2_C_R_int = c_r->mem.c_x_int;
	} else {
		m._m2_C_R_choice = _m2_C_R_bstr;
		m._m2_C_R_bstr.value = c_r->mem.c_x_bstr.ptr;
		m._m2_C_R_bstr.len = c_r->mem.c_x_bstr.len;
	}

	ok = cbor_encode_m2(msg2, *msg2_len, &m, &payload_len_out);
	if (!ok) {
		return cbor_encoding_error;
	}
	*msg2_len = payload_len_out;

	PRINT_ARRAY("message_2 (CBOR Sequence)", msg2, *msg2_len);
	return edhoc_no_error;
}

enum edhoc_error edhoc_responder_run(struct edhoc_responder_context *c,
				     struct other_party_cred *cred_i_array,
				     uint16_t num_cred_i, uint8_t *err_msg,
				     uint32_t *err_msg_len, uint8_t *ead_1,
				     uint64_t *ead_1_len, uint8_t *ead_3,
				     uint64_t *ead_3_len, uint8_t *prk_4x3m,
				     uint16_t prk_4x3m_len, uint8_t *th4,
				     uint16_t th4_len)
{
	enum edhoc_error r;
	/**************** receive and process message 1 ***********************/
	uint8_t msg1[MSG_1_DEFAULT_SIZE];
	uint32_t msg1_len = sizeof(msg1);

	r = rx(msg1, &msg1_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("message_1 (CBOR Sequence)", msg1, msg1_len);

	enum method_type method;
	uint8_t suites_i[5];
	uint64_t suites_i_len = sizeof(suites_i);
	uint8_t g_x[G_X_DEFAULT_SIZE];
	uint64_t g_x_len = sizeof(g_x);
	uint8_t c_i_buf[C_I_DEFAULT_SIZE];
	struct c_x c_i;
	c_x_init(&c_i, c_i_buf, sizeof(c_i_buf));

	r = msg1_parse(msg1, msg1_len, &method, suites_i, &suites_i_len, g_x,
		       &g_x_len, &c_i, ead_1, ead_1_len);
	if (r != edhoc_no_error) {
		return r;
	}

	// if (!(selected_suite_is_supported(suites_i[0], &c->suites_r))) {
	// 	r = tx_err_msg(RESPONDER, method, c_i, c_i_len, NULL, 0,
	// 		       c->suites_r.ptr, c->suites_r.len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	/*After an error message is sent the protocol must be discontinued*/
	// 	return error_message_sent;
	// }

	/*get the method*/
	//enum method_type method = method_corr >> 2;
	/*get corr*/
	//uint8_t corr = method_corr - 4 * method;
	/*get cipher suite*/
	struct suite suite;
	r = get_suite((enum suite_label)suites_i[0], &suite);
	if (r != edhoc_no_error) {
		return r;
	}

	bool static_dh_i, static_dh_r;
	authentication_type_get(method, &static_dh_i, &static_dh_r);

	/******************* create and send message 2*************************/

	uint8_t th2[SHA_DEFAULT_SIZE];
	//uint64_t tmp_c_i_len;
	// if ((corr == 1) || (corr = 3)) {
	// 	tmp_c_i_len = 0;
	// } else {
	// 	tmp_c_i_len = c_i_len;
	// }

	r = th2_calculate(suite.edhoc_hash, msg1, msg1_len, c->g_y.ptr,
			  c->g_y.len, &c->c_r, th2);
	if (r != edhoc_no_error) {
		return r;
	}

	/*calculate the DH shared secret*/
	uint8_t g_xy[ECDH_SECRET_DEFAULT_SIZE];
	r = shared_secret_derive(suite.edhoc_ecdh_curve, c->y.ptr, c->y.len,
				 g_x, g_x_len, g_xy);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("G_XY (ECDH shared secret) ", g_xy, sizeof(g_xy));

	uint8_t PRK_3e2m[PRK_DEFAULT_SIZE];

	uint8_t PRK_2e[PRK_DEFAULT_SIZE];
	r = hkdf_extract(suite.edhoc_hash, NULL, 0, g_xy, sizeof(g_xy), PRK_2e);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("PRK_2e", PRK_2e, sizeof(PRK_2e));

	/*derive prk_3e2m*/
	r = prk_derive(static_dh_r, suite, PRK_2e, sizeof(PRK_2e), g_x, g_x_len,
		       c->r.ptr, c->r.len, PRK_3e2m);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("prk_3e2m", PRK_3e2m, sizeof(PRK_3e2m));

	/*calculate MAC_2*/
	uint32_t mac_2_len;
	uint8_t mac_2[SHA_DEFAULT_SIZE];
	r = mac(PRK_3e2m, sizeof(PRK_3e2m), th2, sizeof(th2), c->id_cred_r.ptr,
		c->id_cred_r.len, c->cred_r.ptr, c->cred_r.len, c->ead_2.ptr,
		c->ead_2.len, "MAC_2", static_dh_r, &suite, mac_2, &mac_2_len);
	if (r != edhoc_no_error) {
		return r;
	}

	uint8_t *sign_or_mac_2 = mac_2;
	uint32_t sign_or_mac_2_len = mac_2_len;

	// uint8_t m_2[A_2M_DEFAULT_SIZE];
	// uint16_t m_2_len = sizeof(m_2);
	// uint8_t sign_or_mac_2[64];
	// uint32_t sign_or_mac_2_len = sizeof(sign_or_mac_2);

	//PRINT_ARRAY("CRED_R", c->cred_r.ptr, c->cred_r.len);

	// r = signature_or_mac_msg_create(
	// 	static_dh_r, suite, "K_2m", "IV_2m", (uint8_t *)&PRK_3e2m,
	// 	sizeof(PRK_3e2m), (uint8_t *)&th2, sizeof(th2),
	// 	c->id_cred_r.ptr, c->id_cred_r.len, c->cred_r.ptr,
	// 	c->cred_r.len, c->ead_2.ptr, c->ead_2.len, m_2, &m_2_len,
	// 	sign_or_mac_2, (uint8_t *)&sign_or_mac_2_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }

	// /*Signature_or_mac_2*/
	// if (!static_dh_r) {
	// 	/*Calculate a signature*/
	// 	sign_or_mac_2_len = 64;
	// 	r = sign(suite.edhoc_sign_curve, c->sk_r.ptr, c->sk_r.len,
	// 		 c->pk_r.ptr, c->pk_r.len, m_2, m_2_len, sign_or_mac_2,
	// 		 &sign_or_mac_2_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	PRINT_ARRAY("Signature_or_MAC_2", sign_or_mac_2,
	// 		    sign_or_mac_2_len);
	// }

	/*Calculate PLAINTEXT_2*/
	uint16_t PLAINTEXT_2_len =
		c->id_cred_r.len + sign_or_mac_2_len + 2 + c->ead_2.len;
	uint8_t PLAINTEXT_2[PLAINTEXT_2_len];

	PRINT_ARRAY("ID_CRED_R", c->id_cred_r.ptr, c->id_cred_r.len);
	r = plaintext_encode(c->id_cred_r.ptr, c->id_cred_r.len, sign_or_mac_2,
			     sign_or_mac_2_len, c->ead_2.ptr, c->ead_2.len,
			     PLAINTEXT_2, &PLAINTEXT_2_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("PLAINTEXT_2", PLAINTEXT_2, PLAINTEXT_2_len);

	/*Calculate KEYSTREAM_2*/
	uint8_t KEYSTREAM_2[PLAINTEXT_2_len];
	r = okm_calc(suite.edhoc_hash, (uint8_t *)&PRK_2e, sizeof(PRK_2e),
		     (uint8_t *)&th2, sizeof(th2), "KEYSTREAM_2", NULL, 0,
		     KEYSTREAM_2, sizeof(KEYSTREAM_2));
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("KEYSTREAM_2", KEYSTREAM_2, sizeof(KEYSTREAM_2));

	/*Ciphertext 2 calculate*/
	uint8_t ciphertext_2[PLAINTEXT_2_len];
	uint32_t ciphertext_2_len = sizeof(ciphertext_2);

	for (uint16_t i = 0; i < ciphertext_2_len; i++) {
		ciphertext_2[i] = PLAINTEXT_2[i] ^ KEYSTREAM_2[i];
	}
	PRINT_ARRAY("ciphertext_2", ciphertext_2, ciphertext_2_len);

	/*message 2 create and send*/
	uint8_t msg2[MSG_2_DEFAULT_SIZE];
	uint32_t msg2_len = sizeof(msg2);
	r = msg2_encode(c->g_y.ptr, c->g_y.len, &c->c_r, ciphertext_2,
			ciphertext_2_len, msg2, &msg2_len);
	if (r != edhoc_no_error) {
		return r;
	}
	r = tx(msg2, msg2_len);
	if (r != edhoc_no_error) {
		return r;
	}

	// /********message 3 receive and process******************************/
	uint8_t msg3[MSG_3_DEFAULT_SIZE];
	uint32_t msg3_len = sizeof(msg3);
	r = rx(msg3, &msg3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	//uint8_t c_r[c->c_r.len];
	//uint64_t c_r_len = sizeof(c_r);
	uint8_t ciphertext_3[CIPHERTEXT3_DEFAULT_SIZE];
	uint32_t ciphertext_3_len = sizeof(ciphertext_3);

	//r = msg3_parse(corr, msg3, msg3_len, c_r, &c_r_len, ciphertext_3,
	//	       &ciphertext_3_len);
	// if (r == error_message_received) {
	// 	/*provide the error message to the caller*/
	// 	r = _memcpy_s(err_msg, *err_msg_len, msg3, msg3_len);
	// 	if (r != edhoc_no_error) {
	// 		return r;
	// 	}
	// 	*err_msg_len = msg3_len;
	// 	return error_message_received;
	// }
	r = decode_byte_string(msg3, msg3_len, ciphertext_3, &ciphertext_3_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("CIPHERTEXT_3", ciphertext_3, ciphertext_3_len);

	uint8_t th3[32];
	r = th3_calculate(suite.edhoc_hash, (uint8_t *)&th2, sizeof(th2),
			  ciphertext_2, ciphertext_2_len, th3);
	if (r != edhoc_no_error) {
		return r;
	}

	uint8_t K_3[AEAD_KEY_DEFAULT_SIZE];
	uint8_t IV_3[AEAD_IV_DEFAULT_SIZE];

	/*Calculate K_3*/
	r = okm_calc(suite.edhoc_hash, PRK_3e2m, sizeof(PRK_3e2m),
		     (uint8_t *)&th3, sizeof(th3), "K_3", NULL, 0, K_3,
		     sizeof(K_3));
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("K_3", K_3, sizeof(K_3));

	/*Calculate IV_3*/
	r = okm_calc(suite.edhoc_hash, PRK_3e2m, sizeof(PRK_3e2m),
		     (uint8_t *)&th3, sizeof(th3), "IV_3", NULL, 0, IV_3,
		     sizeof(IV_3));
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("IV_3", IV_3, sizeof(IV_3));

	/*Associated data A_3ae*/
	uint8_t A_3ae[ASSOCIATED_DATA_DEFAULT_SIZE];
	uint16_t A_3ae_len = sizeof(A_3ae);
	r = associated_data_encode(th3, sizeof(th3), (uint8_t *)&A_3ae,
				   &A_3ae_len);
	if (r != edhoc_no_error) {
		return r;
	}

	uint8_t tag[16];
	uint8_t mac_len = 8;
	//todo move that to a separate function
	if (suite.edhoc_aead == AES_CCM_16_128_128) {
		mac_len = 16;
	}
	uint8_t P_3ae[ciphertext_3_len - mac_len];
	//memcpy(tag, &ciphertext_3[ciphertext_3_len - mac_len], mac_len);
	r = _memcpy_s(tag, sizeof(tag),
		      &ciphertext_3[ciphertext_3_len - mac_len], mac_len);
	if (r != edhoc_no_error) {
		return r;
	}
	r = aead(DECRYPT, ciphertext_3, ciphertext_3_len, K_3, sizeof(K_3),
		 IV_3, sizeof(IV_3), A_3ae, A_3ae_len, P_3ae, sizeof(P_3ae),
		 tag, mac_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("P_3ae", P_3ae, sizeof(P_3ae));

	uint8_t id_cred_i[ID_CRED_DEFAULT_SIZE];
	uint32_t id_cred_i_len = sizeof(id_cred_i);
	uint8_t sign_or_mac[SGN_OR_MAC_DEFAULT_SIZE];
	uint32_t sign_or_mac_len = sizeof(sign_or_mac);
	r = plaintext_split(P_3ae, sizeof(P_3ae), id_cred_i, &id_cred_i_len,
			    sign_or_mac, &sign_or_mac_len, ead_3, ead_3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	PRINT_ARRAY("ID_CRED_I", id_cred_i, id_cred_i_len);
	PRINT_ARRAY("sign_or_mac", sign_or_mac, sign_or_mac_len);
	if (*ead_3_len) {
		PRINT_ARRAY("ead_3", ead_3, *ead_3_len);
	}

	/*check the authenticity of the initiator*/
	uint8_t cred_i[CRED_DEFAULT_SIZE];
	uint16_t cred_i_len = sizeof(cred_i);
	uint8_t pk[PK_DEFAULT_SIZE];
	uint16_t pk_len = sizeof(pk);
	uint8_t g_i[G_I_DEFAULT_SIZE];
	uint16_t g_i_len = sizeof(g_i);

	r = retrieve_cred(static_dh_i, cred_i_array, num_cred_i, id_cred_i,
			  id_cred_i_len, cred_i, &cred_i_len, pk, &pk_len, g_i,
			  &g_i_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("CRED_I", cred_i, cred_i_len);
	PRINT_ARRAY("pk", pk, pk_len);
	PRINT_ARRAY("g_i", g_i, g_i_len);

	/*derive prk_4x3m*/
	r = prk_derive(static_dh_i, suite, (uint8_t *)&PRK_3e2m,
		       sizeof(PRK_3e2m), g_i, g_i_len, c->y.ptr, c->y.len,
		       prk_4x3m);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("prk_4x3m", prk_4x3m, prk_4x3m_len);

	//uint8_t m_3[A_2M_DEFAULT_SIZE];
	//uint16_t m_3_len = sizeof(m_3);
	uint8_t mac_3[16];
	uint32_t mac_3_len = sizeof(mac_3);
	// r = signature_or_mac_msg_create(static_dh_i, suite, "K_3m", "IV_3m",
	// 				prk_4x3m, prk_4x3m_len, (uint8_t *)&th3,
	// 				sizeof(th3), id_cred_i, id_cred_i_len,
	// 				cred_i, cred_i_len, ead_3, *ead_3_len,
	// 				m_3, &m_3_len, mac_3, &mac_3_len);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }

	r = mac(prk_4x3m, prk_4x3m_len, th3, sizeof(th3), id_cred_i,
		id_cred_i_len, cred_i, cred_i_len, ead_3, *ead_3_len, "MAC_3",
		static_dh_r, &suite, mac_3, &mac_3_len);
	if (r != edhoc_no_error) {
		return r;
	}

	if (static_dh_i) {
		/*check inner mac MAC_3*/
		if (0 != memcmp(mac_3, sign_or_mac, mac_3_len)) {
			PRINT_MSG("Initiator authentication failed!");
			// r = tx_err_msg(RESPONDER, corr, c_i, c_i_len, NULL, 0,
			// 	       NULL, 0);
			// if (r != edhoc_no_error) {
			// 	return r;
			// }
			return responder_authentication_failed;
		} else {
			PRINT_MSG("Initiator authentication ok!\n");
		}
	} else {
		// /*the initiator authenticates with a signature*/
		// bool verified = false;
		// r = verify(suite.edhoc_sign_curve, pk, pk_len, (uint8_t *)&m_3,
		// 	   m_3_len, (uint8_t *)&sign_or_mac, sign_or_mac_len,
		// 	   &verified);
		// if (verified) {
		// 	PRINT_MSG("Initiator authentication okful!\n");
		// } else {
		// 	PRINT_MSG("Initiator authentication failed!\n");
		// 	r = tx_err_msg(RESPONDER, corr, c_i, c_i_len, NULL, 0,
		// 		       NULL, 0);
		// 	if (r != edhoc_no_error) {
		// 		return r;
		// 	}
		// 	return responder_authentication_failed;
		// }
	}

	/*TH4*/
	r = th4_calculate(suite.edhoc_hash, th3, sizeof(th3), ciphertext_3,
			  ciphertext_3_len, th4);
	if (r != edhoc_no_error) {
		return r;
	}

	/******************************create and send msg4********************/
	if (c->msg4) {
		/*Ciphertext 4 calculate*/
		uint8_t ciphertext_4[PLAINTEXT_2_len];
		uint32_t ciphertext_4_len = sizeof(ciphertext_4);
		uint8_t msg4[MSG_4_DEFAULT_SIZE];
		uint64_t msg4_len = sizeof(msg2);
		r = ciphertext_gen(CIPHERTEXT4, suite.edhoc_hash, NULL, 0, NULL,
				   0, c->ead_4.ptr, c->ead_4.len, prk_4x3m,
				   prk_4x3m_len, th4, th4_len, ciphertext_4,
				   &ciphertext_4_len);
		if (r != edhoc_no_error) {
			return r;
		}

		r = encode_byte_string(ciphertext_4, ciphertext_4_len, msg4,
				       &msg4_len);
		if (r != edhoc_no_error) {
			return r;
		}

		PRINT_ARRAY("Message 4 ", msg4, msg4_len);

		r = tx(msg4, msg4_len);
		if (r != edhoc_no_error) {
			return r;
		}
	}

	return edhoc_no_error;
}
