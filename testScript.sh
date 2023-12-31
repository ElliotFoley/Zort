#!/bin/bash

rm a.out
gcc sort.c -o zort
rm -r "Test 1"
cp -r "Test 1 (copy)/" "Test 1/"
cd "Test 1/"
../a.out -d -v
ls
cd ..
