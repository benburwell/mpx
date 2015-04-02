#include "mpx.h"
#include <string.h>
#include <stdio.h>
#include <dos.h>

#define STK_PSW    (STACK_SIZE - 1)
#define STK_CS     (STACK_SIZE - 2)
#define STK_IP     (STACK_SIZE - 3)
#define STK_DS     (STACK_SIZE - 9)
#define INIT_STACK (STACK_SIZE - 12)

/**
 * Returns the address of the PCB with the specified name
 */
pcb * search_pcb(pcb * list, char name[]) {
  pcb * current = list;

  // step through the chain
  do {
	if (stricmp(current->name, name) == 0) {
	  return current;
	}
	current = current->chain;
  } while (current != NULL);

  // nothing found
  return NULL;
}

/**
 * Returns the address of the first free PCB.
 * If all PCBs are in use, it returns NULL.
 */
pcb * get_pcb(pcb * list) {
  pcb * current = list;

  // step through the list
  do {
	if (current->type == FREE) {
	  return current;
	}
	current = current->chain;
  } while (current != NULL);

  // no free PCBs
  return NULL;
}

/**
 * Sets the specified PCB to be free.
 */
int free_pcb(pcb * list, pcb * addr) {
  pcb * current = list;

  do {
	if (current == addr) {

	  // check if PCB is already free
	  if (current->type == FREE) {
		// it is, so we don't free it
		return -2;
	  } else {
		// we need to free the PCB
		build_pcb(current, "        ", 0, -1, -1, 0, NULL, NULL, NULL, NULL);
		return 1;
	  }
	}
	current = current->chain;
  } while (current != NULL);

  // invalid PCB address
  return -1;
}

/**
 * Sets the properties of the given PCB.
 */
int build_pcb(pcb * addr, char name[], int type, int state,
			  int suspend, int priority, unsigned cs, unsigned ip,
			  unsigned ds, unsigned psw) {
  // check that address is valid
  if (addr == 0 || addr == NULL) {
	return -1;
  }

  // check that type is FREE, SYS_PROCESS, APP_PROCESS
  if (type != FREE && type != SYS_PROCESS && type != APP_PROCESS) {
	return -2;
  }

  // check that state is READY, RUNNING, BLOCKED, or -1 for free
  if (state != READY && state != RUNNING &&
	  state != BLOCKED && state != -1) {
	return -3;
  }

  // check that suspend is SUSPENDED, NOT_SUSPENDED or -1 for free
  if (suspend != SUSPENDED && suspend != NOT_SUSPENDED && suspend != -1) {
	return -4;
  }

  strcpy(addr->name, name);
  addr->type = type;
  addr->state = state;
  addr->suspend = suspend;
  addr->next = NULL;
  addr->prev = NULL;
  addr->priority = priority;

  // set up the stack
  addr->stack[STK_PSW] = psw;
  addr->stack[STK_CS] = cs;
  addr->stack[STK_IP] = ip;
  addr->stack[STK_DS] = ds;

  addr->stack_ptr =(unsigned) &(addr->stack[INIT_STACK]);
  addr->loadaddr = cs;
  return 1;
}

/**
 * Puts the specified PCB into the specified queue, using either
 * FIFO or priority queue (method==0: priority, method==1: FIFO)
 */
int insert_pcb(pcb **queue, pcb * addr, int method) {
  pcb * current = *queue;
  pcb * one_after;

  disable();

  // if there's nothing in the queue yet, make the PCB the start
  if (current == NULL) {
	*queue = addr;
	enable();
	return 1;
  }

  if (method == 1) {
	// insert at end of queue

	// skip to the end of the queue
	while(current->next != NULL) {
	  current = current->next;
	}

	// add the PCB
	current->next = addr;
	addr->next = NULL;
	addr->prev = current;
	enable();
	return 1;
  } else if (method == 0) {
	// insert in priority order

	// if we need to skip forward at all...
	if (current->priority >= addr->priority) {

	  // ...do it
	  while (current->next != NULL &&
			 (current->next)->priority >= addr->priority) {
			   current = current->next;
	  }

	  // and set the pointers
	  one_after = current->next;
	  current->next = addr;
	  addr->next = one_after;
	  addr->prev = current;
	  if (one_after != NULL) {
		one_after->prev = addr;
	  }
	} else {

	  // otherwise, put it at the head
	  current->prev = addr;
	  addr->next = current;
	  *queue = addr;
	}
	enable();
	return 1;
  } else {
	enable();
	// There was a problem, return error code
	return -1;
  }
}

/**
 * Takes the PCB out of the specified queue
 */
int remove_pcb(pcb **queue, pcb * addr) {
  pcb * current = *queue;

  disable();
  // are we removing the head?
  if (addr == *queue) {
	*queue = addr->next;
	if (addr->next != NULL) {
	  (addr->next)->prev = NULL;
	  addr->next = NULL;
	}
	return 0;
  }

  // not removing the head:
  do {
	if (current->next == addr) {
	  current->next = addr->next;
	  if (current->next != NULL) {
		(current->next)->prev = current;
	  }
	  addr->next = NULL;
	  addr->prev = NULL;
	  return 0;
	}
	current = current->next;
  } while (current != NULL);
  enable();
  return -1;
}
