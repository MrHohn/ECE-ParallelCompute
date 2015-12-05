#include <stdlib.h>
#include <stdio.h>

#include "GameOfLife.h"
#include "timing.h"

#define DEBUG 0

int main(int argc, char ** argv) {

    // create a container for the game board
    int row = 2000;
    int col = 2000;

    GameOfLife game(row, col);
    game.randomInit();
    // game.specificInit();
    if (DEBUG) {
        game.print();
    }

    // start to record time consumption
    reset_and_start_timer();
    
    game.iterateAll(5);

   // stop timer and print out total cycles
    double one_round = get_elapsed_mcycles();
    printf("time consumption:\t\t\t[%.3f] million cycles\n", one_round);

    return 0;
}
