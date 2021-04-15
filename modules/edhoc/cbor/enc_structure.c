/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "enc_structure.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_enc_structure(
		cbor_state_t *state, const struct enc_structure *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((list_start_encode(state, 3) && (int_res = (((tstrx_encode(state, (&(*input)._enc_structure_context))))
	&& ((bstrx_encode(state, (&(*input)._enc_structure_protected))))
	&& ((bstrx_encode(state, (&(*input)._enc_structure_external_aad))))), ((list_end_encode(state, 3)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_enc_structure(
		const struct enc_structure *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_enc_structure(NULL, input);
}


bool cbor_encode_enc_structure(
		uint8_t *payload, size_t payload_len,
		const struct enc_structure *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_enc_structure,
		1, 1);
}
