/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/crypto_wrapper.h"

#include "../inc/byte_array.h"
#include "../inc/error.h"
#include "../inc/print_util.h"
#include "../inc/suites.h"

//#define EDHOC_WITH_TINYCRYPT_AND_C25519

#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
#include <c25519.h>
#include <edsign.h>
#include <string.h>
#include <compact_x25519.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/ccm_mode.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/hmac.h>
#include <tinycrypt/ecc_dsa.h>
#include <tinycrypt/ecc_dh.h>
#endif

enum edhoc_error __attribute__((weak))
aead(enum aes_operation op, const uint8_t *in, const uint16_t in_len,
     const uint8_t *key, const uint16_t key_len, uint8_t *nonce,
     const uint16_t nonce_len, const uint8_t *aad, const uint16_t aad_len,
     uint8_t *out, const uint16_t out_len, uint8_t *tag, const uint16_t tag_len)
{
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
	int result;
	struct tc_ccm_mode_struct c;
	struct tc_aes_key_sched_struct sched;
	tc_aes128_set_encrypt_key(&sched, key);
	tc_ccm_config(&c, &sched, nonce, nonce_len, 8);

	if (op == DECRYPT) {
		result = tc_ccm_decryption_verification(
			out, out_len, aad, aad_len, in, in_len, &c);

		if (result == 0)
			return aead_authentication_failed;

	} else {
		result = tc_ccm_generation_encryption(
			out, (out_len + tag_len), aad, aad_len, in, in_len, &c);

		memcpy(tag, out + out_len, tag_len);

		if (result != 1)
			return aead_failed;
	}
#endif
	return edhoc_no_error;
}

enum edhoc_error __attribute__((weak))
sign(enum sign_alg_curve curve, const uint8_t *sk, const uint8_t sk_len,
     const uint8_t *pk, const uint8_t pk_len, const uint8_t *msg,
     const uint16_t msg_len, uint8_t *out, uint32_t *out_len)
{
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
	if (curve == Ed25519_SIGN) {
		edsign_sign(out, pk, sk, msg, msg_len);
	}
	if (curve == P_256_SIGN) {
		//unsigned int digest[TC_SHA256_DIGEST_SIZE / 4];
		uint8_t digest_bytes[TC_SHA256_DIGEST_SIZE];
		struct tc_sha256_state_struct s;
		int r;
		r = tc_sha256_init(&s);
		if (r != TC_CRYPTO_SUCCESS) {
			return sha_failed;
		};
		r = tc_sha256_update(&s, msg, msg_len);
		if (r != TC_CRYPTO_SUCCESS) {
			return sha_failed;
		};
		r = tc_sha256_final(digest_bytes, &s);
		if (r != TC_CRYPTO_SUCCESS) {
			return sha_failed;
		};
		/* if digest larger than ECC scalar, drop the end
		 * if digest smaller than ECC scalar, zero-pad front */
		// int hash_dwords = TC_SHA256_DIGEST_SIZE / 4;
		// if (NUM_ECC_WORDS < hash_dwords) {
		// 	hash_dwords = NUM_ECC_WORDS;
		// }

		// memset(digest, 0, NUM_ECC_BYTES - 4 * hash_dwords);
		// uECC_vli_bytesToNative(digest + (NUM_ECC_WORDS - hash_dwords),
		// 		       digest_bytes, TC_SHA256_DIGEST_SIZE);

		// unsigned int private[NUM_ECC_WORDS];
		// uint8_t private_bytes[NUM_ECC_BYTES];

		r = uECC_sign(sk, digest_bytes, sizeof(digest_bytes), out,
			      uECC_secp256r1());
		if (r != TC_CRYPTO_SUCCESS) {
			return sign_failed;
		};
	}
#endif
	return edhoc_no_error;
}

enum edhoc_error __attribute__((weak))
verify(enum sign_alg_curve curve, const uint8_t *pk, const uint8_t pk_len,
       const uint8_t *msg, const uint16_t msg_len, const uint8_t *sgn,
       const uint16_t sgn_len, bool *result)
{
	if (curve == Ed25519_SIGN) {
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		int verified = edsign_verify(sgn, pk, msg, msg_len);
		if (verified) {
			*result = true;
		} else {
			*result = false;
		}
#endif
	}
	return edhoc_no_error;
}

