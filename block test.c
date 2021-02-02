#include <stdlib.h>
#include <stdio.h>

int main () {
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
	
	// array[type][rotation][block position]
	int array[7][4][4] = {
		 {{4,5,6,7},{2,6,10,14},{8,9,10,11},{1,5,9,13},},	// I block
		 {{0,4,5,6},{1,2,5,9},{4,5,6,10},{1,5,8,9},},		// L block
		 {{2,4,5,6},{1,5,9,10},{4,5,6,8},{0,1,5,9},},		// flipped L block
		 {{1,2,5,6},{1,2,5,6},{1,2,5,6},{1,2,5,6},},		// Square block
		 {{1,2,4,5},{1,5,6,10},{5,6,8,9},{0,4,5,9},},		// Flipped Z block
		 {{1,4,5,6},{1,5,6,9},{4,5,6,9},{1,4,5,9},},		// T block
		 {{0,1,5,6},{2,5,6,9},{4,5,9,10},{1,4,5,8},},		// Z block
	};


	int newArr[4][4] = {
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
	};

	for (int x = 0; x < 7; x++) {
		for (int y = 0; y < 4; y++) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					newArr[i][j] = 0;
				}
			}


			for (int i = 0; i < 4; i++) {
				newArr[ array[x][y][i]/4 ][ array[x][y][i]%4 ] = 1;
			}

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					printf("%d ", newArr[i][j]);
				}
				printf("\n");
			}

			printf("\n");
		}
	}

}
