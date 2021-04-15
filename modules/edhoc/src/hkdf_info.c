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

#include <string.h>

#include "../inc/byte_array.h"
#include "../inc/error.h"
#include "../inc/suites.h"
#include "../cbor/info.h"

EdhocError create_hkdf_info(enum aead_alg aead_alg, const uint8_t *th,
			    uint8_t th_len, const char *label, uint64_t okm_len,
			    uint8_t *out, uint8_t *out_len)
{
	bool success;
	struct info info;

	info._info_edhoc_aead_id_choice = _info_edhoc_aead_id_int;
	info._info_edhoc_aead_id_int = aead_alg;

	info._info_transcript_hash.value = th;
	info._info_transcript_hash.len = th_len;

	info._info_label.value = label;
	info._info_label.len = strlen(label);

	info._info_length = okm_len;

	size_t payload_len_out;
	success = cbor_encode_info(out, *out_len, &info, &payload_len_out);

	if (!success) {
		return cbor_encoding_error;
	}
	*out_len = payload_len_out;

	return EdhocNoError;
}
