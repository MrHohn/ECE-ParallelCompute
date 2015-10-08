#include <math.h>

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
		// if input num is 0, output 0
		if (nums[i] == 0)
		{
			result[i] = 0;
			continue;
		}

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
