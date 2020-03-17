# linked-list-sorting
Implement insertion sort in singular / circular-doubly linked-list by utilizing linux kernel `list.h`.
This repoistory is for the follow-up questions implementation of quiz1@sysprog-2020.

## Usage
Currently, there are three implementations:

0: Original version in the quiz.

1: Circular-doubly linked-list by utilizing the concept within Linux kernel `list.h`.

2: XOR linked-list implemtation.

Run particular implementation by passing the implementation number as arugment.

### Testing correctness
For testing, you can pass the implementation number which you want to run as command line argument, e.g.
```
$ cd linked-list-sorting
$ make
$ ./sorting 2 
```
or 
```
$ make test [IMPL]
```

Change definition of `VERBOSE` in `line11` in `main.c` to enable / disable printing the list before and after sorting.

If you would like to obeserve cache miss rate by `perf`, run following commands:
```
$ make cache-test 2
```

### Run experiments
Run sorting performance experiments with different split sizes.
```
$ make expr 2
```

In `make cache-test` and `make expr`, the third argument indicates which implementation which will be executed.