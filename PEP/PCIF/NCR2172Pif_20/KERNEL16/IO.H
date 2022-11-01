/*hhhh
*****************************************************************************
**
**  Title:      Device Independent I/O
**
**  File Name:  io.h
**
**  Category:   Device Independent I/O, Kernel
**              2170 XINU Operating System
**
**  Abstruct:   This header file defines the device independent I/O of XINU.
**              The device independent I/O consists of,
**
**                      getdev.c        : XinuDevDev()
**                      init.c          : XinuInit()
**                      open.c          : XinuOpen()
**                      close.c         : XinuClose()
**                      read.c          : XinuRead()
**                      write.c         : XinuWrite()
**                      getc.c          : XinuGetc()
**                      putc.c          : XinuPutc()
**                      seek.c          : XinuSeek()
**                      control.c       : XinuControl()
**                      ioerr.c         : ioerr()
**                      ionull.c        : ionull()
**                      map.c           : mapinit(), maprestore()
**                      eidi.asm        : sys_diabl(), sys_restor(),
**                                        sys_enabl(), sys_wait(), sys_hlt
**                      xeidi.asm       : sys_xdisabl(), sys_xrestor(),
**                                        sys_pcxget(), sys_getstk()
**                      printf.c        : printf()
**                      fprintf.c       : fprintf()
**                      doprnt.c        : _doprnt()
**
**              This file contains the difinitions of:
**
**              1. Symbols used by the device independent I/O
**              2. Interrupt dispatcher and related valiables
**              3. inline procedures: getchar, putchar, fgetc, fputc
**              4. Local funtion prototypes
**
*****************************************************************************
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date        Name        Description
**  00.00.00  Mar 31,88   D.Comer     Original PC-XINU
**  00.00.01  Apr 10,92   Y.Nozawa    Applied coding convension of 2170
**                                    Added function prototypes
**
*****************************************************************************
hhhh*/

#define	INTVECI	inint		/* input interrupt dispatch routine	*/
#define	INTVECO	outint		/* output interrupt dispatch routine	*/
extern	SHORT	INTVECI();
extern	SHORT	INTVECO();

#define NMAPS	0x20		/* number of intmap entries		*/

typedef struct	intmap	{	/* device-to-interrupt routine mapping	*/
	SHORT	ivec;		/* interrupt number			*/
	USHORT	oldisr_off;	/* old int. service routine offset	*/
	USHORT	oldisr_seg;	/* old int. service routine segment	*/
	VOID   (*newisr)(SHORT);/* pointer to the new int. ser. routine */
	UCHAR	mdevno;		/* minor device number			*/
	UCHAR	iflag;		/* if nonzero, call the old isr		*/
} INTMAP;

typedef struct	inthook	{	/* device-to-interrupt routine hook	*/
        UCHAR   hookno;         /* number of hook entry                 */
	UCHAR	callinst;	/* the call instruction			*/
	USHORT	intcom;		/* common interrupt code		*/
	USHORT	index;	        /* index to intmap table	        */
} INTHOOK;

/*
 * NOTE: The intmap structure takes a total of 6 words or 12 bytes per record.
 *       The inthook structure takea a total of 3 words or 6 bytes per record.
 */

extern  VOID FAR * NEAR sys_imp[]; /* pointer to intmap, inthook table	*/
extern	UCHAR   NEAR nmaps;	/* number of active intmap entries	*/

#define	isbaddev(f)	( (f)<0 || (f)>=NDEVS )

/* In-line I/O procedures */

#define	getchar()	XinuGetc(CONSOLE)
#define	putchar(ch)	XinuPutc(CONSOLE,(ch))
#define	fgetc(unit)	XinuGetc((unit))
#define	fputc(unit,ch)	XinuPutc((unit),(ch))

/* function prototypes */

SHORT   mapinit(SHORT vec, VOID (*newisr)(SHORT), SHORT mdevno);
VOID    maprestore(VOID);
