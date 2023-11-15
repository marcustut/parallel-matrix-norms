#!/bin/bash

# Exit on error
set -e

# Build the tests
make test

# Find and run the test binaries
find bin -name 'test_*' -exec ./{} ';'