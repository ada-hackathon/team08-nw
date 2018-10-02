/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*******************************************************************************
Description:
    OpenCL Wide Memory Read/write Example 
    Description: This is vector addition to demonstrate Wide Memory access of 
    128bit Datawidth using uint4 openCL vector datatype.
*******************************************************************************/

#define get_elem(i, j) mat[i + (j *size)]

/*
    Vector Addition Kernel Implementation using uint4 datatype 
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
kernel
void nw(
        const __global unsigned char *seqA, // Read-Only Vector 1
        const __global unsigned char *seqB, // Read-Only Vector 2
        __global  int *mat,             // Output Result
        int size                            // Size in integer
        )
{
	// Global array
	__local int synch[32];

	// Do stuff ... pls
	int row_id = get_global_id(0) + 1;

	synch[row_id + 1] = 0;

	// Moar stuff ...
	for(unsigned int col_id = 1; col_id < size; col_id++)
	{
		if(row_id != 1)
			while(synch[row_id - 1] < col_id);

		int col_left  = col_id - 1;
		int row_left  = row_id - 0;

		int col_top   = col_id - 0;
		int row_top   = row_id - 1;

		int col_tleft = col_id - 1;
		int row_tleft = row_id - 1;

		int m = seqB[row_id - 1] == seqA[col_id - 1] ? 1 : -1;

		int l  = get_elem(row_left , col_left ) - 1;
		int t  = get_elem(row_top  , col_top  ) - 1;
		int tl = get_elem(row_tleft, col_tleft) + m;

		int v  = l;

		if(tl > v)
			v = tl;
		if(t > v)
			v = t;

		mat[row_id + (col_id * size)] = v;

		// Unlock other workers
		synch[row_id] = col_id;
	}
}
