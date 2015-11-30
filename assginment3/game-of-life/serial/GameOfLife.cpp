#include "GameOfLife.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1

GameOfLife::GameOfLife(int row, int col, int** board) {
	row_size = row;
	col_size = col;
	game_board = board;
	cur_iteration = 0;
	cur_alive = 0;
}

GameOfLife::~GameOfLife() {

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

	int buf[row_size][col_size];
	// copy the board
	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			buf[i][j] = game_board[i][j];
		}
	}

	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			int count = countNeighbours(i, j);
			if (count <= 1 || count >= 4) {
				buf[i][j] = DEAD;
			}
			else {
				buf[i][j] = ALIVE;
				++alive;
			}
		}
	}
	cur_alive = alive;

	// copy back the board
	for (int i = 0; i < row_size; ++i) {
		for (int j = 0; j < col_size; ++j) {
			game_board[i][j] = buf[i][j];
		}
	}
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
