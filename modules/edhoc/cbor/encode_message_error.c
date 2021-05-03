/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_message_error.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_message_error_C_x(
		cbor_state_t *state, const struct message_error_C_x_ *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((*input)._message_error_C_x_choice == _message_error_C_x_bstr) ? ((bstrx_encode(state, (&(*input)._message_error_C_x_bstr))))
	: (((*input)._message_error_C_x_choice == _message_error_C_x_int) ? ((intx32_encode(state, (&(*input)._message_error_C_x_int))))
	: false))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_message_error_SUITES_R(
		cbor_state_t *state, const struct message_error_SUITES_R_ *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((*input)._message_error_SUITES_R_choice == _message_error_SUITES_R__supported) ? ((list_start_encode(state, 10) && (int_res = (multi_encode(2, 10, &(*input)._message_error_SUITES_R__supported_supported_count, (void *)intx32_encode, state, (&(*input)._message_error_SUITES_R__supported_supported), sizeof(int32_t))), ((list_end_encode(state, 10)) && int_res))))
	: (((*input)._message_error_SUITES_R_choice == _message_error_SUITES_R_int) ? ((intx32_encode(state, (&(*input)._message_error_SUITES_R_int))))
	: false))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_message_error(
		cbor_state_t *state, const struct message_error *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((present_encode(&((*input)._message_error_C_x_present), (void *)encode_repeated_message_error_C_x, state, (&(*input)._message_error_C_x))
	&& ((tstrx_encode(state, (&(*input)._message_error_DIAG_MSG))))
	&& present_encode(&((*input)._message_error_SUITES_R_present), (void *)encode_repeated_message_error_SUITES_R, state, (&(*input)._message_error_SUITES_R)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_message_error(
		const struct message_error *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_message_error(NULL, input);
}


bool cbor_encode_message_error(
		uint8_t *payload, size_t payload_len,
		const struct message_error *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_message_error,
		3, 2);
}
