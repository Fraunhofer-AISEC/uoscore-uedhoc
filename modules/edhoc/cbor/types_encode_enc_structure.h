/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_ENCODE_ENC_STRUCTURE_H__
#define TYPES_ENCODE_ENC_STRUCTURE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"

#define DEFAULT_MAX_QTY 3

struct enc_structure {
 	cbor_string_type_t _enc_structure_context;
	cbor_string_type_t _enc_structure_protected;
	cbor_string_type_t _enc_structure_external_aad;
};


#endif /* TYPES_ENCODE_ENC_STRUCTURE_H__ */
