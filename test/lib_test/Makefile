# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

LIB_NAME = uoscore-uedhoc.a

include ../lib_test/makefile_config.mk

# $(info    LIB_NAME is $(LIB_NAME))
# $(info    PREFIX is $(PREFIX))
# $(info    SOURCE_DIR is $(SOURCE_DIR))
# $(info    BINARY_DIR is $(BINARY_DIR))
# $(info    CC is $(CC))
# $(info    CFLAGS is $(CFLAGS))
# $(info    OPT is $(OPT))
$(info    CRYPTO_ENGINE is $(CRYPTO_ENGINE))
################################################################################
# Build directory
DIR = $(PREFIX)

################################################################################
# C Source files

# common source files for OSCORE and EDHOC

# EDHOC specific source files
C_SOURCES += $(wildcard ../../modules/edhoc/src/*.c)
C_SOURCES += $(wildcard ../../modules/edhoc/cbor/*.c)

# OSCORE specific source files
#C_SOURCES = $(wildcard ../../modules/oscore/src/*.c)
#C_SOURCES += $(wildcard ../../modules/oscore/cbor/*.c)

# Crypto engine
ifeq ($(findstring COMPACT25519,$(CRYPTO_ENGINE)),COMPACT25519)
C_SOURCES += $(wildcard ../../externals/compact25519/src/c25519/*.c)
C_SOURCES += $(wildcard ../../externals/compact25519/src/*.c)
endif

ifeq ($(findstring TINYCRYPT,$(CRYPTO_ENGINE)),TINYCRYPT)
C_SOURCES += $(wildcard ../../externals/tinycrypt/lib/source/*.c)
endif

ifeq ($(findstring MBEDTLS,$(CRYPTO_ENGINE)),MBEDTLS)

endif

# CBOR engine
ifeq ($(findstring CDDL_GEN,$(CBOR_ENGINE)),CDDL_GEN)
C_SOURCES += $(wildcard ../../externals/cddl-gen/src/*.c)
endif



#$(info    \n C_SOURCES is $(C_SOURCES))
vpath %.c $(sort $(dir $(C_SOURCES)))

################################################################################
# C includes

# common include files for OSCORE and EDHOC


# EDHOC specific include files


# OSCORE specific inlude files


# Crypto engine
ifeq ($(findstring COMPACT25519,$(CRYPTO_ENGINE)),COMPACT25519) 
C_INCLUDES += -I../../externals/compact25519/src/c25519/ 
C_INCLUDES += -I../../externals/compact25519/src/ 
endif

ifeq ($(findstring TINYCRYPT,$(CRYPTO_ENGINE)),TINYCRYPT)
C_INCLUDES += -I../../externals/tinycrypt/lib/include
endif

ifeq ($(findstring MBEDTLS,$(CRYPTO_ENGINE)),MBEDTLS)

endif

# CBOR engine
ifeq ($(findstring CDDL_GEN,$(CBOR_ENGINE)),CDDL_GEN)
C_INCLUDES += -I../../externals/cddl-gen/include
endif

################################################################################
# CFLAGS
################################################################################
FILTERED_CFLAGS = -Os
CFLAGS1 = $(filter-out $(FILTERED_CFLAGS), $(CFLAGS))

#add options form configuration file 
CFLAGS1 += $(OPT)
CFLAGS1 += $(DEBUG_PRINT)
CFLAGS1 += $(CBOR_ENGINE)
CFLAGS1 += $(CRYPTO_ENGINE)

#add include paths
CFLAGS1 += $(C_INCLUDES)

#generate debug symbols
CFLAGS1 += -g -gdwarf-2

# Generate dependency information
CFLAGS1 += -MMD -MP -MF"$(@:%.o=%.d)"

# required for gddl-gen library
CFLAGS1 += -DCDDL_CBOR_CANONICAL 


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

