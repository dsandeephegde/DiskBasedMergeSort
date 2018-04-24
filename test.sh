#!/usr/bin/env bash
cc generateTestFiles.c
./a.out $1
cc main.c
./a.out --basic input.bin sortOut.bin
cmp sortOut.bin sort.bin
exit_status=$?
if [ $exit_status -eq 1 ]; then
    echo "Fail"
else
    echo "Pass"
fi
./a.out --multistep input.bin sortOut.bin
cmp sortOut.bin sort.bin
exit_status=$?
if [ $exit_status -eq 1 ]; then
    echo "Fail"
else
    echo "Pass"
fi
./a.out --replacement input.bin sortOut.bin
cmp sortOut.bin sort.bin
exit_status=$?
if [ $exit_status -eq 1 ]; then
    echo "Fail"
else
    echo "Pass"
fi
