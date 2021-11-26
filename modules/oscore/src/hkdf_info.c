/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include <string.h>

#include "../inc/aad.h"
#include "../inc/hkdf_info.h"
#include "../cbor/info.h"

/*
HKDF = composition of HKDF-Extract and HKDF-Expand (RFC5869)
output = HKDF(salt, IKM, info, L
* salt = Master Salt
* IKM = Master Secret
* info = CBOR-array [
     id: bstr,
     alg_aead: int / tstr,
     type: tstr,
     L: uint,
  ]
     + id: SenderID / RecipientID for keys; empty string for CommonIV
     + alg_aead: AEAD Algorithm
     + type: "Key" / "IV", ascii string without nul-terminator
     + L: size of key/iv for AEAD alg
         - in bytes
* https://www.iana.org/assignments/cose/cose.xhtml
*/

inline enum err create_hkdf_info(struct byte_array *id,
					  struct byte_array *id_context,
					  enum AEAD_algorithm aead_alg,
					  enum derive_type type,
					  struct byte_array *out)
{
	bool success_encoding;
	struct info info_struct;

	char type_enc[10];
	uint8_t len = 0;
	switch (type) {
	case KEY:
		strncpy(type_enc, "Key", 10);
		len = 16;
		break;
	case IV:
		strncpy(type_enc, "IV", 10);
		len = 13;
		break;
	default:
		break;
	}

	info_struct._info_id.value = id->ptr;
	info_struct._info_id.len = id->len;

	if (id_context->len == 0) {
		info_struct._info_id_context_choice = _info_id_context_nil;
	} else {
		info_struct._info_id_context_choice = _info_id_context_bstr;
		info_struct._info_id_context_bstr.value = id_context->ptr;
		info_struct._info_id_context_bstr.len = id_context->len;
	}
	info_struct._info_alg_aead_choice = _info_alg_aead_int;
	info_struct._info_alg_aead_int = aead_alg;

	info_struct._info_type.value = (uint8_t *)type_enc;
	info_struct._info_type.len = strlen(type_enc);
	info_struct._info_L = len;

	uint32_t payload_len_out;
	success_encoding = cbor_encode_info(out->ptr, out->len, &info_struct,
					    &payload_len_out);

	if (!success_encoding) {
		return cbor_encoding_error;
	}
	out->len = payload_len_out;
	return oscore_no_error;
}