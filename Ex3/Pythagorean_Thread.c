/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triplets using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
				- This module handles the pythgorean triplet calculation thread routines
*/

// Includes --------------------------------------------------------------------
#include "Thread_Manager.h"
#include "Pythagorean_Thread.h"

/*
Function PythThreadFunc
------------------------
Description – This function is the main function that executes in the "calculation thread". the function goes over the "ogens" and try to find
			  an ogen and to calculate tripelt.
Parameters	 lpParam is the parameter sent when creating the tread. it contains a struct with relevant data needed for the calculation thread
			 such as main function parameters, mutex aray pointers , etc.
Returns		– SUCCESS_CODE for success, ERROR_CODE for failure
*/
DWORD WINAPI PythThreadFunc(LPVOID lpParam)
{
	int i;
	DWORD				wait_code_counter;
	BOOL				ret_val_counter;
	DWORD				wait_code;
	BOOL				ret_val;
	thread_container	*thread_info = (thread_container*)lpParam;	// Get pointer to relevcant data needed for execution in this thread

	for (i = 0; i < thread_info->max_number; i++)
	{
		wait_code = WaitForSingleObject(thread_info->ogen_mutex_array[i], INFINITE);   // access ogen i mutex
		if (WAIT_OBJECT_0 != wait_code)
		{
			printf("Error when waiting for ogen mutex in place %d\n",i);
			return ERROR_CODE;
		}
		if (thread_info->ogen_flag_array[i] == 1)			// if ogen is alreay taken 
		{
			ret_val = ReleaseMutex(thread_info->ogen_mutex_array[i]);	//relase mutex
			if (FALSE == ret_val)
			{
				printf("Error when releasing ogen in place %d mutex\n", i);
				return ERROR_CODE;
			}
			continue;										// continue the for loop
		}
		else 		// ogen is not taken. need to calculate triplet
		{
			thread_info->ogen_flag_array[i] = 1;			// mark ogen as calculated
			ret_val = ReleaseMutex(thread_info->ogen_mutex_array[i]);	//relase mutex
			if (FALSE == ret_val)
			{
				printf("Error when releasing ogen mutex in place %d\n", i);
				return ERROR_CODE;
			}
			if (CalcTripletPutInBuffer(i + 1, thread_info->max_number, thread_info) == ERROR_CODE)
			{
				return ERROR_CODE;
			}
		}
	}
	wait_code_counter = WaitForSingleObject(thread_counter_mutex, INFINITE);  // access to global counter - each thread updates the counter when finishing
	if (WAIT_OBJECT_0 != wait_code_counter)
	{
		printf("Error when waiting for global counter mutex\n");
		return ERROR_CODE;
	}
	// critical area - update counter
	thread_counter++;
	// finished critical area
	ret_val_counter = ReleaseMutex(thread_counter_mutex);	//release mutex of global counter
	if (FALSE == ret_val)
	{
		printf("Error when releasing global counter mutex\n");
		return ERROR_CODE;
	}
	return SUCCESS_CODE;
}

/*
Function CalcTripletPutInBuffer
------------------------
Description – This function calculates all the triplets with a specific "ogen number" (n).
Parameters	- n is the ogen. m is the max number recieved as a main function parameter. *thread_info contains a struct with relevant
			  data and pointers such as mutex arrays, buffer pointer, etc.
Returns		– SUCCESS_CODE for success, ERROR_CODE for failure
*/
int CalcTripletPutInBuffer(int n, int max, thread_container *thread_info)
{
	int a, b, c, m;
	for (m = n + 1; m <= max; m++)
	{
		if (((n % 2 == 0) && (m % 2 == 1)) || ((n % 2 == 1) && (m % 2 == 0))) // if one of the couple (n,m) is odd and other is even
		{
			if (FindGCD(n, m) == 1)
			{
				CalcABC(n, m, &a, &b, &c);
				if (PutInBuffer(n, m, a, b, c, thread_info)== ERROR_CODE)
				{
					return ERROR_CODE;
				}
			}
			else
				continue;
		}

		else			// n & m have GCD which is larger then 1;
			continue;
	}
	return SUCCESS_CODE;
}

