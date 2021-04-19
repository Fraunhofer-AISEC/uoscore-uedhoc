/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef D_MESSAGE_1_H__
#define D_MESSAGE_1_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "types_d_message_1.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


bool cbor_decode_message_1(
		const uint8_t *payload, size_t payload_len,
		struct message_1 *result,
		size_t *payload_len_out);


#endif /* D_MESSAGE_1_H__ */
