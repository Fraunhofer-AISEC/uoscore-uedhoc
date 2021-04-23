/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef ENCODE_MESSAGE_2_C_I_H__
#define ENCODE_MESSAGE_2_C_I_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "types_encode_message_2_c_i.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


bool cbor_encode_m2ci(
		uint8_t *payload, size_t payload_len,
		const struct m2ci *input,
		size_t *payload_len_out);


#endif /* ENCODE_MESSAGE_2_C_I_H__ */
