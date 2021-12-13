/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef CIPHERTEXT_H
#define CIPHERTEXT_H
enum ciphertext { CIPHERTEXT2, CIPHERTEXT3, CIPHERTEXT4 };

enum err ciphertext_gen(enum ciphertext ctxt, struct suite *suite,
			uint8_t *id_cred, uint32_t id_cred_len,
			uint8_t *signature_or_mac,
			uint32_t signature_or_mac_len, uint8_t *ead,
			uint32_t ead_len, uint8_t *prk, uint32_t prk_len,
			uint8_t *th, uint32_t th_len, uint8_t *ciphertext,
			uint32_t *ciphertext_len);

enum err ciphertext_decrypt_split(enum ciphertext ctxt, struct suite *suite,
				  uint8_t *prk, uint8_t prk_len, uint8_t *th,
				  uint8_t th_len, uint8_t *ciphertext,
				  uint32_t ciphertext_len, uint8_t *id_cred,
				  uint32_t *id_cred_len,
				  uint8_t *signature_or_mac,
				  uint32_t *signature_or_mac_len, uint8_t *ead,
				  uint32_t *ead_len);

#endif