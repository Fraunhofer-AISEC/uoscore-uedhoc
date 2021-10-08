/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/th.h"

#include "../edhoc.h"
#include "../inc/crypto_wrapper.h"
#include "../inc/error.h"
#include "../inc/memcpy_s.h"
#include "../inc/print_util.h"
#include "../cbor/encode_data_2.h"
#include "../cbor/encode_th3.h"
#include "../cbor/encode_th4.h"

/**
 * @brief   Setups a data structure used as input for th2
 * @param   msg1 pointer to a message 1
 * @param   msg1_len length of message 1
 * @param   c_i Pointer to the conception identifier of the initiator
 * @param   g_y Pointer to the public DH parameter
 * @param   c_r Pointer to the conception identifier of the responder
 * @param   th2_input ouput buffer for the data structure
 * @param   th2_input_len length of th2_input
 */
static inline enum edhoc_error
th2_input_encode(uint8_t *msg1, uint32_t msg1_len, uint8_t *c_i,
		 uint32_t c_i_len, uint8_t *g_y, uint32_t g_y_len, uint8_t *c_r,
		 uint32_t c_r_len, uint8_t *th2_input, uint16_t *th2_input_len)
{
	enum edhoc_error r;
	bool success;
	size_t payload_len_out;
	struct data_2 data_2;

	r = _memcpy_s(th2_input, *th2_input_len, msg1, msg1_len);
	if (r != edhoc_no_error) {
		return r;
	}

	/*Encode C_I if present*/
	if (c_i_len != 0) {
		data_2._data_2_C_I_present = true;
		if (c_i_len == 1) {
			data_2._data_2_C_I._data_2_C_I_choice = _data_2_C_I_int;
			data_2._data_2_C_I._data_2_C_I_int = *c_i - 24;
		} else {
			data_2._data_2_C_I._data_2_C_I_choice =
				_data_2_C_I_bstr;
			data_2._data_2_C_I._data_2_C_I_bstr.value = c_i;
			data_2._data_2_C_I._data_2_C_I_bstr.len = c_i_len;
		}
	} else {
		data_2._data_2_C_I_present = false;
	}

	/*Encode G_Y*/
	data_2._data_2_G_Y.value = g_y;
	data_2._data_2_G_Y.len = g_y_len;

	/*Encode C_R as bstr_identifier*/
	if (c_r_len == 1) {
		data_2._data_2_C_R_choice = _data_2_C_R_int;
		data_2._data_2_C_R_int = *c_r - 24;
	} else {
		data_2._data_2_C_R_choice = _data_2_C_R_bstr;
		data_2._data_2_C_R_bstr.value = c_r;
		data_2._data_2_C_R_bstr.len = c_r_len;
	}
	success = cbor_encode_data_2((th2_input + msg1_len),
				     (*th2_input_len - msg1_len), &data_2,
				     &payload_len_out);

	if (!success) {
		return cbor_encoding_error;
	}

	/* Get the the total th2 length */
	*th2_input_len = payload_len_out + msg1_len;

	PRINT_ARRAY("Input to calculate TH_2 (CBOR Sequence)", th2_input,
		    *th2_input_len);
	return edhoc_no_error;
}

/**
 * @brief   Setups a data structure used as input for th3
 * @param   th2 pointer to a th2
 * @param   th2_len length of th2
 * @param   ciphertext_2 
 * @param   ciphertext_2_len  length of ciphertext_2_len
 * @param   data_3 
 * @param   data_3_len  length of data_3_len
 * @param   th3_input ouput buffer for the data structure
 * @param   th3_input_len length of th3_input
 */
static inline enum edhoc_error
th3_input_encode(uint8_t *th2, uint8_t th2_len, uint8_t *ciphertext_2,
		 uint16_t ciphertext_2_len, uint8_t *data_3, uint8_t data_3_len,
		 uint8_t *th3_input, uint16_t *th3_input_len)
{
	bool success;
	struct th3 th3;

	/*Encode th2*/
	th3._th3_th2.value = th2;
	th3._th3_th2.len = th2_len;

	/*Encode ciphertext_2*/
	th3._th3_CIPHERTEXT_2.value = ciphertext_2;
	th3._th3_CIPHERTEXT_2.len = ciphertext_2_len;

	/*Encode C_R*/
	if (data_3_len) {
		th3._th3_data_3_present = true;
		if (data_3_len == 1) {
			th3._th3_data_3._th3_data_3_choice = _th3_data_3_int;
			th3._th3_data_3._th3_data_3_int = *data_3 - 24;
		} else {
			th3._th3_data_3._th3_data_3_choice = _th3_data_3_bstr;
			th3._th3_data_3._th3_data_3_bstr.value = data_3;
			th3._th3_data_3._th3_data_3_bstr.len = data_3_len;
		}
	}

	size_t payload_len_out;
	success = cbor_encode_th3(th3_input, *th3_input_len, &th3,
				  &payload_len_out);

	if (!success) {
		return cbor_encoding_error;
	}
	*th3_input_len = payload_len_out;

	PRINT_ARRAY("Input to calculate TH_3 (CBOR Sequence)", th3_input,
		    *th3_input_len);
	return edhoc_no_error;
}

