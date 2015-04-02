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
void interrupt (*vect08)();  /* Storage for DOS int 08h */
							 /* interrupt vector.       */
unsigned long clock;         /* The clock counter       */

void sys_init()
{
	/* set up interrupt vector for MPX sys_calls */
	vect60 = getvect(0x60);
	setvect(0x60,&sys_call);

	// set up the clock
	clock_open();
}


void sys_exit()
{
  // restore the clock
  clock_close();

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

/*
 * Set up MXP clock
 */
void clock_open() {
	unsigned char imr;

	disable();
	/* set up interrupt vector for timer interrupts */
	vect08 = getvect(0x08);
	setvect(0x08,&clock_int);

	// set the timer mode
	outportb(CMR, 0x36);
	outportb(0x40, 0);
	outportb(0x40, 0);

	// set the count clock
	clock = 0L;

	// enable timer interrupts
	imr = inportb(IMR);       // Get current IMR
	imr = imr & CLOCK_ENABLE; // Clear timer bit
	outportb(IMR, imr);       // Set new IMR

	enable();
}

/*
 * Restore the MS DOS clock
 */
void clock_close() {
  disable();
  setvect(0x08,vect08);
  enable();
}

void interrupt clock_int() {
  disable();
  // if it's been 24 hours, reset clock to 0
  if (clock >= 1572462L) {
	clock = 0L;
  } else {
	clock++;
  }
  outportb(0x20, 0x20);
  enable();
}

void stop_clock() {
  unsigned char imr;
  disable();

  imr = inportb(IMR);        // Get the old imr
  imr = imr | CLOCK_DISABLE; // Disable timer interrupts
  outportb(IMR, imr);        // Set the new imr

  enable();
}

/*
 * Enables the timer interrupt
 */
void start_clock() {
  unsigned char imr;
  disable();

  imr = inportb(IMR);        // Get the old imr
  imr = imr & CLOCK_ENABLE;  // Enable timer interrupts
  outportb(IMR, imr);        // Set the new imr

  enable();
}

/*
 * Sets the value of the clock
 */
int set_clock(int hr, int m, int s) {
  unsigned long ticks;

  // Validate input
  if (hr > 23 || hr < 0 || m > 59 || m < 0 || s > 59 || s < 0) {
	return -1;
  }

  // set clock
  ticks = 0L;
  ticks  = ((unsigned long)hr * 3600L * 91L / 5L);
  ticks += ((unsigned long)m  * 60L   * 91L / 5L);
  ticks += ((unsigned long)s          * 91L / 5L);

  disable();
  clock = ticks;
  enable();

  return 0;
}

/*
 * Gets the value of the clock
 */
void read_clock(int *hr, int *m, int *s) {
  unsigned long ticks;
  long total_seconds = 0L;
  int l_hr = 0;
  int l_m = 0; //local vars for hr, m

  disable();
  ticks = clock;
  enable();

  total_seconds = ticks * 10L;
  total_seconds = total_seconds / 182L;

  //count the whole hours
  while (total_seconds >= 3600L) {
	l_hr++;
	total_seconds = total_seconds - 3600L;
  }

  //count the whole minutes
  while (total_seconds >= 60L) {
	l_m++;
	total_seconds = total_seconds - 60L;
  }

  *hr = l_hr;
  *m = l_m;
  *s = total_seconds;
}
