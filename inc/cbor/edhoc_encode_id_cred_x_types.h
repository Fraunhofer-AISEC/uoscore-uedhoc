/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef EDHOC_ENCODE_ID_CRED_X_TYPES_H__
#define EDHOC_ENCODE_ID_CRED_X_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct id_cred_x_map_kid {
	int32_t _id_cred_x_map_kid;
};

struct id_cred_x_map_x5bag {
	struct zcbor_string _id_cred_x_map_x5bag;
};

struct id_cred_x_map_x5chain {
	struct zcbor_string _id_cred_x_map_x5chain;
};

struct id_cred_x_map_x5t_ {
	union {
		int32_t _id_cred_x_map_x5t_alg_int;
		struct zcbor_string _id_cred_x_map_x5t_alg_tstr;
	};
	enum {
		_id_cred_x_map_x5t_alg_int,
		_id_cred_x_map_x5t_alg_tstr,
	} _id_cred_x_map_x5t_alg_choice;
	struct zcbor_string _id_cred_x_map_x5t_hash;
};

struct id_cred_x_map_x5u {
	struct zcbor_string _id_cred_x_map_x5u;
};

struct id_cred_x_map_c5b {
	struct zcbor_string _id_cred_x_map_c5b;
};

struct id_cred_x_map_c5c {
	struct zcbor_string _id_cred_x_map_c5c;
};

struct id_cred_x_map_c5t_ {
	union {
		int32_t _id_cred_x_map_c5t_alg_int;
		struct zcbor_string _id_cred_x_map_c5t_alg_tstr;
	};
	enum {
		_id_cred_x_map_c5t_alg_int,
		_id_cred_x_map_c5t_alg_tstr,
	} _id_cred_x_map_c5t_alg_choice;
	struct zcbor_string _id_cred_x_map_c5t_hash;
};

struct id_cred_x_map_c5u {
	struct zcbor_string _id_cred_x_map_c5u;
};

struct id_cred_x_map {
	struct id_cred_x_map_kid _id_cred_x_map_kid;
	uint_fast32_t _id_cred_x_map_kid_present;
	struct id_cred_x_map_x5bag _id_cred_x_map_x5bag;
	uint_fast32_t _id_cred_x_map_x5bag_present;
	struct id_cred_x_map_x5chain _id_cred_x_map_x5chain;
	uint_fast32_t _id_cred_x_map_x5chain_present;
	struct id_cred_x_map_x5t_ _id_cred_x_map_x5t;
	uint_fast32_t _id_cred_x_map_x5t_present;
	struct id_cred_x_map_x5u _id_cred_x_map_x5u;
	uint_fast32_t _id_cred_x_map_x5u_present;
	struct id_cred_x_map_c5b _id_cred_x_map_c5b;
	uint_fast32_t _id_cred_x_map_c5b_present;
	struct id_cred_x_map_c5c _id_cred_x_map_c5c;
	uint_fast32_t _id_cred_x_map_c5c_present;
	struct id_cred_x_map_c5t_ _id_cred_x_map_c5t;
	uint_fast32_t _id_cred_x_map_c5t_present;
	struct id_cred_x_map_c5u _id_cred_x_map_c5u;
	uint_fast32_t _id_cred_x_map_c5u_present;
};


#endif /* EDHOC_ENCODE_ID_CRED_X_TYPES_H__ */
