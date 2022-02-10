/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef EDHOC_DECODE_PLAINTEXT_TYPES_H__
#define EDHOC_DECODE_PLAINTEXT_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct map_kid {
	int32_t _map_kid;
};

struct map_x5bag {
	struct zcbor_string _map_x5bag;
};

struct map_x5chain {
	struct zcbor_string _map_x5chain;
};

struct map_x5t_ {
	union {
		int32_t _map_x5t_alg_int;
		struct zcbor_string _map_x5t_alg_tstr;
	};
	enum {
		_map_x5t_alg_int,
		_map_x5t_alg_tstr,
	} _map_x5t_alg_choice;
	struct zcbor_string _map_x5t_hash;
};

struct map_x5u {
	struct zcbor_string _map_x5u;
};

struct map_c5b {
	struct zcbor_string _map_c5b;
};

struct map_c5c {
	struct zcbor_string _map_c5c;
};

struct map_c5t_ {
	union {
		int32_t _map_c5t_alg_int;
		struct zcbor_string _map_c5t_alg_tstr;
	};
	enum {
		_map_c5t_alg_int,
		_map_c5t_alg_tstr,
	} _map_c5t_alg_choice;
	struct zcbor_string _map_c5t_hash;
};

struct map_c5u {
	struct zcbor_string _map_c5u;
};

struct map {
	struct map_kid _map_kid;
	uint_fast32_t _map_kid_present;
	struct map_x5bag _map_x5bag;
	uint_fast32_t _map_x5bag_present;
	struct map_x5chain _map_x5chain;
	uint_fast32_t _map_x5chain_present;
	struct map_x5t_ _map_x5t;
	uint_fast32_t _map_x5t_present;
	struct map_x5u _map_x5u;
	uint_fast32_t _map_x5u_present;
	struct map_c5b _map_c5b;
	uint_fast32_t _map_c5b_present;
	struct map_c5c _map_c5c;
	uint_fast32_t _map_c5c_present;
	struct map_c5t_ _map_c5t;
	uint_fast32_t _map_c5t_present;
	struct map_c5u _map_c5u;
	uint_fast32_t _map_c5u_present;
};

struct plaintext {
	union {
		struct map _plaintext_ID_CRED_x__map;
		struct zcbor_string _plaintext_ID_CRED_x_bstr;
		int32_t _plaintext_ID_CRED_x_int;
	};
	enum {
		_plaintext_ID_CRED_x__map,
		_plaintext_ID_CRED_x_bstr,
		_plaintext_ID_CRED_x_int,
	} _plaintext_ID_CRED_x_choice;
	struct zcbor_string _plaintext_SGN_or_MAC_x;
	struct zcbor_string _plaintext_AD_x;
	uint_fast32_t _plaintext_AD_x_present;
};


#endif /* EDHOC_DECODE_PLAINTEXT_TYPES_H__ */
