/***********************************************************************
*
*	Name:  comprtts
*
*	Purpose:   Test program for both comdrive and prtdrive
*
*	Algorithm: Tests both comdriver and prtdriver with overlapped
*                  out put to both devices.
*
*************************************************************************/

int prt_flag;
int com_flag;
int tmp;
int rc;
char buffer[100];
char prompt[20];
int length;
extern  int com_read(char far *buff, int far *len);
extern  int com_write(char far *buff, int far *len);
extern  int prt_write(char far *buff,int far *len);


int main()
{

 rc = prt_open(&prt_flag);
 rc = com_open(&com_flag,1200);


 strcpy(buffer,"");

 while (strcmp(buffer,"quit") != 0)
  {
  /* command prompt on com port */
  length = 16;
  com_flag = 0;
  rc = com_write("\015\012Enter string: ",&length);

			  /* loop until write complete */
  while (com_flag == 0)
      printf("Writing prompt to Enter string\n");


  /* read string */

  length = 40;
  com_flag = 0;
  rc = com_read(buffer,&length);


  /* loop until read is done */

     while (com_flag == 0)
	 printf("Reading string from serial port\n");

  /* display string entered */

  com_flag = 0;
  tmp = 22;
  rc = com_write("\015\012string entered was: ",&tmp);

  /* display string entered */
  prt_flag = 0;
  tmp = 22;
  rc = prt_write("\015\012string entered was: ",&tmp);


  while ((com_flag == 0) || (prt_flag == 0))
      printf(" com_flag:%3d    prt_flag:%3d \n",com_flag,prt_flag);

  com_flag = 0;
  rc = com_write(buffer,&length);


  prt_flag = 0;
  rc = prt_write(buffer,&length);


  while ((com_flag == 0) || (prt_flag == 0))
      printf(" com_flag:%3d    prt_flag:%3d \n",com_flag,prt_flag);

  }

  length = 33;
  com_flag = 0;
  rc = com_write ("\015\012Com/prt driver test completed\015\012",&length);

  /* if bad return code, display value and exit */
  if (rc != 0)
   {
   printf("\nrc(com_write) = %x",rc);
   }

  length = 34;
  prt_flag = 0;
  rc = prt_write("\015\012Com/prt driver test completed\015\012\n",&length);


  /* loop until last writes are done */
  while ((com_flag == 0) || (prt_flag == 0))
	  printf(" com_flag:%3d    prt_flag:%3d \n",com_flag,prt_flag);

  com_close();
  prt_close();
 }

int IO_complete(int device, int *stkptr)
{
	return 0;
}


