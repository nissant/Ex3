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

		return ERROR_CODE;
	}
	
	// init the ogen mutex and flags
	for (int i = 0; i < max_num; i++) {
		ogen_mutex[i] = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
		if (ogen_mutex[i] == NULL) {
			printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
			return ERROR_CODE;
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
			return ERROR_CODE;
		}
		buffer[i].data_flag = 0;
		buffer[i].pythagorean.a = 0;
		buffer[i].pythagorean.b = 0;
		buffer[i].pythagorean.c = 0;
		buffer[i].pythagorean.m = 0;
		buffer[i].pythagorean.n = 0;
	}
	
	// Load data to container

	thread_data_ptr->prod_thread_count = argv[2];
	thread_data_ptr->buffer_size = buffer_size;
	thread_data_ptr->max_number = max_num;
	thread_data_ptr->ogen_flag_array = ogen_flags;
	thread_data_ptr->pyth_triple_buffer = buffer;
	thread_data_ptr->ogen_mutex_array = ogen_mutex;

	// Reset list counter
	pythagorean_triple_counter = 0;

	return SUCCESS_CODE;
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
	DWORD				wait_res, release_res;
	DWORD				wait_code;
	BOOL				ret_val;
	thread_container	*thread_info = (thread_container*)lpParam;		// Get pointer to thread data container

	while (thread_counter<=thread_info->prod_thread_count)				//	While threads haven't finished passing data to buffer and exit
	{
		wait_res = WaitForSingleObject(buffer_full_sem, INFINITE);		// decrament full semaphore
		if (wait_res != WAIT_OBJECT_0) {
			printf("Error when waiting for buffer semaphore!\n");
			return ERROR_CODE;
		}

		/* Start Critical Section */

		if (clear_buffer(thread_info, false) != 0) {
			printf("Error when clearing buffer!\n");
			return ERROR_CODE;
		}

		/* End Critical Section */

		release_res = ReleaseSemaphore(									// incrament empty semaphore
			buffer_empty_sem,											// lReleasecount
			1, 															// Signal that exactly one cell was emptied
			NULL);														// lpPreviouscount
		if (release_res == FALSE) {
			printf("Error when releasing buffer semaphore!\n");
			return ERROR_CODE;
		}

	}
	// At this point all threads have exited successfully after inserting data to buffer
	clear_buffer(thread_info, true); 
	qsort(pythagorean_triple_lst, pythagorean_triple_counter, sizeof(triple), cmp_function);
	return SUCCESS_CODE;
}


/*
Function sortConsumer
------------------------
Description –
Parameters	–
Returns		–
*/
int clear_buffer(thread_container *thread_info, bool clearall) {
	DWORD wait_res, release_res;

	for (int i; i < thread_info->buffer_size; i++) {
		wait_res = WaitForSingleObject(thread_info->pyth_triple_buffer[i].data_mutex, INFINITE);
		if (wait_res != WAIT_OBJECT_0) {
			printf("Error when waiting for buffer entry mutex\n");
			return ERROR_CODE;
		}
		if (thread_info->pyth_triple_buffer[i].data_flag == 0) {			// No new data availabe in buffer entry
			release_res = ReleaseMutex(thread_info->pyth_triple_buffer[i].data_mutex);
			if (release_res == FALSE) {
				printf("Error when releasing buffer entry mutex\n");
				return ERROR_CODE;
			}
			continue;
		}
		else {																// Found new data in buffer 
			pythagorean_triple_counter++;
			pythagorean_triple_lst = realloc(pythagorean_triple_lst, pythagorean_triple_counter * sizeof(triple));
			if (pythagorean_triple_lst == NULL) {
				printf("Realloc failed!\n");
				return ERROR_CODE;
			}
			pythagorean_triple_lst[pythagorean_triple_counter - 1] = thread_info->pyth_triple_buffer[i].pythagorean;
			thread_info->pyth_triple_buffer[i].data_flag = 0;				// Reset data flag
			release_res = ReleaseMutex(thread_info->pyth_triple_buffer[i].data_mutex);
			if (release_res == FALSE) {
				printf("Error when releasing buffer entry mutex\n");
				return ERROR_CODE;
			}
			if (clearall == false) {
				return SUCCESS_CODE;
			}
				
		}
	}

	return SUCCESS_CODE;
}


/*
Function sortConsumer
------------------------
Description –
Parameters	–
Returns		–
*/
int cmp_function(const void * a, const void * b) {
	triple *firstTriple = (triple*)a;
	triple *secondTriple = (triple*)b;

	if (firstTriple->n > secondTriple->n) {
		return 1;
	}
	else if (firstTriple->n < secondTriple->n) {
		return -1;
	}
	else {		// Their n is equal
		if (firstTriple->m > secondTriple->m) {
			return 1;
		}
		else if (firstTriple->m < secondTriple->m) {
			return -1;
		}
	}
	return 0;

}