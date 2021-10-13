/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef SUITES_H
#define SUITES_H

#include <stdint.h>

#include "error.h"

/*see https://www.iana.org/assignments/cose/cose.xhtml#algorithms for algorithm number reference*/

enum suite_label {
	SUITE_0 = 0,
	SUITE_1 = 1,
	SUITE_2 = 2,
	SUITE_3 = 3,
};

enum aead_alg {
	AES_CCM_16_64_128 = 10,
	AES_CCM_16_128_128 = 30,
};

enum hash_alg { SHA_256 = -16 };

enum ecdh_curve {
	P_256_ECDH = 1,
	X25519 = 4,
};

enum sign_alg {
	ES256 = -7,
	EdDSA = -8,
};

enum sign_alg_curve {
	P_256_SIGN = 1,
	Ed25519_SIGN = 6,
};

struct suite {
	enum suite_label suite_label;
	enum aead_alg edhoc_aead;
	enum hash_alg edhoc_hash;
	enum ecdh_curve edhoc_ecdh_curve;
	enum sign_alg edhoc_sign_alg;
	enum sign_alg_curve edhoc_sign_curve;
	enum aead_alg app_aead;
	enum hash_alg app_hash;
};

/**
 * @brief   retrieves the algorithms coresponding to a given suite label
 * @param   label the suite label 
 * @param   suite the algorithms coresponding to label
 */
enum edhoc_error get_suite(enum suite_label label, struct suite *suite);

/**
 * 
 * 
 */
enum edhoc_error get_hash_len(enum hash_alg alg, uint32_t *len);

/**
 * 
 * 
 */
enum edhoc_error get_mac_len(enum aead_alg alg, uint32_t *len);
#endif