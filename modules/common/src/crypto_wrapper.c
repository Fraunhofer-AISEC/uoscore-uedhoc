/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include <string.h>

#include "crypto_wrapper.h"
#include "byte_array.h"
#include "oscore_edhoc_error.h"
#include "print_util.h"
#include "suites.h"
#include "memcpy_s.h"
#include "../../edhoc/edhoc.h"

//#define MBEDTLS

#ifdef MBEDTLS
/*
IMPORTANT!!!!
make sure MBEDTLS_PSA_CRYPTO_CONFIG is defined in include/mbedtls/mbedtls_config.h


modify setting in include/psa/crypto_config.h 
*/
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include <psa/crypto.h>

#include "mbedtls/mbedtls_config.h"
#include "mbedtls/ecp.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/error.h"
#include "mbedtls/rsa.h"
#include "mbedtls/x509.h"

#endif

#ifdef COMPACT25519
#include <c25519.h>
#include <edsign.h>
#include <compact_x25519.h>
#endif

#ifdef TINYCRYPT
#include <tinycrypt/aes.h>
#include <tinycrypt/ccm_mode.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/hmac.h>
#include <tinycrypt/ecc_dsa.h>
#include <tinycrypt/ecc_dh.h>
#endif

#ifdef MBEDTLS

static inline int mbedtls_ecp_decompress(const mbedtls_ecp_group *grp,
					 const unsigned char *input,
					 size_t ilen, unsigned char *output,
					 size_t *olen, size_t osize)
{
	int ret;
	size_t plen;
	mbedtls_mpi r;
	mbedtls_mpi x;
	mbedtls_mpi n;

	plen = mbedtls_mpi_size(&grp->P);

	*olen = 2 * plen + 1;

	if (osize < *olen)
		return (MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL);

	if (ilen != plen + 1)
		return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

	if (input[0] != 0x02 && input[0] != 0x03)
		return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

	// output will consist of 0x04|X|Y
	memcpy(output, input, ilen);
	output[0] = 0x04;

	mbedtls_mpi_init(&r);
	mbedtls_mpi_init(&x);
	mbedtls_mpi_init(&n);

	// x <= input
	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&x, input + 1, plen));

	// r = x^2
	MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&r, &x, &x));

	// r = x^2 + a
	if (grp->A.p == NULL) {
		// Special case where a is -3
		MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&r, &r, 3));
	} else {
		MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&r, &r, &grp->A));
	}

	// r = x^3 + ax
	MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&r, &r, &x));

	// r = x^3 + ax + b
	MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&r, &r, &grp->B));

	// Calculate square root of r over finite field P:
	//   r = sqrt(x^3 + ax + b) = (x^3 + ax + b) ^ ((P + 1) / 4) (mod P)

	// n = P + 1
	MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&n, &grp->P, 1));

	// n = (P + 1) / 4
	MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&n, 2));

	// r ^ ((P + 1) / 4) (mod p)
	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&r, &r, &n, &grp->P, NULL));

	// Select solution that has the correct "sign" (equals odd/even solution in finite group)
	if ((input[0] == 0x03) != mbedtls_mpi_get_bit(&r, 0)) {
		// r = p - r
		MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&r, &grp->P, &r));
	}

	// y => output
	ret = mbedtls_mpi_write_binary(&r, output + 1 + plen, plen);

cleanup:
	mbedtls_mpi_free(&r);
	mbedtls_mpi_free(&x);
	mbedtls_mpi_free(&n);

	return (ret);
}

#endif

enum err __attribute__((weak))
aead(enum aes_operation op, const uint8_t *in, const uint32_t in_len,
     const uint8_t *key, const uint32_t key_len, uint8_t *nonce,
     const uint32_t nonce_len, const uint8_t *aad, const uint32_t aad_len,
     uint8_t *out, const uint32_t out_len, uint8_t *tag, const uint32_t tag_len)
{
#ifdef TINYCRYPT
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
#ifdef MBEDTLS
	psa_key_id_t key_id = 0;

	TRY(psa_crypto_init());

	psa_algorithm_t alg =
		PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, (uint32_t)tag_len);

	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_usage_flags(&attr,
				PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_ENCRYPT);
	psa_set_key_algorithm(&attr, alg);
	psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&attr, (size_t)(key_len << 3));
	psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
	TRY(psa_import_key(&attr, key, key_len, &key_id));

	if (op == DECRYPT) {
		size_t out_len_re = 0;
		TRY(psa_aead_decrypt(key_id, alg, nonce, nonce_len, aad,
				     aad_len, in, in_len, out, out_len,
				     &out_len_re));
	} else {
		size_t out_len_re;
		TRY(psa_aead_encrypt(key_id, alg, nonce, nonce_len, aad,
				     aad_len, in, in_len, out,
				     (size_t)(in_len + tag_len), &out_len_re));
		memcpy(tag, out + out_len_re - tag_len, tag_len);
	}
	TRY(psa_destroy_key(key_id));

