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
TARGET = oscore_linux_client

######################################
# building variables
######################################
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
C_SOURCES += $(wildcard ../../../modules/oscore/src/*.c)
C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../../externals/tinycbor/src/*.c))
C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../../externals/tinycrypt/lib/source/*.c))


# C defines
C_DEFS =  \
-DOSCORE_WITH_TINYCRYPT 
#-DOSCORE_DEBUG_PRINT 

# C++ defines
CXX_DEFS =  


# C includes
C_INCLUDES =  \
-Iinc/ \
-I../common/ \
-I../../../externals/cantcoap/ \
-I../../../modules/oscore/ \
-I../../../externals/tinycbor/src/ \
-I../../../externals/tinycrypt/lib/include

#-Iexternals/libcoap/examples/lwip/
#########################################
# Use gcc compiler with flags
#########################################
CXX = g++
CC = gcc
SZ = size


##########################################
# CFLAGS
##########################################
#general c flags
CFLAGS =  $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall 
CXXFLAGS = $(CXX_DEFS) $(OPT) -Wall 

# have dubug information
CFLAGS += -g -gdwarf-2
CXXFLAGS += -Wall -g


# generate dependency information
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
