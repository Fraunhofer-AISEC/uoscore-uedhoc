/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef ENCODE_ID_CRED_X_H__
#define ENCODE_ID_CRED_X_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "encode_id_cred_x_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_encode_id_cred_x_map(
		uint8_t *payload, size_t payload_len,
		const struct id_cred_x_map *input,
		size_t *payload_len_out);


#endif /* ENCODE_ID_CRED_X_H__ */
