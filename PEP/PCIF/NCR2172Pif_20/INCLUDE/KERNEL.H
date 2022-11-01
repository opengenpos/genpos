/*hhhh
*****************************************************************************
**
**  Title:      Header File for Kernel of XINU Operating System
**
**  File Name:  kernel.h
**
**  Category:   Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This header file has definitions used throughout XINU.
**              All source files of XINU kernel and device I/O drivers should
**              include it.
**
**              This file contains,
**
**              1. Symbol definitions (#define)
**              2. Funtion Prototypes which can be called by drivers
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**      Ver.    Date        Name        Description
**      0.0     Mar 31,88   D.Comer     Original PC-XINU
**      0.1     Apr 10,92   Y.Nozawa    Applied coding convension of 2170
**                                      Moved public definitions to xinu.h
**                                        and left definitions inside XINU
**                                      Exported functions for drivers
**
*****************************************************************************
hhhh*/

/* Symbolic constants used throughout Xinu */

#define	LOCAL		static		/* Local procedure declaration	*/
#define	INTPROC				/* Interrupt procedure  	*/

#ifndef _INC_WINDOWS
#define WORD		word		/* 16-bit word			*/ 
#endif

#include <xinu.h>

/* initialization constants */

#define	INITARGC	2		/* initial process argc		*/
#define	INITSTK		512		/* initial process stack	*/
#define	INITPRIO	31		/* initial process priority	*/
#define	INITNAME	"XinuMain"	/* initial process name		*/
#define	INITRET		userret		/* processes return address	*/
#define	INITREG		0		/* initial register contents	*/
#define	QUANTUM		2		/* clock ticks until preemption	*/

/* process management variables */

/* function prototypes -- export for drivers */

VOID    _doprnt(CHAR *fmt, SHORT *args, /* defined in doprnt.c of kernel */
                SHORT (*func)(SHORT farg, CHAR c), SHORT farg);

/* function prototypes -- direct I/O interface to TTY driver */

SHORT   kbdgetc(VOID);			/* defined in kbdio.asm	of tty  */
SHORT   kgetc(SHORT);			/* defined in kgetc.c of tty    */
SHORT   kprintf(CHAR *fmt, ...);        /* defined in kprintf.c of tty  */

#define FP_SEG(fp) (*((unsigned FAR *)&(fp)+1))
#define FP_OFF(fp) (*((unsigned FAR *)&(fp)))
#define MK_FP(seg,ofs)	((void FAR *)(((unsigned long)(seg) << 16) | (ofs)))

