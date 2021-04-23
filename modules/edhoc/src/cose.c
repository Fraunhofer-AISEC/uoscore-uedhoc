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

#include "../inc/error.h"
#include "../cbor/encode_enc_structure.h"
#include "../cbor/encode_sig_structure.h"

enum edhoc_error cose_enc_structure_encode(
	const uint8_t *context, uint16_t context_len, const uint8_t *protected,
	uint16_t protected_len, const uint8_t *external_aad,
	uint16_t external_aad_len, uint8_t *out, uint16_t *out_len)
{
	bool success_encoding;
	struct enc_structure enc_structure;

	enc_structure._enc_structure_context.value = context;
	enc_structure._enc_structure_context.len = context_len;
	enc_structure._enc_structure_protected.value = protected;
	enc_structure._enc_structure_protected.len = protected_len;
	enc_structure._enc_structure_external_aad.value = external_aad;
	enc_structure._enc_structure_external_aad.len = external_aad_len;

	size_t payload_len_out;
	success_encoding = cbor_encode_enc_structure(
		out, *out_len, &enc_structure, &payload_len_out);

	if (!success_encoding) {
		return cbor_encoding_error;
	}
	*out_len = payload_len_out;
	return edhoc_no_error;
}

enum edhoc_error
cose_sig_structure_encode(const uint8_t *context, uint16_t context_len,
			  const uint8_t *protected, uint16_t protected_len,
			  const uint8_t *external_aad,
			  uint16_t external_aad_len, const uint8_t *payload,
			  uint16_t payload_len, uint8_t *out, uint16_t *out_len)
{
	bool success_encoding;
	struct sig_structure sig_structure;

	sig_structure._sig_structure_context.value = context;
	sig_structure._sig_structure_context.len = context_len;
	sig_structure._sig_structure_protected.value = protected;
	sig_structure._sig_structure_protected.len = protected_len;
	sig_structure._sig_structure_external_aad.value = external_aad;
	sig_structure._sig_structure_external_aad.len = external_aad_len;
	sig_structure._sig_structure_payload.value = payload;
	sig_structure._sig_structure_payload.len = payload_len;

	size_t payload_len_out;
	success_encoding = cbor_encode_sig_structure(
		out, *out_len, &sig_structure, &payload_len_out);

	if (!success_encoding) {
		return cbor_encoding_error;
	}
	*out_len = payload_len_out;
	return edhoc_no_error;
}