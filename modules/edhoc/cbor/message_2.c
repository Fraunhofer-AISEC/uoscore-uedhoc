/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "message_2.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_repeated_data_2_C_I(
		cbor_state_t *state, const struct data_2_C_I_ *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((*input)._data_2_C_I_choice == _data_2_C_I_int) ? ((intx32_encode(state, (&(*input)._data_2_C_I_int))))
	: (((*input)._data_2_C_I_choice == _data_2_C_I_bstr) ? ((bstrx_encode(state, (&(*input)._data_2_C_I_bstr))))
	: false))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_data_2(
		cbor_state_t *state, const struct data_2 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((present_encode(&((*input)._data_2_C_I_present), (void *)encode_repeated_data_2_C_I, state, (&(*input)._data_2_C_I))
	&& ((bstrx_encode(state, (&(*input)._data_2_G_Y))))
	&& ((((*input)._data_2_C_R_choice == _data_2_C_R_int) ? ((intx32_encode(state, (&(*input)._data_2_C_R_int))))
	: (((*input)._data_2_C_R_choice == _data_2_C_R_bstr) ? ((bstrx_encode(state, (&(*input)._data_2_C_R_bstr))))
	: false))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_message_2(
		cbor_state_t *state, const struct message_2 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((encode_data_2(state, (&(*input)._message_2__data_2))))
	&& ((bstrx_encode(state, (&(*input)._message_2_CIPHERTEXT_2)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_message_2(
		const struct message_2 *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_message_2(NULL, input);
}


bool cbor_encode_message_2(
		uint8_t *payload, size_t payload_len,
		const struct message_2 *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_message_2,
		4, 1);
}
