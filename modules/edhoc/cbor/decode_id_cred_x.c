/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "decode_id_cred_x.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_repeated_id_cred_x_map_kid(
		cbor_state_t *state, struct id_cred_x_map_kid *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (4))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_kid)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_x5chain(
		cbor_state_t *state, struct id_cred_x_map_x5chain *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (33))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_x5chain)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_x5t(
		cbor_state_t *state, struct id_cred_x_map_x5t_ *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = ((((uintx32_expect(state, (34))))
	&& (list_start_decode(state) && (int_res = (((intx32_decode(state, (&(*result)._id_cred_x_map_x5t_int))))
	&& ((bstrx_decode(state, (&(*result)._id_cred_x_map_x5t_bstr))))), ((list_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_id_cred_x_map(
		cbor_state_t *state, struct id_cred_x_map *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = (present_decode(&((*result)._id_cred_x_map_kid_present), (void *)decode_repeated_id_cred_x_map_kid, state, (&(*result)._id_cred_x_map_kid))
	&& present_decode(&((*result)._id_cred_x_map_x5chain_present), (void *)decode_repeated_id_cred_x_map_x5chain, state, (&(*result)._id_cred_x_map_x5chain))
	&& present_decode(&((*result)._id_cred_x_map_x5t_present), (void *)decode_repeated_id_cred_x_map_x5t, state, (&(*result)._id_cred_x_map_x5t))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_decode_id_cred_x_map(
		struct id_cred_x_map *result)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return decode_id_cred_x_map(NULL, result);
}


bool cbor_decode_id_cred_x_map(
		const uint8_t *payload, size_t payload_len,
		struct id_cred_x_map *result,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)result,
		payload_len_out, (void *)decode_id_cred_x_map,
		1, 2);
}
