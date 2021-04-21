/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef DECODE_CERT_H__
#define DECODE_CERT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "types_decode_cert.h"

#if DEFAULT_MAXQ != 3
#error "The type file was generated with a different default_maxq than this file"
#endif


bool cbor_decode_cert(
		const uint8_t *payload, size_t payload_len,
		struct cert *result,
		size_t *payload_len_out);


#endif /* DECODE_CERT_H__ */
