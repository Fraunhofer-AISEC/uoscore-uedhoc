/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_ID_CRED_X_H__
#define TYPES_DECODE_ID_CRED_X_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

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
	uint32_t _id_cred_x_map_kid_present;
	struct id_cred_x_map_x5chain _id_cred_x_map_x5chain;
	uint32_t _id_cred_x_map_x5chain_present;
	struct id_cred_x_map_x5t_ _id_cred_x_map_x5t;
	uint32_t _id_cred_x_map_x5t_present;
};


#endif /* TYPES_DECODE_ID_CRED_X_H__ */
