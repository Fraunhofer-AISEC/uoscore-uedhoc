/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_id_cred_x.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_id_cred_x_map_kid(
		cbor_state_t *state, const struct id_cred_x_map_kid *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (4))))
	&& (intx32_encode(state, (&(*input)._id_cred_x_map_kid)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5bag(
		cbor_state_t *state, const struct id_cred_x_map_x5bag *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (32))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_x5bag)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5chain(
		cbor_state_t *state, const struct id_cred_x_map_x5chain *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (33))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_x5chain)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5t(
		cbor_state_t *state, const struct id_cred_x_map_x5t_ *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = ((((uintx32_put(state, (34))))
	&& (list_start_encode(state, 2) && (int_res = (((((*input)._id_cred_x_map_x5t_alg_choice == _id_cred_x_map_x5t_alg_int) ? ((intx32_encode(state, (&(*input)._id_cred_x_map_x5t_alg_int))))
	: (((*input)._id_cred_x_map_x5t_alg_choice == _id_cred_x_map_x5t_alg_tstr) ? ((tstrx_encode(state, (&(*input)._id_cred_x_map_x5t_alg_tstr))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._id_cred_x_map_x5t_hash))))), ((list_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_x5u(
		cbor_state_t *state, const struct id_cred_x_map_x5u *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (35))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_x5u)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5b(
		cbor_state_t *state, const struct id_cred_x_map_c5b *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (52))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_c5b)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5c(
		cbor_state_t *state, const struct id_cred_x_map_c5c *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (53))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_c5c)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5t(
		cbor_state_t *state, const struct id_cred_x_map_c5t_ *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = ((((uintx32_put(state, (54))))
	&& (list_start_encode(state, 2) && (int_res = (((((*input)._id_cred_x_map_c5t_alg_choice == _id_cred_x_map_c5t_alg_int) ? ((intx32_encode(state, (&(*input)._id_cred_x_map_c5t_alg_int))))
	: (((*input)._id_cred_x_map_c5t_alg_choice == _id_cred_x_map_c5t_alg_tstr) ? ((tstrx_encode(state, (&(*input)._id_cred_x_map_c5t_alg_tstr))))
	: false)))
	&& ((bstrx_encode(state, (&(*input)._id_cred_x_map_c5t_hash))))), ((list_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_repeated_id_cred_x_map_c5u(
		cbor_state_t *state, const struct id_cred_x_map_c5u *input)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = ((((uintx32_put(state, (55))))
	&& (bstrx_encode(state, (&(*input)._id_cred_x_map_c5u)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_id_cred_x_map(
		cbor_state_t *state, const struct id_cred_x_map *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 9) && (int_res = (present_encode(&((*input)._id_cred_x_map_kid_present), (void *)encode_repeated_id_cred_x_map_kid, state, (&(*input)._id_cred_x_map_kid))
	&& present_encode(&((*input)._id_cred_x_map_x5bag_present), (void *)encode_repeated_id_cred_x_map_x5bag, state, (&(*input)._id_cred_x_map_x5bag))
	&& present_encode(&((*input)._id_cred_x_map_x5chain_present), (void *)encode_repeated_id_cred_x_map_x5chain, state, (&(*input)._id_cred_x_map_x5chain))
	&& present_encode(&((*input)._id_cred_x_map_x5t_present), (void *)encode_repeated_id_cred_x_map_x5t, state, (&(*input)._id_cred_x_map_x5t))
	&& present_encode(&((*input)._id_cred_x_map_x5u_present), (void *)encode_repeated_id_cred_x_map_x5u, state, (&(*input)._id_cred_x_map_x5u))
	&& present_encode(&((*input)._id_cred_x_map_c5b_present), (void *)encode_repeated_id_cred_x_map_c5b, state, (&(*input)._id_cred_x_map_c5b))
	&& present_encode(&((*input)._id_cred_x_map_c5c_present), (void *)encode_repeated_id_cred_x_map_c5c, state, (&(*input)._id_cred_x_map_c5c))
	&& present_encode(&((*input)._id_cred_x_map_c5t_present), (void *)encode_repeated_id_cred_x_map_c5t, state, (&(*input)._id_cred_x_map_c5t))
	&& present_encode(&((*input)._id_cred_x_map_c5u_present), (void *)encode_repeated_id_cred_x_map_c5u, state, (&(*input)._id_cred_x_map_c5u))), ((map_end_encode(state, 9)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_id_cred_x_map(
		uint8_t *payload, uint32_t payload_len,
		const struct id_cred_x_map *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[5];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = encode_id_cred_x_map(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
