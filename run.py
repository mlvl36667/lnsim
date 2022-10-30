# Developer: Bence Ladoczki <ladoczki@tmit.bme.hu> 2022-10
# Usage: for i in {0..36..2}; do python3 run.py 10000 200 $i 32;done
# Simulator for nodes in PCN with incoming and outgoing edges with channel capacity and routing fees
import random
import sys
from scipy.stats import poisson


def choose_in_out_edge(edges,transaction_value):
 in_edge = random.randint(1, len(edges))

 out_edge = random.randint(1, len(edges))
 while(out_edge == in_edge):
  out_edge = random.randint(1, len(edges))

 current_value = transaction_value * random.randint(1, 100) / 100
 return [in_edge,out_edge, current_value]

def run_simulation(number_of_edges, max_channel_capacity, average_number_of_transactions):
 
 edges = [x for x in range(1,number_of_edges+1)]
 
 # Draw the number of transactions from a Poisson distribution (mu can be increased to simulate more congested networks)
 number_of_transactions = poisson.rvs(mu=average_number_of_transactions, size=1)[0]
 transactions = []
 [transactions.append(choose_in_out_edge(edges, 100)) for x in range(1,number_of_transactions+1)]
 
# print(str(transactions))
 
 in_edges = [[x[0],x[2]] for x in transactions]
 out_edges = [[x[1],x[2]] for x in transactions]
 
 in_edges_1 = [x[0] for x in transactions]
 out_edges_1 = [x[1] for x in transactions]
 # Get feasible combination by comparing in/out load to max_channel_capacity for each edge
 
 #print(str(in_edges_1))
 #print(str(out_edges_1))
 
 occurrences = lambda s, lst: (i for i,e in enumerate(lst) if e == s)
 
 multiple_in_edge = [[x,list(occurrences(x, in_edges_1))] for x in set(in_edges_1) if len(list(occurrences(x, in_edges_1))) > 1]
 multiple_in_edge_1 = [x[0] for x in multiple_in_edge]
 same_in_edge_transactions = [x for x in transactions if x[0] in multiple_in_edge_1]
 
 multiple_out_edge = [[x,list(occurrences(x, out_edges_1))] for x in set(out_edges_1) if len(list(occurrences(x, out_edges_1))) > 1]
 multiple_out_edge_1 = [x[0] for x in multiple_out_edge]
 same_out_edge_transactions = [x for x in transactions if x[0] in multiple_out_edge_1]
 #print(str(multiple_in_edge_1))
 #print(str(same_in_edge_transactions))
 
 return_value = 0
 for x in set(multiple_in_edge_1):
  cumm_value  = 0
  for y in transactions:
   if(y[0] == x):
    cumm_value += y[2]
  if(cumm_value > max_channel_capacity):
#   print(str(x)+" is overloaded: "+str(cumm_value))
   return_value = 1
   for z in transactions: 
    if(z[1] == x and cumm_value - z[2] < max_channel_capacity):
#     print("Can be solved by: "+str(z))
     return_value = 2

 for x in set(multiple_out_edge_1):
  cumm_value  = 0
  for y in transactions:
   if(y[0] == x):
    cumm_value += y[2]
  if(cumm_value > max_channel_capacity):
   return_value = 3
   for z in transactions: 
    if(z[1] == x and cumm_value - z[2] < max_channel_capacity):
     return_value = 4
# ezt itt nem tudom hogyan kellene kezelni

 return return_value
 
overload_count  = 0 
reorg_count  = 0 

number_of_simulations = int(sys.argv[1])
max_channel_capacity = int(sys.argv[2])
average_number_of_transactions = int(sys.argv[3]) # for Poission
number_of_edges = int(sys.argv[4]) # this sets the degree of freedom in the simulator

for i in range(1,number_of_simulations+1):
 rt = run_simulation(number_of_edges, max_channel_capacity, average_number_of_transactions)
 if(rt == 1 or rt == 3):
  overload_count+=1
 if(rt == 2 or rt == 4):
  overload_count+=1
  reorg_count+=1

print("Number of edges: "+str(number_of_edges))
print("Average number of transactions: "+str(average_number_of_transactions))
print("Max channel capacity: "+str(max_channel_capacity))
print("Number of simulations: "+str(number_of_simulations))
print("Ratio of overloaded channels: "+str(100*overload_count/number_of_simulations)+"%")
print("Ratio of solvable routings with transaction reorg: "+str(100*reorg_count/number_of_simulations)+"%")
