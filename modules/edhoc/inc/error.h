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
	unsupported_cipher_suite = 5,
	unsupported_ecdh_curve = 6,
	unsupported_aead_algorithm = 7,

	responder_authentication_failed = 8,
	aead_authentication_failed = 9,
	certificate_authentication_failed = 10,

	credential_not_found = 11,
	no_such_ca = 12,

	message_buff_to_small = 13,
	dest_buffer_to_small = 14,

	cbor_encoding_error = 15,
	cbor_decoding_error = 16,
	suites_i_list_to_long = 17,

};

#endif
