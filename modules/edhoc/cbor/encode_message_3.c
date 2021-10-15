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
#include "encode_message_3.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_m3_CIPHERTEXT_3(
		cbor_state_t *state, const cbor_string_type_t *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((bstrx_encode(state, (&(*input))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_m3_CIPHERTEXT_3(
		uint8_t *payload, uint32_t payload_len,
		const cbor_string_type_t *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[2];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = encode_m3_CIPHERTEXT_3(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
