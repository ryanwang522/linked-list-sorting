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

static void *insertion_sort(void *start)
{
    if (!start || !((list *)start)->next)
        return start;
    list *left = (list *)start;
    list *right = left->next;
    left->next = NULL; // partition input list into left and right sublist

    left = insertion_sort(left); // list of single element is already sorted
    right = insertion_sort(right); // sorted right sublist

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
    .sort = merge_sort,
    .test = test,
    .list_free = list_free,
};