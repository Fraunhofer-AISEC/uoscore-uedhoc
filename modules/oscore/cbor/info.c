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
#include "info.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_oscore_info(
		zcbor_state_t *state, const struct oscore_info *input)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_list_start_encode(state, 5) && (int_res = (((zcbor_bstr_encode(state, (&(*input)._oscore_info_id))))
	&& ((((*input)._oscore_info_id_context_choice == _oscore_info_id_context_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._oscore_info_id_context_bstr))))
	: (((*input)._oscore_info_id_context_choice == _oscore_info_id_context_nil) ? ((zcbor_nil_put(state, NULL)))
	: false)))
	&& ((((*input)._oscore_info_alg_aead_choice == _oscore_info_alg_aead_int) ? ((zcbor_int32_encode(state, (&(*input)._oscore_info_alg_aead_int))))
	: (((*input)._oscore_info_alg_aead_choice == _oscore_info_alg_aead_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._oscore_info_alg_aead_tstr))))
	: false)))
	&& ((zcbor_tstr_encode(state, (&(*input)._oscore_info_type))))
	&& ((zcbor_uint32_encode(state, (&(*input)._oscore_info_L))))), ((zcbor_list_end_encode(state, 5)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_encode_oscore_info(
		uint8_t *payload, size_t payload_len,
		const struct oscore_info *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_oscore_info(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
