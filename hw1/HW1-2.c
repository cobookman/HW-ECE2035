/* 25 January 2013	               Colin Bookman

ECE 2035 Homework 1-2

This is the only file that should be modified for the C implementation
of Homework 1.

This program computes the union of two sets of ten integers,
SetA and SetB, prints the elements in the union and returns
and prints the size of the union.
*/

#include <stdio.h>
#include <stdlib.h>

/* Macros */
#define arr_size(array) sizeof(array)/sizeof(array[0]) /* macro to find the size of the array */

/* Global Variables */
int SetA[] = {5, -19, 8, 26, 3, 6, -9, 17, 60, -63};
int SetB[] = {8, 11, -63, 18, 26, 17, 25, 12, -9, 60};

/* prototypes */
void BubbleSort(int a[], int array_size);

int main() {
  int Size;
  int Set_Union();
  Size = Set_Union();
  printf("Union Set Size: %d\n", Size);
  return 0;
}

int Set_Union() {
  /* Get length of arrays */
  int lenA = arr_size(SetA);
  int lenB = arr_size(SetB);
  
  /* Sort arrays */
  BubbleSort(SetA, lenA); //Sort SetA
  BubbleSort(SetB, lenB); //Sort SetB

  /* Get the union */
  int i = 0; int j = 0; int unionSize = 0;
  while(i < lenA && j < lenB) {
    if(SetA[i] < SetB[j]) {
        printf(" %d ", SetA[i++]);
      } else if(SetA[i] > SetB[j]) {
          printf(" %d ", SetB[j++]);
      } else {
          printf(" %d ", SetB[j++]);
          i++;
      }
      unionSize++;
  }
  
  /* Print remaining elements of larger array */
  while(i < lenA) { printf(" %d ", SetA[i++]); unionSize++; }
  while(j < lenB) { printf(" %d ", SetB[j++]); unionSize++; }

  /* return the union size */
  return unionSize;
} 

void BubbleSort(int a[], int array_size) {
  int i, j, temp;
  for (i = 0; i < (array_size - 1); ++i) {
    for (j = 0; j < array_size - 1 - i; ++j ) {
      if (a[j] > a[j+1]) {
        temp = a[j+1];    
        a[j+1] = a[j];
        a[j] = temp;
      }
    }
  }
}
