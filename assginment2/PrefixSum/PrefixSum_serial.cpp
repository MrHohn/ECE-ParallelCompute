#include <cstring>

// this method is cited from CMU's assignment description
// http://15418.courses.cs.cmu.edu/spring2015/article/4
void exclusive_scan_serial(int* nums, int len, int* output)
{
    /* parallel version */
    // memmove(output, nums, len * sizeof(int));
    // // upsweep phase.
    // // total work O(n) -> n/2 + n/4 + n/8 + ... + 2
    // for (int starter = 1; starter < len; starter *= 2)
    // {
    //     int interval = starter * 2;
    //     for (int i = 0; i < len; i += interval)
    //     {
    //         output[i + interval - 1] += output[i + starter - 1];
    //     }
    // }

    // output[len - 1] = 0;

    // // downsweep phase.
    // // total work O(n) -> 1 + 2 + 4 + ... + n/2
    // for (int starter = len / 2; starter >= 1; starter /= 2)
    // {
    //     int interval = starter * 2;
    //     for (int i = 0; i < len; i += interval)
    //     {
    //         int temp = output[i + starter - 1];
    //         output[i + starter - 1] = output[i + interval - 1];
    //         output[i + interval - 1] += temp; // change interval to starter to reverse prefix sum.
    //     }
    // }

    /* serial version */
    output[0] = 0;
    for (int i = 1; i < len; ++i)
    {
        output[i] = output[i - 1] + nums[i - 1];
    }
}

int find_repeats_serial(int* nums, int len, int* outputB, int* outputC)
{
    int repeat_count = 0;
    int unique_count = 0;

    for (int i = 0; i < len - 1; ++i)
    {
        if (nums[i] == nums[i + 1])
        {
            outputB[repeat_count] = i;
            ++repeat_count;
        }
        else
        {
            outputC[unique_count] = nums[i];
            ++unique_count;
        }
    }

    return repeat_count;
}
