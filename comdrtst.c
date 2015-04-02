/***********************************************************************
*
*	Name:  comdrtst
*
*	Purpose:  Test program for comdrive read and write
*
*	Algorithm: Tests both comdrive for both input (com_read)
*                  and output (com_write)
*
***************************************************************************/

#include <stdio.h>
#include <dos.h>

extern  int com_read(char far *buff, int far *len);
extern  int com_write(char far *buff, int far *len);


int main()
{
 int e_flag;
 int tmp;
 int rc;
 char buffer[100];
 char prompt[20];

 int length;

 rc = com_open(&e_flag,1200);

 /* if bad return code, display value and exit */
 if (rc != 0)
  {
  printf("\nrc(com_open) = %d",rc);
  exit();
  }

 strcpy(buffer,"");
 while (strcmp(buffer,"quit") != 0)
  {
			 /* command prompt on com port */
    length = 16;
    e_flag = 0;
    rc = com_write("\015\012Enter string: ",&length);

    /* if bad return code, display value and exit */
    if (rc != 0)
    {
       printf("\nrc(com_write) = %x",rc);
       exit();
    }

    /* loop until write complete */
    while (e_flag == 0)
	printf("wait for write\n");

		   /* read string */
    length = 30;
    e_flag = 0;
    rc = com_read(buffer,&length);

		   /* if bad return code, display value and exit */

    if (rc != 0)
    {
       printf("\nrc(com_read) = %x",rc);
       exit();
    }

		 /* loop until read is done */
    while (e_flag == 0)
	printf("wait - reading\n");



    /* display string entered */
    e_flag = 0;
    tmp = 22;
	rc = com_write("\015\012string entered was: ",&tmp);

		   /* if bad return code, display value and exit */
	if (rc != 0)
	{
	printf("\nrc(com_write) = %x",rc);
	exit();
	}

			/* loop until write is done */
	while (e_flag == 0)
	  printf("wait for string to be written\n");


	e_flag = 0;
	rc = com_write(buffer,&length);

	/* if bad return code, display value and exit */
	if (rc != 0)
	{
	   printf("\nrc(com_write) = %x",rc);
	}
	   /* loop until write is done */
	  while (e_flag == 0);
}

  length = 29;
  e_flag = 0;
  rc = com_write ("\015\012Com Driver Test Completed\015\012",&length);

  /* if bad return code, display value and exit */
  if (rc != 0)
   {
   printf("\nrc(com_write) = %x",rc);
   }

  /* loop until last write is done */
  while (e_flag == 0);

  com_close();
  return 0;
 }

int IO_complete(int device, int *stkptr)
{
	 return 0;
}
