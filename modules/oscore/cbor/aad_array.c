/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "aad_array.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
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



bool cbor_encode_aad_array(
		uint8_t *payload, uint32_t payload_len,
		const struct aad_array *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[5];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = encode_aad_array(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
