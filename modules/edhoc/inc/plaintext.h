/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef PLAINTEXT_H
#define PLAINTEXT_H

#include <cbor.h>
#include <stdint.h>

#include "error.h"


/**
 * @brief   decodes id_cred to kid
 * @param   id_cred ID_CRED_x
 * @param   id_cred_len length of id_cred
 * @param   _kid output pointer
 * @param   kid_len length of the kid
 */
EdhocError id_cred2kid(uint8_t *id_cred, uint8_t id_cred_len, uint8_t *_kid,
		       uint32_t *kid_len);

/**
 * @brief   Splits a the plaintext of message 2 to its subfields
 * @param   ptxt pointer to the paintext
 * @param   ptxt_len length of ptxt
 * @param   id_cred_x ID_CRED_x
 * @param   id_cred_x_len length of id_cred_x_len
 * @param   sign_or_mac signature or mac
 * @param   sign_or_mac_len length of sign_or_mac
 * @param   ad axillary data
 * @param   ad_len length of ad
 */
EdhocError plaintext_split(uint8_t *ptxt, const uint16_t ptxt_len,
			   uint8_t *id_cred_x, uint64_t *id_cred_x_len,
			   uint8_t *sign_or_mac, uint64_t *sign_or_mac_len,
			   uint8_t *ad, uint64_t *ad_len);

/**
 * @brief   Encodes a plaintext 
 * @param   id_cred ID_CRED_x
 * @param   id_cred_len length of id_cred_len
 * @param   sign_or_mac signature or mac
 * @param   sign_or_mac_len length of sign_or_mac
 * @param   ad axillary data
 * @param   ad_len length of ad 
 * @param   paintext pointer to the paintext
 * @param   paintext_len length of paintext
 */
EdhocError plaintext_encode(const uint8_t *id_cred, uint8_t id_cred_len,
			    const uint8_t *sgn_or_mac, uint8_t sgn_or_mac_len,
			    const uint8_t *ad, uint16_t ad_len,
			    uint8_t *plaintext, uint16_t *plaintext_len);


#endif
