/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef TYPES_DECODE_CERT_H__
#define TYPES_DECODE_CERT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct cert {
 	int32_t _cert_type;
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
