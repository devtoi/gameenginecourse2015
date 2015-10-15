set term png
set datafile separator ","
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set xlabel "Particles per block"
set ylabel "Time(ms)"
set yrange [0:0.15]
set title "Random Lifetime allocation"
set output "RandLifeAlloc.png"
plot "./RandLifePoolAlloc.txt" using 1:2 title 'Pool' with lines smooth unique,\
	 "./RandLifeMalloc.txt" using 1:2 title 'Malloc' with lines smooth unique

set yrange [0:0.02]
set title "Random Lifetime deallocation"
set output "RandLifeDelloc.png"
plot "./RandLifePoolDealloc.txt" using 1:2 title 'Pool' with lines smooth unique,\
	 "./RandLifeFree.txt" using 1:2 title 'Free' with lines smooth unique

