/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef OSCORE_PRINT_UTIL_H
#define OSCORE_PRINT_UTIL_H

#include <stdint.h>
/**
 *@brief prints an array for debug pourposes 
 */
void print_array(uint8_t* in_data, uint16_t in_len);

#ifdef OSCORE_DEBUG_PRINT
#define PRINT_ARRAY(msg, a, a_len) \
    printf(msg);                   \
    print_array(a, a_len);
#define PRINT_MSG(msg) printf(msg);
#else
#define PRINT_ARRAY(msg, a, a_len) {};
#define PRINT_MSG(msg) {};
#endif

#endif