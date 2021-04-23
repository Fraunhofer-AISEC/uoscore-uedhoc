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
#include <stdio.h>
#include <string.h>

#include "../inc/aad.h"
#include "../inc/byte_array.h"
#include "../inc/coap.h"
#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/nonce.h"
#include "../inc/option.h"
#include "../inc/oscore_cose.h"
#include "../inc/print_util.h"
#include "../inc/security_context.h"

/**
 * @brief Extract input CoAP options into E(encrypted) and U(unprotected)
 * @param in_o_coap: input CoAP packet
 * @param e_options: output pointer to E-options
 * @param e_options_cnt: count number of output E-options
 * @param e_options_len: Byte string length of all E-options, which will be used when forming E-options into plaintext
 * @param U_options: output pointer to U-options
 * @param U_options_cnt: count number of output U-options
 * @return oscore_error
 *
 */
static inline enum oscore_error
e_u_options_extract(struct o_coap_packet *in_o_coap,
		    struct o_coap_option *e_options, uint8_t *e_options_cnt,
		    uint16_t *e_options_len, struct o_coap_option *U_options,
		    uint8_t *U_options_cnt)
{
	enum oscore_error r = oscore_no_error;

	/* Initialize to 0 */
	*e_options_len = 0;

	uint16_t temp_option_nr = 0;
	uint16_t temp_len = 0;
	uint8_t temp_E_option_delta_sum = 0;
	uint8_t temp_U_option_delta_sum = 0;
	uint8_t delta_extra_bytes = 0;
	uint8_t len_extra_bytes = 0;

	for (uint8_t i = 0; i < in_o_coap->options_cnt; i++) {
		delta_extra_bytes = 0;
		len_extra_bytes = 0;

		temp_option_nr += in_o_coap->options[i].delta;
		temp_len = in_o_coap->options[i].len;

		/* Calculate extra byte length of option delta and option length */
		if (in_o_coap->options[i].delta > 13 &&
		    in_o_coap->options[i].delta < 243)
			delta_extra_bytes = 1;
		else if (in_o_coap->options[i].delta >= 243)
			delta_extra_bytes = 2;
		if (in_o_coap->options[i].len > 13 &&
		    in_o_coap->options[i].len < 243)
			len_extra_bytes = 1;
		else if (in_o_coap->options[i].len >= 243)
			len_extra_bytes = 2;

		/* check delta, whether current option U or E */
		if (is_class_e(temp_option_nr) == 1) {
			/* E-options, which will be copied in plaintext to be encrypted*/
			e_options[*e_options_cnt].delta =
				temp_option_nr - temp_E_option_delta_sum;
			e_options[*e_options_cnt].len = temp_len;
			e_options[*e_options_cnt].value =
				in_o_coap->options[i].value;
			e_options[*e_options_cnt].option_number =
				temp_option_nr;

			/* Update delta sum of E-options */
			temp_E_option_delta_sum +=
				e_options[*e_options_cnt].delta;

			/* Increment E-options count */
			(*e_options_cnt)++;
			/* Add option header length and value length */
			(*e_options_len) += 1 + delta_extra_bytes +
					    len_extra_bytes + temp_len;
		} else {
			/* U-options */
			U_options[*U_options_cnt].delta =
				temp_option_nr - temp_U_option_delta_sum;
			U_options[*U_options_cnt].len = temp_len;
			U_options[*U_options_cnt].value =
				in_o_coap->options[i].value;
			U_options[*U_options_cnt].option_number =
				temp_option_nr;

			/* Update delta sum of E-options */
			temp_U_option_delta_sum +=
				U_options[*U_options_cnt].delta;

			/* Increment E-options count */
			(*U_options_cnt)++;
		}
	}
	return r;
}

/**
 * @brief Build up plaintext which should be encrypted and protected
 * @param in_o_coap: input CoAP packet that will be analyzed
 * @param E_options: E-options, which should be protected
 * @param E_options_cnt: count number of E-options
 * @param plaintext: output plaintext, which will be encrypted
 * @return oscore_error
 *
 */
static inline enum oscore_error plaintext_setup(struct o_coap_packet *in_o_coap,
						struct o_coap_option *E_options,
						uint8_t E_options_cnt,
						struct byte_array *plaintext)
{
	enum oscore_error r;
	uint8_t *temp_plaintext_ptr = plaintext->ptr;

	/* Add code to plaintext */
	*temp_plaintext_ptr = in_o_coap->header.code;

