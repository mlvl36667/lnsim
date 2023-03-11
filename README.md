# lnsim

install: sudo apt install libomp-dev

run: export OMP_NUM_THREADS=10 && g++ -o sim sim.cpp -fopenmp && ./sim [NUM_OF_VERTICES] [NUM_OF_EDGES] 
