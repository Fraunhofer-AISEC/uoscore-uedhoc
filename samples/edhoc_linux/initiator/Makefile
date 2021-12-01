# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

# toolchain
CXX = g++
CC = gcc
SZ = size
MAKE = make

# target
TARGET = initiator

# build path
BUILD_DIR = build

# build for 32 bit x68
CFLAGS = -m32
CXXFLAGS = -m32
LDFLAGS = -m32

# debug build?
DEBUG = 1

# optimization
OPT = -Og

# CPP sources
CPP_SOURCES += src/main.cpp
CPP_SOURCES += ../../../externals/cantcoap/cantcoap.cpp

# C sources 
C_SOURCES += $(wildcard ../../common/*.c) 
C_SOURCES += $(wildcard ../../common/cbor/*.c) 
C_SOURCES += $(wildcard ../../../externals/cddl-gen/src/*.c)
C_SOURCES += $(wildcard ../../../externals/compact25519/src/c25519/*.c)
C_SOURCES += $(wildcard ../../../externals/compact25519/src/*.c)
# C_SOURCES += $(wildcard ../../../externals/mbedtls_ecp_compression/*.c)
# C_SOURCES += $(wildcard ../../../externals/tinycrypt/lib/source/*.c)
C_SOURCES += $(wildcard ../../../externals/mbedtls/library/*.c)

# C includes
C_INCLUDES += -I../../../externals/cantcoap/ 
C_INCLUDES += -I../../../modules/edhoc/ 
C_INCLUDES += -I../../../externals/jsmn 
C_INCLUDES += -I../../../externals/cddl-gen/include 
#C_INCLUDES += -I../../../externals/tinycrypt/lib/include
C_INCLUDES += -I../../../externals/mbedtls_ecp_compression
C_INCLUDES += -I../../../externals/mbedtls/library 
C_INCLUDES += -I../../../externals/mbedtls/include 
C_INCLUDES += -I../../../externals/mbedtls/include/mbedtls 
C_INCLUDES += -I../../../externals/mbedtls/include/psa 

# C defines
# make PRINT_ARRAY macro usable in the main file
C_DEFS += -DDEBUG_PRINT

# Linked libraries
LD_LIBRARY_PATH += -L../../../build

LDFLAGS += $(LD_LIBRARY_PATH)
LDFLAGS += -luoscore-uedhoc
##########################################
# CFLAGS
##########################################
#general c flags
CFLAGS +=  $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall
CXXFLAGS += $(C_DEFS) $(C_INCLUDES) $(OPT)

# have dubug information
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
CXXFLAGS += -Wall -g
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" 

# required for gddl-gen library
CFLAGS += -DCDDL_CBOR_CANONICAL 



# use AddressSanitizer to find memory bugs
# comment this out for better speed
#CFLAGS += -fsanitize=address -fno-omit-frame-pointer
#CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
#LDFLAGS += -fsanitize=address -static-libasan




# export the folliwing variables so that they are available as well 
# in the sub makefiles WE NEED TO EXPORT ONLY THE -m32 option
export CFLAGS
export CXXFLAGS
export LDFLAGS



###########################################
# default action: build all
###########################################
#list of objects from c files
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of objects from c++ file
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))



$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CXX) $(OBJECTS)  $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR):
	mkdir $@
	$(MAKE) -C ../../../





clean:
	-rm -fR $(BUILD_DIR)
	$(MAKE) -C ../../../ clean

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)