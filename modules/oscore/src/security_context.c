/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/security_context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/aad.h"
#include "../inc/coap.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/hkdf_info.h"
#include "../inc/memcpy_s.h"
#include "../inc/nonce.h"
#include "../inc/print_util.h"
#include "../oscore.h"

/**
 * @brief       Common derive procedure used to derive the Common IV and 
 *              Sender / Recipient Keys
 * @param cc    pointer to the common context
 * @param id    empty array for Common IV, sender / recipient ID for keys
 * @param type  IV for Common IV, KEY for Sender / Recipient Keys
 * @param out   out-array. Must be initialized
 * @return      OscoreError
 */
static OscoreError derive(struct common_context *cc, struct byte_array *id,
			  enum derive_type type, struct byte_array *out)
{
	OscoreError r;

	// r = hkdf_info_len(id, &cc->id_context, cc->aead_alg, type, &len);
	// if (r != OscoreNoError)
	// 	return r;
	uint8_t info_bytes[30]; //todo use define here
	struct byte_array info = {
		.len = sizeof(info_bytes),
		.ptr = info_bytes,
	};

	r = create_hkdf_info(id, &cc->id_context, cc->aead_alg, type, &info);
	if (r != OscoreNoError)
		return r;

	// uint8_t info_bytes[30];
	// struct byte_array info = {
	// 	.len = sizeof(info_bytes),
	// 	.ptr = info_bytes,
	// };

	// struct info info_struct = {
	// 	._info_id = id,
	// 	._info_type = type };

	// success_encoding = cbor_encode_info(info.ptr, info.len, );

	PRINT_ARRAY("info struct", info.ptr, info.len);

	switch (cc->kdf) {
	case SHA_256:
		r = hkdf_sha_256(&cc->master_secret, &cc->master_salt, &info,
				 out);
		break;
	default:
		r = OscoreUnknownHkdf;
		break;
	}
	return r;
}

/**
 * @brief    Derives the Common IV 
 * @param    cc    pointer to the common context
 * @return   OscoreError
 */
static OscoreError derive_common_iv(struct common_context *cc)
{
	OscoreError r;
	r = derive(cc, &EMPTY_ARRAY, IV, &cc->common_iv);
	PRINT_ARRAY("Common IV", cc->common_iv.ptr, cc->common_iv.len);
	return r;
};

/**
 * @brief    Derives the Sender Key 
 * @param    cc    pointer to the common context
 * @param    sc    pointer to the sender context
 * @return   OscoreError
 */
static OscoreError derive_sender_key(struct common_context *cc,
				     struct sender_context *sc)
{
	OscoreError r;
	r = derive(cc, &sc->sender_id, KEY, &sc->sender_key);
	PRINT_ARRAY("Sender Key", sc->sender_key.ptr, sc->sender_key.len);
	return r;
};

/**
 * @brief    Derives the Recipient Key 
 * @param    cc    pointer to the common context
 * @param    sc    pointer to the recipient context
 * @return   OscoreError
 */
static OscoreError derive_recipient_key(struct common_context *cc,
					struct recipient_context *rc)
{
	OscoreError r;
	r = derive(cc, &rc->recipient_id, KEY, &rc->recipient_key);

	PRINT_ARRAY("Recipient Key", rc->recipient_key.ptr,
		    rc->recipient_key.len);
	return r;
};

OscoreError context_update(enum dev_type dev, struct o_coap_option *options,
			   uint16_t opt_num, struct byte_array *new_piv,
			   struct byte_array *new_kid_context,
			   struct context *c)
{
	OscoreError r = OscoreNoError;

	if (dev == SERVER) {
		/**********************************************************************/
		/*update PIV*/
		r = _memcpy_s(c->rrc.piv.ptr, MAX_PIV_LEN, new_piv->ptr,
			      new_piv->len);
		if (r != OscoreNoError)
			return r;
		c->rrc.piv.len = new_piv->len;

		/**********************************************************************/
		/*update Sender Key, Recipient Key and Common IV if KID context defers 
        from the ID Context*/
		if (!array_equals(&c->cc.id_context, new_kid_context)) {
			/*if the ID Context is equal to the KID_context (the ID_context 
            received with the oscore option) no update of Sender/recipient keys 
            and Common IV required)*/

			/*update KID Context*/
			r = _memcpy_s(c->rrc.kid_context.ptr,
				      MAX_KID_CONTEXT_LEN, new_kid_context->ptr,
				      new_kid_context->len);
			if (r != OscoreNoError)
				return r;

			c->rrc.kid_context.len = new_kid_context->len;

			r = _memcpy_s(c->cc.id_context.ptr,
				      c->cc.id_context.len,
				      new_kid_context->ptr,
				      new_kid_context->len);
			if (r != OscoreNoError)
				return r;
			c->cc.id_context.len = new_kid_context->len;

			PRINT_MSG("Common Context Updated*****************\n");

			r = derive_common_iv(&c->cc);
			if (r != OscoreNoError)
				return r;

			r = derive_sender_key(&c->cc, &c->sc);
			if (r != OscoreNoError)
				return r;

			r = derive_recipient_key(&c->cc, &c->rc);
			if (r != OscoreNoError)
				return r;
		}
	}
	/**************************************************************************/
	/*calculate nonce*/
	r = create_nonce(&c->rrc.kid, &c->rrc.piv, &c->cc.common_iv,
			 &c->rrc.nonce);
	if (r != OscoreNoError)
		return r;

