#!/bin/bash
testsmd=./TESTS.md
rm $testsmd

echo "# E4S Testsuite List" >> $testsmd

echo "## All CUDA Enabled Tests" >> $testsmd
for dir in $(ls -d ./validation_tests/*/ | grep 'cuda'); do
        echo "- $(basename ${dir%/})" >> $testsmd
done

echo "## All ROCM Enabled Tests" >> $testsmd
for dir in $(ls -d ./validation_tests/*/ | grep 'rocm'); do
        echo "- $(basename ${dir%/})" >> $testsmd
done

echo "## All SYCL Enabled Tests" >> $testsmd
for dir in $(ls -d ./validation_tests/*/ | grep 'sycl'); do
        echo "- $(basename ${dir%/})" >> $testsmd
done

echo "## All E4S Tests" >> $testsmd
for dir in $(ls -d ./validation_tests/*/); do
	echo "- $(basename ${dir%/})" >> $testsmd
done
