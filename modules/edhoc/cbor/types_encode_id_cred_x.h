/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_ID_CRED_X_H__
#define TYPES_ENCODE_ID_CRED_X_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct id_cred_x_map_kid {
 	int32_t _id_cred_x_map_kid;
};

struct id_cred_x_map_x5bag {
 	cbor_string_type_t _id_cred_x_map_x5bag;
};

struct id_cred_x_map_x5chain {
 	cbor_string_type_t _id_cred_x_map_x5chain;
};

struct id_cred_x_map_x5t_ {
 	union {
		int32_t _id_cred_x_map_x5t_alg_int;
		cbor_string_type_t _id_cred_x_map_x5t_alg_tstr;
	};
	enum {
		_id_cred_x_map_x5t_alg_int,
		_id_cred_x_map_x5t_alg_tstr,
	} _id_cred_x_map_x5t_alg_choice;
	cbor_string_type_t _id_cred_x_map_x5t_hash;
};

struct id_cred_x_map_x5u {
 	cbor_string_type_t _id_cred_x_map_x5u;
};

struct id_cred_x_map_c5b {
 	cbor_string_type_t _id_cred_x_map_c5b;
};

struct id_cred_x_map_c5c {
 	cbor_string_type_t _id_cred_x_map_c5c;
};

struct id_cred_x_map_c5t_ {
 	union {
		int32_t _id_cred_x_map_c5t_alg_int;
		cbor_string_type_t _id_cred_x_map_c5t_alg_tstr;
	};
	enum {
		_id_cred_x_map_c5t_alg_int,
		_id_cred_x_map_c5t_alg_tstr,
	} _id_cred_x_map_c5t_alg_choice;
	cbor_string_type_t _id_cred_x_map_c5t_hash;
};

struct id_cred_x_map_c5u {
 	cbor_string_type_t _id_cred_x_map_c5u;
};

struct id_cred_x_map {
 	struct id_cred_x_map_kid _id_cred_x_map_kid;
	uint32_t _id_cred_x_map_kid_present;
	struct id_cred_x_map_x5bag _id_cred_x_map_x5bag;
	uint32_t _id_cred_x_map_x5bag_present;
	struct id_cred_x_map_x5chain _id_cred_x_map_x5chain;
	uint32_t _id_cred_x_map_x5chain_present;
	struct id_cred_x_map_x5t_ _id_cred_x_map_x5t;
	uint32_t _id_cred_x_map_x5t_present;
	struct id_cred_x_map_x5u _id_cred_x_map_x5u;
	uint32_t _id_cred_x_map_x5u_present;
	struct id_cred_x_map_c5b _id_cred_x_map_c5b;
	uint32_t _id_cred_x_map_c5b_present;
	struct id_cred_x_map_c5c _id_cred_x_map_c5c;
	uint32_t _id_cred_x_map_c5c_present;
	struct id_cred_x_map_c5t_ _id_cred_x_map_c5t;
	uint32_t _id_cred_x_map_c5t_present;
	struct id_cred_x_map_c5u _id_cred_x_map_c5u;
	uint32_t _id_cred_x_map_c5u_present;
};


#endif /* TYPES_ENCODE_ID_CRED_X_H__ */
