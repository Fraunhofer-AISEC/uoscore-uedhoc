/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include <cbor.h>

#include "../inc/error.h"
#include "../inc/memcpy_s.h"

EdhocError cbor_decoder(
    uint8_t **next_ptr,
    uint8_t *in_buffer, uint32_t in_size,
    void *out_decoded_data, uint64_t *out_decoded_len, CborType *type) {
    CborParser parser;
    CborValue value;
    CborError err;
    uint64_t len = 0;
    EdhocError r;

    /* Initialization */
    err = cbor_parser_init(in_buffer, in_size, 0, &parser, &value);
    if (err != CborNoError) return ErrorDuringCborDecoding;
    /* Get type of input CBOR object */
    *type = cbor_value_get_type(&value);

    switch (*type) {
        case CborIntegerType:
            err = cbor_value_get_int(&value, out_decoded_data);
            if (err != CborNoError) return ErrorDuringCborDecoding;
            *out_decoded_len = 1;
            err = cbor_value_advance_fixed(&value);
            if (err != CborNoError) return ErrorDuringCborDecoding;
            break;

        case CborByteStringType: {
            err = cbor_value_calculate_string_length(&value, (size_t *)&len);
            if (err != CborNoError) return ErrorDuringCborDecoding;
            if (len > *out_decoded_len) return CborByteStringBufferToSmall;
            *out_decoded_len = len;
            /*
             * cbor_value_copy_byte_string will add a null byte after the last copied byte thus the buffer must be one byte bigger, see https://github.com/intel/tinycbor/issues/194
             */
            uint8_t tmp[len + 1];

            err = cbor_value_copy_byte_string(
                &value, tmp,
                (size_t *)out_decoded_len, &value);
            if (err != CborNoError) return ErrorDuringCborDecoding;

            r = _memcpy_s(out_decoded_data, *out_decoded_len, tmp, len);
            if (r != EdhocNoError) return r;
            break;
        }

        case CborTextStringType:
            err = cbor_value_calculate_string_length(&value, (size_t *)&len);
            if (err != CborNoError) return ErrorDuringCborDecoding;
            if (len > *out_decoded_len) return CborByteStringBufferToSmall;
            *out_decoded_len = len;
            err = cbor_value_copy_text_string(
                &value, out_decoded_data,
                (size_t *)out_decoded_len, &value);
            if (err != CborNoError) return ErrorDuringCborDecoding;
            break;

        case CborMapType:
            err = cbor_value_get_map_length(&value, (size_t *)out_decoded_len);
            if (err != CborNoError) return ErrorDuringCborDecoding;
            value.offset++;
            break;
        default:
            return UnsupportedCborType;
            break;
    }

    *next_ptr = (uint8_t *)(in_buffer + value.offset);
    return EdhocNoError;
}
