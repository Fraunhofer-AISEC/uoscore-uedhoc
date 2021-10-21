/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/suites.h"

#include "../inc/error.h"

enum edhoc_error get_suite(enum suite_label label, struct suite *suite)
{
	switch (label) {
	case SUITE_0:
		suite->suite_label = SUITE_0;
		suite->edhoc_aead = AES_CCM_16_64_128;
		suite->edhoc_hash = SHA_256;
		suite->edhoc_ecdh_curve = X25519;
		suite->edhoc_sign_alg = EdDSA;
		suite->edhoc_sign_curve = Ed25519_SIGN;
		suite->app_aead = AES_CCM_16_64_128;
		suite->app_hash = SHA_256;
		break;
	case SUITE_1:
		suite->suite_label = SUITE_1;
		suite->edhoc_aead = AES_CCM_16_128_128;
		suite->edhoc_hash = SHA_256;
		suite->edhoc_ecdh_curve = X25519;
		suite->edhoc_sign_alg = EdDSA;
		suite->edhoc_sign_curve = Ed25519_SIGN;
		suite->app_aead = AES_CCM_16_64_128;
		suite->app_hash = SHA_256;
		break;
	case SUITE_2:
		suite->suite_label = SUITE_2;
		suite->edhoc_aead = AES_CCM_16_64_128;
		suite->edhoc_hash = SHA_256;
		suite->edhoc_ecdh_curve = 1;
		suite->edhoc_sign_alg = ES256;
		suite->edhoc_sign_curve = 1;
		suite->app_aead = AES_CCM_16_64_128;
		suite->app_hash = SHA_256;
		break;
	case SUITE_3:
		suite->suite_label = SUITE_3;
		suite->edhoc_aead = AES_CCM_16_128_128;
		suite->edhoc_hash = SHA_256;
		suite->edhoc_ecdh_curve = 1;
		suite->edhoc_sign_alg = ES256;
		suite->edhoc_sign_curve = 1;
		suite->app_aead = AES_CCM_16_64_128;
		suite->app_hash = SHA_256;
		break;
	default:
		return unsupported_cipher_suite;
		break;
	}
	return edhoc_no_error;
}

uint32_t get_hash_len(enum hash_alg alg)
{
	switch (alg) {
	case SHA_256:
		return 32;
		break;
	}
	return 0;
}

uint32_t get_mac_len(enum aead_alg alg)
{
	switch (alg) {
	case AES_CCM_16_128_128:
		return 16;
		break;
	case AES_CCM_16_64_128:
		return 8;
		break;
	}
	return 0;
}

uint32_t get_aead_key_len(enum aead_alg alg)
{
	switch (alg) {
	case AES_CCM_16_128_128:
	case AES_CCM_16_64_128:
		return 16;
		break;
	}
	return 0;
}

uint32_t get_aead_iv_len(enum aead_alg alg)
{
	switch (alg) {
	case AES_CCM_16_128_128:
	case AES_CCM_16_64_128:
		return 13;
		break;
	}
	return 0;
}

uint32_t get_signature_len(enum sign_alg_curve alg)
{
	switch (alg) {
	case Ed25519_SIGN:
	case P_256_SIGN:
		return 64;
		break;
	}
	return 0;
}