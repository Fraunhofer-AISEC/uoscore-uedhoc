/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_th3.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
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

	bool tmp_result = (((((bstrx_encode(state, (&(*input)._th3_th2))))
	&& ((bstrx_encode(state, (&(*input)._th3_CIPHERTEXT_2))))
	&& present_encode(&((*input)._th3_data_3_present), (void *)encode_repeated_th3_data_3, state, (&(*input)._th3_data_3)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_th3(
		const struct th3 *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_th3(NULL, input);
}


bool cbor_encode_th3(
		uint8_t *payload, size_t payload_len,
		const struct th3 *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_th3,
		3, 1);
}
