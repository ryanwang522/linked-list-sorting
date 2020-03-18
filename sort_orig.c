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

static void front_back_split(list *src, list **front, list **back)
{
    list *fast = src;
    list *slow = src;
    list *prev = NULL;
    while (fast != NULL && fast->next != NULL) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    *front = src;
    if (src == NULL) {
        // empty source
        *back = NULL;
    } else if (fast != NULL) {
        // odd, cut postion is after the slow pointer
        *back = slow->next;
        slow->next = NULL;
    } else {
        // even, cut postion is in front of the slow pointer
        *back = slow;
        prev->next = NULL;
    }
}

static void split(list *src, list **front, list **back, int *front_len, int *back_len)
{
    list *fast = src;
    list *slow = src;
    list *prev = NULL;
    int len = 0;
    while (fast != NULL && fast->next != NULL) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
        len++;
    }

    *front = src;
    if (src == NULL) {
        // empty source
        *back = NULL;
    } else if (fast != NULL) {
        // odd, cut postion is after the slow pointer
        *back = slow->next;
        slow->next = NULL;
        *front_len = len; *back_len = len + 1;
    } else {
        // even, cut postion is in front of the slow pointer
        *back = slow;
        prev->next = NULL;
        *front_len = *back_len = len;
    }
}

static void move_node(list **dest, list **src)
{
    list *target = *src;
    if (target != NULL) {
        *src = target->next;
        target->next = *dest;
        *dest = target;
    }
}

static list *sorted_merge(list *a, list *b)
{
    list *hd = NULL;
    list **last = &hd;

    while (1) {
        if (a == NULL) {
            *last = b;
            break;
        } else if (b == NULL) {
            *last = a;
            break;
        } else {
            if (a->data <= b->data)
                move_node(last, &a);
            else 
                move_node(last, &b);
            last = &((*last)->next);
        }
    }
    return hd;
}

static void *insertion_sort(void *start)
{
    if (!start || !((list *)start)->next)
        return start;

    list *nxt = ((list *)start)->next;
    list *sorted = (list *)start;
    sorted->next = NULL;
    for (list *node = nxt; node;) {
        nxt = node->next;
        node->next = NULL;
        sorted = sorted_merge(sorted, node);
        node = nxt;
    }
    return sorted;
}

static void *merge_sort(void *start)
{
    list *hd = (list *)start;
    list *list1;
    list *list2;

    if (hd == NULL || hd->next == NULL)
        return hd;

    front_back_split(hd, &list1, &list2);
    list1 = merge_sort(list1);
    list2 = merge_sort(list2);
    
    return sorted_merge(list1, list2);
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
    // merge is always in front of the insertion position
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

static void *opt_merge_sort(void *start, int list_len, int split_thres)
{
    list *hd = (list *)start;
    if (hd == NULL || hd->next == NULL)
        return hd;

    if (list_len <= split_thres)
        return insertion_sort(start);

    int right_len = 0, left_len = 0;
    list *right, *left;
    split(hd, &left, &right, &left_len, &right_len);
    right = opt_merge_sort(right, right_len, split_thres);
    left = opt_merge_sort(left, left_len, split_thres);
    return sorted_merge(right, left);
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

Sorting orig_sorting = {
    .initialize = init,
    .push = push,
    .print = print,
    .sort = merge_sort,
    .insertion_sort = insertion_sort,
    .opt_sort = opt_merge_sort,
    .test = test,
    .list_free = list_free,
};