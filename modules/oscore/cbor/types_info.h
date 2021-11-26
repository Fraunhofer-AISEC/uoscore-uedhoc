/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_INFO_H__
#define TYPES_INFO_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct oscore_info {
 	cbor_string_type_t _oscore_info_id;
	union {
		cbor_string_type_t _oscore_info_id_context_bstr;
	};
	enum {
		_oscore_info_id_context_bstr,
		_oscore_info_id_context_nil,
	} _oscore_info_id_context_choice;
	union {
		int32_t _oscore_info_alg_aead_int;
		cbor_string_type_t _oscore_info_alg_aead_tstr;
	};
	enum {
		_oscore_info_alg_aead_int,
		_oscore_info_alg_aead_tstr,
	} _oscore_info_alg_aead_choice;
	cbor_string_type_t _oscore_info_type;
	uint32_t _oscore_info_L;
};


#endif /* TYPES_INFO_H__ */
