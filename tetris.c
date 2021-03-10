#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define HEIGHT 22
#define WIDTH 10

void printGrid();
void filledRow();
void putBlock();
void generateBag();
void printBlock();
void printHold();
void nextBlock();
void update();
void printGhostBlock();
int hold();
int checkGameStatus();
int coordinateToX();
int coordinateToY();
int checkBlock(int x, int y, int blockID, int rotation);
int moveCursor(int x, int y, int blockID, int rotation);
int blockOut(int x, int y, int blockID, int rotation);
int blockCollision(int x, int y, int blockID, int rotation);
int rotateBlock(int rotation, int blockID, int currentRotation);
int shadowBlock(int blockID, int rotation);

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
 *	The blocks follow the Tetris Super Rotation System:
 *	https://strategywiki.org/wiki/Tetris/Rotation_systems
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
	{ {0,6}, {1,10}, {4,10}, {0,9} },	// flipped/normal L/T/Z Block
};

// Data on current cursor
int cursorX;
int cursorY;
int cursorRotation;
int cursorBlock;

// How many seconds can the play manipulate the tetris block
float cursorTime = 10;
// Time for block to drop
float cursorSpeed;

// Defining the playing field
static int grid[HEIGHT][WIDTH];

// Stores a bag/pool of tetrominos
int bag[7];
int bagIndex = 7;

// If already swap block
int cursorHold = 0;

int buffer = 7;

int score = 0;

clock_t timer, dropTime;

int main () {
	// Seed randomness
	srand(time(NULL));

	// Initilazing the grid
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			grid[HEIGHT-1][WIDTH-1] = 0;

	// Terminal screen width/height
	int row, col;

	// Starting ncurses
	initscr();
	cbreak();
	noecho();
	getmaxyx(stdscr,row,col);
	keypad(stdscr, TRUE);
	start_color();

	// Game Start Screen
	curs_set(0);
	mvprintw(row/2,(col-18)/2,"Welcome to Tetris!");
	mvprintw(row/2+1,(col-24)/2,"(Press any key to start)");
	getch();
	clear();

	// Enable Color
	init_pair(1, COLOR_BLACK,COLOR_BLUE);
	init_pair(2, COLOR_BLACK,COLOR_WHITE);
	init_pair(3, COLOR_BLACK,COLOR_CYAN);
	init_pair(4, COLOR_BLACK,COLOR_YELLOW);
	init_pair(5, COLOR_BLACK,COLOR_RED);
	init_pair(6, COLOR_BLACK,COLOR_GREEN);
	init_pair(7, COLOR_BLACK,COLOR_MAGENTA);

	cursorRotation = 0;
	printGrid();

	// Initialize cursor value
	cursorY = 0;
	cursorX = WIDTH/2 - 2; // Center

	update();

	timer = clock();
	dropTime = timer;

	// Main Game Loop
	do {
		// don't print screen if skip = 1
		int skip = 0;

		// Lower block every few seconds
		if ( (double)(clock() - dropTime) / CLOCKS_PER_SEC >= cursorSpeed ) {
			moveCursor(0,1,cursorBlock,cursorRotation);
			update();
			dropTime = clock();
		}

		// Drop block if time is up
		if ( (double)(clock() - timer) / CLOCKS_PER_SEC >= cursorTime ) {
			nextBlock();
			skip = 1;
		}

		int c;
		timeout(0);    // make getch() non blocking
		c = getch();

		// Read keyboard input until place block
		// ad for left right, ws for rotate, eq to flip
		switch (c) {
			case 'a':
			case KEY_LEFT:
				moveCursor(-1,0,cursorBlock,cursorRotation);
				break;
			case KEY_RIGHT:
			case 'd':
				moveCursor(1,0,cursorBlock,cursorRotation);
				break;
			case KEY_DOWN:
			case 's':
				moveCursor(0,1,cursorBlock,cursorRotation);
				break;
			case 'x':
			case 'e':
				rotateBlock(1,cursorBlock,cursorRotation);
				break;
			case KEY_UP:
			case 'q':
				rotateBlock(5,cursorBlock,cursorRotation);	// 5 is 4 + 1, modulus in rotateBlock will turn into -1
				break;
			case 'c':
			case 'w':
				rotateBlock(2,cursorBlock,cursorRotation);
				break;
			case 'z':
			case 'f':
				hold();
				break;
			case ' ':
				nextBlock();
				break;
			default:
				skip = 1;
				break;
		}
		
		// No not reprint screen if there is no change
		if ( skip == 1 ) {
			mvprintw(HEIGHT-1,WIDTH*2+1,"timer: %f", cursorTime - (double)( clock() - timer) / CLOCKS_PER_SEC );
			continue;
		}

		update();

	} while ( ! checkGameStatus() );

	clear();
	mvprintw(row/2,(col-9)/2,"Game end!");
	mvprintw(row/2+1,(col-21)/2,"You Scored %d points",score);
	getch();

	endwin();

	// Debug
	// for (int i = 0; i < HEIGHT; i++) {
	//	   for (int j = 0; j < WIDTH; j++) {
	//		   printf("%d ", grid[i][j]);
	//	   }
	//	   printf("\n");
	// }

	printf("You Scored %d points!\n",score);

	return(0);
}

