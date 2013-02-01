/*     Median of 100 element integer array

This program finds the median value in a 100 element integer array.

1 February 2013	               Colin Bookman				*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
   int	Nums[100];
   int	NumNums, Median;
   int  Load_Mem(char *, int *);

   /* Check if we have an input and output file name */
   if (argc != 3) {
     printf("usage: ./P1-2 valuefile\n");
     exit(1);
   }

   /* Try to load the Input file contents */
   NumNums = Load_Mem(argv[1], Nums);
   if (NumNums != 100) {
      printf("valuefiles must contain 100 entries\n");
      exit(1);
   }
   /* Try to open the output file */
   FILE *output = fopen(argv[2], "w");
   if(!output) {
     printf("ERROR: Could not write to: %s\n", argv[2]);
     exit(1);
   }
   /* Bubble Sort */
   int i, j, temp;
   for(i=0; i<(100-1); i++) {
    for(j=0; j<(100-1-i); j++) {
      if(Nums[j]>Nums[j+1]) {
        temp = Nums[j+1];
        Nums[j+1]=Nums[j];
        Nums[j] = temp;
      }
    }
   }
  
   /* Get Median - As even number its the average if Nums[49]+Nums[50] */
   Median = (Nums[49]+Nums[50])/2;

   /* Write median to file */
   fprintf(output, "The median of the array is %d", Median);
   fclose(output);

   printf("The median of the array is %d\n", Median);
   exit(0);
}

/* This routine loads in up to 100 newline delimited integers from
a named file in the local directory. The values are placed in the
passed integer array. The number of input integers is returned. */

int Load_Mem(char *InputFileName, int IntArray[]) {
   int	N, Addr, Value, NumVals;
   FILE	*FP;

   FP = fopen(InputFileName, "r");
   if (FP == NULL) {
      printf("%s could not be opened; check the filename\n", InputFileName);
      return 0;
   } else {
      for (N=0; N < 100; N++) {
         NumVals = fscanf(FP, "%d: %d", &Addr, &Value);
         if (NumVals == 2)
            IntArray[N] = Value;
         else
            break;
      }
      fclose(FP);
      return N;
   }
}
