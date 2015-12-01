#include <stdlib.h>
#include <stdio.h>

#include "GameOfLife.h"
#include "timing.h"

int main(int argc, char ** argv) {
    // start to record time consumption
    reset_and_start_timer();

    // create a container for the game board
    int row = 6;
    int col = 4;
    int** board = new int*[row];
	for(int i = 0; i < row; ++i)
		board[i] = new int[col];

    GameOfLife game(row, col, board);
    game.randomInit();
    // game.specificInit();
    game.print();
    game.iterateAll(5);

   // stop timer and print out total cycles
    double one_round = get_elapsed_mcycles();
    printf("time of serial run:\t\t\t[%.3f] million cycles\n", one_round);

    delete[] board;
    return 0;
}
