#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

class GameOfLife {
public:
    GameOfLife(int row, int col, int** board);
    ~GameOfLife();
	int randomInit();
	void specificInit();
	void iterateOnce();
	void iterateAll(int iteration);
	void print();
	int getNumAlive();
	bool checkAlive(int row, int col);
	int countNeighbours(int row, int col);
	void copyBoard(int** copyTo, int** copyFrom);

private:
	enum LIFE {
		DEAD,
		ALIVE 
	};
	int row_size;
	int col_size;
	int** game_board;
	int** copy_board;
	int cur_iteration;
	int cur_alive;
};

#endif /* GAMEOFLIFE */