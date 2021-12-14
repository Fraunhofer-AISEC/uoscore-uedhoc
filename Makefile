# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.

LIB_NAME = libuoscore-uedhoc.a

include makefile_config.mk

$(info    CC is $(CC))
# $(info    LIB_NAME is $(LIB_NAME))
# $(info    PREFIX is $(PREFIX))
# $(info    SOURCE_DIR is $(SOURCE_DIR))
# $(info    BINARY_DIR is $(BINARY_DIR))
# $(info    CC is $(CC))
# $(info    CFLAGS is $(CFLAGS))
# $(info    OPT is $(OPT))
# $(info    CRYPTO_ENGINE is $(CRYPTO_ENGINE))
################################################################################
# Build directory
PREFIX ?= build
DIR = $(PREFIX)
$(shell mkdir -p $(DIR))
################################################################################
# C Source files

# common source files for OSCORE and EDHOC
C_SOURCES += $(wildcard modules/common/src/*.c)

# EDHOC specific source files
C_SOURCES += $(wildcard modules/edhoc/src/*.c)
C_SOURCES += $(wildcard modules/edhoc/cbor/*.c)

# OSCORE specific source files
C_SOURCES += $(wildcard modules/oscore/src/*.c)
C_SOURCES += $(wildcard modules/oscore/cbor/*.c)

# Crypto engine
#ifeq ($(findstring COMPACT25519,$(CRYPTO_ENGINE)),COMPACT25519)
#C_SOURCES += $(wildcard ../../externals/compact25519/src/c25519/*.c)
#C_SOURCES += $(wildcard ../../externals/compact25519/src/*.c)
#endif

#ifeq ($(findstring TINYCRYPT,$(CRYPTO_ENGINE)),TINYCRYPT)
#C_SOURCES += $(wildcard ../../externals/tinycrypt/lib/source/*.c)
#endif

#ifeq ($(findstring MBEDTLS,$(CRYPTO_ENGINE)),MBEDTLS)
#C_SOURCES += $(wildcard ../../externals/mbedtls/library/*.c)
#C_SOURCES += $(wildcard ../../externals/mbedtls_ecp_compression/#ecc_point_compression.c)
#endif

# CBOR engine
#ifeq ($(findstring CDDL_GEN,$(CBOR_ENGINE)),CDDL_GEN)
#C_SOURCES += $(wildcard ../../externals/cddl-gen/src/*.c)
#endif


#$(info    \n C_SOURCES is $(C_SOURCES))
vpath %.c $(sort $(dir $(C_SOURCES)))

################################################################################
# libraries 


################################################################################
# C includes

# common include files for OSCORE and EDHOC
C_INCLUDES += -Imodules/common/inc

# EDHOC specific include files
C_INCLUDES += -Imodules/edhoc/inc
C_INCLUDES += -Imodules/edhoc

# OSCORE specific inlude files
C_INCLUDES += -Imodules/oscore/inc
C_INCLUDES += -Imodules/oscore

# Crypto engine
ifeq ($(findstring COMPACT25519,$(CRYPTO_ENGINE)),COMPACT25519) 
C_INCLUDES += -Iexternals/compact25519/src/c25519/ 
C_INCLUDES += -Iexternals/compact25519/src/ 
endif

ifeq ($(findstring TINYCRYPT,$(CRYPTO_ENGINE)),TINYCRYPT)
C_INCLUDES += -Iexternals/tinycrypt/lib/include
endif
 
ifeq ($(findstring MBEDTLS,$(CRYPTO_ENGINE)),MBEDTLS)
C_INCLUDES += -Iexternals/mbedtls/library 
C_INCLUDES += -Iexternals/mbedtls/include 
C_INCLUDES += -Iexternals/mbedtls/include/mbedtls 
C_INCLUDES += -Iexternals/mbedtls/include/psa 
C_INCLUDES += -Iexternals/mbedtls_ecp_compression
endif

# CBOR engine
ifeq ($(findstring CDDL_GEN,$(CBOR_ENGINE)),CDDL_GEN)
C_INCLUDES += -Iexternals/cddl-gen/include
endif

################################################################################
# CFLAGS
################################################################################
#add options form configuration file 
CFLAGS += $(ARCH)
CFLAGS += $(OPT)
CFLAGS += $(DEBUG_PRINT)
CFLAGS += $(CBOR_ENGINE)
CFLAGS += $(CRYPTO_ENGINE)

#add include paths
CFLAGS += $(C_INCLUDES)

#generate debug symbols
CFLAGS += -g -gdwarf-2

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"



#GCC warning flags
ifeq ($(CC),cc)
CFLAGS += -Waddress
CFLAGS += -Waggregate-return
CFLAGS += -Wformat-nonliteral
CFLAGS += -Wformat-security
CFLAGS += -Wformat
CFLAGS += -Winit-self
CFLAGS += -Wmissing-include-dirs
CFLAGS += -Wno-multichar
CFLAGS += -Wno-parentheses
CFLAGS += -Wno-type-limits
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wunreachable-code
CFLAGS += -Wwrite-strings
CFLAGS += -Wpointer-arith
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wcast-qual
CFLAGS += -Wstack-usage=4096
CFLAGS += -Wconversion
CFLAGS += -Wpedantic
#CFLAGS += -Werror

#Clang warning flahs
else ifeq ($(CC),clang-13)
CFLAGS += -Wcast-qual
CFLAGS += -Wconversion
CFLAGS += -Wexit-time-destructors
CFLAGS += -Wglobal-constructors
CFLAGS += -Wmissing-noreturn
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wno-missing-braces
CFLAGS += -Wold-style-cast
#CFLAGS += -Wshadow
CFLAGS += -Wweak-vtables
CFLAGS += -Wall
#CFLAGS += -Wextra
CFLAGS += -Wpedantic
CFLAGS += -Wstack-exhausted
CFLAGS += -Wconversion
#CFLAGS += -Werror
endif

# use AddressSanitizer to find memory bugs
# comment this out for better speed
#CFLAGS += -fsanitize=address -fno-omit-frame-pointer
#CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
#LDFLAGS += -fsanitize=address -static-libasan

#$(info    CFLAGS is $(CFLAGS))
################################################################################
# build the library
################################################################################
OBJ = $(addprefix $(DIR)/,$(notdir $(C_SOURCES:.c=.o)))
#$(info    \n OBJ is $(OBJ))


$(DIR)/$(LIB_NAME): $(OBJ) 
	@echo "[Link (Static)]"
	@$(AR) -rcs $@ $^

$(DIR)/%.o: %.c Makefile makefile_config.mk
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -fR $(DIR)