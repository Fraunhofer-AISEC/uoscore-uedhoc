#!/bin/sh

# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

echo Generating CBOR encoding and decoding functions...  

#rm *.h
#rm *.c

python3 ../../../externals/cddl-gen/zcbor/zcbor.py -c aad_array.cddl code -e -t aad_array --oc aad_array.c --oh aad_array.h

python3 ../../../externals/cddl-gen/zcbor/zcbor.py -c info.cddl code -e -t oscore_info --oc info.c --oh info.h

python3 ../../../externals/cddl-gen/zcbor/zcbor.py -c enc_structure.cddl code -e -t enc_structure --oc enc_structure.c --oh enc_structure.h