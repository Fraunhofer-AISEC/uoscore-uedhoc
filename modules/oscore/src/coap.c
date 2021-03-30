/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include "../inc/coap.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/print_util.h"

OscoreError options_into_byte_string(
    struct o_coap_option *options,
    uint8_t options_cnt,
    struct byte_array *out_byte_string) {
    uint8_t delta_extra_byte = 0;
    uint8_t len_extra_byte = 0;
    uint8_t *temp_ptr = out_byte_string->ptr;

    /* Reset length */
    uint32_t out_byte_string_capacity = out_byte_string->len;
    out_byte_string->len = 0;

    for (uint8_t i = 0; i < options_cnt; i++) {
        delta_extra_byte = 0;
        len_extra_byte = 0;

        /* Special cases for delta and length*/
        if (options[i].delta < 13 && options[i].len < 13)
            *(temp_ptr) = (uint8_t)(options[i].delta << 4) | (uint8_t)(options[i].len);
        else {
            if (options[i].delta >= 13 && options[i].delta < 243)
                delta_extra_byte = 1;
            else if (options[i].delta >= 243)
                delta_extra_byte = 2;

            if (options[i].len >= 13 && options[i].len < 243)
                len_extra_byte = 1;
            else if (options[i].len >= 243)
                len_extra_byte = 2;

            switch (delta_extra_byte) {
                case 0:
                    *(temp_ptr) = (uint8_t)(options[i].delta << 4);
                    break;
                case 1:
                    *(temp_ptr) = (uint8_t)(13 << 4);
                    *(temp_ptr + 1) = options[i].delta + 13;
                    break;
                case 2:
                    *(temp_ptr) = (uint8_t)(14 << 4);
                    uint16_t temp_delta = options[i].delta + 269;
                    *(temp_ptr + 1) = (uint8_t)((temp_delta & 0xFF00) >> 8);
                    *(temp_ptr + 2) = (uint8_t)((temp_delta & 0x00FF) >> 0);
                    break;
                default:
                    return DeltaExtraByteError;
                    break;
            }
            switch (len_extra_byte) {
                case 0:
                    *(temp_ptr) |= (uint8_t)(options[i].len);
                    break;
                case 1:
                    *(temp_ptr) |= 13;
                    *(temp_ptr + delta_extra_byte + 1) = options[i].len + 13;
                    break;
                case 2:
                    *(temp_ptr) |= 14;
                    uint16_t temp_len = options[i].len + 269;
                    *(temp_ptr + delta_extra_byte + 1) = (uint8_t)((temp_len & 0xFF00) >> 8);
                    *(temp_ptr + delta_extra_byte + 2) = (uint8_t)((temp_len & 0x00FF) >> 0);
                    break;
                default:
                    return LenExtraByteError;
                    break;
            }
        }

        /* Move to the position, where option value begins */
        temp_ptr += 1 + delta_extra_byte + len_extra_byte;
        /* Add length of current option*/
        out_byte_string->len += 1 + delta_extra_byte + len_extra_byte + options[i].len;
        /* Copy the byte string of current option into output*/
        uint64_t dest_size = out_byte_string_capacity - (temp_ptr - out_byte_string->ptr);
        OscoreError r = _memcpy_s(temp_ptr, dest_size, options[i].value, options[i].len);
        if (r != OscoreNoError) return r;
        temp_ptr += options[i].len;
    }
    return OscoreNoError;
}

/**
 * @brief Parse incoming options byte string into options structure
 * @param in_data: pointer to input data in byte string format
 * @param in_data_len: length of input byte string
 * @param out_options: pointer to output options structure array
 * @param out_options_count: count number of output options
 * @return  OscoreError
 */
