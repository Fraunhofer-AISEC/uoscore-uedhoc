/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_MESSAGE_4_H__
#define TYPES_MESSAGE_4_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAXQ 3

struct C_I_ {
 	union {
		cbor_string_type_t _C_I_bstr;
		int32_t _C_I_int;
	};
	enum {
		_C_I_bstr,
		_C_I_int,
	} _C_I_choice;
};

struct C_I__ {
 	struct C_I_ _C_I;
	size_t _C_I_present;
};

struct message_4 {
 	struct C_I__ _message_4__data_4;
	cbor_string_type_t _message_4_MAC_4;
};


#endif /* TYPES_MESSAGE_4_H__ */
