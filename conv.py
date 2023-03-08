import json

f = open('listchannels20220412.json')
data = json.load(f)  
unique_sources = []
[unique_sources.append(x['source']) for x in data['channels'] if x['source'] not in unique_sources]
print(str(len(unique_sources)))
unique_dsts = []
[unique_dsts.append(x['destination']) for x in data['channels'] if x['destination'] not in unique_dsts]
print(str(len(unique_dsts)))
edges = []

[edges.append( [x['source'], x['destination']] ) for x in data['channels'] ]

f.close()

ii = 0


of = 'ln_topology'
ff = open(of,'w')
for edge in edges:
 ff.write(str(unique_sources.index(edge[0])) + " " + str(unique_dsts.index(edge[1]))+"\n") 

ff.close()
