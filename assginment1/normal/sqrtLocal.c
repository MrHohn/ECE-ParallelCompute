#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

// global counter to record how many generations we run
int count = 0;
int maxGeneration = 0;

double absDouble(double num)
{
	if (num < 0)
		return -num;
	else
		return num; 
}

double sqrtLocal(double num)
{
	// if input num is 0, return 0
	if (num == 0)
		return 0;

	/*
		Setup the initial estimation, refer wikipedia's Rough estimation:
		https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
		
		Suppose: 
		sqrt(S) = sqrt(a) * 10^n
		sqrt(S) = { 2 * 10^n, if a < 10
		            6 * 10^n, if a >= 10 }

		Since for this assignment, we always calculate the sqrt of random
		number between 0 ~ 3, thus a < 10, and n = 0
		Hence the initial value would be set as 2 * 10^0 = 2
	*/

	// setup the record for last generation and current generation value
	double numLast = 2, numCurrent;
	int curGeneration = 0;
	while (1)
	{
		// record the loop count
		++count;
		++curGeneration;
		// apply Newton's method to calculate the sqrt value
		numCurrent = (numLast + (num / numLast)) / 2;
		// judge if the accuracy is enough, 10^-4 = 0.0001
		if (absDouble(numCurrent - numLast) <= 0.0001)
		{
			if (curGeneration > maxGeneration)
			{
				// record the max generation
				maxGeneration = curGeneration;
			}
			return numCurrent;
		}
		// put the current value as last value to prepare for next loop
		numLast = numCurrent;
	}
}

// function to generate random double number between rMin and rMax
double randNum(double rMin, double rMax)
{
	// use the built-in rand() to generate random int number
	// convert it into fraction of 1
	double ranFraction = (double)rand() / RAND_MAX;
	// now convert the result to between range[rMin, rMax]
	return rMin + ranFraction * (rMax - rMin);
}

void sqrtAll(int N, double* nums, double* result)
{
	// start looping and calculating all the sqrt result
	int i;
	for (i = 0; i < N; ++i)
	{
	    result[i] = sqrtLocal(nums[i]);
	    // printf("result is: %f, loop count is: %d\n", result[i], count);
	}
}

int main()
{
	// generate all the random numbers first
	// loop for 20 millions time -> 20,000,000
	int totalNum = 20000000;
	// allocate memory space for the inputs and results in heap
	double* nums = malloc(totalNum * sizeof(double));
	double* result = malloc(totalNum * sizeof(double));
	int i;
	// initiallize the random seed by current time to avoid duplicate
	srand(time(NULL));
	// generate random numbers
	for (i = 0; i < totalNum; ++i)
	{
	    nums[i] = randNum(0, 3);
	    // printf("input number is: %f\n", num[i]);
	}

	// set up variables to calculate time consumption
	struct timeval tpstart, tpend;
	double timeuse;
	// record the start point
	gettimeofday(&tpstart, NULL);

	// call the sqrtAll function to calculate all the inputs
	sqrtAll(totalNum, nums, result);

	// record the end point
	gettimeofday(&tpend, NULL);
	// calculate the time consumption in us
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec; // notice, should include both s and us
	printf("Total time:%fms\n", timeuse / 1000);

	// print the inputs and outputs
	// for (i = 0; i < totalNum; ++i) {
	// 	printf("input: %f, output: %f\n", nums[i], result[i]);
	// }

	// print the total generations and max generation
	printf("total generations: %d\n", count);
	printf("single max generations: %d\n", maxGeneration);

	// now free the memory
	free(nums);
	free(result);

	return 0;
}
