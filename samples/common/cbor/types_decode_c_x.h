/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_C_X_H__
#define TYPES_DECODE_C_X_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct cx_C_X_ {
 	union {
		cbor_string_type_t _cx_C_X_bstr;
		int32_t _cx_C_X_int;
	};
	enum {
		_cx_C_X_bstr,
		_cx_C_X_int,
	} _cx_C_X_choice;
};


#endif /* TYPES_DECODE_C_X_H__ */
