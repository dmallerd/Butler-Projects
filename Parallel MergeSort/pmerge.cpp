// Sam Badovinac
// Dan Mallerdino
// Pmerge
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <time.h>
#include <cstdlib>
#include <math.h>
#include <algorithm>
#include "mpi.h" // message passing interface

using namespace std;

// New compile and run commands for MPI!
// mpicxx -o blah file.cpp
// mpirun -q -np 32 blah

//global variables
int *a = new int[32];
int my_rank;
int p;

//ran into issues with "pmerge not declared" so i moved it to the bottom
void pmerge(int *, int, int, int, int);
void smerge(int *, int *, int, int, int, int, int);
void mergeSort(int *, int, int);
int Rank(int *, int, int, int);
void print(int *, int);

int main(int argc, char *argv[])
{
	int source;		   // rank of the sender
	int dest;		   // rank of destination
	int tag = 0;	   // message number
	char message[100]; // message itself
	MPI_Status status; // return status for receive

	// Start MPI
	MPI_Init(&argc, &argv);

	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	// THE REAL PROGRAM IS HERE

	//Split up the problem

	int n = 32; //size of the array
	int *a = new int[n];
	//Process 0 creates and brodcasts the entire array
	if (my_rank == 0)
	{
		int b[32] = {4, 6, 8, 9, 16, 17, 18, 19, 20, 21, 23, 25, 27, 29, 31, 32, 1, 2, 3, 5, 7, 10, 11, 12, 13, 14, 15, 22, 24, 26, 28, 30};
		//4,6,8,9,16,17,18,19,20,21,23,25,27,29,31,32,1,2,3,5,7,10,11,12,13,14,15,22,24,26,28,30
		//9, 8, 4, 6, 16, 17, 18, 19, 20, 21, 23, 25, 27, 29, 31, 32, 1, 2, 3, 5, 7, 10, 11, 12, 13, 14, 15, 22, 24, 26, 28, 30
		//36,63,39,33,38,43,58,40,34,50,44,37,61,42,46,35,47,41,49,51,64,52,48,45,60,54,53,55,59,56, 57, 62};
		for (int i = 0; i < 32; i++) { a[i] = b[i]; } 

		srand(1251);

		//Fill the array with random numbers
		//for (int x = 0; x < n; x++)
		//a[x] = rand() % 11;
	}
	// Broadcast the array to all processes
	MPI_Bcast(&a[0], n, MPI_INT, 0, MPI_COMM_WORLD);

	mergeSort(a, 0, n - 1);

	MPI_Finalize();
	delete[] a;
	return 0;
}

