/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "encode_message_error.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_message_error_C_x(
		zcbor_state_t *state, const struct message_error_C_x_ *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((((*input)._message_error_C_x_choice == _message_error_C_x_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._message_error_C_x_bstr))))
	: (((*input)._message_error_C_x_choice == _message_error_C_x_int) ? ((zcbor_int32_encode(state, (&(*input)._message_error_C_x_int))))
	: false))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_message_error_SUITES_R(
		zcbor_state_t *state, const struct message_error_SUITES_R_ *input)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((((*input)._message_error_SUITES_R_choice == _SUITES_R__supported) ? ((zcbor_list_start_encode(state, 10) && (int_res = (zcbor_multi_encode_minmax(2, 10, &(*input)._SUITES_R__supported_supported_count, (void *)zcbor_int32_encode, state, (&(*input)._SUITES_R__supported_supported), sizeof(int32_t))), ((zcbor_list_end_encode(state, 10)) && int_res))))
	: (((*input)._message_error_SUITES_R_choice == _message_error_SUITES_R_int) ? ((zcbor_int32_encode(state, (&(*input)._message_error_SUITES_R_int))))
	: false))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_message_error(
		zcbor_state_t *state, const struct message_error *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_present_encode(&((*input)._message_error_C_x_present), (void *)encode_repeated_message_error_C_x, state, (&(*input)._message_error_C_x))
	&& ((zcbor_tstr_encode(state, (&(*input)._message_error_DIAG_MSG))))
	&& zcbor_present_encode(&((*input)._message_error_SUITES_R_present), (void *)encode_repeated_message_error_SUITES_R, state, (&(*input)._message_error_SUITES_R)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_encode_message_error(
		uint8_t *payload, size_t payload_len,
		const struct message_error *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 3);

	bool ret = encode_message_error(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
