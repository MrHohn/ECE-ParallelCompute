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

export void sqrt_ispc(uniform int N, uniform float nums[], uniform float result[])
{
    foreach (index = 0 ... N)
    {
        // Load the appropriate input value for this program instance.
        float numInput = nums[index];

        // setup the record for previous generation and current generation value
        float numPrev = guess, numCurrent, diff = 1.f;
        // judge if the accuracy is enough, 10^-4
        while (diff > accuracy)
        {
            // apply Newton's method to calculate the sqrt value
            // https://en.wikipedia.org/wiki/Newton%27s_method#Square_root_of_a_number
            numCurrent = (numPrev + numInput / numPrev) * 0.5f;
            diff = abs(numCurrent - numPrev);
            // put the current value as previous value to prepare for next loop
            numPrev = numCurrent;
        }

        // Write the result to the output array.
        result[index] = numCurrent;
    }
}

// task for sqrt_ispc_task
task void sqrt_task(uniform int N, uniform float nums[], uniform float result[], uniform int span)
{
    uniform int istart = taskIndex * span;
    // choose the smaller in case N could not be divided by # of tasks
    uniform int iend = min((taskIndex + 1) * span, (unsigned int) N);

    foreach (index = istart ... iend)
    {
        // Load the appropriate input value for this program instance.
        float numInput = nums[index];

        // setup the record for previous generation and current generation value
        float numPrev = guess, numCurrent, diff = 1.f;
        // judge if the accuracy is enough, 10^-4
        while (diff > accuracy)
        {
            // apply Newton's method to calculate the sqrt value
            // https://en.wikipedia.org/wiki/Newton%27s_method#Square_root_of_a_number
            numCurrent = (numPrev + numInput / numPrev) * 0.5f;
            diff = abs(numCurrent - numPrev);
            // put the current value as previous value to prepare for next loop
            numPrev = numCurrent;
        }

        // Write the result to the output array.
        result[index] = numCurrent;
    }
}

export void sqrt_ispc_task(uniform int N, uniform float nums[], uniform float result[])
{
    uniform int parallel = 4; // 4 tasks parallism
    uniform int span = N / parallel; // one span is quarter N

    launch[parallel] sqrt_task(N, nums, result, span);
}
