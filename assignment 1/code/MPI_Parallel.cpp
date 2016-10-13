#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "mpi.h"
#define MASTER 0
#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

using namespace std;

int getGlobalRank(int array_elem_i, int array_elem_j, int p, int q, int s, MPI_Comm comm_2d);

//inpFile outFile m : matrix row n : matrix col p : process row q : process col
//s : size of slack of s*s l : number of iterations 
//Five scalar variables a b c d e

float** convert1Dto2Darray(float* local1DBuffer, int localBufferSize, int P, int Q, int s)
{
	float** array = (float **) malloc( P * s * sizeof( float * ) );
	int i;

	if(array != NULL)
	{
		array[0] = (float*) malloc (P * s * Q * s * sizeof ( float ) );
		if(array[0] != NULL)
		{
			for( i = 1; i < (P * s); i++)
				array[i] = array[0] + (i * Q * s);
		}
		else 
		{
			free( array );
			array = NULL;
		}
	}

	//a 2D array is created now we need to copy the entire 1D matrix to 2D matrix
	int local1Dindex = 0;
	for(int i = 0 ; i < (P * s); i++)
	{
		for(int j = 0 ; j < (Q * s); j++)
		{
			array[i][j] = local1DBuffer[local1Dindex];
			local1Dindex++;
		}
	}
	return array;
}

float** alloc_2d_float(int m, int n)
{
	float **array = (float **) malloc( m * sizeof( float * ) );
	int i;

	if(array != NULL)
	{
		array[0] = (float*) malloc( m * n * sizeof( float ) );
		if( array[0] != NULL )
		{
			for(i = 1; i < n; i++)
				array[i] = array[0] + i * n;
		}
		else
		{
			free( array );
			array = NULL;
		}
	}
	return array;
}

int** alloc_2d_int(int m, int n)
{
	int **array = (int **) malloc( m * sizeof( int * ) );
	int i;

	if(array != NULL)
	{
		array[0] = (int*) malloc( m * n * sizeof( int ) );
		if( array[0] != NULL )
		{
			for(i = 1; i < n; i++)
				array[i] = array[0] + i * n;
		}
		else
		{
			free( array );
			array = NULL;
		}
	}
	return array;
}

void generateITable(int** itable, int row_divison, int col_division, int p, int q, int s, MPI_Comm comm_2d)
{
	//itable is of size P+2, Q+2 and therefore index is from 0..P+1, 0..Q+1
	//First Row and Last Row
	for(int j = 0 ; j <= col_division+1; j++)
	{
		itable[0][j] = -1;
		itable[row_divison+1][j] = -1;
	}

	//First Coloumn and Last Coloumn
	for(int i = 0; i <= row_divison+1; i++)
	{
		itable[i][0] = -1;
		itable[i][col_division+1] = -1;
	}

	int global_i = 0;
	int global_j = 0;

	for(int i = 1; i <= row_divison; i++)
	{
		for(int j = 1; j <= col_division; j++)
		{
			global_i = (i-1) * s;
			global_j = (j-1) * s;

			itable[i][j] = getGlobalRank(global_i, global_j, p, q, s, comm_2d);
		}
	}
}

void free_2d_float_array( float **array)
{
	if(array != NULL)
	{
		free(array[0]);
		free(array);
	}
	return;
}

void read_array(float** array, int m, int n, int paddedRow, int paddedCol, char* inputFileName)
{
	FILE* filePtr = fopen(inputFileName, "r");
	float data=0.0;
	int i=0, j=0;

	if(filePtr != NULL)
	{
		for(i = 0; i < m; i++)
		{
			for(j = 0; j < n; j++)
			{
				fscanf(filePtr, "%f", &data);
				array[i][j] = data;
			}
			//Padd for extra coloumn
			for( ; j < n + paddedCol; j++)
			{
				array[i][j] = -1;
			}
		}
		//Padd for extra full rows
		for( ; i < m + paddedRow; i++)
		{
			for(j = 0; j < n + paddedCol; j++)
			{
				array[i][j] = -1;
			}
		}
		fclose(filePtr);
	}
	else
	{
		printf("Input File Does Not Open properly\n");
		exit(1);
	}
	return;
}

