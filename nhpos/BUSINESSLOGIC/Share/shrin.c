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
* Title       : Shared Printer Manager Local Function                 
* Category    : Shared Printer Manager, NCR 2170 US HOSP Appl
* Program Name: ShrIn.C                                                      
* --------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*   ShrPrintProcess()
*    -ShrRead1Frame()
*    -ShrCallPmg()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Jun-16-93 : 00.00.01 : NAKAJIMA,Ken : Initial                                   
* Jul-13-93 : 00.00.01 : NAKAJIMA,Ken : Add PmgWait() semaphore.
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata      : V1.0 Initial
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
/**
;===========================================================================
;+                  I N C L U D E     F I L E s                            +
;===========================================================================
**/
#include <tchar.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <appllog.h>
#include <pmg.h>
#include <shr.h>
#include "shrlocal.h"
#include "paraequ.h"
/*
============================================================================
+                  S T A T I C   R A M s                                   +
============================================================================
*/

extern USHORT  usShrSemStartSPP;                /* semaphore handle */
extern USHORT  usShrSemRWDisk;
extern USHORT  usShrSemDrawer;
extern USHORT  usShrSemPmgWait;

extern SHORT   hsShrSpool;                      /* spool file handle */

extern USHORT  usShrReadP;                      /* file pointer */
extern USHORT  usShrWriteP;                     /* file pointer */
extern USHORT  usShrFileEnd;

extern USHORT  fbShrSSFFull;                    /* file full flag */

/**
;===========================================================================
;+              P R O G R A M    D E C L A R A T I O N s                   +
;===========================================================================
**/

/*
*===========================================================================
** Format  : VOID THREADENTRY  ShrPrintProcess(VOID)
*               
*    Input : none
*   Output : none
*            none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints spooled data, using print manager function. 
*===========================================================================
*/
VOID  THREADENTRY  ShrPrintProcess(VOID)
{
    //working with wide characters so should have enough space 
	//to fit '' number of characters.
	UCHAR           auchData[SHR_FRAME_SIZE]; 
    SHORT           sRet;
    SHRHEADERINF   *pHead;

	// Make sure that the frame size - the header is an even value.
	NHPOS_ASSERT( ((SHR_FRAME_SIZE - sizeof(SHRHEADERINF)) % 2) == 0);

    pHead  = (SHRHEADERINF *)auchData;
    for(;;) {                                   /* forever */
        PifRequestSem(usShrSemStartSPP);
        for (;;) {
            sRet = ShrRead1Frame(auchData);     /* read 1 frame data */
            if (sRet == SHR_NOT_EXEC)  break;
            ShrCallPmg(&auchData[sizeof(SHRHEADERINF)], (SHORT)(pHead->usILI - sizeof(SHRHEADERINF)));
                                                /* call print manager function */
            if (sRet == SHR_BREAK)  break;
        } 
    }
}


/*
*===========================================================================
** Format  : SHORT   ShrRead1Frame(VOID *pBuff)
*               
*    Input : pBuff                              -Read Buffer pointer
*   Output : none
*            none
*    InOut : none
** Return  : none
*
** Synopsis: This function read 1 frame data from file to specified buffer.
*===========================================================================
*/
SHORT   ShrRead1Frame(VOID *pBuff)
{
    ULONG   ulDumy, ulActualBytesRead;  //JHHJ 11-10-03
    SHORT   sRet = 0;

    PifRequestSem(usShrSemRWDisk);              /* request sem. for access to disk */

    if (  (usShrWriteP  == usShrReadP) && !(fbShrSSFFull &  SHR_SSF_FULL)  ) {
        PifReleaseSem(usShrSemRWDisk);          /* release sem. for access to disk */
        return (SHR_NOT_EXEC);
    }

    if ( (fbShrSSFFull & SHR_SSF_FULL) && (usShrReadP >= usShrFileEnd) ) {
        usShrReadP    = SHR_FILE_TOP_OFF;       /* move read point to file head */
        fbShrSSFFull &= ~(SHR_SSF_FULL);        /* clear file full flag */
    }

    if ( PifSeekFile(hsShrSpool, usShrReadP, &ulDumy) < 0 ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_SEEK);
    }
    if ( (ULONG)usShrReadP != ulDumy ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_SEEK);
    }

	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
    PifReadFile(hsShrSpool, pBuff, SHR_FRAME_SIZE, &ulActualBytesRead);  /* read data from spool file */

    usShrReadP += ((SHRHEADERINF *)pBuff)->usILI;

    if ( usShrReadP >= SHR_SPOOL_SIZE ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_BAD_LENGTH);
    }

    if (  (usShrWriteP  == usShrReadP) && !(fbShrSSFFull &  SHR_SSF_FULL)  ) {
        sRet = SHR_BREAK;
    }

    PifReleaseSem(usShrSemRWDisk);              /* release sem. for access to disk */

    return(sRet);
}


