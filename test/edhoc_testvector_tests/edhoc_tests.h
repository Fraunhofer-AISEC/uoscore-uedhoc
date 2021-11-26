/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef EDHOC_TESTS_H
#define EDHOC_TESTS_H

#include <stdint.h>
#include "../../samples/common/test_vec_parser.h"

extern struct messages_bufs m_bufs;
extern struct messages m;

/**
 * @brief       Runs an initiator/responder and compares the results with the  *              official test vectors.
 * @param       p is INITIATOR or RESPONDER
 * @param       vec_num the test vector number
 */
int test_edhoc(enum role p, uint8_t vec_num);

#endif