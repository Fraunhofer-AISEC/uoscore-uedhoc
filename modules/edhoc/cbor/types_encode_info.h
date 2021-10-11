/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_INFO_H__
#define TYPES_ENCODE_INFO_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct info {
 	union {
		int32_t _info_edhoc_aead_id_int;
		cbor_string_type_t _info_edhoc_aead_id_tstr;
	};
	enum {
		_info_edhoc_aead_id_int,
		_info_edhoc_aead_id_tstr,
	} _info_edhoc_aead_id_choice;
	cbor_string_type_t _info_transcript_hash;
	cbor_string_type_t _info_label;
	uint32_t _info_length;
};


#endif /* TYPES_ENCODE_INFO_H__ */
