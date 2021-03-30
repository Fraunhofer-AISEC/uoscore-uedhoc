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
#include <stdint.h>

#include "../edhoc.h"
#include "../inc/cbor_decoder.h"
#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/print_util.h"
#include "../inc/retrieve_cred.h"
#include "../inc/signature_or_mac_msg.h"

EdhocError kid2id_cred(
    const void* _kid, uint8_t kid_len,
    uint8_t* id_cred_x, uint64_t* id_cred_x_len) {
    CborEncoder enc, map;
    CborError r;
    cbor_encoder_init(&enc, id_cred_x, *id_cred_x_len, 0);

    r = cbor_encoder_create_map(&enc, &map, 1);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    r = cbor_encode_int(&map, kid);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    r = cbor_encode_byte_string(&map, _kid, kid_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    cbor_encoder_close_container(&enc, &map);

    /* Get the CBOR length */
    *id_cred_x_len = cbor_encoder_get_buffer_size(&enc, id_cred_x);
    return EdhocNoError;
}

EdhocError plaintext_split(
    uint8_t* ptxt, const uint16_t ptxt_len,
    uint8_t* id_cred_x, uint64_t* id_cred_x_len,
    uint8_t* sign_or_mac, uint64_t* sign_or_mac_len,
    uint8_t* ad, uint64_t* ad_len) {
    EdhocError r;
    CborParser parser;
    CborValue value;
    CborType type, ignore;
    CborError err;
    uint8_t* next_temp_ptr;
    uint8_t* temp_ptr;
    uint32_t temp_len;
    uint64_t id_cred_x_buf_cap = *id_cred_x_len;

    /* Initialization */
    err = cbor_parser_init(ptxt, ptxt_len, 0, &parser, &value);
    if (err != CborNoError) return ErrorDuringCborDecoding;
    /* Get type of input CBOR object */
    type = cbor_value_get_type(&value);

    if (type == CborMapType) {
        /*the first element in plaintext is a ID_CRED_x, which starts with a map */
        /*we move to the label of the map*/
        temp_ptr = ptxt + 1;
        temp_len = ptxt_len - 1;

        int map_label;
        uint64_t map_label_len;

        r = cbor_decoder(
            &next_temp_ptr, temp_ptr, temp_len,
            &map_label, &map_label_len, &ignore);
        if (r != EdhocNoError) return r;
        temp_len -= (next_temp_ptr - temp_ptr);
        temp_ptr = next_temp_ptr;

        switch (map_label) {
            case kid:
                break;
            case x5bag:
                /* If a single certificate is conveyed, it is placed in a CBOR
		 bstr. If multiple certificates are conveyed, a CBOR array of bstrs is used, with each certificate being in its own bstr.*/
                break;
            case x5chain:
                /* If a single certificate is conveyed, it is placed in a CBOR
		 bstr. If multiple certificates are conveyed, a CBOR array of bstrs
		 is used, with each certificate being in its own bstr.*/
                PRINT_MSG("ID_CRED of the other party has label x5chain\n");
                r = cbor_decoder(
                    &next_temp_ptr, temp_ptr, temp_len,
                    id_cred_x, id_cred_x_len, &ignore);
                temp_len -= (next_temp_ptr - temp_ptr);
                temp_ptr = next_temp_ptr;
                r = _memcpy_s(id_cred_x, id_cred_x_buf_cap, ptxt, ptxt_len - temp_len);
                if (r != EdhocNoError) return r;
                *id_cred_x_len = ptxt_len - temp_len;
                break;
            case x5t:
                /* The attribute is an array of two
		 elements.  The first element is an algorithm identifier which is
		 an integer or a string containing the hash algorithm identifier.
		 The algorithm is registered in the "COSE Algorithms" registry The
		 second element is a binary string containing the hash value. */
                PRINT_MSG("ID_CRED of the other party has label x5t\n");
                temp_len--;
                temp_ptr++;
                int alg_id;
                uint64_t alg_id_len;
                uint8_t hash_val[SHA_DEFAULT_SIZE];
                uint64_t hash_val_len = sizeof(hash_val);

                r = cbor_decoder(
                    &next_temp_ptr, temp_ptr, temp_len,
                    &alg_id, &alg_id_len, &ignore);
                if (r != EdhocNoError) return r;
                temp_len -= (next_temp_ptr - temp_ptr);
                temp_ptr = next_temp_ptr;

                r = cbor_decoder(
                    &next_temp_ptr, temp_ptr, temp_len,
                    &hash_val, &hash_val_len, &ignore);
                if (r != EdhocNoError) return r;
                temp_len -= (next_temp_ptr - temp_ptr);
                temp_ptr = next_temp_ptr;

                r = _memcpy_s(id_cred_x, id_cred_x_buf_cap, ptxt, ptxt_len - temp_len);
                *id_cred_x_len = ptxt_len - temp_len;
                break;
            case x5u:
                break;
            default:
                return UnknownIdCredMapLabel;
                break;
        }
    } else {
        /*
        - when ID_CRED_x = {4:kix_x} only kid_x is conveyed and  kid_x is encoded as bstr_identifier.
        - when the kid is longer then one byte it is encoded as byte string
        else as integer
        */
        temp_ptr = ptxt;
        temp_len = ptxt_len;
        PRINT_MSG("ID_CRED of the other party has label kid\n");
        if (type == CborIntegerType) {
            int kid;
            uint64_t kid_len = sizeof(kid);
            r = cbor_decoder(
                &next_temp_ptr, temp_ptr, temp_len,
                &kid, &kid_len, &ignore);
            if (r != EdhocNoError) return r;
            kid += 24;
            r = kid2id_cred(&kid, 1, id_cred_x, id_cred_x_len);
            if (r != EdhocNoError) return r;
        } else {
            uint8_t kid[ID_CRED_DEFAULT_SIZE];
            uint64_t kid_len = sizeof(kid);
            r = cbor_decoder(
                &next_temp_ptr, temp_ptr, temp_len,
                &kid, &kid_len, &ignore);
            if (r != EdhocNoError) return r;
            r = kid2id_cred(&kid, 1, id_cred_x, id_cred_x_len);
            if (r != EdhocNoError) return r;
        }
        temp_len -= (next_temp_ptr - temp_ptr);
        temp_ptr = next_temp_ptr;
    }

    r = cbor_decoder(
        &next_temp_ptr, temp_ptr, temp_len,
        sign_or_mac, sign_or_mac_len, &ignore);
    if (r != EdhocNoError) return r;
    temp_len -= (next_temp_ptr - temp_ptr);
    temp_ptr = next_temp_ptr;

    if (temp_len) {
        r = cbor_decoder(
            &next_temp_ptr, temp_ptr, temp_len,
            ad, ad_len, &ignore);
        if (r != EdhocNoError) return r;
    } else {
        *ad_len = 0;
    }
    return EdhocNoError;
}

EdhocError id_cred2kid(
    uint8_t* id_cred, uint8_t id_cred_len,
    uint8_t* _kid, uint32_t* kid_len) {
    CborParser parser;
    CborValue value;
    CborError err;
    uint8_t* next_temp_ptr;
    uint8_t* temp_ptr;
    uint32_t temp_len;
    CborType ignore;
    EdhocError r;

    /* Initialization */
    err = cbor_parser_init(id_cred, id_cred_len, 0, &parser, &value);
    if (err != CborNoError) return ErrorDuringCborDecoding;

    /*the first element in plaintext is a ID_CRED_x, which starts with a map */
    /*we move to the label of the map*/
    temp_ptr = id_cred + 1;
    temp_len = id_cred_len - 1;

    int map_label;
    uint64_t map_label_len;

    r = cbor_decoder(
        &next_temp_ptr, temp_ptr, temp_len,
        &map_label, &map_label_len, &ignore);
    if (r != EdhocNoError) return r;
    temp_len -= (next_temp_ptr - temp_ptr);
    temp_ptr = next_temp_ptr;

    if (map_label == kid) {
        uint8_t kid_str[KID_DEFAULT_SIZE];
        uint64_t kid_str_len = sizeof(kid_str);
        r = cbor_decoder(
            &next_temp_ptr, temp_ptr, temp_len,
            &kid_str, &kid_str_len, &ignore);
        if (r != EdhocNoError) return r;

        if (kid_str_len == 1) {
            int64_t t = kid_str[0] - 24;
            CborEncoder enc;
            CborError r;
            cbor_encoder_init(&enc, _kid, *kid_len, 0);
            r = cbor_encode_int(&enc, t);
            if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
            *kid_len = 1;
        }

    } else {
        *kid_len = 0;
    }
    return EdhocNoError;
}

EdhocError plaintext_encode(
    uint8_t* id_cred, uint8_t id_cred_len,
    const uint8_t* sgn_or_mac, uint8_t sgn_or_mac_len,
    const uint8_t* ad, uint16_t ad_len,
    uint8_t* plaintext, uint16_t* plaintext_len) {
    EdhocError r;

    uint16_t l, enc_sgn_or_mac_len = sgn_or_mac_len + 2;
    uint8_t kid_buf[KID_DEFAULT_SIZE];
    uint32_t kid_len = sizeof(kid_buf);
    id_cred2kid(id_cred, id_cred_len, kid_buf, &kid_len);

    if (kid_len != 0) {
        /*id cred contains a kid*/
        r = _memcpy_s(plaintext, *plaintext_len, kid_buf, kid_len);
        if (r != EdhocNoError) return r;
        l = kid_len;
    } else {
        r = _memcpy_s(plaintext, *plaintext_len, id_cred, id_cred_len);
        if (r != EdhocNoError) return r;
        l = id_cred_len;
    }

    r = encode_byte_string(sgn_or_mac, sgn_or_mac_len, plaintext + l, &enc_sgn_or_mac_len);
    if (r != EdhocNoError) return r;

    r = _memcpy_s(plaintext + l + enc_sgn_or_mac_len, *plaintext_len - l - enc_sgn_or_mac_len, ad, ad_len);
    if (r != EdhocNoError) return r;

    *plaintext_len = l + enc_sgn_or_mac_len + ad_len;

    return EdhocNoError;
}
