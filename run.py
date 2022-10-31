# Developer: Bence Ladoczki <ladoczki@tmit.bme.hu> 2022-10
# Usage: for i in {0..36..2}; do python3 run.py 10000 200 $i 32;done
# Simulator for nodes in PCN with incoming and outgoing edges with channel capacity and routing fees
import random
import matplotlib
import matplotlib.pyplot as plt
import sys
from scipy.stats import poisson
import numpy as np
import math


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
 
def get_curve(xx,yy, number_of_edges, max_channel_capacity, average_number_of_transactions):
 simulations = [10,100,1000,10000]
 for ii in simulations:
  overload_count  = 0 
  reorg_count  = 0 
  for i in range(1,ii+1):
   rt = run_simulation(number_of_edges, max_channel_capacity, average_number_of_transactions)
   if(rt == 1 or rt == 3):
    overload_count+=1
   if(rt == 2 or rt == 4):
    overload_count+=1
    reorg_count+=1
  
  print("Number of edges: "+str(number_of_edges))
  print("Average number of transactions: "+str(average_number_of_transactions))
  print("Max channel capacity: "+str(max_channel_capacity))
  print("Number of simulations: "+str(ii))
  print("Ratio of overloaded channels: "+str(100*overload_count/ii)+"%")
  print("Ratio of solvable routings with transaction reorg: "+str(100*reorg_count/ii)+"%")
  yy.append(100*overload_count/ii)
  xx.append(r'$10^'+str(simulations.index(ii)+1)+'$')

number_of_simulations = int(sys.argv[1])
max_channel_capacity = int(sys.argv[2])
average_number_of_transactions = int(sys.argv[3]) # for Poission
number_of_edges = int(sys.argv[4]) # this sets the degree of freedom in the simulator


yy1 = []
xx = []
get_curve(xx, yy1, number_of_edges, 75, average_number_of_transactions)
yy2 = []
xx = []
get_curve(xx, yy2, number_of_edges, 100, average_number_of_transactions)
yy3 = []
xx = []
get_curve(xx, yy3, number_of_edges, 150, average_number_of_transactions)
yy4 = []
xx = []
get_curve(xx, yy4, number_of_edges, 200, average_number_of_transactions)
yy5 = []
xx = []
get_curve(xx, yy5, number_of_edges, 350, average_number_of_transactions)



matplotlib.use("pgf")
matplotlib.rcParams.update({
    "pgf.texsystem": "pdflatex",
    'font.family': 'serif',
    'text.usetex': True,
    'pgf.rcfonts': False,
})


plt.clf()
plt.xlabel(r'$N_{sim}$')
plt.ylabel(r'$R_{ol}$ (%)')
plt.yticks(np.arange(0, 105, 10))
plt.grid(axis='y', color='0.95')
plt.plot(xx, yy1, linestyle='dotted', label=r'$C_m$: '+str(75)+'')
plt.plot(xx, yy2, linestyle='dashed', label=r'$C_m$: '+str(100)+'')
plt.plot(xx, yy3, linestyle='solid', label=r'$C_m$: '+str(150)+'')
plt.plot(xx, yy4, linestyle=(5, (10, 3)), label=r'$C_m$: '+str(200)+'')
plt.plot(xx, yy5, linestyle='dashdot', label=r'$C_m$: '+str(350)+'')
plt.legend(title=r'$R_{ol}$ wrt. $N_{sim}$')

plt.savefig("output/overloaded_channel_ratio.pdf", bbox_inches='tight')
plt.savefig('output/overloaded_channel_ratio.pgf')

