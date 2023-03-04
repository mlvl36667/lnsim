// ########################################################################################
// #### Bitcoin Lightning Network Simulator of Payment Forwarding Requests    #############
// ########################################################################################
// #### Developed by Bence Ladoczki <ladoczki@tmit.bme.hu> 2022-23  #######################
// ####               All rights reserved                           #######################
// ########################################################################################
// # run: gcc -o sim sim.cpp && ./sim 11 32 3 10 | tee res          #######################
// ########################################################################################
// ########################################################################################

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NUMBER_OF_PAYMENTS 10000
#define AMT_AVG 100

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
 
void findpaths(int tonode, struct d_up* dup, int dcnt, struct path* pth, struct Graph* G){

    if(pth->len > G->E){
     printf(" too much recursion in findpaths... \n");
     for(int i=0; i< dcnt; i++){
      printf("at: %d from: %d \n",dup[i].at, dup[i].from);
     }
     exit(1);
     return;
    }
    if(tonode == 0) {
//       printf("now at: %d \n",tonode);
       pth->nodes[pth->len] = 0;
    return;
    }
    for(int ii = dcnt - 1; ii > -1; ii--){
// TODO ha ketszer szerepel a node akkor nem fog rendesen mukodni...
     if(tonode == dup[ii].at) {
//       printf("now at: %d \n",dup[ii].at);
       pth->nodes[pth->len] = dup[ii].at;
       pth->len = pth->len + 1;
       findpaths(dup[ii].from, dup, dcnt, pth, G);
     }
    }
 
}
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
void selectionSort(int arr[], int n)
{
    int i, j, min_idx;
 
    for (i = 0; i < n - 1; i++) {
 
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
 
        swap(&arr[min_idx], &arr[i]);
    }
}
int map_vertice(int vertice, int offset, int num_ver){
 int new_vertice;
 new_vertice = vertice - offset;
 if(new_vertice < 0) {
  new_vertice = new_vertice + num_ver;
 }
 return new_vertice;
}
void printArray(int arr[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}
int map_vertices(struct Graph* graph, struct Graph* new_graph, int from, int to){

 int new_to;
 int* source_vertices, *source_vertices_original, *target_vertices_original, *tv, *fees, *caps;

 source_vertices = (int*) malloc(sizeof(int)*graph->E);
 source_vertices_original = (int*) malloc(sizeof(int)*graph->E);
 target_vertices_original = (int*) malloc(sizeof(int)*graph->E);
 fees = (int*) malloc(sizeof(int)*graph->E);
 caps = (int*) malloc(sizeof(int)*graph->E);
 tv = (int*) malloc(sizeof(int)*graph->E);

 for (int j = 0; j < graph->E; j++){
  int u = graph->edge[j].source;
  int v = graph->edge[j].destination;
//  printf(" u: %d v: %d new u: %d new v: %d \n",u,v, map_vertice(u, from, graph->V), map_vertice(v, from, graph->V));
  source_vertices[j] = map_vertice(u, from, graph->V);
  source_vertices_original[j] = map_vertice(u, from, graph->V);
  target_vertices_original[j] = map_vertice(v, from, graph->V);
  fees[j] = graph->edge[j].routing_fee;
  caps[j] = graph->edge[j].capacity;
 }
// printArray(source_vertices, graph->E);
 selectionSort(source_vertices, graph->E);
// printArray(source_vertices, graph->E);

 int vcnt = 0;
  for (int j = 0; j < graph->E; j++){
//   printf("source_vertices: %d \n",source_vertices[j]);

    int ln = 0;
    for (int jj = 0; jj < graph->E; jj++){

       tv[jj] = 0;

    }
    for (int jj = 0; jj < graph->E; jj++){
     if(source_vertices_original[jj] == source_vertices[j]){
//       printf("original pair: %d \n",target_vertices_original[jj]);
       tv[ln] = target_vertices_original[jj];
       ln = ln + 1;
     }
    }
    selectionSort(tv, ln);
// printf(" %d \n",tv[vcnt]);
 new_graph->edge[j].source = source_vertices[j];
 new_graph->edge[j].destination = tv[vcnt];

    for (int jj = 0; jj < graph->E; jj++){
     if(source_vertices_original[jj] == new_graph->edge[j].source && new_graph->edge[j].destination == target_vertices_original[jj]){
      new_graph->edge[j].routing_fee = fees[jj];
      new_graph->edge[j].capacity = caps[jj];
     }
    }

  
  vcnt = vcnt + 1;
  if(j < graph->E - 1){
   if(source_vertices[j] != source_vertices[j+1]){
    vcnt = 0;
   }

  }

  }

 return map_vertice(to, from, graph->V);
}
void print_graph(struct Graph* graph){
 for (int j = 0; j < graph->E; j++){
  printf("G.add_edge(\"%d\", \"%d\", weight=%d)  \n", graph->edge[j].source, graph->edge[j].destination, graph->edge[j].capacity);
//  printf("G.add_edge(\"%d\", \"%d\", weight=%d)  \n", graph->edge[j].source, graph->edge[j].destination, graph->edge[j].routing_fee);
 }
 return;
}
struct path* BellmanFord(struct Graph* graph, int from, int to, int amt)
{
    int V = graph->V;
 
    int E = graph->E;
 
    int StoreDistance[V];
 
    int i,j;
 
    struct d_up* dup = (d_up*)malloc(sizeof(d_up)*E*(V-1));

    for(i=0;i<E*(V-1);i++){
     dup[i].at = -1;
     dup[i].from = -1;
    }

//    print_graph(graph);

    if(from != 0){
     struct Graph* new_graph = createGraph(graph->V, graph->E);
     new_graph->V = graph->V;
     new_graph->E = graph->E;
     int new_to = map_vertices(graph, new_graph, from, to);
     struct path* pth2 = BellmanFord(new_graph, 0, new_to, amt);

      for(int jj=0; jj < pth2->len + 1 ; jj++){
       pth2->nodes[jj] = pth2->nodes[jj] + from;
       if(pth2->nodes[jj] > graph->V - 1){
        pth2->nodes[jj] = pth2->nodes[jj] - from;
        pth2->nodes[jj] = pth2->nodes[jj] + from - graph->V ;
       }
      }

     return pth2;
    }

    for (i = 0; i < V; i++)
        StoreDistance[i] = INT_MAX;
 
    StoreDistance[from] = 0;
 
    int dcnt = 0;
    for (i = 1; i <= V-1; i++)
    {
        for (j = 0; j < E; j++)
        {
            int u = graph->edge[j].source;
 
            int v = graph->edge[j].destination;
 
            int routing_fee = graph->edge[j].routing_fee;
 
//            printf("%d. trying edge: %d from: %d %d \n",j, v, u, routing_fee);
            if(graph->edge[j].capacity > amt){ /* use channels with enough capacity */
             if (StoreDistance[u] + routing_fee < StoreDistance[v] && StoreDistance[u] != INT_MAX){
                 StoreDistance[v] = StoreDistance[u] + routing_fee;
//                 printf("%d. distance update at: %d from: %d %d %d \n",j, v, u, routing_fee, StoreDistance[u]);

                 int newelem = 0;
                 for(int ii = 0; ii < dcnt; ii++){
                  if(dup[ii].at == v){
                   dup[ii].from = u;
// printf(" 1. setting from: %d at: %d \n",dup[ii].from, dup[ii].at);
                   newelem = 1;
                  }
                  }

                   if(newelem == 0){
                    dup[dcnt].at = v;
                    dup[dcnt].from = u;
// printf(" 2. setting from: %d at: %d \n",dup[dcnt].from, dup[dcnt].at);
                   }

                 dcnt = dcnt + 1;
             }
            }
        }
    }

     struct path* pth = (path*)malloc(sizeof(path) );
     pth->nodes = (int*)malloc(sizeof(int) * graph->E );
     for(i = 0; i < graph->E; i++){
      pth->nodes[i] = 0;
     }
     pth->len = 0;
//     printf("findpaths %d: \n",to);
// printf("dcnt: %d \n",dcnt);
     findpaths(to, dup, dcnt, pth, graph);



    free(dup);
 
 
    for (i = 0; i < E; i++)
    {
        int u = graph->edge[i].source;
 
        int v = graph->edge[i].destination;
 
        int routing_fee = graph->edge[i].routing_fee;
 
        if (StoreDistance[u] + routing_fee < StoreDistance[v])
            printf("This graph contains negative edge cycle\n");
    }
 
//    FinalSolution(StoreDistance, V);
 
    return pth;
}
 
void load_topology(const char* file_name, Graph* graph, int cap)
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
       graph->edge[counter].capacity = cap;
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

void update_routing_fees(struct Graph* graph){
    for (int i = 0; i <= graph->V ; i++)
    {
        for (int j = 0; j < graph->E; j++)
        {
// TODO how to set this?
          if(graph->edge[j].source == i && graph->edge[j].capacity < 50){
           graph->edge[j].routing_fee = graph->edge[j].routing_fee - graph->edge[j].routing_fee*0.3;
          }
        }
     }
 return;
}
int reduce_cap(struct Graph* graph, int source, int destination, int amt ){
        
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == source && graph->edge[j].destination == destination) {
           graph->edge[j].capacity = graph->edge[j].capacity - amt;
           if(graph->edge[j].capacity < 30) {
//            graph->edge[j].routing_fee = graph->edge[j].routing_fee + 1;
            printf("Warning, low capacity (<30) \n");
//            print_graph(graph);
            return 1;
           }
           return 0;
          }
          
        }
        
 printf(" No such edge: %d %d \n", source,destination);
 return 0;
}
void increase_cap(struct Graph* graph, int source, int destination, int amt ){
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == source && graph->edge[j].destination == destination) {
           graph->edge[j].capacity = graph->edge[j].capacity + amt;
           return;
          }
          
        }
        
 printf(" No such edge: %d %d \n", source,destination);
 return;
}

