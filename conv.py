import json

f = open('listchannels20220412.json')
data = json.load(f)  
nodes = []
[nodes.append(x['source']) for x in data['channels'] if x['source'] not in nodes]
print(str(len(nodes)))
edges = []

[edges.append( [nodes.index(x['source']), nodes.index(x['destination']), x['base_fee_millisatoshi'], x['fee_per_millionth']] ) for x in data['channels'] if x['source'] in nodes and x['destination'] in nodes ]

sted = sorted(edges, key=lambda x: x[0], reverse=False)
sted2 = sorted(sted, key=lambda x: x[1], reverse=False)
sted3 = sorted(sted2, key=lambda x: x[0], reverse=False)

f.close()

ii = 0



of = 'ln_topology_2'
ff = open(of,'w')
vert = 0
numne = -1
bignodes = []
for edge in sted3:
 numne = numne + 1
 if(not edge[0] == vert):
  ff.write(str(vert) + " " + str(numne)+"\n") 
  if(numne > 100):
   bignodes.append(vert) 
  numne = 0
 vert = edge[0]
ff.close()

bn = [] 
[bn.append(x) for x in bignodes if x not in bn]

print(str(len(bn)))

edges = []

[edges.append( [bn.index(x[0]), bn.index(x[1]), x[2], x[3]] ) for x in sted3 if x[0] in bn and x[1] in bn ]

sted = sorted(edges, key=lambda x: x[0], reverse=False)
sted2 = sorted(sted, key=lambda x: x[1], reverse=False)
sted3 = sorted(sted2, key=lambda x: x[0], reverse=False)



of = 'ln_topology'
ff = open(of,'w')
for edge in sted3:
 ff.write(str(edge[0]) + " " + str(edge[1])+" "+str(edge[2])+" "+str(edge[3])+"\n") 
ff.close()
