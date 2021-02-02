#include <stdio.h>

#define HEIGHT 5
#define WIDTH 5

void printGrid();
void printBlock();
void filledRow();
int checkGameStatus();
int coordinateToX();
int coordinateToY();
int moveBlock(int x, int blockID, int rotation);
int blockOut(int x, int blockID, int rotation);
int blockCollision(int x, int blockID, int rotation);

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
	 {{4,5,6,7}, {2,6,10,14},{8,9,10,11},{1,5,9,13},},	// I block
	 {{1,2,5,6},{1,2,5,6},{1,2,5,6},{1,2,5,6},},		// Square block
	 {{0,4,5,6},{1,2,5,9},{4,5,6,10},{1,5,8,9},},		// flipped L block
	 {{2,4,5,6},{1,5,9,10},{4,5,6,8},{0,1,5,9},},		// L block
	 {{1,2,4,5},{1,5,6,10},{5,6,8,9},{0,4,5,9},},		// Flipped Z block
	 {{1,4,5,6},{1,5,6,9},{4,5,6,9},{1,4,5,9},},		// T block
	 {{0,1,5,6},{2,5,6,9},{4,5,9,10},{1,4,5,8},},		// Z block
};

// Stores the corners (top left, bottom right) of each tetromino. Allows for check if tetromino is out of bounds.
// blockSilloute[blockType][rotation][blockNum]
const int blockSilloute[3][4][2] = {
	{ {4,7}, {2,14}, {8,11}, {1,13} },	// I block
	{ {1,6}, {1,6}, {1,6}, {1,6} },		// Square block
	{ {0,6}, {1,10}, {4,10}, {0,9} }, 	// flipped/normal L/T/Z Block
};

int cursorX;
int cursorY;
static int grid[HEIGHT][WIDTH];

int main () {

	// Initialze Grid
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			grid[i][j] = 0;

	// Check if Game Over
	if ( checkGameStatus(grid) ) {
		printf("Game Over\n");
		return 0;
	}

	int cursorX = 0;
	int cursorY = HEIGHT-1;
	printf("%d", moveBlock(1, 0, 0));

	return 0;
}

// Prints out Tetris Grid
void printGrid () {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d, ", grid[i][j]);
		}
		printf("\n");
	}
}

// Remove a filled row and drop rows above
void filledRow () {
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
int checkGameStatus () {
	for (int i = 0; i < WIDTH; i++)
		if ( grid[0][i] == 1 ) 
			return 1;

	return 0;
}

// Print out a tetris block
// Temporary code for debug, can be optimized
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

// Move tetromino left or right
// Imagine that the block is placed ontop of the tetris grid array, with the top left corner of the tetromino 4 by 4 array on x/y of the array
// x is the amount moved (negative for left)
// return 1 if moveBlock failed, 0 if moveBlock is successful
int moveBlock ( int x, int blockID, int rotation ) {
	
	// Check if block out of bounds 
	if ( blockOut(x, blockID, rotation) ) 
		return 1;

	// Check if bo
	if ( blockCollision(x, blockID, rotation) ) 
		return 1;

	cursorX += x;
	return 0;
}

// Parse the coordinate number system to array index (x)
int coordinateToX ( int input ) {
	return input%4;
}

// Parse the coordinate number system to array index (y)
int coordinateToY ( int input ) {
	return input/4;
}

// Check if the tetromino is out of bounds
int blockOut ( int x, int blockID, int rotation ) {
	// BlockID that is than 1 have the same silloute
	int xLeft  = coordinateToX(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][1]);	
	int xRight = coordinateToX(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][2]);	

	// check if out of bounds
	if ( xLeft + cursorX + x < 0 || xRight + cursorX + x >= WIDTH ) 
		return 1;
	
	return 1;
}

// Check if the tetromino is colliding with the playing field (if the tetromino is "in the ground")
int	blockCollision (int x, int blockID, int rotation) {
	// check if hit other objects on the board
	for (int i = 0; i < 4; i++)
		if ( grid[coordinateToX(block[blockID][rotation][i])][coordinateToY(block[blockID][rotation][i])] ) 
			return 1;

}
