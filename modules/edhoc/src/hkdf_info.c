/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/hkdf_info.h"

#include <cbor.h>
#include <string.h>

#include "../inc/byte_array.h"
#include "../inc/error.h"
#include "../inc/suites.h"

EdhocError create_hkdf_info(enum aead_alg aead_alg, const uint8_t *th,
			    uint8_t th_len, const char *label, uint64_t okm_len,
			    uint8_t *out, uint8_t *out_len)
{
	CborEncoder enc;
	cbor_encoder_init(&enc, out, *out_len, 0);
	CborEncoder array_enc;
	CborError r;

	cbor_encoder_create_array(&enc, &array_enc, 4);

	r = cbor_encode_int(&array_enc, aead_alg);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;

	r = cbor_encode_byte_string(&array_enc, th, th_len);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;

	r = cbor_encode_text_stringz(&array_enc, label);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;

	r = cbor_encode_uint(&array_enc, okm_len);
	if (r == CborErrorOutOfMemory)
		return CborEncodingBufferToSmall;

	cbor_encoder_close_container(&enc, &array_enc);

	/* Get the CBOR length */
	*out_len = cbor_encoder_get_buffer_size(&enc, out);

	return EdhocNoError;
}
