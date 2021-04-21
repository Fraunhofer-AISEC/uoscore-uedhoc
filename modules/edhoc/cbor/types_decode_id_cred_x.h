/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_DECODE_ID_CRED_X_H__
#define TYPES_DECODE_ID_CRED_X_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAXQ 3

struct id_cred_x_map_kid {
 	cbor_string_type_t _id_cred_x_map_kid;
};

struct id_cred_x_map_x5chain {
 	cbor_string_type_t _id_cred_x_map_x5chain;
};

struct id_cred_x_map_x5t_ {
 	int32_t _id_cred_x_map_x5t_int;
	cbor_string_type_t _id_cred_x_map_x5t_bstr;
};

struct id_cred_x_map {
 	struct id_cred_x_map_kid _id_cred_x_map_kid;
	size_t _id_cred_x_map_kid_present;
	struct id_cred_x_map_x5chain _id_cred_x_map_x5chain;
	size_t _id_cred_x_map_x5chain_present;
	struct id_cred_x_map_x5t_ _id_cred_x_map_x5t;
	size_t _id_cred_x_map_x5t_present;
};


#endif /* TYPES_DECODE_ID_CRED_X_H__ */
