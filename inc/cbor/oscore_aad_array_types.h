/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef OSCORE_AAD_ARRAY_TYPES_H__
#define OSCORE_AAD_ARRAY_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct aad_array {
	uint32_t _aad_array_oscore_version;
	union {
		int32_t _aad_array_algorithms_alg_aead_int;
		struct zcbor_string _aad_array_algorithms_alg_aead_tstr;
	};
	enum {
		_aad_array_algorithms_alg_aead_int,
		_aad_array_algorithms_alg_aead_tstr,
	} _aad_array_algorithms_alg_aead_choice;
	struct zcbor_string _aad_array_request_kid;
	struct zcbor_string _aad_array_request_piv;
	struct zcbor_string _aad_array_options;
};


#endif /* OSCORE_AAD_ARRAY_TYPES_H__ */
