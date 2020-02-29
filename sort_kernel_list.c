#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "sort.h"
#include "list.h"

typedef struct Node {
    int data;
    struct list_head list_h;
} Node;

typedef struct list_head list_head;

static LIST_HEAD(list_start);
static void *init()
{
    return (void *)&list_start;
}

static void print(void *start, bool newline)
{
    Node *curr_node;
    list_for_each_entry(curr_node, (list_head *)start, list_h)
        printf("%d ", curr_node->data);
    if (newline)
        printf("\n");
}

static void push(void **head_ref, int data)
{
    Node *tmp = malloc(sizeof(Node));
    tmp->data = data;
    INIT_LIST_HEAD(&(tmp->list_h));
    list_add(&(tmp->list_h), (list_head *)*head_ref);
}

static void *sort(void *start)
{
    if (list_empty(start) || ((list_head *)start)->next == ((list_head *)start)->prev)
        return start;
    
    LIST_HEAD(new_start);
    list_head *left = &new_start;
    list_cut_position(left, (list_head *)start, ((list_head *)start)->next);
    list_head *right = (list_head *)start;

    left = sort(left);
    right = sort(right);

    // insertion 
    struct list_head *merge;
    int left_data = list_entry(left->next, Node, list_h)->data;
    list_for_each(merge, right)
        if (left_data < list_entry(merge, Node, list_h)->data)
            break;
    list_add_tail(left->next, merge);
    INIT_LIST_HEAD(left);
    return right;
}

static void list_free(void **head_ref)
{
    Node *next = list_first_entry((list_head *)*head_ref, Node, list_h);
    if (head_ref) {
        while (&next->list_h != (list_head *)*head_ref) {
            Node *tmp = next;
            next = list_next_entry(next, list_h);
            free(tmp);
        }
        INIT_LIST_HEAD(*head_ref);
    }
}

static bool test(void *head, int* ans, int len, Sorting *sorting) 
{
    list_head *curr = (list_head *)head;
    if (!curr) {
        printf("The linked list is empty!\n");
        return false;
    }
    
    qsort(ans, len, sizeof(int), cmp);
    head = curr = sorting->sort(head);

    int i = 0;
    while (i < len) {
        if (list_entry(curr->next, Node, list_h)->data != ans[i])
            return false;
        curr = curr->next;
        i++;
    }
    sorting->print((list_head *)head, false);
    return true;
}

Sorting kernel_list_sorting = {
    .initialize = init,
    .push = push,
    .print = print,
    .sort = sort,
    .test = test,
    .list_free = list_free,
};