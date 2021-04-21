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
#include "../cbor/decode_plaintext.h"
#include "../cbor/encode_id_cred_x.h"



/**
 * @brief 	Encodes IS_CRED_x as a CBOR map
 * @param	label the map label
 * @param	algo the hash algorithm used in x5t. This parameter kan take any 
 * 			othe value when xchain or kid are used
 * @param	id the actual credential identifier
 * @param	id_len length of id
 * @param	
 */
static EdhocError id_cred_x_encode(enum id_cred_x_label label, int algo,
				   const void *id, uint64_t id_len,
				   uint8_t *id_cred_x, uint64_t *id_cred_x_len)
{
	bool success;
	struct id_cred_x_map map;
	size_t payload_len_out;

	map._id_cred_x_map_kid_present = 0;
	map._id_cred_x_map_x5chain_present = 0;
	map._id_cred_x_map_x5t_present = 0;

	switch (label) {
	case kid:
		map._id_cred_x_map_kid_present = true;
		map._id_cred_x_map_kid._id_cred_x_map_kid.value = id;
		map._id_cred_x_map_kid._id_cred_x_map_kid.len = id_len;
		break;
	case x5chain:
		map._id_cred_x_map_x5chain_present = true;
		map._id_cred_x_map_x5chain._id_cred_x_map_x5chain.value = id;
		map._id_cred_x_map_x5chain._id_cred_x_map_x5chain.len = id_len;
		break;
	case x5t:
		map._id_cred_x_map_x5t_present = true;
		map._id_cred_x_map_x5t._id_cred_x_map_x5t_int = algo;
		map._id_cred_x_map_x5t._id_cred_x_map_x5t_bstr.value = id;
		map._id_cred_x_map_x5t._id_cred_x_map_x5t_bstr.len = id_len;
		break;
	default:
		break;
	}

	success = cbor_encode_id_cred_x_map(id_cred_x, *id_cred_x_len, &map,
					    &payload_len_out);

	if (!success) {
		return cbor_encoding_error;
	}
	*id_cred_x_len = payload_len_out;

	return EdhocNoError;
}

EdhocError plaintext_split(uint8_t *ptxt, const uint16_t ptxt_len,
			   uint8_t *id_cred_x, uint64_t *id_cred_x_len,
			   uint8_t *sign_or_mac, uint64_t *sign_or_mac_len,
			   uint8_t *ad, uint64_t *ad_len)
{
	bool success;
	size_t decode_len = 0;
	EdhocError r;
	struct plaintext p;

	success = cbor_decode_plaintext(ptxt, ptxt_len, &p, &decode_len);
	if (!success) {
		return cbor_decoding_error;
	}

	/*ID_CRED_x*/
	if (p._plaintext_ID_CRED_x_choice == _plaintext_ID_CRED_x__map) {
		if (p._plaintext_ID_CRED_x__map._map_x5chain_present) {
			PRINT_MSG(
				"ID_CRED of the other party has label x5chain\n");
			r = id_cred_x_encode(
				x5chain, 0,
				p._plaintext_ID_CRED_x__map._map_x5chain
					._map_x5chain.value,
				p._plaintext_ID_CRED_x__map._map_x5chain
					._map_x5chain.len,
				id_cred_x, id_cred_x_len);

			if (r != EdhocNoError) {
				return r;
			}
		}
		if (p._plaintext_ID_CRED_x__map._map_x5t_present) {
			PRINT_MSG("ID_CRED of the other party has label x5t\n");
			r = id_cred_x_encode(
				x5t,
				p._plaintext_ID_CRED_x__map._map_x5t
					._map_x5t_int,
				p._plaintext_ID_CRED_x__map._map_x5t
					._map_x5t_bstr.value,
				p._plaintext_ID_CRED_x__map._map_x5t
					._map_x5t_bstr.len,
				id_cred_x, id_cred_x_len);

			if (r != EdhocNoError) {
				return r;
			}
		}
	} else {
		/*Note that if ID_CRED_x contains a single 'kid' parameter,
            i.e., ID_CRED_R = { 4 : kid_x }, only the byte string kid_x
            is conveyed in the plaintext encoded as a bstr_identifier,
            see Section 3.3.4 and Section 5.1.*/
		if (p._plaintext_ID_CRED_x_choice ==
		    _plaintext_ID_CRED_x_bstr) {
			r = id_cred_x_encode(kid, p._plaintext_ID_CRED_x_int,
					     p._plaintext_ID_CRED_x_bstr.value,
					     p._plaintext_ID_CRED_x_bstr.len,
					     id_cred_x, id_cred_x_len);
			if (r != EdhocNoError) {
				return r;
			}
		} else {
			int _kid = p._plaintext_ID_CRED_x_int + 24;
			r = id_cred_x_encode(kid, 0, &_kid, 1, id_cred_x,
					     id_cred_x_len);
			if (r != EdhocNoError) {
				return r;
			}
		}
	}
	r = _memcpy_s(sign_or_mac, *sign_or_mac_len,
		      p._plaintext_SGN_or_MAC_x.value,
		      p._plaintext_SGN_or_MAC_x.len);
	*sign_or_mac_len = p._plaintext_SGN_or_MAC_x.len;
	if (r != EdhocNoError) {
		return r;
	}

	if (p._plaintext_AD_x_present == true) {
		r = _memcpy_s(ad, *ad_len, p._plaintext_AD_x.value,
			      p._plaintext_AD_x.len);
		*ad_len = p._plaintext_AD_x.len;
		if (r != EdhocNoError) {
			return r;
		}
	} else {
		*ad_len = 0;
	}

	return EdhocNoError;
}

