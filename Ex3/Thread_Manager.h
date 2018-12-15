#pragma once
/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triples using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
				- This module contains main routines and the sorting thread routine
*/


// Includes --------------------------------------------------------------------
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifndef THREAD_MANAGER_H

	#define THREAD_MANAGER_H

	// Defines ---------------------------------------------------------------------
	#define ERROR_CODE -1
	#define SUCCESS_CODE 0
	#define SORT_BUFFER_WAITE_MS 40

	// Global Variables ------------------------------------------------------------
	HANDLE buffer_full_sem;						// Sempahore handle for buffer. The Producer releases this semaphore (+1)
	HANDLE buffer_empty_sem;					// Sempahore handle for buffer. The Consumer releases this semaphore (+1)

	int thread_counter;							// Finished thread counter
	HANDLE thread_counter_mutex;				// Mutex for thread counter

	typedef struct tri{						
		int a, b, c;							// Pythagorean triple
		int m, n;								// Pythagorean triple parameters 
	}triple;

	int pythagorean_triple_counter;				// Number of triples found
	triple *pythagorean_triple_lst;				// This array holds the sorted final Pythagorean triples

	typedef struct buffer{
		triple pythagorean; 					// This variable holds the pythagorean triple data
		char data_flag;							// Flag that signals if data is available in the corresponding index 
		HANDLE data_mutex;						// Mutex that protects the data in cell
	} data_buffer;

	typedef struct container{
		data_buffer *pyth_triple_buffer;		// Pointer to the allocated pythagorean triple buffer
		int buffer_size;						// Size of allocated buffer
	
		HANDLE *ogen_mutex_array;				// Pointer to array of mutex for each ogen
		char *ogen_flag_array;					// Pointer to array of flags for each ogen
		int max_number;							// Max number for the ogen

		int prod_thread_count;					// Number of producing threads 

	} thread_container;


	// Function Declarations -------------------------------------------------------
	DWORD WINAPI sortConsumer(LPVOID lpParam);
	int initThreadContainer(char **argv, thread_container *thread_data_ptr);
	int runProducerConsumerThreads(thread_container *thread_data_ptr, HANDLE *thread_handles);
	void cleanThreadContainer(thread_container *thread_data_ptr);
	int clear_buffer(thread_container *thread_info, bool clearall);
	void closeThreadHandles(HANDLE *thread_handles, int thread_count);
	int cmp_function(const void * a, const void * b);
	int checkArgs(int argc, int num);
	int checkThreadsAndPrint(HANDLE *thread_handles, int thread_count, char *path);
	int waitForThreads(HANDLE *thread_handles, int thread_count);

#endif