void print_array(float** array, int M, int N)
{
	for(int i = 0; i < M; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%6.2f ", array[i][j]);
		}
		printf("\n");
	}
	return;
}

void print_array(int** array, int M, int N)
{
	for(int i = 0; i < M; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%2d ", array[i][j]);
		}
		printf("\n");
	}
	return;
}

void print1DArray(float* array, int size)
{
	for(int i = 0 ; i < size; i++)
	{
		printf("%4.2f ", array[i]);
	}
	printf("\n");
}

int getGlobalRank(int array_elem_i, int array_elem_j, int p, int q, int s, MPI_Comm comm_2d)
{
	int mapped_process_i = (( array_elem_i/s ) % p );
	int mapped_process_j = (( array_elem_j/s ) % q );
	int rank = 0;
	int mapped_process_coords[2] = {mapped_process_i, mapped_process_j};
	MPI_Cart_rank(comm_2d, mapped_process_coords, &rank);
	return rank;
}


void print_array_with_process_no(float** array, int M, int N, int p, int q, int s, MPI_Comm comm_2d)
{
	for(int i = 0; i < M; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%6.2f : %d ", array[i][j], getGlobalRank(i, j, p, q, s, comm_2d));
		}
		printf("\n");
	}
}

void createLocalArrays(float** whole_array, float (*ptrs[]), int M, int N, int p, int q, int s, int size, MPI_Comm comm_2d)
{
	//This array represents the current index in each processes current local subarray
	int *index = (int*) calloc (size, sizeof(int));

	for(int i = 0 ; i < M ; i++)
	{
		for(int j = 0 ; j < N; j++)
		{
			int getMappedProcess = getGlobalRank(i, j, p, q, s, comm_2d);
			int indexCount = index[getMappedProcess];
			ptrs[getMappedProcess][indexCount] = whole_array[i][j];
			index[getMappedProcess] = indexCount + 1;
		}
	}
}

bool isOnBoundary(int local_i, int local_j, int s)
{
	if((local_i % s == 0) || (local_i % s == s-1) || (local_j % s == 0) || (local_j % s == s-1))
		return true;
	else 
		return false;
}

void local2DtoGlobal2D(int local_i, int local_j, int* global_i, int* global_j, int p, int q, int s, int process_i, int process_j)
{
	*global_i = (local_i / s) * ( p * s ) + process_i * s + (local_i % s);
	*global_j = (local_j / s) * ( q * s ) + process_j * s + (local_j % s);
}

void global2DtoLocal2D(int* local_i, int* local_j, int global_i, int global_j, int p, int q, int s)
{
	*local_i = (global_i / (p * s)) * s + (global_i % s);
	*local_j = (global_j / (q * s)) * s + (global_j % s);
}

float doAveragingOperation(float elem, float up, float left, float right, float down, float a, float b, float c, float d, float e)
{
	if (up == -1) 	{ up = 0;    a = 0; }
	if (left == -1) { left = 0;  b = 0; }
	if (right == -1){ right = 0; d = 0; }
	if (down == -1) { down = 0;  e = 0; }

	float numerator = (a * up) + (b * left) + (c * elem) + (d * right) + (e * down);
	float denominator = (a + b + c + d + e);

	float averagedElement = 0;
	if(numerator != 0)
		averagedElement = numerator / denominator;
	else 
		averagedElement = 0;

	return averagedElement;
}

bool check_if_out_of_matrix(int global_i, int global_j, int M, int N)
{
	if ((global_i < 0) || (global_i > (M-1)) || (global_j < 0) || (global_j > (N-1)))
		return true;
	else
		return false;
}

float get_ith_jth_from_1D(float* array, int i, int j, int P, int s)
{
	return array[(i * P * s) + j];
}

