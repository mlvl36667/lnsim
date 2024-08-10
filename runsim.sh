#!/bin/bash - 
#===============================================================================
#
#          FILE: runsim.sh
# 
#         USAGE: ./runsim.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2024-08-10 10:15
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
g++ -o sim sim.cpp -fopenmp 
cors=(0 100 200 300 400 500 600 700 800 900 1000)
pvalues=(0.05 0.06 0.07 0.08 0.09 0.1 0.2)
pvalues=(0.3 0.4 0.5 0.6 0.7 0.8 0.9)
for cor in "${cors[@]}"; do 
for pv in "${pvalues[@]}"; do 
echo $pv
 export GEOM_P=$pv
 export NUMBER_OF_VERTICES=1300
 export NUM_SIM=12
 export NUMBER_OF_PAYMENTS=1000
 export FEE_CORRECTION=${cor}
 export FEE_CORRECTION_SMALL=${cor}
 export FEE_CORRECTION_LARGE=${cor}
 export CONNECTIVITY=4
 export TOPOLOGY_FILE=topologies/graph_${CONNECTIVITY}
 export OMP_NUM_THREADS=12
# ./sim  
 ./sim  | grep "sum_rev" > results/pvalue_$pv
 # Fájl olvasása és az értékek kinyerése
 values=$(awk '{print $3}' results/pvalue_${pv})
 sum=0
 count=0
 for value in $values; do
     value=$(echo $value | sed 's/BTC//')
     sum=$(echo "$sum + $value" | bc)
     count=$((count + 1))
 done
 mean=$(echo "$sum / $count" | bc -l)
 sum_squares=0
 for value in $values; do
     value=$(echo $value | sed 's/BTC//')
     diff=$(echo "$value - $mean" | bc -l)
     square=$(echo "$diff * $diff" | bc -l)
     sum_squares=$(echo "$sum_squares + $square" | bc -l)
 done
 stddev=$(echo "sqrt($sum_squares / $count)" | bc -l)
 echo "$mean","$stddev" > results/pvalue_${pv}_statistics_${cor}
done
done

