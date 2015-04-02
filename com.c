/**
 * MPX OS
 * CSI350 Operating Systems
 * Fall 2013
 * Averill Morash, Ben Burwell
 *
 * File: com.c
 */

#include <dos.h>
#include <stdio.h>
#include <string.h>
#include "mpx.h";


void interrupt (*vect0c)();

int com_opened;

// Open the com port
int com_open(int * flag, int rate) {
  unsigned char imr;
  unsigned char mcr;
  unsigned char ier;
  unsigned char lcr;

  disable();

  // Ensure com has not already been opened, if
  // it has, we must return.
  if (com_opened == NULL || com_opened == 0) {
	com_opened = 1;
  } else {
	return -2;
  }

  // Modify the interrupt vector table
  vect0c = getvect(0x0c);
  setvect(0x0c, &com_int);

  // set baud rate
  // first we need to set bit 7 of LCR
  // to be 1 so that we can set the baud
  // rate divisor.
  lcr = inportb(LCR);
  lcr = lcr | 0x80;
  outportb(LCR, lcr);

  // Now we can access the divisor.
  // Clock rate is 1.8432 MHz. We want
  // 1200 bps/baud.
  // 1843200 / (1200 * 16) = 96 = 0x0060
  // So the MSB is 0x00 and the LSB is 0x60

  // Ensure baud rate is valid
  if (rate != 1200) {
	return -3;
  }

  outport(BRDR_MSB, 0x00);
  outportb(BRDR_LSB, 0x60);

  // Now set the LCR
  lcr = inportb(LCR);

  // b7, b3, b2 = 0
  lcr = lcr & 0x73;

  // b0, b1 = 1
  lcr = lcr | 0x03;

  outportb(LCR, lcr);

  // set IMR to allow COM interrupts
  // set b4 = 0
  imr = inportb(IMR);
  imr = imr & 0xef;
  outportb(IMR, imr);

  // set MCR to enable interrupts from
  // the UART
  // on MCR, b3 = 1
  mcr = inportb(MCR);
  mcr = mcr | 0x08;
  outportb(MCR, mcr);

  // use IER to indicate which serial
  // interrupts are enabled
  // b0 = 1, b1 = 0
  ier = inportb(IER);
  ier = ier | 0x01;
  ier = ier & 0xfd;
  outportb(IER, ier);

  // set up the DCB
  com.event_flag = flag;
  dcb_init(&com);

  // enable interrupts
  enable();

  return 0;
}

// Com Close
void com_close() {
  disable();

  // restore MS-DOS com vector
  setvect(0x0c, vect0c);

  enable();
}

// Interrupt handler
void interrupt com_int() {
  int iir;
  int *lst_stk;

  // 0 indicates not finished, 1 indicated finshed
  int ret = 0;

  disable();

  // save base pointer
  lst_stk = _BP;

  // check IIR to see what caused the interrupt
  iir = inportb(IIR);
  // only look at bits 1, 2
  iir = iir & 0x06; // 0000-0110

  if (iir == 0x02) { // 0000-0010
	ret = com_write_int();
  } else if (iir == 0x04) { // 0000-0100
	ret = com_read_int();
  } else {
	// interrupt was not read or write
	return;
  }

  // if the operation is done, call IO_complete
  if (ret == 1) {
	IO_complete(COM, lst_stk);
  }

  // write end of interrupt to 8259
  outportb(0x20, 0x20);

  enable();
  return;

}

// Read from the com port
int com_read(char far * buffer, int far * length) {

  char dq;
  int i;

  disable();

  // if we haven't yet opened com, we can't proceed
  if (&com == NULL) {
	enable();
	return -1;
  }

  // check for invalid length
  if (*length < 1) {
	enable();
	return -3;
  }

  // check if DCB is free
  if (com.current_op != NO_OP) {
	// previous IO not yet complete
	enable();
	return -2;
  }

  if (buffer == NULL || length == NULL) {
	printf("error in com_read\n");
	enable();
	return;
  }
  // set up the DCB
  com.buffer = buffer;
  com.length = length;
  com.current_op = READ_OP;
  com.count = 0;

  // clear the buffer
  strcpy(buffer, "");

  // now we need to use the ring buffer to place typed-ahead
  // characters into the buffer

  // keep track of how many characters are copied from the
  // ring buffer.
  i = 0;

  while (com.ring_count > 0 && strlen(buffer) < *length) {
	dq = dcb_dequeue(&com);
	buffer[i] = dq;
	i++;
  }

  // place a null character at the end of the buffer
  buffer[i] = '\0';
  com.count = i;

  enable();
  return 0;
}

int com_write(char far * buffer, int far * length) {
  int ier;

  disable();
  // check that com has been initialized
  if (&com == NULL) {
	enable();
	return -1;
  }

  // check that com is not busy
  if (com.current_op != NO_OP) {
	enable();
	return -2;
  }

  // check that the length is valid
  if (*length < 1) {
	enable();
	return -3;
  }

  // initialize DCB
  com.buffer = buffer;
  com.length = length;
  com.current_op = WRITE_OP;
  com.count = 0;

  // enable THR interrupts
  ier = inportb(IER);
  // bit 1 should be 1
  ier = ier | 0x02;
  outportb(IER, ier);

  enable();
  return 0;
}

int com_read_int() {
  char rbr;

  rbr = inportb(RBR);

  if (com.current_op == READ_OP && com.count < *(com.length)) {
	  if (rbr == 0x0d || com.count == (*(com.length) - 1)) {
		com.buffer[com.count] = '\0';
		com.count++;
		*com.length = com.count;
		com.current_op = NO_OP;
		*(com.event_flag) = 1;
		return 1;
	  } else {
		com.buffer[com.count] = rbr;
		com.count++;
	  }
  } else {
	dcb_enqueue(&com, rbr);
  }

  return 0;
}

int com_write_int() {
  int ier;

  // if there are more characters to write
  if (*(com.length) > com.count) {
	// write a character
	outportb(THR, com.buffer[com.count]);
	com.count++;
	return 0;
  } else {
	// no chars to write
	// disable THR interrupts
	ier = inportb(IER);
	ier = ier & 0xfd;
	outportb(IER, ier);

	// set event flag
	com.current_op = NO_OP;
	*(com.event_flag) = 1;
	return 1;
  }
}
