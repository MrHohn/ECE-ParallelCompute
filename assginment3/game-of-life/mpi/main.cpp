#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "GameOfLife.h"
#include "timing.h"
#include "debug_config.h"

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Please input M, N and K\n");
        return 1;
    }
    int row = atoi(argv[1]);
    int col = atoi(argv[2]);
    int num_iterate = atoi(argv[3]);

    GameOfLife* game = new GameOfLife(row, col, num_iterate);
    // init the game board if this is master process
    if (game->isMaster())
        game->initBoard();
    game->gridAssign();
    game->initWorker();

    // if the game board is not too large, print the result
    if (game->isMaster() && game->notTooLarge()) {
        game->print();
    }

    // start to record time consumption
    if (game->isMaster())
        reset_and_start_timer();

    game->start();

    // stop timer and print out total cycles
    if (game->isMaster()) {
        double one_round = get_elapsed_mcycles();
        printf("total time consumption:\t\t\t[%.3f] million cycles\n", one_round);
        double comm_cost = game->getCommCost();
        printf("communication time consumption:\t\t[%.3f] million cycles\n", comm_cost);
        printf("communication time ratio:\t\t%.3f %%\n\n", comm_cost / one_round * 100);
    }

    usleep(1000 * 400);

    // if the game board is not too large, print the result
    if (game->isMaster() && game->notTooLarge()) {
        game->print();
    }
    
    delete game;

    return 0;
}