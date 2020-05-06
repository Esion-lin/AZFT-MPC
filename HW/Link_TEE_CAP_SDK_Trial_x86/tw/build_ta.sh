#!/bin/bash

TEE_TOP_PATH=../..

echo "Build Internal Lib...."
make clean
make
echo "Copy Client Lib"
cp ../ntw/lib/libtee_client.so ../out
echo "************Build Finished**************"
cd ../out
echo "libtee_internal.so generated! Path:$(pwd)"

