#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
using namespace std;

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
const float guess = 2.f;
// Setup the accuracy, 10^-4
const float accuracy = 1e-4;

void sqrt_serial(int N, float* nums, float* result)
{
	// start looping and calculating all the sqrt result
	for (int i = 0; i < N; ++i)
	{
		// setup the record for previous generation and current generation value
		float numPrev = guess, numCurrent, diff = 1;
		// judge if the accuracy is enough, 10^-4 = 0.0001
		while (diff > accuracy)
		{
			// apply Newton's method to calculate the sqrt value
			// https://en.wikipedia.org/wiki/Newton%27s_method#Square_root_of_a_number
			numCurrent = (numPrev + nums[i] / numPrev) * 0.5f;
			diff = fabs(numCurrent - numPrev);
			// put the current value as previous value to prepare for next loop
			numPrev = numCurrent;
		}

		result[i] = numCurrent;		
	}
}

// this method is used to test the # of iterations of all inputs
// and we could calculate the theoretical value of speed up
void sqrt_test_iterations(int N, float* nums, float* result, int num_vector, int &total_serial, int &total_simd)
{
	// total # of serial iterations
	int num_serial = 0;
	// total # of simd iterations
	int num_simd = 0;
	// the temporary max iteration for current 8 inputs
	int temp_max = 0;
	// start looping and calculating all the sqrt result
	for (int i = 0; i < N; ++i)
	{
		// record the current iterations
		int cur_iteration = 0;
		// setup the record for previous generation and current generation value
		float numPrev = guess, numCurrent, diff = 1;
		// judge if the accuracy is enough, 10^-4 = 0.0001
		while (diff > accuracy)
		{
			// increase the serial interations and current iterations
			++num_serial;
			++cur_iteration;

			// apply Newton's method to calculate the sqrt value
			// https://en.wikipedia.org/wiki/Newton%27s_method#Square_root_of_a_number
			numCurrent = (numPrev + nums[i] / numPrev) * 0.5f;
			diff = fabs(numCurrent - numPrev);
			// put the current value as previous value to prepare for next loop
			numPrev = numCurrent;
		}

		result[i] = numCurrent;

		// below is for # of iterations calculation
		temp_max = max(temp_max, cur_iteration);
		if ((i + 1) % num_vector == 0)
		{
			num_simd += temp_max;
			temp_max = 0;
		}
	}

	total_serial = num_serial;
	total_simd = num_simd;
}
