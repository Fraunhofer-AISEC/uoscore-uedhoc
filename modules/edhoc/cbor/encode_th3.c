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
#include "encode_th3.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_th3_data_3(
		cbor_state_t *state, const struct th3_data_3_ *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((*input)._th3_data_3_choice == _th3_data_3_bstr) ? ((bstrx_encode(state, (&(*input)._th3_data_3_bstr))))
	: (((*input)._th3_data_3_choice == _th3_data_3_int) ? ((intx32_encode(state, (&(*input)._th3_data_3_int))))
	: false))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_th3(
		cbor_state_t *state, const struct th3 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((bstrx_encode(state, (&(*input)._th3_th_2))))
	&& ((bstrx_encode(state, (&(*input)._th3_CIPHERTEXT_2))))
	&& present_encode(&((*input)._th3_data_3_present), (void *)encode_repeated_th3_data_3, state, (&(*input)._th3_data_3)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_th3(
		uint8_t *payload, uint32_t payload_len,
		const struct th3 *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 3);

	bool ret = encode_th3(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
