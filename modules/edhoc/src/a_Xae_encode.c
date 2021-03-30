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

#include "../inc/error.h"
#include "../inc/print_util.h"

EdhocError a_Xae_encode(
    uint8_t* thX, const uint16_t thX_len,
    uint8_t* A_Xae, uint32_t* A_Xae_len) {
    unsigned char tmp;
    char str[] = {"Encrypt0"};
    CborEncoder enc, arrayEnc;
    CborError r;
    cbor_encoder_init(&enc, A_Xae, *A_Xae_len, 0);
    cbor_encoder_create_array(&enc, &arrayEnc, 3);

    r = cbor_encode_text_string(&arrayEnc, str, sizeof(str) - 1);
    if (r != CborNoError) return ErrorDuringA3aeEncoding;
    r = cbor_encode_byte_string(&arrayEnc, &tmp, 0);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    r = cbor_encode_byte_string(&arrayEnc, thX, thX_len);
    if (r == CborErrorOutOfMemory) return CborEncodingBufferToSmall;
    cbor_encoder_close_container(&enc, &arrayEnc);

    /* Get the CBOR length */
    *A_Xae_len = cbor_encoder_get_buffer_size(&enc, A_Xae);
    return EdhocNoError;
}
