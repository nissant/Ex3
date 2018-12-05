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


	exit(EXIT_SUCCESS);
}
