# output as png image
set terminal png

# save file
set output 'images/benchmark_combined_openmp.png'

# graph title
set title 'Matrix norms (OpenMP)'

# y-axis grid
set grid y

#x-axis label
set xlabel "Matrix Size (n)"

#y-axis label
set ylabel "Time Taken (ms)"

set key left

set style line 1 \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 0.5

#plot data from output data
plot 'data/benchmark_parallel_openmp4.dat' with linespoints linestyle 1 lc rgb "dark-magenta" title "4 threads", \
     'data/benchmark_parallel_openmp.dat' with linespoints linestyle 1 lc rgb "orange-red" title "8 threads", \
     'data/benchmark_parallel_openmp16.dat' with linespoints linestyle 1 lc rgb "web-blue" title "16 threads", \
     'data/benchmark_parallel_openmp16.dat' with linespoints linestyle 1 lc rgb "web-green" title "32 threads"