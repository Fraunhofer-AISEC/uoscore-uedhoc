/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_message_2_c_i.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_m2ci(
		cbor_state_t *state, const struct m2ci *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((((*input)._m2ci_C_I_choice == _m2ci_C_I_int) ? ((intx32_encode(state, (&(*input)._m2ci_C_I_int))))
	: (((*input)._m2ci_C_I_choice == _m2ci_C_I_bstr) ? ((bstrx_encode(state, (&(*input)._m2ci_C_I_bstr))))
	: false)))
	&& ((((*input)._m2ci_G_Y_choice == _m2ci_G_Y_tstr) ? ((tstrx_encode(state, (&(*input)._m2ci_G_Y_tstr))))
	: (((*input)._m2ci_G_Y_choice == _m2ci_G_Y_bstr) ? ((bstrx_encode(state, (&(*input)._m2ci_G_Y_bstr))))
	: false)))
	&& ((((*input)._m2ci_C_R_choice == _m2ci_C_R_int) ? ((intx32_encode(state, (&(*input)._m2ci_C_R_int))))
	: (((*input)._m2ci_C_R_choice == _m2ci_C_R_bstr) ? ((bstrx_encode(state, (&(*input)._m2ci_C_R_bstr))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._m2ci_CIPHERTEXT_2)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_m2ci(
		const struct m2ci *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_m2ci(NULL, input);
}


bool cbor_encode_m2ci(
		uint8_t *payload, size_t payload_len,
		const struct m2ci *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_m2ci,
		4, 1);
}
