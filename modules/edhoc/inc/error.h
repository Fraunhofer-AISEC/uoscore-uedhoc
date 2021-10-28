/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef ERROR_H
#define ERROR_H
#include "print_util.h"

/**
 * Error type used throughout the whole edhoc implementation.
 *
 * Every function that might error returns an edhoc_error and writes its
 * return value into an out-parameter.
 */
enum edhoc_error {
	edhoc_no_error = 0,
	error_message_received = 1,
	error_message_sent = 2,

	hkdf_fialed = 3,
	aead_failed = 4,
	sign_failed = 5,
	sha_failed = 6,
	dh_failed = 7,
	unsupported_cipher_suite = 8,
	unsupported_ecdh_curve = 9,
	unsupported_aead_algorithm = 10,
	unsupported_hash_algorithm = 11,

	responder_authentication_failed = 12, /*todo remove this eventually*/
	signature_authentication_failed = 13,
	mac_authentication_failed = 14,
	aead_authentication_failed = 15,
	certificate_authentication_failed = 16,

	credential_not_found = 17,
	no_such_ca = 18,

	message_buff_to_small = 19,
	dest_buffer_to_small = 20,

	cbor_encoding_error = 21,
	cbor_decoding_error = 22,
	suites_i_list_to_long = 23,
	unexpected_result_from_ext_lib = 24,
};

/*This macro checks if a function returns an error and if so it propages 
	the error to the caller function*/
#define TRY(x)                                                                 \
	do {                                                                   \
		int retval = (x);                                              \
		if (retval != 0) {                                             \
			PRINTF("Runtime error: %s error code %d at %s:%d\n",   \
			       #x, retval, __FILE__, __LINE__);                \
			return retval;                                         \
		}                                                              \
	} while (0)

/*This macro checks if a function returns an expected result that may be non 
	zero and if so it returns unexpected_result_from_ext_lib. This macro 
	should be used to check the return value of functions belonging to 
	external libraries that return somthing else but not zero on success*/
#define TRY_EXPECT(x, expected_result)                                         \
	do {                                                                   \
		int retval = (x);                                              \
		if (retval != expected_result) {                               \
			PRINTF("Runtime error: %s error code %d at %s:%d\n",   \
			       #x, retval, __FILE__, __LINE__);                \
			return unexpected_result_from_ext_lib;                 \
		}                                                              \
	} while (0)

#endif
