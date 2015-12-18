#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "GameOfLife.h"
#include "debug_config.h"

using namespace std;

GameOfLife::GameOfLife(int row, int col, int num_iterate) {
	if (row <= 0 || col <= 0) {
		row_size = 6;
		col_size = 4;
		random = false;
	}
	else {
		row_size = row;
		col_size = col;
		random = true;
	}
	this->num_iterate = num_iterate;

	/* initialize for MPI */
	int fake_argc = 1;
	char** fake_argv;
	MPI_Init(&fake_argc, &fake_argv);

	/* get number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &num_process);

	/* get this process's number (ranges from 0 to num_process - 1) */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

GameOfLife::~GameOfLife() {
	// delete the game board if it is master
	if (rank == 0) {
		for (int i = 0; i < row_size; ++i)
			delete game_board[i];
		delete[] game_board;
	}
	delete worker;
	/* clean up for MPI */
	MPI_Finalize();
}

void GameOfLife::initBoard() {
	if (!random) {
		initSpecificBoard();
	}
	else {
		initRandomBoard();		
	}
}

void GameOfLife::initRandomBoard() {
	// create game board
	game_board = new int*[row_size];
	for(int i = 0; i < row_size; ++i)
		game_board[i] = new int[col_size];

	// init game board
	srand(time(0));	
	for (int i = 0; i < row_size; i++) {
		for (int j = 0; j < col_size; j++) {
			game_board[i][j] = rand() % 2;
		}
	}
}

void GameOfLife::initSpecificBoard() {
	// create game board
	game_board = new int*[row_size];
	for(int i = 0; i < row_size; ++i)
		game_board[i] = new int[col_size];

	game_board[0][0] = DEAD;
	game_board[0][1] = DEAD;
	game_board[0][2] = DEAD;
	game_board[0][3] = ALIVE;
	game_board[1][0] = DEAD;
	game_board[1][1] = DEAD;
	game_board[1][2] = DEAD;
	game_board[1][3] = ALIVE;
	game_board[2][0] = DEAD;
	game_board[2][1] = ALIVE;
	game_board[2][2] = DEAD;
	game_board[2][3] = DEAD;
	game_board[3][0] = ALIVE;
	game_board[3][1] = ALIVE;
	game_board[3][2] = ALIVE;
	game_board[3][3] = DEAD;
	game_board[4][0] = DEAD;
	game_board[4][1] = ALIVE;
	game_board[4][2] = DEAD;
	game_board[4][3] = DEAD;
	game_board[5][0] = DEAD;
	game_board[5][1] = ALIVE;
	game_board[5][2] = DEAD;
	game_board[5][3] = DEAD;
}

void GameOfLife::gridAssign() {
	if (DEBUG && rank == 0) printf("--- Grid Assignment ---\n");

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

	for (vector<int>::reverse_iterator i = factors.rbegin(); i < factors.rend(); ++i) {
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

	if (DEBUG && rank == 0) 
		printf(
			" num_node_in_row: %d\n" \
			" num_node_in_col: %d\n" \
			" row_per_node: %d\n" \
			" col_per_node: %d\n" \
			" extra_last_row: %d\n" \
			" extra_last_col: %d\n\n", 
			num_node_in_row, 
			num_node_in_col, 
			row_per_node, 
			col_per_node, 
			extra_last_row, 
			extra_last_col); 
}

void GameOfLife::initWorker() {
	if (DEBUG && rank == 0) printf("--- Init Worker: %d ---\n", rank);

	int row_id, col_id;
	int worker_row_size = row_per_node;
	int worker_col_size = col_per_node;

	// id both start from 1
	row_id = rank / num_node_in_col + 1;
	col_id = rank - (row_id - 1) * num_node_in_col + 1;
	if (DEBUG && rank == 0) printf(" row_id: %d\n col_id: %d\n", row_id, col_id);

	// if it is the last node in a row or column
	// allocate extra work
	if (row_id == num_node_in_row) {
		worker_row_size += extra_last_row;
	}
	if (col_id == num_node_in_col) {
		worker_col_size += extra_last_col;
	}
	if (DEBUG && rank == 0) printf(" worker_row_size: %d\n worker_col_size: %d\n\n", worker_row_size, worker_col_size);

	// create worker
	worker = new GameWorker(
		worker_row_size, 
		worker_col_size, 
		row_id, 
		col_id, 
		row_size, 
		col_size, 
		row_per_node, 
		col_per_node,
		extra_last_row,
		extra_last_col,
		num_node_in_row,
		num_node_in_col,
		num_iterate);

	// init the game board for all slaves
	if (rank == 0) {
		int needed_len = (row_per_node + extra_last_row + 2) * (col_per_node + extra_last_col + 2) * 2 + 1;
		// first send out all sub-boards
		for (int temp_rank = 1; temp_rank < num_process; ++temp_rank) {
			// calculate the starting point and range
			int temp_row_id = temp_rank / num_node_in_col + 1;
			int temp_col_id = temp_rank - (temp_row_id - 1) * num_node_in_col + 1;
			int temp_start_row = (temp_row_id - 1) * row_per_node;
			int temp_start_col = (temp_col_id - 1) * col_per_node;
			int temp_worker_row_size = row_per_node;
			int temp_worker_col_size = col_per_node;
			// if it is the last node in a row or column
			// allocate extra work
			if (temp_row_id == num_node_in_row) {
				temp_worker_row_size += extra_last_row;
			}
			if (temp_col_id == num_node_in_col) {
				temp_worker_col_size += extra_last_col;
			}

			// concatenate cells into a char array and send it out
			char buf[needed_len];
			char* cur = buf;
			for (int i = -1; i < temp_worker_row_size + 1; ++i) {
				for (int j = -1; j < temp_worker_col_size + 1; ++j) {
					sprintf(cur, "%d,", getCellStatus(i + temp_start_row, j + temp_start_col));
					cur += 2;
				}
			}
			// add one more space to avoid messy code
			sprintf(cur, " ");
			MPI_Send(buf, needed_len, MPI_CHAR, temp_rank, 0, MPI_COMM_WORLD);

			/*
			if (DEBUG && rank == 0) 
				printf(
					" temp_rank: %d\n" \
					" temp_row_id: %d\n" \
					" temp_col_id: %d\n" \
					" temp_start_row: %d\n" \
					" temp_start_col: %d\n" \
					" send: %s\n\n", 
					temp_rank,
					temp_row_id, 
					temp_col_id, 
					temp_start_row, 
					temp_start_col, 
					buf);
			*/
		}
		// now init the local board
		worker->initBoardLocal(game_board);
	}
	else {
		worker->initBoardMPI();
	}
}

int GameOfLife::getCellStatus(int row, int col) {
	if (row < 0 || row >= row_size) {
		return DEAD;
	}
	if (col < 0 || col >= col_size) {
		return DEAD;
	}
	return game_board[row][col];
}

void GameOfLife::start() {
	for (int i = 0; i < num_iterate; ++i) {
		iterateOnce();
	}
}

void GameOfLife::iterateOnce() {
	worker->iterateOnce();
	++cur_iteration;

	if (rank == 0 && 
		(PRINT_PROCESS && notTooLarge()) &&
		cur_iteration != num_iterate) {
		print();
	}
}

bool GameOfLife::isMaster() {
	return rank == 0;
}

void GameOfLife::print() {
	printf("Round%2d:\n------\n", cur_iteration);
	for (int i = 0; i < row_size; i++) {
		for (int j = 0; j < col_size; j++) {
			char status = game_board[i][j]?'1':'0';
			printf("%c ", status);
		}
		printf("\n");
	}
	printf("\n");
}

bool GameOfLife::notTooLarge() {
	if (row_size <= 20 && col_size <= 20) {
		return true;
	}
	return false;
}
