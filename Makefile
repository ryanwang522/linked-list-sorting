CC ?= gcc
CFLAGS ?= -Wall

EXEC = sorting
.PHONY: all
all: $(EXEC)

SRCS_common = \
    main.c \
	sort.c

sorting: $(SRCS_common) sort_orig.c sort_dbly.c sort_kernel_list.c
	$(CC) $(CFLAGS) -o $@ $^
