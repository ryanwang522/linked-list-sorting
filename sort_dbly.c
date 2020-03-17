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

    if (!((list *)*head_ref)) {
        // the list is empty, create a single node
        new_head->next = new_head->prev = new_head;
        *head_ref = new_head;
        return;
    }

    list *last = ((list *)*head_ref)->prev;
    new_head->next = *head_ref;
    new_head->prev = last;
    last->next = ((list *)*head_ref)->prev = new_head;
    *head_ref = new_head;
    return;
}

static void print(void *head, bool new_line)
{
    list *curr = (list *)head;
    if (!curr)
        printf("The linked list is empty!\n");

    while (curr->next != (list *)head) {
        printf("%d ", curr->data);
        curr = curr->next;
    }
    printf("%d ", curr->data);
    if (new_line)
        printf("\n");
}

static void *sort(void *start)
{
    // check if input contains only 0/1 node,
    if (!start || ((list *)start)->next == (list *)start)
        return start;

    list *left = (list *)start;
    list *right = left->next;
    
    // one element in left list
    left->next = left; 
    // tail element's next point to the begin of right
    left->prev->next = right; 
    // right head's prev point to the tail element
    right->prev = left->prev; 
    // one element in left list
    left->prev = left;

    left = sort(left);
    right = sort(right);

    // insert to the sorted right list
    // loop until all elements are traversed
    for (list *merge = NULL; left || right != start;) {
        // right list hasn't reach the end (go back to the head of sorted list) or
        // left node has found its position for inserting
        if (right == (list *)start || (left && left->data < right->data)) {
            if (!merge) {
                start = merge = left;
            } else {
                // merge is among right list
                // insert left node after the node pointed by merge
                left->prev = merge;
                left->next = merge->next;
                merge->next = left;
                left->next->prev = left;
                // shift merge pointer
                merge = merge->next;
            }
            left = NULL;
        } else {
            if (!merge) {
                start = merge = right;
            } else {
                // when merge points to the left element
                // (left sublist contains only one element)
                if (merge == merge->next) {
                    // push merge on to right's head
                    merge->next = right;
                    merge->prev = right->prev;
                    right->prev->next = merge;
                    right->prev = merge;
                }
                // shift merge pointer
                merge = merge->next;
            }
            right = right->next;
        }
    }
    return start;
}

static void list_free(void **head)
{
    if (head && (list *)*head) {
        list *next = ((list *)*head)->next;
        while (next && (next != (list *)*head)) {
            list *tmp = next;
            next = next->next;
            free(tmp);
        }

        free((list *)*head);
        *head = NULL;
    }
}

static bool test(void **head_ref, int* ans, int len, bool verbose, Sorting *sorting) 
{
    list *curr = (list *)*head_ref;
    if (!curr) {
        printf("The linked list is empty!\n");
        return false;
    }
    
    qsort(ans, len, sizeof(int), cmp);
    *head_ref = curr = sorting->sort(curr);

    int i = 0;
    while (i < len) {
        if (curr->data != ans[i]) {
            return false;
        }
        curr = curr->next;
        i++;
    }

    if (verbose)
        sorting->print(*head_ref, true);
    return true;
}

Sorting dbly_sorting = {
    .initialize = init,
    .push = push,
    .print = print,
    .sort = sort,
    .test = test,
    .list_free = list_free,
};