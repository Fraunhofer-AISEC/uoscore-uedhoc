/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include <cbor.h>
#include <stdint.h>

#include "../edhoc.h"
#include "../inc/cbor_decoder.h"
#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/print_util.h"
#include "../inc/retrieve_cred.h"
#include "../inc/signature_or_mac_msg.h"
#include "../inc/plaintext.h"
#include "../cbor/decode_id_cred_x.h"

enum edhoc_error id_cred2kid(const uint8_t *id_cred, uint8_t id_cred_len,
		       uint8_t *_kid, uint32_t *kid_len)
{
	// edhoc_error r;
	// bool success;
	// struct id_cred_x_map map;
	// size_t decode_len = 0;
	// success = cbor_decode_id_cred_x_map(id_cred, id_cred_len, &map,
	// 				    &decode_len);
	// if (!success) {
	// 	return cbor_decoding_error;
	// }

	// if (map._id_cred_x_map_kid_present != 0) {
	// 	if (map._id_cred_x_map_kid._id_cred_x_map_kid.len == 1) {
	// 		*_kid = *map._id_cred_x_map_kid._id_cred_x_map_kid
	// 				 .value -
	// 			24;
	// 		//todo insert int cbor encoding-----------------------------

	// 		//----------------------------------------------------------
	// 	} else {
	// 		r = _memcpy_s(
	// 			_kid, *kid_len,
	// 			map._id_cred_x_map_kid._id_cred_x_map_kid.value,
	// 			map._id_cred_x_map_kid._id_cred_x_map_kid.len);
	//		if (r != edhoc_no_error) {
	// 			return r;
	// 		}
	// 	}
	// 	*kid_len = map._id_cred_x_map_kid._id_cred_x_map_kid.len;
	// } else {
	// 	*kid_len = 0;
	// }

	CborParser parser;
	CborValue value;
	CborError err;
	uint8_t *next_temp_ptr;
	uint8_t *temp_ptr;
	uint32_t temp_len;
	CborType ignore;
	enum edhoc_error r;

	/* Initialization */
	err = cbor_parser_init(id_cred, id_cred_len, 0, &parser, &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;

	/*the first element in plaintext is a ID_CRED_x, which starts with a map */
	/*we move to the label of the map*/
	temp_ptr = id_cred + 1;
	temp_len = id_cred_len - 1;

	int map_label;
	uint64_t map_label_len;

	r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, &map_label,
			 &map_label_len, &ignore);
	if (r != edhoc_no_error)
		return r;
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;

	if (map_label == kid) {
		uint8_t kid_str[KID_DEFAULT_SIZE];
		uint64_t kid_str_len = sizeof(kid_str);
		r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, &kid_str,
				 &kid_str_len, &ignore);
		if (r != edhoc_no_error)
			return r;

		if (kid_str_len == 1) {
			int64_t t = kid_str[0] - 24;
			CborEncoder enc;
			CborError r;
			cbor_encoder_init(&enc, _kid, *kid_len, 0);
			r = cbor_encode_int(&enc, t);
			if (r == CborErrorOutOfMemory)
				return CborEncodingBufferToSmall;
			*kid_len = 1;
		}

	} else {
		*kid_len = 0;
	}

	return edhoc_no_error;
}

enum edhoc_error plaintext_encode(const uint8_t *id_cred, uint8_t id_cred_len,
			    const uint8_t *sgn_or_mac, uint8_t sgn_or_mac_len,
			    const uint8_t *ad, uint16_t ad_len,
			    uint8_t *plaintext, uint16_t *plaintext_len)
{
	enum edhoc_error r;

	uint16_t l, enc_sgn_or_mac_len = sgn_or_mac_len + 2;
	uint8_t kid_buf[KID_DEFAULT_SIZE];
	uint32_t kid_len = sizeof(kid_buf);
	id_cred2kid(id_cred, id_cred_len, kid_buf, &kid_len);
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
