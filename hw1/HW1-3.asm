# 25 January 2013	               <Your Name Goes Here>
#
# This program computes the union of two ten element sets of integers.

.data
SetA:	.word	243, -459, 826, -535, 264, 698, 268, 281, 921, 886
SetB:	.word	864, 281, 781, 698, -535, 128, 886, -336, 790, 826
SetC:   .alloc  20

.text
# write your code here...
addi $4, $0, 0      #B's ind = 0 (i)
addi $5, $0, 0      #A's ind = 0 (j)
addi $16, $0, 0     #C ind = 0   (k)
addi $18, $0, 4     #address multiplier
addi $22, $0, 0     #0 variable
addi $23, $0, 40    #40, number of adds to do
j loopB

#Iterate through set B
# $8 = B's temp memory addres - valid numbers are 0-36
# $6 = B[i]
# $4 = i
# $1 = i*4<40 (t/f) (if == 0, then stop looping)

loopB:  mult  $4, $18       #Program Addr
        mflo  $8            #store B addr in temp
        lw    $6, SetB($8)  #load next B
        sltu  $1, $8, $23   #if mem addr < 40, $1=true
        addi  $4, $4,  1    #increment bInd
        bne   $1, $22, loopA #if above is true, then loop a
        j addA              #done iterating through b, add all of A to SetC 
        
        #iterate through set A
        # $5 = j
        # $7 = A[j]
        # $9 = A's temp memory addres
        loopA:  mult  $5, $18         #Prog Addr
                mflo  $9              #store A addr in temp
                lw    $7, SetA($9)    #load next A
                sltu  $1, $9, $23     #if mem addr < 40, $1=true
                addi  $5, $5, 1       #increment aInd
                beq   $1, $22, uniqueB #if mem addr > 40, stop loopA, we found unqiue... 
                beq   $7, $6, resetA  #a[j]==n[i] ... we didn't find a unique value, stop the 'a' loop
                j loopA               #iterate A again

        resetA:     addi $5, $0, 0    #reset j =0
                    j loopB           #get next B

        uniqueB:    addi $5, $0, 0     #reset j = 0
                    mult $16, $18      #get 16's mem addr (k*4)
                    mflo $10           #load 16's mem addr in $10
                    sw   $6, SetC($10) #store B[i]->c[k]
                    addi $16, $16, 1   #k++
                    j loopB           #go and reloop b
# Add the entire SetA to C
addA: addi $5, $0, 0     #reset j=0
      j loopAddA         #add all of A
      loopAddA:   mult  $5, $18       #A's mem location
                  mflo  $9            #store A addr in temp
                  lw    $7, SetA($9)  #load next A
                  sltu  $1, $9, $23   #if meme addr < 40, $1 = true
                  addi  $5, $5, 1     #increment j (a Ind)
                  beq   $1, $22, exit #if mem addr of A < 40, we've added all of A
                  mult  $16, $18      #get SetC's current Mem location
                  mflo  $10           #$10 = SetC's mem Location
                  sw    $7, SetC($10)  #Store A into Set C
                  addi  $16, $16, 1   #k++
                  j loopAddA

exit: jr $31
