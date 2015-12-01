#include "GameOfLife.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 0

GameOfLife::GameOfLife(int row, int col, int** board) {
	row_size = row;
	col_size = col;
	game_board = board;
	cur_iteration = 0;
	cur_alive = 0;
	// create the copy board
	copy_board = new int*[row_size];
    for(int i = 0; i < row_size; ++i)
        copy_board[i] = new int[col_size];
    copyBoard(copy_board, game_board);
}

GameOfLife::~GameOfLife() {
	for (int i = 0; i < row_size; ++i) {
		delete copy_board[i];
	}
	delete[] copy_board;
}

int GameOfLife::randomInit()
{
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

int GameOfLife::getNumAlive() {
	int alive = 0;
	for (int i = 0 ; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			if (game_board[i][j] == ALIVE) {
				++alive;
			}
		}
	}
	return alive;
}

void GameOfLife::iterateOnce() {
	++cur_iteration;
	int alive = 0;

	#pragma omp parallel for reduction(+:alive)
	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			int count = countNeighbours(i, j);
			if (count <= 1 || count >= 4) {
				copy_board[i][j] = DEAD;
			}
			else {
				copy_board[i][j] = ALIVE;
				alive += 1;
			}
		}
	}

	// copy back the board
	#pragma omp parallel for
	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			game_board[i][j] = copy_board[i][j];
		}
	}

	cur_alive = alive;
}

void GameOfLife::iterateAll(int iteration) {
	for (int i = 0; i < iteration; ++i) {
		iterateOnce();
		if (DEBUG) {
			print();
		}
		if (cur_alive == 0) {
			printf("Game Over at Round%2d.\n", cur_iteration);
			break;
		}
	}
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

bool GameOfLife::checkAlive(int row, int col) {
	if (row >= row_size || col >= col_size || row < 0 || col < 0) {
		return false;
	}
	return game_board[row][col] == ALIVE;
}

int GameOfLife::countNeighbours(int row, int col) {
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

void GameOfLife::copyBoard(int** copyTo, int** copyFrom) {
	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			copyTo[i][j] = copyFrom[i][j];
		}
	}
}
