/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
	extern "C" {
#endif
		void send_coap(uint8_t *msg, uint32_t msg_len);
		void recv_coap(uint8_t **msg, uint32_t *msg_len);
#ifdef __cplusplus
	}
#endif


#endif
