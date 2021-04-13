# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

include ../build_lib_test/opt

# $(info    LIB_NAME is $(LIB_NAME))
# $(info    PREFIX is $(PREFIX))
# $(info    SOURCE_DIR is $(SOURCE_DIR))
# $(info    BINARY_DIR is $(BINARY_DIR))
# $(info    CC is $(CC))
# $(info    CFLAGS is $(CFLAGS))
# $(info    OPT is $(OPT))

################################################################################
# Build directory
DIR = $(PREFIX)

################################################################################
# Source files
DO_NOT_COMPILE_SOURCES = \
../../externals/tinycbor/src/open_memstream.c \
../../externals/tinycbor/src/cbortojson.c \
../../externals/tinycbor/src/cborpretty.c \
../../externals/tinycbor/src/cborencoder_close_container_checked.c \
../../externals/tinycbor/src/cborvalidation.c \
../../externals/tinycbor/src/cborparser_dup_string.c \
../../externals/tinycbor/src/cborpretty_stdio.c \
../../externals/tinycbor/src/cborerrorstrings.c \
../../externals/tinycrypt/lib/source/ctr_prng.c \
../../externals/tinycrypt/lib/source/ecc_dh.c \
../../externals/tinycrypt/lib/source/cbc_mode.c \
../../externals/tinycrypt/lib/source/hmac_prng.c \
../../externals/tinycrypt/lib/source/ctr_mode.c \
../../externals/tinycrypt/lib/source/ecc_platform_specific.c \
../../externals/tinycrypt/lib/source/cmac_mode.c \
../../externals/tinycrypt/lib/source/ecc.c \
../../externals/tinycrypt/lib/source/ecc_dsa.c 



ifeq	($(LIB_NAME), libuedhoc.a) 
	C_SOURCES = $(wildcard ../../modules/edhoc/src/*.c)
	C_SOURCES += $(wildcard ../../externals/compact25519/src/c25519/*.c)
	C_SOURCES += $(wildcard ../../externals/compact25519/src/*.c)
endif 

ifeq	($(LIB_NAME), libuoscore.a) 
	C_SOURCES = $(wildcard ../../modules/oscore/src/*.c)
endif 

C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../externals/tinycbor/src/*.c))
C_SOURCES += $(filter-out $(DO_NOT_COMPILE_SOURCES), $(wildcard ../../externals/tinycrypt/lib/source/*.c))

#$(info    \n C_SOURCES is $(C_SOURCES))
vpath %.c $(sort $(dir $(C_SOURCES)))

################################################################################
# C includes
ifeq	($(LIB_NAME), libuedhoc.a) 
	C_INCLUDES =  \
	-I../../externals/tinycbor/src/ \
	-I../../externals/compact25519/src/c25519/ \
	-I../../externals/compact25519/src/ \
	-I../../externals/tinycrypt/lib/include
endif 

ifeq	($(LIB_NAME), libuoscore.a) 
	C_INCLUDES =  \
	-I../../externals/tinycbor/src/ \
	-I../../externals/tinycrypt/lib/include 
endif 

################################################################################
# CFLAGS
################################################################################
FILTERED_CFLAGS = -Os
CFLAGS1 = $(filter-out $(FILTERED_CFLAGS), $(CFLAGS))

#add optimization
CFLAGS1 += $(OPT)

#add include paths
CFLAGS1 += $(C_INCLUDES)

#generate debug symbols
#CFLAGS1 += -g -gdwarf-2

# Generate dependency information
#CFLAGS1 += -MMD -MP -MF"$(@:%.o=%.d)"

ifeq	($(LIB_NAME), libuedhoc.a) 
	#use tyncrypt + ed25519
	CFLAGS1 += -DEDHOC_WITH_TINYCRYPT_AND_C25519
	CFLAGS1 += -DEDHOC_DEBUG_PRINT
endif 

ifeq	($(LIB_NAME), libuoscore.a) 
	#use tyncrypt 
	CFLAGS1 += -DOSCORE_WITH_TINYCRYPT
	#CFLAGS1 += -DOSCORE_DEBUG_PRINT
endif 

#$(info    CFLAGS1 is $(CFLAGS1))
################################################################################
# build the library
################################################################################
OBJ = $(addprefix $(DIR)/,$(notdir $(C_SOURCES:.c=.o)))
#$(info    \n OBJ is $(OBJ))

$(DIR)/libtest.a: $(OBJ)
	@echo "[Link (Static)]"
	@$(AR) -rcs $@ $^

$(DIR)/%.o: %.c
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS1) $< -o $@

