/*hhhh
*****************************************************************************
**
**  Title:      Empty Entry Service Routine
**
**  File Name:  ionull.c
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This module provides the following function:
**
**                SHORT ionull(VOID)
**
**                Return = OK
**
**              The ionull simply returns OK whenever it is called.
**              It is used to fill the empty entry in devtab.
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

/* ionull.c - ionull */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  ionull  --  do nothing (used for "don't care" entries in devtab)
 *------------------------------------------------------------------------
 */
SHORT ionull(VOID)
{
	return(OK);
}

