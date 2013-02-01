#     Median of 100 element integer array
#
# This routine finds the median of a 100 element array.
# The result is returned in $2.
#
#  1 February 2013                       Colin Bookman

.data
Array:  .alloc	100

.text

Median:		addi	$1, $0, Array		# point to array base
		swi	542			# generate random numbers


#########################################################
######            Bubble Sort the array            ######
######                  R2 = i                     ######
######                  R3 = j                     ######
######                R6 = True (1)                ######
######                  R7 = 4                     ######
######                  R8 = 99                    ######
######           R4,R5, R6 R14 for temps           ######
#########################################################
    add $2, $0, $0    #init i=0
    addi $6, $0, 1    #R6 = True
    addi $7, $0, 4    #R7 = 4
    addi $8, $0, 99   #R8 = 99
outerLoop:      addi $3, $0, 0           # re-init that j = 0;
                sltu $4, $2, $8          # if(i<99) { $r4=1 } else { r4 = 0 }
                beq $6, $4, innerLoop    # if(r4 is true) { go to the inner loop }
                ### SORTED, Find Median ###
                j findMedian    
          innerLoop: sub  $4, $8, $2    # R4 = 99-i
                     sltu $4, $3, $4    # if(j<(99-i) { R4 = 1 } else { R4 = 0 }
                     beq  $0, $4, incrementI # !(j<(99-i)), inner loop is done, increment i
                        ## LOOP Opps ##
                        #get mem addr of j (13)
                        mult $3, $7
                        mflo $13             #R13 = &(j)
                        #Check if Arr[j] > Arr[j+1]
                        lw   $4, Array($13)  #R4 = Arr[j]
                        addi $14, $13, 4     #R14 = &(j+1)
                        lw   $15, Array($14) #R15 = Arr[j+1]
                        sltu $5, $15, $4     #if(Arr[j]>Arr[j+1]) { R5 = 1 } else { R5 = 0 }
                        addi $3, $3, 1       #j++
                        beq  $0, $5, innerLoop #If arr[j]<Arr[j+1], keep looping
                          #Arr[j]>Arr[j+1], swap values
                          sw $4, Array($14)         # Arr[j+1]=Arr[j]
                          sw $15, Array($13)        # Arr[j] = Arr[j+1]
                        j innerLoop         

          incrementI: addi $2, $2, 1    #i++
                      j outerLoop       #check loop conditions

#########################################################
##### Find the median of a 100 element sorted Array #####     
#####     - Pass a sorted array by name: 'Array'    #####
#####       -Reeturns in reg2                       #####
##### !important: R2, R3, R4 overwritten            #####
#########################################################
findMedian: addi $2, $0, 49     
            addi $3, $0, 50
            addi $4, $0, 4  #init a 4 for pointers
            mult $2, $4     # 49*4
            mflo $2
            mult $3, $4     # 50*4
            mflo $3
            lw   $2, Array($2) #R2 = Array(49)
            lw   $3, Array($3) #R3 = Array(50)
            add  $2, $2, $3    #R2 = Array(49) + Array(50)
            addi $3, $0, 2     #R3 = 2
            sra $2, $2, 1   #R2 = FLOOR([Array(49) + Array(50)]/2)

jr	$31			# otherwise return to caller
