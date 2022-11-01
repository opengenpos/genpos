/*hhhh
*****************************************************************************
**
**  Title:      Retrieve device number by device name
**
**  File Name:  getdev.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**		  SHORT SYSCALL XinuGetDev(CHAR *pchName)
**
**		    pchName	Device name
**
**              The XinuGetDev retrieves the number of a device given its
**              device name.
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date        Name        Description
**  00.00.00  Mar 31,88   D.Comer     Original PC-XINU
**  00.00.01  Apr 10,92   Y.Nozawa    Applied coding convension of 2170
**
*****************************************************************************
hhhh*/

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  getdev  --  get the device number from a character string name
 *------------------------------------------------------------------------
 */

SHORT SYSCALL XinuGetDev(CHAR *pchName)
{
	int i,j;

	for ( i=0; i<NDEVS; i++ )
		for ( j=0; *(pchName+j)==*(devtab[i].dvnam+j) ; j++)
			if ( *(pchName+j)==0 ) return((SHORT)i);
	return(SYSERR);
}


