#include <stdio.h>

#define HEIGHT 5
#define WIDTH 5

void printGrid ( int grid[HEIGHT][WIDTH] );
void printBlock ( int blockType, int rotation );
void filledRow ( int grid[HEIGHT][WIDTH] );
int checkGameStatus (int grid[HEIGHT][WIDTH] );

/*	Blocks are stored by block, based on a numbered 4 by 4 matrix
 *	+-----------+
 *	|0 |1 |2 |3 |
 *	|--+--+--+--|
 *	|4 |5 |6 |7 |
 *	|--+--+--+--|
 *	|8 |9 |10|11|
 *	|--+--+--+--|
 *	|12|13|14|15|
 *	+-----------+
 *
 *  The blocks follow the Tetris Super Rotation System:
 *  https://strategywiki.org/wiki/Tetris/Rotation_systems
 */

// block[blockType][rotation][blockNum]
const int block[7][4][4] = {
	 {{4,5,6,7},{2,6,10,14},{8,9,10,11},{1,5,9,13},},	// I block
	 {{0,4,5,6},{1,2,5,9},{4,5,6,10},{1,5,8,9},},		// L block
	 {{2,4,5,6},{1,5,9,10},{4,5,6,8},{0,1,5,9},},		// flipped L block
	 {{1,2,5,6},{1,2,5,6},{1,2,5,6},{1,2,5,6},},		// Square block
	 {{1,2,4,5},{1,5,6,10},{5,6,8,9},{0,4,5,9},},		// Flipped Z block
	 {{1,4,5,6},{1,5,6,9},{4,5,6,9},{1,4,5,9},},		// T block
	 {{0,1,5,6},{2,5,6,9},{4,5,9,10},{1,4,5,8},},		// Z block
};


int main () {
	// Initialize grid
	static int grid[HEIGHT][WIDTH];
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			grid[i][j] = 0;

	// Check if Game Over
	if ( checkGameStatus(grid) ) {
		printf("Game Over\n");
		return 0;
	}

	return 0;
}

// Prints out Tetris Grid
void printGrid ( int grid[HEIGHT][WIDTH] ) {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d, ", grid[i][j]);
		}
		printf("\n");
	}
}

// Remove a filled row and drop rows above
void filledRow ( int grid[HEIGHT][WIDTH] ) {
	for (int i = 0; i < HEIGHT; i++) {
		int haveZero = 0;
		for (int j = 0; j < WIDTH; j++) {
			// Check if line is full
			if ( !grid[i][j] ) {
				haveZero = 1;
				break;
			}
		}

		if ( !haveZero ) {
			//Shift row downwards
			for (int y = i; y > 0; y--) {
				for (int x = 0; x < WIDTH; x++) {
					grid[y][x] = grid[y-1][x];
				}
			}
			
			// Add row at top
			for (int x = 0; x < WIDTH; x++) {
				grid[0][x] = 0;
			}
		}
	}
}

// Checking if tetris board is filled
int checkGameStatus (int grid[HEIGHT][WIDTH] ) {
	for (int i = 0; i < WIDTH; i++)
		if ( grid[0][i] == 1 ) 
			return 1;

	return 0;
}

// Print out a tetris block
void printBlock ( int blockType, int rotation ) {
	int output[4][4] = {
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
	};

	for (int i = 0; i < 4; i++) {
		output[ block[blockType][rotation][i]/4 ][ block[blockType][rotation][i]%4 ] = 1;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d ", output[i][j]);
		}
		printf("\n");
	}
}
