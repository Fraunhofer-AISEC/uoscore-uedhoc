# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(auto_test)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

# The external static library that we are linking with does not know
# how to build for this platform so we export all the flags used in
# this zephyr build to the external build system.
zephyr_get_include_directories_for_lang_as_string(       C includes)
zephyr_get_system_include_directories_for_lang_as_string(C system_includes)
zephyr_get_compile_definitions_for_lang_as_string(       C definitions)
zephyr_get_compile_options_for_lang_as_string(           C options)

set(external_project_cflags
  "${includes} ${definitions} ${options} ${system_includes}"
  )

include(ExternalProject)

# Add an external project to be able download and build the third
# party library. In this case downloading is not necessary as it has
# been committed to the repository.
set(lib_test_dir   ${CMAKE_CURRENT_SOURCE_DIR}/lib_test)
set(build_lib_test_dir ${CMAKE_CURRENT_SOURCE_DIR}/build_lib_test)

set(LIB_TEST_LIB_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/build_lib_test)
set(LIB_EDHOC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../modules/edhoc)
set(LIB_OSCORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../modules/oscore)

if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
# https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html
set(submake "$(MAKE)")
else() # Obviously no MAKEFLAGS. Let's hope a "make" can be found somewhere.
set(submake "make")
endif()


# message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
# message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
# message("LIB_TEST_LIB_DIR: ${LIB_TEST_LIB_DIR}")
# message("LIB_EDHOC_INCLUDE_DIR: ${LIB_EDHOC_INCLUDE_DIR}")


ExternalProject_Add(
  libtest                 # Name for custom target
  PREFIX     ${build_lib_test_dir} # Root dir for entire project
  SOURCE_DIR ${lib_test_dir}
  BINARY_DIR ${lib_test_dir} # This particular build system is invoked from the root
  CONFIGURE_COMMAND ""    # Skip configuring the project, e.g. with autoconf
  BUILD_COMMAND
  ${submake}
  PREFIX=${build_lib_test_dir}
  CC=${CMAKE_C_COMPILER}
  AR=${CMAKE_AR}
  CFLAGS=${external_project_cflags}
  INSTALL_COMMAND ""      # This particular build system has no install command
  BUILD_BYPRODUCTS ${LIB_TEST_LIB_DIR}/libtest.a
  )

# Create a wrapper CMake library that our app can link with
add_library(test STATIC IMPORTED GLOBAL)
add_dependencies(
  test
  libtest
  )
set_target_properties(test PROPERTIES IMPORTED_LOCATION             ${LIB_TEST_LIB_DIR}/libtest.a)
target_include_directories(test INTERFACE
  ${LIB_EDHOC_INCLUDE_DIR}
  ${LIB_OSCORE_INCLUDE_DIR}
)


target_link_libraries(app PRIVATE test)
