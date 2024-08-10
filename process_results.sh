#!/bin/bash - 
#===============================================================================
#
#          FILE: process_results.sh
# 
#         USAGE: ./process_results.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2024-08-10 18:16
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
#!/bin/bash

cors=(0 100 200 300 400 500 600 700 800 900 1000)
cors=(0 100 500 1000)
pvalues=(0.3 0.4 0.5 0.6 0.7 0.8 0.9)

# Print the CSV header
echo "pvalue,cor,value" > final_data_range_2.csv

# Iterate over pvalues and cors
    for cor in "${cors[@]}"; do
for pv in "${pvalues[@]}"; do  
        # Read the file and process it
        cat results/pvalue_${pv}_statistics_${cor} | awk -F, -v pv="$pv" -v cor="$cor" '{print pv "," cor "," 0$1}' >> final_data_range_2.csv
    done
done