#endif
	return ok;
}

enum err __attribute__((weak))
sign(enum sign_alg alg, const uint8_t *sk, const uint32_t sk_len,
     const uint8_t *pk, const uint32_t pk_len, const uint8_t *msg,
     const uint32_t msg_len, uint8_t *out, uint32_t *out_len)
{
	if (alg == EdDSA) {
#if defined(COMPACT25519)
		edsign_sign(out, pk, sk, msg, msg_len);
		return ok;
#endif
	} else if (alg == ES256) {
#if defined(MBEDTLS)
		// (void)pk;
		// (void)pk_len;
		psa_algorithm_t alg;
		size_t bits;

		alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
		bits = PSA_BYTES_TO_BITS((size_t)sk_len);

		TRY(psa_crypto_init());

		psa_key_id_t key_id = PSA_KEY_HANDLE_INIT;
		psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

		psa_set_key_usage_flags(&attributes,
					PSA_KEY_USAGE_VERIFY_MESSAGE |
						PSA_KEY_USAGE_VERIFY_HASH |
						PSA_KEY_USAGE_SIGN_MESSAGE |
						PSA_KEY_USAGE_SIGN_HASH);
		psa_set_key_algorithm(&attributes, alg);
		psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(
						      PSA_ECC_FAMILY_SECP_R1));
		psa_set_key_bits(&attributes, bits);
		psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);

		TRY(psa_import_key(&attributes, sk, sk_len, &key_id));
		size_t signature_length;
		//out_len = 0;

		//PRINT_ARRAY("---sk", sk, sk_len);
		//PRINT_ARRAY("---pk", pk, pk_len);
		//PRINT_ARRAY("---msg", msg, msg_len);

		TRY(psa_sign_message(key_id, alg, msg, msg_len, out,
				     SIGNATURE_DEFAULT_SIZE,
				     &signature_length));
		if (signature_length != SIGNATURE_DEFAULT_SIZE) {
			return sign_failed;
		}
		return ok;

#endif
	}
	return unsupported_ecdh_curve;
}

enum err __attribute__((weak))
verify(enum sign_alg alg, const uint8_t *pk, const uint32_t pk_len,
       const uint8_t *msg, const uint32_t msg_len, const uint8_t *sgn,
       const uint32_t sgn_len, bool *result)
{
	if (alg == EdDSA) {
#ifdef COMPACT25519
		int verified = edsign_verify(sgn, pk, msg, msg_len);
		if (verified) {
			*result = true;
		} else {
			*result = false;
		}
#endif
	}
	if (alg == ES256) {
#ifdef MBEDTLS
		psa_status_t status;
		psa_algorithm_t alg;
		size_t bits;

		alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
		bits = PSA_BYTES_TO_BITS(P_256_PRIV_KEY_DEFAULT_SIZE);

		TRY(psa_crypto_init());

		psa_key_id_t key_id = PSA_KEY_HANDLE_INIT;
		psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

		psa_set_key_usage_flags(&attributes,
					PSA_KEY_USAGE_VERIFY_MESSAGE |
						PSA_KEY_USAGE_VERIFY_HASH);
		psa_set_key_algorithm(&attributes, alg);
		psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(
						      PSA_ECC_FAMILY_SECP_R1));
		psa_set_key_bits(&attributes, bits);
		TRY(psa_import_key(&attributes, pk, pk_len, &key_id));

		status = psa_verify_message(key_id, alg, msg, msg_len, sgn,
					    sgn_len);
		if (PSA_SUCCESS == status) {
			*result = true;
		} else {
			*result = false;
		}
		TRY(psa_destroy_key(key_id));
#endif
	}
	return ok;
}

