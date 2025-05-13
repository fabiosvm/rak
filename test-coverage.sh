#!/bin/bash

./clean.sh

cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="--coverage"
cmake --build build

./test.sh -q

mkdir -p coverage
rm -Rf coverage/*

if command -v lcov &> /dev/null; then
    lcov --capture --directory . --output-file coverage/coverage.info
    genhtml coverage/coverage.info --output-directory coverage/lcov_report
    echo "Coverage report generated with 'lcov': coverage/lcov_report/index.html"
elif command -v gcovr &> /dev/null; then
    gcovr -r . --html --html-details -o coverage/coverage_report.html
    echo "Coverage report generated with 'gcovr': coverage/coverage_report.html"
else
    echo "Error: Neither 'lcov' nor 'gcovr' is installed."
    echo "Please install one of them to run 'test-coverage' again."
    exit 1
fi
