# linked-list-sorting
Implement insertion sort in singular / circular-doubly linked-list by utilizing linux kernel `list.h`.
This repoistory is for the follow-up questions implementation of quiz1@sysprog-2020.

## Usage
Currently, there are four implementations:

0: Original version in the quiz.

1: Circular-doubly linked-list implemtation.

2: Refactor by utilizing the concept within Linux kernel `list.h`.

3: XOR linked-list implemtation.

Run particular implementation by passing the implementation number as arugment.

### Testing correctness
For testing, you can pass the implementation number which you want to run as the second command line argument, e.g.
```
$ cd linked-list-sorting
$ make
$ ./sorting 2 
```
Change definition of `VERBOSE` in `line11` in `main.c` to enable / disable printing the list before and after sorting.

If you would like to obeserve cache miss rate by `perf`, run following commands:
```
$ make cache-test 0
```

### Run experiments
Run sorting performance experiments with different split sizes.
```
$ make expr 0
```

In `make cache-test 0` and `make expr 3`, the third argument indicates which implementation which will be executed.