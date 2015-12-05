#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "GameOfLife.h"

#define DEBUG 1

using namespace std;

GameOfLife::GameOfLife(int row, int col, int num_pro) {
	row_size = row;
	col_size = col;
	num_process = num_pro;
	master = false;
}

GameOfLife::~GameOfLife() {
	// delete the game board if it is master
	if (master) {
		for (int i = 0; i < row_size; ++i)
	        delete game_board[i];
	    delete[] game_board;
	}
	delete worker;
}

int GameOfLife::initRandomBoard() {
	// create game board
	master = true;
	game_board = new int*[row_size];
    for(int i = 0; i < row_size; ++i)
        game_board[i] = new int[col_size];

	// init game board
	srand(time(0));	
	int alive = 0;
	for (int i = 0; i < row_size; i++) {
		for (int j = 0; j < col_size; j++) {
			game_board[i][j] = rand() % 2;
			alive += game_board[i][j];
		}
	}
	return alive;
}

void GameOfLife::gridAssign() {
	if (DEBUG) printf("--- Grid Assignment ---\n");

	vector<int> factors;

	// now get all factors (> 2) of the #_processes
	int cur_pro_num = num_process;
	for (int i = 2; i <= cur_pro_num; ++i) {
		// if dividable
		while (cur_pro_num % i == 0) {
			factors.push_back(i);
			cur_pro_num /= i;
		}
	}

	// start calculating the assignment
	num_node_in_row = num_node_in_col = 1;
	row_per_node = row_size; // horizontal
	col_per_node = col_size; // vertical

	for (vector<int>::iterator i = factors.begin(); i < factors.end(); ++i) {
		if (row_per_node >= col_per_node) {
			row_per_node /= *i;
			num_node_in_row *= *i;
		}
		else {
			col_per_node /= *i;
			num_node_in_col *= *i;
		}
	}

	// get the remain num of works
	extra_last_row = row_size - num_node_in_row * row_per_node;
	extra_last_col = col_size - num_node_in_col * col_per_node;

	if (DEBUG) printf(" num_node_in_row: %d\n num_node_in_col: %d\n row_per_node: %d\n col_per_node: %d\n extra_last_row: %d\n extra_last_col: %d\n\n", num_node_in_row, num_node_in_col, row_per_node, col_per_node, extra_last_row, extra_last_col); 
}

void GameOfLife::initWorker(int rank) {
	if (DEBUG) printf("--- Init Worker ---\n");

	int row_id, col_id;
	int worker_row_size = row_per_node;
	int worker_col_size = col_per_node;

	// id both start from 1
	row_id = rank / num_node_in_col + 1;
	col_id = rank - (row_id - 1) * num_node_in_col + 1;
	if (DEBUG) printf(" row_id: %d\n col_id: %d\n", row_id, col_id);

	// if it is the last node in a row or column
	// allocate extra work
	if (row_id == num_node_in_row) {
		worker_row_size += extra_last_row;
	}
	if (col_id == num_node_in_col) {
		worker_col_size += extra_last_col;
	}
	if (DEBUG) printf(" worker_row_size: %d\n worker_col_size: %d\n\n", worker_row_size, worker_col_size);

	// create worker
	worker = new GameWorker(worker_row_size, worker_col_size, row_id, col_id, row_size, col_size, row_per_node, col_per_node);

	// init the game board
	if (master) {
		worker->initBoardLocal(game_board);
	}
	else {
		worker->initBoardMPI();
	}
}
