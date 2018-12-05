/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program ..
*/

// Includes --------------------------------------------------------------------

#include "Thread_Manager.h"

int main(int argc, char *argv[]) {

	// Input cmd line format: ex3.exe <MAX_NUMBER> <NUM_OF_COMPUTATION_THREADS> <OUTPUT_BUFFER_SIZE> <OUTPUT_FILE>
	// Check that exactly 4 cmd line args are present
	if (argc < 5) {
		printf("Not enough input arguments, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	if (argc > 5) {
		printf("Too many input arguments, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	// Allocate data structures & load data to thread container
	thread_container thread_data;
	thread_container *thread_data_ptr = &thread_data;
	if (initThreadContainer(argv, thread_data_ptr) != 0) { 
		printf("Error while creating thread data, couldn't complete the task!\n");
		freeThreadContainer(thread_data_ptr);
		exit(EXIT_FAILURE);
	}
	
	int thread_num = atoi(argv[2]);


	

	

	/*
	// Open all test threads
	HANDLE *thread_handles = NULL;										// Created for using WaitForMultipleObjects
	thread_handles = (HANDLE*)malloc(sizeof(HANDLE)*(test_counter));	// malloc success is checked in runTestThreads routine
	if (runThreads(test_list, thread_handles) != 0) {
		printf("An error occurred when creating test threads, couldn't complete the task!\n");
		ClearTestList(test_list);
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// Wait for threads to finish
	wait_code = WaitForMultipleObjects((DWORD)test_counter, thread_handles, true, INFINITE);
	if (WAIT_OBJECT_0 != wait_code) {
		printf("Error while waiting for threads to finish, couldn't complete the task!\n");
		ClearTestList(test_list);
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// Check thread exit codes
	if (checkThreads(test_list) != 0) {
		ClearTestList(test_list);
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	// At this point all test threads have finished successfully - Create Test results file
	if (createResultsFile(argv[2], test_list) != 0) {
		printf("An error occurred during results file creation, couldn't complete the task!\n");
		ClearTestList(test_list);
		free(thread_handles);
		exit(EXIT_FAILURE);
	}

	ClearTestList(test_list);
	free(thread_handles);
	*/
	exit(EXIT_SUCCESS);
}