void pmerge(int *a, int first, int last, int mid, int n)
{
	int currentprocessor = my_rank;
	//Calculate SRANKA and SRANKB size, might need tweaking with log2(n/2) being a decimal
	double size = ceil((double)(n / 2));
	int SRANKsize = size / (log(size) / log(2));

	//cout << "SRANKsize: " << SRANKsize << endl;

	int *localSRANKA = new int[SRANKsize];
	int *localSRANKB = new int[SRANKsize];
	int *SRANKA = new int[SRANKsize];
	int *SRANKB = new int[SRANKsize];

	for (int x = 0; x < SRANKsize; x++)
	{
		localSRANKA[x] = 0;
		localSRANKB[x] = 0;
		SRANKA[x] = 0;
		SRANKB[x] = 0;
	}

	//cout << "Mid: " << mid << endl;

	//Calculate SRANK
	for (int x = my_rank; x < SRANKsize; x += p)
	{
		int Rightvaluetofind = a[(int)(x * (log(size) / log(2)))];
		int Leftvaluetofind = a[mid + 1 + (int)(x * (log(size) / log(2)))];

		//cout << "Left Val " << Leftvaluetofind << " right val " << Rightvaluetofind << endl;

		int *b = new int[(int)(size)];
		for (int i = 0; i < (int)size; i++){b[i] = a[mid + 1 + i];}
 
		if (Rightvaluetofind > a[n/2])
			localSRANKA[x] = n/2;
		else 
			localSRANKA[x] = Rank(&a[n/2], 0, mid, Rightvaluetofind);
		//cout << "SRANKA[" << x << "] = " << localSRANKA[x] << endl; //bug testing

		if (Leftvaluetofind > a[n-1])
			localSRANKB[x] = n/2;
		else  
			localSRANKB[x] = Rank(&a[0], 0, mid, Leftvaluetofind);
		//cout << "SRANKB[" << x << "] = " << localSRANKB[x] << endl; //bug testing
	
	}

	//will need to reduce localsrank to srank
	MPI_Allreduce(localSRANKA, SRANKA, SRANKsize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(localSRANKB, SRANKB, SRANKsize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	//bug testing

	if (my_rank == 0)
	{
		cout << "SRANKA = {";
		for (int i = 0; i < SRANKsize; i++)
			cout << SRANKA[i] << " ";
		cout << "}" << endl;
		cout << "SRANKB = {";
		for (int i = 0; i < SRANKsize; i++)
			cout << SRANKB[i] << " ";
		cout << "}" << endl;
	}

	//filling Right Hand Merge Array
	int *MergeArrayA = new int[2 * SRANKsize];

	for (int x = 0; x < SRANKsize; x++)
	{
		MergeArrayA[x] = x * (log(size) / log(2));
	}
	for (int x = SRANKsize; x < 2 * SRANKsize; x++)
	{
		MergeArrayA[x] = SRANKA[x - SRANKsize];
	}

	
	MergeArrayA[2 * SRANKsize] = n / 2; //Filling in the final value of MergeArrayA

	sort(MergeArrayA, MergeArrayA + (2 * SRANKsize)); //SORT MERGEARRAYA

	//Filling Left Hand Merge Array
	int *MergeArrayB = new int[2 * SRANKsize];

	for (int x = 0; x < SRANKsize; x++)
	{
		MergeArrayB[x] = x * (log(size) / log(2));
	}
	for (int x = SRANKsize; x < 2 * SRANKsize; x++)
	{
		MergeArrayB[x] = SRANKB[x - SRANKsize];
	}

	MergeArrayB[2 * SRANKsize] = n / 2; //Filling in the final value of MergeArrayB

	sort(MergeArrayB, MergeArrayB + (2 * SRANKsize)); //SORT MERGEARRAYB

	//outputting the merge arrays
	if (my_rank == 0)
	{
		cout << "Merge Array A = {";
		for (int i = 0; i <= 2 * SRANKsize; i++)
			cout << MergeArrayA[i] << " ";
		cout << "}" << endl;
		cout << "Merge Array B = {";
		for (int i = 0; i <= 2 * SRANKsize; i++)
			cout << MergeArrayB[i] << " ";
		cout << "}" << endl;
	}

	//output array when smerge works
	int *WIN = new int[n];
	for (int i = 0; i < n; i++)
		WIN[i] = 0;

	int *FINAL = new int[n];
	for (int i = 0; i < n; i++)
		FINAL[i] = 0;
	
	for (int x = my_rank; x < (2 * SRANKsize) - 1; x += p)
	{
		//Print out the shapes that are being merged
		// cout << endl;
		// cout << "Process: " <<  my_rank << endl;
		// cout << "Merging " << a[MergeArrayB[(int)x]] << " to " << a[MergeArrayB[(int)x + 1] - 1] << " with " << a[MergeArrayA[x] + (int)size];
		// cout << " to " << a[MergeArrayA[x + 1] + (int)size - 1] << endl;
		// cout << endl;

		smerge(&WIN[0], &a[0], MergeArrayB[(int)x], MergeArrayB[(int)x + 1] - 1, MergeArrayA[x] + (int)size, MergeArrayA[x + 1] + (int)size - 1, n);
	}
	if(my_rank == 0)
	{
		smerge(&WIN[0], &a[0], MergeArrayB[(2 * SRANKsize) - 1], MergeArrayB[2 * SRANKsize] - 1, MergeArrayA[(2*SRANKsize) - 1] + (int)size, MergeArrayA[2*SRANKsize] + (int)size - 1, n);
	}

	MPI_Allreduce(WIN, FINAL, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	if (my_rank == 0)
	{
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		cout << "FINAL = {";
		for (int h = 0; h < n; h++)
		{
			cout << FINAL[h] << " ";
		}
		cout << "}" << endl;
	}
	for (int i = 0; i < n; i++)
	{
		a[i] = FINAL[i];
	}

	delete[] localSRANKA;
	delete[] localSRANKB;
	return;
}

void smerge(int *WIN, int *a, int first1, int last1, int first2, int last2, int n)
{
	int i, j, k;
	int sizeL = last1 - first1 + 1;
	int sizeR = last2 - first2 + 1;
	if (sizeL < 1)
	{
		sizeL = 0;
	}
	if (sizeR < 1)
	{
		sizeR = 0;
	}

	int *L = new int[sizeL]; //left half array
	int *R = new int[sizeR]; //right half array
							 //Setting L and R to 0
	for (int h = 0; h < sizeL; h++)
	{
		L[h] = 0;
	}
	for (int h = 0; h < sizeR; h++)
	{
		R[h] = 0;
	}

	if (first1 > last1 && sizeL > 0)
	{
		for (i = 0; i < 1; i++)
		{
			L[i] = 0;
			cout << "L[i] = " << L[i] << " ";
		}
		cout << endl;
	}
	else
	{
		for (i = 0; i < sizeL; i++)
		{
			L[i] = a[first1 + i];
			cout << "L[i] = " << L[i] << " ";
		}
		cout << endl;
	}

	if (first2 > last2 && sizeR > 0)
	{
		for (j = 0; j < 1; j++)
		{
			R[j] = 0;
			cout << "R[j] = " << R[j] << " ";
		}
	}
	else
	{
		for (j = 0; j < sizeR; j++)
		{
			R[j] = a[first2 + j];
			cout << "R[i] = " << R[j] << " ";
		}
		cout << endl;
	}

	i = 0;						   // Initial index of first subarray
	j = 0;						   // Initial index of second subarray
	k = first1 + first2 - (n / 2); // Initial index of merged subarray

	while (i < sizeL && j < sizeR)
	{ //do the merge
		if (L[i] < R[j])
		{
			WIN[k++] = L[i++];
		}
		else
		{
			WIN[k++] = R[j++];
		}
	}

	while (i < sizeL)
	{
		WIN[k++] = L[i++];
		i++;
		k++;
	}

	while (j < sizeR)
	{ //toss the remaining items into that young array
		WIN[k++] = R[j++];
	}
}

void mergeSort(int *a, int first, int last)
{
	int n = last + 1;
	int middle = first + (last - first) / 2;
	if (n <= 4)
	{
		sort(a, a + 4);
		return;
	}

	if (first < last)
	{
		mergeSort(&a[0], first, middle);

		mergeSort(&a[n / 2], first, middle);

		pmerge(&a[first], first, last, middle, n);
	}
}

int Rank(int *b, int first, int last, int valToFind)
{
	//Special case
	if (valToFind > b[last - 1]){return last;}

	// trivial case, array size of 1
	if (last == 1)
	{
		if (valToFind <= b[0])
			return 0;
		else
			return 1;
	}
	else
	{
		if (valToFind < b[(last / 2) - 1])// if value is less than middle value try again with left half of array
			return Rank(&b[0], 0, last / 2, valToFind);
		else //&a[last/2] cuts the pointer of the array such that it is indexed starting at last/2 as position 0 in order to look at right half of array
			return Rank(&b[(last / 2)], 0, last / 2, valToFind) + (last / 2);
	}
	delete[] b;
}

void print(int *a, int n)
{
	for (int x = 0; x < n; x++)
	{
		if (x % 16 != 0 && a[x] < 10)
			cout << "  " << a[x];
		else if (x % 16 != 0 && a[x] >= 10)
			cout << " " << a[x];
		else if (x % 16 == 0 && a[x] < 10 && x != 0)
			cout << "  " << a[x] << endl;
		else if (x % 16 == 0 && a[x] >= 10 && x != 0)
			cout << " " << a[x] << endl;
		else if (a[x] >= 10 && x == 0)
			cout << " " << a[x];
		else if (a[x] < 10 && x == 0)
			cout << "  " << a[x];
	}
	cout << endl;
}