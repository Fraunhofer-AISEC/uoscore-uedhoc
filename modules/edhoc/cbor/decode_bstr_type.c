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
#include "decode_bstr_type.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_bstr_type_b_str(
		cbor_state_t *state, cbor_string_type_t *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((bstrx_decode(state, (&(*result))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_bstr_type_b_str(
		const uint8_t *payload, uint32_t payload_len,
		cbor_string_type_t *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[2];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_bstr_type_b_str(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
