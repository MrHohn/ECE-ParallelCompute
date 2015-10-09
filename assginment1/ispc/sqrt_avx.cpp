#include <stdio.h>
#include "immintrin.h"

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
const __m256 accuracy = _mm256_set1_ps(1e-4);
const __m256 pointFive = _mm256_set1_ps(0.5f); // set 0.5f for computation
const __m256 zero = _mm256_set1_ps(0.f); // set 0.f for computation
const __m256 full_bits = _mm256_set1_ps(-1.f); // full bits to test 0

void sqrt_avx(int N, float* nums, float* result)
{
	const int width = 8;

	for (int i = 0; i < N; i += width)
	{
		__m256 numInput = _mm256_load_ps(nums + i); // load input
		__m256 numPrev = _mm256_set1_ps(guess); // set the initial guess
		__m256 numCurrent;
		int flag = 0;

		while (!flag)
		{
			// numCurrent = (numPrev + numInput / numPrev) * 0.5f;
			numCurrent = _mm256_div_ps(numInput, numPrev);
			numCurrent = _mm256_add_ps(numPrev, numCurrent);
			numCurrent = _mm256_mul_ps(numCurrent, pointFive);
						
			// get the positive diff
			__m256 diff = _mm256_sub_ps(numCurrent, numPrev);
			__m256 diff_negative = _mm256_sub_ps(zero, diff);
			diff = _mm256_max_ps(diff, diff_negative);
			
			// if diff is greater than accuracy, set to 0x80000000
			// otherwise set to 0 (ordered, non_signal)
			// ps: although the document said when greater than
			// result would be set to 0xffffffff
			// but in reality it is 0x80000000
			__m256 comp = _mm256_cmp_ps(diff, accuracy, _CMP_GT_OQ);

			// calculate the and of these two inputs and test zero
			// if all zero, set flag to 1, means accuracy is enough
			flag = _mm256_testz_ps(comp, full_bits);

			// put current value as previous and continue loop
			numPrev = numCurrent;
		}

		// store the result
		_mm256_store_ps(result + i, numCurrent);
	}
}
