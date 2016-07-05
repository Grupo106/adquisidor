#!/usr/bin/env bash
TEST_PATH=bin/tests
TEST_SRC=tests
SRC=src

mkdir -p $TEST_PATH
gcc -v -lpcap -g -Wl,--wrap=bd_insertar,--wrap=bd_commit \
    -o $TEST_PATH/test_adquisidor $SRC/adquisidor.c $TEST_SRC/test_adquisidor.c

$TEST_PATH/test_adquisidor