EdhocError id_cred2kid(uint8_t *id_cred, uint8_t id_cred_len, uint8_t *_kid,
		       uint32_t *kid_len)
{
	CborParser parser;
	CborValue value;
	CborError err;
	uint8_t *next_temp_ptr;
	uint8_t *temp_ptr;
	uint32_t temp_len;
	CborType ignore;
	EdhocError r;

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
	if (r != EdhocNoError)
		return r;
	temp_len -= (next_temp_ptr - temp_ptr);
	temp_ptr = next_temp_ptr;

	if (map_label == kid) {
		uint8_t kid_str[KID_DEFAULT_SIZE];
		uint64_t kid_str_len = sizeof(kid_str);
		r = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len, &kid_str,
				 &kid_str_len, &ignore);
		if (r != EdhocNoError)
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
	return EdhocNoError;
}

EdhocError plaintext_encode(uint8_t *id_cred, uint8_t id_cred_len,
			    const uint8_t *sgn_or_mac, uint8_t sgn_or_mac_len,
			    const uint8_t *ad, uint16_t ad_len,
			    uint8_t *plaintext, uint16_t *plaintext_len)
{
	EdhocError r;

	uint16_t l, enc_sgn_or_mac_len = sgn_or_mac_len + 2;
	uint8_t kid_buf[KID_DEFAULT_SIZE];
	uint32_t kid_len = sizeof(kid_buf);
	id_cred2kid(id_cred, id_cred_len, kid_buf, &kid_len);

	if (kid_len != 0) {
		/*id cred contains a kid*/
		r = _memcpy_s(plaintext, *plaintext_len, kid_buf, kid_len);
		if (r != EdhocNoError)
			return r;
		l = kid_len;
	} else {
		r = _memcpy_s(plaintext, *plaintext_len, id_cred, id_cred_len);
		if (r != EdhocNoError)
			return r;
		l = id_cred_len;
	}

	r = encode_byte_string(sgn_or_mac, sgn_or_mac_len, plaintext + l,
			       &enc_sgn_or_mac_len);
	if (r != EdhocNoError)
		return r;

	r = _memcpy_s(plaintext + l + enc_sgn_or_mac_len,
		      *plaintext_len - l - enc_sgn_or_mac_len, ad, ad_len);
	if (r != EdhocNoError)
		return r;

	*plaintext_len = l + enc_sgn_or_mac_len + ad_len;

	return EdhocNoError;
}
