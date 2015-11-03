#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timing.h"
#include "util.hpp"

using namespace std;

void exclusive_scan_serial(int* nums, int len, int* output);
int find_repeats_serial(int *nums, int len, int *outputB, int* outputC);

int main(int argc, char * argv[])
{
    printf("\n----------------------- GPU TEST BEGIN ---------------------\n\n");

    static int test_iteration = 3;
    int rMin = 0, rMax = 10000;
    int N = 1 * 1000 * 1000;
    // int N = 5;
    // get the needed len by rounding the input array length to the next power of 2 to simplify the calculation
    int len = roundPowerTwo(N);
    // printf("The actual len is : %d\n\n", len);
        
    // first create enough random number in this range (0, 1,000,000)
    int* input = new int[len];
    int* outputB = new int[len];
    int* outputC = new int[len];
    
    // initialize the input integers
    createRand(input, len, rMin, rMax);

    // input[0] = 1;
    // input[1] = 1;
    // input[2] = 2;
    // input[3] = 2;
    // input[4] = 3;

    // printf("input:\n");
    // for (int i = 0; i < N; ++i)
    // {
    //     printf("%d\n", input[i]);
    // }

    printf("First run the [serial find repeats]...\n");
    double minSerialFind = 1e30;
    int repeat_count;
    for (int i = 0; i < test_iteration; ++i)
    {
        // start to record time consumption
        reset_and_start_timer();
        // call the sqrt_serial function to calculate all the inputs
        repeat_count = find_repeats_serial(input, N, outputB, outputC);
        // stop timer and print out total cycles
        double one_round = get_elapsed_mcycles();
        printf("time of serial run %d:\t\t\t[%.3f] million cycles\n", i + 1, one_round);
        minSerialFind = min(minSerialFind, one_round);
    }
    printf("[best of find_serial]:\t\t\t[%.3f] million cycles\n", minSerialFind);

    int unique_count = N - repeat_count;
    len = roundPowerTwo(unique_count);

    printf("\nNow run the [serial exclusive scan]...\n");
    double minSerialScan = 1e30;
    for (int i = 0; i < test_iteration; ++i)
    {
        // start to record time consumption
        reset_and_start_timer();
        // call the sqrt_serial function to calculate all the inputs
        exclusive_scan_serial(input, len, outputB);
        // stop timer and print out total cycles
        double one_round = get_elapsed_mcycles();
        printf("time of serial run %d:\t\t\t[%.3f] million cycles\n", i + 1, one_round);
        minSerialScan = min(minSerialScan, one_round);
    }
    printf("[best of scan_serial]:\t\t\t[%.3f] million cycles\n", minSerialScan);

    // // now check the result
    // printf("Now check the correctness...");
    // if (true)
    //     printf("\t\tOutput correct!\n\n");
    // else
    //     printf("\t\tOutput incorrect!\n\n");

    // printf("output: %d\n", repeat_count);
    // for (int i = 0; i < repeat_count; ++i)
    // {
    //     printf("%d\n", output[i]);
    // }

    printf("\n-------------------------- TEST END --------------------------\n\n");

    delete[] input;
    delete[] outputB;
    delete[] outputC;
}
