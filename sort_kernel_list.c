#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

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

static int front_back_split(list_head *src, list_head *left)
{
    list_head *fast, *slow;
    fast = slow = src->next;
    int len = 0;
    while (fast != src && !list_is_last(fast, src)) {
        slow = slow->next;
        fast = fast->next->next;
        len++;
    }

    if (list_empty(src)) 
        return 0;
    else if (list_is_last(fast, src)) {
        // odd length
        list_cut_position(left, src, slow);
    } else {
        // even length
        list_cut_position(left, src, slow->prev);
    }
    return len;
}

static list_head *sorted_merge(list_head *left, list_head *right)
{
    LIST_HEAD(merged_head);
    list_head *merged = &merged_head;
    while (1) {
        if (list_empty(left)) {
            list_splice_tail(right, merged);
            break;
        } else if (list_empty(right)) {
            list_splice_tail(left, merged);
            break;
        } else {
            if (list_first_entry(left, Node, list_h)->data 
                < list_first_entry(right, Node, list_h)->data)
                list_move_tail(left->next, merged);
            else
                list_move_tail(right->next, merged);
        }
    }
    INIT_LIST_HEAD(right);
    list_splice_tail(merged, right);
    return right;
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

static void *merge_sort(void *start)
{
    if (list_empty(start) || ((list_head *)start)->next == ((list_head *)start)->prev)
        return start;

    LIST_HEAD(left_head);
    front_back_split((list_head *)start, &left_head);

    list_head *right = merge_sort(start);
    list_head *left = merge_sort(&left_head);
    start = sorted_merge(left, right);
 
    return start;
}

static void *opt_merge_sort(void *start, int split_thres)
{
    if (list_empty(start) || ((list_head *)start)->next == ((list_head *)start)->prev)
        return start;

    LIST_HEAD(left_head);
    int partition_len = front_back_split((list_head *)start, &left_head);
    list_head *right = NULL, *left = NULL;

    if (partition_len <= split_thres) {
        left = sort(left);
        right = sort(right);
    } else { 
        right = opt_merge_sort(start, split_thres);
        left = opt_merge_sort(&left_head, split_thres);
    }
    start = sorted_merge(left, right);

    return start;
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
    sorting->list_free((void **)&head);
    return true;
}

Sorting kernel_list_sorting = {
    .initialize = init,
    .push = push,
    .print = print,
    .sort = merge_sort,
    .opt_sort = opt_merge_sort, 
    .test = test,
    .list_free = list_free,
};