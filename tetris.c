#include <stdlib.h>
#include <ncurses.h>

#define HEIGHT 11
#define WIDTH 5

void printGrid();
void filledRow();
int checkGameStatus();
int coordinateToX();
int coordinateToY();
int moveBlock(int x, int blockID, int rotation);
int blockOut(int x, int blockID, int rotation);
int blockCollision(int x, int blockID, int rotation);
int rotateBlock(int rotation, int blockID, int currentRotation);
void putBlock();
int shadowBlock(int blockID, int rotation);
void printHoverBlock(int blockID, int rotation);

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
	 {{0,4,5,6},{1,2,5,9},{4,5,6,10},{1,5,8,9},},		// Flipped L block
	 {{2,4,5,6},{1,5,9,10},{4,5,6,8},{0,1,5,9},},		// L block
	 {{1,2,4,5},{1,5,6,10},{5,6,8,9},{0,4,5,9},},		// Flipped Z block
	 {{0,1,5,6},{2,5,6,9},{4,5,9,10},{1,4,5,8},},		// Z block
	 {{1,4,5,6},{1,5,6,9},{4,5,6,9},{1,4,5,9},},		// T block
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
int cursorRotation;
int	cursorBlock;
// static int grid[HEIGHT][WIDTH];
static int grid[HEIGHT][WIDTH] = {
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
	{0,0,0,0,0},
};

int main () {

	// Terminal screen width/height
	int row, col;

	// Starting ncurses
	initscr();
	cbreak();
	noecho();
	getmaxyx(stdscr,row,col);
	start_color();

	// Game Start Screen
	curs_set(0);
	mvprintw(row/2,(col-18)/2,"Welcome to Tetris!");
	mvprintw(row/2+1,(col-24)/2,"(Press any key to start)");
	getch();
	clear();

	// Enable Color
	init_pair(1, COLOR_BLACK,COLOR_WHITE);
	init_pair(2, COLOR_BLACK,COLOR_BLUE);

	// Start Game
	cursorX = 0;
	cursorY = 0;

	cursorBlock = 4;
	cursorRotation = 0;
	printGrid();
	// Main Game Loop
	do {
		char c;
		c = getch();

		cursorY = 0;
		// Read keyboard input until place block
		// ad for left right, ws for rotate, eq to flip
		switch (c) {
			case 'a':
				moveBlock(-1,cursorBlock,cursorRotation);
				break;
			case 'd':
				moveBlock(1,cursorBlock,cursorRotation);
				break;
			case 'w':
				rotateBlock(1,cursorBlock,cursorRotation);
				break;
			case 's':
				rotateBlock(5,cursorBlock,cursorRotation);	// 5 is 4 + 1, modulus in rotateBlock will turn into -1
				break;
			case 'q':	// Flip Block
			case 'e':
				rotateBlock(2,cursorBlock,cursorRotation);
				break;
			case ' ':
				shadowBlock(cursorBlock,cursorRotation);
				putBlock();
				break;
			default:
				break;
		}

		clear();
		printGrid();
		shadowBlock(cursorBlock,cursorRotation);
		printHoverBlock(cursorBlock,cursorRotation);

		// Diagnostics
		mvprintw(10,10,"%d% d% d %d\n", cursorX, cursorY, cursorBlock, cursorRotation);

	} while ( ! checkGameStatus() );

	/*
	// Print Grid
	printGrid();
	shadowBlock(0,1);
	printHoverBlock(0,1);
	getch();

	putBlock(0,1);
	clear();
	printGrid();
	refresh();
	getch();

	filledRow();
	clear();
	printGrid();
	getch();

	*/

	endwin();
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d ", grid[i][j]);
		}
		printf("\n");
	}
	exit(0);
}

// Prints out Tetris Grid
void printGrid () {
	move(0,0);
	for (int i = 0; i < WIDTH*2; i++)
		printw("-");

	attron(COLOR_PAIR(1));
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			move(i+1,j*2);
			if ( grid[i][j] ) {
				printw("  ");
			}
		}
		printw("\n");
	}
	attroff(COLOR_PAIR(1));
	for (int i = 0; i < WIDTH*2; i++)
		printw("-");
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
// return 1 if gameover, 0 if game is continuing
int checkGameStatus () {
	for (int i = 0; i < WIDTH; i++)
		if ( grid[1][i] == 1 )			// Add one spare row to top for buffer
			return 1;

	return 0;
}

// Move tetromino left or right
// Imagine that the block is placed ontop of the tetris grid array, with the top left corner of the tetromino 4 by 4 array on x/y of the array
// x is the amount moved (negative for left)
// return 1 if moveBlock failed, 0 if moveBlock is successful
int moveBlock ( int x, int blockID, int rotation ) {
	// Check if block out of bounds
	if ( blockOut(x, blockID, rotation) )
		return 1;

	// Check if block collides with grid
	if ( blockCollision(x, blockID, rotation) )
		return 1;

	cursorX += x;
	cursorRotation = rotation;
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
	int xLeft   = coordinateToX(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][0]);
	int xRight  = coordinateToX(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][1]);
	int yTop    = coordinateToY(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][0]);
	int yBottom = coordinateToY(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][1]);

	// check if out of bounds
	if ( xLeft + cursorX + x < 0 || xRight + cursorX + x >= WIDTH )
		return 1;

	if ( cursorY + yTop < 0 || cursorY + yBottom >= HEIGHT )
		return 1;

	return 0;
}

// Check if the tetromino is colliding with the playing field (if the tetromino is "in the ground")
int	blockCollision (int x, int blockID, int rotation) {
	// check if hit other objects on the board
	for (int i = 0; i < 4; i++)
		if ( grid[coordinateToY(block[blockID][rotation][i]) + cursorY][coordinateToX(block[blockID][rotation][i]) + cursorX] )
			return 1;

	printf("\n");
	return 0;
}

// Rotates the block while checking for collision/out of bounds
int rotateBlock (int rotation, int blockID, int currentRotation) {

	int finalRotation = ( currentRotation + rotation ) % 4;

	// Check if block rotation is allowed (moveBlock returns 1 if it fails)
	if ( moveBlock(0, blockID, finalRotation) )
		if ( moveBlock(1, blockID, finalRotation) )			// test moving block and right for wallkick
			if ( moveBlock(-1, blockID, finalRotation) )
				return 1;

	return 0;
}

// Write the block onto the tetris grid
void putBlock() {
	for (int i = 0; i < 4; i++)
		grid[coordinateToY(block[cursorBlock][cursorRotation][i]) + cursorY][coordinateToX(block[cursorBlock][cursorRotation][i]) + cursorX] = 1;
}

// Set cursorY to the lowest possible position
// Returns 1 if block cannot be placed here
int shadowBlock (int blockID, int rotation) {
	// Put block out of bounds
	cursorY = 0;

	// Lower block until block hit grid
	while ( ! moveBlock(0,blockID,rotation) )
		cursorY++;

	if ( cursorY == 0 )
		return 1;

	// Retract Y
	cursorY--;

	return 0;
}

void printHoverBlock (int blockID, int rotation) {
	attron(COLOR_PAIR(2));
	for (int i = 0; i < 4; i++) {
		mvprintw(coordinateToY(block[blockID][rotation][i])+1,(coordinateToX(block[blockID][rotation][i])+cursorX)*2,"  ");
	}
	attroff(COLOR_PAIR(2));
}
