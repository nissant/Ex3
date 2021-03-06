/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triples using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
*/

// Includes --------------------------------------------------------------------

#include "Thread_Manager.h"

int main(int argc, char *argv[]) {
	
	// Check that exactly 4 cmd line args are present, cmd line format: ex3.exe <MAX_NUMBER> <NUM_OF_COMPUTATION_THREADS> <OUTPUT_BUFFER_SIZE> <OUTPUT_FILE>
	if (checkArgs(argc, 4) != 0) {
		exit(EXIT_FAILURE);
	}

	// Allocate container memory, load data to thread container and open mutex\semaphore handles
	thread_container thread_data;
	thread_container *thread_data_ptr = &thread_data;
	if (initThreadContainer(argv, thread_data_ptr) != 0) { 
		printf("Error while creating thread container, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	// Run all threads and keep thread handles
	int thread_num = atoi(argv[2]) + 1;
	HANDLE *thread_handles = NULL;											
	thread_handles = (HANDLE*)malloc(sizeof(HANDLE)*(thread_num));		// malloc success is checked in runProducerConsumerThreads routine
	if (runProducerConsumerThreads(thread_data_ptr,thread_handles) != 0) {
		printf("An error occurred when running threads, couldn't complete the task!\n");
		cleanThreadContainer(thread_data_ptr);							// Close Container handles and free allocated memory
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// Wait for thread to finish
	if (waitForThreads(thread_handles, thread_num) != 0){
		printf("Error while waiting for threads to finish, couldn't complete the task! Last Error = 0x%x\n", GetLastError());
		cleanThreadContainer(thread_data_ptr);							// Close Container handles and free allocated memory
		closeThreadHandles(thread_handles, thread_num);					// Close thread handles
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// Check thread exit codes and print output file if successful
	if (checkThreadsAndPrint(thread_handles, thread_num, argv[4]) != 0) {
		cleanThreadContainer(thread_data_ptr);
		closeThreadHandles(thread_handles, thread_num);
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// At this point all threads have finished and results file created successfully in path 
	cleanThreadContainer(thread_data_ptr);								// Close Container handles and free allocated memory
	closeThreadHandles(thread_handles, thread_num);						// Close thread handles
	free(thread_handles);
	exit(EXIT_SUCCESS);
}
