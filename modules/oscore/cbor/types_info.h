/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_INFO_H__
#define TYPES_INFO_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAXQ 3

struct info {
 	cbor_string_type_t _info_id;
	union {
		cbor_string_type_t _info_id_context_bstr;
	};
	enum {
		_info_id_context_bstr,
		_info_id_context_nil,
	} _info_id_context_choice;
	union {
		int32_t _info_alg_aead_int;
		cbor_string_type_t _info_alg_aead_tstr;
	};
	enum {
		_info_alg_aead_int,
		_info_alg_aead_tstr,
	} _info_alg_aead_choice;
	cbor_string_type_t _info_type;
	uint32_t _info_L;
};


#endif /* TYPES_INFO_H__ */
