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
#include "decode_message_3.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_repeated_m3_C_R(
		cbor_state_t *state, struct m3_C_R_ *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((union_start_code(state) && (int_res = ((((bstrx_decode(state, (&(*result)._m3_C_R_bstr)))) && (((*result)._m3_C_R_choice = _m3_C_R_bstr) || 1))
	|| (((intx32_decode(state, (&(*result)._m3_C_R_int)))) && (((*result)._m3_C_R_choice = _m3_C_R_int) || 1))), union_end_code(state), int_res))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_m3(
		cbor_state_t *state, struct m3 *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((present_decode(&((*result)._m3_C_R_present), (void *)decode_repeated_m3_C_R, state, (&(*result)._m3_C_R))
	&& ((bstrx_decode(state, (&(*result)._m3_CIPHERTEXT_3)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_m3(
		const uint8_t *payload, uint32_t payload_len,
		struct m3 *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 2);

	bool ret = decode_m3(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
