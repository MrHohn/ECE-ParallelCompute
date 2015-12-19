#include <stdlib.h>
#include <stdio.h>

#include "GameOfLife.h"
#include "debug_config.h"
#include "timing.h"

int main(int argc, char ** argv) {
    if (argc < 4) {
        printf("Please input M, N and K\n");
        return 1;
    }
    int row = atoi(argv[1]);
    int col = atoi(argv[2]);
    int num_iterate = atoi(argv[3]);

    GameOfLife* game;
    if (row <= 0 || col <= 0) {
        game = new GameOfLife(6, 4);
        game->specificInit();
    }
    else {
        game = new GameOfLife(row, col);
        game->randomInit();
    }
    if (game->notTooLarge()) {
        game->print();
    }

    // start to record time consumption
    reset_and_start_timer();
    
    game->iterateAll(num_iterate);

   // stop timer and print out total cycles
    double one_round = get_elapsed_mcycles();
    if (game->notTooLarge()) {
        game->print();
    }
    printf("\n-------- Statistic Infomation --------\n\n");
    printf("time consumption:\t\t\t[%.3f] million cycles\n", one_round);

    delete(game);
    return 0;
}
