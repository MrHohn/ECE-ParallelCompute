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
		
		Suppose: 
		sqrt(S) = sqrt(a) * 10^n
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
		// judge if the accuracy is enough, 10^-4 = 0.0001
		if (absDouble(numCurrent - numLast) <= 0.0001)
			return numCurrent;
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

void main()
{
	// set up variables to calculate time consumption
	struct timeval tpstart, tpend;
	double timeuse;
	// record the start point
	gettimeofday(&tpstart, NULL);

	// loop for 20 millions time -> 20,000,000
	int i;
	for (i = 0; i < 20000000; ++i)
	{
	    double num = randNum(0, 3);
	    // printf("input number is: %f\n", num);    
	    double res = sqrtLocal(num);
	    // printf("result is: %f, loop count is: %d\n", res, count);
	}

	// record the end point
	gettimeofday(&tpend, NULL);
	// calculate the time consumption in us
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec; // notice, should include both s and us
	printf("Total time:%fms\n", timeuse / 1000);
}