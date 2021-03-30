/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/err_msg.h"

#include <cbor.h>
#include <stdint.h>

#include "../edhoc.h"
#include "../inc/error.h"
#include "../inc/messages.h"
#include "../inc/print_util.h"
#include "../inc/txrx_wrapper.h"

/**
 * @brief   encodes an error message
 * @param   msg_struct contains all messsage fields
 * @param   msg the encoded error message
 */
static inline EdhocError err_msg_crate(
    struct error_msg* msg_struct,
    struct byte_array* msg) {
    CborEncoder err_msg_enc;
    CborError r;
    cbor_encoder_init(&err_msg_enc, msg->ptr, msg->len, 0);

    /*Encode C_x if present*/
    if (msg_struct->c_x.len) {
        r = cbor_encode_byte_string(&err_msg_enc, msg_struct->c_x.ptr, msg_struct->c_x.len);
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    }

    /*Encode ERR_MSG*/
    r = cbor_encode_byte_string(&err_msg_enc, msg_struct->err_msg.ptr, msg_struct->err_msg.len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;

    /*Encode SUITES_R if present*/
    if (msg_struct->suites_r.len) {
        r = cbor_encode_byte_string(&err_msg_enc, msg_struct->suites_r.ptr, msg_struct->suites_r.len);
        if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    }

    /* Get the CBOR length */
    msg->len = cbor_encoder_get_buffer_size(&err_msg_enc, msg->ptr);

    PRINT_ARRAY("Error message (CBOR Sequence)", msg->ptr, msg->len);
    return EdhocNoError;
}

EdhocError tx_err_msg(
    enum role role, uint8_t corr,
    uint8_t* c_x, uint8_t c_x_len,
    uint8_t* err_msg_str, uint8_t err_msg_str_len,
    uint8_t* suites, uint8_t suites_len) {
    EdhocError r;
    uint8_t err_msg_buf[ERR_MSG_DEFAULT_SIZE];
    struct byte_array err_msg = {
        .len = sizeof(err_msg_buf),
        .ptr = err_msg_buf,
    };

    struct error_msg err_struct = {
        .err_msg.ptr = err_msg_str,
        .err_msg.len = err_msg_str_len,
        .suites_r.ptr = suites,
        .suites_r.len = suites_len,
    };

    if ((role == RESPONDER && (corr == 0 || corr == 2)) ||
        (role == INITIATOR && (corr == 0 || corr == 1))) {
        err_struct.c_x.ptr = c_x;
        err_struct.c_x.len = c_x_len;
    } else {
        err_struct.c_x.ptr = NULL;
        err_struct.c_x.len = 0;
    }

    r = err_msg_crate(&err_struct, &err_msg);
    if (r != EdhocNoError) return r;
    return tx(err_msg.ptr, err_msg.len);
}
