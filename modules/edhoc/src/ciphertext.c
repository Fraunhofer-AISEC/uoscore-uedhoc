

#include "../edhoc.h"
#include "../inc/error.h"
#include "../inc/okm.h"
#include "../inc/ciphertext.h"
#include "../inc/crypto_wrapper.h"
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
static enum edhoc_error ciphertext_encrypt_decrypt(
	enum ciphertext ctxt, enum aes_operation op, const uint8_t *in,
	const uint16_t in_len, const uint8_t *key, const uint16_t key_len,
	uint8_t *nonce, const uint16_t nonce_len, const uint8_t *aad,
	const uint16_t aad_len, uint8_t *out, const uint16_t out_len,
	uint8_t *tag, const uint16_t tag_len)
{
	enum edhoc_error r;
	if (ctxt == CIPHERTEXT2) {
		xor_arrays(in, key, key_len, out);
	} else {
		r = aead(op, in, in_len, key, key_len, nonce, nonce_len, aad,
			 aad_len, out, out_len, tag, tag_len);
		if (r != edhoc_no_error) {
			return r;
		}
	}
	return edhoc_no_error;
}

/**
 * 
 * 
 * 
 */
static enum edhoc_error
ciphertext_key_gen(enum ciphertext ctxt, enum hash_alg edhoc_hash, uint8_t *prk,
		   uint8_t prk_len, uint8_t *th, uint8_t th_len, uint8_t *key,
		   uint32_t key_len, uint8_t *iv, uint32_t iv_len)
{
	enum edhoc_error r;
	switch (ctxt) {
	case CIPHERTEXT2:
		r = okm_calc(edhoc_hash, prk, prk_len, th, th_len,
			     "KEYSTREAM_2", NULL, 0, key, key_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("KEYSTREAM_2", key, key_len);
		break;

	case CIPHERTEXT3:
		r = okm_calc(edhoc_hash, prk, prk_len, th, th_len, "K_3", NULL,
			     0, key, key_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("K_3", key, key_len);

		r = okm_calc(edhoc_hash, prk, prk_len, th, th_len, "IV_3", NULL,
			     0, iv, iv_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("IV_3", iv, iv_len);
		break;

	case CIPHERTEXT4:
		r = edhoc_exporter(edhoc_hash, prk, prk_len, th, th_len,
				   "EDHOC_K_4", key, key_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("K_4", key, key_len);

		r = edhoc_exporter(edhoc_hash, prk, prk_len, th, th_len,
				   "EDHOC_IV_4", iv, iv_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("IV_4", iv, iv_len);
		break;
	}
	return edhoc_no_error;
}

/**
 * @brief	Decrypts and decomposes a ciphertext. For that the function 
 * 		first computes the required key form prk and th.
 * 
 * 
 */
enum edhoc_error ciphertext_decrypt_split(
	enum ciphertext ctxt, struct suite *suite, uint8_t *prk,
	uint8_t prk_len, uint8_t *th, uint8_t th_len, uint8_t *ciphertext,
	uint32_t ciphertext_len, uint8_t *id_cred, uint32_t *id_cred_len,
	uint8_t *signature_or_mac, uint32_t *signature_or_mac_len, uint8_t *ead,
	uint32_t *ead_len)
{
	enum edhoc_error r;

	/*generate key and iv (no iv in for ciphertext 2)*/
	uint32_t key_len = get_aead_key_len(suite->edhoc_aead);
	if (ctxt == CIPHERTEXT2) {
		key_len = ciphertext_len;
	}
	uint8_t key[key_len];

	uint32_t iv_len = get_aead_iv_len(suite->edhoc_aead);
	uint8_t iv[iv_len];

	r = ciphertext_key_gen(ctxt, suite->edhoc_hash, prk, prk_len, th,
			       th_len, key, key_len, iv, iv_len);
	if (r != edhoc_no_error) {
		return r;
	}

	/*Associated data*/
	uint8_t associated_data[ASSOCIATED_DATA_DEFAULT_SIZE];
	uint16_t associated_data_len = sizeof(associated_data);
	r = associated_data_encode(th, th_len, (uint8_t *)&associated_data,
				   &associated_data_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("associated_data", associated_data, associated_data_len);

	uint32_t tag_len = get_mac_len(suite->edhoc_aead);
	//uint8_t tag[tag_len];
	uint32_t plaintext_len = ciphertext_len - tag_len;
	uint8_t plaintext[plaintext_len];
	r = ciphertext_encrypt_decrypt(ctxt, DECRYPT, ciphertext,
				       ciphertext_len, key, key_len, iv, iv_len,
				       associated_data, associated_data_len,
				       plaintext, plaintext_len,
				       ciphertext - tag_len, tag_len);
	if (r != edhoc_no_error) {
		return r;
	}

	if (ctxt == CIPHERTEXT4 && plaintext_len != 0) {
		r = decode_byte_string(plaintext, plaintext_len, ead, ead_len);
		;
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("EAD_4", ead, ead_len);
	} else if (ctxt == CIPHERTEXT4 && plaintext_len == 0) {
		ead = NULL;
		*ead_len = 0;
		PRINT_MSG("No EAD_4\n");
	} else {
		r = plaintext_split(plaintext, plaintext_len, id_cred,
				    id_cred_len, signature_or_mac,
				    signature_or_mac_len, ead, ead_len);
		if (r != edhoc_no_error) {
			return r;
		}
	}

	return edhoc_no_error;
}

/**
 * @brief       Computes ciphertext_2, ciphertext_3 or ciphertext_4. For that 
 *              the function first computes the required key form prk and th 
 *              and constructs the plaintext. 
 * @param
 * 
 */
enum edhoc_error ciphertext_gen(enum ciphertext ctxt, enum hash_alg edhoc_hash,
				uint8_t *id_cred, uint32_t id_cred_len,
				uint8_t *signature_or_mac,
				uint32_t signature_or_mac_len, uint8_t *ead,
				uint32_t ead_len, uint8_t *prk, uint8_t prk_len,
				uint8_t *th, uint8_t th_len,
				uint8_t *ciphertext, uint32_t *ciphertext_len)
{
	enum edhoc_error r;

	/*Encode plaintext*/
	uint8_t plaintext[PLAINTEXT_DEFAULT_SIZE];
	uint32_t plaintext_len = sizeof(plaintext);

	uint8_t signature_or_mac_enc[signature_or_mac_len + 2];
	uint64_t signature_or_mac_enc_len = sizeof(signature_or_mac_enc);
	r = encode_byte_string(signature_or_mac, signature_or_mac_len,
			       signature_or_mac_enc, &signature_or_mac_enc_len);
	if (r != edhoc_no_error) {
		return r;
	}

	if (ctxt != CIPHERTEXT4) {
		uint8_t kid_buf[KID_DEFAULT_SIZE];
		uint32_t kid_len = sizeof(kid_buf);
		r = id_cred2kid(id_cred, id_cred_len, kid_buf, &kid_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("kid", kid_buf, kid_len);

		if (kid_len != 0) {
			/*id_cred_x is a KID*/
			r = _memcpy_s(plaintext, plaintext_len, kid_buf,
				      kid_len);
			if (r != edhoc_no_error) {
				return r;
			}
			r = _memcpy_s(plaintext + kid_len,
				      plaintext_len - kid_len,
				      signature_or_mac_enc,
				      signature_or_mac_enc_len);
			if (r != edhoc_no_error) {
				return r;
			}
			plaintext_len = signature_or_mac_enc_len + kid_len;
		} else {
			/*id_cred_x is NOT a KID*/
			r = _memcpy_s(plaintext, plaintext_len, id_cred,
				      id_cred_len);
			if (r != edhoc_no_error) {
				return r;
			}
			r = _memcpy_s(plaintext + id_cred_len,
				      plaintext_len - id_cred_len,
				      signature_or_mac_enc,
				      signature_or_mac_enc_len);
			if (r != edhoc_no_error) {
				return r;
			}
			plaintext_len = id_cred_len + signature_or_mac_enc_len;
		}
	} else {
		plaintext_len = 0;
	}
	if (ead_len > 0) {
		r = _memcpy_s(plaintext + plaintext_len,
			      sizeof(plaintext) - plaintext_len, ead, ead_len);
		if (r != edhoc_no_error) {
			return r;
		}
		plaintext_len += ead_len;
	}

	PRINT_ARRAY("plaintext", plaintext, plaintext_len);

	/*Calculate keys and encrypt*/
	if (ctxt == CIPHERTEXT2) {
		/*Derive KEYSTREAM_2*/
		uint64_t KEYSTREAM_2_len = plaintext_len;
		uint8_t KEYSTREAM_2[KEYSTREAM_2_len];
		r = okm_calc(edhoc_hash, prk, prk_len, th, th_len,
			     "KEYSTREAM_2", NULL, 0, KEYSTREAM_2,
			     KEYSTREAM_2_len);
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("KEYSTREAM_2", KEYSTREAM_2, sizeof(KEYSTREAM_2));

		/*encrypt*/
		//todo move this to a separate function
		for (uint64_t i = 0; i < KEYSTREAM_2_len; i++) {
			ciphertext[i] = KEYSTREAM_2[i] ^ plaintext[i];
		}
		*ciphertext_len = plaintext_len;
		return edhoc_no_error;
	}

	/*Calculate key, nonce and encrypt*/
	//todo take the length of the key and nonce from the ciphersuite
	uint8_t K[16], N[13];
	if (ctxt == CIPHERTEXT3) {
		r = okm_calc(edhoc_hash, prk, prk_len, th, th_len, "K_3", NULL,
			     0, K, sizeof(K));
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("K_3", K, sizeof(K));

		/*Calculate IV_3*/
		r = okm_calc(edhoc_hash, prk, prk_len, th, th_len, "IV_3", NULL,
			     0, N, sizeof(N));
		if (r != edhoc_no_error) {
			return r;
		}
		PRINT_ARRAY("IV_3", N, sizeof(N));
	}

	if (ctxt == CIPHERTEXT4) {
		r = edhoc_exporter(SHA_256, prk, prk_len, th, th_len,
				   "EDHOC_K_4", K, sizeof(K));
		if (r != edhoc_no_error) {
			return r;
		}
		r = edhoc_exporter(SHA_256, prk, prk_len, th, th_len,
				   "EDHOC_IV_4", N, sizeof(N));
		if (r != edhoc_no_error) {
			return r;
		}
	}

	/*Associated data*/
	uint8_t associated_data[ASSOCIATED_DATA_DEFAULT_SIZE];
	uint16_t associated_data_len = sizeof(associated_data);
	r = associated_data_encode(th, th_len, (uint8_t *)&associated_data,
				   &associated_data_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("associated_data", associated_data, associated_data_len);

	/*Ciphertext 3 calculate*/
	uint8_t mac_len = 8;
	//todo move this to separate function
	// if (suite.edhoc_aead == AES_CCM_16_128_128) {
	// 	mac_len = 16;
	// }
	uint8_t tag[mac_len];
	// uint32_t ciphertext_3_len = plaintext_len;
	// uint8_t ciphertext_3[ciphertext_3_len + mac_len];
	*ciphertext_len = plaintext_len;

	r = aead(ENCRYPT, plaintext, plaintext_len, K, sizeof(K), N, sizeof(N),
		 associated_data, associated_data_len, ciphertext,
		 *ciphertext_len, tag, mac_len);
	if (r != edhoc_no_error) {
		return r;
	}
	*ciphertext_len += mac_len;
	PRINT_ARRAY("ciphertext_2/3/4", ciphertext, *ciphertext_len);
	return edhoc_no_error;
}
