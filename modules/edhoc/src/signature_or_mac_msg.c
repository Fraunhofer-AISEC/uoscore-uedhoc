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
#include <stdint.h>

#include "../edhoc.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/hkdf_info.h"
#include "../inc/okm.h"
#include "../inc/print_util.h"
#include "../inc/suites.h"
#include "../inc/cose.h"
#include "../cbor/encode_enc_structure.h"
#include "../cbor/encode_sig_structure.h"
//#include "../cbor/encode_byte_string.h"

EdhocError encode_byte_string(const uint8_t *in, const uint8_t in_len,
			      uint8_t *out, uint16_t *out_len)
{
	// bool success;
	// size_t payload_len_out;
	// success = cbor_encode_b_str(out, *out_len, in, &payload_len_out);
	// if (!success) {
	// 	return cbor_encoding_error;
	// }
	// *out_len = payload_len_out;

	// todo fix this when bug in cddl-gen is fixed

	int r;
	CborEncoder enc;
	cbor_encoder_init(&enc, out, *out_len, 0);
	r = cbor_encode_byte_string(&enc, in, in_len);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;
	*out_len = cbor_encoder_get_buffer_size(&enc, out);

	return EdhocNoError;
}

/**
 * @brief   Encodes data to be signed or macked
 */
static EdhocError m_encode(enum cose_context cose_context,
			   const uint8_t *id_cred, const uint8_t id_cred_len,
			   const uint8_t *th2, const uint8_t th_len,
			   const uint8_t *cred, const uint8_t cred_len,
			   const uint8_t *ad, const uint8_t ad_len,
			   const uint8_t *mac, const uint8_t mac_len,
			   uint8_t *out, uint16_t *out_len)
{
	EdhocError r;

	uint8_t th_enc[th_len + 2];
	uint16_t th_enc_len = sizeof(th_enc);
	r = encode_byte_string(th2, th_len, th_enc, &th_enc_len);
	if (r != EdhocNoError) {
		return r;
	}

	uint8_t tmp[th_enc_len + cred_len + ad_len];
	memcpy(tmp, th_enc, th_enc_len);
	memcpy(tmp + th_enc_len, cred, cred_len);
	if (ad_len != 0) {
		memcpy(tmp + th_enc_len + cred_len, ad, ad_len);
	}

	if (cose_context == Encrypt0) {
		uint8_t context_str[] = { "Encrypt0" };
		r = cose_enc_structure_encode(context_str, strlen(context_str),
					      id_cred, id_cred_len, tmp,
					      sizeof(tmp), out, out_len);
		if (r != EdhocNoError) {
			return r;
		}
	}

	if (cose_context == Signature1) {
		uint8_t context_str[] = { "Signature1" };
		r = cose_sig_structure_encode(context_str, strlen(context_str),
					      id_cred, id_cred_len, tmp,
					      sizeof(tmp), mac, mac_len, out,
					      out_len);
		if (r != EdhocNoError) {
			return r;
		}
	}

	return EdhocNoError;
}

EdhocError signature_or_mac_msg_create(
	bool static_dh_auth, struct suite suite, const char *label_k,
	const char *label_iv, const uint8_t *prk, const uint8_t prk_len,
	const uint8_t *th, const uint8_t th_len, const uint8_t *id_cred,
	const uint32_t id_cred_len, const uint8_t *cred,
	const uint32_t cred_len, const uint8_t *ad, const uint32_t ad_len,
	uint8_t *m, uint16_t *m_len, uint8_t *mac, uint8_t *mac_len)
{
	EdhocError r;

	/*calculate K_2m K_3m*/
	uint8_t K_m[AEAD_KEY_DEFAULT_SIZE];
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, label_k, prk, prk_len,
		     th, th_len, K_m, sizeof(K_m));
	if (r != EdhocNoError) {
		return r;
	}
	PRINT_ARRAY("inner MAC key (K_2m or K_3m)", K_m, sizeof(K_m));

	/*calculate IV_2m*/
	uint8_t IV_m[AEAD_IV_DEFAULT_SIZE];
	r = okm_calc(suite.edhoc_aead, suite.edhoc_hash, label_iv, prk, prk_len,
		     th, th_len, IV_m, sizeof(IV_m));
	if (r != EdhocNoError) {
		return r;
	}
	PRINT_ARRAY("inner MAC IV (IV_2m or IV_3m)", IV_m, sizeof(IV_m));

	/*A_2m or A_3m encode (additional data for msg2 msg3)*/
	uint8_t A_m[A_2M_DEFAULT_SIZE];
	uint16_t A_m_len = sizeof(A_m);

	r = m_encode(Encrypt0, id_cred, id_cred_len, th, th_len, cred, cred_len,
		     ad, ad_len, NULL, 0, A_m, &A_m_len);

	if (r != EdhocNoError) {
		return r;
	}
	PRINT_ARRAY("A_2m or A_3m", A_m, A_m_len);

	/* === Calculate MAC_2 ===
	 * We calculate a MAC through a AEAD algorithm.
	 * The AEAD algorithm AES_CCM_16_64_128 of produces a tag of 8 byte which 
	 * is appended to the cipher text, therefore the out[] buffer needs to have 
	 * a length of tag + ciphertext. In our case cipher_text_len = 0 and tag is 
	 * 8 -> out[8]
	 */
	uint8_t in, out[8];
	if (suite.edhoc_aead == AES_CCM_16_64_128) {
		*mac_len = 8; /*8 byte tag (64bit)*/
	} else {
		return UnsupportedAEADAlgorithm;
	}
	r = aead(ENCRYPT, &in, 0, K_m, sizeof(K_m), IV_m, sizeof(IV_m), A_m,
		 A_m_len, (uint8_t *)&out, 0, mac, *mac_len);
	if (r != EdhocNoError)
		return r;
	PRINT_ARRAY("MAC (MAC_2 or MAC_3)", mac, *mac_len);

	if (!static_dh_auth) {
		/*Encode M_2 structure to be signed or macked*/
		r = m_encode(Signature1, id_cred, id_cred_len, th, th_len, cred,
			     cred_len, ad, ad_len, mac, *mac_len, m, m_len);
		if (r != EdhocNoError)
			return r;
		PRINT_ARRAY("data to be signed (M_2 or M_3)", m, *m_len);
	}
	return EdhocNoError;
}
