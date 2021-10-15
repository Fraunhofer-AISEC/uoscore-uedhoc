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

#include "../edhoc.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/print_util.h"
#include "../inc/suites.h"

enum edhoc_error prk_derive(bool static_dh_auth, struct suite suite,
			    const uint8_t *prk_in, const uint8_t prk_in_len,
			    const uint8_t *stat_pk, const uint16_t stat_pk_len,
			    const uint8_t *stat_sk, const uint16_t stat_sk_len,
			    uint8_t *prk_out)
{
	enum edhoc_error r;

	if (static_dh_auth) {
		uint8_t dh_secret[ECDH_SECRET_DEFAULT_SIZE];

		r = shared_secret_derive(suite.edhoc_ecdh_curve, stat_sk,
					 stat_sk_len, stat_pk, stat_pk_len,
					 dh_secret);
		if (r != edhoc_no_error)
			return r;
		PRINT_ARRAY("dh_secret", dh_secret, sizeof(dh_secret));
		r = hkdf_extract(suite.edhoc_hash, prk_in, prk_in_len,
				 dh_secret, sizeof(dh_secret), prk_out);
		if (r != edhoc_no_error)
			return r;
	} else {
		/*it is save to do that since prks have the same size*/
		memcpy(prk_out, prk_in, prk_in_len);
	}
	return edhoc_no_error;
}