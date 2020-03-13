#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "sort.h"

static list *head = NULL;
static void *init()
{
    return (void *)head;
}

static void push(void **head_ref, int data)
{
    list *new_head = malloc(sizeof(list));
    new_head->data = data;
    new_head->next = (list *)*head_ref;
    new_head->prev = NULL;
    *head_ref = new_head;
    return;
}

static void print(void *head, bool new_line)
{
    list *curr = (list *)head;
    if (!curr)
        printf("The linked list is empty!\n");

    while (curr) {
        printf("%d ", curr->data);
        curr = curr->next;
    }
    if (new_line)
        printf("\n");
}

static void *sort(void *start)
{
    if (!start || !((list *)start)->next)
        return start;
    list *left = (list *)start;
    list *right = left->next;
    left->next = NULL; // partition input list into left and right sublist

    left = sort(left); // list of single element is already sorted
    right = sort(right); // sorted right sublist

    // insertion until the two sublists both been traversed
    // merge is always infront of the insertion position
    for (list *merge = NULL; left || right;) {
        // right list hasn't reach the end or
        // left node has found its position for inserting
        if (right == NULL || (left && left->data < right->data)) {
            if (!merge) {
                // start points to the node with min value
                // merge starts from min value
                start = merge = left; // LL1
            }
            else {
                // insert left node between merge and right point to
                merge->next = left; // LL2
                merge = merge->next; 
            }
            left = left->next; // LL3
        }
        else {
            if (!merge) {
                start = merge = right; // LL4
            } else {
                // shift until right == NULL
                // inset left between merge and right when min is in left sublist (LL1->LL5-> shift until right == NULL)
                merge->next = right; // LL5
                merge = merge->next;
            }
            right = right->next; // LL6
        }
    }
    return start;
}

static void list_free(void **head_ref)
{
    list *cur = (list *)*head_ref;
    list *tp;
    while (cur != NULL) {
        tp = cur;
        cur = cur->next;
        free(tp);
    }
    *head_ref = NULL;
}

static bool test(void *head, int* ans, int len, Sorting *sorting) 
{
    list *curr = (list *)head;
    if (!curr) {
        printf("The linked list is empty!\n");
        return false;
    }
    
    qsort(ans, len, sizeof(int), cmp);
    head = curr = sorting->sort(head);

    int i = 0;
    while (i < len) {
        if (curr->data != ans[i]) {
            return false;
        }
        curr = curr->next;
        i++;
    }
    sorting->print(head, false);
    sorting->list_free((void **)&head);
    return true;
}

Sorting orig_sorting = {
    .initialize = init,
    .push = push,
    .print = print,
    .sort = sort,
    .test = test,
    .list_free = list_free,
};