/*
Function PutInBuffer
------------------------
Description – This function gets a triplet and puts it in an empty buffer slot
Parameters	- n,m are the two numbers which the tripelt is being calculated with accordance to. a,b,c are the tripelt itself.
			   *thread_info contains a struct with relevant data and pointers such as mutex arrays, buffer pointer, etc.
Returns		– SUCCESS_CODE for success, ERROR_CODE for failure
*/
int PutInBuffer(int n, int m, int a, int b, int c, thread_container *thread_info)
{
	DWORD				wait_code_sema;
	BOOL				release_val_sema;
	BOOL				release_val_mutex;
	int					slot;

	wait_code_sema = WaitForSingleObject(buffer_empty_sem, INFINITE);
	if (wait_code_sema != WAIT_OBJECT_0)
	{
		printf("Error when waiting for buffer empty semaphore\n");
		return ERROR_CODE;
	}
	slot = FindBuffSlot(thread_info);
	if (slot == ERROR_CODE)		//if the function returned with error
	{
		return ERROR_CODE;
	}

	else				// the function found an empty slot
	{
		thread_info->pyth_triple_buffer[slot].pythagorean.a = a;
		thread_info->pyth_triple_buffer[slot].pythagorean.b = b;
		thread_info->pyth_triple_buffer[slot].pythagorean.c = c;
		thread_info->pyth_triple_buffer[slot].pythagorean.n = n;
		thread_info->pyth_triple_buffer[slot].pythagorean.m = m;
		release_val_mutex = ReleaseMutex(thread_info->pyth_triple_buffer[slot].data_mutex);  // release mutex (the mutex is taken in FindBuffSlot func)
		if (FALSE == release_val_mutex)
		{
			printf("Error when releasing buff slot mutex in place %d\n", slot);
			return ERROR_CODE;
		}
		release_val_sema = ReleaseSemaphore(buffer_full_sem,1,NULL);
		if (FALSE == release_val_sema)
		{
			printf("failed realising \"buff_full_sema\" semaphore");
			return ERROR_CODE;
		}
		return SUCCESS_CODE;
	}
}

/*
Function FindGCD
------------------------
Description – This function finds the Greates Common Divisor of two numbers and returns it.
Parameters	- n,m are the two numbers that a common divisor is being calculated for.
Returns		– Returns the GCD
*/
int FindGCD(int n, int m)
{
	int  i, gcd;
	for (i = 1; i <= n && i <= m; i++)
	{
		// Checks if i is factor of both integers
		if (n%i == 0 && m%i == 0)
			gcd = i;
	}
	return gcd;
}

/*
Function CalcABC
------------------------
Description – This function calcualtes and updates the a,b,c pointers with the calculated tripelt.
Parameters	- n,m are the two numbers used to calculate the tripelt (a,b,c) according to the given formula
Returns		– Returns nothing. The function updates the a,b,c pointers with the calculation result.
*/
void CalcABC(int n, int m, int *a, int *b, int *c)
{
	*a = (m*m) - (n*n);
	*b = 2 * n * m;
	*c = (m*m) + (n*n);
}

/*
Function FindBuffSlot
------------------------
Description – This function goes over the buffer flag array and looks for a free slot in the buffer.
Parameters	-  *thread_info contains a struct with relevant data and pointers such as mutex arrays, buffer pointer, etc.
Returns		– Returns Error code for failure or the buffer slot index that found to be empty.
*/
int FindBuffSlot(thread_container *thread_info)
{
	DWORD	wait_code;
	BOOL	retval;
	int i;
	for (i = 0; i < thread_info->buffer_size; i++)
	{
		wait_code = WaitForSingleObject(thread_info->pyth_triple_buffer[i].data_mutex, INFINITE);
		if (wait_code != WAIT_OBJECT_0)
		{
			printf("Error when waiting for buffer mutex in place %d \n", i);
			return ERROR_CODE;
		}
		if (thread_info->pyth_triple_buffer[i].data_flag == 1)					// if buffer slot is full
		{
			retval = ReleaseMutex(thread_info->pyth_triple_buffer[i].data_mutex); // release mutex
			if (FALSE == retval)
			{
				printf("Error when releasing buff slot mutex in place %d\n", i);
				return ERROR_CODE;
			}
			continue;   // continue to look for next slot
		}
		else		// found empty slot in buffer
		{
			thread_info->pyth_triple_buffer[i].data_flag = 1;			 // mark the slot as taken
			return i;			// return the index of the slot
		}
	}
}