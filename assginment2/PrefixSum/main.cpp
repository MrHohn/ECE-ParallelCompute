#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "timing.h"
#include "util.h"

using namespace std;

void exclusive_scan_serial(int* nums, int len, int* output);
void exclusive_scan_parallel(int* nums, int len, int* output, double& time_cost);
int find_repeats_serial(int* nums, int len, int* outputB, int* outputC);
int find_repeats_parallel(int* nums, int len, int* outputB, int* outputC, double& time_cost);

int main(int argc, char* argv[])
{
    static int test_iteration = 3;
    // define output stream
    ofstream outputFile;

    int N, len; // N is the actual input length, might be rounded to len
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
            exit(0);
        }

        // open a new file for output
        outputFile.open ("sampleoutput");
        outputFile << "input sample array:\n";
        for (int i = 0; i < N; ++i)
        {
            outputFile << input[i] << " ";
        }
        outputFile << "\n\n";
    }

    int* outputBAnswer = new int[len];
    int* outputCAnswer = new int[len];
    int* outputBResult = new int[len];
    int* outputCResult = new int[len];
    
    printf("\n--------------------- FIND REPEAT SECTION --------------------\n\n");

    printf("First run the [serial find repeats]...\n");
    double minSerialFind = 1e30;
    int repeat_count;
    for (int i = 0; i < test_iteration; ++i)
    {
        // flush the output buffer
        flushBuffer(outputBAnswer, len);
        flushBuffer(outputCAnswer, len);
        // start to record time consumption
        reset_and_start_timer();
        repeat_count = find_repeats_serial(input, N, outputBAnswer, outputCAnswer);
        // stop timer and print out total cycles
        double one_round = get_elapsed_mcycles();
        printf("*time of serial run %d:\t\t\t[%.3f] million cycles\n", i + 1, one_round);
        minSerialFind = min(minSerialFind, one_round);
    }
    printf("[best of find_serial]:\t\t\t[%.3f] million cycles\n", minSerialFind);

    /* ------------------------------------------------------------- */

    printf("\nNow run the [parallel find repeats]...\n");
    double minParallelFind = 1e30;
    int repeat_count_GPU;
    for (int i = 0; i < test_iteration; ++i)
    {
        // flush the output buffer
        flushBuffer(outputBResult, len);
        flushBuffer(outputCResult, len);
        double one_round = 1e30;
        repeat_count_GPU = find_repeats_parallel(input, N, outputBResult, outputCResult, one_round);
        printf("*time of parallel run %d:\t\t[%.3f] million cycles\n", i + 1, one_round);
        minParallelFind = min(minParallelFind, one_round);
    }
    printf("[best of find_parallel]:\t\t[%.3f] million cycles\n", minParallelFind);
    printf("\t\t\t\t\t(%.2fx speedup on GPU)\n", minSerialFind / minParallelFind);

    // now check the result
    printf("Now check the correctness...");
    bool correctB = checkCorrect(outputBAnswer, outputBResult, repeat_count);
    bool correctC = checkCorrect(outputCAnswer, outputCResult, N - repeat_count);
    if (repeat_count_GPU == repeat_count && correctB && correctC)
        printf("\t\tOutput correct!\n");
    else
        printf("\t\tOutput incorrect!\n");

    // write out the result if use sample file
    if (argc >= 1)
    {
        outputFile << "number of entries in array B:\n";
        outputFile << repeat_count << "\n\n";

        outputFile << "array B for find_repeats:\n";
        for (int i = 0; i < repeat_count; ++i)
        {
            outputFile << outputBAnswer[i] << " ";
        }
        outputFile << "\n\n";

        outputFile << "array C for find_repeats:\n";
        for (int i = 0; i < N - repeat_count; ++i)
        {
            outputFile << outputCAnswer[i] << " ";
        }
        outputFile << "\n\n";
    }

    printf("\n-------------------- EXCLUSIVE SCAN SECTION -------------------\n\n");

    printf("First run the [serial exclusive scan]...\n");
    int unique_count = N - repeat_count;
    len = roundPowerTwo(unique_count);
    double minSerialScan = 1e30;
    for (int i = 0; i < test_iteration; ++i)
    {
        // flush the output buffer
        flushBuffer(outputBAnswer, len);
        // start to record time consumption
        reset_and_start_timer();
        exclusive_scan_serial(outputCAnswer, len, outputBAnswer);
        // stop timer and print out total cycles
        double one_round = get_elapsed_mcycles();
        printf("*time of serial run %d:\t\t\t[%.3f] million cycles\n", i + 1, one_round);
        minSerialScan = min(minSerialScan, one_round);
    }
    printf("[best of scan_serial]:\t\t\t[%.3f] million cycles\n", minSerialScan);

    /* ------------------------------------------------------------- */

    printf("\nNow run the [parallel exclusive scan]...\n");
    double minParallelScan = 1e30;
    for (int i = 0; i < test_iteration; ++i)
    {
        // flush the output buffer
        flushBuffer(outputBResult, len);
        double one_round = 1e30;
        exclusive_scan_parallel(outputCAnswer, unique_count, outputBResult, one_round);
        printf("*time of parallel run %d:\t\t[%.3f] million cycles\n", i + 1, one_round);
        minParallelScan = min(minParallelScan, one_round);
    }
    printf("[best of scan_parallel]:\t\t[%.3f] million cycles\n", minParallelScan);
    printf("\t\t\t\t\t(%.2fx speedup on GPU)\n", minSerialScan / minParallelScan);

    // now check the result
    printf("Now check the correctness...");
    if (checkCorrect(outputBAnswer, outputBResult, unique_count))
        printf("\t\tOutput correct!\n");
    else
        printf("\t\tOutput incorrect!\n");

    printf("\n-------------------------- TEST END --------------------------\n\n");

    // write out the result if use sample file
    if (argc >= 1)
    {
        outputFile << "exclusive_scan of array C:\n";
        for (int i = 0; i < unique_count; ++i)
        {
            outputFile << outputBAnswer[i] << " ";
        }
        outputFile << "\n";
        outputFile.close();
    }

    delete[] input;
    delete[] outputBAnswer;
    delete[] outputCAnswer;
    delete[] outputBResult;
    delete[] outputCResult;
}
