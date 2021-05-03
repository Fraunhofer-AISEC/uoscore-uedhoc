/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_MESSAGE_2_C_I_H__
#define TYPES_DECODE_MESSAGE_2_C_I_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct m2ci {
 	union {
		int32_t _m2ci_C_I_int;
		cbor_string_type_t _m2ci_C_I_bstr;
	};
	enum {
		_m2ci_C_I_int,
		_m2ci_C_I_bstr,
	} _m2ci_C_I_choice;
	union {
		cbor_string_type_t _m2ci_G_Y_tstr;
		cbor_string_type_t _m2ci_G_Y_bstr;
	};
	enum {
		_m2ci_G_Y_tstr,
		_m2ci_G_Y_bstr,
	} _m2ci_G_Y_choice;
	union {
		int32_t _m2ci_C_R_int;
		cbor_string_type_t _m2ci_C_R_bstr;
	};
	enum {
		_m2ci_C_R_int,
		_m2ci_C_R_bstr,
	} _m2ci_C_R_choice;
	cbor_string_type_t _m2ci_CIPHERTEXT_2;
};


#endif /* TYPES_DECODE_MESSAGE_2_C_I_H__ */
