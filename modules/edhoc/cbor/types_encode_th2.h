/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_TH2_H__
#define TYPES_ENCODE_TH2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct th2 {
 	cbor_string_type_t _th2_hash_msg1;
	cbor_string_type_t _th2_G_Y;
	union {
		cbor_string_type_t _th2_C_R_bstr;
		int32_t _th2_C_R_int;
	};
	enum {
		_th2_C_R_bstr,
		_th2_C_R_int,
	} _th2_C_R_choice;
};


#endif /* TYPES_ENCODE_TH2_H__ */
