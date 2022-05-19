/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Shared Printer Manager, Header File
* Category    : Shared Printer Manager, NCR 2170 HOSP US ENH. Application
* Program Name: SHR.H
* --------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date      Ver.        Name            Description
* Jun-16-93 00.00.01    NAKAJIMA,Ken    Initial
*
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/


/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/
/* Shared Printer subsystem error codes

   These codes should be synchronized with similar values used by
   the lower level STUB_ error codes as well as the Totals
   error codes defined in file csttl.h and the Guest Check
   error codes defined in file csgcs.h
 */
/* -- function return value --
	Try to keep in synch with STUB_ values in csstbfcc.h,
	with SERV_ values in servmin.h, and KPS_ values in cskp.h
*/
#define SHR_PERFORM                0
#define SHR_TIMERON                1
#define SHR_BUSY                  (-3)
#define	SHR_TIME_OUT			  (-7)
#define SHR_ILLEGAL				  (-8)
#define SHR_NOT_SHARED_PRINTER   (-20)    // target terminal is not sharing printer
#define SHR_FAIL                 (-31)
#define SHR_BUFFER_FULL          (-33)    /* SHR RAM Disk is now full */
#define SHR_BUFFER_PRINTER_ERR   (-37)    // target printer has error condition

// Following are used to transform Network Layer (STUB_) errors into KPS_ errors
// Notice that error code is same as STUB_ plus STUB_RETCODE.
#define	SHR_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define	SHR_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define SHR_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define SHR_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define SHR_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define SHR_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)


/* -- data flame name -- */
#define SHR_FIRST_FRAME         0x01
#define SHR_CONTINUE_FRAME      0x02
#define SHR_END_FRAME           0x10

/* -- flame size -- */
// This value is used for the size of the buffer
// that will read from the shared printing spool file
// which contains the data that will be sent to the printer.
//
// This value must be a combination of the header size
// and the actual data that will be sent to the printer.
// If the size of the header (SHRHEADERINF) changes this
// size should also be reflected in this buffer to ensure
// that the buffer available for the actual data that
// is being printed is an even number.
//
// The buffer size for the data must be even because the
// characters that are being printed are two bytes wide.
// Since the double width characters require 2 bytes for each
// character, if there is a full read from the shared printing
// spool file i.e. SHR_FRAME_SIZE, an odd number of bytes
// available for the actual data will result in the
// last character in the buffer only Having one byte of
// the two being read and that last character will be
// printed incorrectly. ESMITH

#define SHR_FRAME_HEADER		 sizeof(SHRHEADERINF)
#define SHR_MAX_DATA_SIZE		 (700)		//Number of characters (data stored as wide characters)
#define SHR_FRAME_SIZE           (SHR_MAX_DATA_SIZE + SHR_FRAME_HEADER)
//#define SHR_MAX_DATA_SIZE_BYTES	 (SHR_MAX_DATA_SIZE)	//Multiplied by 2 for wide characters
//#define SHR_FRAME_SIZE_BYTES	 (SHR_MAX_DATA_SIZE_BYTES + SHR_FRAME_HEADER)

/* -- font change command header -- */
#define SHR_FONT_MSG_1          0xFF
#define SHR_FONT_MSG_2          0xFF

/* -- printer status request command header -- */
#define SHR_STAT_MSG_1          0xFB
#define SHR_STAT_MSG_2          0xFB

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

// If the size of this header changes the size of the
// SHR_FRAME_SIZE buffer must reflect this change.
// Please read the note above. ESMITH
typedef struct {
    USHORT usILI;
    UCHAR  uchTermAdr;
    USHORT usConsNo;
    UCHAR  fbFrame;
    UCHAR  uchSeqNo;
} SHRHEADERINF;

typedef struct {
    UCHAR  uchHead1;
    UCHAR  uchHead2;
    USHORT usType;
} SHRFONTMSG;

typedef struct {
    UCHAR  uchHead1;
    UCHAR  uchHead2;
    USHORT usType;
} SHRPRNTSTATMSG;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID  ShrInit(VOID);
SHORT ShrCancel(UCHAR uchTerm);
SHORT ShrPrint(VOID *pData);
SHORT ShrTermLock(UCHAR uchTermAdr);
SHORT ShrTermUnLock(VOID);
SHORT ShrPolling(VOID);

/* ---------- End of SHR.H ---------- */

