/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef EDHOC_DECODE_MESSAGE_2_H__
#define EDHOC_DECODE_MESSAGE_2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "edhoc_decode_message_2_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_decode_m2(
		const uint8_t *payload, size_t payload_len,
		struct m2 *result,
		size_t *payload_len_out);


#endif /* EDHOC_DECODE_MESSAGE_2_H__ */
