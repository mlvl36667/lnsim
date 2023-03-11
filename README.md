## Before you start

```
sudo apt install libomp-dev
```

## Run
```
export OMP_NUM_THREADS=10 && g++ -o sim sim.cpp -fopenmp && ./sim [NUM_OF_VERTICES] [NUM_OF_EDGES] 
```
