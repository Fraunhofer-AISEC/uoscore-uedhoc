/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef EDHOC_ENCODE_ENC_STRUCTURE_TYPES_H__
#define EDHOC_ENCODE_ENC_STRUCTURE_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct edhoc_enc_structure {
	struct zcbor_string _edhoc_enc_structure_context;
	struct zcbor_string _edhoc_enc_structure_protected;
	struct zcbor_string _edhoc_enc_structure_external_aad;
};


#endif /* EDHOC_ENCODE_ENC_STRUCTURE_TYPES_H__ */
