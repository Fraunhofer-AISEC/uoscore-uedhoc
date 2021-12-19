/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_PLAINTEXT_H__
#define TYPES_DECODE_PLAINTEXT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct map_kid {
 	int32_t _map_kid;
};

struct map_x5bag {
 	cbor_string_type_t _map_x5bag;
};

struct map_x5chain {
 	cbor_string_type_t _map_x5chain;
};

struct map_x5t_ {
 	union {
		int32_t _map_x5t_alg_int;
		cbor_string_type_t _map_x5t_alg_tstr;
	};
	enum {
		_map_x5t_alg_int,
		_map_x5t_alg_tstr,
	} _map_x5t_alg_choice;
	cbor_string_type_t _map_x5t_hash;
};

struct map_x5u {
 	cbor_string_type_t _map_x5u;
};

struct map_c5b {
 	cbor_string_type_t _map_c5b;
};

struct map_c5c {
 	cbor_string_type_t _map_c5c;
};

struct map_c5t_ {
 	union {
		int32_t _map_c5t_alg_int;
		cbor_string_type_t _map_c5t_alg_tstr;
	};
	enum {
		_map_c5t_alg_int,
		_map_c5t_alg_tstr,
	} _map_c5t_alg_choice;
	cbor_string_type_t _map_c5t_hash;
};

struct map_c5u {
 	cbor_string_type_t _map_c5u;
};

struct map {
 	struct map_kid _map_kid;
	uint32_t _map_kid_present;
	struct map_x5bag _map_x5bag;
	uint32_t _map_x5bag_present;
	struct map_x5chain _map_x5chain;
	uint32_t _map_x5chain_present;
	struct map_x5t_ _map_x5t;
	uint32_t _map_x5t_present;
	struct map_x5u _map_x5u;
	uint32_t _map_x5u_present;
	struct map_c5b _map_c5b;
	uint32_t _map_c5b_present;
	struct map_c5c _map_c5c;
	uint32_t _map_c5c_present;
	struct map_c5t_ _map_c5t;
	uint32_t _map_c5t_present;
	struct map_c5u _map_c5u;
	uint32_t _map_c5u_present;
};

struct plaintext {
 	union {
		struct map _plaintext_ID_CRED_x__map;
		cbor_string_type_t _plaintext_ID_CRED_x_bstr;
		int32_t _plaintext_ID_CRED_x_int;
	};
	enum {
		_plaintext_ID_CRED_x__map,
		_plaintext_ID_CRED_x_bstr,
		_plaintext_ID_CRED_x_int,
	} _plaintext_ID_CRED_x_choice;
	cbor_string_type_t _plaintext_SGN_or_MAC_x;
	cbor_string_type_t _plaintext_AD_x;
	uint32_t _plaintext_AD_x_present;
};


#endif /* TYPES_DECODE_PLAINTEXT_H__ */
