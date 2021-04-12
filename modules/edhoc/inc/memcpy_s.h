/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef MEMCPY_S_H
#define MEMCPY_S_H

#include <stdint.h>

#include "error.h"

/**
 * @brief memcpy_s (see [1]) may not be available in some setups thus our own 
 * implementation 
 * [1]: https://docs.microsoft.com/de-de/cpp/c-runtime-library/reference/memcpy-s-wmemcpy-s?view=msvc-160
 */
EdhocError _memcpy_s(uint8_t *dest, uint64_t destSize, const uint8_t *src,
		     uint64_t count);

#endif
