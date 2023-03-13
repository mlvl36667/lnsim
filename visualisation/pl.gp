set term postscript enhanced color eps font "ArialMT, 18pt"
set output "payment.eps"

set linestyle 1 lt 2 lw 3
set key ins vert

set format y "%.3f"


set xrange [0:0.5]


set grid

set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#00FF00' linetype 1 linewidth 3 pointtype 2 pointsize 1
set style line 3 linecolor rgb '#0000FF' linetype 1 linewidth 3 pointtype 3 pointsize 1
set style line 4 linecolor rgb '#800080' linetype 1 linewidth 3 pointtype 4 pointsize 1
set style line 5 linecolor rgb '#000000' linetype 1 linewidth 3 pointtype 5 pointsize 1

set grid noxtics


set ylabel 'R [BTC] '
set xlabel 'Fee Correction [%]'
plot "payment.data" using 1:2 notitle ''  with linespoints linestyle 1,\
     "payment.data" using 1:2:3 notitle '' with yerrorbars linestyle 1
