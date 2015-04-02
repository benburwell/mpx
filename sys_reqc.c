/*
 *   file:  sys_reqc.c
 *
 *   This is the MPX system request function which
 *   uses the C gets function.
 */



/*    sys_req   - implements the MPX system request function.
 *
 *    Parameters: number - request number: a device or exit code.
 *                type   - request type: read, write, wait, signal.
 *                s      - address of read/write buffer.
 *                length - address of buffer length variable.
 *
 *    Return value: None
 *
 *    In this module, the only system request allowed is to
 *    read from the console (keyboard).  This is done using
 *    the standard C function gets().
 */


void sys_req(int number, int type, char *s, int *length)
{
    gets(s);                /* Read a string from the keyboard.       */
    *length = strlen(s);    /* Get its length.                        */
}

