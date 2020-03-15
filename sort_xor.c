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

static int front_back_split(xor_list* src, xor_list **front, xor_list **back)
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
    } else {
        // even
        slow_prev->addr = XOR(slow_prev->addr, slow);
        slow->addr = XOR(slow->addr, slow_prev);
        *back = slow;
    }
    return len;
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

static xor_list *sorted_merge(xor_list *a, xor_list *b)
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

static void *insertion_sort(void *start)
{
    if (!start || !((xor_list *)start)->addr)
        return start;

    xor_list *left = (xor_list *)start, *right = ((xor_list *)start)->addr;
    left->addr = NULL;
    right->addr = XOR(right->addr, left);

    left = insertion_sort(left);
    right = insertion_sort(right);

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

static void *merge_sort(void *start)
{
    xor_list *hd = (xor_list *)start;
    xor_list *left, *right;

    if (hd == NULL || hd->addr == NULL)
        return hd;
    
    front_back_split(hd, &left, &right);
    left = merge_sort(left);
    right = merge_sort(right);

    return sorted_merge(left, right);
}

static void *opt_merge_sort(void *start, int split_thres)
{
    xor_list *hd = (xor_list *)start;
    xor_list *left, *right;

    if (hd == NULL || hd->addr == NULL)
        return hd;

    int partition_len = front_back_split(hd, &left, &right);
    if (partition_len <= split_thres) {
        left = insertion_sort(left);
        right = insertion_sort(right);
    } else {
        left = merge_sort(left);
        right = merge_sort(right);
    }
    return sorted_merge(left, right);
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
    .sort = merge_sort,
    .opt_sort = opt_merge_sort,
    .test = test,
    .list_free = delete_xor_list,
};