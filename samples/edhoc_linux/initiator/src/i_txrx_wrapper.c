/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

// #include <edhoc.h>
// #include <stdint.h>
// #include <string.h>

// #include "main.h"

// static int _memcpy_s(uint8_t *dest, uint32_t destSize, const uint8_t *src,
// 		     uint8_t count)
// {
// 	if (destSize < count) {
// 		return -1;
// 	} else {
// 		memcpy(dest, src, count);
// 	}
// 	return 0;
// }

// enum edhoc_error tx(uint8_t *data, uint32_t data_len)
// {
// 	send_coap(data, data_len);
// 	return edhoc_no_error;
// }

// enum edhoc_error rx(uint8_t *data, uint32_t *data_len)
// {
// 	uint8_t *data_p;
// 	uint32_t data_buf_capacity = *data_len;
// 	recv_coap(&data_p, data_len);
// 	PRINT_ARRAY("RX raw data", data_p, *data_len);
// 	int r = _memcpy_s(data, data_buf_capacity, data_p, *data_len);
// 	if (r != 0)
// 		return message_buff_to_small;
// 	return edhoc_no_error;
// }
