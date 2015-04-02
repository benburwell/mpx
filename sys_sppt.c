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
}


void sys_exit()
{
	/* restore interrupt vector 60 and exit */

	setvect(0x60,vect60);
	exit();
}


void interrupt dispatch()
{
 /*  MOD 4 dispatch */
 do {
   cop = cop -> next;
 } while (cop != NULL && cop->suspend == SUSPENDED);

 if (cop == NULL) {
   _SP = sp_save;
 } else {
   _SP = cop -> stack_ptr;
 }
}


void interrupt sys_call()
{
	struct parm {
		int op_number;
		int op_type;
		char *buffer;
		int *length;
	};

	static struct parm *parm_add;

	/* Save stack pointer for current process */
	cop->stack_ptr = _SP;

	/* Get address of sys_call parameters */
	parm_add = _SP + 0x1c; // parameter offset = 0x1C

	/* Note that you should save this parameter address
	   somewhere in the pcb as suggested below*/
	cop->parm_add = parm_add;

	// find out if the process wants to die... and kill it
	if (parm_add->op_number == EXIT_CODE) {
	  cop->suspend = SUSPENDED;
	}

	dispatch();
}