enum err __attribute__((weak))
hkdf_extract(enum hash_alg alg, const uint8_t *salt, uint32_t salt_len,
	     uint8_t *ikm, uint32_t ikm_len, uint8_t *out)
{
	/*"Note that [RFC5869] specifies that if the salt is not provided, 
	it is set to a string of zeros.  For implementation purposes, not providing the salt is the same as setting the salt to the empty byte 
	string. OSCORE sets the salt default value to empty byte string, which 
	is converted to a string of zeroes (see Section 2.2 of [RFC5869])".*/

	/*all currently prosed suites use hmac-sha256*/
	if (alg == SHA_256) {
#ifdef TINYCRYPT
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
#ifdef MBEDTLS
		TRY(psa_crypto_init());
		psa_algorithm_t psa_alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
		psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
		psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
		psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH);
		psa_set_key_algorithm(&attr, psa_alg);
		psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
		psa_key_id_t key_id = 0;
		if (salt && salt_len) {
			TRY(psa_import_key(&attr, salt, salt_len, &key_id));
		} else {
			uint8_t zero_salt[32] = { 0 };
			TRY(psa_import_key(&attr, zero_salt, 32, &key_id));
		}
		size_t out_len;
		TRY(psa_mac_compute(key_id, psa_alg, ikm, ikm_len, out, 32,
				    &out_len));
		TRY(psa_destroy_key(key_id));

#endif
	}
	return ok;
}

enum err __attribute__((weak))
hkdf_expand(enum hash_alg alg, const uint8_t *prk, const uint32_t prk_len,
	    const uint8_t *info, const uint32_t info_len, uint8_t *out,
	    uint32_t out_len)
{
	if (alg == SHA_256) {
		/* "N = ceil(L/HashLen)" */
		uint32_t iterations = (out_len + 31) / 32;
		/* "L length of output keying material in octets (<= 255*HashLen)"*/
		if (iterations > 255) {
			return hkdf_fialed;
		}

#ifdef TINYCRYPT
		uint8_t t[32] = { 0 };
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
#ifdef MBEDTLS
		psa_status_t status;
		TRY(psa_crypto_init());

		psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
		psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
		psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH);
		psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
		psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
		psa_key_id_t key_id = 0;
		TRY(psa_import_key(&attr, prk, prk_len, &key_id));
		size_t combo_len = (size_t)(32 + info_len + 1);

		TRY(check_buffer_size(INFO_DEFAULT_SIZE, (uint32_t)combo_len));

		uint8_t combo[INFO_DEFAULT_SIZE];
		uint8_t tmp_out[32];
		memset(tmp_out, 0, 32);
		memcpy(combo + 32, info, info_len);
		size_t offset = 32;
		for (uint8_t i = 1; i <= iterations; i++) {
			memcpy(combo, tmp_out, 32);
			combo[combo_len - 1] = i;
			size_t tmp_out_len;
			status = psa_mac_compute(key_id,
						 PSA_ALG_HMAC(PSA_ALG_SHA_256),
						 combo + offset,
						 combo_len - offset, tmp_out,
						 32, &tmp_out_len);
			if (PSA_SUCCESS != status) {
				psa_destroy_key(key_id);
				key_id = 0;
				return unexpected_result_from_ext_lib;
			}
			offset = 0;
			uint8_t *dest = out + ((i - 1) << 5);
			if (out_len < (uint32_t)(i << 5)) {
				memcpy(dest, tmp_out, out_len & 31);
			} else {
				memcpy(dest, tmp_out, 32);
			}
		}
		if (key_id) {
			psa_destroy_key(key_id);
		}

#endif
	}
	return ok;
}

enum err __attribute__((weak))
hkdf_sha_256(struct byte_array *master_secret, struct byte_array *master_salt,
	     struct byte_array *info, struct byte_array *out)
{
	uint8_t prk[SHA_DEFAULT_SIZE];
	TRY(hkdf_extract(SHA_256, master_salt->ptr, master_salt->len,
			 master_secret->ptr, master_secret->len, prk));

	TRY(hkdf_expand(SHA_256, prk, sizeof(prk), info->ptr, info->len,
			out->ptr, out->len));
	return ok;
}