	/* Calculate the maximal length of all options, i.e. all options have two bytes extra delta and length */
	uint16_t temp_opt_bytes_len = 0;
	for (uint8_t i = 0; i < E_options_cnt; i++)
		temp_opt_bytes_len += 1 + 2 + 2 + E_options[i].len;
	/* Setup buffer */
	uint8_t temp_opt_bytes[temp_opt_bytes_len];
	memset(temp_opt_bytes, 0, temp_opt_bytes_len);
	struct byte_array E_option_byte_string = {
		.len = temp_opt_bytes_len,
		.ptr = temp_opt_bytes,
	};

	/* Convert all E-options structure to byte string, and copy it to output*/
	r = options_into_byte_string(E_options, E_options_cnt,
				     &E_option_byte_string);
	if (r != oscore_no_error)
		return r;

	uint64_t dest_size =
		plaintext->len - (temp_plaintext_ptr + 1 - plaintext->ptr);
	r = _memcpy_s(++temp_plaintext_ptr, dest_size, temp_opt_bytes,
		      E_option_byte_string.len);
	if (r != oscore_no_error)
		return r;
	temp_plaintext_ptr += E_option_byte_string.len;

	/* Add payload to plaintext*/
	if (in_o_coap->payload_len != 0) {
		/* An extra byte 0xFF before payload*/
		*temp_plaintext_ptr = 0xff;

		dest_size = plaintext->len -
			    (temp_plaintext_ptr + 1 - plaintext->ptr);
		r = _memcpy_s(++temp_plaintext_ptr, dest_size,
			      in_o_coap->payload, in_o_coap->payload_len);
		if (r != oscore_no_error)
			return r;
	}
	PRINT_ARRAY("Plain text", plaintext->ptr, plaintext->len);
	return oscore_no_error;
}

/**
 * @brief   Encrypt incoming plaintext
 * @param   c OSCORE context
 * @param   in_o_coap: input CoAP packet, which will be used to calculate AAD
 *          (additional authentication data)
 * @param   in_plaintext: input plaintext that will be encrypted
 * @param   out_ciphertext: output ciphertext, which contains the encrypted data
 * @return  oscore_error
 *
 */
static inline enum oscore_error
plaintext_encrypt(struct context *c, struct o_coap_packet *in_o_coap,
		  struct byte_array *in_plaintext, uint8_t *out_ciphertext,
		  uint32_t out_ciphertext_len)
{
	return cose_encrypt(in_plaintext, out_ciphertext, out_ciphertext_len,
			    &c->rrc.nonce, &c->rrc.aad, &c->sc.sender_key);
}

/**
 * @brief   OSCORE option value length
 * @param   piv set to the sender sequence number in requests or NULL in 
 *          responses
 * @param   kid set to Sender ID in requests or NULL in responses
 * @param   kid_context set to ID context in request when present. If not present or a response set to NULL
 * @return  length of the OSCORE option value
 */
static inline uint16_t get_oscore_opt_val_len(struct byte_array *piv,
					      struct byte_array *kid,
					      struct byte_array *kid_context)
{
	uint16_t l;
	l = piv->len + kid_context->len + kid->len;
	if (l) {
		/*if any of piv, kit_context or kit is present 1 byte for the flags is reserved */
		l++;
	}
	if (kid_context->len) {
		/*if kit_context is present one byte is reserved for the s field*/
		l++;
	}
	return l;
}

/**
 * @brief   Generate an OSCORE option. The oscore option value length must 
 *          be calculated before this function is called and set in 
 *          oscore_option.len. In addition oscore_option.val pointer should
 *          be set to a buffer with length oscore_option.len. 
 * @param   piv set to the trimmed sender sequence number in requests or NULL 
 *          in responses
 * @param   kid set to Sender ID in requests or NULL in responses
 * @param   kid_context set to ID context in request when present. If not 
 *          present or a response set to NULL
 * @param   oscore_option: output pointer OSCORE option structure
 * @return  oscore_error
 */
static inline enum oscore_error
oscore_option_generate(struct byte_array *piv, struct byte_array *kid,
		       struct byte_array *kid_context,
		       struct oscore_option *oscore_option)
{
	uint64_t dest_size;
	enum oscore_error r;
	oscore_option->option_number = COAP_OPTION_OSCORE;

