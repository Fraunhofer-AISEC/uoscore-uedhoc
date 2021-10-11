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
#include "decode_message_1.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_message_1(
		cbor_state_t *state, struct message_1 *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((intx32_decode(state, (&(*result)._message_1_METHOD))))
	&& ((union_start_code(state) && (int_res = ((((list_start_decode(state) && (int_res = (multi_decode(2, 10, &(*result)._SUITES_I__suite_suite_count, (void *)intx32_decode, state, (&(*result)._SUITES_I__suite_suite), sizeof(int32_t))), ((list_end_decode(state)) && int_res)))) && (((*result)._message_1_SUITES_I_choice = _SUITES_I__suite) || 1))
	|| (union_elem_code(state) && (((intx32_decode(state, (&(*result)._message_1_SUITES_I_int)))) && (((*result)._message_1_SUITES_I_choice = _message_1_SUITES_I_int) || 1)))), union_end_code(state), int_res)))
	&& ((bstrx_decode(state, (&(*result)._message_1_G_X))))
	&& ((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._message_1_C_I_int)))) && (((*result)._message_1_C_I_choice = _message_1_C_I_int) || 1))
	|| (((bstrx_decode(state, (&(*result)._message_1_C_I_bstr)))) && (((*result)._message_1_C_I_choice = _message_1_C_I_bstr) || 1))), union_end_code(state), int_res)))
	&& present_decode(&((*result)._message_1_ead_1_present), (void *)bstrx_decode, state, (&(*result)._message_1_ead_1)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_message_1(
		const uint8_t *payload, uint32_t payload_len,
		struct message_1 *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 5);

	bool ret = decode_message_1(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
