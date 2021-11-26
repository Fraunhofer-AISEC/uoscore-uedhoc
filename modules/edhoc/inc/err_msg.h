/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef ERR_MSG_H
#define ERR_MSG_H

#include <stdint.h>

#include "error.h"

enum role { INITIATOR, RESPONDER };

/**
 * @brief   creates and sends an error message
 * @param   role INITIATOR or RESPONDER
 * @param   c_x connection identifier
 * @param   c_x_len length of c_x
 * @param   err_msg_str human readable error message string
 * @param   err_msg_str_len length of err_msg_str
 * @param   suites list of suported suites. To be used only after message 1
 * @param   suites_len length of suites
 */
enum err tx_err_msg(enum role role, uint8_t corr, uint8_t *c_x,
		      uint8_t c_x_len, uint8_t *err_msg_str,
		      uint8_t err_msg_str_len, uint8_t *suites,
		      uint8_t suites_len);

#endif