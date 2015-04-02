#include <stdio.h>
#include "mpx.h"

int main() {
  dcb com;
  char c;

  dcb_init(&com);

  for (c = 'a'; c <= 'z'; c++) {
	dcb_enqueue(&com, c);
  }

  for (c = 'a'; c <= 'z' + 2; c++) {
	printf("Dequeueing %c \n", dcb_dequeue(&com));
  }

  return 0;
}