#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H
#include <mpi.h> /* MPI header file */
#include "GameWorker.h"

class GameOfLife {
public:
    GameOfLife(int row, int col);
    ~GameOfLife();
	void initBoard();
	void gridAssign();
	void initWorker();
	void iterateOnce();
	bool isMaster();
	void print();

private:
	MPI_Status status;
	void initRandomBoard();
	void initSpecificBoard();
	int getCellStatus(int row, int col);
	enum LIFE {
		DEAD,
		ALIVE 
	};
	int** game_board;
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
	int rank;
};

#endif /* GAMEOFLIFE */