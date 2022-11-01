/*hhhh
******************************************************************************
**                                                                          **
**  Title:      System Initialization                                       **
**                                                                          **
**  File Name:  intiali.c                                                   **
**                                                                          **
**  Category:   System Initialization, Kernel                               **
**              2170 XINU Operating System                                  **
**                                                                          **
**  Abstruct:   This module initializes each devices.                       **
**                                                                          **
**                VOID SYSCALL XinuSysInit(VOID);                           **
**                                                                          **
******************************************************************************
******************************************************************************
**                                                                          **
**  Modification History:                                                   **
**                                                                          **
**  Ver.      Date        Name        Description                           **
**  00.00.00  Mar 31,88   D.Comer     Original PC-XINU                      **
**  00.00.01  Apr 10,92   Y.Nozawa    Applied coding convension of 2170     **
**                                    Changed free memory block structure   **
**                                      display messages                    **
**                                      interrupt vector mapping            **
**  00.00.02  Aug 20,92   Tkojima     remain only initializing the device   **
**                                    table                                 **
**                                                                          **
******************************************************************************
hhhh*/ 

#include <conf.h>
#include <kernel.h>

VOID SYSCALL XinuSysInit(VOID)
{
    SHORT   i;

	for (i = 0; i < NDEVS; i++) {           /* initialize devices       */
        XinuInit(i);                        /* initialize the device    */
    }
}

