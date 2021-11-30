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
	ok = 0,

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
	unsupported_signature_algorithm = 12,

	responder_authentication_failed = 13, /*todo remove this eventually*/
	signature_authentication_failed = 14,
	mac_authentication_failed = 15,
	aead_authentication_failed = 16,
	certificate_authentication_failed = 17,

	credential_not_found = 18,
	no_such_ca = 19,

	message_buff_to_small = 20,
	dest_buffer_to_small = 21,

	cbor_encoding_error = 22,
	cbor_decoding_error = 23,
	suites_i_list_to_long = 24,
	unexpected_result_from_ext_lib = 25,

	/*OSCORE specific errors*/
	oscore_tiny_crypt_error = 101,
	oscore_unknown_hkdf = 102,
	oscore_out_too_long = 103,
	oscore_invalid_algorithm_aead = 104,
	oscore_invalid_algorithm_hkdf = 105,
	oscore_kid_recipent_id_mismatch = 106,
	oscore_authentication_error = 107,
	oscore_valuelen_to_long_error = 108,
	oscore_inpkt_invalid_tkl = 109,
	oscore_inpkt_invalid_option_delta = 110,
	oscore_inpkt_invalid_optionlen = 111,
	oscore_inpkt_invalid_piv = 112,
	//oscore_info_to_long = 13,
	//dest_buffer_to_small = 14,
	delta_extra_byte_error = 115,
	len_extra_byte_error = 116,
	//cbor_encoding_error = 117,
	not_valid_input_packet = 118,
	replayed_packed_received = 119,

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
