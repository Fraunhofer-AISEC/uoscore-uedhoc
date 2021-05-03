/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "decode_message_1.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_message_1(
		cbor_state_t *state, struct message_1 *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((intx32_decode(state, (&(*result)._message_1_METHOD_CORR))))
	&& ((union_start_code(state) && (int_res = ((((list_start_decode(state) && (int_res = (((intx32_decode(state, (&(*result)._message_1_SUITES_I__selected_selected))))
	&& multi_decode(2, 10, &(*result)._message_1_SUITES_I__selected_supported_count, (void *)intx32_decode, state, (&(*result)._message_1_SUITES_I__selected_supported), sizeof(int32_t))), ((list_end_decode(state)) && int_res)))) && (((*result)._message_1_SUITES_I_choice = _message_1_SUITES_I__selected) || 1))
	|| (union_elem_code(state) && (((intx32_decode(state, (&(*result)._message_1_SUITES_I_int)))) && (((*result)._message_1_SUITES_I_choice = _message_1_SUITES_I_int) || 1)))), union_end_code(state), int_res)))
	&& ((bstrx_decode(state, (&(*result)._message_1_G_X))))
	&& ((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._message_1_C_I_int)))) && (((*result)._message_1_C_I_choice = _message_1_C_I_int) || 1))
	|| (((bstrx_decode(state, (&(*result)._message_1_C_I_bstr)))) && (((*result)._message_1_C_I_choice = _message_1_C_I_bstr) || 1))), union_end_code(state), int_res)))
	&& present_decode(&((*result)._message_1_AD_1_present), (void *)bstrx_decode, state, (&(*result)._message_1_AD_1)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_decode_message_1(
		struct message_1 *result)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return decode_message_1(NULL, result);
}


bool cbor_decode_message_1(
		const uint8_t *payload, size_t payload_len,
		struct message_1 *result,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)result,
		payload_len_out, (void *)decode_message_1,
		5, 2);
}