// Prints out Tetris Grid
void printGrid () {
	move(0,0);
	for (int i = 0; i < WIDTH*2; i++)
		printw("-");

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			move(i+1,j*2);
			if ( grid[i][j] != 0 ) {
				attron(COLOR_PAIR(grid[i][j]));
				printw("  ");
			}
		}
		printw("\n");
	}
	attroff(COLOR_PAIR(cursorBlock+1));
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

			// Clearing a row gives 5 points
			score += 5;
		}
	}
}

// Checking if tetris board is filled
// return 1 if gameover, 0 if game is continuing
int checkGameStatus () {
	for (int i = 0; i < WIDTH; i++)
		if ( grid[2][i] > 0 )			// Add one spare row to top for buffer
			return 1;

	return 0;
}

// Check if a a block can be put into grid[y][x]
// return 1 if moveCursor failed, 0 if moveCursor is successful
int checkBlock ( int x, int y, int blockID, int rotation ) {
	// Check if block out of bounds
	if ( blockOut(x, y, blockID, rotation) )
		return 1;

	// Check if block collides with grid
	if ( blockCollision(x, y, blockID, rotation) )
		return 1;

	return 0;
}

// Move cursor into tetromino fits
// Imagine that the block is placed ontop of the tetris grid array, with the top left corner of the tetromino 4 by 4 array on x/y of the array
int moveCursor ( int x, int y, int blockID, int rotation ) {
	// Check if block out of bounds
	if ( checkBlock(x + cursorX, y + cursorY, blockID, rotation) )
		return 1;

	cursorX += x;
	cursorY += y;
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
int blockOut ( int x, int y, int blockID, int rotation ) {
	// BlockID that is more than 1 have the same silloute
	int xLeft	= coordinateToX(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][0]);
	int xRight	= coordinateToX(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][1]);
	int yBottom = coordinateToY(blockSilloute[ (blockID <= 1) ? blockID : 2 ][rotation][1]);

	// check if out of bounds
	if ( xLeft + x < 0 || xRight + x >= WIDTH )
		return 1;

	if ( yBottom + y >= HEIGHT )
		return 1;

	return 0;
}

// Check if the tetromino is colliding with the playing field (if the tetromino is "in the ground")
int blockCollision (int x, int y, int blockID, int rotation) {
	// check if hit other objects on the board
	for (int i = 0; i < 4; i++)
		if ( grid[coordinateToY(block[blockID][rotation][i]) + y][coordinateToX(block[blockID][rotation][i]) + x] )
			return 1;

	return 0;
}

// Rotates the block while checking for collision/out of bounds
int rotateBlock (int rotation, int blockID, int currentRotation) {

	int finalRotation = ( currentRotation + rotation ) % 4;

	// Check if block rotation is allowed
	if ( moveCursor(0, 0, blockID, finalRotation) )
		if ( moveCursor(1, 0, blockID, finalRotation) )			// test moving block and right for wallkick
			if ( moveCursor(-1, 0, blockID, finalRotation) )
				return 1;

	return 0;
}

// Write the block defined by cursor variables onto the tetris grid
void putBlock() {
	for (int i = 0; i < 4; i++)
		grid[coordinateToY(block[cursorBlock][cursorRotation][i]) + cursorY][coordinateToX(block[cursorBlock][cursorRotation][i]) + cursorX] = cursorBlock+1;

	// Dropping block adds 1 point
	score++;
}

// Set cursorY to the lowest possible position
// Returns 1 if block cannot be placed here
int shadowBlock (int blockID, int rotation) {
	// Put block out of bounds
	// cursorY = 0;

	// Lower block until block hit grid
	while ( ! moveCursor(0,1,blockID,rotation) )
		cursorY++;

	// Retract Y
	if ( checkBlock(cursorX,cursorY,cursorBlock,cursorRotation) )
		cursorY--;

	return 0;
}

