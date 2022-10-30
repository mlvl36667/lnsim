# Simulator for nodes in PCN with incoming and outgoing edges with channel capacity and routing fees
import random
from scipy.stats import poisson


def choose_in_out_edge(edges,transaction_value):
 in_edge = random.randint(1, len(edges))

 out_edge = random.randint(1, len(edges))
 while(out_edge == in_edge):
  out_edge = random.randint(1, len(edges))

 current_value = transaction_value * random.randint(1, 100) / 100
 return [in_edge,out_edge, current_value]

number_of_edges = 8
max_channel_capacity = 200
average_number_of_transactions = 10

edges = [x for x in range(1,number_of_edges+1)]

# Draw the number of transactions from a Poisson distribution (mu can be increased to simulate more congested networks)
number_of_transactions = poisson.rvs(mu=average_number_of_transactions, size=1)[0]
transactions = []
[transactions.append(choose_in_out_edge(edges, 100)) for x in range(1,number_of_transactions+1)]

print(str(transactions))

in_edges = [[x[0],x[2]] for x in transactions]
out_edges = [[x[1],x[2]] for x in transactions]

# Get feasible combination by comparing in/out load to max_channel_capacity for each edge
