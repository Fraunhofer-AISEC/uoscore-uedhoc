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
#include "encode_message_2_c_i.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_m2ci(
		cbor_state_t *state, const struct m2ci *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((((*input)._m2ci_C_I_choice == _m2ci_C_I_int) ? ((intx32_encode(state, (&(*input)._m2ci_C_I_int))))
	: (((*input)._m2ci_C_I_choice == _m2ci_C_I_bstr) ? ((bstrx_encode(state, (&(*input)._m2ci_C_I_bstr))))
	: false)))
	&& ((((*input)._m2ci_G_Y_choice == _m2ci_G_Y_tstr) ? ((tstrx_encode(state, (&(*input)._m2ci_G_Y_tstr))))
	: (((*input)._m2ci_G_Y_choice == _m2ci_G_Y_bstr) ? ((bstrx_encode(state, (&(*input)._m2ci_G_Y_bstr))))
	: false)))
	&& ((((*input)._m2ci_C_R_choice == _m2ci_C_R_int) ? ((intx32_encode(state, (&(*input)._m2ci_C_R_int))))
	: (((*input)._m2ci_C_R_choice == _m2ci_C_R_bstr) ? ((bstrx_encode(state, (&(*input)._m2ci_C_R_bstr))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._m2ci_CIPHERTEXT_2)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_m2ci(
		uint8_t *payload, uint32_t payload_len,
		const struct m2ci *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 4);

	bool ret = encode_m2ci(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
