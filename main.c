#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "sort.h"

#define MAX_SPLIT_SIZE 1000
#define VERBOSE false

Sorting *impl_provider[] = {
    &orig_sorting,
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
    assert((argc < 4) && "Usage: ./sorting impl_selector [mode]");
    assert((atoi(argv[1]) < 3) && "Can't find impl");
    int impl_i = atoi(argv[1]);
    Sorting *sorting_impl = impl_provider[impl_i];
    srand(time(NULL));

    if (strcmp(argv[2], "test") == 0) {
        int correct = 0;
        for (int i = 0; i < 100; i++) {
            int testcase_len = rand() % 50 + 1;
            int *testcase = malloc(sizeof(int) * testcase_len);
            for (int j = 0; j < testcase_len; j++)
                testcase[j] = rand() % 50;
            
            void *head = sorting_impl->initialize();

            for (int j = testcase_len - 1; j >= 0; j--)
                sorting_impl->push((void **)&head, testcase[j]);

            if (VERBOSE) {
                printf("Testcase %d: ", i+1);
                sorting_impl->print(head, false);
                printf(" -->  ");
            }

            if (sorting_impl->test(&head, testcase, testcase_len, VERBOSE, sorting_impl))
                correct++;

            sorting_impl->list_free((void **)&head);
            free(testcase);
        }
        printf("Testcases %d/100 passed.\n", correct);
    } else if (strcmp(argv[2], "expr") == 0) {
        /* Experiments for quiz3 */

        // find optial split size
        int testcase_len = MAX_SPLIT_SIZE * 2;
        for (int thres = 1; thres < MAX_SPLIT_SIZE; thres++) {
            int *testcase = malloc(sizeof(int) * testcase_len);
            for (int j = 0; j < testcase_len; j++)
                testcase[j] = rand() % testcase_len;
            void *head = sorting_impl->initialize();
            for (int i = testcase_len - 1; i >= 0; i--)
                sorting_impl->push((void **)&head, testcase[i]);
            
            // optimized sorting
            struct timespec start, end;
            clock_gettime(CLOCK_REALTIME, &start);
            head = sorting_impl->opt_sort(head, testcase_len, thres);
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
    } else if (strcmp(argv[2], "cmp") == 0) {
        int split_size = 20;
        int testcase_len = 1000;
        for (int i = 0; i < 50; i++) {
            for (int j = 0; j < 3; j++) {
                sorting_impl = impl_provider[j];
                void *head = sorting_impl->initialize();
                for (int k = testcase_len - 1; k >= 0; k--)
                    sorting_impl->push((void **)&head, rand() % testcase_len);

                struct timespec start, end;
                clock_gettime(CLOCK_REALTIME, &start);
                head = sorting_impl->opt_sort(head, testcase_len, split_size);
                clock_gettime(CLOCK_REALTIME, &end);
                printf("%lf ", diff_in_second(start, end));
                sorting_impl->list_free(&head);
            }
            printf("\n");
        }
    }
    return 0;
}