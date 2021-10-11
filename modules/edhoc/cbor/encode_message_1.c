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
#include "encode_message_1.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_message_1(
		cbor_state_t *state, const struct message_1 *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((intx32_encode(state, (&(*input)._message_1_METHOD))))
	&& ((((*input)._message_1_SUITES_I_choice == _SUITES_I__suite) ? ((list_start_encode(state, 10) && (int_res = (multi_encode(2, 10, &(*input)._SUITES_I__suite_suite_count, (void *)intx32_encode, state, (&(*input)._SUITES_I__suite_suite), sizeof(int32_t))), ((list_end_encode(state, 10)) && int_res))))
	: (((*input)._message_1_SUITES_I_choice == _message_1_SUITES_I_int) ? ((intx32_encode(state, (&(*input)._message_1_SUITES_I_int))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._message_1_G_X))))
	&& ((((*input)._message_1_C_I_choice == _message_1_C_I_int) ? ((intx32_encode(state, (&(*input)._message_1_C_I_int))))
	: (((*input)._message_1_C_I_choice == _message_1_C_I_bstr) ? ((bstrx_encode(state, (&(*input)._message_1_C_I_bstr))))
	: false)))
	&& present_encode(&((*input)._message_1_ead_1_present), (void *)bstrx_encode, state, (&(*input)._message_1_ead_1)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_message_1(
		uint8_t *payload, uint32_t payload_len,
		const struct message_1 *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 5);

	bool ret = encode_message_1(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
