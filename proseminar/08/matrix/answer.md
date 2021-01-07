# Matrix multiplication

My "optimized" version of the matrix multiplication can be found in [multiplication.c](multiplication.c).
It's based on transposing the right hand Matrix factor, and then tiling the multiplication.
I did try tiling the multiplication without transposition, but that resulted in much worse performance.

I didn't try to implement a divide and conquer method for matrix multiplication.
