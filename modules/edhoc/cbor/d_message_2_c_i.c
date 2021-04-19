/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "d_message_2_c_i.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool decode_m2ci(
		cbor_state_t *state, struct m2ci *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._m2ci_C_I_int)))) && (((*result)._m2ci_C_I_choice = _m2ci_C_I_int) || 1))
	|| (union_elem_code(state) && (((bstrx_decode(state, (&(*result)._m2ci_C_I_bstr)))) && (((*result)._m2ci_C_I_choice = _m2ci_C_I_bstr) || 1)))), union_end_code(state), int_res)))
	&& ((union_start_code(state) && (int_res = ((((tstrx_decode(state, (&(*result)._m2ci_G_Y_tstr)))) && (((*result)._m2ci_G_Y_choice = _m2ci_G_Y_tstr) || 1))
	|| (union_elem_code(state) && (((bstrx_decode(state, (&(*result)._m2ci_G_Y_bstr)))) && (((*result)._m2ci_G_Y_choice = _m2ci_G_Y_bstr) || 1)))), union_end_code(state), int_res)))
	&& ((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._m2ci_C_R_int)))) && (((*result)._m2ci_C_R_choice = _m2ci_C_R_int) || 1))
	|| (union_elem_code(state) && (((bstrx_decode(state, (&(*result)._m2ci_C_R_bstr)))) && (((*result)._m2ci_C_R_choice = _m2ci_C_R_bstr) || 1)))), union_end_code(state), int_res)))
	&& ((bstrx_decode(state, (&(*result)._m2ci_CIPHERTEXT_2)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_decode_m2ci(
		struct m2ci *result)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return decode_m2ci(NULL, result);
}


bool cbor_decode_m2ci(
		const uint8_t *payload, size_t payload_len,
		struct m2ci *result,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)result,
		payload_len_out, (void *)decode_m2ci,
		4, 1);
}
