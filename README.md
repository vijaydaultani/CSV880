## Parallel Computation using MPI

During my masters course I completed the below two assignments as a course project for CSV 880: Special Module in Parallel Computation.

### Assignment 1
> Averaging based computations on a matrix using block-cyclic distribution. 

- **Goal** is to write an MPI program to perform the template (averaging) based computations on a matrix `A` specified as follows:

	The program will read a matrix `A` of size `m x n` comprising of floating point numbers from inpFile. The program should run on `p x q` ranks logically arranged as a 2-D grid. The matrix should be distributed on the ranks in a block-cyclic distribution with block size `s x s`. The program will perform `l` iterations: 

	![equation][Equation_embedding]


- **Details**: [here][Assignment1_doc]

### Assignment 2
> Broadcasting on a 2D Torus using a single spanning tree. 

- It performs a broadcast of a 2D Torus using a single spanning tree. The code reports the total packets received by every node and the maximum congestion on any link. In initial version of the code, each node receives 1 packet and congestion=1. **Goal** was to modify the code to perform the broadcast using 4 non-contending spanning trees. Each node should receive 4 packets, and congestion should remain 1.

- **Details**: [here][Assignment2_doc]


[Assignment1_doc]: https://github.com/vijaydaultani/CSV880/blob/master/assignment%201/docs/Assignment1.pdf

[Assignment2_doc]: https://github.com/vijaydaultani/CSV880/blob/master/assignment%202/docs/Assignment2.pdf

[Equation_embedding]: http://latex.codecogs.com/gif.latex?A[i][j]%20=%20\frac{a%20.%20A[i-1][j]%20+%20b%20.%20A[i][j-1]%20+%20c%20.%20A[i][j]%20+%20d%20.%20A[i][j+1]%20+%20e%20.%20A[i+1][j]%20}{a%20+%20b%20+%20c%20+%20d%20+%20e}