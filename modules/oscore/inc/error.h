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
 * Error type used throughout the whole oscore implementation.
 *
 * Every function that might error returns an oscore_error and writes its
 * return value into an out-parameter.
 */
enum oscore_error {
	oscore_no_error = 0,
	oscore_tiny_crypt_error = 1,
	oscore_unknown_hkdf = 2,
	oscore_out_too_long = 3,
	oscore_invalid_algorithm_aead = 4,
	oscore_invalid_algorithm_hkdf = 5,
	oscore_kid_recipent_id_mismatch = 6,
	oscore_authentication_error = 7,
	oscore_valuelen_to_long_error = 8,
	oscore_inpkt_invalid_tkl = 9,
	oscore_inpkt_invalid_option_delta = 10,
	oscore_inpkt_invalid_optionlen = 11,
	oscore_inpkt_invalid_piv = 12,
	//oscore_info_to_long = 13,
	dest_buffer_to_small = 14,
	delta_extra_byte_error = 15,
	len_extra_byte_error = 16,
	cbor_encoding_error = 17,
	not_valid_input_packet = 18,
	replayed_packed_received = 19,
};

#endif
