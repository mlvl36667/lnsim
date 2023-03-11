## Before you start

For CPU parallel run, install libomp-dev. 

```
sudo apt install libomp-dev
```

## Run

A widely used topology can be downloaded from here:

https://www.rene-pickhardt.de/listchannels20220412.json

Then, conv.py can be used to convert the topology. Modify it according to your desires. The program can only handle a sorted list of vertices in the `ln_topology` file. If you want to use larger topologies, create and convert accordingly. 

```
export OMP_NUM_THREADS=10 && g++ -o sim sim.cpp -fopenmp && ./sim [NUM_OF_VERTICES] [NUM_OF_EDGES] 
```
