/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "decode_suites_i.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_suites(
		cbor_state_t *state, struct suites_ *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((union_start_code(state) && (int_res = ((((list_start_decode(state) && (int_res = (multi_decode(2, 3, &(*result)._suites__int_int_count, (void *)intx32_decode, state, (&(*result)._suites__int_int), sizeof(int32_t))), ((list_end_decode(state)) && int_res)))) && (((*result)._suites_choice = _suites__int) || 1))
	|| (union_elem_code(state) && (((intx32_decode(state, (&(*result)._suites_int)))) && (((*result)._suites_choice = _suites_int) || 1)))), union_end_code(state), int_res))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_suites(
		const uint8_t *payload, uint32_t payload_len,
		struct suites_ *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_suites(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
