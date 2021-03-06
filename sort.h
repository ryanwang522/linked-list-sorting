#ifndef _SORT_H
#define _SORT_H

#include <stdbool.h>

typedef struct __list {
    int data;
    struct __list *next;
    struct __list *prev;
} list;

/* for qsort */
extern int cmp(const void *a, const void *b);

typedef struct __INTERFACE {
    void *(*initialize)();
    void (*push)(void **head_ref, int data);
    void (*print)(void *head, bool new_line);
    void *(*sort)(void *start);
    void *(*insertion_sort)(void *start);
    void *(*opt_sort)(void *start, int list_len, int split_thres);
    bool (*test)(void **head_ref, int *ans, int len, bool verbose, struct __INTERFACE *sorting);
    void (*list_free)(void **head);
} Sorting;

extern Sorting orig_sorting;
extern Sorting dbly_sorting;
extern Sorting kernel_list_sorting;
extern Sorting xor_sorting;

#endif