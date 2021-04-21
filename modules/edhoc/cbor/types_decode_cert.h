/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
 * Generated with a default_maxq of 3
 */

#ifndef TYPES_DECODE_CERT_H__
#define TYPES_DECODE_CERT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAXQ 3

struct cert {
 	int32_t _cert_cert_type;
	cbor_string_type_t _cert_serial_number;
	cbor_string_type_t _cert_issuer;
	int32_t _cert_validity_not_before;
	int32_t _cert_validity_not_after;
	cbor_string_type_t _cert_subject;
	cbor_string_type_t _cert_pk;
	int32_t _cert_extensions;
	cbor_string_type_t _cert_signature;
};


#endif /* TYPES_DECODE_CERT_H__ */
