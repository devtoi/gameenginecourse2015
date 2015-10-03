set   autoscale                        # scale axes automatically
set term png
set datafile separator ","
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set xlabel "Blocksize"
set ylabel "Time(ms)"

set title "Simple allocation"
set output "SimpleAlloc.png" 
plot    "./SimpleSTDalloc.txt" using 1:2 title 'Standard' with lines smooth unique, \
        "./SimpleAllocPoolShared.txt" using 1:2 title 'Shared Pool' with lines smooth unique, \
        "./SimpleAllocPoolThreadLocal.txt" using 1:2 title 'Thread local Pool' with lines smooth unique

set title "Simple deallocation"
set output "SimpleDealloc.png" 
plot    "./SimpleSTDdealloc.txt" using 1:2 title 'Standard' with lines smooth unique, \
        "./SimpleDeallocPoolShared.txt" using 1:2 title 'Shared Pool' with lines smooth unique, \
        "./SimpleDeallocPoolThreadLocal.txt" using 1:2 title 'Thread local Pool' with lines smooth unique

set title "Concurrency 1 thread"
set output "Concurrency 1 thread.png" 
plot    "<awk -F, '{if($2 == 1){print $0}}' './Pool concurrency standard test.txt'" using 1:3 title 'Standard' with lines smooth unique, \
        "<awk -F, '{if($2 == 1){print $0}}' './Pool concurrency shared test.txt'" using 1:3 title 'Shared Pool' with lines smooth unique, \
        "<awk -F, '{if($2 == 1){print $0}}' './Pool concurrency thread local.txt'" using 1:3 title 'Thread local Pool' with lines smooth unique

set title "Concurrency 2 thread"
set output "Concurrency 2 thread.png" 
plot    "<awk -F, '{if($2 == 2){print $0}}' './Pool concurrency standard test.txt'" using 1:3 title 'Standard' with lines smooth unique, \
        "<awk -F, '{if($2 == 2){print $0}}' './Pool concurrency shared test.txt'" using 1:3 title 'Shared Pool' with lines smooth unique, \
        "<awk -F, '{if($2 == 2){print $0}}' './Pool concurrency thread local.txt'" using 1:3 title 'Thread local Pool' with lines smooth unique

set title "Concurrency 4 thread"
set output "Concurrency 4 thread.png" 
plot    "<awk -F, '{if($2 == 4){print $0}}' './Pool concurrency standard test.txt'" using 1:3 title 'Standard' with lines smooth unique, \
        "<awk -F, '{if($2 == 4){print $0}}' './Pool concurrency shared test.txt'" using 1:3 title 'Shared Pool' with lines smooth unique, \
        "<awk -F, '{if($2 == 4){print $0}}' './Pool concurrency thread local.txt'" using 1:3 title 'Thread local Pool' with lines smooth unique

set title "Concurrency 8 thread"
set output "Concurrency 8 thread.png" 
plot    "<awk -F, '{if($2 == 8){print $0}}' './Pool concurrency standard test.txt'" using 1:3 title 'Standard' with lines smooth unique, \
        "<awk -F, '{if($2 == 8){print $0}}' './Pool concurrency shared test.txt'" using 1:3 title 'Shared Pool' with lines smooth unique, \
        "<awk -F, '{if($2 == 8){print $0}}' './Pool concurrency thread local.txt'" using 1:3 title 'Thread local Pool' with lines smooth unique
