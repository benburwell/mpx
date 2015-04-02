/*
 *    file:  direct.c
 *
 *    This file contains the function which reads the names and
 *    sizes of the MPX/OS process files from the disk, and stores
 *    them in the directory entry array.
 */

#include <dos.h>        /* Borland header file */
#include <fcntl.h>      /* Borland header file */
#include <string.h>     /* Borland header file */
#include <dir.h>        /* Borland header file */
#include "mpx.h"

dir direct[MAXSIZE];    /* The array of directory entries. */

/*
 *
 *   directory  -  This procedure performs a sequential read of the MPX
 *	           directory, obtaining all .MPX files up to the size of the
 *                 directory.  These are the file that can be loaded by
 *                 COMHAN. Note that the file specific details are saved in
 *	           the dir structure, direct.
 *
 *   Parameters:   direct   - the array of directory entries.
 *                 dir_size - the capacity of the direct array.
 *
 *   Return value:  The number of files entries made to the
 *                  direct array.
 */


int directory(dir *direct, int dir_size)
{
  int   num_procs;       /* Number of .mpx files found.                 */
  char  filename[15];    /* Name of a file with .mpx extension.         */
  int   done;            /* Flags when no more .mpx files can be found. */
  struct ffblk ffblk;

  num_procs = 0;    /* number of .MPX file entries placed in directory */

  done = findfirst ("*.MPX",&ffblk,0);
  while (!done && num_procs < dir_size) {
      strcpy (filename,ffblk.ff_name);
      strcpy(direct->dirnam,filename);
      direct->dirnam[strcspn(filename,".")] = '\0';
      direct->dirsiz = ffblk.ff_fsize;
      ++num_procs;
      direct++;
      done = findnext(&ffblk);
  }

  return(num_procs);
}

