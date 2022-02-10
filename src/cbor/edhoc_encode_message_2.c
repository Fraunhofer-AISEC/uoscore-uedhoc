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
#include "edhoc_encode_message_2.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_m2(
		zcbor_state_t *state, const struct m2 *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((((zcbor_bstr_encode(state, (&(*input)._m2_G_Y_CIPHERTEXT_2))))
	&& ((((*input)._m2_C_R_choice == _m2_C_R_int) ? ((zcbor_int32_encode(state, (&(*input)._m2_C_R_int))))
	: (((*input)._m2_C_R_choice == _m2_C_R_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._m2_C_R_bstr))))
	: false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_encode_m2(
		uint8_t *payload, size_t payload_len,
		const struct m2 *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 2);

	bool ret = encode_m2(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
