/*  file:  main.c
 *
 *  This file contains the function main
 *  for the MPX Operating System - this is where
 *  the program's execution begins
 */

#include <stdio.h>
#include <dos.h>
#include "mpx.h"

pcb * pcb_list;
pcb * ready_queue_locked;
pcb * io_init_queue_locked;
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

dcb com;
dcb prt;
dcb con;
int com_eflag;
int con_eflag;
int prt_eflag;

unsigned sys_stack[STACK_SIZE];

int main(void) {
	pcb * pcb_addr;
	dcb * d = &con;//test
	char *args[] = {"load", "idle"};

	printf("Booting MPX... \n");

	sys_init();

	if (DEBUG_PRINTS) printf("did init \n");

	d->event_flag = &con_eflag;

	if (DEBUG_PRINTS) printf("making chain \n");

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

	if (DEBUG_PRINTS) printf("building pcbs \n");

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

	// set up command handler as a PCB
	pcb_addr = get_pcb(pcb_list);
	build_pcb(pcb_addr, "ComHan", SYS_PROCESS, READY, NOT_SUSPENDED, 127, _CS,
	  (unsigned)comhan, _DS, 0x200);

	disable();
	insert_pcb(&ready_queue_locked, pcb_addr, 0);
	enable();

	if (DEBUG_PRINTS) printf("comhan in queue \n");


	// set up idle process
	cmd_load(args);

	if (DEBUG_PRINTS) printf("loaded \n");

	// update priority for idle process
	// we can't do this when initially calling because of
	// APP_PROCESS type priority validation
	pcb_addr = search_pcb(pcb_list, "idle");
	pcb_addr->priority = -128;
	pcb_addr->suspend = NOT_SUSPENDED;
	pcb_addr->type = SYS_PROCESS;

	if (DEBUG_PRINTS) printf("pri set \n");

	// ALREADY IN QUEUE THANKS TO CMD_LOAD
	//insert_pcb(&ready_queue, pcb_addr, 0);

	if (DEBUG_PRINTS) printf("idle in queue \n");

	dispatch();

	sys_exit();

	return 0;
}

