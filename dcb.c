/**
 * MPX OS
 * Operating Systems - Fall 2013
 * Averill Morash, Ben Burwell
 */

#include "mpx.h"

/**
 * Returns the character at the front of the ring queue
 * associated with the dcb, or '\0' if the queue is empty.
 */
char dcb_dequeue(dcb *d) {
  char c;

  if (d->ring_count == 0) {
	return '\0';
  }

  c = d->ring[d->ring_front];

  if (d->ring_front == d->ring_size - 1) {
	d->ring_front = 0;
  } else {
	d->ring_front++;
  }

  d->ring_count--;

  return c;
}

/**
 * Adds the given character to the end of the ring queue
 * in the given dcb if there is room
 */
void dcb_enqueue(dcb *d, char c) {
  if (d->ring_count >= d->ring_size) {
	return;
  }

  if (d->ring_rear == d->ring_size - 1) {
	d->ring_rear = 0;
  } else {
	d->ring_rear++;
  }

  d->ring[d->ring_rear] = c;

  d->ring_count++;

  return;
}

/**
 * Set up a dcb's circular queue, and set the current_op to NO_OP
 */
void dcb_init(dcb *d) {
  d->ring_front = 0;
  d->ring_rear = -1;
  d->ring_size = INPUT_BUFFER_MAX;
  d->ring_count = 0;
  d->current_op = NO_OP;
  return;
}
