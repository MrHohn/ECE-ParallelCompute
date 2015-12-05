#include <stdlib.h>
#include <stdio.h>

#include "GameOfLife.h"
#include "timing.h"

#define DEBUG 1

int main(int argc, char ** argv) {
	int row = 99;
	int col = 99;
	int num_process = 10;

    GameOfLife* game = new GameOfLife(row, col, num_process);
    game->gridAssign();
    game->initRandomBoard();
    game->initWorker(0);

    delete game;

	return 0;
}