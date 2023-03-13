// ########################################################################################
// #### Bitcoin Lightning Network Simulator of Payment Forwarding Requests    #############
// ########################################################################################
// #### Developed by Bence Ladoczki <ladoczki@tmit.bme.hu> 2022-23  #######################
// ####               All rights reserved                           #######################
// ########################################################################################
// # install: sudo apt install libomp-dev
// # run: export OMP_NUM_THREADS=10 && g++ -o sim sim.cpp -fopenmp && ./sim [NUM_OF_VERTICES] [NUM_OF_EDGES] 
// ########################################################################################
// ########################################################################################

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

#define NUMBER_OF_PAYMENTS  10000
#define NUM_SIM                10
#define TR_AMT             200000 /* 0.003 BTC */
#define INIT_CAP          1000000 /* 0.010 BTC */
#define FEE_CORRECTION          0
#define FEE_CORRECTION_LARGE 2000
#define FEE_CORRECTION_SMALL 1000
#define AM_BND               1.1 /* bound for large/small amounts */
//#define CAPACITY_LIMIT       800
#define SATOSHI_TO_BTC 100000000 /*   10^8    */

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

struct Edge
{
    int source;
    int destination;
    int routing_fee;
    int capacity;
    int base_fee;
    int variable_fee;
    int routing_revenue;
    int number_of_routed_payments;
    int vf_lrg; /* variable fee for large transactions */
    int vf_sml; /* variable fee for smaller transactions */
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
    for (int j = 0; j < graph->E; j++)
    {
     graph->edge[j].source = INT_MAX;
     graph->edge[j].destination = INT_MAX;
    }

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

// printf("now at: %d \n",tonode);
    if(pth->len > G->E){
     printf(" too much recursion in findpaths... \n");
// printf("dcnt: %d \n",dcnt);
     for(int i=0; i< dcnt; i++){
//      printf("at: %d from: %d \n",dup[i].at, dup[i].from);
     }
     exit(1);
     return;
    }
    if(tonode == 0) {
       pth->nodes[pth->len] = 0;
    return;
    }
    for(int ii = dcnt - 1; ii > -1; ii--){
     if(tonode == dup[ii].at) {
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
// The SP algoirthm works only when the edges are sorted in ascending order
// Here we reorder the edges if the source is not '0' and return a new graph object
 int new_to;
 int* source_vertices, *source_vertices_original, *target_vertices_original, *tv, *fees, *caps;

 source_vertices = (int*) malloc(sizeof(int)*graph->E);
 source_vertices_original = (int*) malloc(sizeof(int)*graph->E);
 target_vertices_original = (int*) malloc(sizeof(int)*graph->E);
 fees = (int*) malloc(sizeof(int)*graph->E);
 caps = (int*) malloc(sizeof(int)*graph->E);
 tv = (int*) malloc(sizeof(int)*graph->E);
 int* rev = (int*) malloc(sizeof(int)*graph->E);
 int* pms = (int*) malloc(sizeof(int)*graph->E);
 int* vfees = (int*) malloc(sizeof(int)*graph->E);
 int* vf_lrg = (int*) malloc(sizeof(int)*graph->E);
 int* vf_sml = (int*) malloc(sizeof(int)*graph->E);

 for (int j = 0; j < graph->E; j++){
  int u = graph->edge[j].source;
  int v = graph->edge[j].destination;
//  printf(" u: %d v: %d new u: %d new v: %d \n",u,v, map_vertice(u, from, graph->V), map_vertice(v, from, graph->V));
  source_vertices[j] = map_vertice(u, from, graph->V);
  source_vertices_original[j] = map_vertice(u, from, graph->V);
  target_vertices_original[j] = map_vertice(v, from, graph->V);
  fees[j] = graph->edge[j].base_fee;
  vfees[j] = graph->edge[j].variable_fee;

  vf_lrg[j] = graph->edge[j].vf_lrg;
  vf_sml[j] = graph->edge[j].vf_sml;

  caps[j] = graph->edge[j].capacity;
  rev[j] = graph->edge[j].routing_revenue;
  pms[j] = graph->edge[j].number_of_routed_payments;
 }
 selectionSort(source_vertices, graph->E);

 int vcnt = 0;
  for (int j = 0; j < graph->E; j++){
   int ln = 0;
   for (int jj = 0; jj < graph->E; jj++){
      tv[jj] = 0;
   }
   for (int jj = 0; jj < graph->E; jj++){
    if(source_vertices_original[jj] == source_vertices[j]){
      tv[ln] = target_vertices_original[jj];
      ln = ln + 1;
    }
   }
   selectionSort(tv, ln);
   new_graph->edge[j].source = source_vertices[j];
   new_graph->edge[j].destination = tv[vcnt];

   for (int jj = 0; jj < graph->E; jj++){
    if(source_vertices_original[jj] == new_graph->edge[j].source && new_graph->edge[j].destination == target_vertices_original[jj]){
     new_graph->edge[j].base_fee = fees[jj];
     new_graph->edge[j].variable_fee = vfees[jj];
     new_graph->edge[j].vf_lrg = vf_lrg[jj];
     new_graph->edge[j].vf_sml = vf_sml[jj];

     new_graph->edge[j].capacity = caps[jj];
     new_graph->edge[j].routing_revenue = rev[jj];
     new_graph->edge[j].number_of_routed_payments = pms[jj];
    }
   }
   vcnt = vcnt + 1;
   if(j < graph->E - 1){
    if(source_vertices[j] != source_vertices[j+1]){
     vcnt = 0;
    }
   }

  }

 free(source_vertices);
 free(source_vertices_original);
 free(target_vertices_original);
 free(fees);
 free(vfees);
 free(vf_lrg);
 free(vf_sml);
 free(caps);
 free(tv);
 free(rev);
 free(pms);

 return map_vertice(to, from, graph->V);
}
void print_graph(struct Graph* graph){
 for (int j = 0; j < graph->E; j++){
  printf("G.add_edge(\"%d\", \"%d\", weight=%d)  \n", graph->edge[j].source, graph->edge[j].destination, graph->edge[j].capacity);
//  printf("G.add_edge(\"%d\", \"%d\", weight=%d)  \n", graph->edge[j].source, graph->edge[j].destination, graph->edge[j].base_fee);
 }
 return;
}
struct path* BellmanFord(struct Graph* graph, int from, int to, int amt)
{
    int V = graph->V;
 
    int E = graph->E;
 
    int StoreDistance[V];
    int StoreAmount[V];
 
    int i,j;
    int fee;
 
//    print_graph(graph);
    if(from != 0){
// The SP algoirthm works only when the edges are sorted in ascending order
     struct Graph* new_graph = createGraph(graph->V, graph->E);
     new_graph->V = graph->V;
     new_graph->E = graph->E;
     int new_to = map_vertices(graph, new_graph, from, to);
     struct path* pth2 = BellmanFord(new_graph, 0, new_to, amt);
     free(new_graph->edge);
     free(new_graph);

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

    for (i = 0; i < V; i++)
        StoreAmount[i] = 0;
 
    StoreAmount[from] = amt;
 
    int dcnt = 0;
    for (i = 1; i <= V-1; i++)
    {
        for (j = 0; j < E; j++)
        {
            int u = graph->edge[j].source;
            int v = graph->edge[j].destination;
//            if(graph->edge[j].capacity > CAPACITY_LIMIT){
//             graph->edge[j].base_fee = 0;
//            }
            if(StoreAmount[u] < TR_AMT/AM_BND && StoreDistance[u] != INT_MAX){
            /* small amounts */
             fee = StoreAmount[u]*((double)graph->edge[j].vf_sml/1000000) + graph->edge[j].base_fee; // This is the dynamic cost of the edge depending on the transaction amount for small amounts
            }
            else if(StoreAmount[u] > TR_AMT*AM_BND && StoreDistance[u] != INT_MAX){
            /* large amounts */
             fee = StoreAmount[u]*((double)graph->edge[j].vf_lrg/1000000) + graph->edge[j].base_fee; // This is the dynamic cost of the edge depending on the transaction amount for large amounts
            }
            else{
             fee = StoreAmount[u]*((double)graph->edge[j].variable_fee/1000000) + graph->edge[j].base_fee; // This is the dynamic cost of the edge depending on the transaction amount for averaged size transactions
            }

            if(fee < 0){
             printf("StoreAmount[u] %d \n",StoreAmount[u]);
             printf("graph->edge[j].variable_fee %d \n",graph->edge[j].variable_fee);
             printf("graph->edge[j].base_fee %d \n",graph->edge[j].base_fee);
             exit(1);
            }
//            printf("%d. trying edge: %d from: %d %d \n",j, v, u, base_fee);
            if(graph->edge[j].capacity > amt){ /* use channels with enough capacity */
             if (StoreDistance[u] + fee < StoreDistance[v] && StoreDistance[u] != INT_MAX){
                 StoreDistance[v] = StoreDistance[u] + fee;
                 StoreAmount[v] = StoreAmount[u] - fee; // This is the remaining amount when v is reached from u
//                 printf("%d. distance update at: %d from: %d %d %d \n",j, v, u, base_fee, StoreDistance[u]);
                 dcnt = dcnt + 1;
             }
            }
        }
    }

// printf("dcnt1 %d \n",dcnt);
     struct d_up* dup = (d_up*)malloc(sizeof(d_up)*(dcnt));
     if(dup == NULL){
       printf(" memory error in BellmanFord...\n");
       exit(1);
     }
     else{
//      printf("mem allocated (Kb): %lu dcnt: %d \n",sizeof(d_up)*(dcnt+1)/(1024), dcnt);
     }
    for(i=0;i< dcnt;i++){
     dup[i].at = INT_MAX;
     dup[i].from = INT_MAX;
    }

    for (i = 0; i < V; i++)
        StoreDistance[i] = INT_MAX;
 
    StoreDistance[from] = 0;
 
    for (i = 0; i < V; i++)
        StoreAmount[i] = 0;
 
    StoreAmount[from] = amt;

    dcnt = 0;
    for (i = 1; i <= V-1; i++)
    {
        for (j = 0; j < E; j++)
        {
            int u = graph->edge[j].source;
 
            int v = graph->edge[j].destination;
 

//            if(graph->edge[j].capacity > CAPACITY_LIMIT){
//             graph->edge[j].base_fee = 0;
//            }
            if(StoreAmount[u] < TR_AMT/AM_BND && StoreDistance[u] != INT_MAX){
            /* small amounts */
             fee = StoreAmount[u]*((double)graph->edge[j].vf_sml/1000000) + graph->edge[j].base_fee; // This is the dynamic cost of the edge depending on the transaction amount for small amounts
            }
            else if(StoreAmount[u] > TR_AMT*AM_BND && StoreDistance[u] != INT_MAX){
            /* large amounts */
             fee = StoreAmount[u]*((double)graph->edge[j].vf_lrg/1000000) + graph->edge[j].base_fee; // This is the dynamic cost of the edge depending on the transaction amount for large amounts
            }
            else{
             fee = StoreAmount[u]*((double)graph->edge[j].variable_fee/1000000) + graph->edge[j].base_fee; // This is the dynamic cost of the edge depending on the transaction amount for averaged size transactions
            }
            if(graph->edge[j].capacity > amt){ /* use channels with enough capacity */
             if (StoreDistance[u] + fee < StoreDistance[v] && StoreDistance[u] != INT_MAX){
                 StoreDistance[v] = StoreDistance[u] + fee;
                 StoreAmount[v] = StoreAmount[u] - fee; // This is the remaining amount when v is reached from u
//                 printf("%d. trying edge: %d from: %d %d \n",j, v, u, fee);

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
                  dcnt = dcnt + 1;
                 }
             }
            }
        }
    }

// printf("dcnt2 %d \n",dcnt);
     struct path* pth = (path*)malloc(sizeof(path) );
     pth->nodes = (int*)malloc(sizeof(int) * graph->E );
     for(i = 0; i < graph->E; i++){
      pth->nodes[i] = 0;
     }
     pth->len = 0;
     findpaths(to, dup, dcnt, pth, graph);
     free(dup);
//     for (i = 0; i < E; i++)
//     {
//         int u = graph->edge[i].source;
//  
//         int v = graph->edge[i].destination;
//  
//         int routing_fee = graph->edge[i].routing_fee;
//  
// //        if (StoreDistance[u] + routing_fee < StoreDistance[v])
// //            printf("This graph contains negative edge cycle\n");
//     }
//  
// //    FinalSolution(StoreDistance, V);
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
      if(ii % 4 == 0){
       graph->edge[counter].source = i;
      }
      if(ii % 4 == 1){
       graph->edge[counter].destination = i;
      }
      if(ii % 4 == 2){
       graph->edge[counter].routing_fee = i;
       graph->edge[counter].base_fee = 1000;
       graph->edge[counter].capacity = cap;
       graph->edge[counter].routing_revenue = 0;
       graph->edge[counter].number_of_routed_payments = 0;
      }
      if(ii % 4 == 3){
       graph->edge[counter].variable_fee = 1000;

       graph->edge[counter].vf_lrg = 1000;
       graph->edge[counter].vf_sml = 1000;

       counter = counter + 1;
      }
      ii = ii + 1;
    }
  fclose (file);        
}

int reduce_cap(struct Graph* graph, int source, int destination, int amt ){
        
        for (int j = 0; j < graph->E; j++)
        {
          if(graph->edge[j].source == source && graph->edge[j].destination == destination) {
           graph->edge[j].capacity = graph->edge[j].capacity - amt;

           if(graph->edge[j].capacity < INIT_CAP/2) {
//            graph->edge[j].base_fee = graph->edge[j].base_fee * 2;
            graph->edge[j].variable_fee = graph->edge[j].variable_fee + FEE_CORRECTION;
            graph->edge[j].vf_lrg = graph->edge[j].vf_lrg + FEE_CORRECTION_LARGE;
            graph->edge[j].vf_sml = graph->edge[j].vf_sml + FEE_CORRECTION_SMALL;
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
           if(graph->edge[j].capacity > INIT_CAP + INIT_CAP/2) {
//            graph->edge[j].base_fee = MAX(graph->edge[j].base_fee / 2, 1 );
            graph->edge[j].variable_fee = MAX(graph->edge[j].variable_fee - FEE_CORRECTION,0);
            graph->edge[j].vf_lrg = MAX(graph->edge[j].vf_lrg - FEE_CORRECTION_LARGE, 0);
            graph->edge[j].vf_sml = MAX(graph->edge[j].vf_sml - FEE_CORRECTION_SMALL, 0);
           }
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
int get_routing_revenue(int from, int to, struct Graph* graph, int amt){
 for (int j = 0; j < graph->E; j++)
 {
   if(graph->edge[j].source == from && graph->edge[j].destination == to) {
    
    graph->edge[j].number_of_routed_payments += 1;

    if(amt < TR_AMT/AM_BND){
     graph->edge[j].routing_revenue += graph->edge[j].base_fee + amt*((double)graph->edge[j].vf_sml/1000000);
     return graph->edge[j].base_fee + amt*((double)graph->edge[j].vf_sml/1000000);
    }
    if(amt > TR_AMT*AM_BND){
     graph->edge[j].routing_revenue += graph->edge[j].base_fee + amt*((double)graph->edge[j].vf_lrg/1000000);
     return graph->edge[j].base_fee + amt*((double)graph->edge[j].vf_lrg/1000000);
    }
    else{
     graph->edge[j].routing_revenue += graph->edge[j].base_fee + amt*((double)graph->edge[j].variable_fee/1000000);
     return graph->edge[j].base_fee + amt*((double)graph->edge[j].variable_fee/1000000);
    }
   }
 }
 printf(" No such edge: %d %d \n", from,to);
 return 0;
}
int send_payment(struct payment* pm, struct Graph* graph, int* nofp){
 int capacity_critical;

 printf("sending payment from %d to %d amount: %d  \n",pm->from,pm->to,pm->amount);

 clock_t t;
 t  = clock();
 struct path* pth = BellmanFord(graph, pm->from, pm->to, pm->amount);
 t = clock() - t;
 double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
// printf("BellmanFord() took %f seconds to execute \n", time_taken);


 capacity_critical = 0;

// printf("sp len: %d ",pth->len);
 int routing_revenue = 0;

 if(pth->len > 0){
  printf("(%d) %d ->",pth->len, pth->nodes[0]);

  for(int jj=1; jj < pth->len+1 ; jj++){
   if(jj == pth->len){
   printf(" %d ", pth->nodes[jj]);
   }
   else{
    printf(" %d ->", pth->nodes[jj]);
    nofp[pth->nodes[jj]] = nofp[pth->nodes[jj]] + 1;
   }
   capacity_critical = reduce_cap(graph, pth->nodes[jj], pth->nodes[jj-1], pm->amount );
   increase_cap(graph, pth->nodes[jj-1], pth->nodes[jj], pm->amount );
   routing_revenue += get_routing_revenue(pth->nodes[jj] , pth->nodes[jj-1] , graph, pm->amount); 
  }

 }

 printf(" \n");
// printf("routing_revenue: %d \n",routing_revenue);
 free(pm);
 free(pth->nodes);
 free(pth);
 return routing_revenue;
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
    clock_t start, end;

    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    printf("Start Time : %s\n", time_str);
 
    sscanf (argv[1],"%d",&V);
    sscanf (argv[2],"%d",&E);

//    int** sim_res = (int**)malloc(sizeof(int*)*NUM_SIM);
//     for(int ii=0; ii < NUM_SIM; ii++) sim_res[ii] = (int*)malloc(sizeof(int)*NUMBER_OF_PAYMENTS);

    #pragma omp parallel for 
    for(int isim = 0; isim < NUM_SIM; isim++){

    struct Graph* graph = createGraph(V, E);
 
    int i;
    int* number_of_forwarded_payments = (int*)malloc(sizeof(int)*V);

    for(int ii=0; ii < V; ii++){
     number_of_forwarded_payments[ii] = 0;
    }

    clock_t t;
    #pragma omp critical
    {
     printf("Loading topology... %d %d \n", omp_get_thread_num(), isim);
     t  = clock();
     load_topology("ln_topology", graph, INIT_CAP);
     printf("Topology loaded... \n");
     t = clock() - t;
     double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
     printf("topology() took %f seconds to execute \n", time_taken);
    }
    int revs = 0;
// Simulate payments:
    for(int ii=1; ii < NUMBER_OF_PAYMENTS + 1; ii++){
//      sim_res[isim][ii] = get_channel_capacity(6,8, graph);

      from = get_random_number() % V;
      to = get_random_number() % V;
      while(to == from){
       to = get_random_number() % V;
      }

      int amt = get_random_number() % TR_AMT;
      while(amt == 0){
       amt = get_random_number() % TR_AMT;
      }

      printf("%d. ",ii);
      revs = revs + send_payment(create_payment(from, to, amt), graph, number_of_forwarded_payments);

      int maxrr = 0;
      int maxnr = 0;
      for (int j = 0; j < graph->E; j++)
       {
          maxrr = MAX(graph->edge[j].routing_revenue, maxrr); 
          maxnr = MAX(graph->edge[j].number_of_routed_payments, maxnr); 
       }
      for (int j = 0; j < graph->E; j++)
       {
          if(graph->edge[j].routing_revenue == maxrr && graph->edge[j].number_of_routed_payments > 5){
           printf("%d/%d u: %d v: %d rr: %f BTC r_payments: %d vf_lrg: %d vf_sml: %d maxrevenue \n",omp_get_thread_num(), ii, graph->edge[j].source, graph->edge[j].destination, (double)maxrr/SATOSHI_TO_BTC, graph->edge[j].number_of_routed_payments, graph->edge[j].vf_lrg, graph->edge[j].vf_sml);
          }
          if(graph->edge[j].number_of_routed_payments > maxnr - 2 && graph->edge[j].number_of_routed_payments > 5){
           printf("%d/%d u: %d v: %d r_payments: %d rr: %f BTC vf_lrg: %d vf_sml: %d maxnumber \n",omp_get_thread_num(), ii, graph->edge[j].source, graph->edge[j].destination, maxnr, (double)graph->edge[j].routing_revenue/SATOSHI_TO_BTC, graph->edge[j].vf_lrg, graph->edge[j].vf_sml);
          }
       }

    }
//      print_graph(graph);

    printf("%d. sum_rev: %f BTC \n",isim, (double)revs/SATOSHI_TO_BTC);
//     printf(" %5.3f \n",(float)(100*cc) / (float)NUMBER_OF_PAYMENTS);
//     for(int ii=0; ii < V; ii++){
//      printf("id %d number_of_forwarded_payments: %d \n",ii, number_of_forwarded_payments[ii]);
//     }
//     free(graph->edge);
//     free(graph);
     }
// 
//     FILE *out_file = fopen("vis", "w"); // write only
//     fprintf(out_file, "[");
//     for(int ii = 0; ii < NUMBER_OF_PAYMENTS; ii++){
//      int res0 = sim_res[0][ii];
//      int res1 = sim_res[1][ii];
//      int res2 = sim_res[2][ii];
//      int res3 = sim_res[3][ii];
//      int res4 = sim_res[4][ii];
//      int res5 = sim_res[5][ii];
//      int res6 = sim_res[6][ii];
//      int res7 = sim_res[7][ii];
//      int res8 = sim_res[8][ii];
//      int res9 = sim_res[9][ii];
//       int av = (res0+res1+res2+res3+res4+res5+res6+res7+res8+res9)/10;
//       fprintf(out_file, "[%d,",ii );
// //      fprintf(out_file, "%d, ",res0 );
// //      fprintf(out_file, "%d, ",res1 );
// //      fprintf(out_file, "%d, ",res2 );
// //      fprintf(out_file, "%d, ",res3 );
// //      fprintf(out_file, "%d, ",res4 );
// //      fprintf(out_file, "%d, ",res5 );
// //      fprintf(out_file, "%d, ",res6 );
// //      fprintf(out_file, "%d, ",res7 );
// //      fprintf(out_file, "%d, ",res8 );
//      if(ii < NUMBER_OF_PAYMENTS - 1){
//       fprintf(out_file, "%d ], \n ",av );
//      }
//      else{
//       fprintf(out_file, "%d ]\n ",av );
//      }
//     }
// 
//  fprintf(out_file, "]");
//  fclose(out_file);


//    print_graph(graph);
    mytime = time(NULL);
    time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    printf("End Time : %s\n", time_str);
 
    return 0;
}
