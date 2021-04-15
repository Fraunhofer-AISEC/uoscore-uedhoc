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

###   messages   ###

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_1.cddl code -e -t message_1 --oc message_1.c --oh message_1.h

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_2.cddl code -e -t message_2 --oc message_2.c --oh message_2.h

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_3.cddl code -e -t message_3 --oc message_3.c --oh message_3.h

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_4.cddl code -e -t message_4 --oc message_4.c --oh message_4.h


python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_error.cddl code -e -t message_error --oc message_error.c --oh message_error.h


###   cose   ###

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c enc_structure.cddl code -e -t enc_structure --oc enc_structure.c --oh enc_structure.h

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c sig_structure.cddl code -e -t sig_structure --oc sig_structure.c --oh sig_structure.h



###   other  ###

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c info.cddl code -e -t info --oc info.c --oh info.h

