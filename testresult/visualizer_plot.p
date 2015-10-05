set style line 1 lc rgb "#BA55D3"
set style line 2 lc rgb "#3CB371"

set style fill solid
set boxwidth 0.5
set ylabel "Time(ms)"

set title "Visualizer allocation (malloc vs custom stack allocator)"
set output "Visualizer.png" 
set term png size 600, 300

plot "visualizer_data.txt" every ::0::1 using 1:3:xtic(2) with boxes title 'malloc' ls 1, \
     "visualizer_data.txt" every ::2::3 using 1:3:xtic(2) with boxes title 'Stack allocator' ls 2