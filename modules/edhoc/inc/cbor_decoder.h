/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/
#ifndef EDHOC_CBOR_DECODER_H
#define EDHOC_CBOR_DECODER_H

#include <cbor.h>
#include <stdint.h>

#include "../inc/error.h"

/**
 * @brief   Decodes a CBOR encoded data item
 * @param   next_ptr pointer to the next element in a large input buffer
 * @param   in_buffer buffer containing the CBOR data item
 * @param   in_size the size of in_buffer
 * @param   out_decoded_data the decoded data
 * @param   out_decoded_len the length of the decoded data
 */
enum edhoc_error cbor_decoder(uint8_t **next_ptr, uint8_t *in_buffer,
			      uint32_t in_size, void *out_decoded_data,
			      uint64_t *out_decoded_len, CborType *type);

#endif