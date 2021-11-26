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
#include "../inc/option.h"
#include "../inc/print_util.h"
#include "../cbor/aad_array.h"
#include "../../common/inc/oscore_edhoc_error.h"

enum err create_aad(struct o_coap_option *options, uint16_t opt_num,
		    enum AEAD_algorithm aead_alg, struct byte_array *kid,
		    struct byte_array *piv, struct byte_array *out)
{

	struct aad_array aad_array;

	aad_array._aad_array_oscore_version = 1;
	aad_array._aad_array_algorithms_alg_aead_choice =
		_aad_array_algorithms_alg_aead_int;
	aad_array._aad_array_algorithms_alg_aead_int = aead_alg;
	aad_array._aad_array_request_kid.value = kid->ptr;
	aad_array._aad_array_request_kid.len = kid->len;
	aad_array._aad_array_request_piv.value = piv->ptr;
	aad_array._aad_array_request_piv.len = piv->len;

	/* options */
	uint32_t encoded_opt_i_len =
		encoded_option_len(options, opt_num, CLASS_I);
	uint8_t encoded_opt_i_bytes[encoded_opt_i_len];
	struct byte_array opts_i = {
		.len = encoded_opt_i_len,
		.ptr = encoded_opt_i_bytes,
	};
	TRY(encode_options(options, opt_num, CLASS_I, &opts_i.ptr[0],
			   encoded_opt_i_len));

	aad_array._aad_array_options.len = opts_i.len;
	aad_array._aad_array_options.value = opts_i.ptr;

	uint32_t payload_len_out;
	TRY_EXPECT(cbor_encode_aad_array(out->ptr, out->len, &aad_array,
					 &payload_len_out),
		   true);

	out->len = payload_len_out;
	PRINT_ARRAY("AAD", out->ptr, out->len);
	return ok;
}