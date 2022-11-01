/*hhhh
*****************************************************************************
**
**  Title:      Header File for XINU Operating System
**
**  File Name:  butler.h
**
**  Category:   Butler
**              2170 XINU Operating System
**
**  Abstruct:   This header file defines the butler process of the 2170 XINU
**              Operating system. The butler process is useful to display
**              information inside the kernel or device I/O drivers. But it
**              should not be used for release version to reduce memory size.
**
**              This file contains,
**
**              1. Symbol definitions
**              2. Interface Funtion Prototypes
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

/* butler demon process definitions for housekeeping and other chores	*/

#define	BTLRNAME	"*BUTLER*"	/* process name			*/
#define	BTLRSTK		512		/* butler process stack size	*/
#define BTLRPRIO	1000		/* butler priority		*/

extern	SHORT	NEAR butlerpid;		/* butler process pid for msgs	*/

#define	MSGKILL		-1		/* kill xinu			*/
#define	MSGPSNAP        1		/* print a process snapshot	*/
#define	MSGTSNAP	2		/* print a tty snapshot		*/
#define	MSGDSNAP	3		/* print a disk snapshot	*/

VOID PROCESS butler(VOID);		/* butler process code		*/
VOID psnap(VOID);
VOID tsnap(VOID);
VOID dsnap(VOID);
