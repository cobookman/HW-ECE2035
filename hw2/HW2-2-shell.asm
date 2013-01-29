#     Median of 100 element integer array
#
# This routine finds the median of a 100 element array.
# The result is returned in $2.
#
#  1 February 2013                       <your name here>

.data
Array:  .alloc	100

.text

Median:		addi	$1, $0, Array		# point to array base
		swi	542			# generate random numbers

# your code goes here

		jr	$31			# otherwise return to caller