	if (oscore_option->len == 0) {
		oscore_option->value = NULL;
	} else {
		memset(oscore_option->value, 0, oscore_option->len);

		uint8_t *temp_ptr = oscore_option->value;

		if (piv->len != 0) {
			/* Set header bits of PIV */
			oscore_option->value[0] |= piv->len;
			/* copy PIV (sender sequence) */

			dest_size = oscore_option->len -
				    (temp_ptr + 1 - oscore_option->value);
			r = _memcpy_s(++temp_ptr, dest_size, piv->ptr,
				      piv->len);
			if (r != oscore_no_error)
				return r;

			temp_ptr += piv->len;
		}

		if (kid_context->len != 0) {
			/* Set header flag bit of KID context */
			oscore_option->value[0] |= COMP_OSCORE_OPT_KIDC_H_MASK;
			/* Copy length and context value */
			*temp_ptr = kid_context->len;

			dest_size = oscore_option->len -
				    (temp_ptr + 1 - oscore_option->value);
			r = _memcpy_s(++temp_ptr, dest_size, kid_context->ptr,
				      kid_context->len);
			if (r != oscore_no_error)
				return r;

			temp_ptr += kid_context->len;
		}

		/* Set header flag bit of KID */
		/* The KID header flag is set always in requests */
		/* This function is not called in responses */
		oscore_option->value[0] |= COMP_OSCORE_OPT_KID_K_MASK;
		if (kid->len != 0) {
			/* Copy KID */
			dest_size = oscore_option->len -
				    (temp_ptr - oscore_option->value);
			r = _memcpy_s(temp_ptr, dest_size, kid->ptr, kid->len);
			if (r != oscore_no_error)
				return r;
		}
	}

	PRINT_ARRAY("OSCORE option value", oscore_option->value,
		    oscore_option->len);
	return oscore_no_error;
}

/**
 * @brief Generate an OSCORE packet with all needed data
 * @param in_o_coap: input CoAP packet
 * @param out_oscore: output pointer to OSCORE packet
 * @param U_options: pointer to array of all unprotected options, including OSCORE_option
 * @param U_options_cnt: count number of U-options
 * @param in_ciphertext: input ciphertext, will be set into payload in OSCORE packet
 * @param oscore_option: The OSCORE option
 * @return oscore_error
 *
 */
static inline enum oscore_error
oscore_pkg_generate(struct o_coap_packet *in_o_coap,
		    struct o_coap_packet *out_oscore,
		    struct o_coap_option *u_options, uint8_t u_options_cnt,
		    uint8_t *in_ciphertext, uint32_t in_ciphertext_len,
		    struct oscore_option *oscore_option)
{
	/* Set OSCORE header and Token*/
	out_oscore->header.ver = in_o_coap->header.ver;
	out_oscore->header.type = in_o_coap->header.type;
	out_oscore->header.TKL = in_o_coap->header.TKL;
	out_oscore->header.MID = in_o_coap->header.MID;
	if (out_oscore->header.TKL == 0) {
		out_oscore->token = NULL;
	} else {
		out_oscore->token = in_o_coap->token;
	}

	if ((in_o_coap->header.code & CODE_CLASS_MASK) == REQUEST_CLASS) {
		/*set code of requests to POST*/
		out_oscore->header.code = POST;
	} else {
		/*set code of responses to Changed*/
		out_oscore->header.code = Changed;
	}

	/* U-options + OSCORE option (compare oscore option number with others)
	 Find out the appropriate position of OSCORE option */
	uint8_t oscore_opt_pos = u_options_cnt;
	for (uint8_t i = 0; i < u_options_cnt; i++) {
		/* Once found, finish the for-loop */
		if (u_options[i].option_number > COAP_OPTION_OSCORE) {
			oscore_opt_pos = i;
			break;
		}
	}

	/* Update options count number to output*/
	out_oscore->options_cnt = 1 + u_options_cnt;

	uint8_t temp_opt_number_sum = 0;
	/* Show the position of U-options */
	uint8_t u_opt_pos = 0;
	for (uint8_t i = 0; i < u_options_cnt + 1; i++) {
		if (i == oscore_opt_pos) {
			/* OSCORE_option */
			out_oscore->options[i].delta =
				oscore_option->option_number -
				temp_opt_number_sum;
			out_oscore->options[i].len = oscore_option->len;
			out_oscore->options[i].option_number =
				oscore_option->option_number;
			out_oscore->options[i].value = oscore_option->value;
		} else {
			/* U-options */
			out_oscore->options[i].delta =
				u_options[u_opt_pos].option_number -
				temp_opt_number_sum;
			out_oscore->options[i].len = u_options[u_opt_pos].len;
			out_oscore->options[i].option_number =
				u_options[u_opt_pos].option_number;
			out_oscore->options[i].value =
				u_options[u_opt_pos].value;

			u_opt_pos++;
		}
		temp_opt_number_sum += out_oscore->options[i].delta;
	}

	/* Protected Payload */
	out_oscore->payload_len = in_ciphertext_len;
	out_oscore->payload = in_ciphertext;
	return oscore_no_error;
}

