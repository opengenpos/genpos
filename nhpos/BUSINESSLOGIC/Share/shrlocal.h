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
* Title       : Shared Printer Manager, Local Header File
* Category    : Shared Printer Manager, NCR 2170 HOSP US ENH. Application
* Program Name: ShrLocal.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Zap
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
* Apr-13-95 02.05.04    M.Suzuki        Chg R3.0 Allow K/P
* Sep-13-95 03.00.07    M.Suzuki        Chg R3.0 Stack Size
*
** NCR2172 **
*
* Oct-05-99 01.00.00    M.Teraki        Added #pragma(...)
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
/* 
	If the SHR_SPOOL_SIZE size increases, the stored read and write pointers
	within the shared printer module (SHR.C) should be changed to accomodate
	this change.
*/
#define     SHR_SPOOL_SIZE      60*1024          /* spool file size */
#define     SHR_FILE_TOP_OFF        0           /* spool file data top offset */

// bit flags used with shared printing global fbShrSSFFull to communicate
// spool file and printer status between the spool output thread ShrPrintProcess()
// and the client/server thread function ShrPrint().
#define     SHR_SSF_FULL         0x01           /* buffer full flag */
#define     SHR_TEMP_SSF_FULL    0x02           /* buffer full flag (temporary) */
#define     SHR_PRINTER_ERR      0x10           // Printer error.  Please check printer.

#define     SHR_LOCK                1           /* file lock flag */

#define     SHR_MAX_TERM           16+1         /* max terminal # of 1 cluster */
                                                /* Chg +1(K/P) R3.0            */

#define     SHR_SAME                1           /* same data */

#define     SHR_BREAK               1            
#define     SHR_NOT_EXEC            2

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


typedef struct{
    UCHAR fbTermLock;
    UCHAR uchTermAdr;
} SHRLOCK;


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

/* -- Shr.c -- */
VOID   ShrInitFile(VOID);
VOID   ShrInitSem(VOID);
VOID   ShrInitSPP(VOID);
VOID   ShrRestoreWrtP(VOID);
SHORT  ShrLockCheck(UCHAR uchTermAdr);
SHORT  ShrLock(UCHAR uchTermAdr);
SHORT  ShrUnLock(UCHAR uchTermAdr);
SHORT  ShrPrevDataCheck(SHRHEADERINF *pInf, UCHAR uchInfNo);
SHORT  ShrMakeRetVal(SHRHEADERINF *pInf);
SHORT  ShrFrameCheck(SHRHEADERINF *pInf, UCHAR uchInfNo);
VOID   ShrDataLost(UCHAR uchInfNo);
SHORT  ShrWriteSpool(SHRHEADERINF *pInf);
VOID   ShrWriteFile(VOID *pData, USHORT usDataLen);


/* -- ShrIn.c -- */
VOID THREADENTRY ShrPrintProcess(VOID);
SHORT  ShrRead1Frame(VOID *pBuff);
VOID   ShrCallPmg(UCHAR  *puchData, SHORT sDataLen);

/* ---------- End of ShrLocal.H ---------- */
