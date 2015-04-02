/*  file:  main.c
 *
 *  This file contains the function main
 *  for the MPX Operating System - this is where
 *  the program's execution begins
 */

#include <stdio.h>
#include "mpx.h"

pcb * pcb_list;
pcb * ready_queue;
pcb * io_init_queue;

int main(void) {

	/* Put initialization code here */

	// allocate memory for PCBs
	pcb pcb0;
	pcb pcb1;
	pcb pcb2;
	pcb pcb3;
	pcb pcb4;
	pcb pcb5;
	pcb pcb6;
	pcb pcb7;
	pcb pcb8;
	pcb pcb9;
	pcb pcb10;
	pcb pcb11;

	// create the chain
	pcb0.chain  = &pcb1;
	pcb1.chain  = &pcb2;
	pcb2.chain  = &pcb3;
	pcb3.chain  = &pcb4;
	pcb4.chain  = &pcb5;
	pcb5.chain  = &pcb6;
	pcb6.chain  = &pcb7;
	pcb7.chain  = &pcb8;
	pcb8.chain  = &pcb9;
	pcb9.chain  = &pcb10;
	pcb10.chain = &pcb11;
	pcb11.chain = NULL;

	build_pcb(&pcb0, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb1, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb2, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb3, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb4, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb5, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb6, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb7, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb8, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb9, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb10, "        ", FREE, -1, -1, 0);
	build_pcb(&pcb11, "        ", FREE, -1, -1, 0);

	pcb_list = &pcb0;

	printf("... booting MPX\n\n");

	comhan();    /* Execute the command handler */

	return 0;
}

