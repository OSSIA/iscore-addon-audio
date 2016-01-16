#!/bin/bash -eux
set -o pipefail
export PS4='+ ${FUNCNAME[0]:+${FUNCNAME[0]}():}line ${LINENO}: '

CMAKE_BIN=$(which cmake)
export CMAKE_COMMON_FLAGS="-GNinja"
export CTEST_OUTPUT_ON_FAILURE=1

mkdir -p build
cd build

export CONFIG_FOLDER=$(pwd)/../tools/travis/configs/
source "$CONFIG_FOLDER/$CONF.sh"

