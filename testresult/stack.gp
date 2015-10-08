set style line 1 lc rgb "#BA55D3"
set style line 2 lc rgb "#3CB371"

set style fill solid
set boxwidth 0.5
set ylabel "Time(ms)"

set title "Stack allocation (malloc vs custom stack allocator) 50 331 648 allocations"
set output "Stack.png" 
set term png size 600, 300

plot "stack_data.txt" every ::0::1 using 1:3:xtic(2) with boxes title 'malloc' ls 1, \
     "stack_data.txt" every ::2::3 using 1:3:xtic(2) with boxes title 'Stack allocator' ls 2

set title "Stack allocation (malloc vs custom stack allocator) 12 582 912 allocations"
set output "Stack2.png" 
set term png size 600, 300

plot "stack_data.txt" every ::4::5 using 1:3:xtic(2) with boxes title 'malloc' ls 1, \
     "stack_data.txt" every ::6::7 using 1:3:xtic(2) with boxes title 'Stack allocator' ls 2