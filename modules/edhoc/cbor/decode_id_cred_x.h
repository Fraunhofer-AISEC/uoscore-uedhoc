/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef DECODE_ID_CRED_X_H__
#define DECODE_ID_CRED_X_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "decode_id_cred_x_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_decode_id_cred_x_map(
		const uint8_t *payload, size_t payload_len,
		struct id_cred_x_map *result,
		size_t *payload_len_out);


#endif /* DECODE_ID_CRED_X_H__ */
