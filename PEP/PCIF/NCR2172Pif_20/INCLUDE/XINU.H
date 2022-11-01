/*hhhh
*****************************************************************************
**
**  Title:      Header File for XINU Operating System
**
**  File Name:  xinu.h
**
**  Category:   2170 XINU Operating System
**
**  Abstruct:   This header file defines the Application Programming Interface
**              (API) provicded the 2170 XINU Operating system. It will be 
**              included by source code of both the 2170 XINU Operating
**              System and the Portable Platform Interface.
**
**              This file contains,
**
**              1. Data type defintions (typedef)
**              2. Symbol definitions (#define)
**              3. Interface Funtion Prototypes
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**      Ver.    Date        Name        Description
**      0.00.01 Apr 17,92   Y.Nozawa    Initial Version
**              Aug 15,92   Y.Nozawa    Added prototype of XinuSleepm()
**              Aug 28,92   O.Nakada    Added typedef of COMCONTROL
**              Jun 12,98   O.Nakada    Win32
**
*****************************************************************************
hhhh*/

/* Symbolic constants used throughout Xinu */

typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef int             INT;

#ifdef	WIN32
#ifndef	NEAR
#define NEAR
#endif
#else
#ifndef _INC_WINDOWS
#define NEAR  
#endif
#endif

#ifndef _INC_WINDOWS
typedef unsigned int    UINT;
#endif

#ifndef _INC_WINDOWS
typedef long            LONG;
#endif

typedef unsigned long   ULONG;

#ifndef _INC_WINDOWS
typedef USHORT          BOOL;
#endif

#define CONST           const

#ifdef	WIN32
#ifndef	FAR
#define FAR
#endif
#else
#ifndef _INC_WINDOWS
#define FAR  
#endif
#endif

#define VOID            void
#define	SYSCALL				            /* System call declaration	    */
#define	PROCESS		    		        /* Process declaration		    */

#define	FALSE		        0		    /* Boolean constants		    */
#define	TRUE		        1

#ifndef _INC_WINDOWS
#define	NULL		        (char *)0	/* Null pointer for linked lists    */
#endif

#define	OK		            1 		    /* returned when system	call ok	    */
#define	SYSERR		        -1		    /* returned when sys. call fails    */
#define TIMEOUT             -3          /* timeout                          */
#define XINU_NOT_PROVIDED   -4          /* not provided                     */
#define XINU_ALREADY_EXIST  -5          /* already exist                    */
#define XINU_EOF            -6          /* end of file                      */
#define XINU_DISK_FULL      -7          /* disk is full                     */
#define XINU_NOT_FOUND      -8          /* file not found                   */
#define XINU_ACCESS_DENIED  -10         /* access is denied                 */
#define XINU_READ_ERROR     -11         /* read (receive) error             */
#define XINU_WRITE_ERROR    -12         /* write (send) error occurred      */
#define XINU_OFFLINE        -13         /* network is not ready             */
#define XINU_UNREACHABLE    -14         /* network address unreachable      */
#define XINU_MESSAGE_ERROR  -15         /* invalid message                  */

/* misc. utility functions */

#define	isodd(x)	(01&(int)(x))
#define	disable(x)	(x)=sys_disabl() /* save interrupt status	    */
#define	restore(x)	sys_restor(x)	/* restore interrupt status	    */
#define	enable()	sys_enabl()	/* enable interrupts		    */
#define	pause()		sys_wait()	/* machine "wait for interr."	    */
#define	halt()		sys_hlt()	/* halt 2170 Xinu			    */
#define xdisable(x)	(x)=sys_xdisabl() /* save int & dosflag status	    */
#define xrestore(x)	sys_xrestor(x)	/* restore int & dosflag status	    */

/* system-specific functions and variables */

SHORT   sys_disabl(VOID);	        /* return flags & disable ints	    */
VOID    sys_restor(SHORT ps);	        /* restore the flag register	    */
VOID    sys_enabl(VOID);	        /* enable interrupts		    */
VOID    sys_wait(VOID);		        /* wait for an interrupt	    */
VOID    sys_hlt(VOID);		        /* halt the processor		    */
SHORT   sys_xdisabl(VOID);	        /* Return interrupts to MS-DOS	    */
VOID    sys_xrestor(SHORT ps);	        /* Interrupts back to Xinu	    */
SHORT   sys_pcxget(VOID);               /* get the current value of pcxflag */
VOID    sys_getstk(SHORT *sssp,         /* get the stack parameters         */
            SHORT *spsp);               /*   for panic printing             */

