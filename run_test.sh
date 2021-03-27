#!/bin/bash

set -e

timeout -s 9 3600 bash -c "
set -x;
./test/unittest/run.sh base
"
