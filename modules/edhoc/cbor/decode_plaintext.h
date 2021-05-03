/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_max_qty of 3
 */

#ifndef DECODE_PLAINTEXT_H__
#define DECODE_PLAINTEXT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "types_decode_plaintext.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_decode_plaintext(
		const uint8_t *payload, size_t payload_len,
		struct plaintext *result,
		size_t *payload_len_out);


#endif /* DECODE_PLAINTEXT_H__ */
