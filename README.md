## Parallel Computation using MPI

During my masters course I completed the below two assignments as a course project for CSV 880: Special Module in Parallel Computation.

### Assignment 1
> Averaging based computations on a matrix using block-cyclic distribution. 

- Goal is to write an MPI program to perform the template (averaging) based computations on a matrix `A` specified as follows:

	The program will read a matrix `A` of size `m x n` comprising of floating point numbers from inpFile. The program should run on `p x q` ranks logically arranged as a 2-D grid. The matrix should be distributed on the ranks in a block-cyclic distribution with block size `s x s`. The program will perform `l` iterations: 
	
	<img src="http://www.sciweavers.org/tex2img.php?eq=A%5Bi%5D%5Bj%5D%20%3D%20%20%5Cfrac%7Ba%20.%20A%5Bi-1%5D%5Bj%5D%20%2B%20b%20.%20A%5Bi%5D%5Bj-1%5D%20%2B%20c%20.%20A%5Bi%5D%5Bj%5D%20%2B%20d%20.%20A%5Bi%5D%5Bj%2B1%5D%20%2B%20e%20.%20A%5Bi%2B1%5D%5Bj%5D%20%7D%7Ba%20%2B%20b%20%2B%20c%20%2B%20d%20%2B%20e%7D%20&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0" align="center" border="0" alt="A[i][j] =  \frac{a . A[i-1][j] + b . A[i][j-1] + c . A[i][j] + d . A[i][j+1] + e . A[i+1][j] }{a + b + c + d + e} " width="640" height="44" />

	- Details: [here][Assignment1_doc]

### Assignment 2
> Broadcasting on a 2D Torus using a single spanning tree. 

- It performs a broadcast of a 2D Torus using a single spanning tree. The code reports the total packets received by every node and the maximum congestion on any link. In initial version of the code, each node receives 1 packet and congestion=1. Goal was to modify the code to perform the broadcast using 4 non-contending spanning trees. Each node should receive 4 packets, and congestion should remain 1.
	- Details: [here][Assignment2_doc]


[Assignment1_doc]: https://github.com/vijaydaultani/CSV880/blob/master/assignment%201/docs/Assignment1.pdf

[Assignment2_doc]: https://github.com/vijaydaultani/CSV880/blob/master/assignment%202/docs/Assignment2.pdf

[Equation_embedding]: http://www.sciweavers.org/tex2img.php?eq=A%5Bi%5D%5Bj%5D%20%3D%20%20%5Cfrac%7Ba%20.%20A%5Bi-1%5D%5Bj%5D%20%2B%20b%20.%20A%5Bi%5D%5Bj-1%5D%20%2B%20c%20.%20A%5Bi%5D%5Bj%5D%20%2B%20d%20.%20A%5Bi%5D%5Bj%2B1%5D%20%2B%20e%20.%20A%5Bi%2B1%5D%5Bj%5D%20%7D%7Ba%20%2B%20b%20%2B%20c%20%2B%20d%20%2B%20e%7D%20&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0

[Equation_website]: # (<img src="http://www.sciweavers.org/tex2img.php?eq=A%5Bi%5D%5Bj%5D%20%3D%20%20%5Cfrac%7Ba%20.%20A%5Bi-1%5D%5Bj%5D%20%2B%20b%20.%20A%5Bi%5D%5Bj-1%5D%20%2B%20c%20.%20A%5Bi%5D%5Bj%5D%20%2B%20d%20.%20A%5Bi%5D%5Bj%2B1%5D%20%2B%20e%20.%20A%5Bi%2B1%5D%5Bj%5D%20%7D%7Ba%20%2B%20b%20%2B%20c%20%2B%20d%20%2B%20e%7D%20&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0" align="center" border="0" alt="A[i][j] =  \frac{a . A[i-1][j] + b . A[i][j-1] + c . A[i][j] + d . A[i][j+1] + e . A[i+1][j] }{a + b + c + d + e} " width="640" height="44" />)
