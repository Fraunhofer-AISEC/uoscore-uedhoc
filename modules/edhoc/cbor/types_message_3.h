/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_MESSAGE_3_H__
#define TYPES_MESSAGE_3_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAXQ 3

struct C_R_ {
 	union {
		cbor_string_type_t _C_R_bstr;
		int32_t _C_R_int;
	};
	enum {
		_C_R_bstr,
		_C_R_int,
	} _C_R_choice;
};

struct C_R__ {
 	struct C_R_ _C_R;
	size_t _C_R_present;
};

struct message_3 {
 	struct C_R__ _message_3__data_3;
	cbor_string_type_t _message_3_CIPHERTEXT_3;
};


#endif /* TYPES_MESSAGE_3_H__ */
