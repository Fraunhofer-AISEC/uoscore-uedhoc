/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "encode_id_cred_x.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_id_cred_x_map_kid(
		zcbor_state_t *state, const struct id_cred_x_map_kid *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (4))))
	&& (zcbor_int32_encode(state, (&(*input)._id_cred_x_map_kid)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5bag(
		zcbor_state_t *state, const struct id_cred_x_map_x5bag *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (32))))
	&& (zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_x5bag)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5chain(
		zcbor_state_t *state, const struct id_cred_x_map_x5chain *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (33))))
	&& (zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_x5chain)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5t(
		zcbor_state_t *state, const struct id_cred_x_map_x5t_ *input)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = ((((zcbor_uint32_put(state, (34))))
	&& (zcbor_list_start_encode(state, 2) && (int_res = (((((*input)._id_cred_x_map_x5t_alg_choice == _id_cred_x_map_x5t_alg_int) ? ((zcbor_int32_encode(state, (&(*input)._id_cred_x_map_x5t_alg_int))))
	: (((*input)._id_cred_x_map_x5t_alg_choice == _id_cred_x_map_x5t_alg_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._id_cred_x_map_x5t_alg_tstr))))
	: false)))
	&& ((zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_x5t_hash))))), ((zcbor_list_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5u(
		zcbor_state_t *state, const struct id_cred_x_map_x5u *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (35))))
	&& (zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_x5u)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5b(
		zcbor_state_t *state, const struct id_cred_x_map_c5b *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (52))))
	&& (zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_c5b)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5c(
		zcbor_state_t *state, const struct id_cred_x_map_c5c *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (53))))
	&& (zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_c5c)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5t(
		zcbor_state_t *state, const struct id_cred_x_map_c5t_ *input)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = ((((zcbor_uint32_put(state, (54))))
	&& (zcbor_list_start_encode(state, 2) && (int_res = (((((*input)._id_cred_x_map_c5t_alg_choice == _id_cred_x_map_c5t_alg_int) ? ((zcbor_int32_encode(state, (&(*input)._id_cred_x_map_c5t_alg_int))))
	: (((*input)._id_cred_x_map_c5t_alg_choice == _id_cred_x_map_c5t_alg_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._id_cred_x_map_c5t_alg_tstr))))
	: false)))
	&& ((zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_c5t_hash))))), ((zcbor_list_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5u(
		zcbor_state_t *state, const struct id_cred_x_map_c5u *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (55))))
	&& (zcbor_bstr_encode(state, (&(*input)._id_cred_x_map_c5u)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_id_cred_x_map(
		zcbor_state_t *state, const struct id_cred_x_map *input)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 9) && (int_res = (zcbor_present_encode(&((*input)._id_cred_x_map_kid_present), (void *)encode_repeated_id_cred_x_map_kid, state, (&(*input)._id_cred_x_map_kid))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_x5bag_present), (void *)encode_repeated_id_cred_x_map_x5bag, state, (&(*input)._id_cred_x_map_x5bag))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_x5chain_present), (void *)encode_repeated_id_cred_x_map_x5chain, state, (&(*input)._id_cred_x_map_x5chain))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_x5t_present), (void *)encode_repeated_id_cred_x_map_x5t, state, (&(*input)._id_cred_x_map_x5t))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_x5u_present), (void *)encode_repeated_id_cred_x_map_x5u, state, (&(*input)._id_cred_x_map_x5u))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_c5b_present), (void *)encode_repeated_id_cred_x_map_c5b, state, (&(*input)._id_cred_x_map_c5b))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_c5c_present), (void *)encode_repeated_id_cred_x_map_c5c, state, (&(*input)._id_cred_x_map_c5c))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_c5t_present), (void *)encode_repeated_id_cred_x_map_c5t, state, (&(*input)._id_cred_x_map_c5t))
	&& zcbor_present_encode(&((*input)._id_cred_x_map_c5u_present), (void *)encode_repeated_id_cred_x_map_c5u, state, (&(*input)._id_cred_x_map_c5u))), ((zcbor_map_end_encode(state, 9)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_encode_id_cred_x_map(
		uint8_t *payload, size_t payload_len,
		const struct id_cred_x_map *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_id_cred_x_map(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
