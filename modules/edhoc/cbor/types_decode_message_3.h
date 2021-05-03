/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_MESSAGE_3_H__
#define TYPES_DECODE_MESSAGE_3_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct m3_C_R_ {
 	union {
		cbor_string_type_t _m3_C_R_bstr;
		int32_t _m3_C_R_int;
	};
	enum {
		_m3_C_R_bstr,
		_m3_C_R_int,
	} _m3_C_R_choice;
};

struct m3 {
 	struct m3_C_R_ _m3_C_R;
	size_t _m3_C_R_present;
	cbor_string_type_t _m3_CIPHERTEXT_3;
};


#endif /* TYPES_DECODE_MESSAGE_3_H__ */
