#ifndef GAMEWORKER_H
#define GAMEWORKER_H

class GameWorker {
public:
	GameWorker(int row_size, int col_size, int row_id, int col_id, int total_row, int total_col, int row_size_normal, int col_size_normal);
    ~GameWorker();
	void initBoardLocal(int** whole_board);
	void initBoardMPI();
	void iterateOnce();

private:
	enum LIFE {
		DEAD,
		ALIVE 
	};
	bool checkExist(int row, int col);
	void copyBoard(int** copyTo, int** copyFrom, int row_copy, int col_copy);
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
	int**game_board;
	int**copy_board;
	bool updated;
	int cur_iteration;
};

#endif /* GAMEWORKER */