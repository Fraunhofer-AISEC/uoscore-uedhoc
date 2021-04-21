/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_id_cred_x.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


static bool encode_repeated_id_cred_x_map_kid(
		cbor_state_t *state, const struct id_cred_x_map_kid *input)
{
	cbor_print("%s\n", __func__);
	uint32_t tmp_value;

	bool tmp_result = ((((uintx32_encode(state, ((tmp_value = 4, &tmp_value)))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_kid)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5chain(
		cbor_state_t *state, const struct id_cred_x_map_x5chain *input)
{
	cbor_print("%s\n", __func__);
	uint32_t tmp_value;

	bool tmp_result = ((((uintx32_encode(state, ((tmp_value = 33, &tmp_value)))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_x5chain)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5t(
		cbor_state_t *state, const struct id_cred_x_map_x5t_ *input)
{
	cbor_print("%s\n", __func__);
	uint32_t tmp_value;
	bool int_res;

	bool tmp_result = ((((uintx32_encode(state, ((tmp_value = 34, &tmp_value)))))
	&& (list_start_encode(state, 2) && (int_res = (((intx32_encode(state, (&(*input)._id_cred_x_map_x5t_int))))
	&& ((bstrx_encode(state, (&(*input)._id_cred_x_map_x5t_bstr))))), ((list_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_id_cred_x_map(
		cbor_state_t *state, const struct id_cred_x_map *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 3) && (int_res = (present_encode(&((*input)._id_cred_x_map_kid_present), (void *)encode_repeated_id_cred_x_map_kid, state, (&(*input)._id_cred_x_map_kid))
	&& present_encode(&((*input)._id_cred_x_map_x5chain_present), (void *)encode_repeated_id_cred_x_map_x5chain, state, (&(*input)._id_cred_x_map_x5chain))
	&& present_encode(&((*input)._id_cred_x_map_x5t_present), (void *)encode_repeated_id_cred_x_map_x5t, state, (&(*input)._id_cred_x_map_x5t))), ((map_end_encode(state, 3)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



__attribute__((unused)) static bool type_test_encode_id_cred_x_map(
		const struct id_cred_x_map *input)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return encode_id_cred_x_map(NULL, input);
}


bool cbor_encode_id_cred_x_map(
		uint8_t *payload, size_t payload_len,
		const struct id_cred_x_map *input,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)input,
		payload_len_out, (void *)encode_id_cred_x_map,
		1, 2);
}
