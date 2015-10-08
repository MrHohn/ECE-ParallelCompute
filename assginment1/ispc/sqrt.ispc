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
// Setup the accuracy, 10^-4 = 0.0001
const float accuracy = 0.0001f;

export void sqrt_ispc(uniform int N, uniform float nums[], uniform float result[])
{
    foreach (index = 0 ... N)
    {
        // Load the appropriate input value for this program instance.
        float numInput = nums[index], res = 0.f;

        // if input num is 0, return 0
        if (numInput != 0.f)
        {
            // setup the record for last generation and current generation value
            float numLast = guess, numCurrent, diff = 1;
            // judge if the accuracy is enough, 10^-4 = 0.0001
            while (diff > accuracy)
            {
                // apply Newton's method to calculate the sqrt value
                // https://en.wikipedia.org/wiki/Newton%27s_method#Square_root_of_a_number
                numCurrent = (numLast + (numInput / numLast)) * 0.5f;
                diff = abs(numCurrent - numLast);
                // put the current value as last value to prepare for next loop
                numLast = numCurrent;
            }
            res = numCurrent;
        }

        // Write the result to the output array.
        result[index] = res;
    }
}
