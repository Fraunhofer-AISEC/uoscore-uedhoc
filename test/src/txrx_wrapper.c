/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include <edhoc.h>
#include <stdbool.h>
#include <string.h>

bool rx_initiator_switch = false;

extern struct byte_array M1, M2, M3;

/**
 * memcpy_s (see [1]) may not be available in some setups thus our own 
 * implementation 
 * [1]: https://docs.microsoft.com/de-de/cpp/c-runtime-library/reference/memcpy-s-wmemcpy-s?view=msvc-160
 */
static int _memcpy_s(uint8_t *dest, uint32_t destSize, const uint8_t *src,
		     uint8_t count)
{
	if (destSize < count) {
		return -1;
	} else {
		memcpy(dest, src, count);
	}
	return 0;
}

enum edhoc_error rx(uint8_t *data, uint32_t *data_len)
{
	int r;
	if (rx_initiator_switch) {
		PRINTF("TXRX wrapper test vectors\n");
		/*The sender must get msg2*/
		r = _memcpy_s(data, *data_len, M2.ptr, M2.len);
		if (r != 0)
			return message_buff_to_small;
		*data_len = M2.len;

	} else {
		PRINTF("TXRX wrapper test vectors\n");
		static uint8_t msg_cnt = 1;
		/*The recipient must get msg1 and msg3*/
		if (msg_cnt == 1) {
			/*message 1 */
			r = _memcpy_s(data, *data_len, M1.ptr, M1.len);
			if (r != 0)
				return message_buff_to_small;
			*data_len = M1.len;
			msg_cnt++;
		} else {
			/*message 2*/
			r = _memcpy_s(data, *data_len, M3.ptr, M3.len);
			if (r != 0)
				return message_buff_to_small;
			*data_len = M3.len;
			msg_cnt = 1;
		}
	}
	return edhoc_no_error;
}

enum edhoc_error tx(uint8_t *data, uint32_t data_len)
{
	return edhoc_no_error;
}
