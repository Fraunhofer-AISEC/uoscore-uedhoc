/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "message_4.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_repeated_C_I(
		cbor_state_t *state, const struct C_I_ *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((*input)._C_I_choice == _C_I_bstr) ? ((bstrx_encode(state, (&(*input)._C_I_bstr))))
	: (((*input)._C_I_choice == _C_I_int) ? ((intx32_encode(state, (&(*input)._C_I_int))))
	: false))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_C_I(
		cbor_state_t *state, const struct C_I__ *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (present_encode(&((*input)._C_I_present), (void *)encode_repeated_C_I, state, (&(*input)._C_I)));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_message_4(
		cbor_state_t *state, const struct message_4 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((encode_C_I(state, (&(*input)._message_4__data_4))))
	&& ((bstrx_encode(state, (&(*input)._message_4_MAC_4)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_message_4(
		const struct message_4 *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_message_4(NULL, input);
}


bool cbor_encode_message_4(
		uint8_t *payload, size_t payload_len,
		const struct message_4 *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_message_4,
		2, 1);
}
