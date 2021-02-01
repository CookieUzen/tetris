#include <stdio.h>

#define HEIGHT 5
#define WIDTH 5

void printGrid ( int grid[HEIGHT][WIDTH] ) {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d, ", grid[i][j]);
		}
		printf("\n");
	}
}

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

int checkGameStatus (int grid[HEIGHT][WIDTH] ) {
	for (int i = 0; i < WIDTH; i++)
		if ( grid[0][i] == 1 ) 
			return 1;

	return 0;
}

typedef struct block {
	int block1[3][3];
	int lowest1[3];
	int block2[3][3];
	int lowest2[3];
	int block3[3][3];
	int lowest3[3];
	int block4[3][3];
	int lowest4[3];
} block;



int main () {

	/*
	// Initialize grid
	static int grid[HEIGHT][WIDTH];
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			grid[i][j] = 0;
	*/

	int grid[HEIGHT][WIDTH] = {
		{1,0,0,0,0},
		{1,1,0,0,0},
		{1,1,1,0,0},
		{1,1,1,1,0},
		{1,1,1,1,1},
	};

	filledRow(grid);
	printGrid(grid);
	if (checkGameStatus(grid)) {
		printf("Game Over\n");
		return 0;
	}

	return 0;
}
