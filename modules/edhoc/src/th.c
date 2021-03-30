/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/th.h"

#include <cbor.h>

#include "../edhoc.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/print_util.h"

/**
 * @brief   Setups a data structure used as input for th2
 * @param   msg1 pointer to a message 1
 * @param   msg1_len length of message 1
 * @param   c_i Pointer to the conception identifier of the initiator
 * @param   g_y Pointer to the public DH parameter
 * @param   c_r Pointer to the conception identifier of the responder
 * @param   th2_input ouput buffer for the data structure
 * @param   th2_input_len length of th2_input
 */
static inline EdhocError th2_input_encode(
    uint8_t* msg1, uint32_t msg1_len,
    uint8_t* c_i, uint32_t c_i_len,
    uint8_t* g_y, uint32_t g_y_len,
    uint8_t* c_r, uint32_t c_r_len,
    uint8_t* th2_input,
    uint16_t* th2_input_len) {
    CborEncoder enc;
    CborError r;
    EdhocError err;

    err = _memcpy_s(th2_input, *th2_input_len, msg1, msg1_len);
    if (err != EdhocNoError) return err;

    cbor_encoder_init(&enc, (th2_input + msg1_len), (*th2_input_len - msg1_len), 0);

    /*Encode C_I if present*/
    if (c_i_len) {
        r = cbor_encode_byte_string(&enc, c_i, c_i_len);
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    }

    /*Encode G_Y*/
    r = cbor_encode_byte_string(&enc, g_y, g_y_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    /*Encode C_R as bstr_identifier*/
    if (c_r_len == 1) {
        r = cbor_encode_int(&enc, (*c_r - 24));
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    } else {
        r = cbor_encode_byte_string(&enc, c_r, c_r_len);
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    }

    /* Get the CBOR length */
    *th2_input_len = cbor_encoder_get_buffer_size(&enc, th2_input);

    PRINT_ARRAY("Input to calculate TH_2 (CBOR Sequence)", th2_input, *th2_input_len);
    return EdhocNoError;
}

/**
 * @brief   Setups a data structure used as input for th3
 * @param   th2 pointer to a th2
 * @param   th2_len length of th2
 * @param   ciphertext_2 
 * @param   ciphertext_2_len  length of ciphertext_2_len
 * @param   data_3 
 * @param   data_3_len  length of data_3_len
 * @param   th3_input ouput buffer for the data structure
 * @param   th3_input_len length of th3_input
 */
static inline EdhocError th3_input_encode(
    uint8_t* th2, uint8_t th2_len,
    uint8_t* ciphertext_2, uint16_t ciphertext_2_len,
    uint8_t* data_3, uint8_t data_3_len,
    uint8_t* th3_input, uint16_t* th3_input_len) {
    CborEncoder enc;
    CborError r;

    cbor_encoder_init(&enc, th3_input, *th3_input_len, 0);

    /*Encode th2*/
    r = cbor_encode_byte_string(&enc, th2, th2_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    /*Encode ciphertext_2*/
    r = cbor_encode_byte_string(&enc, ciphertext_2, ciphertext_2_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    /*Encode C_R*/
    if (data_3_len == 1) {
        r = cbor_encode_int(&enc, (*data_3 - 24));
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    } else {
        r = cbor_encode_byte_string(&enc, data_3, data_3_len);
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    }

    /* Get the CBOR length */
    *th3_input_len = cbor_encoder_get_buffer_size(&enc, th3_input);

    PRINT_ARRAY("Input to calculate TH_3 (CBOR Sequence)", th3_input, *th3_input_len);
    return EdhocNoError;
}

/**
 * @brief   Setups a data structure used as input for th4
 * @param   th3 pointer to a th3
 * @param   th3_len length of th3
 * @param   ciphertext_3
 * @param   ciphertext_3_len  length of ciphertext_3_len
 * @param   th4_input ouput buffer for the data structure
 * @param   th4_input_len length of th4_input
 */
static inline EdhocError th4_input_encode(
    uint8_t* th3, uint8_t th3_len,
    uint8_t* ciphertext_3, uint16_t ciphertext_3_len,
    uint8_t* th4_input, uint64_t* th4_input_len) {
    CborEncoder enc;
    CborError r;

    cbor_encoder_init(&enc, th4_input, *th4_input_len, 0);

    /*Encode th2*/
    r = cbor_encode_byte_string(&enc, th3, th3_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    /*Encode ciphertext_2*/
    r = cbor_encode_byte_string(&enc, ciphertext_3, ciphertext_3_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    /* Get the CBOR length */
    *th4_input_len = cbor_encoder_get_buffer_size(&enc, th4_input);

    PRINT_ARRAY("Input to calculate TH_4 (CBOR Sequence)", th4_input, *th4_input_len);
    return EdhocNoError;
}

EdhocError th2_calculate(
    enum hash_alg alg,
    uint8_t* msg1, uint32_t msg1_len,
    uint8_t* c_i, uint32_t c_i_len,
    uint8_t* g_y, uint32_t g_y_len,
    uint8_t* c_r, uint32_t c_r_len,
    uint8_t* th2) {
    uint8_t th2_input[TH_INPUT_DEFAULT_SIZE];
    uint16_t th2_input_len = sizeof(th2_input);
    EdhocError r;

    PRINT_ARRAY("msg1", msg1, msg1_len);
    PRINT_ARRAY("c_r", c_r, c_r_len);
    PRINT_ARRAY("msg1", msg1, 37);
    PRINT_ARRAY("c_r", c_r, 1);

    r = th2_input_encode(
        msg1, msg1_len,
        c_i, c_i_len,
        g_y, g_y_len,
        c_r, c_r_len,
        th2_input, &th2_input_len);
    if (r != EdhocNoError) return r;

    r = hash(alg, th2_input, th2_input_len, th2);
    if (r != EdhocNoError) return r;
    PRINT_ARRAY("TH2", th2, SHA_DEFAULT_SIZE);
    return EdhocNoError;
}

EdhocError th3_calculate(
    enum hash_alg alg,
    uint8_t* th2, uint8_t th2_len,
    uint8_t* ciphertext_2, uint16_t ciphertext_2_len,
    uint8_t* data_3, uint8_t data_3_len,
    uint8_t* th3) {
    uint8_t th3_input[th2_len + ciphertext_2_len + data_3_len + 6];
    uint16_t th3_input_len = sizeof(th3_input);
    EdhocError r = th3_input_encode(
        th2, th2_len,
        ciphertext_2, ciphertext_2_len,
        data_3, data_3_len,
        th3_input, &th3_input_len);
    if (r != EdhocNoError) return r;
    r = hash(alg, th3_input, th3_input_len, th3);
    if (r != EdhocNoError) return r;
    PRINT_ARRAY("TH3", th3, 32);
    return EdhocNoError;
}

EdhocError th4_calculate(
    enum hash_alg alg,
    uint8_t* th3, uint8_t th3_len,
    uint8_t* ciphertext_3, uint16_t ciphertext_3_len,
    uint8_t* th4) {
    uint8_t th4_input[th3_len + ciphertext_3_len + 4];
    uint64_t th4_input_len = sizeof(th4_input);

    EdhocError r = th4_input_encode(
        th3, th3_len,
        ciphertext_3, ciphertext_3_len,
        th4_input, &th4_input_len);
    if (r != EdhocNoError) return r;

    r = hash(alg, th4_input, th4_input_len, th4);
    if (r != EdhocNoError) return r;
    PRINT_ARRAY("TH4", th4, SHA_DEFAULT_SIZE);
    return EdhocNoError;
}
