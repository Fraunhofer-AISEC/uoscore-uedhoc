#!/usr/bin/env python3
# Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
# file at the top-level directory of this distribution.

# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.


import shutil
import os
import subprocess
import tarfile
from pathlib import Path
from collections import namedtuple

arc = namedtuple("arc", "board, cpu_arc")


build_path = 'build'
build_lib_test_path = 'build_lib_test'
results_path = 'packaged'


def remove_folder(path):
    """
    Removes a folder. 
    """
    if os.path.exists(path):
        shutil.rmtree(path)


def clean_all():
    """
    Removes all build artefacts and the already saved static libraries in
    folder packaged/. 
    """
    print("\nClean all!\n")
    clean()
    remove_folder(results_path)


def clean():
    """
    Removes all build artefacts. 
    """
    remove_folder(build_path)
    remove_folder(build_lib_test_path)


def execute_ext(cmd):
    """
    Executes an external program.
    cmd: program with arguments
    """
    process = subprocess.Popen(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process.wait()
    for line in process.stdout:
        print(line)
        if "FAIL" in str(line):
            exit()


def build(name, opt, arc):
    """
    Builds a static library.
    name: name of the library -- libuoscore.a or libuedhoc.a
    opt: optimization level
    arc: the name of the architecture (the Zephyr OS board name)
    """
    # crate a file containing make variable indicating the optimization level
    # and the library which we want to build -- osocre or edhoc
    print("\n")
    print("===================================================================")
    print("\nBuilding " + name + " for architecture " +
          arc.cpu_arc + " with optimization " + opt + "\n")
    print("===================================================================")

    os.mkdir(build_lib_test_path)
    f = open(build_lib_test_path + "/opt", "x")
    f.write("OPT = " + opt + "\n")
    f.write("LIB_NAME = " + name + "\n")
    f.close()

    m = open("src/main.h", "w+")
    if (name == 'libuoscore.a'):
        m.write("#define OSCORE_TESTS")

    if (name == 'libuedhoc.a'):
        m.write("#define EDHOC_TESTS")
    m.close

    # build with west
    execute_ext(['west', 'build', '-b='+arc.board])


def save(name, arc):
    """
    Saves a oscore or edhoc library for a specific architecture in folder 
    packaged.
    name: name of the library -- libuoscore.a or libuedhoc.a
    arc: the name of the architecture (the Zephyr OS board name)
    """
    print("\nSaving!\n")
    Path(results_path).mkdir(parents=True, exist_ok=True)
    name_only = os.path.splitext(os.path.basename(name))[0]

    t = tarfile.open(results_path + '/' + name_only +
                     '_' + arc.cpu_arc + '.tar.gz', 'x')
    t.add(build_lib_test_path + '/' + 'libtest.a', arcname=name)
    if (name == 'libuedhoc.a'):
        t.add('../modules/edhoc/edhoc.h', arcname='edhoc.h')

    if (name == 'libuoscore.a'):
        t.add('../modules/oscore/oscore.h', arcname='oscore.h')

    t.close()


def test(arc):
    """
    Tests a static library agains the test vectors.
    arc: architecture
    """
    if (
            (arc.board == 'native_posix_64') |
            (arc.board == 'native_posix')):

        print("\nTesting!\n")
        execute_ext(['west', 'build', '-t', 'run'])
    else:
        execute_ext(['west', 'flash'])
        input(
            "Examine the results printed over the debugger and press Enter to continue...")


def run_tests(name, arc):
    """
    Builds, tests and saves an oscore or an edhoc static library for a specific
    architecture. The tests are executed for libraries build with different 
    optimizations.
    name: name of the library -- libuoscore.a or libuedhoc.a
    arc: the name of the architecture (the Zephyr OS board name)
    """
    opt = ("-O0", "-O1", "-O2", "-O3")
    for o in opt:
        clean()
        build(name, o, arc)
        test(arc)
    save(name, arc)


def main():
    """
    Builds static libraries from uOSCORE and uEDHOC for different 
    architectures, tests the libraries agains the test vectors and saves the 
    tested libraries in the folder packaged   
    """
    clean_all()

    # build('libuedhoc.a', '-O0', arc('native_posix', 'x86'))
    # test(arc('native_posix', 'x86'))

    # x86
    run_tests('libuoscore.a', arc('native_posix', 'x86'))
    #run_tests('libuedhoc.a', arc('native_posix', 'x86'))

    # x86-64
    run_tests('libuoscore.a', arc('native_posix_64', 'x86-64'))
    #run_tests('libuedhoc.a', arc('native_posix_64', 'x86-64'))

    # to run the following tests a real hardware must be connect to the PC
    # executing this script. The results of the test can be examined over a serial consol such as GTKterm

    # Cortex M0
    #run_tests('libuoscore.a', arc('nrf51dk_nrf51422', 'cortex-m0'))
    #run_tests('libuedhoc.a', arc('nrf51dk_nrf51422', 'cortex-m0'))

    # Cortex M3
    #run_tests('libuoscore.a', arc('nucleo_l152re', 'cortex-m3'))
    #run_tests('libuedhoc.a', arc('nucleo_l152re', 'cortex-m3'))

    # Cortex M4
    #run_tests('libuoscore.a', arc('nrf52dk_nrf52832','cortex-m4'))
    #run_tests('libuedhoc.a',  arc('nrf52dk_nrf52832','cortex-m4'))
    #run_tests('libuoscore.a',  arc('nrf52840dk_nrf52840','cortex-m4'))
    #run_tests('libuedhoc.a',  arc('nrf52840dk_nrf52840','cortex-m4'))

    # Cortex M33
    #run_tests('libuoscore.a', arc('nrf9160dk_nrf9160', 'cortex-m33'))
    #run_tests('libuedhoc.a', arc('nrf9160dk_nrf9160', 'cortex-m33'))


if __name__ == "__main__":
    main()
