#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

#include "timing.h"
#include "util.h"

__global__ void upSweep(int* device_result, int length, int twod, int twod1) {
    int index = (blockIdx.x * blockDim.x + threadIdx.x) * twod1;
    if ((index + twod1 - 1) >= length) {  // check boundary
        return;
    }
    device_result[index + twod1 - 1] += device_result[index + twod - 1];
}

__global__ void downSweep(int* device_result, int length, int twod, int twod1) {
    int index = (blockIdx.x * blockDim.x + threadIdx.x) * twod1;
    if ((index + twod1 - 1) >= length) {  // check boudnary
        return;
    }
    int tmp = device_result[index + twod - 1];
    device_result[index + twod - 1] = device_result[index + twod1 - 1];
    device_result[index + twod1 - 1] += tmp;
}

double exclusive_scan_parallel(int* nums, int len, int* output)
{
    int* device_result;
    int* device_input;

    int roundedLen = roundPowerTwo(len);

    // allocate space on GPU and copy inputs into it
    cudaMalloc((void **)&device_result, roundedLen * sizeof(int));
    cudaMalloc((void **)&device_input, roundedLen * sizeof(int));
    cudaMemcpy(device_input, nums, len * sizeof(int), 
               cudaMemcpyHostToDevice);
    cudaMemcpy(device_result, nums, len * sizeof(int), 
               cudaMemcpyHostToDevice);

    // double startTime = CycleTimer::currentSeconds();
    // start to do GPU computing

    int threadsPerBlock = 256;
    
    // upsweep phase
    for (int twod = 1; twod < roundedLen; twod *= 2) {
        int twod1 = twod * 2;
        int blocksPerGrid = (roundedLen/twod1 + threadsPerBlock - 1) / threadsPerBlock;
        upSweep<<<blocksPerGrid, threadsPerBlock>>>(device_result, roundedLen, twod, twod1);
    }

    // set last element to zero
    int zero = 0;
    cudaMemcpy(&device_result[roundedLen - 1], &zero, sizeof(int), cudaMemcpyHostToDevice);

    // downsweep phase
    for (int twod = roundedLen / 2; twod >= 1; twod /= 2) {
        int twod1 = twod * 2;
        int blocksPerGrid = (roundedLen/twod1 + threadsPerBlock - 1) / threadsPerBlock;
        downSweep<<<blocksPerGrid, threadsPerBlock>>>(device_result, roundedLen, twod, twod1);
    }

    // Wait for any work left over to be completed.
    cudaThreadSynchronize();

    // finished GPU computing
    // double endTime = CycleTimer::currentSeconds();
    // double overallDuration = endTime - startTime;
    
    // copy back the result
    cudaMemcpy(output, device_result, len * sizeof(int),
               cudaMemcpyDeviceToHost);
    // free device memory
    cudaFree(device_result);
    cudaFree(device_input);

    return 0;
}

void printCudaInfo()
{
    // for fun, just print out some stats on the machine

    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);

    printf("---------------------------------------------------------\n");
    printf("Found %d CUDA devices\n", deviceCount);

    for (int i = 0; i < deviceCount; i++)
    {
        cudaDeviceProp deviceProps;
        cudaGetDeviceProperties(&deviceProps, i);
        printf("Device %d: %s\n", i, deviceProps.name);
        printf("   SMs:        %d\n", deviceProps.multiProcessorCount);
        printf("   Global mem: %.0f MB\n",
               static_cast<float>(deviceProps.totalGlobalMem) / (1024 * 1024));
        printf("   CUDA Cap:   %d.%d\n", deviceProps.major, deviceProps.minor);
    }
    printf("---------------------------------------------------------\n"); 
}
