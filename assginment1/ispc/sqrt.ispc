
static inline float abs_ispc(float num)
{
    if (num < 0)
        return -num;
    else
        return num; 
}

export void sqrt_ispc(uniform int N, uniform float nums[], uniform float result[])
{
    // Setup the accuracy, 10^-4 = 0.0001
    uniform float base = 2;
    uniform float accuracy = 0.0001;

    foreach (index = 0 ... N)
    {
        // Load the appropriate input value for this program instance.
        float numSingle = nums[index], res = 0;

        // if input num is 0, return 0
        if (numSingle != 0)
        {
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
            float numLast = 2, numCurrent;
            while (1)
            {
                // apply Newton's method to calculate the sqrt value
                numCurrent = (numLast + (numSingle / numLast)) / base;
                // judge if the accuracy is enough, 10^-4 = 0.0001
                if (abs_ispc(numCurrent - numLast) <= accuracy)
                {
                    res = numCurrent;
                    break;
                }
                // put the current value as last value to prepare for next loop
                numLast = numCurrent;
            }
        }

        // Write the result to the output array.
        result[index] = res;
        // printf("result is: %f\n", result[i]);
    }

}
