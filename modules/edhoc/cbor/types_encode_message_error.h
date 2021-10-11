/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_MESSAGE_ERROR_H__
#define TYPES_ENCODE_MESSAGE_ERROR_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct message_error_C_x_ {
 	union {
		cbor_string_type_t _message_error_C_x_bstr;
		int32_t _message_error_C_x_int;
	};
	enum {
		_message_error_C_x_bstr,
		_message_error_C_x_int,
	} _message_error_C_x_choice;
};

struct message_error_SUITES_R_ {
 	union {
		struct {
			int32_t _SUITES_R__supported_supported[10];
			uint32_t _SUITES_R__supported_supported_count;
		};
		int32_t _message_error_SUITES_R_int;
	};
	enum {
		_SUITES_R__supported,
		_message_error_SUITES_R_int,
	} _message_error_SUITES_R_choice;
};

struct message_error {
 	struct message_error_C_x_ _message_error_C_x;
	uint32_t _message_error_C_x_present;
	cbor_string_type_t _message_error_DIAG_MSG;
	struct message_error_SUITES_R_ _message_error_SUITES_R;
	uint32_t _message_error_SUITES_R_present;
};


#endif /* TYPES_ENCODE_MESSAGE_ERROR_H__ */
