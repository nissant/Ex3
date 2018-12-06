/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triplets using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
				- This module contains main routines and the sorting thread routine
*/


// Includes --------------------------------------------------------------------
#include "Thread_Manager.h"


// Function Definitions -------------------------------------------------------

/*
Function freeThreadContainer
------------------------
Description –
Parameters	–
Returns		– 
*/
int initThreadContainer(char **argv, thread_container *thread_data_ptr) {

	int max_num = atoi(argv[1]);
	int buffer_size = atoi(argv[3]);

	data_buffer *buffer = (data_buffer*)malloc(sizeof(data_buffer)*buffer_size);
	char *ogen_flags = (char*)malloc(sizeof(char)*max_num);
	HANDLE *ogen_mutex = (HANDLE*)malloc(sizeof(HANDLE)*max_num);

	if (buffer == NULL || ogen_flags == NULL || ogen_mutex == NULL) {

		return -1;
	}
	
	// init the ogen mutex and flags
	for (int i = 0; i < max_num; i++) {
		ogen_mutex[i] = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
		if (ogen_mutex[i] == NULL) {
			printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
			return -1;
		}
		ogen_flags[i] = 0;
		
	}
	ogen_flags[max_num - 1] = 1; // mark the last place as calculated because ogen must be < max_num

	// init the buffer valus, flags and mutex
	for (int i = 0; i < buffer_size; i++) {
		buffer[i].data_mutex = CreateMutex(
			NULL,   /* default security attributes */
			FALSE,	/* don't lock mutex immediately */
			NULL); /* un-named */
		if (buffer[i].data_mutex == NULL) {
			printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
			return -1;
		}
		buffer[i].data_flag = 0;
		buffer[i].a = 0;
		buffer[i].b = 0;
		buffer[i].c = 0;
		buffer[i].m = 0;
		buffer[i].n = 0;
	}
	
	// Load data to container
	thread_data_ptr->out_path = argv[4];
	thread_data_ptr->last_thread_done = 0;
	thread_data_ptr->buffer_size = buffer_size;
	thread_data_ptr->max_number = max_num;
	thread_data_ptr->ogen_flag_array = ogen_flags;
	thread_data_ptr->pyth_triple_buffer = buffer;
	thread_data_ptr->ogen_mutex_array = ogen_mutex;

	return 0;
}


/*
Function freeThreadContainer
------------------------
Description – 
Parameters	– 
Returns		– None
*/
void freeThreadContainer(thread_container *thread_data_ptr) {

	// Close ogen mutex handles
	for (int i = 0; i < thread_data_ptr->max_number; i++) {
		CloseHandle(thread_data_ptr->ogen_mutex_array[i]);
	}

	// Close buffer mutex handles
	for (int i = 0; i < thread_data_ptr->buffer_size; i++) {
		CloseHandle(thread_data_ptr->pyth_triple_buffer[i].data_mutex);
	}

	// Free the 3 dynamic allocations
	free(thread_data_ptr->ogen_mutex_array);
	free(thread_data_ptr->pyth_triple_buffer);
	free(thread_data_ptr->ogen_flag_array);
}


/*
Function sortConsumer
------------------------
Description –
Parameters	–
Returns		–
*/
DWORD WINAPI sortConsumer(LPVOID lpParam) {
	/*
	DWORD wait_res;
	BOOL release_res;
	LONG previous_count;

	while (TRUE)
	{
		//  Sleep for the purposes of the demonstration 
		Sleep(PRODUCER_WAIT_TIME_IN_MILISECONDS);

		produce_item();

		wait_res = WaitForSingleObject(empty, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

		wait_res = WaitForSingleObject(mutex, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

		// Start Critical Section 

		insert_item(items);

		///End Critical Section 

		release_res = ReleaseMutex(mutex);
		if (release_res == FALSE) ReportErrorAndEndProgram();

		release_res = ReleaseSemaphore(
			full,
			1, 		//  Signal that exactly one cell was filled 
			&previous_count);
		if (release_res == FALSE) ReportErrorAndEndProgram();
		printf("Producer inserted one item. Previous count is: %ld\n", previous_count);
	}
	*/
	return 0;
}