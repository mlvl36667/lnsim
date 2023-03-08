/* echo "plot" | tr "\n" " "  && for i in 2 3 4 5 6 7 8; do echo \"lccsd_data\" using 1:$i title "'E^{($i)}'"  with linespoints linestyle $i,; done | tr "\n" " " */

set key font ",40"

set ylabel font ",40"
set ytics font ", 40"
set ylabel 'Channel capacity / mSatoshi'

set xlabel '# payments'
set xlabel font ",40"
set xtics font ", 40"

set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#00FF00' linetype 1 linewidth 3 pointtype 2 pointsize 1
set style line 3 linecolor rgb '#0000FF' linetype 1 linewidth 3 pointtype 3 pointsize 1
set style line 4 linecolor rgb '#800080' linetype 1 linewidth 3 pointtype 4 pointsize 1
set style line 5 linecolor rgb '#00FFFF' linetype 1 linewidth 3 pointtype 5 pointsize 1
set style line 6 linecolor rgb '#FF00FF' linetype 1 linewidth 3 pointtype 6 pointsize 1
set style line 7 linecolor rgb '#C0C0C0' linetype 1 linewidth 3 pointtype 7 pointsize 1
set style line 8 linecolor rgb '#800000' linetype 1 linewidth 3 pointtype 8 pointsize 1
set style line 9 linecolor rgb '#000000' linetype 1 linewidth 3 