enum err __attribute__((weak))
shared_secret_derive(enum ecdh_alg alg, const uint8_t *sk,
		     const uint32_t sk_len, const uint8_t *pk,
		     const uint32_t pk_len, uint8_t *shared_secret)
{
	if (alg == X25519) {
#ifdef COMPACT25519
		uint8_t e[F25519_SIZE];
		f25519_copy(e, sk);
		c25519_prepare(e);
		c25519_smult(shared_secret, pk, e);
#endif
	}
	if (alg == P256) {
#ifdef MBEDTLS
		psa_key_id_t key_id;
		psa_algorithm_t alg;
		size_t bits;

		alg = PSA_ALG_ECDH;
		bits = PSA_BYTES_TO_BITS(sk_len);

		TRY(psa_crypto_init());

		psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
		psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
		psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);
		psa_set_key_algorithm(&attr, alg);
		psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(
						PSA_ECC_FAMILY_SECP_R1));
		psa_import_key(&attr, sk, (size_t)sk_len, &key_id);
		psa_key_type_t type = psa_get_key_type(&attr);
		size_t shared_size =
			PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(type, bits);

		size_t shared_secret_len = 0;
		PRINT_ARRAY("pk", pk, pk_len);

		size_t pk_decompressed_len;
		uint8_t pk_decompressed[P_256_PUB_KEY_DEFAULT_SIZE];

		mbedtls_pk_context ctx_verify;
		mbedtls_pk_init(&ctx_verify);
		mbedtls_pk_setup(&ctx_verify,
				 mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
		mbedtls_ecp_group_load(&mbedtls_pk_ec(ctx_verify)->grp,
				       MBEDTLS_ECP_DP_SECP256R1);
		mbedtls_ecp_decompress(&mbedtls_pk_ec(ctx_verify)->grp, pk,
				       pk_len, pk_decompressed,
				       &pk_decompressed_len,
				       sizeof(pk_decompressed));

		PRINT_ARRAY("pk_decompressed", pk_decompressed,
			    (uint32_t)pk_decompressed_len);

		TRY(psa_raw_key_agreement(PSA_ALG_ECDH, key_id, pk_decompressed,
					  pk_decompressed_len, shared_secret,
					  shared_size, &shared_secret_len));
		TRY(psa_destroy_key(key_id));

#endif
	}

	return ok;
}

enum err __attribute__((weak))
ephemeral_dh_key_gen(enum ecdh_alg alg, uint32_t seed, uint8_t *sk, uint8_t *pk)
{
	if (alg == X25519) {
#ifdef COMPACT25519
		uint8_t extended_seed[32];
#ifdef TINYCRYPT
		struct tc_sha256_state_struct s;
		TRY_EXPECT(tc_sha256_init(&s), 1);
		TRY_EXPECT(tc_sha256_update(&s, (uint8_t *)&seed, sizeof(seed)),
			   1);
		TRY_EXPECT(tc_sha256_final(extended_seed, &s), 1);
#endif
#ifdef MBEDTLS
		size_t length;
		TRY(psa_hash_compute(PSA_ALG_SHA_256, (uint8_t *)&seed,
				     sizeof(seed), sk, SHA_DEFAULT_SIZE,
				     &length));
		if (length != 32) {
			return sha_failed;
		}
#endif
		compact_x25519_keygen(sk, pk, extended_seed);
#endif
	} else if (alg == P256) {
#ifdef MBEDTLS
		psa_key_id_t key_id = PSA_KEY_HANDLE_INIT;
		psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
		psa_algorithm_t alg;
		size_t bits;
		uint8_t priv_key_size;
		uint8_t pub_key_size;

		alg = PSA_ALG_ECDH;
		priv_key_size = P_256_PRIV_KEY_DEFAULT_SIZE;
		pub_key_size = P_256_PUB_KEY_DEFAULT_SIZE;
		bits = PSA_BYTES_TO_BITS((size_t)priv_key_size);

		TRY(psa_crypto_init());

		psa_set_key_usage_flags(&attributes,
					PSA_KEY_USAGE_EXPORT |
						PSA_KEY_USAGE_DERIVE |
						PSA_KEY_USAGE_SIGN_MESSAGE |
						PSA_KEY_USAGE_SIGN_HASH);
		psa_set_key_algorithm(&attributes, alg);
		psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(
						      PSA_ECC_FAMILY_SECP_R1));
		psa_set_key_bits(&attributes, bits);

		TRY(psa_generate_key(&attributes, &key_id));

		size_t key_len = 0;
		size_t public_key_len = 0;

		TRY(psa_export_key(key_id, sk, priv_key_size, &key_len));
		TRY(psa_export_public_key(key_id, pk, pub_key_size,
					  &public_key_len));
		TRY(psa_destroy_key(key_id));

		return ok;
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
#ifdef TINYCRYPT
		struct tc_sha256_state_struct s;
		TRY_EXPECT(tc_sha256_init(&s), 1);
		TRY_EXPECT(tc_sha256_update(&s, in, in_len), 1);
		TRY_EXPECT(tc_sha256_final(out, &s), 1);
#endif
#ifdef MBEDTLS
		size_t length;
		TRY(psa_hash_compute(PSA_ALG_SHA_256, in, in_len, out,
				     SHA_DEFAULT_SIZE, &length));
		if (length != SHA_DEFAULT_SIZE) {
			return sha_failed;
		}
#endif
	}

	return ok;
}