enum edhoc_error __attribute__((weak))
hkdf_extract(enum hash_alg alg, const uint8_t *salt, uint32_t salt_len,
	     uint8_t *ikm, uint8_t ikm_len, uint8_t *out)
{
	/*all currently prosed suites use hmac-sha256*/
	if (alg == SHA_256) {
		/*"Note that [RFC5869] specifies that if the salt is not provided, it is
    set to a string of zeros.  For implementation purposes, not providing
    the salt is the same as setting the salt to the empty byte string.
    OSCORE sets the salt default value to empty byte string, which is
    converted to a string of zeroes (see Section 2.2 of [RFC5869])".*/

#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		struct tc_hmac_state_struct h;
		memset(&h, 0x00, sizeof(h));
		if (salt == NULL || salt_len == 0) {
			uint8_t zero_salt[32] = { 0 };
			tc_hmac_set_key(&h, zero_salt, 32);
		} else {
			tc_hmac_set_key(&h, salt, salt_len);
		}
		tc_hmac_init(&h);
		tc_hmac_update(&h, ikm, ikm_len);
		tc_hmac_final(out, TC_SHA256_DIGEST_SIZE, &h);
#endif
	}
	return edhoc_no_error;
}

enum edhoc_error __attribute__((weak))
hkdf_expand(enum hash_alg alg, const uint8_t *prk, const uint8_t prk_len,
	    const uint8_t *info, const uint8_t info_len, uint8_t *out,
	    uint64_t out_len)
{
	if (alg == SHA_256) {
		/* "N = ceil(L/HashLen)" */
		uint32_t iterations = (out_len + 31) / 32;
		/* "L length of output keying material in octets (<= 255*HashLen)"*/
		if (iterations > 255) {
			return hkdf_fialed;
		}

		uint8_t t[32] = { 0 };

#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		struct tc_hmac_state_struct h;
		for (uint8_t i = 1; i <= iterations; i++) {
			memset(&h, 0x00, sizeof(h));
			tc_hmac_set_key(&h, prk, prk_len);
			tc_hmac_init(&h);
			if (i > 1) {
				tc_hmac_update(&h, t, 32);
			}
			tc_hmac_update(&h, info, info_len);
			tc_hmac_update(&h, &i, 1);
			tc_hmac_final(t, TC_SHA256_DIGEST_SIZE, &h);
			if (out_len < i * 32) {
				memcpy(&out[(i - 1) * 32], t, out_len % 32);
			} else {
				memcpy(&out[(i - 1) * 32], t, 32);
			}
		}
#endif
	}
	return edhoc_no_error;
}

// static void decompress(const uint8_t *compressed, uint8_t *public_key,
// 		       uECC_Curve curve)
// {
// #if uECC_VLI_NATIVE_LITTLE_ENDIAN
// 	uECC_word_t *point = (uECC_word_t *)public_key;
// #else
// 	uECC_word_t point[4 * 2];
// #endif
// 	uECC_word_t *y = point + curve->num_words;
// #if uECC_VLI_NATIVE_LITTLE_ENDIAN
// 	bcopy(public_key, compressed + 1, curve->num_bytes);
// #else
// 	uECC_vli_bytesToNative(point, compressed + 1, curve->num_bytes);
// #endif
// 	curve->x_side(y, point, curve);
// 	curve->mmod_fast(y, curve);

// 	if ((y[0] & 0x01) != (compressed[0] & 0x01)) {
// 		uECC_vli_sub(y, curve->p, y, curve->num_words);
// 	}

// #if uECC_VLI_NATIVE_LITTLE_ENDIAN == 0
// 	uECC_vli_nativeToBytes(public_key, curve->num_bytes, point);
// 	uECC_vli_nativeToBytes(public_key + curve->num_bytes, curve->num_bytes,
// 			       y);
// #endif
// }

