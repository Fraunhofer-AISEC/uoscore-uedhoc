/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_MESSAGE_1_H__
#define TYPES_DECODE_MESSAGE_1_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct message_1 {
 	int32_t _message_1_METHOD_CORR;
	union {
		struct {
			int32_t _message_1_SUITES_I__suite_suite[10];
			size_t _message_1_SUITES_I__suite_suite_count;
		};
		int32_t _message_1_SUITES_I_int;
	};
	enum {
		_message_1_SUITES_I__suite,
		_message_1_SUITES_I_int,
	} _message_1_SUITES_I_choice;
	cbor_string_type_t _message_1_G_X;
	union {
		int32_t _message_1_C_I_int;
		cbor_string_type_t _message_1_C_I_bstr;
	};
	enum {
		_message_1_C_I_int,
		_message_1_C_I_bstr,
	} _message_1_C_I_choice;
	cbor_string_type_t _message_1_AD_1;
	size_t _message_1_AD_1_present;
};


#endif /* TYPES_DECODE_MESSAGE_1_H__ */
