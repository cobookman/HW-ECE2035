/*************************************************
 *         Author:  Colin Bookman                *
 *       Class: ECE 2035, Spring 2013            *
 *                                               *
 * This program solves the puzzle by rotating    *
 * tiles clockwise.  Each peice is selected from *
 * top left to bottom right.  And we check for a *
 * match left, and match up                      *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>

/* Protypes */
int getIndex(int row, int col, int dir, int * numCols); /* row, col, dir #[0(N), 1(S),2(E),3(W)]*/
int loopSolve(int * puzzle, int col, int row, int * numCols, int * numRows, int * solution);
int checkTile(int row, int col, int * numCols, int * puzzle, int * solution);
void rotate90(int row, int col, int *puzzle, int *numCols);

/* Consts */
#define DONE       1;
#define CELL_ITEMS 4
#define VALID      1;
#define INVALID    0;
#define ERROR     -1;

void solver(int row_size, int column_size, int num_color, int *packed_puzzle, int * solution) {
    int i, j;
    int *puzzle;
    /* In the following example, the puzzle is a row*(column*4) array, where the puzzle is laid out
     in the following order: N, E, S, W of (1,1), N, E, S, W of (1, 2), ... , the next row, ... */
    
    /* It's perfectly fine if you choose a different data layout. */
    puzzle = (int *)malloc( sizeof(int) * row_size * column_size * 4);

    for(i=0;i<row_size;i++) {
        for(j=0;j<column_size;j++) {
            puzzle[(i*column_size+j)*4+1] = (packed_puzzle[i*column_size+j] & 0xff) ;
            puzzle[(i*column_size+j)*4+2] = (packed_puzzle[i*column_size+j] & 0xff00) >> 8;
            puzzle[(i*column_size+j)*4+3] = (packed_puzzle[i*column_size+j] & 0xff0000) >> 16;
            puzzle[(i*column_size+j)*4] = (packed_puzzle[i*column_size+j] & 0xff000000) >> 24;
        }
    }
    /* Launch the recursive solve */
    loopSolve(puzzle, 0, 0, &column_size, &row_size, solution);
    /* Finished Sorting */
}


int loopSolve(int * puzzle, int col, int row, int * numCols, int * numRows, int * solution) {
   while(row!=(*numRows)) {

       /*         !!!!!!!!!IMPORTANT!!!!!!!!!
       if solution[row*col]>3, a FALSE is returned.  
       This is done to tell theq program that an cell to the right/top 
       needs to be rotated as this one has  done a full rotation already.   */
   int isMatched = checkTile(row,col, numCols, puzzle,solution);
   if(!isMatched) { /*ERROR, go back a cell*/
       /* send error if we're at the starting location*/
        if(col==0 && row==0) { return ERROR;  }

       /* Reset Solution, as the cell was rotated 360 degrees*/
          solution[col + row*(*numCols)]=0;
          
       /* Check if we need to go up a row, or back a cell */
        if(col==0) { col=((*numCols)-1); --row; } else { --col; }

    /* Go to next cell*/
    } else { 
        if((col+1)==(*numCols)) { col=0; ++row; } else { ++col; }
    }
  }
  return DONE;
}
/**********************************************
 * Checks to the left, and up if colors match *
 *    - rotates, and returns a true/false     *
 **********************************************/
int checkTile(int row, int col, int * numCols, int * puzzle, int * solution) {
    while(solution[row*(*numCols)+col]<4) { /*Don't rotate tile more than 360 */
        int matchTop  = INVALID; 
        int matchLeft = INVALID;

      /* Rotate 90 Clockwise by default */
        rotate90(row, col, puzzle, numCols);
        ++solution[row*(*numCols) + col];

      /* Check if the top has a match */
        if(row==0) { 
            matchTop=1; 
        } else if(puzzle[getIndex(row, col, 0, numCols)] == puzzle[getIndex((row-1), col, 2, numCols)]) {
            matchTop=1;
        }

      /* Check if the left has a match */
        if(col==0) {
            matchLeft=1;
        } else if(puzzle[getIndex(row, col, 3, numCols)] == puzzle[getIndex(row, (col-1), 1, numCols)]) {
            matchLeft=1;
        }
      /* Check if its a match, else rotate */
        if(matchTop==1 && matchLeft==1) { 
            return VALID; 
        }
    }
    return INVALID; /* Rotated around 4 times, invalid */
}

/*********************************
*  rotate 90                     *
*    rotates puzzle 90 Clockwise *
**********************************/
void rotate90(int row, int col, int * puzzle, int * numCols) {
    int north = getIndex(row, col, 0, numCols);
    int temp1, temp2;

    /* North = West */
    temp1 = puzzle[north]; 
    puzzle[north]   = puzzle[north+3];

    /*East = North*/
    temp2 = puzzle[north+1];
    puzzle[north+1] = temp1;
    
    /* South = East */
    temp1 = puzzle[north+2];
    puzzle[north+2]=temp2;

    /* West = South */
    puzzle[north+3] = temp1;
}

/*************************************************************
 * getIndex(row, col, dir, numCols)                          *
 *      -Returns the correct index of puzzle[...]   *
 *       Dir: 0 -> North                                     *
 *            1 -> East                                      *
 *            2 -> South                                     *
 *            3 -> West                                      *
 *************************************************************/
int getIndex(int row, int col, int dir, int * numCols) {
    return (row*CELL_ITEMS*(*numCols)) + (col*CELL_ITEMS)+dir;
}
