#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H
#include <mpi.h> /* MPI header file */
#include "GameWorker.h"

class GameOfLife {
public:
    GameOfLife(int row, int col, int num_iterate);
    ~GameOfLife();
	void   initBoard();
	void   gridAssign();
	void   initWorker();
	bool   isMaster();
	void   print();
	void   start();
	bool   notTooLarge();
	double getCommCost();
private:
	void iterateOnce();
	void initRandomBoard();
	void initSpecificBoard();
	int  getCellStatus(int row, int col);

	MPI_Status  status;
	GameWorker* worker;
	enum LIFE {
		DEAD,
		ALIVE 
	};

	int** game_board;
	int   num_iterate;
	int   cur_iteration;
	int   row_size;
	int   col_size;
	int   num_process;
	int   num_node_in_row;
	int   num_node_in_col;
	int   row_per_node;
	int   col_per_node;
	int   extra_last_row;
	int   extra_last_col;
	int   rank;
	bool  random;
};

#endif /* GAMEOFLIFE */