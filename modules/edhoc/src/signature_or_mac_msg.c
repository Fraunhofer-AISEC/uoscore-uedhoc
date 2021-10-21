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

#include "../inc/signature_or_mac_msg.h"
#include "../edhoc.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/hkdf_info.h"
#include "../inc/okm.h"
#include "../inc/print_util.h"
#include "../inc/suites.h"
#include "../inc/cose.h"
#include "../inc/memcpy_s.h"
#include "../cbor/encode_enc_structure.h"
#include "../cbor/encode_sig_structure.h"
#include "../cbor/encode_bstr_type.h"
#include "../cbor/decode_bstr_type.h"

enum edhoc_error encode_byte_string(const uint8_t *in, uint32_t in_len,
				    uint8_t *out, uint64_t *out_len)
{
	bool ok;
	size_t payload_len_out;
	cbor_string_type_t tmp;
	tmp.value = in;
	tmp.len = in_len;
	ok = cbor_encode_bstr_type_b_str(out, *out_len, &tmp, &payload_len_out);
	if (!ok) {
		return cbor_encoding_error;
	}
	*out_len = payload_len_out;

	return edhoc_no_error;
}

enum edhoc_error decode_byte_string(const uint8_t *in, const uint32_t in_len,
				    uint8_t *out, uint32_t *out_len)
{
	bool ok;
	cbor_string_type_t str;
	size_t decode_len = 0;
	enum edhoc_error r;

	ok = cbor_decode_bstr_type_b_str(in, in_len, &str, &decode_len);
	if (!ok) {
		return cbor_decoding_error;
	}
	r = _memcpy_s(out, *out_len, str.value, str.len);
	if (r != edhoc_no_error) {
		return r;
	}
	*out_len = str.len;

	return edhoc_no_error;
}

enum edhoc_error mac(const uint8_t *prk, uint8_t prk_len, const uint8_t *th,
		     uint8_t th_len, const uint8_t *id_cred,
		     uint32_t id_cred_len, const uint8_t *cred,
		     uint32_t cred_len, const uint8_t *ead, uint32_t ead_len,
		     const char *mac_label, bool static_dh, struct suite *suite,
		     uint8_t *mac, uint32_t *mac_len)
{
	enum edhoc_error r;
	uint32_t context_mac_len = id_cred_len + cred_len + ead_len;
	uint8_t context_mac[context_mac_len];
	r = _memcpy_s(context_mac, sizeof(context_mac), id_cred, id_cred_len);
	if (r != edhoc_no_error) {
		return r;
	}
	r = _memcpy_s(context_mac + id_cred_len,
		      sizeof(context_mac) - id_cred_len, cred, cred_len);
	if (r != edhoc_no_error) {
		return r;
	}
	r = _memcpy_s(context_mac + id_cred_len + cred_len,
		      sizeof(context_mac) - id_cred_len - cred_len, ead,
		      ead_len);
	if (r != edhoc_no_error) {
		return r;
	}

	PRINT_ARRAY("MAC context", context_mac, context_mac_len);

	if (static_dh) {
		*mac_len = get_mac_len(suite->edhoc_aead);

	} else {
		*mac_len = get_hash_len(suite->edhoc_hash);
	}

	r = okm_calc(suite->edhoc_hash, prk, prk_len, th, th_len, mac_label,
		     context_mac, context_mac_len, mac, *mac_len);
	if (r != edhoc_no_error) {
		return r;
	}

	PRINT_ARRAY("MAC 2/3", mac, *mac_len);
	return edhoc_no_error;
}

static enum edhoc_error
signature_struct_gen(const uint8_t *th, uint8_t th_len, const uint8_t *id_cred,
		     uint32_t id_cred_len, const uint8_t *cred,
		     uint32_t cred_len, const uint8_t *ead, uint32_t ead_len,
		     const uint8_t *mac, uint32_t mac_len, uint8_t *out,
		     uint16_t *out_len)
{
	enum edhoc_error r;
	uint8_t th_enc[th_len + 10];
	uint64_t th_enc_len = sizeof(th_enc);
	r = encode_byte_string(th, th_len, th_enc, &th_enc_len);
	if (r != edhoc_no_error) {
		return r;
	}

	uint8_t tmp[th_enc_len + cred_len + ead_len];
	memcpy(tmp, th_enc, th_enc_len);
	memcpy(tmp + th_enc_len, cred, cred_len);
	if (ead_len != 0) {
		memcpy(tmp + th_enc_len + cred_len, ead, ead_len);
	}

	uint8_t context_str[] = { "Signature1" };
	r = cose_sig_structure_encode(context_str, strlen((char *)context_str),
				      id_cred, id_cred_len, tmp, sizeof(tmp),
				      mac, mac_len, out, out_len);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("COSE_Sign1 object to be signed", out, *out_len);
	return edhoc_no_error;
}

