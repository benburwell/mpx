/***********************************************************************
*
*	Name:  testn.c
*
*	Purpose:  Provides statically-loaded (via linker) test processes
*                 for Round Robin Dispatcher of Module 3.
*
* 	Procedures In Module:
*                             test1 - test process
*                             test2 - test process
*                             test3 - test process
*                             test4 - test process
*                             test5 - test process
*
*       Name:       Processes test1, test2, test3, test4, and test5.
*
*	Algorithm:  Each process prints a message to the screen and gives up
*                   control to the dispatcher using sys_req.  Each process
*                   loops a certain number of times, displaying a message to
*                   the screen inside the loop.  (test1 loops 5 times, test2
*                   loops 10, test3 loops 15, test4 loops 20, and test5 loops
*                   25 times).  Each test process eventually requests
*                   termination.  If a dispatcher dispatches a test process
*                   after it requested termination, it prints a message
*                   indicating so, and the process starts over.
*
*****************************************************************************/

#include <dos.h>
#include "mpx.h"

#define op       1               /* dummy operation request code */
#define type     1               /* dummy operation type */

void test1(void)
{
     char buffer[8];
     int len;
     int i;

     while (1) {
	 	 for (i=1; i <= 5; i++) {
	       printf("\ntest1 dispatched; Value of i = %d",i);
	     	/* give up control to the dispatcher */
	       sys_req(op,type,buffer,&len);
	     }
	     sys_req(EXIT_CODE,0,buffer,&len);
	     printf ("\ntest1 dispatched after it exited!!!");
     }
}


void test2(void)
{
    char buffer[8];
    int len;
    int i;

    while (1) {
		for (i=1; i <= 10; i++) {
	    	printf("\ntest2 dispatched; Value of i = %d",i);
	    	/* give up control to the dispatcher */
	    	sys_req(op,type,buffer,&len);
	}
		sys_req(EXIT_CODE,0,buffer,&len);
		printf ("\ntest1 dispatched after it exited!!!");
    }
}

void test3(void)
{
    char buffer[8];
    int len;
    int i;
    while (1) {
		for (i=1; i <= 15; i++) {
	    	printf("\ntest3 dispatched; Value of i = %d",i);
	    	/* give up control to the dispatcher */
	    	sys_req(op,type,buffer,&len);
		}
		sys_req(EXIT_CODE,0,buffer,&len);
		printf ("\ntest3 dispatched after it exited!!!");
    }
}

void test4(void)
{
    char buffer[8];
    int len;
    int i;

    while (1) {
		for (i=1; i <= 20; i++) {
	    	printf("\ntest4 dispatched; Value of i = %d",i);
	    	/* give up control to the dispatcher */
	    	sys_req(op,type,buffer,&len);
		}
		sys_req(EXIT_CODE,0,buffer,&len);
		printf ("\ntest4 dispatched after it exited!!!");
    }
}

void test5(void)
{
    char buffer[8];
    int len;
    int i;

    while (1) {
		for (i=1; i <= 25; i++) {
	    	printf("\ntest5 dispatched; Value of i = %d",i);
	    	/* give up control to the dispatcher */
	    	sys_req(op,type,buffer,&len);
		}
		sys_req(EXIT_CODE,0,buffer,&len);
		printf ("\ntest5 dispatched after it exited!!!");
    }
}

