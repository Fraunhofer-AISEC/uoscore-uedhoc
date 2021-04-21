/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_info.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_info(
		cbor_state_t *state, const struct info *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((list_start_encode(state, 4) && (int_res = (((((*input)._info_edhoc_aead_id_choice == _info_edhoc_aead_id_int) ? ((intx32_encode(state, (&(*input)._info_edhoc_aead_id_int))))
	: (((*input)._info_edhoc_aead_id_choice == _info_edhoc_aead_id_tstr) ? ((tstrx_encode(state, (&(*input)._info_edhoc_aead_id_tstr))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._info_transcript_hash))))
	&& ((tstrx_encode(state, (&(*input)._info_label))))
	&& ((uintx32_encode(state, (&(*input)._info_length))))), ((list_end_encode(state, 4)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_info(
		const struct info *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_info(NULL, input);
}


bool cbor_encode_info(
		uint8_t *payload, size_t payload_len,
		const struct info *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_info,
		1, 2);
}