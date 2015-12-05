#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H
#include "GameWorker.h"

class GameOfLife {
public:
    GameOfLife(int row, int col, int num_pro);
    ~GameOfLife();
	int randomInit();
	void specificInit();
	void GridAssign();

private:
	int** game_board;
	bool master;
	GameWorker* worker;
	int row_size;
	int col_size;
	int num_process;
	int num_node_in_row;
	int num_node_in_col;
	int row_per_node;
	int col_per_node;
	int extra_last_row;
	int extra_last_col;
};

#endif /* GAMEOFLIFE */