/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_AAD_ARRAY_H__
#define TYPES_AAD_ARRAY_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct aad_array {
 	uint32_t _aad_array_oscore_version;
	union {
		int32_t _aad_array_algorithms_alg_aead_int;
		cbor_string_type_t _aad_array_algorithms_alg_aead_tstr;
	};
	enum {
		_aad_array_algorithms_alg_aead_int,
		_aad_array_algorithms_alg_aead_tstr,
	} _aad_array_algorithms_alg_aead_choice;
	cbor_string_type_t _aad_array_request_kid;
	cbor_string_type_t _aad_array_request_piv;
	cbor_string_type_t _aad_array_options;
};


#endif /* TYPES_AAD_ARRAY_H__ */
