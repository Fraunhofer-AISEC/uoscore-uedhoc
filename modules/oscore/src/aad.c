/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include "../inc/aad.h"

#include <cbor.h>

#include "../inc/option.h"
#include "../inc/print_util.h"

OscoreError aad_length(struct o_coap_option *options, uint16_t opt_num,
		       enum AEAD_algorithm aead_alg, struct byte_array *kid,
		       struct byte_array *piv, uint32_t *out)
{
	CborEncoder enc;
	cbor_encoder_init(&enc, NULL, 0, 0);
	CborEncoder array_enc;
	CborEncoder array_enc2;
	OscoreError r;

	cbor_encoder_create_array(&enc, &array_enc, 5);
	/* oscore_version */
	cbor_encode_uint(&array_enc, 1);
	/* algorithms */
	cbor_encoder_create_array(&array_enc, &array_enc2, 1);
	/* alg_aead */
	cbor_encode_int(&array_enc2, aead_alg);
	cbor_encoder_close_container(&array_enc, &array_enc2);
	/* request_kid */
	cbor_encode_byte_string(&array_enc, kid->ptr, kid->len);
	/* request_piv */
	cbor_encode_byte_string(&array_enc, piv->ptr, piv->len);
	/* options */
	uint32_t encoded_opt_i_len =
		encoded_option_len(options, opt_num, CLASS_I);
	uint8_t encoded_opt_i_bytes[encoded_opt_i_len];
	struct byte_array opts_i = {
		.len = encoded_opt_i_len,
		.ptr = encoded_opt_i_bytes,
	};
	r = encode_options(options, opt_num, CLASS_I, &opts_i.ptr[0],
			   encoded_opt_i_len);
	if (r != OscoreNoError)
		return r;
	cbor_encode_byte_string(&array_enc, opts_i.ptr, opts_i.len);
	/* finish up */
	cbor_encoder_close_container(&enc, &array_enc);

	*out = cbor_encoder_get_extra_bytes_needed(&enc);
	return OscoreNoError;
}

OscoreError create_aad(struct o_coap_option *options, uint16_t opt_num,
		       enum AEAD_algorithm aead_alg, struct byte_array *kid,
		       struct byte_array *piv, struct byte_array *out)
{
	CborEncoder enc;
	cbor_encoder_init(&enc, out->ptr, out->len, 0);
	CborEncoder array_enc;
	CborEncoder array_enc2;
	OscoreError r;

	cbor_encoder_create_array(&enc, &array_enc, 5);
	/* oscore_version */
	cbor_encode_uint(&array_enc, 1);
	/* algorithms */
	cbor_encoder_create_array(&array_enc, &array_enc2, 1);
	/* alg_aead */
	cbor_encode_int(&array_enc2, aead_alg);
	cbor_encoder_close_container(&array_enc, &array_enc2);
	/* request_kid */
	cbor_encode_byte_string(&array_enc, kid->ptr, kid->len);
	/* request_piv */
	cbor_encode_byte_string(&array_enc, piv->ptr, piv->len);

	/* options */
	uint32_t encoded_opt_i_len =
		encoded_option_len(options, opt_num, CLASS_I);
	uint8_t encoded_opt_i_bytes[encoded_opt_i_len];
	struct byte_array opts_i = {
		.len = encoded_opt_i_len,
		.ptr = encoded_opt_i_bytes,
	};
	r = encode_options(options, opt_num, CLASS_I, &opts_i.ptr[0],
			   encoded_opt_i_len);
	if (r != OscoreNoError)
		return r;

	cbor_encode_byte_string(&array_enc, opts_i.ptr, opts_i.len);
	/* finish up */
	cbor_encoder_close_container(&enc, &array_enc);

	PRINT_ARRAY("AAD", out->ptr, out->len);
	return OscoreNoError;
}
