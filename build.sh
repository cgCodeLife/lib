#!/bin/bash

help_message="$(basename "$0") [-mode=<release|debug>] [--enable_gcov] [--enable_asan] [-h]

where:
    -h|--help       Show this help message.
    -mode=<mode>   Build mode. Can be one of 'release' or 'debug'. If not specified, will use RelWithDebInfo by default.
    --enable_asan  Enable address sanitizer
    --enable_gcov   Enable gcov for coverage test"

BUILD_MODE="debug"
ENABLE_GCOV=false
ENABLE_ASAN=false
BUILD_VERSION=""

for i in "$@"
do
    case $i in
        -h|--help)
            echo "$help_message"
            exit
            ;;
        --mode=*)
            BUILD_MODE="${i#*=}"
            shift
            ;;
        --enable_gcov)
            ENABLE_GCOV=true
            shift
            ;;
        --enable_asan)
            ENABLE_ASAN=true
            shift
            ;;
        *)
            echo "unknow option ${i}"
            echo "$help_message"
            exit
            ;;
    esac
done

set -ex

NUM_CPU=`grep -c ^processor /proc/cpuinfo 2>/dev/null`
SOURCE_DIR=`pwd`
BUILD_DIR=$SOURCE_DIR/build
BUILD_TYPE="unknown"
PACKAGE_DIR=$SOURCE_DIR/pkg
GCOV=OFF
ASAN=OFF

git submodule update --init --recursive

if [ $BUILD_VERSION ]; then
	mkdir -p $PACKAGE_DIR/ \
		&& cd $PACKAGE_DIR/ \
		&& wget "https://cmake.org/files/v3.9/cmake-3.9.2.tar.gz" \
		&& tar -zxf "cmake-3.9.2.tar.gz" \
		&& cd "cmake-3.9.2/" \
		&& ./configure --parallel=$NUM_CPU \
		&& make -j$NUM_CPU \
		&& sudo make install \
		&& cd $SOURCE_DIR/ \
		&& echo "Build Cmake Completed"
fi

if [ "${BUILD_MODE}" == "debug" ]; then
	BUILD_TYPE="Debug"
elif  [ "${BUILD_MODE}" == "release" ]; then
	BUILD_TYPE="Release"
else
	BUILD_TYPE="RelWithDebInfo"
fi

GCC_VERSION=""
GCC_DUMP_VERSION=`gcc -dumpversion | cut -f1 -d.`
if [ $GCC_DUMP_VERSION -lt 6  ];
then
        GCC_VERSION="4.9.2"
    else
        GCC_VERSION="6.3.0"
fi
echo "GCC_VERSION = $GCC_VERSION"

if [ "$ENABLE_GCOV" = true ]; then
    GCOV="ON"
fi

if [ "$ENABLE_ASAN" = true ]; then
    ASAN="ON"
fi

ok=1
mkdir -p $BUILD_DIR/ \
		&& cd $BUILD_DIR/ \
	    && cmake -DGCC_VERSION=${GCC_VERSION} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DBUILD_TESTING=ON -DENABLE_PROFILER=ON -DBUILD_GF_COMPLETE=ON  -DENABLE_GCOV=${GCOV} -DENABLE_ASAN=${ASAN} $SOURCE_DIR \
        && make -j$NUM_CPU \
        && cd .. \
        && find ./lib/ -name \*.h -print -o -name \*.cc -print | split -l 10 -d -a 3 && mv x??? $BUILD_DIR/ \
        && rm -rf $BUILD_DIR/cpplint.result && for file in `ls $BUILD_DIR/x???`; do python scripts/cpplint.py `cat $file` 2>> $BUILD_DIR/cpplint.result & done && wait \
        && rm -rf $BUILD_DIR/x[0-9]?? && cat $BUILD_DIR/cpplint.result | grep -v "Total errors found" && ! (grep "Total errors found" $BUILD_DIR/cpplint.result) \
        && ok=0

if [ 0 -eq ${ok} ]; then
    echo "Build Completed"
fi
exit ${ok}
