/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_th4.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_th4(
		cbor_state_t *state, const struct th4 *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((bstrx_encode(state, (&(*input)._th4_th_3))))
	&& ((bstrx_encode(state, (&(*input)._th4_CIPHERTEXT_3)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_th4(
		const struct th4 *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_th4(NULL, input);
}


bool cbor_encode_th4(
		uint8_t *payload, size_t payload_len,
		const struct th4 *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_th4,
		2, 0);
}
