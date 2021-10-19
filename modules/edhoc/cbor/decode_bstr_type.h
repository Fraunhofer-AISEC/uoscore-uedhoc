/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef DECODE_BSTR_TYPE_H__
#define DECODE_BSTR_TYPE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "types_decode_bstr_type.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_decode_bstr_type_b_str(
		const uint8_t *payload, uint32_t payload_len,
		cbor_string_type_t *result,
		uint32_t *payload_len_out);


#endif /* DECODE_BSTR_TYPE_H__ */
