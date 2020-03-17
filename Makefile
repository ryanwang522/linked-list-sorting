CC ?= gcc
CFLAGS ?= -Wall -Wno-unused-function

EXEC = sorting
.PHONY: all
all: $(EXEC)

SRCS_common = \
    main.c \
	sort.c

# If the first argument is "cache-test"...
ifneq (,$(filter $(firstword $(MAKECMDGOALS)),test cache-test expr))
  # use the rest as arguments for "cache-test"
  IMPL := $(wordlist 2,3,$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(IMPL):;@:)
endif

sorting: $(SRCS_common) sort_orig.c sort_dbly.c sort_kernel_list.c sort_xor.c
	$(CC) $(CFLAGS) -o $@ $^

test: $(EXEC)
	./sorting ${IMPL} test

cache-test: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
			-e cache-misses,cache-references,instructions,cycles \
			./sorting ${IMPL} test

expr: $(EXEC)
	taskset 0x1 ./sorting ${IMPL} expr > opt_output.txt
	python3 plot.py

cmp: $(EXEC)
	./sorting 0 cmp > cmp_output.txt