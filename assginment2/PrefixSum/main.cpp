#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "timing.h"
#include "util.hpp"

using namespace std;

void exclusive_scan_serial(int* nums, int len, int* output);
int find_repeats_serial(int* nums, int len, int* outputB, int* outputC);

int main(int argc, char* argv[])
{
    printf("\n----------------------- GPU TEST BEGIN ---------------------\n\n");

    static int test_iteration = 3;

    int N, len;
    string line;
    int* input;
    if (argc == 1)
    {
        N = 1 * 1000 * 1000;
        // get the needed len by rounding the input array length to the next power of 2 to simplify the calculation
        len = roundPowerTwo(N);
        // printf("The actual len is : %d\n\n", len);
        
        input = new int[len];
        // first create enough random number in this range (0, 1,000,000)
        // initialize the input integers
        int rMin = 0, rMax = 10000;
        createRand(input, len, rMin, rMax);
    }
    // read input file
    else
    {
        string file_name(argv[1]);
        ifstream sample(file_name);
        if (sample.is_open())
        {
            std::getline(sample, line);
            sample.close();
            char* line_chars = new char[line.size()];
            sprintf(line_chars, "%s", line.c_str());
            char* token = strtok(line_chars, " ,");
            // first argument is the length
            N = atoi(token);

            len = roundPowerTwo(N);
            // now initialize the input numbers from file
            input = new int[len];
            for (int i = 0; i < len; ++i)
            {
                if (i < N)
                {
                    token = strtok(NULL, " ,");
                    input[i] = atoi(token);
                }
                else
                {
                    input[i] = 0;                    
                }
            }
        }
        else
        {
            printf("File: [%s] Not Found!\n", file_name.c_str());
            printf("Exit now...\n");
            printf("\n-------------------------- TEST END --------------------------\n\n");
            exit(0);
        }
    }

    // printf("input:\n");
    // for (int i = 0; i < N; ++i)
    // {
    //     printf("%d\n", input[i]);
    // }

    int* outputB = new int[len];
    int* outputC = new int[len];
    

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
        exclusive_scan_serial(outputC, len, outputB);
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
