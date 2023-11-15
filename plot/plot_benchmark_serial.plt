# output as png image
set terminal png

# save file
set output 'images/benchmark_serial.png'

# graph title
set title 'Matrix norms (serial)'

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
plot 'data/benchmark_serial.dat' with linespoints linestyle 1 lc rgb "web-blue" title "Serial"