#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "sort.h"

typedef struct __xor_list {
    int data;
    struct __xor_list *addr;
} xor_list;

#define XOR(a, b) ((xor_list *)((uintptr_t)(a) ^ (uintptr_t)(b)))

xor_list *list_head = NULL;
static void *init() 
{
    return (void *)list_head;
}

static void print(void *head, bool new_line)
{
    xor_list *curr = (xor_list *)head;
    if (!curr)
        printf("The linked list is empty!\n");

    xor_list *prev = NULL;
    xor_list *next = NULL;
    while (curr) {
        printf("%d ", curr->data);
        next = XOR(prev, curr->addr);
        prev = curr;
        curr = next;
    }
    if (new_line)
        printf("\n");
}

static void insert_node(void **l, int d)
{
    xor_list *tmp = malloc(sizeof(xor_list));
    tmp->data = d;

    if (!((xor_list *)*l)) {
        tmp->addr = NULL;
    } else {
        ((xor_list *)*l)->addr = XOR(tmp, ((xor_list *)*l)->addr);
        tmp->addr = (xor_list *)*l;
    }
    *l = tmp;
}

static void *sort(void *start)
{
    if (!start || !((xor_list *)start)->addr)
        return start;

    xor_list *left = (xor_list *)start, *right = ((xor_list *)start)->addr;
    left->addr = NULL;
    right->addr = XOR(right->addr, left);

    left = sort(left);
    right = sort(right);

    for (xor_list *merge = NULL; left || right;) {
        if (!right || (left && left->data < right->data)) {
            xor_list *next = left->addr;
            if (next)
                next->addr = XOR(left, next->addr);

            if (!merge) {
                start = merge = left;
                merge->addr = NULL;
            } else {
                merge->addr = XOR(merge->addr, left);
                left->addr = merge;
                merge = left;
            }
            left = next;
        } else {
            xor_list *next = right->addr;
            if (next)
                next->addr = XOR(right, next->addr);

            if (!merge) {
                start = merge = right;
                merge->addr = NULL;
            } else {
                merge->addr = XOR(merge->addr, right);
                right->addr = merge;
                merge = right;
            }
            right = next;
        }
    }

    return start;
}

static bool test(void *head, int* ans, int len, Sorting *sorting) 
{
    xor_list *curr = (xor_list *)head;
    if (!curr) {
        printf("The linked list is empty!\n");
        return false;
    }
    
    qsort(ans, len, sizeof(int), cmp);
    head = curr = sorting->sort(head);

    int i = 0;
    xor_list *prev = NULL;
    xor_list *next = NULL;
    while (i < len && curr) {
        if (curr->data != ans[i])
            return false;
        next = XOR(prev, curr->addr);
        prev = curr;
        curr = next;
        i++;
    }
    sorting->print(head, false);
    sorting->list_free((void **)&head);
    return true;
}

static void delete_xor_list(void **head_ref)
{
    xor_list *l = (xor_list *)*head_ref;
    while (l) {
        xor_list *next = l->addr;
        if (next)
            next->addr = XOR(next->addr, l);
        free(l);
        l = next;
    }
}

Sorting xor_sorting = {
    .initialize = init,
    .push = insert_node,
    .print = print,
    .sort = sort,
    .test = test,
    .list_free = delete_xor_list,
};