/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triplets using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
				- This module handles the pythgorean triplet calculation thread routines 
*/


#ifndef PYTHAGOREAN_THREAD_H
	#define PYTHAGOREAN_THREAD_H

	// Includes --------------------------------------------------------------------


	// Defines -------------------------------------------------------------------
	#define ERROR_CODE -1
	#define SUCCESS_CODE 0

	// Function Declarations -------------------------------------------------------

	int FindBuffSlot(thread_container *thread_info);
	void CalcABC(int n, int m, int *a, int *b, int *c);
	int FindGCD(int n, int m);
	int PutInBuffer(int n, int m, int a, int b, int c, thread_container *thread_info);
	int CalcTripletPutInBuffer(int n, int max, thread_container *thread_info);
	DWORD WINAPI PythThreadFunc(LPVOID lpParam);

#endif // 