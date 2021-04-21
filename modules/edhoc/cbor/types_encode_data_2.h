/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_ENCODE_DATA_2_H__
#define TYPES_ENCODE_DATA_2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAXQ 3

struct data_2_C_I_ {
 	union {
		int32_t _data_2_C_I_int;
		cbor_string_type_t _data_2_C_I_bstr;
	};
	enum {
		_data_2_C_I_int,
		_data_2_C_I_bstr,
	} _data_2_C_I_choice;
};

struct data_2 {
 	struct data_2_C_I_ _data_2_C_I;
	size_t _data_2_C_I_present;
	cbor_string_type_t _data_2_G_Y;
	union {
		int32_t _data_2_C_R_int;
		cbor_string_type_t _data_2_C_R_bstr;
	};
	enum {
		_data_2_C_R_int,
		_data_2_C_R_bstr,
	} _data_2_C_R_choice;
};


#endif /* TYPES_ENCODE_DATA_2_H__ */
