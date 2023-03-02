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
    int source, destination, weight;
};
struct d_up
{
    int at, from;
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
 
void FinalSolution(int dist[], int n)
{
    printf("\nVertex\tDistance from Source Vertex\n");
    int i;
 
    for (i = 0; i < n; ++i){
     printf("%d \t\t %d\n", i, dist[i]);
    }
}
 
void findpaths(int tonode, struct d_up* dup, int dcnt){

    if(tonode == 0) {
       printf("now at: %d \n",tonode);
    return;
    }
    for(int ii = dcnt - 1; ii > -1; ii--){
     if(tonode == dup[ii].at) {
       printf("now at: %d \n",dup[ii].at);
       findpaths(dup[ii].from, dup, dcnt);
     }
    }
 
}
void BellmanFord(struct Graph* graph)
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
 
            int weight = graph->edge[j].weight;
 
            if (StoreDistance[u] + weight < StoreDistance[v]){
                StoreDistance[v] = StoreDistance[u] + weight;
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

    for(int ii=0;ii < 6; ii++){
     printf("findpaths %d: \n",ii);
     findpaths(ii, dup, dcnt);
    }

    free(dup);
 
 
    for (i = 0; i < E; i++)
    {
        int u = graph->edge[i].source;
 
        int v = graph->edge[i].destination;
 
        int weight = graph->edge[i].weight;
 
        if (StoreDistance[u] + weight < StoreDistance[v])
            printf("This graph contains negative edge cycle\n");
    }
 
    FinalSolution(StoreDistance, V);
 
    return;
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
       graph->edge[counter].weight = i;
       counter = counter + 1;
      }

      ii = ii + 1;

    }
  fclose (file);        
}
void set_routing_fee(struct Graph* graph, int source, int destination, int weight ){
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == source && graph->edge[j].destination == destination) {
           graph->edge[j].weight = weight;
           return;
          }
          
        }
        
 printf(" No such edge: %d %d \n", source,destination);
 return;
}
int main(int argc, char *argv[])
{
    int V,E; 
 
    sscanf (argv[1],"%d",&V);
    sscanf (argv[2],"%d",&E);

    struct Graph* graph = createGraph(V, E);
 
    int i;

    read_ints("topology", graph);

    set_routing_fee(graph, 1, 4, 2);
    BellmanFord(graph);
    set_routing_fee(graph, 1, 4, 20);
    BellmanFord(graph);
 
    return 0;
}
