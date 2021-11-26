/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "crypto_wrapper.h"

#include "byte_array.h"
#include "oscore_edhoc_error.h"
#include "print_util.h"
#include "suites.h"

#ifdef EDHOC_WITH_MBEDTLS
/*
IMPORTANT!!!!
make sure MBEDTLS_PSA_CRYPTO_CONFIG is defined in include/mbedtls/mbedtls_config.h


modify setting in include/psa/crypto_config.h 
*/
#include <psa/crypto.h>
#endif

#if defined(TINYCRYPT) || defined(COMPACT25519)
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

enum err __attribute__((weak))
aead(enum aes_operation op, const uint8_t *in, const uint16_t in_len,
     const uint8_t *key, const uint16_t key_len, uint8_t *nonce,
     const uint16_t nonce_len, const uint8_t *aad, const uint16_t aad_len,
     uint8_t *out, const uint16_t out_len, uint8_t *tag, const uint16_t tag_len)
{
#if defined(TINYCRYPT) || defined(COMPACT25519)
	struct tc_ccm_mode_struct c;
	struct tc_aes_key_sched_struct sched;
	TRY_EXPECT(tc_aes128_set_encrypt_key(&sched, key), 1);
	TRY_EXPECT(tc_ccm_config(&c, &sched, nonce, nonce_len, tag_len), 1);

	if (op == DECRYPT) {
		TRY_EXPECT(tc_ccm_decryption_verification(
				   out, out_len, aad, aad_len, in, in_len, &c),
			   1);

	} else {
		TRY_EXPECT(tc_ccm_generation_encryption(
				   out, (out_len + tag_len), aad, aad_len, in,
				   in_len, &c),
			   1);
		memcpy(tag, out + out_len, tag_len);
	}
#endif
#ifdef EDHOC_WITH_MBEDTLS
	/*TODO add mbedtls code here*/
#endif
	return ok;
}

enum err __attribute__((weak))
sign(enum sign_alg alg, const uint8_t *sk, const uint8_t sk_len,
     const uint8_t *pk, const uint8_t pk_len, const uint8_t *msg,
     const uint16_t msg_len, uint8_t *out, uint32_t *out_len)
{
	if (alg == EdDSA) {
#if defined(TINYCRYPT) || defined(COMPACT25519)
		edsign_sign(out, pk, sk, msg, msg_len);
#endif
	}
	if (alg == ES256) {
#ifdef EDHOC_WITH_MBEDTLS
		/*TODO add mbedtls code here*/
#endif
	}

	return ok;
}

enum err __attribute__((weak))
verify(enum sign_alg alg, const uint8_t *pk, const uint8_t pk_len,
       const uint8_t *msg, const uint16_t msg_len, const uint8_t *sgn,
       const uint16_t sgn_len, bool *result)
{
	if (alg == EdDSA) {
#if defined(TINYCRYPT) || defined(COMPACT25519)
		int verified = edsign_verify(sgn, pk, msg, msg_len);
		if (verified) {
			*result = true;
		} else {
			*result = false;
		}
#endif
	}
	if (alg == ES256) {
#ifdef EDHOC_WITH_MBEDTLS
		/*TODO add mbedtls code here*/
#endif
	}
	return ok;
}

enum err __attribute__((weak))
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

#if defined(TINYCRYPT) || defined(COMPACT25519)
		struct tc_hmac_state_struct h;
		memset(&h, 0x00, sizeof(h));
		if (salt == NULL || salt_len == 0) {
			uint8_t zero_salt[32] = { 0 };
			TRY_EXPECT(tc_hmac_set_key(&h, zero_salt, 32), 1);
		} else {
			TRY_EXPECT(tc_hmac_set_key(&h, salt, salt_len), 1);
		}
		TRY_EXPECT(tc_hmac_init(&h), 1);
		TRY_EXPECT(tc_hmac_update(&h, ikm, ikm_len), 1);
		TRY_EXPECT(tc_hmac_final(out, TC_SHA256_DIGEST_SIZE, &h), 1);
#endif
	}
	return ok;
}

enum err __attribute__((weak))
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

