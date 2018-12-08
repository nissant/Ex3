/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triplets using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
				- This module contains main routines and the sorting thread routine
*/


// Includes --------------------------------------------------------------------
#include "Thread_Manager.h"
#include "Pythagorean_Thread.h"


// Function Definitions -------------------------------------------------------

/*
Function cleanThreadContainer
------------------------
Description –
Parameters	–
Returns		– 
*/
int initThreadContainer(char **argv, thread_container *thread_data_ptr) {

	int i,j;
	int max_num = atoi(argv[1]);
	int buffer_size = atoi(argv[3]);

	// Allocate ogen,flags and buffer arrays 
	data_buffer *buffer = (data_buffer*)malloc(sizeof(data_buffer)*buffer_size);
	char *ogen_flags = (char*)malloc(sizeof(char)*max_num);
	HANDLE *ogen_mutex = (HANDLE*)malloc(sizeof(HANDLE)*max_num);

	if (buffer == NULL || ogen_flags == NULL || ogen_mutex == NULL) {
		printf("Memory allocation failed!\n");
		goto Mem_Clean;
	}

	// Create buffer semaphore

	buffer_empty_sem = CreateSemaphore(
		NULL,				/* Default security attributes */
		buffer_size,		/* Initial Count - all slots are empty */
		buffer_size,		/* Maximum Count */
		NULL);				/* un-named */

	if (buffer_empty_sem == NULL) {
		printf("Encountered error while creating semaphore, ending program. Last Error = 0x%x\n", GetLastError());
		goto Mem_Clean;
	}

	buffer_full_sem = CreateSemaphore(
		NULL,				/* Default security attributes */
		0,					/* Initial Count - no slots are full */
		buffer_size,		/* Maximum Count */
		NULL); /* un-named */

	if (buffer_full_sem == NULL) {
		printf("Encountered error while creating semaphore, ending program. Last Error = 0x%x\n", GetLastError());
		goto Mem_Clean;
	}
	
	// Create thread counter mutex
	thread_counter_mutex = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
	if (thread_counter_mutex == NULL) {
		printf("Encountered error while creating mutex, ending program. Last Error = 0x%x\n", GetLastError());
		goto Mem_Clean;
	}
	

	// init the ogen mutex and flags
	for (i = 0; i < max_num; i++) {
		ogen_mutex[i] = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
		if (ogen_mutex[i] == NULL) {			// Handle error
			// Clean Up
			for (int j = 0; j < i; j++) {
				CloseHandle(ogen_mutex[j]);
			}
			CloseHandle(thread_counter_mutex);
			printf("Encountered error while creating mutex, ending program. Last Error = 0x%x\n", GetLastError());
			goto Mem_Clean;
		}
		ogen_flags[i] = 0;
		
	}

	// init the buffer valus, flags and mutex
	for (i = 0; i < buffer_size; i++) {
		buffer[i].data_mutex = CreateMutex(
			NULL,   /* default security attributes */
			FALSE,	/* don't lock mutex immediately */
			NULL); /* un-named */
		if (buffer[i].data_mutex == NULL) {		// Handle error
			// Clean Up
			for (j = 0; j < max_num; j++) {
				CloseHandle(ogen_mutex[j]);
			}
			for (j = 0; j < i; j++) {
				CloseHandle(buffer[j].data_mutex);
			}
			CloseHandle(thread_counter_mutex);
			printf("Encountered error while creating mutex, ending program. Last Error = 0x%x\n", GetLastError());
			goto Mem_Clean;
		}
		// init buffer entries
		buffer[i].data_flag = 0;
		buffer[i].pythagorean.a = 0;
		buffer[i].pythagorean.b = 0;
		buffer[i].pythagorean.c = 0;
		buffer[i].pythagorean.m = 0;
		buffer[i].pythagorean.n = 0;
	}
	
	ogen_flags[max_num - 1] = 1; // mark the last place as calculated because ogen must be < max_num

	// Load data to container

	thread_data_ptr->prod_thread_count = atoi(argv[2]);
	thread_data_ptr->buffer_size = buffer_size;
	thread_data_ptr->max_number = max_num;
	thread_data_ptr->ogen_flag_array = ogen_flags;
	thread_data_ptr->pyth_triple_buffer = buffer;
	thread_data_ptr->ogen_mutex_array = ogen_mutex;
	

	// Reset list counter
	pythagorean_triple_counter = 0;

	return SUCCESS_CODE;

Mem_Clean:
	free(buffer);
	free(ogen_flags);
	free(ogen_mutex);
	return ERROR_CODE;
}


/*
Function cleanThreadContainer
------------------------
Description – 
Parameters	– 
Returns		– None
*/
void cleanThreadContainer(thread_container *thread_data_ptr) {

	// Close buffer semaphores
	CloseHandle(buffer_empty_sem);
	CloseHandle(buffer_full_sem);
	
	// Close thread counter mutex handle
	CloseHandle(thread_counter_mutex);
	
	// Close ogen mutex handles
	for (int i = 0; i < thread_data_ptr->max_number; i++) {
		CloseHandle(thread_data_ptr->ogen_mutex_array[i]);
	}

	// Close buffer mutex handles
	for (int i = 0; i < thread_data_ptr->buffer_size; i++) {
		CloseHandle(thread_data_ptr->pyth_triple_buffer[i].data_mutex);
	}

	// Free the 4 dynamic allocations
	free(thread_data_ptr->ogen_mutex_array);
	free(thread_data_ptr->pyth_triple_buffer);
	free(thread_data_ptr->ogen_flag_array);
	free(pythagorean_triple_lst);
}


