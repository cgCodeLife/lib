#!/bin/bash

set -exu

ulimit -c unlimited

SOURCE_DIR=`pwd`
LIB_UNITTEST_DIR=${SOURCE_DIR}/build/lib/${1}

if [[ ! -n "${1}" || ! -d ${LIB_UNITTEST_DIR}  ]]; then
    echo "INVALID PATH: ${LIB_UNITTEST_DIR}"
    exit 1
fi

cd ${LIB_UNITTEST_DIR}
rm -rf valgrind_ignore.supp
cp ${SOURCE_DIR}/scripts/valgrind_ignore.supp ./
cat ${SOURCE_DIR}/test/unittest/glibc_ignore.supp >> valgrind_ignore.supp
CTEST_OUTPUT_ON_FAILURE=1 timeout 900 make test ARGS=-j`grep -c ^processer /proc/cpuinfo 2>/dev/null` && \
python ${SOURCE_DIR}/scripts/parallel_testing.py
