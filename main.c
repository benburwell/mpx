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
pcb * cop;

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

unsigned sys_stack[STACK_SIZE];

int main(void) {
	pcb * pcb_addr;
	printf("Booting MPX... \n");
	/* Put initialization code here */

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

	build_pcb(&pcb0,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb1,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb2,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb3,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb4,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb5,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb6,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb7,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb8,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb9,  "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb10, "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);
	build_pcb(&pcb11, "        ", FREE, -1, -1, 0, NULL, NULL, NULL, NULL);

	pcb_list = &pcb0;

	// initialize the currently running process
	cop = NULL;

	// initialize the mod3 test pcbs
	/*
	pcb_addr = get_pcb(pcb_list);
	build_pcb(pcb_addr, "test1", APP_PROCESS, READY, NOT_SUSPENDED, 0, _CS,
			  (unsigned)test1, _DS, 0x200);
	insert_pcb(&ready_queue, pcb_addr, 0);

	pcb_addr = get_pcb(pcb_list);
	build_pcb(pcb_addr, "test2",APP_PROCESS, READY, NOT_SUSPENDED, 0, _CS,
			  (unsigned)test2, _DS, 0x200);
	insert_pcb(&ready_queue, pcb_addr, 0);

	pcb_addr = get_pcb(pcb_list);
	build_pcb(pcb_addr, "test3", APP_PROCESS, READY, NOT_SUSPENDED, 0, _CS,
			  (unsigned)test3, _DS, 0x200);
	insert_pcb(&ready_queue, pcb_addr, 0);

	pcb_addr = get_pcb(pcb_list);
	build_pcb(pcb_addr, "test4", APP_PROCESS, READY, NOT_SUSPENDED, 0, _CS,
			  (unsigned)test4, _DS, 0x200);
	insert_pcb(&ready_queue, pcb_addr, 0);

	pcb_addr = get_pcb(pcb_list);
	build_pcb(pcb_addr, "test5",  APP_PROCESS, READY, NOT_SUSPENDED, 0, _CS,
			  (unsigned)test5, _DS, 0x200);
	insert_pcb(&ready_queue, pcb_addr, 0);


	// initialize the system stack
	_SP = (unsigned) &sys_stack[STACK_SIZE -1];

	//get the show on the road!
	sys_init();
	dispatch();
	*/

	sys_init();
	comhan();    /* Execute the command handler */
	sys_exit();

	return 0;
}

