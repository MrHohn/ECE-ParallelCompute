#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "GameWorker.h"
#include "debug_config.h"

using namespace std;

GameWorker::GameWorker(
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
	int num_iterate) {

	this->row_size = row_size;
	this->col_size = col_size;
	this->row_id = row_id;
	this->col_id = col_id;
	this->total_row = total_row;
	this->total_col = total_col;
	this->row_size_normal = row_size_normal;
	this->col_size_normal = col_size_normal;
	this->extra_last_row = extra_last_row;
	this->extra_last_col = extra_last_col;
	this->num_node_in_row = num_node_in_row;
	this->num_node_in_col = num_node_in_col;
	this->num_iterate = num_iterate;

	// create the game boards
	game_board = new int*[row_size + 2];
	copy_board = new int*[row_size + 2];
	for(int i = 0; i < row_size + 2; ++i) {
		game_board[i] = new int[col_size + 2];
		copy_board[i] = new int[col_size + 2];
	}

	updated = false;
	cur_iteration = 0;
}

GameWorker::~GameWorker() {
	for (int i = 0; i < row_size + 2; ++i) {
		delete game_board[i];
		delete copy_board[i];
	}
	delete[] game_board;
	delete[] copy_board;
}

void GameWorker::initBoardLocal(int** whole_board) {
	int start_row = (row_id - 1) * row_size_normal;
	int start_col = (col_id - 1) * col_size_normal;
	// if (DEBUG) printf(" start_row: %d\n start_col: %d\n\n", start_row, start_col);
	for (int i = -1; i < row_size + 1; ++i) {
		for (int j = -1; j < col_size + 1; ++j) {
			if (checkExist(start_row + i, start_col + j)) {
				game_board[i + 1][j + 1] = whole_board[start_row + i][start_col + i];
			}
		}
	}

	// copy the board
	copyBoard(copy_board, game_board, 0, row_size + 1, 0, col_size + 1);

	updated = true;
}

void GameWorker::initBoardMPI() {
	// call MPI to retrieve data from master
	int needed_len = (row_size_normal + extra_last_row + 2) * (col_size_normal + extra_last_col + 2) * 2;
	char buf[needed_len];
	// receive initial data from master
	MPI_Recv(buf, needed_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
	// if (DEBUG) printf("%d %d: %s\n", row_id, col_id, buf);
	for (int i = 0; i < row_size + 2; ++i) {
		for (int j = 0; j < col_size + 2; ++j) {
			char* tmp;
			if (i == 0 && j == 0) {
				tmp = strtok(buf, ",");
			}
			else {
				tmp = strtok(NULL, ",");
			}
			game_board[i][j] = atoi(tmp);
		}
	}

	updated = true;
}

// check if a specific cell exist in the original matrix
bool GameWorker::checkExist(int row, int col) {
	if (row < 0 || row >= total_row) {
		return false;
	}
	if (col < 0 || col >= total_col) {
		return false;
	}
	return true;
}

void GameWorker::copyBoard(
	int** copyTo, 
	int** copyFrom, 
	int   start_row, 
	int   end_row, 
	int   start_col, 
	int   end_col) {
	
	for (int i = start_row; i < end_row; ++i) {
		for (int j = start_col; j < end_col; ++j) {
			copyTo[i][j] = copyFrom[i][j];
		}
	}
}

void GameWorker::iterateOnce() {
	if (!updated) {
		// call MPI to get updated neighbours
		// send out the local copy first
		sendToNeighbours();
		// then retrieve remote copies
		recvFromNeighbours();
	}

	// now iterate
	for (int i = 1; i < row_size + 1; ++i) {
		for (int j = 1; j < col_size + 1; ++j) {
			int count = countNeighbours(i, j);
			if (count <= 1 || count >= 4) {
				copy_board[i][j] = DEAD;
			}
			else {
				copy_board[i][j] = ALIVE;
			}
		}
	}
	++cur_iteration;

	// copy back the board
	copyBoard(game_board, copy_board, 1, row_size - 1, 0, col_size - 1);

	updated = false;

	// if need to print the process, send intermediate results to master
	if (cur_iteration == num_iterate || PRINT_PROCESS) {
		if (row_id == 0 && col_id == 0) {
			recvSubBoard();
		}
		else {
			sendSubBoard();
		}
	}
}

void GameWorker::sendToNeighbours() {

}

void GameWorker::recvFromNeighbours() {

}

void GameWorker::sendSubBoard() {

}

void GameWorker::recvSubBoard() {

}

int GameWorker::countNeighbours(int row, int col) {
	int alive = 0;
	if (checkAlive(row - 1, col - 1)) ++alive;
	if (checkAlive(row - 1, col)) ++alive;
	if (checkAlive(row - 1, col + 1)) ++alive;
	if (checkAlive(row, col - 1)) ++alive;
	if (checkAlive(row, col + 1)) ++alive;
	if (checkAlive(row + 1, col - 1)) ++alive;
	if (checkAlive(row + 1, col)) ++alive;
	if (checkAlive(row + 1, col + 1)) ++alive;

	return alive;
}

bool GameWorker::checkAlive(int row, int col) {
	return game_board[row][col] == ALIVE;
}