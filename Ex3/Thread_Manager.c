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
		NULL);				/* un-named */

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
	DWORD thread_id;

	if (thread_handles == NULL) {
		return -1;
	}

	// Itterate over producing thread count and open threads
	for (i=0;i<thread_data_ptr->prod_thread_count;i++) {
		// Open new thread and pass thread routine and pointer to data container
		thread_handles[i] = CreateThread(
			NULL,					/*  default security attributes */
			0,						/*  use default stack size */
			PythThreadFunc,			/*  thread function */
			thread_data_ptr,		/*  argument to thread function */
			0,						/*  use default creation flags */
			&thread_id);			/*  returns the thread identifier */

		if (thread_handles[i] == NULL) {
			printf("Failed to create producing thread number %d!\n",i);
			// Thread creation failed - Clean up
			for (j=1; j < i; j++) {
				CloseHandle(thread_handles[j]);
			}
			CloseHandle(thread_handles[0]);
			return -1;
		}
	}
	// Create sorting thread
	thread_handles[thread_data_ptr->prod_thread_count] = CreateThread(
		NULL,					/*  default security attributes */
		0,						/*  use default stack size */
		sortConsumer,			/*  thread function */
		thread_data_ptr,		/*  argument to thread function */
		0,						/*  use default creation flags */
		&thread_id);			/*  returns the thread identifier */

	if (thread_handles[thread_data_ptr->prod_thread_count] == NULL) {
		printf("Failed to create sort thread!\n");
		return -1;
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
	DWORD				wait_res; 
	DWORD				release_res;
	BOOL				ret_val;
	int					t_counter = 0;
	thread_container	*thread_info = (thread_container*)lpParam;		// Get pointer to thread data container
	bool				keepGoing = TRUE;
	
	wait_res = WaitForSingleObject(thread_counter_mutex, INFINITE);  // access to global counter - each thread updates the counter when finishing
	if (WAIT_OBJECT_0 != wait_res)
	{
		printf("Error when waiting for global counter mutex\n");
		return ERROR_CODE;
	}
	// critical area - update counter
	t_counter = thread_counter;
	// finished critical area
	ret_val = ReleaseMutex(thread_counter_mutex);	//release mutex of global counter
	if (FALSE == ret_val)
	{
		printf("Error when releasing global counter mutex\n");
		return ERROR_CODE;
	}

	while (t_counter < thread_info->prod_thread_count)				//	While threads haven't finished passing data to buffer and exit
	{
		
		if (isDataInbuffer(thread_info)) { 
			// Consumer thread waits and decraments 'full' semaphore - It's consumption time!
			wait_res = WaitForSingleObject(buffer_full_sem, INFINITE);
			if (wait_res != WAIT_OBJECT_0) {
				printf("Error when waiting for buffer semaphore!\n");
				return ERROR_CODE;
			}
		}
		else {	// No data in buffer
			continue;
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

		// access to global counter - each thread updates the counter when finishing
		//wait_res = WaitForSingleObject(thread_counter_mutex, INFINITE);  
		//if (WAIT_OBJECT_0 != wait_res)
	//	{
	//		printf("Error when waiting for global counter mutex\n");
	//		return ERROR_CODE;
	//	}
		// critical area - update counter
		t_counter = thread_counter;
		// finished critical area
	//	ret_val = ReleaseMutex(thread_counter_mutex);	//release mutex of global counter
	//	if (FALSE == ret_val)
	//	{
	//		printf("Error when releasing global counter mutex\n");
	//		return ERROR_CODE;
	//	}
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

	for (i=0; i < thread_info->buffer_size; i++) {
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
bool isDataInbuffer(thread_container *thread_info) {
	int i;
	DWORD wait_res, release_res;

	for (i = 0; i < thread_info->buffer_size; i++) {
		if (thread_info->pyth_triple_buffer[i].data_flag == 1) {			// Found data availabe in buffer entry
			return true;
		}
		else{
			continue;
		}
	}
	// No data found in buffer
	return false;
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
Function sortConsumer
------------------------
Description –
Parameters	–
Returns		–
*/
int checkArgs(int argc, int num) {
	if (argc < num+1) {
		printf("Not enough input arguments, couldn't complete the task!\n");
	}

	else if (argc > num+1) {
		printf("Too many input arguments, couldn't complete the task!\n");
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
int checkThreadsAndPrint(HANDLE *thread_handles, int thread_count, thread_container *thread_data_ptr, char *path) {
	DWORD ExitCode;
	DWORD *lpExitCode = &ExitCode;
	int i;
	int errFlag=0;
	// Itterate over test list and check thread exit codes
	for (i = 0; i < thread_count;i++) {
		if (GetExitCodeThread(thread_handles[i], lpExitCode)) {
			if (ExitCode != 0) {
				if (i == thread_count-1)
					printf("Error in sorting thread execution! Exit code = 0x%x\n", ExitCode);
				else
					printf("Error in calculation thread %d execution! Exit code = 0x%x\n", i, ExitCode);
				errFlag++;
			}
		}
	}
	if (errFlag != 0)
		return -1;

	FILE *fp_results = fopen(path, "w");
	if (fp_results == NULL)							// Handle errors
	{
		printf("Error when opening output file stream!\n");
		return (-1);
	}

	for (i = 0; i < pythagorean_triple_counter;i++) {
		fprintf(fp_results, "%d,%d,%d\n", pythagorean_triple_lst[i].a, pythagorean_triple_lst[i].b, pythagorean_triple_lst[i].c);
	}
	fclose(fp_results);
	return 0;
}

