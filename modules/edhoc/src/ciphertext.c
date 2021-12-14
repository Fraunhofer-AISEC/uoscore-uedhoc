

#include "../edhoc.h"
#include "../../common/inc/crypto_wrapper.h"
#include "../../common/inc/oscore_edhoc_error.h"
#include "../inc/okm.h"
#include "../inc/ciphertext.h"
#include "../inc/signature_or_mac_msg.h"
#include "../inc/plaintext.h"
#include "../inc/memcpy_s.h"
#include "../inc/associated_data_encode.h"
#include "../inc/suites.h"

/**
 * 
 * 
 */
static inline void xor_arrays(const uint8_t *in1, const uint8_t *in2,
			      uint32_t len, uint8_t *out)
{
	for (uint32_t i = 0; i < len; i++) {
		out[i] = in1[i] ^ in2[i];
	}
}

/**
 * 
 * 
 * 
 */
static enum err ciphertext_encrypt_decrypt(
	enum ciphertext ctxt, enum aes_operation op, const uint8_t *in,
	const uint32_t in_len, const uint8_t *key, const uint32_t key_len,
	uint8_t *nonce, const uint32_t nonce_len, const uint8_t *aad,
	const uint32_t aad_len, uint8_t *out, const uint32_t out_len,
	uint8_t *tag, const uint32_t tag_len)
{
	if (ctxt == CIPHERTEXT2) {
		xor_arrays(in, key, key_len, out);
	} else {
		PRINT_ARRAY("in", in, in_len);
		TRY(aead(op, in, in_len, key, key_len, nonce, nonce_len, aad,
			 aad_len, out, out_len, tag, tag_len));
	}
	return ok;
}

/**
 * 
 * 
 * 
 */
static enum err
ciphertext_key_gen(enum ciphertext ctxt, enum hash_alg edhoc_hash, uint8_t *prk,
		   uint32_t prk_len, uint8_t *th, uint32_t th_len, uint8_t *key,
		   uint32_t key_len, uint8_t *iv, uint32_t iv_len)
{
	switch (ctxt) {
	case CIPHERTEXT2:
		TRY(okm_calc(edhoc_hash, prk, prk_len, th, th_len,
			     "KEYSTREAM_2", NULL, 0, key, key_len));
		PRINT_ARRAY("KEYSTREAM_2", key, key_len);
		break;

	case CIPHERTEXT3:
		TRY(okm_calc(edhoc_hash, prk, prk_len, th, th_len, "K_3", NULL,
			     0, key, key_len));
		PRINT_ARRAY("K_3", key, key_len);
		TRY(okm_calc(edhoc_hash, prk, prk_len, th, th_len, "IV_3", NULL,
			     0, iv, iv_len));
		PRINT_ARRAY("IV_3", iv, iv_len);
		break;

	case CIPHERTEXT4:
		TRY(edhoc_exporter(edhoc_hash, prk, prk_len, th, th_len,
				   "EDHOC_K_4", key, key_len));
		PRINT_ARRAY("K_4", key, key_len);
		TRY(edhoc_exporter(edhoc_hash, prk, prk_len, th, th_len,
				   "EDHOC_IV_4", iv, iv_len));
		PRINT_ARRAY("IV_4", iv, iv_len);
		break;
	}
	return ok;
}

/**
 * @brief	Decrypts and decomposes a ciphertext. For that the function 
 * 		first computes the required key form prk and th.
 * 
 * 
 */