void copy2DBufferForMeTo2DBufferforOthers(float** local2DBufferForMe, float** local2DBufferForOthers, int P, int Q, int s)
{
	for(int i = 0; i < P * s; i++)
	{
		for(int j = 0 ; j < Q * s; j++)
			{
				local2DBufferForOthers[i][j] = local2DBufferForMe[i][j];
			}
	}
	return;
}

void print_array_with_local_global_info(float** array, int M, int N, int p, int q, int s, int process_i, int process_j)
{
	int global_i = 0;
	int global_j = 0;
	int local_i = 0;
	int local_j = 0;
	for(int i = 0; i < M; i++)
	{
		for(int j = 0; j < N; j++)
		{
			local2DtoGlobal2D(i, j, &global_i, &global_j, p, q, s, process_i, process_j);
			global2DtoLocal2D(&local_i, &local_j, global_i, global_j, p, q, s);
			//printf("%6.2f, [%d : %d], [%d : %d], [%d : %d] ** ", array[i][j], i, j, local_i, local_j, global_i, global_j);
			printf("%6.2f, [%d : %d] ", array[i][j], i, j);
		}
		printf("\n");
	}
	return;
}

void convert1Dto2DarrayWithoutAllocating(float* local1DBuffer, float** local2DBufferForMe, int P, int Q, int s)
{
	int oneDIndex = 0;
	for(int i = 0 ; i < P * s; i++)
	{
		for(int j = 0; j < Q * s; j++)
		{
			local2DBufferForMe[i][j] = local1DBuffer[oneDIndex];
			oneDIndex++;
		}
	}
	return;
}

void get_up_send(float** local2DBufferForOthers, float* up_send, int I, int J, int s)
{
	int index = 0;

	for(int j = J * s; j < ((J * s) + s); j++)
	{
		up_send[index] = local2DBufferForOthers[I][j];
		index++;
	}
}

void get_left_send(float** local2DBufferForOthers, float* left_send, int I, int J, int s)
{
	int index = 0;

	for(int i = I * s; i < ((I * s) + s); i++)
	{
		left_send[index] = local2DBufferForOthers[i][J];
		index++;
	}
}

void get_right_send(float** local2DBufferForOthers, float* right_send, int I, int J, int s)
{
	int index = 0;

	for(int i = I * s; i < ((I * s) + s); i++)
	{
		right_send[index] = local2DBufferForOthers[i][J+s-1];
		index++;
	}
}

void get_down_send(float** local2DBufferForOthers, float* down_send, int I, int J, int s)
{
	int index = 0;

	for(int j = J * s; j < ((J * s) + s); j++)
	{
		down_send[index] = local2DBufferForOthers[I+s-1][j];
		index++;
	}
}

