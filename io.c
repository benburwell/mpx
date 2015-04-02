/*
 * IO functionality
 * for MPX-OS
 * Ben Burwell & Averill Morash
 * CSI350 - Fall 2013
 */

#include <dos.h>
#include <stdio.h>
#include "mpx.h"

#define DS_OFFSET   6 // was 3 before

int count = 0;
void IO_sup(pcb *p) {
  char far *buffer;
  int far *length;
  unsigned *ds_add;
  int con_w = 0;
  int rc;

  disable();

  if (DEBUG_PRINTS) { printf("in io_sup \n");}

  //make the far pointers (see page 107)
  ds_add = p->stack_ptr + DS_OFFSET;
  buffer = MK_FP(*ds_add, p->parm_add->buffer);
  length = MK_FP(*ds_add, p->parm_add->length);

  //call com_read or equivalent
  switch (p->parm_add->op_number){
	case COM:
	  // IO_sched should have validated input
	  // Assume that op_type is either read or write
	  if (p->parm_add->op_type == READ) {
		 if (DEBUG_PRINTS){printf("com read req \n");}
		 com_eflag = 0;
		 rc = com_read(buffer, length);
	  } else {
		 if (DEBUG_PRINTS) {printf("com write req \n");}
		 com_eflag = 0;
		 rc = com_write(buffer, length);
	  }
	  break;
	case CON:
	  if (p->parm_add->op_type == READ) {
		if (DEBUG_PRINTS){printf("con read req\n");}
		con_eflag = 0;
		con.current_op = READ_OP;
		rc = con_read(buffer, length);
	  } else {
		if (DEBUG_PRINTS){printf("con_write req\n");}
		con_w = 1;
		rc = con_write(buffer, length);
	  }
	  break;
	case PRT:
	  if (DEBUG_PRINTS){printf("prt write req\n");}
	  prt_eflag = 0;
	  rc = prt_write(buffer, length);
	  break;
  }

  if (rc < 0) {
	printf("Somthing went wrong in io_sup\n");
  }

  // put the pcb back in the ready queue, if it was a con_write
  if (con_w == 1) {
	insert_pcb(&ready_queue_locked, p,0);
  }
  enable();
  return;
}

// Handle completion of an IO request
int IO_complete(int device, int *stk_ptr) {

  pcb *current;
  dcb *d;

  disable();
  switch (device){
	case CON:
	  d = &con;
	  d->current_op = NO_OP;
	  break;
	case COM:
	  d = &com;
	  break;
	case PRT:
	  d = &prt;
	  break;
  }

  if (DEBUG_PRINTS){printf("in io complete\n");}

  // save interrupted process's stack pointer
  cop->stack_ptr = stk_ptr;

  // insert running process into ready queue
  insert_pcb(&ready_queue_locked, cop, 0);

  // set running process to ready state
  cop->state = READY;

  // get address of PCB whose IO just completed
  current = d->current_pcb;

  // insert into ready queue
  insert_pcb(&ready_queue_locked, current, 0);

  // set its state to ready
  current->state = READY;

  if (strcmp(d->current_pcb->name, "iocom2")==0) {
	count++;
	if (count > 20) {
	d = &com;
	}
  }
  if (d->pcb_head != NULL) {
	// if there are pending IO requests for this device
	//schedule them (IO_sched)
	remove_pcb(&(d->pcb_head), d->pcb_head);
	IO_sched(d->pcb_head);
  }

  outportb(0x20,0x20); // EOI signal
  enable();
  dispatch();

  return 0;
}

// Service an IO request
// Returns:
//    0 for success
//   -1 for invalid operation
//   -2 for invalid device
int IO_sched(pcb *p) {

  dcb *d;
  int op_num;
  int op_type;

  if (DEBUG_PRINTS) {printf("in io sched\n");}

  // determine which device is requested
  op_num = p->parm_add->op_number;
  op_type = p->parm_add->op_type;

  switch (op_num) {
	case CON:
	  if (op_type != READ
		&& op_type != WRITE) {
		  return -1;
	  }
	  d = &con;
	  break;
	case COM:
	  if (op_type != READ
		&& op_type != WRITE) {
		  return -1;
	  }
	  d = &com;
	  break;
	case PRT:
	  if (op_type != WRITE) {
		return -1;
	  }
	  d = &prt;
	  break;
	default:
	  return -2;
  }

  if (d->current_op != NO_OP) {
	disable();
	// put pcb in IO queue
	insert_pcb(&(d->pcb_head), p, 1);
	// take pcb out of ready q
	remove_pcb(&ready_queue_locked, p);
	enable();
  } else {
	d->current_pcb = p;
	IO_sup(p);
  }

  return 0;
}
