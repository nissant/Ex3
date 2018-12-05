#pragma once
/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program ..
				- This module handles the organizing/sorting thread and access to memory buffer
				- This is also the main project header file
*/


// Includes --------------------------------------------------------------------
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tchar.h>


#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

// Constants -------------------------------------------------------------------

// Variables -------------------------------------------------------------------

HANDLE buffer_full_sem;
HANDLE buffer_empty_sem;

typedef struct buffer{
	int a, b, c;							// Pythagorean triple integer
	int m, n;								// Pythagorean triplet parameters 
	char data_flag;							// Flag that signals if data is available in the corresponding index 
	HANDLE data_mutex;						// Mutex that protects the data in cell
} data_buffer;

typedef struct container{
	data_buffer *pyth_triple_buffer;		// Pointer to the allocated pythagorean triple buffer
	int buffer_size;						// Size of allocated buffer
	
	HANDLE *ogen_mutex_array;				// Pointer to array of mutex for each ogen
	char *ogen_flag_array;					// Pointer to array of flags for each ogen
	int max_number;							// Max number for the ogen

} thread_container;


// Function Declarations -------------------------------------------------------
DWORD WINAPI sortConsumer(LPVOID lpParam);
int initThreadContainer(char **argv, thread_container *thread_data_ptr);

#endif