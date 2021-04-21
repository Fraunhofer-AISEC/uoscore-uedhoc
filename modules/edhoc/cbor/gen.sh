#!/bin/sh

# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

echo Generating CBOR encoding and decoding functions...  

rm *.h
rm *.c

###   messages   ###

#encode message 1
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_1.cddl code -e -t message_1 --oc encode_message_1.c --oh encode_message_1.h
#decode message 1
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_1.cddl code -d -t message_1 --oc decode_message_1.c --oh decode_message_1.h

#encode message 2
#python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_2.cddl code -e -t m2 --oc e_message_2.c --oh e_message_2.h

#python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_2.cddl code -e -t m2ci --oc e_message_2_c_i.c --oh e_message_2_c_i.h

#decode message 2
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_2.cddl code -d -t m2 --oc decode_message_2.c --oh decode_message_2.h

python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_2.cddl code -d -t m2ci --oc decode_message_2_c_i.c --oh decode_message_2_c_i.h




# python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_3.cddl code -e -t message_3 --oc message_3.c --oh message_3.h

# python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_4.cddl code -e -t message_4 --oc message_4.c --oh message_4.h


# python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c message_error.cddl code -e -t message_error --oc message_error.c --oh message_error.h


# ###   cose   ###

# python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c enc_structure.cddl code -e -t enc_structure --oc enc_structure.c --oh enc_structure.h

# python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c sig_structure.cddl code -e -t sig_structure --oc sig_structure.c --oh sig_structure.h



###   other  ###

# encode data_2 
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c data_2.cddl code -e -t data_2 --oc encode_data_2.c --oh encode_data_2.h

# info
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c info.cddl code -e -t info --oc encode_info.c --oh encode_info.h

# plaintext
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c plaintext.cddl code -d -t plaintext --oc decode_plaintext.c --oh decode_plaintext.h

# encode ID_CRED_x
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c plaintext.cddl code -e -t id_cred_x_map --oc encode_id_cred_x.c --oh encode_id_cred_x.h

# decode ID_CRED_x
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c plaintext.cddl code -d -t id_cred_x_map --oc decode_id_cred_x.c --oh decode_id_cred_x.h

# decode Native CBOR certificate
python3 ../../../externals/cddl-gen/scripts/cddl_gen.py -c cert.cddl code -d -t cert --oc decode_cert.c --oh decode_cert.h