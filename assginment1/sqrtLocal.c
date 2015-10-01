#include <stdio.h>
#include <stdlib.h>

// global counter to record how many generations we run
int count = 0;

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
		
		Suppose: sqrt(S) = sqrt(a) * 10^n
				 sqrt(S) = { 2 * 10^n, if a < 10
							 6 * 10^n, if a >= 10 }

		Since for this assignment, we always calculate the sqrt of random
		number between 0 ~ 3, hense a < 10, and n = 0
		Hence the initial value would be set as 2 * 10^0 = 2
	*/

	// setup the record for last generation and current generation value
	double numLast = 2, numCurrent;
	while (1)
	{
		// record the loop count
		++count;
		// apply Newton's method to calculate the sqrt value
		numCurrent = (numLast + (num / numLast)) / 2;
		if (absDouble(numCurrent - numLast) <= 0.01)
			break;
		// put the current value as last value to prepare for next loop
		numLast = numCurrent;
	}

	return numCurrent;
}

void main()
{
    double x = 0.5;
    double res = sqrtLocal(x);
    printf("result is: %f, loop count is: %d\n", res, count);
}