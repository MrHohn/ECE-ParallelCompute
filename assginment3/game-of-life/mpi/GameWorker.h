#ifndef GAMEWORKER_H
#define GAMEWORKER_H

#include <mpi.h> /* MPI header file */

class GameWorker {
public:
	GameWorker(
		int row_size, 
		int col_size, 
		int row_id, 
		int col_id, 
		int total_row, 
		int total_col, 
		int row_size_normal, 
		int col_size_normal, 
		int extra_last_row, 
		int extra_last_col,
		int num_node_in_row,
		int num_node_in_col,
		int num_iterate);
	~GameWorker();
	void   initBoardLocal(int** whole_board);
	void   initBoardMPI();
	void   iterateOnce();
	double getCommCost();
	double getLastCost();

private:
	void print();
	void sendSubBoard();
	void recvSubBoard();
	void sendToNeighbours();
	void recvFromNeighbours();
	bool checkAlive(int row, int col);
	bool checkExist(int row, int col);
	bool checkBlockExist(int rid, int cid);
	int  countNeighbours(int row, int col);
	int  getRank(int temp_row_id, int temp_col_id);
	void copyBoard(
		int** copyTo, 
		int** copyFrom, 
		int start_row, 
		int end_row, 
		int start_col, 
		int end_col);

	MPI_Status status;
	enum LIFE {
		DEAD,
		ALIVE 
	};

	int  num_iterate;
	int  cur_iteration;
	int  num_node_in_row;
	int  num_node_in_col;
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
	int**whole_board;
	int  num_process;
	bool updated;
    double comm_cost;
    double last_recv_cost;
};

#endif /* GAMEWORKER */