
/*  file:  main.c
 *
 *  This file contains the function main
 *  for the MPX Operating System - this is where
 *  the program's execution begins
 */

#include <stdio.h>
#include "mpx.h"


int main(void) {
	printf("... booting MPX\n\n");

	/* Put initialization code here */

	comhan();    /* Execute the command handler */

	return 0;
}

