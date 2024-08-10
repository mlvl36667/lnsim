import random
import numpy as np
import sys
import os
from collections import defaultdict

import random

def generate_degrees(num_nodes, p):
    # Generáljuk a fokszámokat geometriai eloszlás alapján
    degrees = np.random.geometric(p, num_nodes) 
    degrees = [min(degree, num_nodes - 1) for degree in degrees]
    return degrees

def generate_graph(num_nodes, min_degree, filename):
    graph = defaultdict(set)

    print(degrees)
    # Listázd a csomópontokat és azok fokszámait
    remaining_degrees = degrees[:]

    # Generáljuk az éleket
    for u in range(num_nodes):
     for v in range(u + 1, num_nodes):
        if len(graph[u]) < remaining_degrees[u] and len(graph[v]) < remaining_degrees[v]:
            if len(graph[u]) < remaining_degrees[u] and len(graph[v]) < remaining_degrees[v]:
                if v not in graph[u] and u not in graph[v]:
                    graph[u].add(v)
                    graph[v].add(u)
                    remaining_degrees[u] -= 1
                    remaining_degrees[v] -= 1

                    # Kilépés, ha a fokszámok teljesülnek
                    if all(deg == 0 for deg in remaining_degrees):
                        break
    for u in range(num_nodes):
        print(str(u)+": "+str(graph[u]))
    
    # Flatten the graph into edge list format
    edges = []
    for u in range(num_nodes):
        for v in graph[u]:
            if u < v:  # to avoid duplicates
                edges.append((u, v))

    # Write the graph to the file
    with open(filename, 'w') as f:
        for u, v in edges:
            f.write(f"{u} {v} 0 0\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_graph.py <min_degree> <output_file>")
        sys.exit(1)
    
    min_degree = int(sys.argv[1])
    output_file = sys.argv[2]
    
    num_nodes = int(os.getenv('NUMBER_OF_VERTICES'))
    p = float(os.getenv('GEOM_P'))
    degrees = generate_degrees(num_nodes, p)

    generate_graph(num_nodes, min_degree, output_file)
    print(f"Graph with {num_nodes} nodes and minimum degree {min_degree} written to {output_file}.")