/**
 *@brief 	Converts a CoAP packet to OSCORE packet
 *
 *@param	buf_o_coap a buffer containing a CoAP packet
 *@param	buf_o_coap_len length of the CoAP buffer
 *@param	buf_oscore a buffer where the OSCORE packet will be written
 *@param	buf_oscore_len length of the OSCORE packet
 *@param	c a struct containing the OSCORE context
 *
 *@return	oscore_error
 */
enum oscore_error coap2oscore(uint8_t *buf_o_coap, uint16_t buf_o_coap_len,
			      uint8_t *buf_oscore, uint16_t *buf_oscore_len,
			      struct context *c)
{
	enum oscore_error r = oscore_no_error;
	struct o_coap_packet o_coap_pkt;
	struct byte_array buf;
	uint16_t plaintext_len = 0;

	PRINT_MSG(
		"\n\n\ncoap2oscore*******************************************\n");
	PRINT_ARRAY("Input CoAP packet", buf_o_coap, buf_o_coap_len);

	buf.len = buf_o_coap_len;
	buf.ptr = buf_o_coap;

	/*Parse the coap buf into a CoAP struct*/
	r = buf2coap(&buf, &o_coap_pkt);
	if (r != oscore_no_error)
		return r;

	/* 1. Divide CoAP options into E-option and U-option */
	struct o_coap_option e_options[MAX_OPTION_COUNT];
	uint8_t e_options_cnt = 0;
	uint16_t e_options_len = 0;
	struct o_coap_option u_options[MAX_OPTION_COUNT];
	uint8_t u_options_cnt = 0;

	/* Analyze CoAP options, extract E-options and U-options */
	r = e_u_options_extract(&o_coap_pkt, e_options, &e_options_cnt,
				&e_options_len, u_options, &u_options_cnt);
	if (r != oscore_no_error)
		return r;

	/* 2. Create plaintext (code + E-options + o_coap_payload) */
	/* Calculate complete plaintext length: 1 byte code + E-options + 1 byte 0xFF + payload */
	plaintext_len = 1 + e_options_len;

	if (o_coap_pkt.payload_len) {
		plaintext_len = plaintext_len + 1 + o_coap_pkt.payload_len;
	}

	/* Setup buffer for plaintext */
	uint8_t plaintext_bytes[plaintext_len];
	struct byte_array plaintext = {
		.len = plaintext_len,
		.ptr = plaintext_bytes,
	};

	/* Combine code, E-options and payload of CoAP to plaintext */
	r = plaintext_setup(&o_coap_pkt, e_options, e_options_cnt, &plaintext);
	if (r != oscore_no_error)
		return r;

	/* Generate OSCORE option */
	struct oscore_option oscore_option;

	/*
    - Only if the packet is a request the OSCORE option has a value 
    - Only if the packet is a request the nonce and the add need to be generated
    */
	if ((CODE_CLASS_MASK & o_coap_pkt.header.code) == 0) {
		/*update the piv in the request response context*/
		r = sender_seq_num2piv(c->sc.sender_seq_num++, &c->rrc.piv);
		if (r != oscore_no_error)
			return r;
		r = context_update(CLIENT,
				   (struct o_coap_option *)&o_coap_pkt.options,
				   o_coap_pkt.options_cnt, NULL, NULL, c);
		if (r != oscore_no_error)
			return r;

		/*calculate the OSCORE option value*/
		oscore_option.len = get_oscore_opt_val_len(
			&c->rrc.piv, &c->rrc.kid, &c->rrc.kid_context);
		if (oscore_option.len > OSCORE_OPT_VALUE_LEN) {
			return oscore_valuelen_to_long_error;
		}

		oscore_option.value = oscore_option.buf;
		r = oscore_option_generate(&c->rrc.piv, &c->rrc.kid,
					   &c->rrc.kid_context, &oscore_option);
		if (r != oscore_no_error)
			return r;
	} else {
		oscore_option.option_number = COAP_OPTION_OSCORE;
		oscore_option.len = 0;
		oscore_option.value = NULL;
	}

	/*3. Encrypt the created plaintext*/
	uint8_t ciphertext[plaintext.len + AUTH_TAG_LEN];

	r = plaintext_encrypt(c, &o_coap_pkt, &plaintext,
			      (uint8_t *)&ciphertext, sizeof(ciphertext));
	if (r != oscore_no_error)
		return r;

	/*create an OSCORE packet*/
	struct o_coap_packet oscore_pkt;
	r = oscore_pkg_generate(&o_coap_pkt, &oscore_pkt, u_options,
				u_options_cnt, (uint8_t *)&ciphertext,
				sizeof(ciphertext), &oscore_option);
	if (r != oscore_no_error)
		return r;

	/*convert the oscore pkg to byte string*/
	return coap2buf(&oscore_pkt, buf_oscore, buf_oscore_len);
}
