/**
 *  Operating Systems
 *  Mod 6
 *  Averill and Ben
 *  Writing awesome printer stuff.
 */

#include <dos.h>
#include <stdio.h>
#include "mpx.h";

void interrupt (*vect0f)();

int prt_opened;

int prt_open(int * prt_flag) {
  unsigned char imr;
  unsigned char pcr;

  disable();

  if (prt_opened == NULL || prt_opened == 0) {
	prt_opened = 1;
  } else {
	return -2;
  }

  // set up vector interrupt
  vect0f = getvect(0x0f);
  setvect(0x0f, &prt_int);

  // enable printer interrupts on 8259
  imr = inportb(IMR);
  imr = imr & 0x7f; //0111-1111
  outportb(IMR, imr);

  // clear init bit (b2), set select bit (b3)
  // 0000 1010
  outportb(PCR, 0x0a);

  // initialize a printer control block
  prt.current_op = NO_OP;
  prt.event_flag = prt_flag;
  prt.count = 0;

  enable();
  return 0;
}

int prt_write(char far *buffer, int far *length) {
  unsigned char pcr;

  disable();
  // check that prt isn't busy
  if (prt.current_op != NO_OP) {
	return -2;
  }

  // check that length is valid
  if (*length < 1) {
	return -3;
  }

  //save buffer and length in prt dcb
  prt.buffer = buffer;
  prt.length = length;
  prt.current_op = WRITE_OP;
  prt.count = 0;

  // enable printer interrupts on PCR
  // 0001 1010
  outportb(PCR, 0x1a);

  // strobe printer
  // 0001 1011
  outportb(PCR, 0x1d);

  // print a null
  // NOTE: something is not right here because when i print something
  // other than a null it still doesn't print
  outportb(PDR, 0x00);

  // unstrobe printer
  // 0001 1010
  outportb(PCR, 0x1c);

  enable();
  return 0;
}

int prt_close() {
  unsigned char pcr;
  unsigned char imr;

  disable();

  // reset printer
  // set init bit (b2) to 1 all else clear
  // 0000 0100
  outportb(PCR, 0x04);

  //disable printer interrupts
  imr = inportb(IMR);
  imr = imr | 0x80;
  outportb(IMR, imr);

  // restore ms-dos's printer interrupt vector
  setvect(0x0f, vect0f);

  enable();
  return 0;
}

void interrupt prt_int() {
  unsigned char pcr;
  int *lst_stk;
  float temp;

  lst_stk = _BP;

  // if no more char to write
  if (prt.count == *(prt.length)) {
	// disable printer interrupts at PCR
	// clear b4 on PCR
	pcr = inportb(PCR);
	pcr = pcr & 0xef; // 1110-1111
	outportb(PCR, pcr);

	// set event flag
	prt.current_op = NO_OP;
	*(prt.event_flag) = 1;

	// call io-complete
	// TODO: the book says (pg 86) that there should be stuff passed to io-complete
	IO_complete(PRT, lst_stk);

	// send end of interrupt signal
	outportb(0x20, 0x20);
  } else {

	// strobe printer
	// 0001 1101
	outportb(PCR, 0x1d);

	// write char to PDR
	outportb(PDR, prt.buffer[prt.count]);
	prt.count++;

	// unstrobe printer
	// 0001 1100
	outportb(PCR, 0x1c);
  }

  // send EOI
  outportb(0x20, 0x20);
}