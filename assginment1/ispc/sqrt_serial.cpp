#include <stdio.h>

double absDouble(double num)
{
	if (num < 0)
		return -num;
	else
		return num; 
}

void sqrt_serial(int N, double* nums, double* result)
{
	// start looping and calculating all the sqrt result
	for (int i = 0; i < N; ++i)
	{
		// if input num is 0, return 0
		if (nums[i] == 0)
		{
			result[i] = 0;
			continue;
		}

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
		while (1)
		{
			// apply Newton's method to calculate the sqrt value
			numCurrent = (numLast + (nums[i] / numLast)) / 2;
			// judge if the accuracy is enough, 10^-4 = 0.0001
			if (absDouble(numCurrent - numLast) <= 0.0001)
			{
				result[i] = numCurrent;
			    // printf("result is: %f\n", result[i]);
				break;
			}
			// put the current value as last value to prepare for next loop
			numLast = numCurrent;
		}
	}
}

