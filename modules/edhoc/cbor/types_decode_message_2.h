/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_DECODE_MESSAGE_2_H__
#define TYPES_DECODE_MESSAGE_2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAXQ 3

struct m2 {
 	union {
		cbor_string_type_t _m2_G_Y_tstr;
		cbor_string_type_t _m2_G_Y_bstr;
	};
	enum {
		_m2_G_Y_tstr,
		_m2_G_Y_bstr,
	} _m2_G_Y_choice;
	union {
		int32_t _m2_C_R_int;
		cbor_string_type_t _m2_C_R_bstr;
	};
	enum {
		_m2_C_R_int,
		_m2_C_R_bstr,
	} _m2_C_R_choice;
	cbor_string_type_t _m2_CIPHERTEXT_2;
};


#endif /* TYPES_DECODE_MESSAGE_2_H__ */
