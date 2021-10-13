/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_MESSAGE_2_H__
#define TYPES_ENCODE_MESSAGE_2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct m2 {
 	cbor_string_type_t _m2_G_Y_CIPHERTEXT_2;
	union {
		int32_t _m2_C_R_int;
		cbor_string_type_t _m2_C_R_bstr;
	};
	enum {
		_m2_C_R_int,
		_m2_C_R_bstr,
	} _m2_C_R_choice;
};


#endif /* TYPES_ENCODE_MESSAGE_2_H__ */
