/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_message_2.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_m2(
		cbor_state_t *state, const struct m2 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((((*input)._m2_G_Y_choice == _m2_G_Y_tstr) ? ((tstrx_encode(state, (&(*input)._m2_G_Y_tstr))))
	: (((*input)._m2_G_Y_choice == _m2_G_Y_bstr) ? ((bstrx_encode(state, (&(*input)._m2_G_Y_bstr))))
	: false)))
	&& ((((*input)._m2_C_R_choice == _m2_C_R_int) ? ((intx32_encode(state, (&(*input)._m2_C_R_int))))
	: (((*input)._m2_C_R_choice == _m2_C_R_bstr) ? ((bstrx_encode(state, (&(*input)._m2_C_R_bstr))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._m2_CIPHERTEXT_2)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_m2(
		const struct m2 *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_m2(NULL, input);
}


bool cbor_encode_m2(
		uint8_t *payload, size_t payload_len,
		const struct m2 *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_m2,
		3, 1);
}
