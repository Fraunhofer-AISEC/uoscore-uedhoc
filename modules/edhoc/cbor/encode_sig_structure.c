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
#include "encode_sig_structure.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_sig_structure(
		cbor_state_t *state, const struct sig_structure *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((list_start_encode(state, 4) && (int_res = (((tstrx_encode(state, (&(*input)._sig_structure_context))))
	&& ((bstrx_encode(state, (&(*input)._sig_structure_protected))))
	&& ((bstrx_encode(state, (&(*input)._sig_structure_external_aad))))
	&& ((bstrx_encode(state, (&(*input)._sig_structure_payload))))), ((list_end_encode(state, 4)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_sig_structure(
		uint8_t *payload, uint32_t payload_len,
		const struct sig_structure *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = encode_sig_structure(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