int get_channel_capacity(int from, int to, struct Graph* graph){
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == from && graph->edge[j].destination == to) {
           return graph->edge[j].capacity;
//           return graph->edge[j].routing_fee;
          }
          
        }
        
 printf(" No such edge: %d %d \n", from,to);
 return 0;
}
int send_payment(struct payment* pm, struct Graph* graph){
 int capacity_critical;

 printf("sending payment from %d to %d amount: %d  \n",pm->from,pm->to,pm->amount);

 struct path* pth = BellmanFord(graph, pm->from, pm->to, pm->amount);

 capacity_critical = 0;
// printf("sp len: %d \n",pth->len);
 printf(" %d ->", pth->nodes[0]);
 for(int jj=1; jj < pth->len+1 ; jj++){
  if(jj == pth->len){
   printf(" %d ", pth->nodes[jj]);
  }
  else{
   printf(" %d ->", pth->nodes[jj]);
  }
  capacity_critical = reduce_cap(graph, pth->nodes[jj], pth->nodes[jj-1], pm->amount );
  increase_cap(graph, pth->nodes[jj-1], pth->nodes[jj], pm->amount );
 }
 printf(" \n");

 free(pm);
 free(pth->nodes);
 free(pth);
 return capacity_critical;
}
int get_random_number(){
    int randomvalue;
    FILE *fpointer;
    fpointer = fopen("/dev/urandom", "rb");
    if(fpointer == NULL){

 printf("fpointer == NULL in get_random_number \n");
    }
    fread(&randomvalue,sizeof(int),1,fpointer);  
    fclose(fpointer);
    return abs(randomvalue);
}
int main(int argc, char *argv[])
{
    int V,E; 
    int from,to; 
 
    sscanf (argv[1],"%d",&V);
    sscanf (argv[2],"%d",&E);
    sscanf (argv[3],"%d",&from);
    sscanf (argv[4],"%d",&to);

    struct Graph* graph = createGraph(V, E);
 
    int i;

    load_topology("topology", graph, 1000);

    set_routing_fee(graph, 1, 2, 2);
    int cc = 0;
    FILE *out_file = fopen("vis", "w"); // write only
    fprintf(out_file, "[");
// Simulate payments:
    for(int ii=0; ii < MAX_NUMBER_OF_PAYMENTS; ii++){
     if(ii == MAX_NUMBER_OF_PAYMENTS - 1){
      fprintf(out_file, "[%d, %d] ",ii, get_channel_capacity(9,7, graph));
     }
     else{
      fprintf(out_file, "[%d, %d], \n",ii, get_channel_capacity(9,7, graph));
     }
     from = get_random_number() % V;
     to = get_random_number() % V;
     while(to == from){
      to = get_random_number() % V;
     }
     int amt = get_random_number() % AMT_AVG;
     while(amt == 0){
      amt = get_random_number() % AMT_AVG;
     }

     cc = cc + send_payment(create_payment(from, to, amt), graph);
//     print_graph(graph);
    }

 fprintf(out_file, "]");
 fclose(out_file);
 printf("cc: %d \n",cc);
 printf(" %5.3f \n",(float)(cc) / (float)MAX_NUMBER_OF_PAYMENTS);
// szimulálni jó sok fizetést és megnézni hogyan ürülnek ki a csatornák, majd frissíteni a csúcsok fee-jét bizonyos dinamikus stratégiákkal
// bele kell rakni a profit képletét is
// ki kell tudni számolni egy kollektív profitot is --> ezt kellene maximalizálni valahogyan
// bele kell rakni egy időfaktort is, hogy legyen ideje a csomópontoknak fee-t állítgatni

//    update_routing_fees(graph);
//    send_payment(create_payment(0, 3, 50), graph);
//    update_routing_fees(graph);
//    send_payment(create_payment(0, 3, 20), graph);
//    update_routing_fees(graph);

//    print_graph(graph);
 
    return 0;
}
