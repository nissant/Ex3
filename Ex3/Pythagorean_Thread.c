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

	DWORD				wait_code;
	BOOL				ret_val;
	/*
	DWORD				exitcode;

	int					res;
	*/

	thread_container	*thread_info = (thread_container*)lpParam;	// Get pointer to relevcant data needed for execution in this thread
	int ogen;

	for (int i = 0; i < thread_info->max_number; i++)
	{
		wait_code = WaitForSingleObject(thread_info->ogen_mutex_array[i], INFINITE);   // access mutex
		if (WAIT_OBJECT_0 != wait_code)
		{
			printf("Error when waiting for ogen mutex\n");
			return ERROR_CODE;
		}
		if (thread_info->ogen_flag_array[i] == 1)			// if ogen is alreay taken 
		{
			ret_val = ReleaseMutex(thread_info->ogen_mutex_array[i]);	//relase mutex
			if (FALSE == ret_val)
			{
				printf("Error when releasing\n");
				return ERROR_CODE;
			}
			continue;										// continue the for loop
		}
		else 		// ogen is not taken. need to calculate triplet
		{
			thread_info->ogen_flag_array[i] = 1;
			ret_val = ReleaseMutex(thread_info->ogen_mutex_array[i]);	//relase mutex
			if (FALSE == ret_val)
			{
				printf("Error when releasing\n");
				return ERROR_CODE;
			}
			CalcTripelt(i + 1, thread_info->max_number, thread_info); /////////////////////////////////***************
		}
	}


}

int CalcTriplet(int n, int max, thread_container *thread_info)
{
	int a, b, c;
	for (int m = n + 1; m < max + 1; m++)
	{
		if (((n % 2 == 0) && (m % 2 == 1)) || ((n % 2 == 1) && (m % 2 == 0))) // if one of the couple (n,m) is odd and other is even
		{
			if (FindGCD(n, m) == 1)
			{
				ClaclABC(n, m, &a, &b, &c);
				PutTripInBuff(n, m, a, b, c, thread_info);          /////////////////////////////////////****************
			}
			else
				continue;
		}

		else			// n & m have GCD which is larger then 1;
			continue;
	}
}

int PutTripInBuff(int n, int m, int a, int b, int c, thread_container *thread_info)
{

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