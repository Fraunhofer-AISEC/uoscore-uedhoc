/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include <string.h>

#include "../inc/aad.h"
#include "../inc/hkdf_info.h"

#include <cbor.h>

/*
HKDF = composition of HKDF-Extract and HKDF-Expand (RFC5869)
output = HKDF(salt, IKM, info, L
* salt = Master Salt
* IKM = Master Secret
* info = CBOR-array [
     id: bstr,
     alg_aead: int / tstr,
     type: tstr,
     L: uint,
  ]
     + id: SenderID / RecipientID for keys; empty string for CommonIV
     + alg_aead: AEAD Algorithm
     + type: "Key" / "IV", ascii string without nul-terminator
     + L: size of key/iv for AEAD alg
         - in bytes
* https://www.iana.org/assignments/cose/cose.xhtml
*/
OscoreError hkdf_info_len(
    struct byte_array *id, 
    struct byte_array *id_context, 
    enum AEAD_algorithm aead_alg, 
    enum derive_type type, 
    uint64_t* out) {
    CborEncoder enc;
    cbor_encoder_init(&enc, NULL, 0, 0);

    CborEncoder array_enc;
    char type_enc[10];
    uint64_t len = 0;
    switch (type) {
        case KEY:
            strncpy(type_enc, "Key", 10);
            len = 16;
            break;
        case IV:
            strncpy(type_enc, "IV", 10);
            len = 13;
            break;
        default:
            break;
    }

    cbor_encoder_create_array(&enc, &array_enc, 1);

    cbor_encode_byte_string(&array_enc, id->ptr, id->len);

    if (id_context->len == 0) {
        cbor_encode_null(&array_enc);

    } else {
        cbor_encode_byte_string(&array_enc, id_context->ptr, id_context->len);
    }

    cbor_encode_int(&array_enc, aead_alg);
    cbor_encode_text_stringz(&array_enc, type_enc);
    cbor_encode_uint(&array_enc, len);
    cbor_encoder_close_container(&enc, &array_enc);

    *out = cbor_encoder_get_extra_bytes_needed(&enc);

    return OscoreNoError;
}

inline OscoreError create_hkdf_info(
    struct byte_array *id, 
    struct byte_array *id_context, 
    enum AEAD_algorithm aead_alg, 
    enum derive_type type, 
    struct byte_array *out) {
    CborEncoder enc;
    cbor_encoder_init(&enc, out->ptr, out->len, 0);
    CborEncoder array_enc;
    char type_enc[10];
    uint64_t len = 0;
    switch (type) {
        case KEY:
            strncpy(type_enc, "Key", 10);
            len = 16;
            break;
        case IV:
            strncpy(type_enc, "IV", 10);
            len = 13;
            break;
        default:
            break;
    }

    cbor_encoder_create_array(&enc, &array_enc, 5);
    cbor_encode_byte_string(&array_enc, id->ptr, id->len);

    if (id_context->len == 0) {
        cbor_encode_null(&array_enc);
    } else {
        cbor_encode_byte_string(&array_enc, id_context->ptr, id_context->len);
    }

    cbor_encode_int(&array_enc, aead_alg);
    cbor_encode_text_stringz(&array_enc, type_enc);
    cbor_encode_uint(&array_enc, len);
    cbor_encoder_close_container(&enc, &array_enc);

    return OscoreNoError;
}
