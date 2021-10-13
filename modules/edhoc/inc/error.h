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

	responder_authentication_failed = 12,
	aead_authentication_failed = 13,
	certificate_authentication_failed = 14,

	credential_not_found = 15,
	no_such_ca = 16,

	message_buff_to_small = 17,
	dest_buffer_to_small = 18,

	cbor_encoding_error = 19,
	cbor_decoding_error = 20,
	suites_i_list_to_long = 21,

};

#endif