enum err ciphertext_decrypt_split(enum ciphertext ctxt, struct suite *suite,
				  uint8_t *prk, uint32_t prk_len, uint8_t *th,
				  uint32_t th_len, uint8_t *ciphertext,
				  uint32_t ciphertext_len, uint8_t *id_cred,
				  uint32_t *id_cred_len,
				  uint8_t *signature_or_mac,
				  uint32_t *signature_or_mac_len, uint8_t *ead,
				  uint32_t *ead_len)
{
	/*generate key and iv (no iv in for ciphertext 2)*/
	uint32_t key_len;
	uint8_t key[CIPHERTEXT2_DEFAULT_SIZE];
	if (ctxt == CIPHERTEXT2) {
		key_len = ciphertext_len;
	} else {
		key_len = get_aead_key_len(suite->edhoc_aead);
	}
	TRY(check_buffer_size(CIPHERTEXT2_DEFAULT_SIZE, key_len));

	uint32_t iv_len = get_aead_iv_len(suite->edhoc_aead);
	TRY(check_buffer_size(AEAD_IV_DEFAULT_SIZE, iv_len));
	uint8_t iv[AEAD_IV_DEFAULT_SIZE];

	TRY(ciphertext_key_gen(ctxt, suite->edhoc_hash, prk, prk_len, th,
			       th_len, key, key_len, iv, iv_len));

	/*Associated data*/
	uint8_t associated_data[ASSOCIATED_DATA_DEFAULT_SIZE];
	uint32_t associated_data_len = sizeof(associated_data);
	TRY(associated_data_encode(th, th_len, (uint8_t *)&associated_data,
				   &associated_data_len));

	PRINT_ARRAY("associated_data", associated_data, associated_data_len);

	uint32_t tag_len = get_aead_mac_len(suite->edhoc_aead);
	uint32_t plaintext_len = ciphertext_len;
	if (ctxt != CIPHERTEXT2) {
		plaintext_len -= tag_len;
	}
	TRY(check_buffer_size(PLAINTEXT_DEFAULT_SIZE, plaintext_len));
	uint8_t plaintext[PLAINTEXT_DEFAULT_SIZE];
	TRY(ciphertext_encrypt_decrypt(
		ctxt, DECRYPT, ciphertext, ciphertext_len, key, key_len, iv,
		iv_len, associated_data, associated_data_len, plaintext,
		plaintext_len, ciphertext - tag_len, tag_len));

	PRINT_ARRAY("plaintext", plaintext, plaintext_len);

	if (ctxt == CIPHERTEXT4 && plaintext_len != 0) {
		TRY(decode_byte_string(plaintext, plaintext_len, ead, ead_len));
		PRINT_ARRAY("EAD_4", ead, *ead_len);
	} else if (ctxt == CIPHERTEXT4 && plaintext_len == 0) {
		ead = NULL;
		*ead_len = 0;
		PRINT_MSG("No EAD_4\n");
	} else {
		TRY(plaintext_split(plaintext, plaintext_len, id_cred,
				    id_cred_len, signature_or_mac,
				    signature_or_mac_len, ead, ead_len));
		PRINT_ARRAY("ID_CRED", id_cred, *id_cred_len);
		PRINT_ARRAY("sign_or_mac", signature_or_mac,
			    *signature_or_mac_len);
		if (*ead_len) {
			PRINT_ARRAY("ead", ead, *ead_len);
		}
	}

	return ok;
}

/**
 * @brief       Computes ciphertext_2, ciphertext_3 or ciphertext_4. For that 
 *              the function first computes the required key form prk and th 
 *              and constructs the plaintext. 
 * @param
 * 
 */
