/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_bstr_type.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_b_str(
		cbor_state_t *state, const cbor_string_type_t *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((bstrx_encode(state, (&(*input))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_b_str(
		const cbor_string_type_t *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_b_str(NULL, input);
}


bool cbor_encode_b_str(
		uint8_t *payload, size_t payload_len,
		const cbor_string_type_t *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_b_str,
		1, 0);
}
