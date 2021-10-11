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
#include "decode_message_2_c_i.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_m2ci(
		cbor_state_t *state, struct m2ci *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._m2ci_C_I_int)))) && (((*result)._m2ci_C_I_choice = _m2ci_C_I_int) || 1))
	|| (((bstrx_decode(state, (&(*result)._m2ci_C_I_bstr)))) && (((*result)._m2ci_C_I_choice = _m2ci_C_I_bstr) || 1))), union_end_code(state), int_res)))
	&& ((union_start_code(state) && (int_res = ((((tstrx_decode(state, (&(*result)._m2ci_G_Y_tstr)))) && (((*result)._m2ci_G_Y_choice = _m2ci_G_Y_tstr) || 1))
	|| (((bstrx_decode(state, (&(*result)._m2ci_G_Y_bstr)))) && (((*result)._m2ci_G_Y_choice = _m2ci_G_Y_bstr) || 1))), union_end_code(state), int_res)))
	&& ((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._m2ci_C_R_int)))) && (((*result)._m2ci_C_R_choice = _m2ci_C_R_int) || 1))
	|| (((bstrx_decode(state, (&(*result)._m2ci_C_R_bstr)))) && (((*result)._m2ci_C_R_choice = _m2ci_C_R_bstr) || 1))), union_end_code(state), int_res)))
	&& ((bstrx_decode(state, (&(*result)._m2ci_CIPHERTEXT_2)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_m2ci(
		const uint8_t *payload, uint32_t payload_len,
		struct m2ci *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 4);

	bool ret = decode_m2ci(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
