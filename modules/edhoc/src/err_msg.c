/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include "../inc/err_msg.h"

#include <stdint.h>

#include "../edhoc.h"
#include "../../common/inc/error.h"
#include "../inc/messages.h"
#include "../inc/print_util.h"
#include "../inc/txrx_wrapper.h"
#include "../cbor/encode_message_error.h"
/**
 * @brief   encodes an error message
 * @param   msg_struct contains all messsage fields
 * @param   msg the encoded error message
 */
static inline enum err err_msg_crate(struct error_msg *msg_struct,
					     struct byte_array *msg)
{
	// 	bool success;
	// 	struct message_error mr;

	// 	/*Encode C_x if present*/
	// 	if (msg_struct->c_x.len) {
	// 		mr._message_error_C_x_present = true;
	// 		if (msg_struct->c_x.len == 1) {
	// 			mr._message_error_C_x._message_error_C_x_choice =
	// 				_message_error_C_x_int;
	// 			mr._message_error_C_x._message_error_C_x_int =
	// 				*(msg_struct->c_x.ptr) - 24;
	// 		} else {
	// 			mr._message_error_C_x._message_error_C_x_choice =
	// 				_message_error_C_x_bstr;
	// 			mr._message_error_C_x._message_error_C_x_bstr.value =
	// 				msg_struct->c_x.ptr;
	// 			mr._message_error_C_x._message_error_C_x_bstr.len =
	// 				msg_struct->c_x.len;
	// 		}
	// 	}

	// 	/*Encode DIAG_MSG*/
	// 	mr._message_error_DIAG_MSG.value = msg_struct->diag_msg.ptr;
	// 	mr._message_error_DIAG_MSG.len = msg_struct->diag_msg.len;

	// 	/*Encode SUITES_R if present*/
	// 	if (msg_struct->suites_r.len) {
	// 		//todo fix that
	// 		// mr._message_error_SUITES_R_present = true;
	// 		// if (msg_struct->suites_r.len == 1) {
	// 		// 	mr._message_error_SUITES_R
	// 		// 		._message_error_SUITES_R_choice =
	// 		// 		_message_error_SUITES_R_int;
	// 		// 	mr._message_error_SUITES_R._message_error_SUITES_R_int =
	// 		// 		msg_struct->suites_r.ptr[0];
	// 		// } else {
	// 		// 	mr._message_error_SUITES_R
	// 		// 		._message_error_SUITES_R_choice =
	// 		// 		_message_error_SUITES_R_int;
	// 		// 	uint32_t i;
	// 		// 	for (i = 0; i < msg_struct->suites_r.len; i++) {
	// 		// 		mr._message_error_SUITES_R
	// 		// 			._message_error_SUITES_R_int[i] =
	// 		// 			msg_struct->suites_r.ptr[i];
	// 		// 	}
	// 		// 	mr._message_error_SUITES_R
	// 		// 		._message_error_SUITES_R_int_supported_count =
	// 		// 		i - 1;
	// 	}
	// }

	// size_t payload_len_out;
	// success = cbor_encode_message_error(msg->ptr, msg->len, &mr, &payload_len_out);

	// if (!success) {
	// 	return cbor_encoding_error;
	// }
	// msg->len = payload_len_out;

	// PRINT_ARRAY("Error message (CBOR Sequence)", msg->ptr, msg->len);
	return edhoc_no_error;
}

enum err tx_err_msg(enum role role, uint8_t corr, uint8_t *c_x,
			    uint8_t c_x_len, uint8_t *err_msg_str,
			    uint8_t err_msg_str_len, uint8_t *suites,
			    uint8_t suites_len)
{
	// enum err r;
	// uint8_t err_msg_buf[ERR_MSG_DEFAULT_SIZE];
	// struct byte_array err_msg = {
	// 	.len = sizeof(err_msg_buf),
	// 	.ptr = err_msg_buf,
	// };

	// struct error_msg err_struct = {
	// 	.diag_msg.ptr = err_msg_str,
	// 	.diag_msg.len = err_msg_str_len,
	// 	.suites_r.ptr = suites,
	// 	.suites_r.len = suites_len,
	// };

	// if ((role == RESPONDER && (corr == 0 || corr == 2)) ||
	//     (role == INITIATOR && (corr == 0 || corr == 1))) {
	// 	err_struct.c_x.ptr = c_x;
	// 	err_struct.c_x.len = c_x_len;
	// } else {
	// 	err_struct.c_x.ptr = NULL;
	// 	err_struct.c_x.len = 0;
	// }

	// r = err_msg_crate(&err_struct, &err_msg);
	// if (r != edhoc_no_error) {
	// 	return r;
	// }
	// return tx(err_msg.ptr, err_msg.len);
	return edhoc_no_error;
}