//todo refactore this function simalar to ciphertext_decrypt_split
enum err ciphertext_gen(enum ciphertext ctxt, struct suite *suite,
			uint8_t *id_cred, uint32_t id_cred_len,
			uint8_t *signature_or_mac,
			uint32_t signature_or_mac_len, uint8_t *ead,
			uint32_t ead_len, uint8_t *prk, uint32_t prk_len,
			uint8_t *th, uint32_t th_len, uint8_t *ciphertext,
			uint32_t *ciphertext_len)
{
	/*Encode plaintext*/
	uint8_t plaintext[PLAINTEXT_DEFAULT_SIZE];
	uint32_t plaintext_len = sizeof(plaintext);

	TRY(check_buffer_size(SGN_OR_MAC_DEFAULT_SIZE,
			      signature_or_mac_len + 2));
	uint8_t signature_or_mac_enc[SGN_OR_MAC_DEFAULT_SIZE];
	uint32_t signature_or_mac_enc_len = signature_or_mac_len + 2;
	TRY(encode_byte_string(signature_or_mac, signature_or_mac_len,
			       signature_or_mac_enc,
			       &signature_or_mac_enc_len));

	if (ctxt != CIPHERTEXT4) {
		uint8_t kid_buf[KID_DEFAULT_SIZE];
		uint32_t kid_len = sizeof(kid_buf);
		TRY(id_cred2kid(id_cred, id_cred_len, kid_buf, &kid_len));

		PRINT_ARRAY("kid", kid_buf, kid_len);

		if (kid_len != 0) {
			/*id_cred_x is a KID*/
			TRY(_memcpy_s(plaintext, plaintext_len, kid_buf,
				      kid_len));

			TRY(_memcpy_s(plaintext + kid_len,
				      plaintext_len - kid_len,
				      signature_or_mac_enc,
				      signature_or_mac_enc_len));

			plaintext_len = signature_or_mac_enc_len + kid_len;
		} else {
			/*id_cred_x is NOT a KID*/
			TRY(_memcpy_s(plaintext, plaintext_len, id_cred,
				      id_cred_len));

			TRY(_memcpy_s(plaintext + id_cred_len,
				      plaintext_len - id_cred_len,
				      signature_or_mac_enc,
				      signature_or_mac_enc_len));

			plaintext_len = id_cred_len + signature_or_mac_enc_len;
		}
	} else {
		plaintext_len = 0;
	}
	if (ead_len > 0) {
		TRY(_memcpy_s(plaintext + plaintext_len,
			      (uint32_t)sizeof(plaintext) - plaintext_len, ead,
			      ead_len));

		plaintext_len += ead_len;
	}

	PRINT_ARRAY("plaintext", plaintext, plaintext_len);

	/*Calculate keys and encrypt*/
	if (ctxt == CIPHERTEXT2) {
		/*Derive KEYSTREAM_2*/
		uint32_t KEYSTREAM_2_len = plaintext_len;
		TRY(check_buffer_size(PLAINTEXT_DEFAULT_SIZE, KEYSTREAM_2_len));
		uint8_t KEYSTREAM_2[PLAINTEXT_DEFAULT_SIZE];
		TRY(okm_calc(suite->edhoc_hash, prk, prk_len, th, th_len,
			     "KEYSTREAM_2", NULL, 0, KEYSTREAM_2,
			     KEYSTREAM_2_len));

		PRINT_ARRAY("KEYSTREAM_2", KEYSTREAM_2, sizeof(KEYSTREAM_2));

		/*encrypt*/
		//todo move this to a separate function
		for (uint64_t i = 0; i < KEYSTREAM_2_len; i++) {
			ciphertext[i] = KEYSTREAM_2[i] ^ plaintext[i];
		}
		*ciphertext_len = plaintext_len;
		return ok;
	}

	/*Calculate key, nonce and encrypt*/
	//todo take the length of the key and nonce from the ciphersuite
	uint8_t K[16], N[13];
	if (ctxt == CIPHERTEXT3) {
		TRY(okm_calc(suite->edhoc_hash, prk, prk_len, th, th_len, "K_3",
			     NULL, 0, K, sizeof(K)));
		PRINT_ARRAY("K_3", K, sizeof(K));

		/*Calculate IV_3*/
		TRY(okm_calc(suite->edhoc_hash, prk, prk_len, th, th_len,
			     "IV_3", NULL, 0, N, sizeof(N)));

		PRINT_ARRAY("IV_3", N, sizeof(N));
	}

	if (ctxt == CIPHERTEXT4) {
		TRY(edhoc_exporter(SHA_256, prk, prk_len, th, th_len,
				   "EDHOC_K_4", K, sizeof(K)));
		TRY(edhoc_exporter(SHA_256, prk, prk_len, th, th_len,
				   "EDHOC_IV_4", N, sizeof(N)));
	}

	/*Associated data*/
	uint8_t associated_data[ASSOCIATED_DATA_DEFAULT_SIZE];
	uint32_t associated_data_len = sizeof(associated_data);
	TRY(associated_data_encode(th, th_len, (uint8_t *)&associated_data,
				   &associated_data_len));
	PRINT_ARRAY("associated_data", associated_data, associated_data_len);

	/*Ciphertext 3 calculate*/
	uint32_t mac_len = get_aead_mac_len(suite->edhoc_aead);
	TRY(check_buffer_size(MAC_DEFAULT_SIZE, mac_len));
	uint8_t tag[MAC_DEFAULT_SIZE];
	*ciphertext_len = plaintext_len;

	TRY(aead(ENCRYPT, plaintext, plaintext_len, K, sizeof(K), N, sizeof(N),
		 associated_data, associated_data_len, ciphertext,
		 *ciphertext_len, tag, mac_len));
	*ciphertext_len += mac_len;
	PRINT_ARRAY("ciphertext_2/3/4", ciphertext, *ciphertext_len);
	return ok;
}
