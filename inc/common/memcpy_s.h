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

#include "oscore_edhoc_error.h"

/**
 * @brief Checks if a buffer has suficent size
 * 
 * @param is_size the actual size of the buffer in bytes
 * @param required_size the required size in bytes
 * @return enum err 
 */
enum err check_buffer_size(uint32_t is_size, uint32_t required_size);

/**
 * @brief A save memcpy function equivalent to memcpy_s (see [1]). 
 *        memcpy_s may not be available in some environments thus we 
 *        provide our own implementation.
 * 
 * @param dest destination buffer
 * @param dest_len lenhgt of the destination buffer
 * @param source source buffer
 * @param source_len lenhgt of the source buffer
 * @return enum err
 * 
 * [1]: https://docs.microsoft.com/de-de/cpp/c-runtime-library/reference/memcpy-s-wmemcpy-s?view=msvc-160
 */
enum err _memcpy_s(uint8_t *dest, uint32_t dest_len, const uint8_t *source,
		   uint32_t source_len);

#endif
