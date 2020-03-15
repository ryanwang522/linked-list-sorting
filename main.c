#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "sort.h"

Sorting *impl_provider[] = {
    &orig_sorting,
    &dbly_sorting,
    &kernel_list_sorting,
    &xor_sorting
};

int main(int argc, char *argv[])
{
    assert((argc == 2) && "Usage: ./sorting impl_selector");
    assert((atoi(argv[1]) < 4) && "Can't find impl");
    Sorting *sorting_impl = impl_provider[atoi(argv[1])];
    
    int correct = 0;
    int testcase_len = 10;
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        int testcase[10] = { 0 };
        for (int j = 0; j < testcase_len; j++)
            testcase[j] = rand() % 50;
        
        void *head = sorting_impl->initialize();

        for (int j = testcase_len - 1; j >= 0; j--)
            sorting_impl->push((void **)&head, testcase[j]);

        printf("Testcase %d: ", i+1);
        sorting_impl->print(head, false);

        printf(" -->  ");
        if (sorting_impl->test(head, testcase, testcase_len, sorting_impl))
            correct++;
        // memory deallocation is conducted in fucntion `test` 
        printf("\n");
    }
    printf("Testcases %d/100 passed.\n", correct);

    return 0;
}