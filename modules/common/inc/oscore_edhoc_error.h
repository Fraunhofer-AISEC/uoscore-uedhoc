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

#define RED "\x1B[31m"
#define RESET "\033[0m"

/**
 * Error type used throughout the whole edhoc implementation.
 *
 * Every function that might error returns an err and writes its
 * return value into an out-parameter.
 */
enum err {
	/*common errors*/
	ok = 0,
	buffer_to_small = 1,
	hkdf_fialed = 2,
	aead_failed = 3,

	/*EDHOC specifc errors*/
	error_message_received = 101,
	error_message_sent = 102,

	sign_failed = 103,
	sha_failed = 104,
	dh_failed = 105,
	unsupported_cipher_suite = 106,
	unsupported_ecdh_curve = 107,
	unsupported_aead_algorithm = 108,
	unsupported_hash_algorithm = 109,
	unsupported_signature_algorithm = 110,

	responder_authentication_failed = 111, /*todo remove this eventually*/
	signature_authentication_failed = 112,
	mac_authentication_failed = 113,
	aead_authentication_failed = 114,
	certificate_authentication_failed = 115,

	credential_not_found = 116,
	no_such_ca = 117,

	message_buff_to_small = 118,

	cbor_encoding_error = 119,
	cbor_decoding_error = 120,
	suites_i_list_to_long = 121,
	unexpected_result_from_ext_lib = 122,

	/*OSCORE specific errors*/
	oscore_tiny_crypt_error = 201,
	oscore_unknown_hkdf = 202,
	oscore_out_too_long = 203,
	oscore_invalid_algorithm_aead = 204,
	oscore_invalid_algorithm_hkdf = 205,
	oscore_kid_recipent_id_mismatch = 206,
	oscore_authentication_error = 207,
	oscore_valuelen_to_long_error = 208,
	oscore_inpkt_invalid_tkl = 209,
	oscore_inpkt_invalid_option_delta = 210,
	oscore_inpkt_invalid_optionlen = 211,
	oscore_inpkt_invalid_piv = 212,
	delta_extra_byte_error = 215,
	len_extra_byte_error = 216,
	not_valid_input_packet = 218,
	replayed_packed_received = 219,

};


/*This macro checks if a function returns an error and if so it propages 
	the error to the caller function*/
#define TRY(x)                                                                     \
	do {                                                                       \
		int retval = (x);                                                  \
		if (retval != 0) {                                                 \
			PRINTF(RED                                                 \
			       "Runtime error: %s error code %d at %s:%d\n" RESET, \
			       #x, retval, __FILE__, __LINE__);                    \
			return retval;                                             \
		}                                                                  \
	} while (0)

/*This macro checks if a function returns an expected result that may be non 
	zero and if so it returns unexpected_result_from_ext_lib. This macro 
	should be used to check the return value of functions belonging to 
	external libraries that return somthing else but not zero on success*/
#define TRY_EXPECT(x, expected_result)                                             \
	do {                                                                       \
		int retval = (x);                                                  \
		if (retval != expected_result) {                                   \
			PRINTF(RED                                                 \
			       "Runtime error: %s error code %d at %s:%d\n" RESET, \
			       #x, retval, __FILE__, __LINE__);                    \
			return unexpected_result_from_ext_lib;                     \
		}                                                                  \
	} while (0)

#endif
