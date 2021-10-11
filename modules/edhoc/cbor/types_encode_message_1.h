/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_MESSAGE_1_H__
#define TYPES_ENCODE_MESSAGE_1_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct message_1 {
 	int32_t _message_1_METHOD;
	union {
		struct {
			int32_t _SUITES_I__suite_suite[10];
			uint32_t _SUITES_I__suite_suite_count;
		};
		int32_t _message_1_SUITES_I_int;
	};
	enum {
		_SUITES_I__suite,
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
	cbor_string_type_t _message_1_ead_1;
	uint32_t _message_1_ead_1_present;
};


#endif /* TYPES_ENCODE_MESSAGE_1_H__ */