enum edhoc_error
signature_or_mac(enum sgn_or_mac_op op, bool static_dh, struct suite *suite,
		 const uint8_t *sk, uint8_t sk_len, const uint8_t *pk,
		 uint8_t pk_len, const uint8_t *prk, uint8_t prk_len,
		 const uint8_t *th, uint8_t th_len, const uint8_t *id_cred,
		 uint32_t id_cred_len, const uint8_t *cred, uint32_t cred_len,
		 const uint8_t *ead, uint32_t ead_len, const char *mac_label,
		 uint8_t *signature_or_mac, uint32_t *signature_or_mac_len)
{
	enum edhoc_error r;
	if (op == GENERATE) {
		/*we always calculate the mac*/
		r = mac(prk, prk_len, th, th_len, id_cred, id_cred_len, cred,
			cred_len, ead, ead_len, mac_label, static_dh, suite,
			signature_or_mac, signature_or_mac_len);
		if (r != edhoc_no_error) {
			return r;
		}

		if (static_dh) {
			/*signature_or_mac is mac when the caller of this function authenticates with static DH keys*/
			return edhoc_no_error;
		} else {
			uint8_t signature_struct[300];
			uint16_t signature_struct_len =
				sizeof(signature_struct);
			r = signature_struct_gen(th, th_len, id_cred,
						 id_cred_len, cred, cred_len,
						 ead, ead_len, signature_or_mac,
						 *signature_or_mac_len,
						 signature_struct,
						 &signature_struct_len);
			if (r != edhoc_no_error) {
				return r;
			}

			*signature_or_mac_len =
				get_signature_len(suite->edhoc_sign_curve);
			r = sign(suite->edhoc_sign_curve, sk, sk_len, pk,
				 pk_len, signature_struct, signature_struct_len,
				 signature_or_mac, signature_or_mac_len);
			if (r != edhoc_no_error) {
				return r;
			}
			PRINT_ARRAY("signature_or_mac (is signature)",
				    signature_or_mac, *signature_or_mac_len);
		}
	} else { /*we verify here*/
		uint32_t mac_buf_len = get_hash_len(suite->edhoc_hash);
		uint8_t mac_buf[mac_buf_len];

		r = mac(prk, prk_len, th, th_len, id_cred, id_cred_len, cred,
			cred_len, ead, ead_len, mac_label, static_dh, suite,
			mac_buf, &mac_buf_len);
		if (r != edhoc_no_error) {
			return r;
		}
		if (static_dh) {
			/*signature_or_mac is mac when the caller of this function authenticates with static DH keys*/

			if (0 != memcmp(mac_buf, signature_or_mac,
					*signature_or_mac_len)) {
				return mac_authentication_failed;
			}

		} else {
			uint8_t signature_struct[300];
			uint16_t signature_struct_len =
				sizeof(signature_struct);
			r = signature_struct_gen(th, th_len, id_cred,
						 id_cred_len, cred, cred_len,
						 ead, ead_len, mac_buf,
						 mac_buf_len, signature_struct,
						 &signature_struct_len);
			if (r != edhoc_no_error) {
				return r;
			}

			bool result;
			r = verify(suite->edhoc_sign_curve, pk, pk_len,
				   signature_struct, signature_struct_len,
				   signature_or_mac, *signature_or_mac_len,
				   &result);
			if (r != edhoc_no_error) {
				return r;
			}
			if (!result) {
				return signature_authentication_failed;
			}
			PRINT_MSG(
				"Signiture or MAC verification successfull!\n");
		}
	}
	return edhoc_no_error;
}

