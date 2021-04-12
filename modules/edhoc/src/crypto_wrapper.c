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

#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
#include <c25519.h>
#include <edsign.h>
#include <string.h>
#include <compact_x25519.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/ccm_mode.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/hmac.h>
#endif

EdhocError __attribute__((weak))
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
			return AEADAuthenticationFailed;

	} else {
		result = tc_ccm_generation_encryption(
			out, (out_len + tag_len), aad, aad_len, in, in_len, &c);

		memcpy(tag, out + out_len, tag_len);

		if (result != 1)
			return ErrorDuringAEAD;
	}
#endif
	return EdhocNoError;
}

EdhocError __attribute__((weak))
sign(enum sign_alg_curve curve, const uint8_t *sk, const uint8_t sk_len,
     const uint8_t *pk, const uint8_t pk_len, const uint8_t *msg,
     const uint16_t msg_len, uint8_t *out, uint32_t *out_len)
{
	if (curve == Ed25519_SIGN) {
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		edsign_sign(out, pk, sk, msg, msg_len);
#endif
	}
	return EdhocNoError;
}

EdhocError __attribute__((weak))
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
	return EdhocNoError;
}

EdhocError __attribute__((weak))
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
	return EdhocNoError;
}

EdhocError __attribute__((weak))
hkdf_expand(enum hash_alg alg, const uint8_t *prk, const uint8_t prk_len,
	    const uint8_t *info, const uint8_t info_len, uint8_t *out,
	    uint64_t out_len)
{
	if (alg == SHA_256) {
		/* "N = ceil(L/HashLen)" */
		uint32_t iterations = (out_len + 31) / 32;
		/* "L length of output keying material in octets (<= 255*HashLen)"*/
		if (iterations > 255) {
			return ErrorDuringHKDFCalculation;
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
	return EdhocNoError;
}

EdhocError __attribute__((weak))
shared_secret_derive(enum ecdh_curve curve, const uint8_t *sk,
		     const uint32_t sk_len, const uint8_t *pk,
		     const uint32_t pk_len, uint8_t *shared_secret)
{
	if (curve == X25519) {
#ifdef EDHOC_WITH_TINYCRYPT_AND_C25519
		uint8_t e[F25519_SIZE];
		f25519_copy(e, sk);
		c25519_prepare(e);
		c25519_smult(shared_secret, pk, e);
#endif
	}

	return EdhocNoError;
}

EdhocError __attribute__((weak))
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
		return UnsupportedEcdhCurve;
	}
	return EdhocNoError;
}

EdhocError __attribute__((weak))
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

	return EdhocNoError;
}
