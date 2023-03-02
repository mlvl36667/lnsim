// ########################################################################################
// #### Developed by Bence Ladoczki <ladoczki@tmit.bme.hu> 2022-23  #######################
// ####               All rights reserved                           #######################
// ########################################################################################
// # run: gcc -o sim sim.cpp && ./sim 6 10 ################################################
// ########################################################################################
// ########################################################################################

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct Edge
{
    int source, destination, routing_fee, capacity;
};
struct d_up
{
    int at, from;
};
struct payment
{
    int from, to, amount;
};
 
struct path
{
    int len;
    int* nodes;
};
struct Graph
{
    int V, E;
    struct Edge* edge;
};
 
struct Graph* createGraph(int V, int E)
{
    struct Graph* graph = (struct Graph*) malloc( sizeof(struct Graph));
    graph->V = V;   
    graph->E = E;
    graph->edge = (struct Edge*) malloc( graph->E * sizeof( struct Edge ) );
    return graph;
}
struct payment* create_payment(int f, int t, int a)
{
    struct payment* pm = (struct payment*) malloc( sizeof(struct payment));
    pm->from = f;   
    pm->to = t;   
    pm->amount = a;   
    return pm;
}
 
void FinalSolution(int dist[], int n)
{
    printf("\nVertex\tDistance from Source Vertex\n");
    int i;
 
    for (i = 0; i < n; ++i){
     printf("%d \t\t %d\n", i, dist[i]);
    }
}
 
void findpaths(int tonode, struct d_up* dup, int dcnt, struct path* pth){

    if(tonode == 0) {
//       printf("now at: %d \n",tonode);
       pth->nodes[pth->len] = 0;
    return;
    }
    for(int ii = dcnt - 1; ii > -1; ii--){
     if(tonode == dup[ii].at) {
//       printf("now at: %d \n",dup[ii].at);
       pth->nodes[pth->len] = dup[ii].at;
       pth->len = pth->len + 1;
       findpaths(dup[ii].from, dup, dcnt, pth);
     }
    }
 
}
struct path* BellmanFord(struct Graph* graph, int to)
{
    int V = graph->V;
 
    int E = graph->E;
 
    int StoreDistance[V];
 
    int i,j;
 
    struct d_up* dup = (d_up*)malloc(sizeof(d_up)*E*(V-1));

    int source = 0;

    for (i = 0; i < V; i++)
        StoreDistance[i] = INT_MAX;
 
    StoreDistance[source] = 0;
 
    int dcnt = 0;
    for (i = 1; i <= V-1; i++)
    {
        for (j = 0; j < E; j++)
        {
            int u = graph->edge[j].source;
 
            int v = graph->edge[j].destination;
 
            int routing_fee = graph->edge[j].routing_fee;
 
            if (StoreDistance[u] + routing_fee < StoreDistance[v]){
                StoreDistance[v] = StoreDistance[u] + routing_fee;
                printf("distance update at: %d from: %d \n",v, u);

                int newelem = 0;
                for(int ii = 0; ii < dcnt; ii++){
                 if(dup[ii].at == v){
                  dup[ii].from = u;
                  newelem = 1;
                 }
                 }

                  if(newelem == 0){
                   dup[dcnt].at = v;
                   dup[dcnt].from = u;
                  }

                dcnt = dcnt + 1;
            }
        }
    }

     struct path* pth = (path*)malloc(sizeof(path) );
     pth->nodes = (int*)malloc(sizeof(int) * graph->E );
     pth->len = 0;
     printf("findpaths %d: \n",to);
     findpaths(to, dup, dcnt, pth);



    free(dup);
 
 
    for (i = 0; i < E; i++)
    {
        int u = graph->edge[i].source;
 
        int v = graph->edge[i].destination;
 
        int routing_fee = graph->edge[i].routing_fee;
 
        if (StoreDistance[u] + routing_fee < StoreDistance[v])
            printf("This graph contains negative edge cycle\n");
    }
 
    FinalSolution(StoreDistance, V);
 
    return pth;
}
 
void read_ints (const char* file_name, Graph* graph)
{
  FILE* file = fopen (file_name, "r");
  int i = 0;
  int counter = 0;
  int ii = 0;

  
  while (!feof (file))
    {  
      fscanf (file, "%d", &i);      

      if(ii % 3 == 0){
       graph->edge[counter].source = i;
      }
      if(ii % 3 == 1){
       graph->edge[counter].destination = i;
      }
      if(ii % 3 == 2){
       graph->edge[counter].routing_fee = i;
       graph->edge[counter].capacity = 100;
       counter = counter + 1;
      }

      ii = ii + 1;

    }
  fclose (file);        
}
void set_routing_fee(struct Graph* graph, int source, int destination, int routing_fee ){
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == source && graph->edge[j].destination == destination) {
           graph->edge[j].routing_fee = routing_fee;
           return;
          }
          
        }
        
 printf(" No such edge: %d %d \n", source,destination);
 return;
}

void reduce_cap(struct Graph* graph, int source, int destination, int amt ){
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == source && graph->edge[j].destination == destination) {
           graph->edge[j].capacity = graph->edge[j].capacity - amt;
           return;
          }
          
        }
        
 printf(" No such edge: %d %d \n", source,destination);
 return;
}

void print_graph(struct Graph* graph){
 for (int j = 0; j < graph->E; j++){
  printf("G.add_edge(\"%d\", \"%d\", weight=%d)  \n", graph->edge[j].source, graph->edge[j].destination, graph->edge[j].capacity);
//  printf("G.add_edge(\"%d\", \"%d\", weight=%d)  \n", graph->edge[j].source, graph->edge[j].destination, graph->edge[j].routing_fee);
 }
 return;
}
int send_payment(struct payment* pm, struct Graph* graph){
 printf("sending payment from %d to %d amount: %d  \n",pm->from,pm->to,pm->amount);

 struct path* pth = BellmanFord(graph, pm->to);

 printf("sp len: %d \n",pth->len);
 for(int jj=1; jj < pth->len+1 ; jj++){
  printf(" %d ->", pth->nodes[jj]);
  reduce_cap(graph, pth->nodes[jj], pth->nodes[jj-1], pm->amount );
 }

 printf(" \n");
    print_graph(graph);

 return 0;
}
int main(int argc, char *argv[])
{
    int V,E; 
 
    sscanf (argv[1],"%d",&V);
    sscanf (argv[2],"%d",&E);

    struct Graph* graph = createGraph(V, E);
    struct payment* pm = create_payment(0, 3, 2);
 
    int i;

    read_ints("topology", graph);
    print_graph(graph);

    set_routing_fee(graph, 1, 4, 2);
    send_payment(pm, graph);
 
    return 0;
}