// Generates the bags (bag of 7 blocks)
void generateBag () {
	int i = 1;
	bag[0] = rand()%7;

	for (int i = 1; i < 7; i++) {
		bag[i] = 7;
	}

	while ( i < 7 ) {
		int r = rand()%7;
		int repeat = 0;

		for (int j = 0; j < 7; j++) {
			if ( r == bag[j] ) {
				repeat = 1;
				break;
			}
		}

		if ( ! repeat ) {
			bag[i++] = r;
		}
	}
}

// Show position of block if placed
void printBlock () {
	attron(A_BOLD);
	attron(COLOR_PAIR(cursorBlock+1));

	for (int i = 0; i < 4; i++) {
		move(coordinateToY(block[cursorBlock][cursorRotation][i])+1+cursorY,(coordinateToX(block[cursorBlock][cursorRotation][i])+cursorX)*2);
		printw("  ");
	}

	attroff(COLOR_PAIR(cursorBlock+1));
	attroff(A_BOLD);
}

// Put the current block in hold
int hold () {
	// Don't swap if already used hold
	if ( cursorHold )
		return 1;

	// Check if block can fit
	if ( moveCursor(0, 0, buffer, cursorRotation) )
		if ( moveCursor(1, 0, buffer, cursorRotation) )    // test moving block and right for wallkick
			if ( moveCursor(-1, 0, buffer, cursorRotation) )
				if ( moveCursor(0, -1, buffer, cursorRotation) )
					if ( moveCursor(0, -2, buffer, cursorRotation) )
						return 1;

	// If no block in hold
	if ( buffer == 7 ) {
		buffer = bag[bagIndex];
		bag[bagIndex] = 7;
		bagIndex++;
	} else {	// swap hold with bag[bagIndex]
		int temp = buffer;
		buffer = bag[bagIndex];
		bag[bagIndex] = temp;
	}


	// disable hold after one hold
	// cursorHold = 1;
	// cursorY = 0;
	// timer = clock();

	return 0;
}

// Show the block on hold
void printHold () {
	attron(COLOR_PAIR(buffer+1));

	for (int i = 0; i < 4; i++) {
		move(coordinateToY(block[buffer][0][i])+1,(coordinateToX(block[buffer][0][i])+WIDTH+1)*2);
		printw("  ");
	}

	attroff(COLOR_PAIR(buffer+1));
}

// Quick drop block from cursor position, then place block
// Then prepare for the next block
void nextBlock () {
	shadowBlock(cursorBlock,cursorRotation);
	putBlock();

	filledRow();
	bagIndex++;

	cursorY = 0;
	cursorX = (WIDTH - 1 - (coordinateToX(blockSilloute[ (cursorBlock <= 1) ? cursorBlock : 2 ][cursorRotation][1] - blockSilloute[ (cursorBlock <= 1) ? cursorBlock : 2 ][cursorRotation][0])))/2; // Center

	cursorHold = 0;
	timer = clock();

	if ( cursorTime > 4 )	// minimal time threshold
		cursorTime -= 0.1;	// Less time

	update();
}

void update () {
	// Check if bag is empty, fill bag
	if ( bagIndex == 7 ) {
		generateBag();
		bagIndex = 0;
	}

	// Set current block
	cursorBlock = bag[bagIndex];

	// update cursorSpeed
	cursorSpeed = cursorTime/HEIGHT;

	// Print grid and Print block
	clear();
	printGrid();
	printGhostBlock();
	printBlock();
	printHold();

	// Diagnostics
	mvprintw(HEIGHT-1,WIDTH*2+1,"timer: %f", cursorTime - (double)( clock() - timer) / CLOCKS_PER_SEC );
	mvprintw(HEIGHT,WIDTH*2+1,"Score: %d", score);
	mvprintw(HEIGHT-2,WIDTH*2+1,"cx:%d cy:%d cr:%d cb:%d", cursorX, cursorY, cursorRotation, cursorBlock);
}

// Show position of block if placed
void printGhostBlock () {
	int temp = cursorY;

	shadowBlock(cursorBlock,cursorRotation);
	for (int i = 0; i < 4; i++) {
		move(coordinateToY(block[cursorBlock][cursorRotation][i])+1+cursorY,(coordinateToX(block[cursorBlock][cursorRotation][i])+cursorX)*2);
		printw("[]");
	}

	cursorY = temp;
}
