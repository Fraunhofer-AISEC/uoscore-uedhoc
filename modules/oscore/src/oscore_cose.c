/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/oscore_cose.h"

#include <cbor.h>
#include <stdio.h>

#include "../inc/crypto_wrapper.h"
#include "../inc/print_util.h"
#include "../inc/security_context.h"

OscoreError create_enc_structure(struct byte_array* external_aad,
                                 struct byte_array* out) {
    CborEncoder enc;
    cbor_encoder_init(&enc, out->ptr, out->len, 0);
    CborEncoder array_enc;
    cbor_encoder_create_array(&enc, &array_enc, 3);
    /* context */
    cbor_encode_text_stringz(&array_enc, "Encrypt0");
    /* protected */
    cbor_encode_byte_string(&array_enc, NULL, 0);
    /* external_aad */
    cbor_encode_byte_string(&array_enc, external_aad->ptr, external_aad->len);
    cbor_encoder_close_container(&enc, &array_enc);

    return OscoreNoError;
}

OscoreError enc_structure_length(struct byte_array* external_aad, size_t* out) {
    CborEncoder enc;
    cbor_encoder_init(&enc, NULL, 0, 0);
    CborEncoder array_enc;
    cbor_encoder_create_array(&enc, &array_enc, 3);
    /* context */
    cbor_encode_text_stringz(&array_enc, "Encrypt0");
    /* protected */
    cbor_encode_byte_string(&array_enc, NULL, 0);
    /* external_aad */
    cbor_encode_byte_string(&array_enc, external_aad->ptr, external_aad->len);
    cbor_encoder_close_container(&enc, &array_enc);
    *out = cbor_encoder_get_extra_bytes_needed(&enc);
    return OscoreNoError;
}

OscoreError cose_decrypt(
    struct byte_array* in_ciphertext,
    struct byte_array* out_plaintext,
    struct byte_array* nonce,
    struct byte_array* recipient_aad,
    struct byte_array* key) {
    /* get enc_structure */
    OscoreError r;
    size_t aad_len;
    r = enc_structure_length(recipient_aad, &aad_len);
    if (r != OscoreNoError) return r;

    uint8_t aad_bytes[aad_len];
    struct byte_array aad = {
        .len = aad_len,
        .ptr = aad_bytes,
    };
    r = create_enc_structure(recipient_aad, &aad);
    if (r != OscoreNoError) return r;
    PRINT_ARRAY("AAD encoded", aad.ptr, aad.len);

    struct byte_array tag = {
        .len = 8,
        .ptr = in_ciphertext->ptr + in_ciphertext->len - 8};

    PRINT_ARRAY("Ciphertext", in_ciphertext->ptr, in_ciphertext->len);

    r = aes_ccm_16_64_128(
        DECRYPT,
        in_ciphertext,
        out_plaintext,
        key,
        nonce,
        &aad,
        &tag);

    if (r != OscoreNoError) return r;

    PRINT_ARRAY("Decrypted plaintext", out_plaintext->ptr, out_plaintext->len);
    return r;
}

OscoreError cose_encrypt(
    struct byte_array* in_plaintext,
    uint8_t* out_ciphertext, uint32_t out_ciphertext_len,
    struct byte_array* nonce,
    struct byte_array* sender_aad, struct byte_array* key) {
    /* get enc_structure  */
    OscoreError r;
    size_t aad_len;
    r = enc_structure_length(sender_aad, &aad_len);
    if (r != OscoreNoError) return r;
    uint8_t aad_bytes[aad_len];
    struct byte_array aad = {
        .len = aad_len,
        .ptr = aad_bytes,
    };
    r = create_enc_structure(sender_aad, &aad);
    if (r != OscoreNoError) return r;
    struct byte_array tag = {
        .len = 8,
        .ptr = out_ciphertext + in_plaintext->len,
    };

    struct byte_array ctxt = {
        .len = out_ciphertext_len,
        .ptr = out_ciphertext,
    };
    r = aes_ccm_16_64_128(ENCRYPT, in_plaintext, &ctxt, key, nonce, &aad, &tag);
    if (r != OscoreNoError) return r;

    PRINT_ARRAY("Ciphertext", out_ciphertext, out_ciphertext_len);
    return OscoreNoError;
}
