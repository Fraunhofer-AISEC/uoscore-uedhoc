/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
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
 	cbor_string_type_t _map_kid;
};

struct map_x5chain {
 	cbor_string_type_t _map_x5chain;
};

struct map_x5t_ {
 	int32_t _map_x5t_int;
	cbor_string_type_t _map_x5t_bstr;
};

struct map {
 	struct map_kid _map_kid;
	size_t _map_kid_present;
	struct map_x5chain _map_x5chain;
	size_t _map_x5chain_present;
	struct map_x5t_ _map_x5t;
	size_t _map_x5t_present;
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
	size_t _plaintext_AD_x_present;
};


#endif /* TYPES_DECODE_PLAINTEXT_H__ */
