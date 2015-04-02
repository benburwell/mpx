/***********************************************************************
*
*	Name:   comdrtsw
*
*	Purpose: Test program for write portion of comdrive
*
*	Algorithm: Tests com_open(), com_write(), and com_close()
*
************************************************************************/

#include <stdio.h>
#include <dos.h>;

extern int com_read(char far *buff, int far *len);
extern int com_write(char far *buff, int far *len);

int main()
 {
 int e_flag;
 int tmp;
 int rc;
 char buffer[100];
 int length;

 /* open com port */

 rc = com_open(&e_flag,1200);
 if ( rc != 0)
  {
  printf("\nrc(open) = %d",rc);
  exit();
  }

 /* prepare test string */
 strcpy(buffer,"This is a test of the com_write procedure  .. 1234567890......\015\012");
 length = 64;

				 /* print test string 25 times */
 for (tmp = 1; tmp <25; tmp++)
  {
      e_flag = 0;                       /* Clear event flag */
      rc = com_write(buffer,&length);   /* Start COM write  */
      if (rc != 0)
      {
      printf("\nrc(write) = %d",rc);
      exit();
      }

				   /* Loop until com write is done.    */
				   /* This look like an infinite loop */
      while (e_flag == 0)
	  printf("waiting %d\n",tmp);
  }

  e_flag = 0;
  length = 28;
  rc = com_write("End of Com Driver Test ...\015\012",&length);
  if (rc != 0)
   {
   printf("\nrc(write) = %d",rc);
   exit();
   }

  /* loop until com write is done */
  while (e_flag == 0);

 com_close();
 }

int IO_complete(int device, int *stkptr)
{
    return 0;
}
