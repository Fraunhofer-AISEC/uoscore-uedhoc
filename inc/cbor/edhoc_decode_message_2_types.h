/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef EDHOC_DECODE_MESSAGE_2_TYPES_H__
#define EDHOC_DECODE_MESSAGE_2_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct m2 {
	struct zcbor_string _m2_G_Y_CIPHERTEXT_2;
	union {
		int32_t _m2_C_R_int;
		struct zcbor_string _m2_C_R_bstr;
	};
	enum {
		_m2_C_R_int,
		_m2_C_R_bstr,
	} _m2_C_R_choice;
};


#endif /* EDHOC_DECODE_MESSAGE_2_TYPES_H__ */
