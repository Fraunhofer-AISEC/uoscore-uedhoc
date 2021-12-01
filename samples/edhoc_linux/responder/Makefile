# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

# in order to rebuild the uoscore-uedhoc.a and responder application call: 
# make oscore_edhoc; make

# toolchain
CXX = g++
CC = gcc
SZ = size
MAKE = make

# target
TARGET = responder

# build path
BUILD_DIR = build

# libusocore-uedhoc path
USOCORE_UEDHOC_PATH = ../../../
USOCORE_UEDHOC_BUILD_PATH = $(USOCORE_UEDHOC_PATH)build

# build for 32 bit x68
# export the varible so that it is availbale in the uoscore-uedhoc Makefile 
ARCH = -m32
export ARCH

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
C_INCLUDES += -I../../../modules/edhoc/ 
C_INCLUDES += -I../../../externals/cantcoap/ 
C_INCLUDES += -I../../../externals/jsmn 
C_INCLUDES += -I../../../externals/cddl-gen/include 
# C_INCLUDES += -I../../../externals/tinycrypt/lib/include
C_INCLUDES += -I../../../externals/mbedtls_ecp_compression
C_INCLUDES += -I../../../externals/mbedtls/library 
C_INCLUDES += -I../../../externals/mbedtls/include 
C_INCLUDES += -I../../../externals/mbedtls/include/mbedtls 
C_INCLUDES += -I../../../externals/mbedtls/include/psa 

# C defines
# make PRINT_ARRAY macro usable in the main file
C_DEFS += -DDEBUG_PRINT

# Linked libraries
LD_LIBRARY_PATH += -L$(USOCORE_UEDHOC_BUILD_PATH)

LDFLAGS += $(LD_LIBRARY_PATH)
LDFLAGS += -luoscore-uedhoc
LDFLAGS += $(ARCH) 
##########################################
# CFLAGS
##########################################
#general c flags
CFLAGS +=  $(ARCH) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall
CXXFLAGS += $(ARCH) $(C_DEFS) $(C_INCLUDES) $(OPT)

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

###########################################
# default action: build all
###########################################
#list of objects from c files
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of objects from c++ file
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

USOCORE_UEDHOC_OBJ = $(wildcard $(USOCORE_UEDHOC_PATH)/*.o) 

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile $(USOCORE_UEDHOC_PATH)/Makefile
	$(MAKE) -C $(USOCORE_UEDHOC_PATH)
	$(CXX) $(OBJECTS)  $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR):
	mkdir $@



oscore_edhoc:
	$(MAKE) -C $(USOCORE_UEDHOC_PATH)

clean_oscore_edhoc:
	$(MAKE) -C $(USOCORE_UEDHOC_PATH) clean

clean:
	-rm -fR $(BUILD_DIR)
	$(MAKE) -C $(USOCORE_UEDHOC_PATH) clean


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)





























# ######################################
# # target
# ######################################
# TARGET = responder

# ######################################
# # building variables
# ######################################
# # 32bit build
# CFLAGS = -m32
# CXXFLAGS = -m32
# LDFLAGS = -m32

# # debug build?
# DEBUG = 1
# # optimization
# OPT = -O0


# #######################################
# # paths
# #######################################
# # Build path
# BUILD_DIR = build

# ######################################
# # source, defines and includes
# ######################################

# # CPP sources
# CPP_SOURCES = \
# src/main.cpp \
# ../../../externals/cantcoap/cantcoap.cpp

# # C sources
# DO_NOT_COMPILE_SOURCES = \
# ../../../externals/tinycrypt/lib/source/ctr_prng.c \
# ../../../externals/tinycrypt/lib/source/cbc_mode.c \
# ../../../externals/tinycrypt/lib/source/hmac_prng.c \
# ../../../externals/tinycrypt/lib/source/ctr_mode.c \
# ../../../externals/tinycrypt/lib/source/cmac_mode.c \


# C_SOURCES = $(wildcard src/*.c)
# C_SOURCES += $(wildcard ../../common/*.c) 
# C_SOURCES += $(wildcard ../../common/cbor/*.c) 
# C_SOURCES += $(wildcard ../../../modules/edhoc/src/*.c)
# C_SOURCES += $(wildcard ../../../modules/edhoc/cbor/*.c)
# C_SOURCES += $(wildcard ../../../externals/cddl-gen/src/*.c)
# #C_SOURCES += $(wildcard ../../../externals/compact25519/src/c25519/*.c)
# #C_SOURCES += $(wildcard ../../../externals/compact25519/src/*.c)

# #C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../../externals/tinycrypt/lib/source/*.c))
# C_SOURCES += $(wildcard ../../../externals/mbedtls/library/*.c)

# #$(info    C_SOURCES is $(C_SOURCES)) 

# # C defines
# C_DEFS =   \
# -DEDHOC_WITH_MBEDTLS \
# -DEDHOC_DEBUG_PRINT
# #-DEDHOC_WITH_TINYCRYPT_AND_C25519 \

# # C includes
# C_INCLUDES =  \
# -Isrc/ \
# -I../../../externals/cantcoap/ \
# -I../../../modules/edhoc/ \
# -I../../../externals/tinycbor/src/ \
# -I../../../externals/compact25519/src/c25519/ \
# -I../../../externals/compact25519/src/\
# -I../../../externals/tinycrypt/lib/include \
# -I../../../externals/cddl-gen/include \
# -I../../../externals/jsmn \
# -I../../../externals/mbedtls/library \
# -I../../../externals/mbedtls/include \
# -I../../../externals/mbedtls/include/mbedtls \
# -I../../../externals/mbedtls/include/psa \


# #########################################
# # Use gcc compiler with flags
# #########################################
# CXX = g++
# CC = gcc
# SZ = size

# #LDFLAGS=-lm 

# # ../../../externals/cantcoap/libcantcoap.a \
# # ../../../externals/tinycbor/lib/libtinycbor.a \
# # -L/usr/local/lib \
# # -lwolfssl 

# #-lssl \
# -lcrypto \

# ##########################################
# # CFLAGS
# ##########################################
# #general c flags
# CFLAGS +=  $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall
# CXXFLAGS += $(C_DEFS) $(C_INCLUDES) $(OPT)

# # have dubug information
# CFLAGS += -g -gdwarf-2
# CXXFLAGS += -Wall -g


# # Generate dependency information
# CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
# CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" 

# # required for gddl-gen library
# CFLAGS += -DCDDL_CBOR_CANONICAL 

# # use AddressSanitizer to find memory bugs
# # comment this out for better speed
# CFLAGS += -fsanitize=address -fno-omit-frame-pointer
# CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
# LDFLAGS += -fsanitize=address -static-libasan

# ###########################################
# # default action: build all
# ###########################################
# all: $(BUILD_DIR)/$(TARGET)

# #list of objects from c files
# OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
# vpath %.c $(sort $(dir $(C_SOURCES)))
# # list of objects from c++ file
# OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
# vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

# $(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
# 	$(CXX) -c $(CXXFLAGS) $< -o $@

# $(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
# 	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# $(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
# 	$(CXX) $(OBJECTS)  $(LDFLAGS) -o $@
# 	$(SZ) $@

# $(BUILD_DIR):
# 	mkdir $@		

# #######################################
# # clean up
# #######################################
# clean:
# 	-rm -fR $(BUILD_DIR)
  
# #######################################
# # dependencies
# #######################################
# -include $(wildcard $(BUILD_DIR)/*.d)
