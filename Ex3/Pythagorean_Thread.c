/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program finds Pythagorean triplets using thread "parallelism", the triplets are sorted (using n,m comperator) and printed to output file
				- This module handles the pythgorean triplet calculation thread routines
*/

#include "Thread_Manager.h"
#include "Pythagorean_Thread.h"

DWORD WINAPI PythThreadFunc(LPVOID lpParam)
{
	int i;
	DWORD				wait_code_counter;
	BOOL				ret_val_counter;
	DWORD				wait_code;
	BOOL				ret_val;
	thread_container	*thread_info = (thread_container*)lpParam;	// Get pointer to relevcant data needed for execution in this thread

	//ReleaseSemaphore(buffer_full_sem, 1, NULL);
	for (i = 0; i < thread_info->max_number; i++)
	{
		wait_code = WaitForSingleObject(thread_info->ogen_mutex_array[i], INFINITE);   // access ogen mutex
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

void CalcABC(int n, int m, int *a, int *b, int *c)
{
	*a = (m*m) - (n*n);
	*b = 2 * n * m;
	*c = (m*m) + (n*n);
}

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