/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_message_1.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_message_1(
		cbor_state_t *state, const struct message_1 *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((intx32_encode(state, (&(*input)._message_1_METHOD_CORR))))
	&& ((((*input)._message_1_SUITES_I_choice == _message_1_SUITES_I__selected) ? ((list_start_encode(state, 11) && (int_res = (((intx32_encode(state, (&(*input)._message_1_SUITES_I__selected_selected))))
	&& multi_encode(2, 10, &(*input)._message_1_SUITES_I__selected_supported_count, (void *)intx32_encode, state, (&(*input)._message_1_SUITES_I__selected_supported), sizeof(int32_t))), ((list_end_encode(state, 11)) && int_res))))
	: (((*input)._message_1_SUITES_I_choice == _message_1_SUITES_I_int) ? ((intx32_encode(state, (&(*input)._message_1_SUITES_I_int))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._message_1_G_X))))
	&& ((((*input)._message_1_C_I_choice == _message_1_C_I_int) ? ((intx32_encode(state, (&(*input)._message_1_C_I_int))))
	: (((*input)._message_1_C_I_choice == _message_1_C_I_bstr) ? ((bstrx_encode(state, (&(*input)._message_1_C_I_bstr))))
	: false)))
	&& present_encode(&((*input)._message_1_AD_1_present), (void *)bstrx_encode, state, (&(*input)._message_1_AD_1)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_message_1(
		const struct message_1 *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_message_1(NULL, input);
}


bool cbor_encode_message_1(
		uint8_t *payload, size_t payload_len,
		const struct message_1 *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_message_1,
		5, 2);
}
