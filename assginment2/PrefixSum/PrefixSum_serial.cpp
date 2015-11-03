#include <cstring>

void exclusive_scan_serial(int* nums, int len, int* output)
{
    memmove(output, nums, len * sizeof(int));
    // upsweep phase.
    // total work O(n) -> n/2 + n/4 + n/8 + ... + 2
    for (int twod = 1; twod < len; twod *= 2)
    {
        int twod1 = twod * 2;
        for (int i = 0; i < len; i += twod1)
        {
            output[i + twod1 - 1] += output[i + twod - 1];
        }
    }

    output[len - 1] = 0;

    // downsweep phase.
    // total work O(n) -> 1 + 2 + 4 + ... + n/2
    for (int twod = len / 2; twod >= 1; twod /= 2)
    {
        int twod1 = twod * 2;
        for (int i = 0; i < len; i += twod1)
        {
            int t = output[i + twod - 1];
            output[i + twod - 1] = output[i + twod1 - 1];
            output[i + twod1 - 1] += t; // change twod1 to twod to reverse prefix sum.
        }
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
