/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "aad_array.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_aad_array(
		cbor_state_t *state, const struct aad_array *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((list_start_encode(state, 5) && (int_res = (((uintx32_encode(state, (&(*input)._aad_array_oscore_version))))
	&& ((list_start_encode(state, 1) && (int_res = (((((*input)._aad_array_algorithms_alg_aead_choice == _aad_array_algorithms_alg_aead_int) ? ((intx32_encode(state, (&(*input)._aad_array_algorithms_alg_aead_int))))
	: (((*input)._aad_array_algorithms_alg_aead_choice == _aad_array_algorithms_alg_aead_tstr) ? ((tstrx_encode(state, (&(*input)._aad_array_algorithms_alg_aead_tstr))))
	: false)))), ((list_end_encode(state, 1)) && int_res))))
	&& ((bstrx_encode(state, (&(*input)._aad_array_request_kid))))
	&& ((bstrx_encode(state, (&(*input)._aad_array_request_piv))))
	&& ((bstrx_encode(state, (&(*input)._aad_array_options))))), ((list_end_encode(state, 5)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_aad_array(
		const struct aad_array *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_aad_array(NULL, input);
}


bool cbor_encode_aad_array(
		uint8_t *payload, size_t payload_len,
		const struct aad_array *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_aad_array,
		1, 3);
}