/*
Function runProducerConsumerThreads
------------------------
Description – The function opens the tests threads and returns a list of thread handles
Parameters	– *test_list_ptr pointer to test list, *thread_handles pointer to array of list handles
Returns		– 0 for success, -1 for failure
*/
int runProducerConsumerThreads(thread_container *thread_data_ptr, HANDLE *thread_handles) {
	int i, j;
	HANDLE tmp_thread_handle;
	HANDLE *tmp_thread_handle_ptr = &tmp_thread_handle;

	if (thread_handles == NULL) {
		return -1;
	}

	// Create sorting thread
	if (CreateThreadSimple(sortConsumer, thread_data_ptr, NULL, tmp_thread_handle_ptr) != 0) {
		return -1;
	}
	thread_handles[thread_data_ptr->prod_thread_count - 1] = tmp_thread_handle;

	// Itterate over producing thread count and open threads
	for (i=0;i<thread_data_ptr->prod_thread_count-1;i++) {
		// Open new thread and pass thread routine and pointer to data container
		if (CreateThreadSimple(PythThreadFunc, thread_data_ptr, NULL, tmp_thread_handle_ptr) != 0) {
			// Thread creation failed - Clean up
			for (j; j < i; j++) {
				CloseHandle(thread_handles[j]);
			}
			return -1;
		}
		thread_handles[i] = tmp_thread_handle;
	}

	return 0;
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
	//DWORD				wait_code;
	//BOOL				ret_val;
	thread_container	*thread_info = (thread_container*)lpParam;		// Get pointer to thread data container
	
	// Get producing thread counter
	int t_counter = 0;
	wait_res = WaitForSingleObject(thread_counter_mutex, INFINITE);
	if (wait_res == FALSE) {
		printf("Error when waiting for thread counter mutex. Last Error = 0x%x\n", GetLastError());
		return ERROR_CODE;
	}
	t_counter = thread_counter;
	release_res = ReleaseMutex(thread_counter_mutex);
	if (release_res == FALSE) {
		printf("Error when releasing thread counter mutexLast Error = 0x%x\n", GetLastError());
		return ERROR_CODE;
	}


	while (t_counter <= thread_info->prod_thread_count)				//	While threads haven't finished passing data to buffer and exit
	{
		// Consumer thread waits and decraments 'full' semaphore
		wait_res = WaitForSingleObject(buffer_full_sem, INFINITE);		
		if (wait_res != WAIT_OBJECT_0) {
			printf("Error when waiting for buffer semaphore!\n");
			return ERROR_CODE;
		}

		// Clear one entry from buffer
		if (clear_buffer(thread_info, false) != 0) {
			printf("Error when clearing buffer!\n");
			return ERROR_CODE;
		}

		// Consumer thread incrament 'empty' semaphore
		release_res = ReleaseSemaphore(									// incrament empty semaphore
			buffer_empty_sem,											// lReleasecount
			1, 															// Signal that exactly one cell was emptied
			NULL);														// lpPreviouscount
		if (release_res == FALSE) {
			printf("Error when releasing buffer semaphore!\n");
			return ERROR_CODE;
		}

		// Update t_counter
		wait_res = WaitForSingleObject(thread_counter_mutex, INFINITE);
		if (release_res == FALSE) {
			printf("Error when waiting for thread counter mutex\n");
			return ERROR_CODE;
		}
		t_counter = thread_counter;
		release_res = ReleaseMutex(thread_counter_mutex);
		if (release_res == FALSE) {
			printf("Error when releasing thread counter mutex\n");
			return ERROR_CODE;
		}

	}
	// At this point all threads have exited successfully after inserting data into buffer
	clear_buffer(thread_info, true);	// Clear buffer
	qsort(pythagorean_triple_lst, pythagorean_triple_counter, sizeof(triple), cmp_function);	// Sort the array
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
	int i;
	DWORD wait_res, release_res;

	for (i; i < thread_info->buffer_size; i++) {
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


/*
Function sortConsumer
------------------------
Description –
Parameters	–
Returns		–
*/
void closeThreadHandles(HANDLE *thread_handles,int thread_count) {
	for (int i = 0; i < thread_count-1; i++) {
		CloseHandle(thread_handles[i]);
	}
}


/*
Function CreateThreadSimple
------------------------
Description – A simplified API for creating threads. This function is just a wrapper for CreateThread.
Parameters	– p_start_routine: A pointer to the function to be executed by the thread,
			  test_app *tst_ptr: A pointer to the test being passed to the thread as argument,
			  p_thread_id: A pointer to a variable that receives the thread identifier,
*					If this parameter is NULL, the thread identifier is not returned.
			HANDLE *thread_handle_ptr: A pointer to the handle to the new thread. If the function
			fails the return value us NULL
Returns		– 0 for success, -1 for failure
*/
int CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, thread_container *data, LPDWORD p_thread_id, HANDLE *thread_handle_ptr)
{
	HANDLE thread_handle = NULL;
	*thread_handle_ptr = thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Received null pointer for thread routine");
		return -1;
	}


	if (NULL == data)
	{
		printf("Received null pointer for routine argument");
		return -1;
	}

	thread_handle = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		data,			 /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		return -1;
	}

	*thread_handle_ptr = thread_handle;
	return 0;
}