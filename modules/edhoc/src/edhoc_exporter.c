/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include <stdint.h>

#include "../edhoc.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/hkdf_info.h"
#include "../inc/okm.h"
#include "../inc/suites.h"

EdhocError edhoc_exporter(
    enum hash_alg app_hash_alg,
    enum aead_alg app_aead_alg,
    const uint8_t* prk_4x3m,
    uint16_t prk_4x3m_len,
    const uint8_t* th4,
    uint16_t th4_len,
    const char* label,
    uint8_t* out,
    uint16_t out_len) {

    return okm_calc(app_aead_alg, app_hash_alg, label, prk_4x3m, prk_4x3m_len, th4, th4_len, out, out_len);
}