#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

#include "timing.h"
#include "util.h"

// this method is cited from CMU's assignment description
// http://15418.courses.cs.cmu.edu/spring2015/article/4
__global__ void buildUpTree(int* device_output, int len, int starter, int interval)
{
    int index = (blockIdx.x * blockDim.x + threadIdx.x) * interval;
    // return if out of range
    if (index + interval > len)
    {  
        return;
    }
    device_output[index + interval - 1] += device_output[index + starter - 1];
}

__global__ void buildDownTree(int* device_output, int len, int starter, int interval)
{
    int index = (blockIdx.x * blockDim.x + threadIdx.x) * interval;
    // return if out of range
    if (index + interval > len)
    {  
        return;
    }
    int temp = device_output[index + starter - 1];
    device_output[index + starter - 1] = device_output[index + interval - 1];
    device_output[index + interval - 1] += temp;
}

void launch_scan(int roundedLen, int* device_output, int threadsPerBlock)
{   
    // build up tree
    for (int starter = 1; starter < roundedLen; starter *= 2)
    {
        int interval = starter * 2;
        // plus (threadsPerBlock - 1) is actually ceiling the block number
        int numBlocks = (roundedLen / interval + threadsPerBlock - 1) / threadsPerBlock;
        buildUpTree<<<numBlocks, threadsPerBlock>>>(device_output, roundedLen, starter, interval);
    }

    // set last element to zero
    int zero = 0;
    cudaMemcpy(&device_output[roundedLen - 1], &zero, sizeof(int), cudaMemcpyHostToDevice);

    // build down tree
    for (int starter = roundedLen / 2; starter >= 1; starter /= 2)
    {
        int interval = starter * 2;
        // plus (threadsPerBlock - 1) is actually ceiling the block number
        int numBlocks = (roundedLen / interval + threadsPerBlock - 1) / threadsPerBlock;
        buildDownTree<<<numBlocks, threadsPerBlock>>>(device_output, roundedLen, starter, interval);
    }
}

void exclusive_scan_parallel(int* nums, int len, int* output, double& time_cost)
{
    int* device_output;

    int roundedLen = roundPowerTwo(len);

    // Allocate space on GPU and copy inputs into it
    cudaMalloc((void **)&device_output, roundedLen * sizeof(int));
    cudaMemcpy(device_output, nums, len * sizeof(int), cudaMemcpyHostToDevice);

    // Start to do GPU computing
    reset_and_start_timer();

    // Since in-place algorithm is used, we did not allocate device_input
    launch_scan(roundedLen, device_output, 256);

    // Wait for all instances to finished
    cudaThreadSynchronize();

    // Finished GPU computing
    time_cost = get_elapsed_mcycles();
    
    // Copy back the result
    cudaMemcpy(output, device_output, len * sizeof(int), cudaMemcpyDeviceToHost);
    // Free device memory
    cudaFree(device_output);
}

__global__ void setRepeat(int* device_input, int* device_outputBTemp, int* device_outputCTemp, int len)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    // return if out of range
    if (index + 1 >= len)
    {
        return;
    }

    // set repeated bits
    if (device_input[index] == device_input[index + 1])
    {
        device_outputBTemp[index] = 1;
        device_outputCTemp[index] = 0;
    }
    else
    {
        device_outputBTemp[index] = 0;
        device_outputCTemp[index] = 1;
    }
}

__global__ void getRepeat(int* nums, int* device_outputB, int* device_outputC, int* device_outputBTemp, int* device_outputCTemp, int len)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    // check last bit
    if (index + 1 == len)
    {
        int curC = device_outputCTemp[index];
        if (curC != device_outputCTemp[index - 1])
        {
            device_outputC[curC] = nums[index];
        }
        return;
    }
    // return if out of range
    if (index + 1 > len)
    {
        return;
    }

    int curB = device_outputBTemp[index];
    int curC = device_outputCTemp[index];
    if (curB < device_outputBTemp[index + 1])
    {
        device_outputB[curB] = index;
    }
    if (curC < device_outputCTemp[index + 1])
    {
        device_outputC[curC] = nums[index];
    }
}

int launch_find(int len, int roundedLen, int* device_input, int* device_outputB, int* device_outputC, int* device_outputBTemp, int* device_outputCTemp, int threadsPerBlock)
{
    // call setRepeat function first
    int numBlocks = (len + threadsPerBlock - 1) / threadsPerBlock;
    setRepeat<<<numBlocks, threadsPerBlock>>>(device_input, device_outputBTemp, device_outputCTemp, len);

    // call exclusive scan again to add up previous output
    launch_scan(roundedLen, device_outputBTemp, threadsPerBlock);
    launch_scan(roundedLen, device_outputCTemp, threadsPerBlock);

    // get number of repeats
    int repeat_count;
    cudaMemcpy(&repeat_count, &device_outputBTemp[len - 1], sizeof(int), cudaMemcpyDeviceToHost);

    // call getRepeat function now
    getRepeat<<<numBlocks, threadsPerBlock>>>(device_input, device_outputB, device_outputC, device_outputBTemp, device_outputCTemp, len);

    return repeat_count;
}

int find_repeats_parallel(int* nums, int len, int* outputB, int* outputC, double& time_cost)
{
    int* device_input;
    int* device_outputB;
    int* device_outputBTemp;
    int* device_outputC;
    int* device_outputCTemp;

    int roundedLen = roundPowerTwo(len);

    // Allocate space on GPU and copy inputs into it
    cudaMalloc((void **)&device_input, len * sizeof(int));
    cudaMalloc((void **)&device_outputB, len * sizeof(int));
    cudaMalloc((void **)&device_outputC, len * sizeof(int));
    cudaMalloc((void **)&device_outputBTemp, roundedLen * sizeof(int));
    cudaMalloc((void **)&device_outputCTemp, roundedLen * sizeof(int));
    cudaMemcpy(device_input, nums, len * sizeof(int), cudaMemcpyHostToDevice);
    
    // Start to do GPU computing
    reset_and_start_timer();

    int repeat_count = launch_find(len, roundedLen, device_input, device_outputB, device_outputC, device_outputBTemp, device_outputCTemp, 256);

    // Wait for all instances to finished
    cudaThreadSynchronize();

    // Finished GPU computing
    time_cost = get_elapsed_mcycles();
    
    // Copy back the result
    cudaMemcpy(outputB, device_outputB, repeat_count * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(outputC, device_outputC, (len - repeat_count) * sizeof(int), cudaMemcpyDeviceToHost);
    // Free device memory
    cudaFree(device_input);
    cudaFree(device_outputB);
    cudaFree(device_outputC);
    cudaFree(device_outputBTemp);
    cudaFree(device_outputCTemp);

    return repeat_count;
}
