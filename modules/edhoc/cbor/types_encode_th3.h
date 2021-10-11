/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_TH3_H__
#define TYPES_ENCODE_TH3_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

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
 	cbor_string_type_t _th3_th_2;
	cbor_string_type_t _th3_CIPHERTEXT_2;
	struct th3_data_3_ _th3_data_3;
	uint32_t _th3_data_3_present;
};


#endif /* TYPES_ENCODE_TH3_H__ */
