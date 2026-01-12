#!/bin/sh

make -j6 ASAN=1 NAT=1 all test
