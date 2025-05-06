#!/bin/bash

if [ ! -d "tests" ]; then
    echo "Error: 'tests' directory not found." >&2
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is not installed or not in PATH." >&2
    exit 1
fi

python3 tests/run.py -e "build/rak" "$@"
