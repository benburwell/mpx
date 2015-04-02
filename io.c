/*
 * IO functionality
 * for MPX-OS
 * Ben Burwell & Averill Morash
 * CSI350 - Fall 2013
 */

#include <dos.h>
#include <stdio.h>
#include "mpx.h"

#define DS_OFFSET   6

void IO_sup(pcb *p) {
  char far *buffer;  // far pointer to the io buffer
  int far *length;   // far pointer to an int representing length of the io buffer
  unsigned *ds_add;  // data segment address
  int con_w = 0;     // flag for if the request was a con_write request
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
	p->state = READY;
  } else {
	p->state = BLOCKED;
  }
  enable();
  return;
}

// Handle completion of an IO request
int IO_complete(int device, int *stk_ptr) {

  pcb *current;
  dcb *d;

  disable();
  // get the dcb for the requested device
  switch (device){
	case CON:
	  d = &con;
	  // the con dcb isn't managed by con drive
	  // so we need to reset it here, but the
	  // other dcbs are handled in those files
	  // before calling io_complete
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

  // set its state to ready, unless it became
  // a zombie while it was waiting for io to complete
  if (current->state != ZOMBIE) {
	current->state = READY;
  }

  if (io_init_queue_locked != NULL) {
	// if there are pending IO requests schedule them
	current = io_init_queue_locked;
	while (current != NULL && current->parm_add->op_number != device) {
	  current = current->next;
	}

	if (current != NULL) {
	  // take the pcb out of the io_init queue
	  remove_pcb(&io_init_queue_locked, current);

	  // if it wasn't terminated while waiting for io...
	  if (current->state != ZOMBIE) {
		// ... schedule the io
		IO_sched(current);
	  } else {
		// send the zombie back to be ... dispatched ... muahaha
		insert_pcb(&ready_queue_locked, current, 0);
	  }
	}
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

  disable();
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

  // If the device is busy
  if (d->current_op != NO_OP) {
	disable();
	// put pcb in IO queue
	insert_pcb(&io_init_queue_locked, p, 1);
	p->state = BLOCKED;
	enable();
  } else {
	// If the device wasn't busy, start the io
	d->current_pcb = p;
	IO_sup(p);
  }
  enable();
  return 0;
}
