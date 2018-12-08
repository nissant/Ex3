/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triples using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
*/

// Includes --------------------------------------------------------------------

#include "Thread_Manager.h"

int main(int argc, char *argv[]) {
	DWORD wait_code;
	// Check that exactly 4 cmd line args are present, cmd line format: ex3.exe <MAX_NUMBER> <NUM_OF_COMPUTATION_THREADS> <OUTPUT_BUFFER_SIZE> <OUTPUT_FILE>
	if (argc < 5) {
		printf("Not enough input arguments, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	if (argc > 5) {
		printf("Too many input arguments, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	// Allocate memory, load data to thread container and open mutex\semaphore handles
	thread_container thread_data;
	thread_container *thread_data_ptr = &thread_data;
	if (initThreadContainer(argv, thread_data_ptr) != 0) { 
		printf("Error while creating thread container, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	// Run all threads
	int thread_num = atoi(argv[2]) + 1;
	HANDLE *thread_handles = NULL;											
	thread_handles = (HANDLE*)malloc(sizeof(HANDLE)*(thread_num));		// malloc success is checked in runProducerConsumerThreads routine
	if (runProducerConsumerThreads(thread_data_ptr,thread_handles) != 0) {
		printf("An error occurred when running threads, couldn't complete the task!\n");
		cleanThreadContainer(thread_data_ptr);			// Close Container handles and free allocated memory
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// Wait for thread to finish
	wait_code = WaitForMultipleObjects((DWORD)thread_num, thread_handles, true, INFINITE);
	if (WAIT_OBJECT_0 != wait_code) {
		printf("Error while waiting for threads to finish, couldn't complete the task! Last Error = 0x%x\n", GetLastError());
		cleanThreadContainer(thread_data_ptr);			// Close Container handles and free allocated memory
		closeThreadHandles(thread_handles, thread_num);	// Close thread handles
		free(thread_handles);
		exit(EXIT_FAILURE);
	}
	/*
	// Check thread exit codes
	if (checkThreads(thread_handles) != 0) {
		cleanThreadContainer(thread_data_ptr);
		closeThreadHandles(thread_handles, thread_num);
		free(thread_handles);
		exit(EXIT_FAILURE);
	}
	*/
	// At this point all threads have finished and results file created successfully in path 
	cleanThreadContainer(thread_data_ptr);				// Close Container handles and free allocated memory
	closeThreadHandles(thread_handles, thread_num);		// Close thread handles
	free(thread_handles);
	exit(EXIT_SUCCESS);
}