/*------------------------------------------------------------------------
 *  function prototypes of XINU system calls
 *------------------------------------------------------------------------
 */

/* process management */
SHORT SYSCALL XinuCreate(VOID (*caddr)(VOID), SHORT ssize, SHORT priority,
                         CHAR *namep, SHORT nargs, ...);
SHORT SYSCALL XinuKill(SHORT pid);
SHORT SYSCALL XinuResume(SHORT pid);
SHORT SYSCALL XinuSuspend(SHORT pid);
SHORT SYSCALL XinuGetPid(VOID);
SHORT SYSCALL XinuGetPrio(SHORT pid);
SHORT SYSCALL XinuChPrio(SHORT pid, SHORT sNewPrio);
VOID  SYSCALL XinuDone(VOID);
SHORT SYSCALL XinuReady(SHORT pid);
VOID  SYSCALL XinuResched(VOID);
 
/* message passing */
SHORT SYSCALL XinuReceive(VOID);
SHORT SYSCALL XinuRecvClr(VOID);
SHORT SYSCALL XinuRecvTim(USHORT usTicks);
SHORT SYSCALL XinuSend(SHORT pid, SHORT msg);
SHORT SYSCALL XinuSendf(SHORT pid, SHORT msg);
SHORT SYSCALL XinuSendn(SHORT pid, SHORT msg);

/* semaphores */
SHORT SYSCALL XinuSCount(SHORT sem);
SHORT SYSCALL XinuSCreate(SHORT count);
SHORT SYSCALL XinuSDelete(SHORT sem);
SHORT SYSCALL XinuSignal(SHORT sem);
SHORT SYSCALL XinuSignaln(SHORT sem, SHORT count);
SHORT SYSCALL XinuWait(SHORT sem);
SHORT SYSCALL XinuSReset(SHORT sem, SHORT count);

/* real-time clock management */
SHORT SYSCALL XinuSleep(SHORT secs);
SHORT SYSCALL XinuSleept(SHORT ticks);
SHORT SYSCALL XinuSleepm(USHORT usMSecs);

/* device I/O */
SHORT SYSCALL XinuGetDev(CHAR *name);
SHORT SYSCALL XinuInit(SHORT dev);
SHORT SYSCALL XinuOpen(SHORT dev,...);
SHORT SYSCALL XinuClose(SHORT dev);
SHORT SYSCALL XinuRead(SHORT dev, VOID *buffer, SHORT numchars);
SHORT SYSCALL XinuWrite(SHORT dev, VOID *buff, SHORT count);
SHORT SYSCALL XinuControl(SHORT dev, SHORT function,...);
SHORT SYSCALL XinuGetc(SHORT dev);
SHORT SYSCALL XinuPutc(SHORT dev, CHAR ch);
SHORT SYSCALL XinuSeek(SHORT dev, long position, ...);
SHORT SYSCALL XinuPrintf(CHAR *fmt, ...);
SHORT SYSCALL XinuFPrintf(SHORT dev, CHAR *fmt, ...);

/* exception handling and support */
VOID  SYSCALL XinuPanic(CHAR *msg);

/* memory management */
VOID * SYSCALL XinuGetMem(USHORT usBytes);
SHORT SYSCALL XinuFreeMem(VOID *pBlock);

/* message queue port management */
SHORT SYSCALL XinuPInit(VOID);
SHORT SYSCALL XinuPCreate(SHORT sCount);
SHORT SYSCALL XinuPSend(SHORT hPort, VOID *pMsg);
VOID * SYSCALL XinuPReceive(SHORT hPort);
SHORT SYSCALL XinuPCount(SHORT hPort);
SHORT SYSCALL XinuPDelete(SHORT hPort, VOID (*pDispose)(VOID *pMsg) );
SHORT SYSCALL XinuPReset(SHORT hPort, VOID (*pDispose)(VOID *pMsg) );