	/**************************************************************************/
	/*calculate AAD*/
	r = aad_length(options, opt_num, c->cc.aead_alg, &c->rrc.kid,
		       &c->rrc.piv, &c->rrc.aad.len);
	if (r != OscoreNoError)
		return r;

	return create_aad(options, opt_num, c->cc.aead_alg, &c->rrc.kid,
			  &c->rrc.piv, &c->rrc.aad);
}

OscoreError oscore_context_init(struct oscore_init_params *params,
				struct context *c)
{
	OscoreError r;

	if (params->dev_type == CLIENT) {
		PRINT_MSG(
			"\n\n\nClient context initialization*********************\n");
	} else {
		PRINT_MSG(
			"\n\n\nServer context initialization*********************\n");
	}

	/*derive common context****************************************************/

	if (params->aead_alg != AES_CCM_16_64_128) {
		return OscoreInvalidAlgorithmAEAD;
	} else {
		c->cc.aead_alg = AES_CCM_16_64_128; /*thats the default*/
	}

	if (params->hkdf != SHA_256) {
		return OscoreInvalidAlgorithmHKDF;
	} else {
		c->cc.kdf = SHA_256; /*thats the default*/
	}

	c->cc.master_secret = params->master_secret;
	c->cc.master_salt = params->master_salt;
	c->cc.id_context = params->id_context;
	c->cc.common_iv.len = sizeof(c->cc.common_iv_buf);
	c->cc.common_iv.ptr = c->cc.common_iv_buf;
	r = derive_common_iv(&c->cc);
	if (r != OscoreNoError)
		return r;

	/*derive Recipient Context*************************************************/
	c->rc.recipient_id = params->recipient_id;
	c->rc.recipient_key.len = sizeof(c->rc.recipient_key_buf);
	c->rc.recipient_key.ptr = c->rc.recipient_key_buf;
	r = derive_recipient_key(&c->cc, &c->rc);
	if (r != OscoreNoError)
		return r;

	/*derive Sender Context****************************************************/
	c->sc.sender_id = params->sender_id;
	c->sc.sender_key.len = sizeof(c->sc.sender_key_buf);
	c->sc.sender_key.ptr = c->sc.sender_key_buf;
	r = derive_sender_key(&c->cc, &c->sc);
	if (r != OscoreNoError)
		return r;

	c->sc.sender_seq_num = 0;

	/*set up the request response context**************************************/
	c->rrc.nonce.len = sizeof(c->rrc.nonce_buf);
	c->rrc.nonce.ptr = c->rrc.nonce_buf;

	c->rrc.aad.len = sizeof(c->rrc.aad_buf);
	c->rrc.aad.ptr = c->rrc.aad_buf;

	c->rrc.piv.len = sizeof(c->rrc.piv_buf);
	c->rrc.piv.ptr = c->rrc.piv_buf;

	c->rrc.kid_context.len = sizeof(c->rrc.kid_context_buf);
	c->rrc.kid_context.ptr = c->rrc.kid_context_buf;

	c->rrc.kid.len = sizeof(c->rrc.kid_buf);
	c->rrc.kid.ptr = c->rrc.kid_buf;

	if (params->dev_type == CLIENT) {
		r = _memcpy_s(c->rrc.kid_context.ptr, c->rrc.kid_context.len,
			      params->id_context.ptr, params->id_context.len);
		if (r != OscoreNoError)
			return r;
		c->rrc.kid_context.len = params->id_context.len;

		r = _memcpy_s(c->rrc.kid.ptr, c->rrc.kid.len,
			      params->sender_id.ptr, params->sender_id.len);
		if (r != OscoreNoError)
			return r;
		c->rrc.kid.len = params->sender_id.len;

		PRINT_ARRAY("KID context", c->rrc.kid_context.ptr,
			    c->rrc.kid_context.len);
	} else {
		r = _memcpy_s(c->rrc.kid.ptr, c->rrc.kid.len,
			      params->recipient_id.ptr,
			      params->recipient_id.len);
		if (r != OscoreNoError)
			return r;
		c->rrc.kid.len = params->recipient_id.len;
	}
	PRINT_ARRAY("KID", c->rrc.kid.ptr, c->rrc.kid.len);
	return OscoreNoError;
}

OscoreError sender_seq_num2piv(uint64_t ssn, struct byte_array *piv)
{
	uint8_t *p = (uint8_t *)&ssn;
	OscoreError r;

	for (int8_t i = 7; i >= 0; i--) {
		if (*(p + i) > 0) {
			r = _memcpy_s(piv->ptr, MAX_PIV_LEN, p, i + 1);
			if (r != OscoreNoError)
				return r;
			piv->len = i + 1;
			return OscoreNoError;
		}
	}

	/*if the sender seq number is 0 piv has value 0 and length 1*/
	*piv->ptr = 0;
	piv->len = 1;
	return OscoreNoError;
}
