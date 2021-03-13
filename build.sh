#!/bin/sh

set -x

git submodule update --init --recursive

SOURCE_DIR=`pwd`
BUILD_DIR=$SOURCE_DIR/build
NUM_CPU_CORES=`cat /proc/cpuinfo |grep processor -c`
mkdir -p $BUILD_DIR/ \
    && cd $BUILD_DIR/ \
    && cmake $SOURCE_DIR -DBUILD_TESTING=ON -DBYTE_BUILD_TESTS=ON -DBUILD_MSGPACK=ON -DBUILD_GF_COMPLETE=ON -DBUILD_JERASURE=ON \
    && make -j${NUM_CPU_CORES} \
    && cd .. && python scripts/cpplint.py `find byte/ -name \*.h -print -o -name \*.cc -print`
