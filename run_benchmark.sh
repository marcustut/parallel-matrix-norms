#!/bin/bash

# Exit on error
set -e

# Build the benchmarks
make benchmark

# Find and run the benchmark binaries
find bin -name 'benchmark_*' -exec ./{} ';'

# Plot the results
# gnuplot plot/plot_benchmark_serial.plt
# gnuplot plot/plot_benchmark_parallel.plt
# gnuplot plot/plot_benchmark_combined.plt