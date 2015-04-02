/***********************************************************************
*
*	Name:  prtdrtst
*
*	Purpose:   Test program for prtdrive
*
*	Algorithm: Tests prt_open, prt_write, and prt_close of prtdrive
*
**************************************************************************/

#include <stdio.h>
#include <dos.h>;

int main()
{
    int e_flag;
    int tmp;
    int rc;
    char buffer[100];
    int length;
    extern int prt_write(char far *buff,int far *len);

    /* open printer */
    rc = 1;
    rc = prt_open(&e_flag);
    if ( rc != 0) {
	printf("\nrc(open) = %d",rc);
	exit();
    }

    /* prepare test string */
    strcpy(buffer,"This is a test of the printer driver ... 1234567890 ...........\015\012");
    length = 65;

    /* print test string 25 times */
    for (tmp = 1; tmp < 26; tmp++) {
	e_flag = 0;
	rc = prt_write(buffer,&length);
	if (rc != 0) {
	   printf("\nrc(write) = %d",rc);
	}

	/* loop until printer done */
	while (e_flag == 0)
	    printf("waiting  %d\n",tmp);
    }



    e_flag = 0;
    length = 29;
	rc = prt_write("End of Printer Driver Test\015\012\n",&length);
	if (rc != 0) {
	printf("\nrc(write) = %d",rc);
	}

	/* loop until printer done */
	while (e_flag == 0)
	printf("Waiting for Printer test to finish\n");

	prt_close();
	return 0;
}

int IO_complete(int device, int *stkptr)
{
	return 0;
}
