# CS6346 Multicore Programming - Project 1 Lock Algorithms
This project implements several lock algorithms in C++ for the purpose of measuring performance differences between different
locks and how locking contention affects system throughout. The following locks are implemented:
- Black Box Filter Lock
- Classic Filter Lock
- Bakery Lock
- Tournament Tree Lock
- TAS Lock
- TTAS Lock
- Ticket Lock
- Anderson Lock
- CHL Lock
- MCS Lock

## main.cpp
The main program is a driver to test system performance and throughput of each lock. When run it will
construct the lock under test, start a number of threads, and measure the elapsed time for all threads
to finish some number of increment operations on a shared integer value.

The results of each run are printed to stdout and the average throughput over all runs is 
calculated and printed for each lock.

## Compile and run the code
To compile the code, there is a Makefile included. Run the following command:
```
$ make
```
to compile all source files. The final result will be written to ./bin/main.exe

To compile with debug symbols and debugging logs for thread progress run:
```
$ make debug
```
After compiling the debug build, if you wish to compile without debugging again you must first clean the repository with:
```
$ make clean
```

And finally to run the tests run the following command:
```
$ bin/main.exe <number of threads> <workload per thread> <number of test runs>
```

