/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_SUITES_I_H__
#define TYPES_DECODE_SUITES_I_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct suites_ {
 	union {
		struct {
			int32_t _suites__int_int[3];
			uint32_t _suites__int_int_count;
		};
		int32_t _suites_int;
	};
	enum {
		_suites__int,
		_suites_int,
	} _suites_choice;
};


#endif /* TYPES_DECODE_SUITES_I_H__ */
