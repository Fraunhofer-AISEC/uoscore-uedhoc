/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "decode_message_3.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_repeated_m3_C_R(
		cbor_state_t *state, struct m3_C_R_ *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((union_start_code(state) && (int_res = ((((bstrx_decode(state, (&(*result)._m3_C_R_bstr)))) && (((*result)._m3_C_R_choice = _m3_C_R_bstr) || 1))
	|| (((intx32_decode(state, (&(*result)._m3_C_R_int)))) && (((*result)._m3_C_R_choice = _m3_C_R_int) || 1))), union_end_code(state), int_res))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_m3(
		cbor_state_t *state, struct m3 *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((present_decode(&((*result)._m3_C_R_present), (void *)decode_repeated_m3_C_R, state, (&(*result)._m3_C_R))
	&& ((bstrx_decode(state, (&(*result)._m3_CIPHERTEXT_3)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_decode_m3(
		struct m3 *result)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return decode_m3(NULL, result);
}


bool cbor_decode_m3(
		const uint8_t *payload, size_t payload_len,
		struct m3 *result,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)result,
		payload_len_out, (void *)decode_m3,
		2, 1);
}