/* crc-ccitt */
USHORT  XinuCrc(VOID FAR *pBuffer, USHORT usLength, USHORT usInitCrc);

/* API Initialization Entry */
VOID    XinuMain(VOID);
VOID    SYSCALL XinuSysInit(VOID);    /* initialize device    */

/* Network Communication */
#pragma pack(push, 1)
typedef struct {
    UCHAR   uchPort;                        /* Port # */
    USHORT  usBaudRate;                     /* BaudRate */
    UCHAR   uchByteSize;                    /* Byte Size of Data */
    UCHAR   uchParity;                      /* Parity Bits */
    UCHAR   uchStopBits;                    /* Stop Bits */
} COMCONTROL;
#pragma pack(pop)

SHORT XinuNetStart(COMCONTROL *pControl, UCHAR CONST *puchIPAddr);
SHORT XinuNetStop(VOID);

/*------------------------------------------------------------------------
 *  Disk Driver Interface Constants
 *------------------------------------------------------------------------
 */

/* disk control function codes */

#define	DSKSYNC	0			/* synchronize (flush all I/O)	*/
#define DSKNAME 1			/* rename a file		*/
#define DSKZAP	2			/* remove a file		*/
#define DSKFMT	3			/* format a disk		*/

/*------------------------------------------------------------------------
 *  Network Interface Constants
 *------------------------------------------------------------------------
 */

/* DLC (Data Link Control) constants */

#define DMINPAK     5               /* minimum packet length                */
/* For Release 2.x, we change DMAXPAK to match the max size in NHPOS for the max
size of the buffer that it was sending over the network.  SR 784 JHHJ*/

#define DMAXPAK     1400             /* maximum packet length                */
//#define DMAXPAK     512             /* maximum packet length                */
#define DHLEN       4               /* size of DLC packet header            */
#define DDLEN       (DMAXPAK-DHLEN) /* size of DLC packet header            */
#define DBCAST      0x00            /* DLC broadcast address                */

/* DARPA IP (Internet Protocol) constants */

#define IPLEN       4               /* IP address length in byte (octets)   */
typedef UCHAR       IPADDR[IPLEN];  /* DARPA internet address               */
#define IPHLEN      20              /* IP datagram header length in bytes   */
#define IMAXLEN     DDLEN           /* maximum IP datagram length           */
                                    /*  Note: must be <= DDLEN              */

/* DARPA User Datagram Protocol (UDP) constants */

#define UHLEN       8               /* UDP header length in byte            */
#define UMAXLEN (IMAXLEN-IPHLEN-UHLEN) /* maximum data in UDP packet        */

/* Constants for dg pseudo-device control functions */

#define DG_SETMODE      1               /* Set mode of device           */
#define DG_CLEAR        2               /* clear all waiting datagrams  */
#define DG_SETTIME      3               /* Set timer value of device    */

/* Constants for dg pseudo-device mode bits */

#define DG_NMODE        0x01            /* normal (datagram) mode       */
#define DG_DMODE        0x02            /* data-only mode               */
#define DG_TMODE        0x20            /* timeout all reads            */
 
/* Structure of xinugram as dg interface delivers it to user */

typedef struct xgram {                  /* Xinu datagram (not UDP)      */
        UCHAR   xg_faddr[4];            /* foreign host IP address      */
        SHORT   xg_fport;               /* foreign UDP port number      */
        SHORT   xg_lport;               /* local UDP port number        */
        CHAR    xg_data[UMAXLEN];       /* maximum data to/from UDP     */
} XDGRAM;

/* DLC control function codes */

#define	DLCDIAG     0			        /* diagnostics                  */

/* NET control function codes */

#define	NET_PINGPONG    0  		        /* reset and start DLC service	*/

/*------------------------------------------------------------------------
 *  System Constants
 *------------------------------------------------------------------------
 */
#define TICK    55                      /* sleept() ticks every 55ms    */
#define XINU_CONSOLE    0               /* Handle for TTY               */

/*------------------------------------------------------------------------
 *  System Valiables
 *------------------------------------------------------------------------
 */
#ifdef	WIN32
extern  UCHAR           currpid;
#else
extern  UCHAR   _near   currpid;
#endif
/* extern	UCHAR	NEAR currpid; */		/* currently executing process	*/
