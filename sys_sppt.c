/***********************************************************************
*
*	Name:   sys_sppt.c
*
*	Purpose:  Support Routines for Modules 3 and 4 of MPX-PC
*
*	Sample Call:
*                     sys_init()
*                     sys_exit()
*
* 	Procedures In Module:
*                     sys_init - Sets up interrupt vector 60 for MPX-PC
*                     sys_exit - Resets interrupt vector 60 and exits to DOS
*
*
****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include "mpx.h"

void interrupt (*vect60)();  /* Storage for DOS int 60h */
							 /* interrupt vector.       */

void sys_init()
{
	/* set up interrupt vector for MPX sys_calls */
	vect60 = getvect(0x60);
	setvect(0x60,&sys_call);

	// set up the clock
	clock_open();

	// set up com
	com_open(&com_eflag, 1200);

	// set up printer
	prt_open(&prt_eflag);

	// set up con
	con_open(&con_eflag);
}


void sys_exit()
{
  disable();
  // restore the clock
  clock_close();

  // close IO devices
  com_close();
  con_close();
  prt_close();

  /* restore interrupt vector 60 and exit */
  setvect(0x60,vect60);

  enable();
  exit();
}

// dispatch the process at the head of the ready queue
void interrupt dispatch()
{
 disable();
 if (DEBUG_PRINTS) {printf("in dispatch \n ");}

 cop = ready_queue_locked;

 // kill zombie processes
 while (cop->state == ZOMBIE) {
   remove_pcb(&ready_queue_locked, cop);
   freemem(cop->loadaddr);
   free_pcb(pcb_list, cop);
   cop = ready_queue_locked;
 }

 // skip over suspended processes
 while (cop != NULL && cop->suspend == SUSPENDED) {
   cop = cop -> next;
 }

 if (DEBUG_PRINTS){
   if (cop == NULL) {
	 printf("!!cop null?!!!\n");
   } else {
	 printf("cop - %s\n", cop->name);
   }
 }

 remove_pcb(&ready_queue_locked, cop);
 cop->state = RUNNING;

 enable();
 _SP = cop -> stack_ptr;

}


void interrupt sys_call()
{
   static parm *parm_add;

   if (DEBUG_PRINTS) {printf("in sys_call \n ");}

	/* Save stack pointer for current process */
	cop->stack_ptr = _SP;

	/* Get address of sys_call parameters */
	parm_add = _SP + 0x1c; // parameter offset = 0x1C

	/* Note that you should save this parameter address
	   somewhere in the pcb as suggested below*/
	cop->parm_add = parm_add;

	_SP = &sys_stack[STACK_SIZE-1];

	// find out if the process wants to die... and kill it
	if (parm_add->op_number == EXIT_CODE) {
	  freemem(cop->loadaddr);
	  free_pcb(pcb_list, cop);
	  // NOTE: the process just made this sys_req so it cannot have any io
	} else {
	  IO_sched(cop);
	}

	dispatch();
}



