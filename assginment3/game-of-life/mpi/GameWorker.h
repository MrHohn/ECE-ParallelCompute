#ifndef GAMEWORKER_H
#define GAMEWORKER_H

#include <mpi.h> /* MPI header file */

class GameWorker {
public:
	GameWorker(int row_size, 
			   int col_size, 
			   int row_id, 
			   int col_id, 
			   int total_row, 
			   int total_col, 
			   int row_size_normal, 
			   int col_size_normal, 
			   int extra_last_row, 
			   int extra_last_col);
    ~GameWorker();
	void initBoardLocal(int** whole_board);
	void initBoardMPI();
	void iterateOnce();

private:
	MPI_Status status;
	enum LIFE {
		DEAD,
		ALIVE 
	};
	bool checkExist(int row, int col);
	void copyBoard(int** copyTo, int** copyFrom, int start_row, int end_row, int start_col, int end_col);
	int  countNeighbours(int row, int col);
	bool checkAlive(int row, int col);
	int  row_size;
	int  col_size;
	int  row_size_normal;
	int  col_size_normal;
	int  total_row;
	int  total_col;
	int  row_id;
	int  col_id;
	int  extra_last_row;
	int  extra_last_col;
	int**game_board;
	int**copy_board;
	bool updated;
	int cur_iteration;
};

#endif /* GAMEWORKER */