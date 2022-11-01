/*hhhh
*****************************************************************************
**
**  Title:      Interface between XINU Operating System and BIOS
**
**  File Name:  bios.h
**
**  Category:   BIOS Interface, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This header file defines the interface between the 2170 XINU
**              Operating system and the BIOS. 
**              The BIOS interface consists of:
**
**                      cbrkint.c       : cbrkint()
**                      clkint.c        : clkint()
**                      panic.c         : XinuPanic(), _panic()
**              
**              This file contains,
**
**              1. Interrupt Vector number defintions
**              2. Function prototypes
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**      Ver.    Date        Name        Description
**      0.0     Mar 31,88   D.Comer     Original PC-XINU
**      0.1     Apr 10,92   Y.Nozawa    Applied coding convension of 2170
**
*****************************************************************************
hhhh*/

/*------------------------------------------------------------------------
 *  ROM BIOS interface information for 2170
 *------------------------------------------------------------------------
 */
#define DB0VEC		0x00		/* divide by zero exception vec	*/
#define SSTEPVEC	0x01		/* single step exception vector	*/
#define BKPTVEC		0x03		/* breakpoint exception vector	*/
#define OFLOWVEC	0x04		/* overflow exception vector	*/

#ifdef PC
#define	CLKVEC	0x08			/* Clock interrupt vector	*/
#define	KBDVEC	0x09			/* Keyboard interrupt vector	*/
#define	COM1VEC	0x0b			/* COM1 interrupt vector	*/
#define	COM2VEC	0x0c			/* COM2 interrupt vector	*/
#define	FLOPVEC	0x0e			/* Floppy interrupt vector	*/
#define	PRLLVEC	0x0f			/* Parallel port interrupt vec	*/
#define CBRKVEC	0x1b			/* Ctrl-Break interrupt vector	*/
#define IHCVEC  0x0d            /* Token Bus interrupt vector   */
#else
#define	CLKVEC	0x1e			/* Clock interrupt vector	*/
#define	KBDVEC	0x87			/* Keyboard interrupt vector	*/
#define IHCVEC  0x1a            /* Token Bus interrupt vector   */
#endif

#define	BIOSFLG	0x100			/* BIOS flag for intmap		*/

/*------------------------------------------------------------------------
 *  ROM BIOS I/O port address information for 2170
 *------------------------------------------------------------------------
 */
#ifdef  PC
#define IHCIO   0x270           /* IHC port address */
#else
#define IHCIO   0x4000          /* IHC port address */
#endif

/*------------------------------------------------------------------------
 *  Exception Handling Functions Prototypes for 2170
 *------------------------------------------------------------------------
 */
VOID    _panic(SHORT typ);              /* exception handler            */
VOID INTPROC cbrkint(VOID);             /* ctrl-break handler           */
VOID INTPROC clkint(SHORT mdevno);      /* clock interrupt handler      */