enum edhoc_error __attribute__((weak))
shared_secret_derive(enum ecdh_curve curve, const uint8_t *sk,
		     const uint32_t sk_len, const uint8_t *pk,
		     const uint32_t pk_len, uint8_t *shared_secret)
{
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
	if (curve == X25519) {
		uint8_t e[F25519_SIZE];
		f25519_copy(e, sk);
		c25519_prepare(e);
		c25519_smult(shared_secret, pk, e);
	} else if (curve == P_256_ECDH) {
		// 	uint8_t G_X[] = { 0x47, 0x57, 0x76, 0xf8, 0x44, 0x97, 0x9a,
		// 			  0xd0, 0xb4, 0x63, 0xc5, 0xa6, 0xa4, 0x34,
		// 			  0x3a, 0x66, 0x3d, 0x17, 0xa3, 0xa8, 0x0e,
		// 			  0x38, 0xa8, 0x1d, 0x3e, 0x34, 0x96, 0xf6,
		// 			  0x06, 0x1f, 0xd7, 0x16 };

		// 	uint8_t Y[] = { 0x73, 0x97, 0xba, 0x34, 0xa7, 0xb6, 0x0a, 0x4d,
		// 			0x98, 0xef, 0x5e, 0x91, 0x56, 0x3f, 0xc8, 0x54,
		// 			0x9f, 0x35, 0x54, 0x49, 0x4f, 0x1f, 0xeb, 0xd4,
		// 			0x65, 0x36, 0x0c, 0x4b, 0x90, 0xe7, 0x41, 0x71 };
		// 	uint8_t public_key[64];
		// 	decompress(G_X, public_key, uECC_secp256r1());
		// 	int r = uECC_shared_secret(public_key, Y, shared_secret,
		// 				   uECC_secp256r1());
		// 	PRINT_ARRAY("G_XY from G_X and Y", shared_secret, 32);

		// 	uint8_t G_Y[] = { 0x81, 0xdf, 0x54, 0xb3, 0x75, 0x6a, 0xcf,
		// 			  0xc8, 0xa1, 0xe9, 0xb0, 0x8b, 0xa1, 0x0d,
		// 			  0xe4, 0xe7, 0xe7, 0xdd, 0x93, 0x45, 0x87,
		// 			  0xa1, 0xec, 0xdb, 0x21, 0xb9, 0x2f, 0x8f,
		// 			  0x22, 0xc3, 0xa3, 0x8d };

		// 	uint8_t X[] = { 0x0a, 0xe7, 0x99, 0x77, 0x5c, 0xb1, 0x51, 0xbf,
		// 			0xc2, 0x54, 0x87, 0x35, 0xf4, 0x4a, 0xcf, 0x1d,
		// 			0x94, 0x29, 0xcf, 0x9a, 0x95, 0xdd, 0xcd, 0x2a,
		// 			0x13, 0x9e, 0x3a, 0x28, 0xd8, 0x63, 0xa0, 0x81 };
		// 	decompress(G_Y, public_key, uECC_secp256r1());
		// 	r = uECC_shared_secret(public_key, X, shared_secret,
		// 			       uECC_secp256r1());
		// 	PRINT_ARRAY("G_XY from G_Y and X", shared_secret, 32);
		// 	//int r = uECC_shared_secret(pk, sk,shared_secret, uECC_secp256r1());
		// 	if (r != TC_CRYPTO_SUCCESS) {
		// 		return dh_failed;
		// 	}
	}
#endif
	return edhoc_no_error;
}

enum edhoc_error __attribute__((weak))
ephemeral_dh_key_gen(enum ecdh_curve curve, uint32_t seed, uint8_t *sk,
		     uint8_t *pk)
{
	uint8_t extended_seed[32];
	if (curve == X25519) {
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		struct tc_sha256_state_struct s;
		tc_sha256_init(&s);
		tc_sha256_update(&s, (uint8_t *)&seed, sizeof(seed));
		tc_sha256_final(extended_seed, &s);

		compact_x25519_keygen(sk, pk, extended_seed);
#endif
	} else {
		return unsupported_ecdh_curve;
	}
	return edhoc_no_error;
}

enum edhoc_error __attribute__((weak))
hash(enum hash_alg alg, const uint8_t *in, const uint64_t in_len, uint8_t *out)
{
	/*all currently prosed suites use sha256*/
	if (alg == SHA_256) {
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		struct tc_sha256_state_struct s;
		tc_sha256_init(&s);
		tc_sha256_update(&s, in, in_len);
		tc_sha256_final(out, &s);
#endif
	}

	return edhoc_no_error;
}
