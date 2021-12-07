/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#include <stdint.h>
#include <string.h>

#include "oscore_edhoc_error.h"

enum err _memcpy_s(uint8_t *dest, uint64_t destSize, const uint8_t *src,
			   uint64_t count)
{
	if (destSize < count) {
		return dest_buffer_to_small;
	} else {
		memcpy(dest, src, count);
	}
	return ok;
}