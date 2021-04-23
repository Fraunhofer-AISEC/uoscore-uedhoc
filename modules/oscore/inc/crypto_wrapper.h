/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef CRYPTO_WRAPPER_H
#define CRYPTO_WRAPPER_H

#include "error.h"
#include "byte_array.h"

/*Indicates what kind of operation a symmetric cipher will execute*/
enum aes_operation {
	ENCRYPT,
	DECRYPT,
};

/**
 * @brief   aes_ccm_16_64_128 symmetric algorithm
 * @param   op ENCRYPT/DECRYPT
 * @param   in byte array containing the plaintext/ciphertext
 * @param   out byte array containing the plaintext/ciphertext
 * @param   key the key (16 Byte)
 * @param   nonce the nonce (13 Byte)
 * @param   aad data which is only authenticated not encrypted
 * @param   tag outputs the authentication tag in case of encryption. 
 *          In case of decryption the it is an input parameter.
 * @retval  oscore_authentication_error if the authentication fails
 *          else oscore_no_error
 */
enum oscore_error
aes_ccm_16_64_128(enum aes_operation op, struct byte_array *in,
		  struct byte_array *out, struct byte_array *key,
		  struct byte_array *nonce, struct byte_array *aad,
		  struct byte_array *tag);

/**
 * @brief   HKDF funcion used for the derivation of the Common IV, 
 *          Recipient/Sender keys.
 * @param   master_secret the master secret
 * @param   master_salt the master salt
 * @param   info a CBOR structure containing id, id_context, alg_aead, type, L 
 * @param   out the derived Common IV, Recipient/Sender keys
 */
enum oscore_error hkdf_sha_256(struct byte_array *master_secret,
			       struct byte_array *master_salt,
			       struct byte_array *info, struct byte_array *out);

#endif
