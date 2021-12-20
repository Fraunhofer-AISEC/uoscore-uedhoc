/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
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
	&& (intx32_decode(state, (&(*result)._id_cred_x_map_kid)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_x5bag(
		cbor_state_t *state, struct id_cred_x_map_x5bag *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (32))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_x5bag)))));

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
	&& (list_start_decode(state) && (int_res = (((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._id_cred_x_map_x5t_alg_int)))) && (((*result)._id_cred_x_map_x5t_alg_choice = _id_cred_x_map_x5t_alg_int) || 1))
	|| (((tstrx_decode(state, (&(*result)._id_cred_x_map_x5t_alg_tstr)))) && (((*result)._id_cred_x_map_x5t_alg_choice = _id_cred_x_map_x5t_alg_tstr) || 1))), union_end_code(state), int_res)))
	&& ((bstrx_decode(state, (&(*result)._id_cred_x_map_x5t_hash))))), ((list_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_x5u(
		cbor_state_t *state, struct id_cred_x_map_x5u *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (35))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_x5u)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_c5b(
		cbor_state_t *state, struct id_cred_x_map_c5b *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (52))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_c5b)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_c5c(
		cbor_state_t *state, struct id_cred_x_map_c5c *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (53))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_c5c)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_c5t(
		cbor_state_t *state, struct id_cred_x_map_c5t_ *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = ((((uintx32_expect(state, (54))))
	&& (list_start_decode(state) && (int_res = (((union_start_code(state) && (int_res = ((((intx32_decode(state, (&(*result)._id_cred_x_map_c5t_alg_int)))) && (((*result)._id_cred_x_map_c5t_alg_choice = _id_cred_x_map_c5t_alg_int) || 1))
	|| (((tstrx_decode(state, (&(*result)._id_cred_x_map_c5t_alg_tstr)))) && (((*result)._id_cred_x_map_c5t_alg_choice = _id_cred_x_map_c5t_alg_tstr) || 1))), union_end_code(state), int_res)))
	&& ((bstrx_decode(state, (&(*result)._id_cred_x_map_c5t_hash))))), ((list_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_repeated_id_cred_x_map_c5u(
		cbor_state_t *state, struct id_cred_x_map_c5u *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_expect(state, (55))))
	&& (bstrx_decode(state, (&(*result)._id_cred_x_map_c5u)))));

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
	&& present_decode(&((*result)._id_cred_x_map_x5bag_present), (void *)decode_repeated_id_cred_x_map_x5bag, state, (&(*result)._id_cred_x_map_x5bag))
	&& present_decode(&((*result)._id_cred_x_map_x5chain_present), (void *)decode_repeated_id_cred_x_map_x5chain, state, (&(*result)._id_cred_x_map_x5chain))
	&& present_decode(&((*result)._id_cred_x_map_x5t_present), (void *)decode_repeated_id_cred_x_map_x5t, state, (&(*result)._id_cred_x_map_x5t))
	&& present_decode(&((*result)._id_cred_x_map_x5u_present), (void *)decode_repeated_id_cred_x_map_x5u, state, (&(*result)._id_cred_x_map_x5u))
	&& present_decode(&((*result)._id_cred_x_map_c5b_present), (void *)decode_repeated_id_cred_x_map_c5b, state, (&(*result)._id_cred_x_map_c5b))
	&& present_decode(&((*result)._id_cred_x_map_c5c_present), (void *)decode_repeated_id_cred_x_map_c5c, state, (&(*result)._id_cred_x_map_c5c))
	&& present_decode(&((*result)._id_cred_x_map_c5t_present), (void *)decode_repeated_id_cred_x_map_c5t, state, (&(*result)._id_cred_x_map_c5t))
	&& present_decode(&((*result)._id_cred_x_map_c5u_present), (void *)decode_repeated_id_cred_x_map_c5u, state, (&(*result)._id_cred_x_map_c5u))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_id_cred_x_map(
		const uint8_t *payload, uint32_t payload_len,
		struct id_cred_x_map *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[5];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_id_cred_x_map(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