static inline OscoreError buf2options(uint8_t *in_data, uint16_t in_data_len,
                                      struct o_coap_option *out_options, uint8_t *out_options_count) {
    uint8_t *temp_options_ptr = in_data;
    uint8_t temp_options_count = 0;
    uint8_t temp_option_header_len = 0;
    uint16_t temp_option_delta = 0;
    uint16_t temp_option_len = 0;
    uint8_t temp_option_number = 0;

    /* Go through the in_data to find out how many options are there */
    uint16_t i = 0;
    while (i < in_data_len) {
        temp_option_header_len = 1;
        /* Parser first byte,lower 4 bits for option value length and higher 4 bits for option delta*/
        temp_option_delta = ((*temp_options_ptr) & 0xF0) >> 4;
        temp_option_len = (*temp_options_ptr) & 0x0F;

        temp_options_ptr++;

        /* Special cases for extended option delta: 13 - 1 extra delta byte, 14 - 2 extra delta bytes, 15 - reserved */
        switch (temp_option_delta) {
            case 13:
                temp_option_header_len += 1;
                temp_option_delta = *temp_options_ptr - 13;
                temp_options_ptr += 1;
                break;
            case 14:
                temp_option_header_len += 2;
                temp_option_delta = ((uint16_t)(*temp_options_ptr) << 8 | *(temp_options_ptr + 1)) - 269;
                temp_options_ptr += 2;
                break;
            case 15:
                // ERROR
                return OscoreInPktInvalidOptionDelta;
                break;
            default:
                break;
        }

        /* Special cases for extended option value length: 13 - 1 extra length byte, 14 - 2 extra length bytes, 15 - reserved */
        switch (temp_option_len) {
            case 13:
                temp_option_header_len += 1;
                temp_option_len = *temp_options_ptr + 13;
                temp_options_ptr += 1;
                break;
            case 14:
                temp_option_header_len += 2;
                temp_option_len = ((uint16_t)(*temp_options_ptr) << 8 | *(temp_options_ptr + 1)) + 269;
                temp_options_ptr += 2;
                break;
            case 15:
                /* ERROR */
                return OscoreInPktInvalidOptionLen;
                break;
            default:
                break;
        }

        temp_option_number += temp_option_delta;
        /* Update in output options */
        out_options[temp_options_count].delta = temp_option_delta;
        out_options[temp_options_count].len = temp_option_len;
        out_options[temp_options_count].option_number =
            temp_option_number;
        if (temp_option_len == 0)
            out_options[temp_options_count].value = NULL;
        else
            out_options[temp_options_count].value =
                temp_options_ptr;

        /* Update parameters*/
        i += temp_option_header_len + temp_option_len;
        temp_options_ptr += temp_option_len;
        temp_options_count++;
    }

    // Assign options count number
    *out_options_count = temp_options_count;

    return OscoreNoError;
}

OscoreError buf2coap(struct byte_array *in, struct o_coap_packet *out) {
    OscoreError r;
    uint8_t *tmp_p = in->ptr;
    uint16_t payload_len = in->len;

    /* Read CoAP/OSCORE header (4 bytes)*/
    out->header.ver = ((*tmp_p) & HEADER_VERSION_MASK) >> HEADER_VERSION_OFFSET;
    out->header.type = ((*tmp_p) & HEADER_TYPE_MASK) >> HEADER_TYPE_OFFSET;
    out->header.TKL = ((*tmp_p) & HEADER_CODE_MASK) >> HEADER_CODE_OFFSET;
    out->header.code = *(tmp_p + 1);
    out->header.MID = (uint16_t)(*(tmp_p + 2)) << 8 | *(tmp_p + 3);

    /* Update pointer and length*/
    tmp_p += 4;
    payload_len -= 4;

    /*Read the token, if it exists*/
    if (out->header.TKL == 0) {
        out->token = NULL;
    } else if (out->header.TKL <= 8) {
        out->token = tmp_p;
    } else {
        /* ERROR: CoAP token length maximal 8 bytes */
        return OscoreInPktInvalidTKL;
    }
    /* Update pointer and length */
    tmp_p += out->header.TKL;
    payload_len -= out->header.TKL;

    /* Options, if any */
    /* Check if there any options exist*/
    if (*tmp_p == 0xFF || payload_len == 0) {
        /* No options*/
        out->options_cnt = 0;
    } else {
        /* Length of options byte string */
        uint16_t options_len = 0;
        uint8_t *temp_option_ptr = tmp_p;

        /* Move tmp_p to the payload to get the length of options byte string*/
        while (*tmp_p != 0xFF && payload_len != 0) {
            payload_len--;
            tmp_p++;
            options_len++;
        }

        /* Parser all options */
        if (options_len > 0) {
            r = buf2options(temp_option_ptr, options_len, out->options,
                            &(out->options_cnt));
            if (r != OscoreNoError) return r;
        } else {
            out->options_cnt = 0;
        }
    }

    /* Payload, if any */
    ++tmp_p;
    if (payload_len == 0) {
        out->payload_len = 0;
        out->payload = NULL;
    } else {
        out->payload_len = --payload_len;
        out->payload = tmp_p;
    }

    return OscoreNoError;
}

