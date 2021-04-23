/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/oscore_cose.h"

#include <stdio.h>

#include "../inc/crypto_wrapper.h"
#include "../inc/print_util.h"
#include "../inc/security_context.h"
#include "../cbor/enc_structure.h"

/*the additional bytes in the enc_structure are constant*/
#define ENCRYPT0_ENCODING_OVERHEAD 12

/**
 * @brief Encode the input AAD to defined COSE structure
 * @param external_aad: input aad to form COSE structure
 * @param out: output encoded COSE byte string
 * @return oscore_error
 */
static enum oscore_error create_enc_structure(struct byte_array *external_aad,
					      struct byte_array *out)
{
	bool success_encoding;
	struct enc_structure enc_structure;

	uint8_t context[] = { "Encrypt0" };
	enc_structure._enc_structure_context.value = context;
	enc_structure._enc_structure_context.len = strlen(context);
	enc_structure._enc_structure_protected.value = NULL;
	enc_structure._enc_structure_protected.len = 0;
	enc_structure._enc_structure_external_aad.value = external_aad->ptr;
	enc_structure._enc_structure_external_aad.len = external_aad->len;

	size_t payload_len_out;
	success_encoding = cbor_encode_enc_structure(
		out->ptr, out->len, &enc_structure, &payload_len_out);

	if (!success_encoding) {
		return cbor_encoding_error;
	}
	out->len = payload_len_out;
	return oscore_no_error;
}

enum oscore_error cose_decrypt(struct byte_array *in_ciphertext,
			       struct byte_array *out_plaintext,
			       struct byte_array *nonce,
			       struct byte_array *recipient_aad,
			       struct byte_array *key)
{
	/* get enc_structure */
	enum oscore_error r;
	uint32_t aad_len = recipient_aad->len + ENCRYPT0_ENCODING_OVERHEAD;
	uint8_t aad_bytes[aad_len];
	struct byte_array aad = {
		.len = aad_len,
		.ptr = aad_bytes,
	};
	r = create_enc_structure(recipient_aad, &aad);
	if (r != oscore_no_error)
		return r;
	PRINT_ARRAY("AAD encoded", aad.ptr, aad.len);

	struct byte_array tag = {
		.len = 8, .ptr = in_ciphertext->ptr + in_ciphertext->len - 8
	};

	PRINT_ARRAY("Ciphertext", in_ciphertext->ptr, in_ciphertext->len);

	r = aes_ccm_16_64_128(DECRYPT, in_ciphertext, out_plaintext, key, nonce,
			      &aad, &tag);

	if (r != oscore_no_error)
		return r;

	PRINT_ARRAY("Decrypted plaintext", out_plaintext->ptr,
		    out_plaintext->len);
	return r;
}

enum oscore_error
cose_encrypt(struct byte_array *in_plaintext, uint8_t *out_ciphertext,
	     uint32_t out_ciphertext_len, struct byte_array *nonce,
	     struct byte_array *sender_aad, struct byte_array *key)
{
	/* get enc_structure  */
	enum oscore_error r;

	uint32_t aad_len = sender_aad->len + ENCRYPT0_ENCODING_OVERHEAD;
	uint8_t aad_bytes[aad_len];
	struct byte_array aad = {
		.len = aad_len,
		.ptr = aad_bytes,
	};
	r = create_enc_structure(sender_aad, &aad);
	if (r != oscore_no_error)
		return r;
	struct byte_array tag = {
		.len = 8,
		.ptr = out_ciphertext + in_plaintext->len,
	};

	struct byte_array ctxt = {
		.len = out_ciphertext_len,
		.ptr = out_ciphertext,
	};
	r = aes_ccm_16_64_128(ENCRYPT, in_plaintext, &ctxt, key, nonce, &aad,
			      &tag);
	if (r != oscore_no_error)
		return r;

	PRINT_ARRAY("Ciphertext", out_ciphertext, out_ciphertext_len);
	return oscore_no_error;
}
