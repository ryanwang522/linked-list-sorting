#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "sort.h"

#define MAX_SPLIT_SIZE 1000

Sorting *impl_provider[] = {
    &orig_sorting,
    &dbly_sorting,
    &kernel_list_sorting,
    &xor_sorting
};

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    assert((argc < 4) && "Usage: ./sorting impl_selector [expr]");
    assert((atoi(argv[1]) < 4) && "Can't find impl");
    int impl_i = atoi(argv[1]);
    Sorting *sorting_impl = impl_provider[impl_i];
    bool expr = argc == 3 && strcmp(argv[2], "expr") == 0 ? true : false; 
    srand(time(NULL));

    if (!expr) {
        int correct = 0;
        for (int i = 0; i < 100; i++) {
            int testcase_len = 500; //rand() % 500 + 1;
            int *testcase = malloc(sizeof(int) * testcase_len);
            for (int j = 0; j < testcase_len; j++)
                testcase[j] = rand() % 500;
            
            void *head = sorting_impl->initialize();

            for (int j = testcase_len - 1; j >= 0; j--)
                sorting_impl->push((void **)&head, testcase[j]);

            if (sorting_impl->test(head, testcase, testcase_len, sorting_impl))
                correct++;
            // memory deallocation of list is conducted in fucntion `test` 
            free(testcase);
        }
        printf("Testcases %d/100 passed.\n", correct);
    } else {
        if (impl_i == 1) {
            printf("Optimized merge sort in dbly impl is not supported\n");
            return 0;
        }

        // TODO: find optial split siz
        int testcase_len = MAX_SPLIT_SIZE * 2;
        for (int thres = 1; thres < MAX_SPLIT_SIZE; thres++) {
            int *testcase = malloc(sizeof(int) * testcase_len);
            for (int j = 0; j < testcase_len; j++)
                testcase[j] = rand() % 500;
            void *head = sorting_impl->initialize();
            for (int i = testcase_len - 1; i >= 0; i--)
                sorting_impl->push((void **)&head, testcase[i]);
            
            // optimized sorting
            struct timespec start, end;
            clock_gettime(CLOCK_REALTIME, &start);
            head = sorting_impl->opt_sort(head, thres);
            clock_gettime(CLOCK_REALTIME, &end);
            printf("%lf", diff_in_second(start, end));
            sorting_impl->list_free(&head);

            head = sorting_impl->initialize();
            for (int i = testcase_len - 1; i >= 0; i--)
                sorting_impl->push((void **)&head, testcase[i]);

            // naive mergesort
            clock_gettime(CLOCK_REALTIME, &start);
            head = sorting_impl->sort(head);
            clock_gettime(CLOCK_REALTIME, &end);
            printf(" %lf\n", diff_in_second(start, end));
            sorting_impl->list_free(&head);
            free(testcase);
        }
    }
    return 0;
}