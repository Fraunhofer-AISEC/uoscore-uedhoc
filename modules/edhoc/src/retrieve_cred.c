/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/retrieve_cred.h"

#include <cbor.h>
#include <inttypes.h>
#include <string.h>

#include "../edhoc.h"
#include "../inc/cbor_decoder.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/print_util.h"

/**
 * @brief   Verifies an certificate
 * @param   cert a native CBOR encoded certificate
 * @param   cer_len the length of the certificate
 * @param   cred_array an array containing credentials 
 * @param   cred_num number of elements in cred_array
 * @param   pk public key contained in the certificate
 * @param   pk_len the length pk
 * @param   verified true if verification successfull
 */
static EdhocError cert_verify(uint8_t *cert, uint16_t cert_len,
			      const struct other_party_cred *cred_array,
			      uint16_t cred_num, uint8_t **pk, uint16_t *pk_len,
			      bool *verified)
{
	CborParser parser;
	CborValue value;
	CborError err;
	uint64_t len = 0;
	uint32_t offset = 0;

	uint8_t signature[SGN_OR_MAC_DEFAULT_SIZE];
	uint64_t signature_len = sizeof(signature);

	PRINT_MSG("start parsing a CBOR certificate\n");

	/*1) type encoded as int in single byte */
	err = cbor_parser_init(cert, cert_len, 0, &parser, &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	int64_t type;
	err = cbor_value_get_int64(&value, &type);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	PRINTF("%" PRId64 "\n", type);
	offset++;

	/*2) serial number as bytestring */
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;

	err = cbor_value_calculate_string_length(&value, (size_t *)&len);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	offset++;
	PRINT_ARRAY("serial number", cert + offset, len);
	offset += len;

	/*3) issuer as bytestring */
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;

	err = cbor_value_calculate_string_length(&value, (size_t *)&len);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	offset++;
	PRINT_ARRAY("issuer", cert + offset, len);
	/*get the CAs public key*/
	uint8_t *root_pk = NULL;
	uint8_t root_pk_len = 0;
	for (uint16_t i = 0; i < cred_num; i++) {
		if (memcmp(cred_array[i].ca.ptr, (cert + offset),
			   cred_array[i].ca.len)) {
			root_pk = cred_array[i].ca_pk.ptr;
			root_pk_len = cred_array[i].ca_pk.len;
			PRINT_ARRAY("Root PK of the CA", root_pk, root_pk_len);
			break;
		}
		return NoSuchCA;
	}
	offset += len;

	/*4) validity_notBefore as uint */
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	uint64_t notBefore;
	cbor_value_get_uint64(&value, &notBefore);
	offset += 5;

	/*5)  validity_notAfter as uint,*/
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	uint64_t notAfter;
	cbor_value_get_uint64(&value, &notAfter);
	offset += 5;

	/*6) subject*/
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	err = cbor_value_calculate_string_length(&value, (size_t *)&len);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	offset++;
	PRINT_ARRAY("subject", cert + offset, len);
	offset += len;

	/*7) pk*/
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	err = cbor_value_calculate_string_length(&value, (size_t *)&len);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	offset += 2;
	*pk = cert + offset;
	*pk_len = len;
	PRINT_ARRAY("pk", *pk, *pk_len);
	offset += len;

	/*8)extentions*/
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	int64_t ext;
	cbor_value_get_int64(&value, &ext);
	PRINT_ARRAY("extensions", (uint8_t *)&ext, sizeof(ext));
	offset++;

	/*9)signature*/
	err = cbor_parser_init(cert + offset, cert_len - offset, 0, &parser,
			       &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	err = cbor_value_calculate_string_length(&value, (size_t *)&len);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	if (len > signature_len)
		return CborByteStringBufferToSmall;
	signature_len = len;
	err = cbor_value_copy_byte_string(&value, signature,
					  (size_t *)&signature_len, &value);

	PRINT_ARRAY("signature", signature, signature_len);
	return verify(Ed25519_SIGN, root_pk, root_pk_len, cert,
		      cert_len - 2 - signature_len, signature, signature_len,
		      verified);
}

EdhocError retrieve_cred(bool static_dh_auth,
			 struct other_party_cred *cred_array, uint16_t cred_num,
			 uint8_t *id_cred, uint8_t id_cred_len, uint8_t **cred,
			 uint16_t *cred_len, uint8_t **pk, uint16_t *pk_len,
			 uint8_t **g, uint16_t *g_len)
{
	EdhocError r1;
	CborParser parser;
	CborValue value;
	CborType type, ignore;
	CborError err;
	uint8_t *next_temp_ptr;
	uint8_t *temp_ptr;
	uint32_t temp_len;

	/*check first if the credential is preestablished (RPK)*/
	for (uint16_t i = 0; i < cred_num; i++) {
		if (cred_array[i].id_cred.len == id_cred_len) {
			if (0 == memcmp(cred_array[i].id_cred.ptr, id_cred,
					id_cred_len)) {
				*cred = cred_array[i].cred.ptr;
				*cred_len = cred_array[i].cred.len;
				if (static_dh_auth) {
					*pk_len = 0;
					*g = cred_array[i].g.ptr;
					*g_len = cred_array[i].g.len;
				} else {
					*g_len = 0;
					*pk = cred_array[i].pk.ptr;
					*pk_len = cred_array[i].pk.len;
				}
				return EdhocNoError;
			}
		}
	}

	/* if the credential is not preestablished a certificate may be contained in the ID_CRED_x */

	/* Initialization */
	err = cbor_parser_init(id_cred, id_cred_len, 0, &parser, &value);
	if (err != CborNoError)
		return ErrorDuringCborDecoding;
	/* Get type of input CBOR object */
	type = cbor_value_get_type(&value);

	if (type == CborMapType) {
		/*the first element in plaintext is a ID_CRED_x, which starts with a map */
		/*we move to the label of the map*/
		temp_ptr = id_cred + 1;
		temp_len = id_cred_len - 1;

		int map_label;
		uint64_t map_label_len;

		r1 = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
				  &map_label, &map_label_len, &ignore);
		temp_len -= (next_temp_ptr - temp_ptr);
		temp_ptr = next_temp_ptr;

		uint8_t cert[CERT_DEFAUT_SIZE];
		uint64_t cert_len = sizeof(cert);
		switch (map_label) {
		case x5bag:
			/*implement the x5bag parsing here*/
			break;
		case x5chain:
			r1 = cbor_decoder(&next_temp_ptr, temp_ptr, temp_len,
					  cert, &cert_len, &ignore);
			if (r1 != EdhocNoError)
				return r1;
			PRINT_ARRAY("ID_CRED_x contains certificate", cert,
				    cert_len);
			*cred = temp_ptr + 2;
			*cred_len = cert_len;
			bool verified;

			if (static_dh_auth) {
				*pk_len = 0;
				r1 = cert_verify(*cred, *cred_len, cred_array,
						 cred_num, g, g_len, &verified);
			} else {
				*g_len = 0;
				r1 = cert_verify(*cred, *cred_len, cred_array,
						 cred_num, pk, pk_len,
						 &verified);
			}

			if (r1 != EdhocNoError)
				return r1;

			if (verified) {
				PRINT_MSG(
					"Certificate verification successful!\n");
				return EdhocNoError;
			} else {
				return CertificateAuthenticationFailed;
			}
			break;
		}
	}

	return CredentialNotFound;
}
