#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
// Include the header file that the ispc compiler generates
#include "sqrt_ispc.h"
using namespace ispc;

extern void sqrt_serial(int N, double* nums, double* result);

// function to generate random double number between rMin and rMax
double randNum(double rMin, double rMax)
{
	// use the built-in rand() to generate random int number
	// convert it into fraction of 1
	double ranFraction = (double)rand() / RAND_MAX;
	// now convert the result to between range[rMin, rMax]
	return rMin + ranFraction * (rMax - rMin);
}

int main()
{
	// generate all the random numbers first
	// loop for 20 millions time -> 20,000,000
	int totalNum = 20000000;
	// allocate memory space for the inputs and results in heap
	double* nums = (double*) malloc(totalNum * sizeof(double));
	double* result = (double*) malloc(totalNum * sizeof(double));
	// initiallize the random seed by current time to avoid duplicate
	srand(time(NULL));
	// generate random numbers
	for (int i = 0; i < totalNum; ++i)
	{
	    nums[i] = randNum(0, 3);
	    // printf("input number is: %f\n", num[i]);
	}

	// set up variables to calculate time consumption
	struct timeval tpstart, tpend;
	double timeuse;
	
	printf("Run the serial version first...\n");
	// record the start point
	gettimeofday(&tpstart, NULL);

	// call the sqrt_serial function to calculate all the inputs
	sqrt_serial(totalNum, nums, result);

	// record the end point
	gettimeofday(&tpend, NULL);
	// calculate the time consumption in us
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec; // notice, should include both s and us
	printf("Serial version time consumption:%fms\n", timeuse / 1000);

	// print the inputs and outputs
	// for (int i = 0; i < totalNum; ++i) {
	// 	printf("input: %f, output: %f\n", nums[i], result[i]);
	// }

	printf("Now run the ISPC version...\n");
	// record the start point
	gettimeofday(&tpstart, NULL);

	// call the sqrtAll function to calculate all the inputs
	sqrt_ispc(totalNum, nums, result);

	// record the end point
	gettimeofday(&tpend, NULL);
	// calculate the time consumption in us
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec; // notice, should include both s and us
	printf("ISPC version time consumption:%fms\n", timeuse / 1000);

	// print the inputs and outputs
	// for (int i = 0; i < totalNum; ++i) {
	// 	printf("input: %f, output: %f\n", nums[i], result[i]);
	// }

	// now free the memory
	free(nums);
	free(result);

	return 0;
}
