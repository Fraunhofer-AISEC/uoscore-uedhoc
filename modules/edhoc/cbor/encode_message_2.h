/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#ifndef ENCODE_MESSAGE_2_H__
#define ENCODE_MESSAGE_2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "types_encode_message_2.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_encode_m2(
		uint8_t *payload, size_t payload_len,
		const struct m2 *input,
		size_t *payload_len_out);


#endif /* ENCODE_MESSAGE_2_H__ */
