/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : RAM Disk Driver Header File
:  Category           : RAM Disk Driver, 2170 XINU Operating System
:  Program Name       : DISK.H
:  -----------------------------------------------------------------------
:  Description : This module defines the RAM Disk Driver based on XINU.
:                RAM Disk Driver is implemented by:
:
:                       dskinit.c       : Dsk, DskInit()
:                       dskfinit.c      : aDskFile, DskFInit()
:                       dskopen.c       : DskOpen()
:                       dskclose.c      : DskClose()
:                       dskread.c       : DskRead()
:                       dskwrite.c      : DskWrite()
:                       dskseek.c       : DskSeek()
:                       dskcntl.c       : DskCntl()
:                       dsknewblk.c     : DskNewBlk()
:                       dskupdate.c     : DskUpDate()
:                       dskckmd.c       : DskCkMd()
:                       dsksrch.c       : DskSrch()
:                       dskalloc.c      : DskAlloc()
:                       dskixget.c      : DskIxGet()
:                       dskixput.c      : DskIxPut()
:                       dskixnum.c      : DskIxNum()
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 6.00A by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /ACw /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
\*------------------------------------------------------------------------*/

/* Local disk layout:                                                   */
/* Disk block 0 is directory, then super-index area, and then index     */
/* and data blocks.  The disk block (512 bytes) in the super-index area	*/
/* contains 32 indexes, which are 16 bytes long. The first super-index  */
/* is used for free block list. Others are used for user files. The     */
/* super-index i is associated with the directory entry i.              */
/*                                                                      */
/* Each super-index contains direct pointers to IBLEN-2 (6) data        */
/* blocks, one pointer to a single indirect index block, and one        */
/* pointer to a double indirect index block.                            */
/*                                                                      */
/*                 super-index                                          */
/*                    _____                                             */
/*    direct index 0 |_____|-------------------------------> data block */
/*                 1 |_____|         _____                              */
/*                 2 |     |   +-->0|_____|----------------> data block */
/*                      :      |   1|_____|                             */
/*                      :      |   2|     |                             */
/*                 5 |_____|   |       :                                */
/*    single indirect|_____|---+       :                                */
/*    double indirect|_____|---+    |_____|                             */
/*                             | 255|_____|                             */
/*                             |                                        */
/*                             |     _____       _____                  */
/*                             +-->0|_____|--->0|_____|----> data block */
/*                                 1|_____|    1|_____|                 */
/*                                 2|     |    2|     |                 */
/*                                     :           :                    */
/*                                     :           :                    */
/*                                  |_____|     |_____|                 */
/*                               255|_____|  255|_____|                 */
/*                                                                      */
/*                                                                      */

#define	DBUFSIZ	512			/* size of disk data block	*/
#define	AREADIR	0			/* block used for directory	*/
#define	AREASI	1	        /* block used for super-indexes */
#define NFDES	31			/* number of files / directory	*/
#define FDNLEN	10			/* length of file name + 1	*/
#define	SILEN	8	        /* # of pointers in super-index	*/
#define IXLEN   (DBUFSIZ/sizeof(DBADDR))/* # of pointers in index block */
#define	FDELM	229	        /* file delete make ascii data */
#define	DSKCLS	-1	        /* file close status */

#define	EOF			-2		/* value returned on end-of-file*/
#define	FLREAD		0x01	/* fl_mode bit for "read"	*/
#define	FLWRITE		0x02	/* fl_mode bit for "write"	*/
#define	FLRW		0x03	/* fl_mode bits for read+write	*/
#define	FLNEW		0x10	/* fl_mode bit for "new file"	*/
#define	FLOLD		0x20	/* fl_mode bit for "old file"	*/

typedef	USHORT  DBADDR;		    /* disk block addresses	        */

typedef struct	{		        /* super-index block layout	*/
	DBADDR	IDirect[SILEN-2];   /* direct index to data blocks  */
	DBADDR	ISingle;	        /* single indirect index	*/
	DBADDR	IDouble;	        /* double indirect index	*/
} SIBLK;

typedef struct	{				/* description of each file	*/
	UCHAR	szName[FDNLEN];	    /* zero terminated file name	*/
	ULONG	ulSize;		        /* length in bytes		*/
	USHORT	usNBlocks;			/* # of disk blocks in use      */
} FDES;

typedef struct	{				/* directory layout		*/
	FDES	aFiles[NFDES];	    /* description of the files	*/
	USHORT	usTotalBlks;		/* total number of blocks	*/
	USHORT	usUsedBlks;			/* number of used blocks	*/
	USHORT	usFreeBlks;			/* number of free blocks	*/
	SHORT	sNFiles;			/* current number of files	*/
	USHORT	usReserved[4];		/* 16 bytes alignment           */
} DIR;

typedef struct	{				/* disk driver control block	*/
	SHORT	sSem;		        /* mutual exclution semaphore	*/
	DIR     Dir;		        /* in-core directory	        */
} DSBLK;

typedef struct	{			    /* file "device" control block	*/
	SHORT	hFile;   		    /* file's "device id" in devtab	*/
	SHORT	sPid;			    /* process id accessing the file*/
	SHORT	sDirNum;	        /* file's in-core dir. entry	*/
	SHORT	sMode;  	        /* FLREAD, FLWRITE, or both	*/
	ULONG	ulPosition; 		/* current file position (bytes)*/
} FLBLK;

/*
 *      Disk and File control table
 */
#ifdef  Ndsk
extern  DSBLK	NEAR Dsk;
#endif

#ifdef	Ndf
extern  FLBLK	NEAR aDskFiles[Ndf];
#endif

/*
 *      Local Function Prototypes
 */

DBADDR  DskNewBlk(VOID);
SHORT   DskCkMd(CHAR *szMode);
SHORT	DskSrch(CHAR *szFileName, SHORT sMode);
SHORT   DskAlloc(VOID);
DBADDR  DskIxGet(SIBLK *pSuperIx, USHORT usIxNum);
VOID    DskIxPut(SIBLK *pSuperIx, USHORT usIxNum, DBADDR DbAddr);
USHORT  DskIxNum(ULONG ulPosition);
VOID    DskUpDate(ULONG ulPosition, VOID *pBuff);
SHORT	DskFrmt(VOID);