void doAveragingIteration(float** local2DBufferForMe, float** local2DBufferForOthers, int** itable, int M, int N, int P, int Q, int p, int q, int s, 
	float a, float b, float c, float d, float e, int l, MPI_Comm comm_2d)
{
	int global_element_own[2] = {0};
	int global_element_neighbour_up[2] = {0};
	int global_element_neighbour_left[2] = {0};
	int global_element_neighbour_right[2] = {0};
	int global_element_neighbour_down[2] = {0};

	int own_process_rank = 0;
	MPI_Comm_rank(comm_2d, &own_process_rank);
	int up_process_rank = 0;
	int left_process_rank = 0;
	int right_process_rank = 0;
	int down_process_rank = 0;

	int mycoords[2] ={0};
	MPI_Cart_coords(comm_2d, own_process_rank, 2, mycoords);

	float own_element = 0;
	float up_element = 0;
	float left_element = 0;
	float right_element = 0;
	float down_element = 0;

	float* up_send = (float*) calloc(s, sizeof( float ));
	float* left_send = (float*) calloc(s, sizeof( float ));
	float* right_send = (float*) calloc(s, sizeof( float ));
	float* down_send = (float*) calloc(s, sizeof( float ));

	float* up_recv = (float*) calloc(s, sizeof( float ));	
	float* left_recv = (float*) calloc(s, sizeof( float ));
	float* right_recv = (float*) calloc(s, sizeof( float ));
	float* down_recv = (float*) calloc(s, sizeof( float ));

	MPI_Request send_request[4] = {MPI_REQUEST_NULL,MPI_REQUEST_NULL,MPI_REQUEST_NULL,MPI_REQUEST_NULL};
	MPI_Request recv_request[4] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL};
	MPI_Status send_status[4];
	MPI_Status recv_status[4];

	/*printf("Process : %d local2DBufferforothers\n", own_process_rank);
	print_array(local2DBufferForOthers, P * s, Q * s);
	printf("Process : %d local2DBufferforMe\n", own_process_rank);
	print_array(local2DBufferForMe, P * s, Q * s);
	*/
	for(int itr = 0 ; itr < l; itr++)
	{
		for(int I = 0 ; I < P; I++)
		{
			for(int J = 0 ; J < Q; J++)
			{

				int global_i, global_j;
				local2DtoGlobal2D(I * s, J * s, &global_i, &global_j, p, q, s, mycoords[0], mycoords[1]);
				int itable_i = (global_i / s) + 1;
				int itable_j = (global_j / s) + 1;

				//Send up row, left coloum, right coloumn, down row
				//recieve up row, left coloumn, right coloum, down row
				if(itable[itable_i - 1][itable_j] != -1)
				{
					get_up_send(local2DBufferForOthers, up_send, I, J, s);
					MPI_Isend(up_send, s, MPI_FLOAT, itable[itable_i-1][itable_j], UP, comm_2d, &(send_request[0]));	
				}
				//Left
				if(itable[itable_i][itable_j-1] != -1)
				{
					get_left_send(local2DBufferForOthers, left_send, I, J, s);
					MPI_Isend(left_send, s, MPI_FLOAT, itable[itable_i][itable_j-1], LEFT, comm_2d, &(send_request[1]));	
				}
				//Right
				if(itable[itable_i][itable_j+1] != -1)
				{
					get_right_send(local2DBufferForOthers, right_send, I, J, s);
					MPI_Isend(right_send, s, MPI_FLOAT, itable[itable_i][itable_j+1], RIGHT, comm_2d, &(send_request[2]));	
				}
				//Down
				if(itable[itable_i + 1][itable_j] != -1)
				{
					get_down_send(local2DBufferForOthers, down_send, I, J, s);
					MPI_Isend(down_send, s, MPI_FLOAT, itable[itable_i+1][itable_j], DOWN, comm_2d, &(send_request[3]));
				}

				//UP
				if(itable[itable_i - 1][itable_j] != -1)
				{
					MPI_Irecv(up_recv, s, MPI_FLOAT, itable[itable_i-1][itable_j], DOWN, comm_2d, &(recv_request[0]));
				}
				//Left
				if(itable[itable_i][itable_j-1] != -1)
				{
					MPI_Irecv(left_recv, s, MPI_FLOAT, itable[itable_i][itable_j-1], RIGHT, comm_2d, &(recv_request[1]));
				}
				//Right
				if(itable[itable_i][itable_j+1] != -1)
				{
					MPI_Irecv(right_recv, s, MPI_FLOAT, itable[itable_i][itable_j+1], LEFT, comm_2d, &(recv_request[2]));
				}
				//Down
				if(itable[itable_i + 1][itable_j] != -1)
				{
					MPI_Irecv(down_recv, s, MPI_FLOAT, itable[itable_i+1][itable_j], UP, comm_2d, &(recv_request[3]));
				}
				/*MPI_Wait(&(send_request[0]), &(send_status[0]));
				MPI_Wait(&(send_request[0]), &(send_status[1]));
				MPI_Wait(&(send_request[0]), &(send_status[2]));
				MPI_Wait(&(send_request[0]), &(send_status[3]));
				*/
				//printf("**********************************%d\n", own_process_rank);
				//fflush(stdout);
				MPI_Waitall(4, recv_request, MPI_STATUSES_IGNORE);
				//MPI_Wait(&(send_request[0]), &(recv_status[0]));
				//MPI_Wait(&(send_request[0]), &(recv_status[1]));
				//MPI_Wait(&(send_request[0]), &(recv_status[2]));
				//MPI_Wait(&(send_request[0]), &(recv_status[3]));
				//printf("Process %d recieved UP ******\n",own_process_rank);
				//print1DArray(up_recv, s);			
				//printf("Process %d recieved LEFT ******\n", own_process_rank);
				//print1DArray(left_recv, s);			
				//printf("Process %d recieved RIGHT ******\n", own_process_rank);
				//print1DArray(right_recv, s);			
				//printf("Process %d recieved DOWN ******\n", own_process_rank);
				//print1DArray(down_recv, s);			
				
				//printf("Process : %d up : %d, left : %d, right : %d, down : %d\n",own_process_rank, itable[itable_i - 1][itable_j], itable[itable_i][itable_j-1],
					//itable[itable_i][itable_j+1], itable[itable_i + 1][itable_j]);

				for(int i = I * s; i < ((I * s) + s); i++)
				{
					for(int j = J * s; j < ((J * s) + s); j++)
					{
						if(local2DBufferForOthers[i][j] != -1)
						//if(get_ith_jth_from_1D(local1DBuffer, i, j, P, s) != -1)
						{
							//1. Get global_i, global_j for each local element
							//2. Also calculate global neighbours of each element
							//3. Check who is associated process for each global neighbour element and request for 
							//local_i, local_j from the responsible process to that neighbour element.
							local2DtoGlobal2D(i, j, &global_element_own[0], &global_element_own[1], p, q, s, mycoords[0], mycoords[1]);

							//Now global_element_own[0] contains i cordinate of the element and similarly for global_element_own[1]
							global_element_neighbour_up[0] = global_element_own[0] - 1  ; global_element_neighbour_up[1] = global_element_own[1];
							global_element_neighbour_left[0] = global_element_own[0]    ; global_element_neighbour_left[1] = global_element_own[1] - 1;
							global_element_neighbour_right[0] = global_element_own[0]   ; global_element_neighbour_right[1] = global_element_own[1] + 1;
							global_element_neighbour_down[0] = global_element_own[0] + 1; global_element_neighbour_down[1] = global_element_own[1];

							//First check if the neighbours are outside the matrix then we don't ask for their owner
							//Now get owner/responsible process for each neighbour
							if(check_if_out_of_matrix(global_element_neighbour_up[0], global_element_neighbour_up[1], M, N))
								up_process_rank = -1;
							else
								up_process_rank = getGlobalRank(global_element_neighbour_up[0], global_element_neighbour_up[1] , p, q, s, comm_2d);

							if(check_if_out_of_matrix(global_element_neighbour_left[0], global_element_neighbour_left[1], M, N))
								left_process_rank = -1;
							else
								left_process_rank = getGlobalRank(global_element_neighbour_left[0], global_element_neighbour_left[1] , p, q, s, comm_2d);

							if(check_if_out_of_matrix(global_element_neighbour_right[0], global_element_neighbour_right[1], M, N))
								right_process_rank = -1;
							else
								right_process_rank = getGlobalRank(global_element_neighbour_right[0], global_element_neighbour_right[1] , p, q, s, comm_2d);

							if(check_if_out_of_matrix(global_element_neighbour_down[0], global_element_neighbour_down[1], M, N))
								down_process_rank = -1;
							else
								down_process_rank = getGlobalRank(global_element_neighbour_down[0], global_element_neighbour_down[1] , p, q, s, comm_2d);

							//Gather all the 5 elements 1 by 1.
							own_element = local2DBufferForOthers[i][j];
							//own_element = get_ith_jth_from_1D(local1DBuffer, i, j, P, s);

							//Check if the ownner process of up neighbour is same as the owner process of central element then no need to ask any other process
							if(up_process_rank == own_process_rank)
								up_element = local2DBufferForOthers[i-1][j];
								//up_element = get_ith_jth_from_1D(local1DBuffer, i-1, j, P, s);
							else
							{
								//Else check if either my up is not out of matrix
								if( up_process_rank == -1)
									up_element = -1;
								//Else we know some other processor is above this element 
								else
									up_element = up_recv[global_element_neighbour_up[1] % s];
							}
							//Left
							if(left_process_rank == own_process_rank)
								left_element = local2DBufferForOthers[i][j-1];
								//left_element = get_ith_jth_from_1D(local1DBuffer, i, j-1, P, s);
							else
							{
								if( left_process_rank == -1)
									left_element = -1;
								else
									left_element = left_recv[global_element_neighbour_left[0] % s];
							}
							//Right
							if(right_process_rank == own_process_rank)
								right_element = local2DBufferForOthers[i][j+1];
								//right_element = get_ith_jth_from_1D(local1DBuffer, i, j+1, P, s);
							else
							{
								if( right_process_rank == -1)
									right_element = -1;
								else
									right_element = right_recv[global_element_neighbour_right[0] % s];
							}
							//Down
							if(down_process_rank == own_process_rank)
								down_element = local2DBufferForOthers[i+1][j];
								//down_element = get_ith_jth_from_1D(local1DBuffer, i+1, j, P, s);
							else
							{
								if( down_process_rank == -1)
									down_element = -1;
								else
									down_element = down_recv[global_element_neighbour_down[1] % s];
							}
							float averagedElement = doAveragingOperation(own_element, up_element, left_element, right_element, down_element, a, b, c, d, e);
							//printf("Co : [%d, %d] Elem: %f, up : %f, left : %f, right : %f, down : %f, average = %f\n",global_element_own[0], global_element_own[1],own_element, up_element, left_element, right_element, down_element, averagedElement);
							fflush(stdout);
							local2DBufferForMe[i][j] = averagedElement;
						}
						
					}
				}
			}
		}
		//printf("Process : %d, Iteration : %d\n", own_process_rank, itr);
		//print_array_with_local_global_info(local2DBufferForMe, P * s, Q * s, p, q, s, mycoords[0], mycoords[1]);
		//printf("***********************\n");
	
		copy2DBufferForMeTo2DBufferforOthers(local2DBufferForMe, local2DBufferForOthers, P, Q, s);
		//convert1Dto2DarrayWithoutAllocating(local1DBuffer, local2DBufferForMe, P, Q, s);
	}
}

