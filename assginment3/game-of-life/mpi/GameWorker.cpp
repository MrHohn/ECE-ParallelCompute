#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "GameWorker.h"
#include "debug_config.h"

#define MASTER 0

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

	this->row_size =        row_size;
	this->col_size =        col_size;
	this->row_id =          row_id;
	this->col_id =          col_id;
	this->total_row =       total_row;
	this->total_col =       total_col;
	this->row_size_normal = row_size_normal;
	this->col_size_normal = col_size_normal;
	this->extra_last_row =  extra_last_row;
	this->extra_last_col =  extra_last_col;
	this->num_node_in_row = num_node_in_row;
	this->num_node_in_col = num_node_in_col;
	this->num_iterate =     num_iterate;

	// create the game boards
	game_board = new int*[row_size + 2];
	copy_board = new int*[row_size + 2];
	for(int i = 0; i < row_size + 2; ++i) {
		game_board[i] = new int[col_size + 2];
		copy_board[i] = new int[col_size + 2];
	}

	updated = false;
	cur_iteration = 0;
	num_process = 0;
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
	this->whole_board = whole_board;
	/* get number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &num_process);

	int start_row = (row_id - 1) * row_size_normal;
	int start_col = (col_id - 1) * col_size_normal;
	if (DEBUG) printf(" start_row: %d\n start_col: %d\n\n", start_row, start_col);
	for (int i = -1; i < row_size + 1; ++i) {
		for (int j = -1; j < col_size + 1; ++j) {
			if (checkExist(start_row + i, start_col + j)) {
				game_board[i + 1][j + 1] = whole_board[start_row + i][start_col + j];
			}
		}
	}

	// copy the board
	copyBoard(copy_board, game_board, 0, row_size + 1, 0, col_size + 1);

	updated = true;
}

void GameWorker::initBoardMPI() {
	// call MPI to retrieve data from master
	int needed_len = (row_size_normal + extra_last_row + 2) * (col_size_normal + extra_last_col + 2) * 2 + 1;
	char buf[needed_len];
	// receive initial data from master
	MPI_Recv(buf, needed_len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
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

	// copy the board
	copyBoard(copy_board, game_board, 0, row_size + 1, 0, col_size + 1);

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

// check if a specific block exists
bool GameWorker::checkBlockExist(int rid, int cid) {
	if (rid <= 0 || rid >= num_node_in_row) {
		return false;
	}
	if (cid <= 0 || cid >= num_node_in_col) {
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
	
	for (int i = start_row; i <= end_row; ++i) {
		for (int j = start_col; j <= end_col; ++j) {
			copyTo[i][j] = copyFrom[i][j];
		}
	}
}

void GameWorker::iterateOnce() {
	// if (!updated) {
	// 	// call MPI to get updated neighbours
	// 	// send out the local copy first
	// 	sendToNeighbours();
	// 	// then retrieve remote copies
	// 	recvFromNeighbours();
	// }
	
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
	copyBoard(game_board, copy_board, 1, row_size, 1, col_size);

	updated = false;

	// if need to print the process, send intermediate results to master
	if (cur_iteration == num_iterate || PRINT_PROCESS) {
		if (row_id == 1 && col_id == 1) {
			// remember to also copy back the master copy
			for (int i = 0; i < row_size; ++i) {
				for (int j = 0; j < col_size; ++j) {
					whole_board[i][j] = game_board[i + 1][j + 1];
				}
			}
			recvSubBoard();
		}
		else {
			sendSubBoard();
		}
	}
}

int GameWorker::getRank(int temp_row_id, int temp_col_id) {
	int temp_rank = 0;
	temp_rank += (temp_row_id - 1) * num_node_in_col;
	temp_rank += temp_col_id;
	return temp_rank;
}

void GameWorker::sendToNeighbours() {
	// top left
	int temp_rank;
	if (checkBlockExist(row_id - 1, col_id - 1)) {
		temp_rank = getRank(row_id - 1, col_id - 1);
		MPI_Send(&game_board[1][1], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD);
	}
	// bottom left
	if (checkBlockExist(row_id + 1, col_id - 1)) {
		temp_rank = getRank(row_id + 1, col_id - 1);
		MPI_Send(&game_board[row_size][1], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD);
	}
	// top right
	if (checkBlockExist(row_id - 1, col_id + 1)) {
		temp_rank = getRank(row_id - 1, col_id + 1);
		MPI_Send(&game_board[1][col_size], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD);
	}
	// bottom right
	if (checkBlockExist(row_id + 1, col_id + 1)) {
		temp_rank = getRank(row_id + 1, col_id + 1);
		MPI_Send(&game_board[row_size][col_size], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD);
	}

	int temp_needed_len;
	// left
	if (checkBlockExist(row_id, col_id - 1)) {
		temp_rank = getRank(row_id, col_id - 1);
		temp_needed_len = row_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		char* cur = buf;
		for (int i = 1; i < row_size + 1; ++i) {
			sprintf(cur, "%d,", game_board[i][1]);
			cur += 2;
		}
		// add one more space to avoid messy code
		sprintf(cur, " ");
		MPI_Send(buf, temp_needed_len, MPI_CHAR, temp_rank, 0, MPI_COMM_WORLD);
	}
	// right
	if (checkBlockExist(row_id, col_id + 1)) {
		temp_rank = getRank(row_id, col_id + 1);
		temp_needed_len = row_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		char* cur = buf;
		for (int i = 1; i < row_size + 1; ++i) {
			sprintf(cur, "%d,", game_board[i][col_size]);
			cur += 2;
		}
		// add one more space to avoid messy code
		sprintf(cur, " ");
		MPI_Send(buf, temp_needed_len, MPI_CHAR, temp_rank, 0, MPI_COMM_WORLD);
	}
	// top
	if (checkBlockExist(row_id - 1, col_id)) {
		temp_rank = getRank(row_id - 1, col_id);
		temp_needed_len = col_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		char* cur = buf;
		for (int i = 1; i < col_size + 1; ++i) {
			sprintf(cur, "%d,", game_board[1][i]);
			cur += 2;
		}
		// add one more space to avoid messy code
		sprintf(cur, " ");
		MPI_Send(buf, temp_needed_len, MPI_CHAR, temp_rank, 0, MPI_COMM_WORLD);
	}
	// bottom
	if (checkBlockExist(row_id + 1, col_id)) {
		temp_rank = getRank(row_id + 1, col_id);
		temp_needed_len = col_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		char* cur = buf;
		for (int i = 1; i < col_size + 1; ++i) {
			sprintf(cur, "%d,", game_board[row_size][i]);
			cur += 2;
		}
		// add one more space to avoid messy code
		sprintf(cur, " ");
		MPI_Send(buf, temp_needed_len, MPI_CHAR, temp_rank, 0, MPI_COMM_WORLD);
	}
}

void GameWorker::recvFromNeighbours() {
	// top left
	int temp_rank;
	if (checkBlockExist(row_id - 1, col_id - 1)) {
		temp_rank = getRank(row_id - 1, col_id - 1);
		MPI_Recv(&copy_board[0][0], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD, &status);
	}
	// bottom left
	if (checkBlockExist(row_id + 1, col_id - 1)) {
		temp_rank = getRank(row_id + 1, col_id - 1);
		MPI_Recv(&copy_board[row_size + 1][0], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD, &status);
	}
	// top right
	if (checkBlockExist(row_id - 1, col_id + 1)) {
		temp_rank = getRank(row_id - 1, col_id + 1);
		MPI_Recv(&copy_board[0][col_size + 1], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD, &status);
	}
	// bottom right
	if (checkBlockExist(row_id + 1, col_id + 1)) {
		temp_rank = getRank(row_id + 1, col_id + 1);
		MPI_Recv(&copy_board[row_size + 1][col_size + 1], 1, MPI_INT, temp_rank, 0, MPI_COMM_WORLD, &status);
	}

	int temp_needed_len;
	// left
	if (checkBlockExist(row_id, col_id - 1)) {
		temp_rank = getRank(row_id, col_id - 1);
		temp_needed_len = row_size * 2 + 1;
		char buf[temp_needed_len];
		MPI_Recv(buf, temp_needed_len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
		for (int i = 1; i < row_size + 1; ++i) {
			char* tmp;
			if (i == 1) {
				tmp = strtok(buf, ",");
			}
			else {
				tmp = strtok(NULL, ",");
			}
			copy_board[i][1] = atoi(tmp);
		}
	}
	// right
	if (checkBlockExist(row_id, col_id + 1)) {
		temp_rank = getRank(row_id, col_id + 1);
		temp_needed_len = row_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		MPI_Recv(buf, temp_needed_len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
		for (int i = 1; i < row_size + 1; ++i) {
			char* tmp;
			if (i == 1) {
				tmp = strtok(buf, ",");
			}
			else {
				tmp = strtok(NULL, ",");
			}
			copy_board[i][col_size] = atoi(tmp);
		}
	}
	// top
	if (checkBlockExist(row_id - 1, col_id)) {
		temp_rank = getRank(row_id - 1, col_id);
		temp_needed_len = col_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		MPI_Recv(buf, temp_needed_len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
		for (int i = 1; i < col_size + 1; ++i) {
			char* tmp;
			if (i == 1) {
				tmp = strtok(buf, ",");
			}
			else {
				tmp = strtok(NULL, ",");
			}
			copy_board[1][i] = atoi(tmp);
		}
	}
	// bottom
	if (checkBlockExist(row_id + 1, col_id)) {
		temp_rank = getRank(row_id + 1, col_id);
		temp_needed_len = col_size * 2 + 1;
		// concatenate cells into a char array and send it out
		char buf[temp_needed_len];
		MPI_Recv(buf, temp_needed_len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
		for (int i = 1; i < col_size + 1; ++i) {
			char* tmp;
			if (i == 1) {
				tmp = strtok(buf, ",");
			}
			else {
				tmp = strtok(NULL, ",");
			}
			copy_board[row_size][i] = atoi(tmp);
		}
	}
}

void GameWorker::sendSubBoard() {
	int temp_needed_len = row_size * col_size * 2 + 1;
	// concatenate cells into a char array and send it out
	char buf[temp_needed_len];
	char* cur = buf;
	for (int i = 1; i < row_size + 1; ++i) {
		for (int j = 1; j < col_size + 1; ++j) {
			sprintf(cur, "%d,", game_board[i][j]);
			cur += 2;
		}
	}
	// add one more space to avoid messy code
	sprintf(cur, " ");
	MPI_Send(buf, temp_needed_len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD);
}

void GameWorker::recvSubBoard() {
	// first update self board to original board
	int temp_start_row = (row_id - 1) * row_size_normal;
	int temp_start_col = (col_id - 1) * col_size_normal;
	// if (DEBUG) printf(" temp_start_row: %d\n temp_start_col: %d\n\n", temp_start_row, temp_start_col);
	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			whole_board[temp_start_row + i][temp_start_col + i] = game_board[i + 1][j + 1];
		}
	}

	// receive subboards from all slaves	
	for (int temp_rank = 1; temp_rank < num_process; ++temp_rank) {
		int temp_row_id, temp_col_id;
		int worker_row_size = row_size_normal;
		int worker_col_size = col_size_normal;

		// id both start from 1
		temp_row_id = temp_rank / num_node_in_col + 1;
		temp_col_id = temp_rank - (temp_row_id - 1) * num_node_in_col + 1;

		// if it is the last node in a row or column
		// allocate extra work
		if (temp_row_id == num_node_in_row) {
			worker_row_size += extra_last_row;
		}
		if (temp_col_id == num_node_in_col) {
			worker_col_size += extra_last_col;
		}

		temp_start_row = (temp_row_id - 1) * row_size_normal;
		temp_start_col = (temp_col_id - 1) * col_size_normal;

		// finished calculation, now receiving
		int needed_len = worker_row_size * worker_col_size * 2 + 1;
		char buf[needed_len];
		MPI_Recv(buf, needed_len, MPI_CHAR, temp_rank, 0, MPI_COMM_WORLD, &status);
		if (DEBUG) printf("#%d, buf: %s\n", temp_rank, buf);

		for (int i = 0; i < worker_row_size; ++i) {
			for (int j = 0; j < worker_col_size; ++j) {
				char* tmp;
				if (i == 0 && j == 0) {
					tmp = strtok(buf, ",");
				}
				else {
					tmp = strtok(NULL, ",");
				}
				whole_board[temp_start_row + i][temp_start_col + j] = atoi(tmp);
			}
		}
	}
}

int GameWorker::countNeighbours(int row, int col) {
	int alive = 0;
	if (checkAlive(row - 1, col - 1))
		++alive;
	if (checkAlive(row - 1, col))
		++alive;
	if (checkAlive(row - 1, col + 1))
		++alive;
	if (checkAlive(row, col - 1))
		++alive;
	if (checkAlive(row, col + 1))
		++alive;
	if (checkAlive(row + 1, col - 1))
		++alive;
	if (checkAlive(row + 1, col))
		++alive;
	if (checkAlive(row + 1, col + 1))
		++alive;

	return alive;
}

bool GameWorker::checkAlive(int row, int col) {
	return game_board[row][col] == ALIVE;
}

void GameWorker::print() {
	for (int i = 0; i < row_size + 2; i++) {
		for (int j = 0; j < col_size + 2; j++) {
			char status = game_board[i][j]?'1':'0';
			printf("%c ", status);
		}
		printf("\n");
	}
	printf("\n");
}
