/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_ENCODE_TH3_H__
#define TYPES_ENCODE_TH3_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAXQ 3

struct th3_data_3_ {
 	union {
		cbor_string_type_t _th3_data_3_bstr;
		int32_t _th3_data_3_int;
	};
	enum {
		_th3_data_3_bstr,
		_th3_data_3_int,
	} _th3_data_3_choice;
};

struct th3 {
 	cbor_string_type_t _th3_th2;
	cbor_string_type_t _th3_CIPHERTEXT_2;
	struct th3_data_3_ _th3_data_3;
	size_t _th3_data_3_present;
};


#endif /* TYPES_ENCODE_TH3_H__ */
