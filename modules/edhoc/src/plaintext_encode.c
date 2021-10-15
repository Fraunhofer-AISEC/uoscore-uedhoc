/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include <stdint.h>

#include "../edhoc.h"
#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/print_util.h"
#include "../inc/retrieve_cred.h"
#include "../inc/signature_or_mac_msg.h"
#include "../inc/plaintext.h"
#include "../cbor/decode_id_cred_x.h"
#include "../cbor/encode_int_type.h"

enum edhoc_error id_cred2kid(const uint8_t *id_cred, uint8_t id_cred_len,
			     uint8_t *_kid, uint32_t *kid_len)
{
	//enum edhoc_error r;
	bool ok;
	struct id_cred_x_map map;
	size_t payload_len_out;
	size_t decode_len = 0;
	ok = cbor_decode_id_cred_x_map(id_cred, id_cred_len, &map, &decode_len);
	if (!ok) {
		return cbor_decoding_error;
	}

	if (map._id_cred_x_map_kid_present != 0) {
		// if (map._id_cred_x_map_kid._id_cred_x_map_kid.len == 1) {
		// 	int32_t i =
		// 		*map._id_cred_x_map_kid._id_cred_x_map_kid.value;
		// 	ok = cbor_encode_int_type_i(_kid, *kid_len, &i,
		// 				    &payload_len_out);
		// 	if (!ok) {
		// 		return cbor_encoding_error;
		// 	}
		// 	*kid_len = payload_len_out;
		// } else {
		// 	r = _memcpy_s(
		// 		_kid, *kid_len,
		// 		map._id_cred_x_map_kid._id_cred_x_map_kid.value,
		// 		map._id_cred_x_map_kid._id_cred_x_map_kid.len);
		// 	if (r != edhoc_no_error) {
		// 		return r;
		// 	}
		// 	*kid_len =
		// 		map._id_cred_x_map_kid._id_cred_x_map_kid.len;
		// }
		//*_kid = map._id_cred_x_map_kid._id_cred_x_map_kid;
		ok = cbor_encode_int_type_i(
			_kid, *kid_len,
			&map._id_cred_x_map_kid._id_cred_x_map_kid,
			&payload_len_out);
		if (!ok) {
			return cbor_encoding_error;
		}
		*kid_len = payload_len_out;
	} else {
		*kid_len = 0;
	}

	return edhoc_no_error;
}

enum edhoc_error plaintext_encode(const uint8_t *id_cred, uint8_t id_cred_len,
				  const uint8_t *sgn_or_mac,
				  uint8_t sgn_or_mac_len, const uint8_t *ad,
				  uint16_t ad_len, uint8_t *plaintext,
				  uint16_t *plaintext_len)
{
	enum edhoc_error r;

	uint16_t l, enc_sgn_or_mac_len = sgn_or_mac_len + 2;
	uint8_t kid_buf[KID_DEFAULT_SIZE];
	uint32_t kid_len = sizeof(kid_buf);
	r = id_cred2kid(id_cred, id_cred_len, kid_buf, &kid_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("kid", kid_buf, kid_len);
	if (kid_len != 0) {
		/*id cred contains a kid*/
		r = _memcpy_s(plaintext, *plaintext_len, kid_buf, kid_len);
		if (r != edhoc_no_error)
			return r;
		l = kid_len;
	} else {
		r = _memcpy_s(plaintext, *plaintext_len, id_cred, id_cred_len);
		if (r != edhoc_no_error)
			return r;
		l = id_cred_len;
	}

	r = encode_byte_string(sgn_or_mac, sgn_or_mac_len, plaintext + l,
			       &enc_sgn_or_mac_len);
	if (r != edhoc_no_error)
		return r;

	r = _memcpy_s(plaintext + l + enc_sgn_or_mac_len,
		      *plaintext_len - l - enc_sgn_or_mac_len, ad, ad_len);
	if (r != edhoc_no_error)
		return r;

	*plaintext_len = l + enc_sgn_or_mac_len + ad_len;

	return edhoc_no_error;
}
