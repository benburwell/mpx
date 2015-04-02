/*
 * Clock support for MPX-OS
 * Ben Burwell & Averill Morash
 * CSI350 - Fall 2013
 */

#include <dos.h>
#include "mpx.h"

void interrupt (*vect08)();
unsigned long clock;         /* The clock counter       */

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