#if defined(TINYCRYPT) || defined(COMPACT25519)
		struct tc_hmac_state_struct h;
		for (uint8_t i = 1; i <= iterations; i++) {
			memset(&h, 0x00, sizeof(h));
			TRY_EXPECT(tc_hmac_set_key(&h, prk, prk_len), 1);
			tc_hmac_init(&h);
			if (i > 1) {
				TRY_EXPECT(tc_hmac_update(&h, t, 32), 1);
			}
			TRY_EXPECT(tc_hmac_update(&h, info, info_len), 1);
			TRY_EXPECT(tc_hmac_update(&h, &i, 1), 1);
			TRY_EXPECT(tc_hmac_final(t, TC_SHA256_DIGEST_SIZE, &h),
				   1);
			if (out_len < i * 32) {
				memcpy(&out[(i - 1) * 32], t, out_len % 32);
			} else {
				memcpy(&out[(i - 1) * 32], t, 32);
			}
		}
#endif
	}
	return ok;
}

enum err __attribute__((weak))
hkdf_sha_256(struct byte_array *master_secret, struct byte_array *master_salt,
	     struct byte_array *info, struct byte_array *out)
{
#ifdef TINYCRYPT
	uint8_t prk[32];
	TRY(hkdf_extract(SHA_256, master_salt->ptr, master_salt->len,
			 master_secret->ptr, master_secret->len, prk));

	TRY(hkdf_expand(SHA_256, prk, sizeof(prk), info->ptr, info->len,
			out->ptr, out->len));

#endif /* TINYCRYPT */
}

enum err __attribute__((weak))
shared_secret_derive(enum ecdh_alg alg, const uint8_t *sk,
		     const uint32_t sk_len, const uint8_t *pk,
		     const uint32_t pk_len, uint8_t *shared_secret)
{
	if (alg == X25519) {
#if defined(TINYCRYPT) || defined(COMPACT25519)
		uint8_t e[F25519_SIZE];
		f25519_copy(e, sk);
		c25519_prepare(e);
		c25519_smult(shared_secret, pk, e);
#endif
	}
	if (alg == P256) {
#ifdef EDHOC_WITH_MBEDTLS
		psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
		psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
		psa_ecc_family_t ecc_family = PSA_ECC_FAMILY_SECP_R1;
		size_t key_bits = 256;
		psa_key_id_t key = PSA_KEY_HANDLE_INIT;

		psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
		psa_set_key_algorithm(&attributes, alg);
		psa_set_key_type(&attributes,
				 PSA_KEY_TYPE_ECC_KEY_PAIR(ecc_family));
		psa_set_key_bits(&attributes, key_bits);

		TRY(psa_import_key(&attributes, sk, sk_len, &key));

		uint32_t lenhgt;
		psa_raw_key_agreement(PSA_ALG_ECDH, key, pk, pk_len,
				      shared_secret, 32, &lenhgt);

		if (lenhgt != 32) {
			return dh_failed;
		}
#endif
	}

	return ok;
}

enum err __attribute__((weak))
ephemeral_dh_key_gen(enum ecdh_alg alg, uint32_t seed, uint8_t *sk, uint8_t *pk)
{
	if (alg == X25519) {
#if defined(TINYCRYPT) || defined(COMPACT25519)
		uint8_t extended_seed[32];
		struct tc_sha256_state_struct s;
		TRY_EXPECT(tc_sha256_init(&s), 1);
		TRY_EXPECT(tc_sha256_update(&s, (uint8_t *)&seed, sizeof(seed)),
			   1);
		TRY_EXPECT(tc_sha256_final(extended_seed, &s), 1);

		compact_x25519_keygen(sk, pk, extended_seed);
#endif
	} else {
		return unsupported_ecdh_curve;
	}
	return ok;
}

enum err __attribute__((weak))
hash(enum hash_alg alg, const uint8_t *in, const uint64_t in_len, uint8_t *out)
{
	if (alg == SHA_256) {
#if defined(TINYCRYPT) || defined(COMPACT25519)
		struct tc_sha256_state_struct s;
		TRY_EXPECT(tc_sha256_init(&s), 1);
		TRY_EXPECT(tc_sha256_update(&s, in, in_len), 1);
		TRY_EXPECT(tc_sha256_final(out, &s), 1);
#endif
#ifdef EDHOC_WITH_MBEDTLS
		uint32_t length;
		TRY(psa_hash_compute(PSA_ALG_SHA_256, in, in_len, out, 32,
				     &length));
		if (length != 32) {
			return sha_failed;
		}
#endif
	}

	return ok;
}