/*
*===========================================================================
** Format  : VOID   ShrCallPmg(UCHAR  *puchData, USHORT usDataLen)
*               
*    Input : puchData                   -print data pointer
*            usDataLen                  -print data length
*   Output : none
*            none
*    InOut : none
** Return  : none
*
** Synopsis: This function call print manager function.
*            
*   -function receives following data          
*            
*   1byte
*   +---+------------------------+---+------------------+---+------------+
*   |ILI|   string data          |ILI|   string data    |ILI| string data|       
*   +---+------------------------+---+------------------+---+------------+
*   |<----------------------    usDataLen  ----------------------------->|
*   |
* puchData
*
		This function must line up its use of the buffer referenced
		through the pointer puchData with the use of the buffer area
		as created in function PrtShrPrint().
*===========================================================================
*/
VOID   ShrCallPmg(UCHAR  *puchData, SHORT sDataLen)
{
	USHORT usPrntStatus = 0;
    UCHAR uchLineLen;
	USHORT  fbPrtStatus = 0;                /* printer status */

    for (;                             
         sDataLen > 0;                          /* read all buffer ? */
		 sDataLen -= uchLineLen + 1) {          /* 1 for line length area */

		uchLineLen = *puchData;                 /* get ILI, number of bytes in print line */
		puchData++;

        /* -- if drawer not provided, not request semaphore -- */
        PifRequestSem(usShrSemDrawer);
        /* -- font change -- */
        if ( ( uchLineLen  == sizeof(SHRFONTMSG) ) &&
             ( ((SHRFONTMSG *)puchData)->uchHead1 == SHR_FONT_MSG_1 ) &&
             ( ((SHRFONTMSG *)puchData)->uchHead2 == SHR_FONT_MSG_2 )    )
		{
            PmgFont(PMG_PRT_RECEIPT, ((SHRFONTMSG *)puchData)->usType);
        } else if ( ( uchLineLen  == sizeof(SHRPRNTSTATMSG) ) &&
             ( ((SHRFONTMSG *)puchData)->uchHead1 == SHR_STAT_MSG_1 ) &&
             ( ((SHRFONTMSG *)puchData)->uchHead2 == SHR_STAT_MSG_2 )    )
		{
			PmgGetStatus(PMG_PRT_RECEIPT, &fbPrtStatus);
			if ( fbPrtStatus & PRT_STAT_POWER_OFF) {
				fbShrSSFFull |= SHR_PRINTER_ERR;
			}
			else {
				fbShrSSFFull &= ~SHR_PRINTER_ERR;
			}
		}
		else {
            if(CliParaMDCCheck(MDC_DBLSIDEPRINT_ADR,ODD_MDC_BIT3))
			{
				usPrntStatus = PmgDblShrdPrint(PMG_PRT_RECEIPT, (USHORT* ) puchData, (USHORT)(uchLineLen/sizeof(TCHAR)));
			}
			else
			{
			   usPrntStatus = PmgPrint(PMG_PRT_RECEIPT, (USHORT* ) puchData, (USHORT)(uchLineLen/sizeof(TCHAR)));
			}
			// if printer error set the fbShrSSFFull bit map appropriately
			// if not printer error, clear the bit in case it is set.
			if (usPrntStatus != PMG_SUCCESS) {
				fbShrSSFFull |= SHR_PRINTER_ERR;
			}
			else {
				fbShrSSFFull &= ~SHR_PRINTER_ERR;
			}
        }
        PifReleaseSem(usShrSemDrawer);

        puchData += uchLineLen; 
    }
}
/* ---------- End of ShrIn.c ---------- */

