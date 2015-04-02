/***********************************************************************
*
*	Module Name:	load
*
*	Purpose: To provide support for loading files.
*
*	Functions in Module:
*		load

************************************************************************/

#include <dos.h>        /* Borland header file */
#include <fcntl.h>      /* Borland header file */
#include <dir.h>        /* Borland header file */

#include "mpx.h"

#define intnum 33 	/* int86 number for load only */
#define success 0	/* return code for successful process load */
#define error   -1      /* return code for failure on process load */


/**************************************************************************
 *
 *	Function: load
 *	Abstract: Dynamically loads a file from disk
 *	Algorithm:
 *                 This procedure dynamically loads a process with a
 *                 file extension of .MPX.  The memory address provided
 *		   is the segment address at which to load the process.
 *                 The process with the name in the string pname is
 *		   loaded into memory at the segment address found in
 *                 load_addr.  If load is successful, load returns a 0
 *                 return code, otherwise it returns a -1  return code
 *
 **************************************************************************/

int load(unsigned *load_addr,char pname[])
{
    int i,flags,carry;
    char fname[30];

    struct PRMBLK
      {
	 int segaddr;
	 int reloctn;
      };

    struct PRMBLK prmbk;
    union  REGS *inregs;     /* input registers pointer */
    union  REGS *outregs;    /* output registers pointer */
    struct SREGS *segregs;  /* segment registers pointer */

    union  REGS inr;	   /* input register structure */
    union  REGS outr;	   /* output register structure */
    struct SREGS segr;	   /* segment register structure */

    disable();

    /* SEGMENT REGISTER VALUE CALL */

    segregs = &segr;
    segread(segregs);    /* returns current segme  disablter values */

    strcpy(fname, pname);
    strcat(fname, ".MPX");

    inregs = &inr;
    outregs = &outr;
    inregs->h.ah = 75;
    inregs->x.dx = &fname[0];
    prmbk.segaddr = load_addr;
    prmbk.reloctn = load_addr;
    segr.es = segr.ds;		/* assigning the ES to the data segment */
    inregs->x.bx = &prmbk;
    inregs->h.al = 3;		/* load only */

    flags = int86x(intnum,inregs,outregs,segregs);
    carry = flags;   /* Could be omitted */
    carry = outregs->x.cflag;
    if(carry)
    {
	printf("\nerror code:%3d",outregs->x.ax);
	enable();
	return(error);
    }

    /* load successful */
    enable();
    return(success);
  }

/***********************************************************************/
