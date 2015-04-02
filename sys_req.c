/***********************************************************************
*
*	Procedure Name:  sys_req
*
*
*	Purpose: System request for round robin dispatcher test.
*
*	Sample Call: sys_req(op_num,op_type,&buffer,&len)
*                    where
*                          op_num specifies the operation requested
*                          op_type specifies read or write
*                          buffer is the data buffer
*                          length is the number of characters to read or write
*
*	Algorithm:  Request to sys_call is made via an int 60.  Parameters
*                   passed to sys_req remain on the stack for use by sys_call.
*
************************************************************************/


#include <dos.h>

extern void interrupt sys_call();


void sys_req(int op_number,int op_type,char *buff_add,int *length)
{
  geninterrupt(0x60);   /* If all is set up right, this should      */
}                       /* invoke sys_call() to handle the request. */

