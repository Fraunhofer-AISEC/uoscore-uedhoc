# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.


######################################
# target
######################################
TARGET = responder

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source, defines and includes
######################################

# CPP sources
CPP_SOURCES = \
src/main.cpp \
../../../externals/cantcoap/cantcoap.cpp

# C sources
DO_NOT_COMPILE_SOURCES = \
../../../externals/tinycbor/src/open_memstream.c \
../../../externals/tinycbor/src/cbortojson.c \
../../../externals/tinycbor/src/cborpretty.c \
../../../externals/tinycbor/src/cborencoder_close_container_checked.c \
../../../externals/tinycbor/src/cborvalidation.c \
../../../externals/tinycbor/src/cborparser_dup_string.c \
../../../externals/tinycbor/src/cborpretty_stdio.c \
../../../externals/tinycbor/src/cborerrorstrings.c \
../../../externals/tinycrypt/lib/source/ctr_prng.c \
../../../externals/tinycrypt/lib/source/ecc_dh.c \
../../../externals/tinycrypt/lib/source/cbc_mode.c \
../../../externals/tinycrypt/lib/source/hmac_prng.c \
../../../externals/tinycrypt/lib/source/ctr_mode.c \
../../../externals/tinycrypt/lib/source/ecc_platform_specific.c \
../../../externals/tinycrypt/lib/source/cmac_mode.c \
../../../externals/tinycrypt/lib/source/ecc.c \
../../../externals/tinycrypt/lib/source/ecc_dsa.c 

C_SOURCES = $(wildcard src/*.c)
C_SOURCES += $(wildcard ../../common/*.c) 
C_SOURCES += $(wildcard ../../../modules/edhoc/src/*.c)
C_SOURCES += ../../../modules/edhoc/crypto_wrapper.c
C_SOURCES += $(wildcard ../../../externals/compact25519/src/c25519/*.c)
C_SOURCES += $(wildcard ../../../externals/compact25519/src/*.c)
C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../../externals/tinycbor/src/*.c))
C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../../externals/tinycrypt/lib/source/*.c))

#$(info    C_SOURCES is $(C_SOURCES)) 

# C defines
C_DEFS =   \
-DEDHOC_WITH_TINYCRYPT_AND_C25519 \
-DEDHOC_DEBUG_PRINT


# C includes
C_INCLUDES =  \
-Isrc/ \
-I../../../externals/cantcoap/ \
-I../../../modules/edhoc/ \
-I../../../externals/tinycbor/src/ \
-I../../../externals/compact25519/src/c25519/ \
-I../../../externals/compact25519/src/\
-I../../../externals/tinycrypt/lib/include


#########################################
# Use gcc compiler with flags
#########################################
CXX = g++
CC = gcc
SZ = size

#LDFLAGS=-lm 

# ../../../externals/cantcoap/libcantcoap.a \
# ../../../externals/tinycbor/lib/libtinycbor.a \
# -L/usr/local/lib \
# -lwolfssl 

#-lssl \
-lcrypto \

##########################################
# CFLAGS
##########################################
#general c flags
CFLAGS =  $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall 
CXXFLAGS = $(C_DEFS) $(C_INCLUDES) $(OPT) 

# have dubug information
CFLAGS += -g -gdwarf-2
CXXFLAGS += -Wall -g


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" 

# use AddressSanitizer to find memory bugs
# comment this out for better speed
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address -static-libasan

###########################################
# default action: build all
###########################################
all: $(BUILD_DIR)/$(TARGET)

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

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)
