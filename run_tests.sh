#!/usr/bin/env bash
TEST_PATH=bin/tests
TEST_SRC=tests
SRC=src
LINK_FLAGS="-lpcap"

mkdir -p $TEST_PATH
gcc  -g -Wall -Wl,--wrap=bd_insertar,--wrap=bd_commit \
     -o $TEST_PATH/test_adquisidor \
     $SRC/adquisidor.c $TEST_SRC/test_adquisidor.c $LINK_FLAGS

$TEST_PATH/test_adquisidor
exit
