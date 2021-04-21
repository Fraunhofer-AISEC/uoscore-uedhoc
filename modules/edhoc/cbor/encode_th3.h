/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef ENCODE_TH3_H__
#define ENCODE_TH3_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "types_encode_th3.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


bool cbor_encode_th3(
		uint8_t *payload, size_t payload_len,
		const struct th3 *input,
		size_t *payload_len_out);


#endif /* ENCODE_TH3_H__ */
