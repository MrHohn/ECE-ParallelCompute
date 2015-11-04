#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

#include "timing.h"
#include "util.h"

// this method is cited from CMU's assignment description
// http://15418.courses.cs.cmu.edu/spring2015/article/4
__global__ void buildUpTree(int* device_result, int len, int starter, int interval)
{
    int index = (blockIdx.x * blockDim.x + threadIdx.x) * interval;
    // return if out of range
    if (index + interval > len)
    {  
        return;
    }
    device_result[index + interval - 1] += device_result[index + starter - 1];
}

__global__ void buildDownTree(int* device_result, int len, int starter, int interval)
{
    int index = (blockIdx.x * blockDim.x + threadIdx.x) * interval;
    // return if out of range
    if (index + interval > len)
    {  
        return;
    }
    int temp = device_result[index + starter - 1];
    device_result[index + starter - 1] = device_result[index + interval - 1];
    device_result[index + interval - 1] += temp;
}

void launch_scan(int roundedLen, int* device_result)
{
    int threadsPerBlock = 256;
    
    // build up tree
    for (int starter = 1; starter < roundedLen; starter *= 2)
    {
        int interval = starter * 2;
        // plus (threadsPerBlock - 1) is actually ceiling the block number
        int numBlocks = (roundedLen / interval + threadsPerBlock - 1) / threadsPerBlock;
        buildUpTree<<<numBlocks, threadsPerBlock>>>(device_result, roundedLen, starter, interval);
    }

    // set last element to zero
    int zero = 0;
    cudaMemcpy(&device_result[roundedLen - 1], &zero, sizeof(int), cudaMemcpyHostToDevice);

    // build down tree
    for (int starter = roundedLen / 2; starter >= 1; starter /= 2)
    {
        int interval = starter * 2;
        // plus (threadsPerBlock - 1) is actually ceiling the block number
        int numBlocks = (roundedLen / interval + threadsPerBlock - 1) / threadsPerBlock;
        buildDownTree<<<numBlocks, threadsPerBlock>>>(device_result, roundedLen, starter, interval);
    }
}


void exclusive_scan_parallel(int* nums, int len, int* output, double& time_cost)
{
    int* device_result;

    int roundedLen = roundPowerTwo(len);

    // Allocate space on GPU and copy inputs into it
    cudaMalloc((void **)&device_result, roundedLen * sizeof(int));
    cudaMemcpy(device_result, nums, len * sizeof(int), cudaMemcpyHostToDevice);

    // Start to do GPU computing
    reset_and_start_timer();

    // Since in-place algorithm are used, we did not allocate device_input
    launch_scan(roundedLen, device_result);

    // Wait for all instances to finished
    cudaThreadSynchronize();

    // Finished GPU computing
    time_cost = get_elapsed_mcycles();
    
    // Copy back the result
    cudaMemcpy(output, device_result, len * sizeof(int), cudaMemcpyDeviceToHost);
    // Free device memory
    cudaFree(device_result);
}
