#!/bin/bash -eux

git submodule update --init --recursive
# we are in ~/i-score
./tools/travis/deps.sh

# TODO install faust.
case "$TRAVIS_OS_NAME" in
  linux)
  ;;
  osx)
    set +e
    set -e
  ;;
esac
