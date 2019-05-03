# ParallelQuickMergeSort
QuickMerge Sort version Parallelized using OpenMP and MPI during the Parallel and Massive Computation Subject, URV.

## Build
For building the code you can use the Makefile included in the src folder:
```
$ make
```

## OpenMP version Usage

For a sigle execution:
```
$ ./parallel.obj num_data num_threads
```
* num_data ->  lenght of the array that will be sorted
* num_threads -> number of threads that will be used for parallelizing the code

You can also use the execute script to carry out some executions using a different number of threads
```
$ ./execute.sh
```

Finally use the getResults script for getting the results into a CSV
```
$ ./getResults.sh INPUT_FILE num_threads
```

## Authors

* **Oussama El Azizi** - [OussamaElazizi](https://github.com/OussamaElazizi)
* **Cristòfol Daudén Esmel** - [toful](https://github.com/toful)
