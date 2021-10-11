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
#include "encode_data_2.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_data_2_C_I(
		cbor_state_t *state, const struct data_2_C_I_ *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((*input)._data_2_C_I_choice == _data_2_C_I_int) ? ((intx32_encode(state, (&(*input)._data_2_C_I_int))))
	: (((*input)._data_2_C_I_choice == _data_2_C_I_bstr) ? ((bstrx_encode(state, (&(*input)._data_2_C_I_bstr))))
	: false))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_data_2(
		cbor_state_t *state, const struct data_2 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((present_encode(&((*input)._data_2_C_I_present), (void *)encode_repeated_data_2_C_I, state, (&(*input)._data_2_C_I))
	&& ((bstrx_encode(state, (&(*input)._data_2_G_Y))))
	&& ((((*input)._data_2_C_R_choice == _data_2_C_R_int) ? ((intx32_encode(state, (&(*input)._data_2_C_R_int))))
	: (((*input)._data_2_C_R_choice == _data_2_C_R_bstr) ? ((bstrx_encode(state, (&(*input)._data_2_C_R_bstr))))
	: false))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_data_2(
		uint8_t *payload, uint32_t payload_len,
		const struct data_2 *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 3);

	bool ret = encode_data_2(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