/**
 * @brief   Setups a data structure used as input for th4
 * @param   th3 pointer to a th3
 * @param   th3_len length of th3
 * @param   ciphertext_3
 * @param   ciphertext_3_len  length of ciphertext_3_len
 * @param   th4_input ouput buffer for the data structure
 * @param   th4_input_len length of th4_input
 */
static inline enum edhoc_error th4_input_encode(uint8_t *th3, uint8_t th3_len,
						uint8_t *ciphertext_3,
						uint16_t ciphertext_3_len,
						uint8_t *th4_input,
						uint64_t *th4_input_len)
{
	bool success;
	struct th4 th4;

	/*Encode th2*/
	th4._th4_th_3.value = th3;
	th4._th4_th_3.len = th3_len;

	/*Encode ciphertext_3*/
	th4._th4_CIPHERTEXT_3.value = ciphertext_3;
	th4._th4_CIPHERTEXT_3.len = ciphertext_3_len;

	size_t payload_len_out;
	success = cbor_encode_th4(th4_input, *th4_input_len, &th4,
				  &payload_len_out);

	if (!success) {
		return cbor_encoding_error;
	}
	*th4_input_len = payload_len_out;

	PRINT_ARRAY("Input to calculate TH_4 (CBOR Sequence)", th4_input,
		    *th4_input_len);
	return edhoc_no_error;
}

enum edhoc_error th2_calculate(enum hash_alg alg, uint8_t *msg1,
			       uint32_t msg1_len, uint8_t *c_i,
			       uint32_t c_i_len, uint8_t *g_y, uint32_t g_y_len,
			       uint8_t *c_r, uint32_t c_r_len, uint8_t *th2)
{
	uint8_t th2_input[TH_INPUT_DEFAULT_SIZE];
	uint16_t th2_input_len = sizeof(th2_input);
	enum edhoc_error r;

	PRINT_ARRAY("C_R", c_r, c_r_len);

	r = th2_input_encode(msg1, msg1_len, c_i, c_i_len, g_y, g_y_len, c_r,
			     c_r_len, th2_input, &th2_input_len);
	if (r != edhoc_no_error) {
		return r;
	}

	r = hash(alg, th2_input, th2_input_len, th2);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("TH2", th2, SHA_DEFAULT_SIZE);
	return edhoc_no_error;
}

enum edhoc_error th3_calculate(enum hash_alg alg, uint8_t *th2, uint8_t th2_len,
			       uint8_t *ciphertext_2, uint16_t ciphertext_2_len,
			       uint8_t *data_3, uint8_t data_3_len,
			       uint8_t *th3)
{
	uint8_t th3_input[th2_len + ciphertext_2_len + data_3_len + 6];
	uint16_t th3_input_len = sizeof(th3_input);
	enum edhoc_error r =
		th3_input_encode(th2, th2_len, ciphertext_2, ciphertext_2_len,
				 data_3, data_3_len, th3_input, &th3_input_len);
	if (r != edhoc_no_error) {
		return r;
	}
	r = hash(alg, th3_input, th3_input_len, th3);
	if (r != edhoc_no_error) {
		return r;
	}
	PRINT_ARRAY("TH3", th3, 32);
	return edhoc_no_error;
}

enum edhoc_error th4_calculate(enum hash_alg alg, uint8_t *th3, uint8_t th3_len,
			       uint8_t *ciphertext_3, uint16_t ciphertext_3_len,
			       uint8_t *th4)
{
	uint8_t th4_input[th3_len + ciphertext_3_len + 4];
	uint64_t th4_input_len = sizeof(th4_input);

	enum edhoc_error r =
		th4_input_encode(th3, th3_len, ciphertext_3, ciphertext_3_len,
				 th4_input, &th4_input_len);
	if (r != edhoc_no_error)
		return r;

	r = hash(alg, th4_input, th4_input_len, th4);
	if (r != edhoc_no_error)
		return r;
	PRINT_ARRAY("TH4", th4, SHA_DEFAULT_SIZE);
	return edhoc_no_error;
}
