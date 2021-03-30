/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef HKDF_INFO_H
#define HKDF_INFO_H

#include "byte_array.h"
#include "error.h"
#include "security_context.h"
#include "supported_algorithm.h"

/**
 * @brief   Returns the length of the encoded HKDF Info as defined by OSCORE
 * @param   id "Sender ID or Recipient ID when deriving keys and the empty 
 *          byte string when deriving the Common IV"
 * @param   id_context ID Context
 * @param   aead_alg AEAD Algorithm
 * @param   type type of operation this HKDF Info is going to be used for
 * @param   out out-pointer containing the length afterwards
 * @return  OscoreError
 */
OscoreError hkdf_info_len(
    struct byte_array *id, 
    struct byte_array *id_context, 
    enum AEAD_algorithm aead_alg, 
    enum derive_type type, 
    uint64_t* out);

/**
 * @brief   Encodes the HKDF Info as defined by OSCORE into the out-array
 * @param   id "Sender ID or Recipient ID when deriving keys and the empty 
 *          byte string when deriving the Common IV"
 * @param   id_context ID Context
 * @param   aead_alg AEAD Algorithm
 * @param   type type of operation this HKDF Info is going to be used for
 * @param   out out-array. Must have a length of exactly the value returned 
 *          in the out-parameter by `hkdf_info_len`.
 * @return  OscoreError
 */
OscoreError create_hkdf_info(
    struct byte_array *id, 
    struct byte_array *id_context, 
    enum AEAD_algorithm aead_alg,
    enum derive_type type, 
    struct byte_array *out);

#endif