int main(int argc, char* argv[])
{
	char* inputFileName = (char*) malloc (sizeof (argv[1]) + 1);
	char* outputFileName = (char*) malloc (sizeof (argv[2]) + 1);
	strcpy(inputFileName, argv[1]);
	strcpy(outputFileName, argv[2]);

	int m = atoi(argv[3]);
	int n = atoi(argv[4]);
	int p = atoi(argv[5]);
	int q = atoi(argv[6]);
	int s = atoi(argv[7]);
	int l = atoi(argv[8]);
	int a = atof(argv[9]);
	int b = atof(argv[10]);
	int c = atof(argv[11]);
	int d = atof(argv[12]);
	int e = atof(argv[13]);

	float** whole_array;
	float** array;
	float* recvPtr;
	int** itab;

	int i, j;
 	int rank, size;
	float* buffer[2];
	int npes, dims[2], periods[2];
	int myrank, my2drank, mycoords[2];
	int uprank, downrank, leftrank, rightrank, coords[2];
	int shiftsource, shiftdest;
	MPI_Status stat_local_buffer;
	MPI_Status stat_itable;
	MPI_Comm comm_2d;
	MPI_Request reqs[4];

	MPI_Init(NULL, NULL);

	/* Get the communicator related information. */
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Set up the cartesian topology. */
	dims[0] = p;
	dims[1] = q;

	/* Unset the periods for the wrap around connections. */
	periods[0] = 0;
	periods[1] = 0;

	/* Create the cartesian topology with rank reordering. */
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_2d);

	/* Get the rank and coordinates with respect to the new topology. */
	MPI_Comm_rank(comm_2d, &my2drank);
	MPI_Cart_coords(comm_2d, my2drank, 2, mycoords);
	//printf("Rank : %d, coords : [%d][%d]\n", my2drank, mycoords[0],mycoords[1]);

	/* Padding Required. Total Rows M = multiple of p * s and Total Cols N = multiple of q * s.*/
	int extraCol = n % ( q * s );
	int extraRow = m % ( p * s );

	int paddedCol = ( ( q * s ) - extraCol ) % ( q * s );
	int paddedRow = ( ( p * s ) - extraRow ) % ( p * s );

	int N = n + paddedCol;
	int M = m + paddedRow;

	int P = ( M / s ) / p;
	int Q = ( N / s ) / q;

	int row_divison = M / s;
	int col_division = N / s;

	int localBufferSize = P * s * Q * s;

	//Create Local Buffer within each Processor
	float* local1DBuffer = (float*) calloc(localBufferSize, sizeof(float));
	int** itable = alloc_2d_int(row_divison + 2, col_division + 2);

	/* Master Process perform Padding. */
	if(rank == MASTER)
	{
		/*Intialise the whole_array with the values readed from inputfile and padded values. */
		whole_array = alloc_2d_float(M, N);
		read_array(whole_array, m, n, paddedRow, paddedCol, inputFileName);
		//printf("***************Whole Array With Process No*********************\n");
		//print_array_with_process_no(whole_array, M, N, p, q, s, comm_2d);

		itab = alloc_2d_int(row_divison + 2, col_division + 2);
		generateITable(itab, row_divison, col_division , p, q, s, comm_2d);
		//printf("*************** ITable ****************************************\n");
		//print_array(itab, row_divison + 2, col_division + 2);
		//printf("***************************************************************\n");

		//Construct arrays for each process
		float (*ptrs[size]);

		for(int i = 0 ; i < size ; i++)
		{
			ptrs[i] = (float*) malloc (sizeof ( float ) *  ( localBufferSize ) );
		}

		createLocalArrays(whole_array, ptrs, M, N, p, q, s, size, comm_2d);

		for(int i = 0; i < size ; i++)
		{
			MPI_Send(ptrs[i], localBufferSize, MPI_FLOAT, i, 0, comm_2d);
			MPI_Send(&(itab[0][0]), (row_divison + 2) * (col_division + 2), MPI_INT, i, 1, comm_2d);
		}

	}

	MPI_Recv(local1DBuffer, localBufferSize, MPI_FLOAT, 0, 0, comm_2d, &stat_local_buffer);
	MPI_Recv(&(itable[0][0]), (row_divison + 2) * (col_division + 2), MPI_INT, 0, 1, comm_2d, &stat_itable);
	
	//Barrier because first the matrix should be readed from file and distributed to every process.

	/*printf("Process %d recieving : \n", my2drank);
	print1DArray(local1DBuffer, localBufferSize);
	printf("***************************************************************\n");
	*/
	//Printing the received 1DBuffer after conversion to the 2DBuffer
	float** local2DBufferForMe = convert1Dto2Darray(local1DBuffer, localBufferSize, P, Q, s);
	float** local2DBufferForOthers = convert1Dto2Darray(local1DBuffer, localBufferSize, P, Q, s);
	
	MPI_Barrier(comm_2d);
	doAveragingIteration(local2DBufferForMe, local2DBufferForOthers, itable, M, N, P, Q, p, q, s, a, b, c, d, e, l, comm_2d);

	//print_array_with_local_global_info(local2DBufferForMe, P * s, Q * s, p, q, s, mycoords[0], mycoords[1]);
	MPI_Finalize();
	return 0;
}