#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// function to generate random integer number between rMin and rMax
int randNum(int rMin, int rMax)
{
    // use the built-in rand() to generate random int number
    return rMin + (rand() % (int)(rMax - rMin + 1));
}

void createRand(int* input, int N, int rMin, int rMax)
{
	srand(time(NULL));
    for (int i = 0; i < N; ++i)
    {
        *input = randNum(rMin, rMax);
        ++input;
    }
}

int roundPowerTwo(int len)
{
	int rounded = 1;
	while (rounded < len)
	{
		rounded *= 2;
	}
	return rounded;
}

bool checkCorrect(int* answer, int* result, int len)
{
	return true;
}