OscoreError coap2buf(
    struct o_coap_packet *in,
    uint8_t *out_byte_string,
    uint16_t *out_byte_string_len) {
    uint8_t *temp_out_ptr = out_byte_string;
    OscoreError r;

    /* First byte in header (version + type + token length) */
    *temp_out_ptr = (in->header.ver << HEADER_VERSION_OFFSET) | (in->header.type << HEADER_TYPE_OFFSET) | (in->header.TKL);
    /* Following 3 bytes in header (1 byte code + 2 bytes message ID)*/
    *(temp_out_ptr + 1) = in->header.code;
    uint16_t temp_MID = in->header.MID;
    *(temp_out_ptr + 2) = (uint8_t)((temp_MID & 0xFF00) >> 8);
    *(temp_out_ptr + 3) = (uint8_t)(temp_MID & 0x00FF);

    temp_out_ptr += 4;
    /* Copy token */
    if (in->header.TKL > 0) {
        uint64_t dest_size = *out_byte_string_len - (temp_out_ptr - out_byte_string);
        r = _memcpy_s(temp_out_ptr, dest_size, in->token, in->header.TKL);
        if (r != OscoreNoError) return r;

        temp_out_ptr += in->header.TKL;
    }

    /* Calculate the maximal length of all options, i.e. all options have two bytes extra delta and length*/
    uint16_t temp_opt_bytes_len = 0;
    for (uint8_t i = 0; i < in->options_cnt; i++)
        temp_opt_bytes_len += 1 + 2 + 2 + in->options[i].len;
    uint8_t temp_opt_bytes[temp_opt_bytes_len];
    memset(temp_opt_bytes, 0, sizeof(temp_opt_bytes));

    struct byte_array option_byte_string = {
        .len = temp_opt_bytes_len,
        .ptr = temp_opt_bytes,
    };

    /* Convert all OSCORE U-options structure into byte string*/
    r = options_into_byte_string(
        in->options,
        in->options_cnt,
        &option_byte_string);
    if (r != OscoreNoError) return r;
    /* Copy options byte string into output*/

    uint64_t dest_size = *out_byte_string_len - (temp_out_ptr - out_byte_string);
    r = _memcpy_s(temp_out_ptr, dest_size, temp_opt_bytes, option_byte_string.len);
    if (r != OscoreNoError) return r;

    temp_out_ptr += option_byte_string.len;

    /* Payload */
    if (in->payload_len != 0) {
        *temp_out_ptr = 0xFF;

        dest_size = *out_byte_string_len - (temp_out_ptr + 1 - out_byte_string);
        r = _memcpy_s(++temp_out_ptr, dest_size, in->payload, in->payload_len);
        if (r != OscoreNoError) return r;
    }
    *out_byte_string_len = 4 + in->header.TKL + option_byte_string.len;
    if (in->payload_len) {
        *out_byte_string_len += 1 + in->payload_len;
    }

    PRINT_ARRAY("Byte string of the converted packet", out_byte_string, *out_byte_string_len);
    return OscoreNoError;
}
