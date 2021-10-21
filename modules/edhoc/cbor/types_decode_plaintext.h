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

struct map_x5chain {
 	cbor_string_type_t _map_x5chain;
};

struct map_x5t_ {
 	int32_t _map_x5t_int;
	cbor_string_type_t _map_x5t_bstr;
};

struct map_x5u {
 	cbor_string_type_t _map_x5u;
};

struct map {
 	struct map_kid _map_kid;
	uint32_t _map_kid_present;
	struct map_x5chain _map_x5chain;
	uint32_t _map_x5chain_present;
	struct map_x5t_ _map_x5t;
	uint32_t _map_x5t_present;
	struct map_x5u _map_x5u;
	uint32_t _map_x5u_present;
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
