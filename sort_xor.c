#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "sort.h"

typedef struct __xor_list {
    int data;
    struct __xor_list *addr;
} xor_list;

#define XOR(a, b) ((xor_list *)((uintptr_t)(a) ^ (uintptr_t)(b)))

xor_list *head = NULL;
static void *init() 
{
    return (void *)head;
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

static void insert_node(void **head, int d)
{
    xor_list *l = (xor_list *)*head;
    xor_list *tmp = malloc(sizeof(xor_list));
    tmp->data = d;
    
    if (!l) {
        tmp->addr = NULL;
    } else {
        l->addr = XOR(tmp, l->addr);
        tmp->addr = l;
    }
    *head = tmp;
}

static void front_back_split(xor_list* src, xor_list **front, xor_list **back, int *front_len, int *back_len)
{
    int len = 0;
    xor_list *fast = src,  *fast_prev = NULL, *fast_next;
    xor_list *slow = src, *slow_prev = NULL, *slow_next;
    while (fast && XOR(fast->addr, fast_prev)) {
        // fast = fast->next->next
        fast_next = XOR(fast->addr, fast_prev);
        fast = XOR(fast_next->addr, fast);
        fast_prev = fast_next;
        // slow = slow->next
        slow_next = XOR(slow->addr, slow_prev);
        slow_prev = slow;
        slow = slow_next;
        len++;
    }

    *front = src;
    if (!src) {
        *back = NULL;
    } else if (fast != NULL) {
        // odd
        xor_list *next = XOR(slow->addr, slow_prev);
        next->addr = XOR(next->addr, slow);
        *back = next;
        slow->addr = XOR(slow->addr, next);
        *front_len = len; *back_len = len + 1;
    } else {
        // even
        slow_prev->addr = XOR(slow_prev->addr, slow);
        slow->addr = XOR(slow->addr, slow_prev);
        *back = slow;
        *front_len = *back_len = len;
    }
}

/* remove the 1st node in src and append it onto dest tail */
static void move_node(xor_list **dest_tail, xor_list ** src)
{
    xor_list *target = *src;
    if (target) {
        // extract target from src
        xor_list *next = target->addr;
        if (next)
            next->addr = XOR(next->addr, target);
        *src = next;
        // append to dest tail
        target->addr = *dest_tail;
        if (*dest_tail)
            (*dest_tail)->addr = XOR((*dest_tail)->addr, target);
        else
            *dest_tail = target;
    }
}

static xor_list *my_sorted_merge(xor_list *a, xor_list *b)
{
    xor_list *hd = NULL;
    xor_list **last = &hd;
    xor_list *prev = NULL, *next = NULL;

    while (1) {
        if (!a) {
            if (*last) {
                (*last)->addr = XOR((*last)->addr, b);
                b->addr = XOR(b->addr, *last);
            } else
                *last = b;
            break;
        } else if (!b) {
            if (*last) {
                (*last)->addr = XOR((*last)->addr, a);
                a->addr = XOR(a->addr, *last);
            } else
                *last = a;
            break;
        } else {
            if (a->data <= b->data)
                move_node(last, &a);
            else
                move_node(last, &b);

            next = XOR(prev, (*last)->addr);
            if (hd->addr) {
                // length > 1
                // since next is the last element
                // next->addr will point to the second to last
                prev = next->addr;  
                last = &next;
            }
        }
    }
    return hd;
}

static xor_list *sorted_merge(xor_list *l1, xor_list* l2)
{
    xor_list *start = NULL;
    for (xor_list *merge = NULL; l1 || l2;) {
        if (!l2 || (l1 && l1->data < l2->data)) {
            xor_list *next = l1->addr;
            if (next)
                next->addr = XOR(l1, next->addr);

            if (!merge) {
                start = merge = l1;
                merge->addr = NULL;
            } else {
                merge->addr = XOR(merge->addr, l1);
                l1->addr = merge;
                merge = l1;
            }
            l1 = next;
        } else {
            xor_list *next = l2->addr;
            if (next)
                next->addr = XOR(l2, next->addr);

            if (!merge) {
                start = merge = l2;
                merge->addr = NULL;
            } else {
                merge->addr = XOR(merge->addr, l2);
                l2->addr = merge;
                merge = l2;
            }
            l2 = next;
        }
    }
    return start;
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

    return sorted_merge(left, right);
}

static void *insertion_sort(void *start)
{
    if (!start || !((xor_list *)start)->addr)
        return start;

    xor_list *sorted = (xor_list *)start;
    xor_list *next = sorted->addr;
    if (next)
        next->addr = XOR(sorted, next->addr);
    sorted->addr = NULL;

    for (xor_list *curr = next; curr;) {
        next = curr->addr;
        if (next)
            next->addr = XOR(curr, next->addr);
        curr->addr = NULL;
        sorted = sorted_merge(curr, sorted);
        curr = next;
    }
    return sorted;
}

static void *merge_sort(void *start)
{
    xor_list *hd = (xor_list *)start;
    xor_list *left, *right;
    int r_len, l_len;

    if (hd == NULL || hd->addr == NULL)
        return hd;
    
    front_back_split(hd, &left, &right, &r_len, &l_len);
    left = merge_sort(left);
    right = merge_sort(right);

    return sorted_merge(left, right);
}

static void *opt_merge_sort(void *start, int list_len, int split_thres)
{
    xor_list *hd = (xor_list *)start;
    if (hd == NULL || hd->addr == NULL)
        return hd;
    if (list_len <= split_thres)
        return insertion_sort(start);

    xor_list *left, *right;
    int left_len = 0, right_len = 0;
    front_back_split(hd, &left, &right, &left_len, &right_len);
    left = opt_merge_sort(left, left_len, split_thres);
    right = opt_merge_sort(right, right_len, split_thres);
    return sorted_merge(left, right);
}

static bool test(void **head_ref, int* ans, int len, bool verbose, Sorting *sorting) 
{
    xor_list *curr = (xor_list *)*head_ref;
    if (!curr) {
        printf("The linked list is empty!\n");
        return false;
    }
    
    qsort(ans, len, sizeof(int), cmp);
    *head_ref = curr = sorting->sort(curr);

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

    if (verbose)
        sorting->print(*head_ref, true);
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
    .sort = merge_sort,
    .opt_sort = opt_merge_sort,
    .test = test,
    .list_free = delete_xor_list,
};