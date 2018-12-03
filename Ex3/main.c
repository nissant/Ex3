/*
Authors			- Eli Slavutsky (308882992) & Nisan Tagar (302344031)
Project Name	- Ex3
Description		- This program ..
*/

// Includes --------------------------------------------------------------------

#include "Thread_Manager.h"

int main(int argc, char *argv[]) {


	// Check that exactly 2 cmd line args are present
	if (argc < 3) {
		printf("Not enough input arguments, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}

	if (argc > 3) {
		printf("Too many input arguments, couldn't complete the task!\n");
		exit(EXIT_FAILURE);
	}


	exit(EXIT_SUCCESS);
}
