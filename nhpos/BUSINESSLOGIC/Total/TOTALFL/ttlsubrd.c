/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Subroutine for File Read                         
* Category    : TOTAL, NCR 2170 US Hospitality Application         
* Program Name: Ttlsubrd.c                                                      
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlReadFile()     - File Read
*           SHORT TtlreadFin()      - Read Financial Total File
*           SHORT TtlreadHour()     - Read Hourly Total File
*           SHORT TtlDeptHeadRead() - Read Dept File Header
*           SHORT TtlDeptMiscRead() - Read Dept Misc Total Table
*           SHORT TtlDeptTotalRead() - Read Dept Total
*           SHORT TtlPLUHeadRead()  - Read PLU File Header
*           SHORT TtlPLUISearch()   - Search PLU Index Table
*           SHORT TtlPLUBlockRead() - Read PLU Block Table
*           SHORT TtlPLUTotalRead() - Read PLU Total
*           SHORT TtlPLUMiscRead()  - Read PLU Misc Total Table
*           SHORT TtlCpnHeadRead()  - Read Coupon File Header        R3.0
*           SHORT TtlCpnTotalRead() - Read Coupon Total              R3.0
*           SHORT TtlCasHeadRead()  - Read Cashier File Header
*           SHORT TtlCasIndexRead() - Read Cashier Index Table
*           SHORT TtlCasEmptyRead() - Read Cashier Empty Table
*           SHORT TtlCasEmptyAssin() - Assin Cashier Empty Table Bit
*           SHORT TtlCasAssin()     - Assin Cashier
*           SHORT TtlCasTotalRead() - Read Cashier Total 
*           SHORT TtlGetCasOffset() - Get Cashier Total Offset
*           SHORT TtlreadCas()      - Read Cashier
*           SHORT TtlEmptyTableSearch() - Search Empty Table
*           SHORT TtlGetFinFilePosition() - Get Financial Data Position
*           SHORT TtlIndFinHeadRead() - Read Individual File Header, R3.1
*           SHORT TtlIndFinTotalRead() - Read Individual File Total, R3.1
*           SHORT TtlIndFinIndexRead() - Read Individual File Index, R3.1
*           SHORT TtlIndFinISearch()   - Search Individual File Table, R3.1
*           int   TtlcmpIndFingle()    - Search Method
*           SHORT TtlreadIndFin() -  Read Individual File, R3.1
*           SHORT TtlGetIndFinOffset() - Get Individual Financial Offset, R3.1
*           SHORT TtlIndFinAssin()  -   Assign Individual Financial, R3.1
*           SHORT TtlIndFinEmptyAssin() - Assin Individual Financial Empty Table Bit
*           SHORT TtlIndFinEmptyRead()  - Read Terminal Empty Table, R3.1
*           SHORT TtlreadSerTime()  -   Read Service Time Total, R3.1
*           SHORT TtlGetSerTimeFilePosition() - Get Service Time Position, R3.1
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name     : Description
* May-06-92 : 00.00.01 :K.Teari     :                                    
* Mar-03-95 : 03.00.00 :hkato       : R3.0
* Nov-15-95 : 03.01.00 :T.Nakahata  : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Aug-11-99 : 03.05.00 : K.Iwata    : R3.5 Remov WAITER_MODEL
* Aug-13-99 : 03.05.00 : K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-14-99 : 01.00.00 : hrkato     : Saratoga
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
#include	<tchar.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ecr.h>
#include <pif.h>
#include <rfl.h>
#include <plu.h>
#include <csttl.h>
#include <ttl.h>
#include <appllog.h>
#include <log.h>

#include "pluttld.h"    /* ### ADD 2172 Rel1.0 (12.30.99) */


/*
*============================================================================
**Synopsis:     int TtlDeptcmpgle(TTLCSDEPTINDX  *elem1, TTLCSDEPTINDX *elem2)
*
*    Input:     TTLCSDEPTINDX  *elem1    - Dept Number 1
*               TTLCSDEPTINDX  *elem2    - Dept Number 2
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      
*
** Description  This function compares Dept Number.
*
*============================================================================
*/
static SHORT TtlDeptcmpgle(const TTLCSDEPTINDX  *elem1, const TTLCSDEPTINDX *elem2) /* ### Mod (2171 for Win32)V1.0 */
{
    if (elem1->usDeptNo > elem2->usDeptNo) {
        return 1;
    }
    if (elem1->usDeptNo < elem2->usDeptNo) {
        return -1;
    } 
    return 0;
}

/*
*============================================================================
**Synopsis:     int TtlcmpCasgle(TTLCSCASINDX  *elem1, TTLCSCASINDX *elem2)
*
*    Input:     TTLCSCASINDX  *elem1    - Cashier Number 1
*               TTLCSCASINDX  *elem2    - Cashier Number 2
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      
*
** Description  This function compares Cashier Number.
*
*============================================================================
*/
static SHORT TtlcmpCasgle(const TTLCSCASINDX  *elem1, const TTLCSCASINDX *elem2) /* ### Mod (2171 for Win32)V1.0 */
{
    if (elem1->ulCasNo > elem2->ulCasNo) {
        return 1;
    }
    if (elem1->ulCasNo < elem2->ulCasNo) {
        return -1;
    } 
    /* if (elem1->usCasNo == elem2->usCasNo) { *//* ### Del (2171 for Win32) V1.0 */
        return 0;
    /* } *//* ### Del (2171 for Win32) V1.0 */
}

/*
*============================================================================
**Synopsis:     int TtlcmpIndFingle(TTLCSINDINDX  *elem1, TTLCSINDINDX *elem2)
*
*    Input:     TTLCSINDINDX  *elem1    - Terminal Number 1
*               TTLCSINDINDX  *elem2    - Terminal Number 2
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      
*
** Description  This function compares Terminal Number.
*
*============================================================================
*/
static int TtlcmpIndFingle(const TTLCSINDINDX  *elem1, const TTLCSINDINDX  *elem2) /* ### Mod (2171 for Win32) V1.0 */
{
	if (elem1->usTerminalNo > elem2->usTerminalNo) {
        return 1;
    }
    if (elem1->usTerminalNo < elem2->usTerminalNo) {
        return -1;
    } 
    /* if (elem1->usTerminalNo == elem2->usTerminalNo) { *//* ### Del (2171 for Win32) V1.0 */
        return 0;
    /* } *//* ### Del (2171 for Win32) V1.0 */
}

/*
*============================================================================
**Synopsis:     SHORT TtlEmptyTableSearch(USHORT *pTmpBuff, USHORT usMaxNo,
*                                         USHORT *pusOffset)
*
*    Input:     USHORT *pTmpBuff        - Pointer of Empty Table
*               USHORT usMaxNo          - Number of Max Empty Table 
*               USHORT *pusOffset       - Pointer of Target Total Offset Set Buffer
*
*   Output:     USHORT *pusOffset       - Target Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Searching Empty Table 
*               TTL_NOTEMPTY - Failed Searching Empty Table(Empty Table Full)
*
** Description  This function searchs the Empty Table.
*
*============================================================================
*/
static SHORT TtlEmptyTableSearch(USHORT *pTmpBuff, USHORT usMaxNo, USHORT *pusOffset)
{
    USHORT  i;
    USHORT  usWork;

    for( i = 0; i < usMaxNo; i += 16) {
        if ((usWork = *(pTmpBuff + (i / 16))) != 0xffff) {
            break;
        }
    }
    for ( ; i < usMaxNo ; i++) {
        if ((usWork & (1 << (i % 16))) == 0) {
                                    /* Check Empty Table */
            *pusOffset = i;         /* Set Emtpy Block Offset */
            return (TTL_SUCCESS);   /* Return Success */
        }
    }
    return (TTL_NOTEMPTY);          /* Return Empty Table Full */
}

/*
*============================================================================
**Synopsis:     SHORT TtlReadFile(USHORT hsHandle, ULONG ulPosition, 
*                                 VOID *pTmpBuff, ULONG ulSize)
*
*    Input:     USHORT hsHandle         - File Handle
*               ULONG ulPosition        - Read File Position
*               VOID *pTmpBuff          - Pointer of Read File Data Buffer
*               USHORT usSize           - Read Buffer Size
*
*   Output:     VOID *pTmpBuff          - Read File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Traget.
*
*============================================================================
*/
#if defined(TtlReadFile)
SHORT   TtlReadFile_Special(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize);
SHORT   TtlReadFile_Debug(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize, char *aszFilePath, int nLineNo)
{
	int    iLen = 0;
	char   xBuffer[256];
	SHORT  sResult;

	sResult = TtlReadFile_Special(hsHandle, ulPosition, pTmpBuff, ulSize);

	if (sResult != TTL_SUCCESS) {
		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf (xBuffer, "TtlReadFile_Debug(): sResult = %d  File %s  lineno = %d", sResult, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sResult;
}

SHORT   TtlReadFile_Special(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize)
#else
SHORT TtlReadFile(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize)
#endif
{
    ULONG   ulActualPosition, ulActualBytesRead;   //JHHJ 11-10-03/* Actual Seek Pointer */
	SHORT   sRetStatus;

    if (ulSize == 0) {
        return (TTL_SUCCESS);           /* Return Success */
    }

	if (hsHandle < 0) {                /* Check for valid File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

    if ((sRetStatus = PifSeekFile(hsHandle, ulPosition, &ulActualPosition)) < 0) {
        return (TTL_FILE_SEEK_ERR);
    }
	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
    sRetStatus = PifReadFile(hsHandle, pTmpBuff, ulSize, &ulActualBytesRead);
    if (ulActualBytesRead != ulSize) {
		char  xBuff[128];
		sprintf(xBuff, "TtlReadFile(): hsHandle=%d  ulActualBytesRead=%d  ulPosition=%d  ulSize=%d  sRetStatus = %d", hsHandle, ulActualBytesRead, ulPosition, ulSize, sRetStatus);
		NHPOS_ASSERT_TEXT((ulActualBytesRead == ulSize), xBuff);
        return (TTL_FILE_READ_ERR); /* Return File Read Error */
    }
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     TtlreadFin(UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               TTLCSREGFIN *pTmpBuff   - Pointer of Financial Read Buffer
*
*   Output:     TTLCSREGFIN *pTmpBuff   - Financial Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Financial Total File 
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Financial Total File.
*
*============================================================================
*/
SHORT TtlreadFin(UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff)
{
    ULONG  ulPosition;         /* Seek Position */
    SHORT  sRetvalue;

    sRetvalue = TtlGetFinFilePosition(uchMinorClass, &ulPosition);

	// zero out the buffer
	// Now people can depend on the values to be zero if record does not exist.
	memset (pTmpBuff, 0, sizeof(TTLCSREGFIN));

    if (sRetvalue == TTL_SUCCESS) {
		/* --- Read Financial Total --- */
		sRetvalue = TtlReadFile (hsTtlBaseHandle, ulPosition, pTmpBuff, sizeof(TTLCSREGFIN));
    }

    return (sRetvalue);
}
/*
*============================================================================
**Synopsis:     SHORT TtlGetFinFilePosition(UCHAR uchClass, ULONG *pulPosition)
*
*    Input:     UCHAR uchClass          - Minor Class
*               ULONG *pulPosition      - File Position
*
*   Output:     ULONG *pulPosition      - File Position Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function gets the Target Financial Data Position.
*
*============================================================================
*/
SHORT TtlGetFinFilePosition(UCHAR uchClass, ULONG *pulPosition)
{
    if (hsTtlBaseHandle < 0) {      /* Check Base Total File Handle */
		NHPOS_ASSERT(hsTtlBaseHandle >= 0);
        return (TTL_FILE_HANDLE_ERR);
    }

	/* --- Set Read Total Offset --- */
    switch(uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        *pulPosition = TTL_FIN_DAYCURR;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        *pulPosition = TTL_FIN_DAYCURR + TTL_FIN_SIZE;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
        *pulPosition = TTL_FIN_PTDCURR;
        break;

    case CLASS_TTLSAVPTD:           /* Previous PTD File */
        *pulPosition = TTL_FIN_PTDCURR + TTL_FIN_SIZE;
        break;

    default:
        return (TTL_NOTCLASS);      /* Return Not Class */
    }
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlreadHour(UCHAR uchMinorClass, UCHAR uchOffset,
*                                 HOURLY *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               UCHAR uchOffset         - Hourly Block Offset
*               HOULY *pTmpBuff         - Pointer of Hourly Read Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Hourly Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS - Set Illigal Class Code 
*
** Description  This function reads the Hourly Total File.
*
*============================================================================
*/
SHORT TtlreadHour(UCHAR uchMinorClass, UCHAR uchOffset, HOURLY *pTmpBuff)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    /* Get Hourly Data Position */
    sRetvalue = TtlGetHourFilePosition(uchMinorClass, &ulPosition);

	// zero out the buffer
	// Now people can depend on the values to be zero if record does not exist.
	memset (pTmpBuff, 0, sizeof(HOURLY));

    if (sRetvalue == TTL_SUCCESS) {
		/* --- Read Hourly Total --- */
		ulPosition = ulPosition + TTL_STAT_SIZE + (sizeof(HOURLY) * uchOffset);
		sRetvalue = TtlReadFile (hsTtlBaseHandle, ulPosition, pTmpBuff, sizeof(HOURLY));
    } else {
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_POINTER_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchMinorClass);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchOffset);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
	}

    return (sRetvalue);
}
/*
*============================================================================
**Synopsis:     SHORT TtlGetHourFilePosition(UCHAR uchClass, ULONG *pulPosition)
*
*    Input:     UCHAR uchClass          - Minor Class
*               ULONG *pulPosition      - File Position
*
*   Output:     ULONG *pulPosition      - File Position Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function gets the Target Hourly Data Position.
*
*============================================================================
*/
SHORT TtlGetHourFilePosition(UCHAR uchClass, ULONG *pulPosition)
{
    if (hsTtlBaseHandle < 0) {      /* Check Base Total File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

	/* --- Set Read Total Offset --- */
    switch(uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        *pulPosition = TTL_HOUR_DAYCURR;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        *pulPosition = TTL_HOUR_DAYCURR + TTL_HOUR_SIZE;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
        *pulPosition = TTL_HOUR_PTDCURR;
        break;

    case CLASS_TTLSAVPTD:           /* Previous PTD File */
        *pulPosition = TTL_HOUR_PTDCURR + TTL_HOUR_SIZE;
        break;

    default:
        return (TTL_NOTCLASS);      /* Return Not Class */
    }
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlDeptHeadRead(TTLCSDEPTHEAD *pDepthd)
*
*    Input:     TTLCSDEPTHEAD *pDepthd    - Pointer of Dept File Header Read Buffer
*
*   Output:     TTLCSDEPTHEAD *pDepthd    - Dept File Header Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Dept File Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Dept Header.
*
*============================================================================
*/
SHORT TtlDeptHeadRead(TTLCSDEPTHEAD *pDepthd)
{
    return(TtlReadFile(hsTtlDeptHandle, 0L, pDepthd, sizeof(TTLCSDEPTHEAD)));  /* Read the Dept File Header Data */
}

/*
*============================================================================
**Synopsis:     SHORT TtlDeptMiscRead(ULONG ulPosition, TTLCSDEPTMISC *pDeptMisc)
*
*    Input:     ULONG ulPosition        - Dept Misc Total Position
*               TTLCSDEPTMISC *pDeptMisc  - Pointer of Dept Misc Total Read Buffer
*
*   Output:     TTLCSDEPTMISC *pDeptMisc  - Dept Misc Total Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Dept Misc Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Dept Block Table.
*
*============================================================================
*/
SHORT TtlDeptMiscRead(ULONG ulPosition, TTLCSDEPTMISC *pDeptMisc)
{
    return(TtlReadFile(hsTtlDeptHandle, ulPosition, pDeptMisc, sizeof(TTLCSDEPTMISC)));  /* Read the Dept Misc Data */
}

/*
*============================================================================
**Synopsis:     SHORT TtlDeptISearch(ULONG ulPosition, USHORT usDeptNo, 
*                                   TTLCSDEPTINDX *DeptInd, VOID *pTmpBuff,
*                                   USHOT usSize, USHORT *pusOffset, 
*                                   UCHAR uchMethod)
*
*    Input:     ULONG ulPosition        - Search Index Table Position
*               USHORT usDeptNo          - Number of Dept
*               TTLCSDEPTINDX *DeptInd    - Pointer of Target Dept number
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHOT usSize            - Work Buffer Size
*               USHORT *pusOffset       - Pointer of Target Dept Offset Buffer
*               UCHAR uchMethod         - Method
*
*   Output:     USHORT *Offset          - Target Dept Offset Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Searching Dept 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE       - Not IN FILE Dept
*
** Description  This function searchs the Dept Index Table.
*
*============================================================================
*/
SHORT TtlDeptISearch(ULONG ulPosition, USHORT usDeptNo, TTLCSDEPTINDX *DeptInd,
                    VOID *pTmpBuff, ULONG *pulOffset, UCHAR uchMethod)
{
    USHORT  usNoDept;            /* Counter */
    ULONG   ulPositionInd;      /* Pointer */
    ULONG   ulPositionPreInd;   /* Pointer */
    SHORT   sRetvalue;

    *pulOffset = 0;

    usNoDept = usDeptNo;
    ulPositionInd =  ulPosition;
    ulPositionPreInd = ulPosition;
    for (;usNoDept > TTL_DEPT_TMPBUF_INDEXREAD;) {
		ULONG   ulActualPosition, ulActualBytesRead;
		USHORT  usUpNoDept = usNoDept / 2 + (usNoDept % 2);      /* counter */

        usNoDept /= 2;
        ulPositionInd += usNoDept * sizeof(TTLCSDEPTINDX); 
        if (PifSeekFile(hsTtlDeptHandle, ulPositionInd, &ulActualPosition) < 0) {   /* Seek Pointer of Dept Index Table */
            return (TTL_FILE_SEEK_ERR);
        }
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlDeptHandle, pTmpBuff, sizeof(TTLCSDEPTINDX), &ulActualBytesRead);
        if (ulActualBytesRead != sizeof(TTLCSDEPTINDX)) {
            return (TTL_FILE_READ_ERR);
        }

        sRetvalue = TtlDeptcmpgle(DeptInd, pTmpBuff);
        if (sRetvalue == 0) {
            *pulOffset += usNoDept;
            return (TTL_SUCCESS);       /* Return SUCCESS */
        } else if (sRetvalue < 0) {
            ulPositionInd = ulPositionPreInd;
        } else {
            ulPositionPreInd = ulPositionInd;
            *pulOffset += usNoDept;
            usNoDept = usUpNoDept;
        }
    }

    if (usNoDept) {
		ULONG   ulActualPosition, ulActualBytesRead;
		ULONG	ulReadlength;   /* Reading length of buffer */  //JHHJ 11-10-03
		CHAR    *RetPointer;        /* Return Value */

        if (PifSeekFile(hsTtlDeptHandle, ulPositionInd, &ulActualPosition) < 0) {    /* Seek Pointer of Dept Index Table */
            return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
        }

        ulReadlength = usNoDept * sizeof(TTLCSDEPTINDX);   /* Calculate Reading length */
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlDeptHandle, pTmpBuff, ulReadlength, &ulActualBytesRead);
        if (ulActualBytesRead != ulReadlength) {
            return (TTL_FILE_READ_ERR);/* Return File Read Error */
        }

        sRetvalue = Rfl_SpBsearch(DeptInd, sizeof(TTLCSDEPTINDX), pTmpBuff, usNoDept, &RetPointer, TtlDeptcmpgle);
        *pulOffset += (USHORT)((RetPointer - (CHAR *)pTmpBuff) / sizeof(TTLCSDEPTINDX));           /* ### Mod V1.0 */
        if (sRetvalue == RFL_HIT) {
            return (TTL_SUCCESS);   /* Return SUCCESS */
        }
        return (TTL_NOTINFILE);     /* Return Not In File */
    }

    if (uchMethod == TTL_SEARCH_GT) {
        *pulOffset = usDeptNo;
    }
    return (TTL_NOTINFILE);         /* Return Not In File */
}

/*
*============================================================================
**Synopsis:     SHORT TtlDeptBlockRead(ULONG ulPosition, USHORT usOffset,
*                                     USHORT *usDeptBlk)
*
*    Input:     ULONG ulPosition        - Dept Block Table Position
*               USHORT usOffset         - Target Block Offset
*               USHORT *usDeptBlk        - Pointer of Target Block Data Read Buffer
*
*   Output:     USHORT *usDeptBlk        - Target Block Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Dept Block Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Dept Block Table.
*
*============================================================================
*/
SHORT TtlDeptBlockRead(ULONG ulPosition, ULONG ulOffset, USHORT *pusDeptBlk)
{
    ULONG   ulPositionBlk = ulPosition + (ULONG)(ulOffset * TTL_DEPTBLOCKRECORD_SIZE);  /* Calculate Taget Block Data Offset */
    return(TtlReadFile(hsTtlDeptHandle, ulPositionBlk, pusDeptBlk, TTL_DEPTBLOCKRECORD_SIZE)); /* Read the Dept Block Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptTotalRead(ULONG ulPosition, USHORT usDeptBlk,
*                                     TOTAL *DepTtl)
*
*    Input:     ULONG ulPosition         - Dept TOTAL Table Position
*               USHORT usDeptBlk          - Target Dept Total Block Data
*               TOTAL *DeptTtl           - Pointer of Dept Total Read Buffer 
*
*   Output:     TOTAL *DeptTtl           - Dept Total Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Dept Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Dept Total.
*
*============================================================================
*/
SHORT TtlDeptTotalRead(ULONG ulPosition, USHORT usDeptBlk, TOTAL *pDeptTtl)
{
    ULONG   ulPositionTtl = ulPosition + ((ULONG)usDeptBlk * (ULONG)TTL_DEPTTOTALRECORD_SIZE);  /* Calculate Taget Total Data Offset */
    return(TtlReadFile(hsTtlDeptHandle, ulPositionTtl, pDeptTtl, TTL_DEPTTOTALRECORD_SIZE)); /* Read the Dept Total Data */
}

/*
*============================================================================
**Synopsis:     SHORT TtlDeptEmptyRead(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition        - Dept Empty Table Position
*               VOID *pTmpBuff          - Pointer of Dept Empty Table Data Read Buffer
*
*   Output:     VOID *pTmpBuff          - Dept Empty Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Dept Empty Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Dept Empty Table.
*
*============================================================================
*/
SHORT TtlDeptEmptyRead(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlReadFile(hsTtlDeptHandle, ulPosition, pTmpBuff, TTL_DEPTEMP_SIZE));   /* Read the Dept Empty Table */
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptAssin(TTLDEPT *pTotal, USHORT usPositionMsc, 
*                                 USHORT usPositionInd, USHORT usPositionBlk, 
*                                 USHORT usPositionEmp, USHORT *pusPositionTtl, 
*                                 VOID *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass          - Minor Class
*               USHORT usDeptNumber           - Dept Number
*               UCHAR  uchAjectNo            - Ajective Number
*               USHORT *pusPositionTtl       - Pointer of Dept Total Offset
*               VOID *pTmpBuff               - Pointer of Dept Empty Table Data Read Buffer
*
*   Output:     USHORT *pusPositionTtl       - Dept Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Assin Dept 
*               TTL_SIZEOVER        - Max Size Over
*               TTL_NOTEMPTY        - Empty Table Full Error
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assins the Dept.
*
*============================================================================
*/
SHORT TtlDeptAssin(TTLDEPT *pTotal, TTLCSFLDEPTHEAD *DeptSave, ULONG ulOffset, VOID *pTmpBuff)
{
    TTLCSDEPTHEAD    *pDepthd = pTmpBuff;   /* Pointer of Read Buffer for Dept File Header */
    TTLCSDEPTINDX    DeptNumber;            /* Dept Number, Ajective Number */
    USHORT           usEmpOffset;           /* Empty Block Offset */
    SHORT            sRetvalue;

    if (DeptSave->usMaxNo < (DeptSave->usDeptNo + 1)) {
        return (TTL_SIZEOVER);
    }
    
    /* Read Empty Table */
    if ((sRetvalue = TtlDeptEmptyRead(DeptSave->ulPositionEmp, pTmpBuff)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    /* Search Empty Table */
    if ((sRetvalue = TtlEmptyTableSearch(pTmpBuff, DeptSave->usMaxNo, &usEmpOffset)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    /* Update Empty Table */
    if ((sRetvalue = TtlEmptyTableUpdate(pTmpBuff, DeptSave->usMaxNo, usEmpOffset, TTL_EMPBIT_SET)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    /* Write Empty Table */
    if ((sRetvalue = TtlDeptEmptyWrite(DeptSave->ulPositionEmp, pTmpBuff)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

    /* Update Dept Index Table */
    DeptNumber.usDeptNo = pTotal->usDEPTNumber;/* Set Dept Number */
    if ((sRetvalue = TtlDeptIAssin(DeptSave->ulPositionInd, DeptSave->usDeptNo, &DeptNumber, pTmpBuff, ulOffset)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    /* Update Dept Block Table */
    if ((sRetvalue = TtlDeptBAssin(DeptSave->ulPositionBlk, DeptSave->usDeptNo, usEmpOffset, pTmpBuff, ulOffset)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    DeptSave->ulPositionTtl = DeptSave->ulPositionTtl + (TTL_DEPTTOTALRECORD_SIZE * usEmpOffset);

    /* Read Dept File Header */
    if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0){   
        return (sRetvalue);         /* Return Error Status */
    }

	/* --- Set Read Total Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:             /* Current Daily File */
        pDepthd->usCurDayDept += 1;      /* Set Number of Current Daily Dept */
        break;

    case CLASS_TTLSAVDAY:             /* Previous Daily File */
        pDepthd->usPreDayDept += 1;      /* Set Number of Previous Daily Dept */
        break;

    case CLASS_TTLCURPTD:             /* Current PTD File */
        pDepthd->usCurPTDDept += 1;      /* Set Number of Current PTD Dept */
        break;

    case CLASS_TTLSAVPTD:             /* Previous PTD File */
        pDepthd->usPrePTDDept += 1;      /* Set Number of Previous PTD Dept */
        break;

	/*  default:    Not Used */
    }

    /* Write Dept File Header */
    if ((sRetvalue = TtlDeptHeadWrite(pDepthd)) != 0){   
        return (sRetvalue);
    }
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlreadDept(TTLDEPT *pTotal, VOID *pTmpBuff, 
*                                ULONG  *pulPositionMsc, 
*                                ULONG  *pulPositionTtl, UCHAR uchMethod)
*
*    Input:     TTLDEPT *pTotal          - Major & Minor Class, Dept Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*               ULONG *pulPositionMsc   - Pointer of Misc Table Position
*               ULONG *pulPositionTtl   - Pointer of Total Position
*
*   Output:     TTLDEPT *pTotal          - Dept Total File Data
*               USHORT *pusPositionMsc  - Misc Table Position
*               USHORT *pusPositionTtl  - Total Position
*               Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reading Dept Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE       - Not In File Dept
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*
** Description  This function reads the Dept Total File.
*
*============================================================================
*/
SHORT TtlreadDept(TTLDEPT *pTotal, VOID *pTmpBuff, ULONG *pulPositionMsc,
                 ULONG *pulPositionTtl, UCHAR uchMethod)
{

    USHORT            usDeptBlk;   /* Dept Block Table Buffer */
    TOTAL             DeptTtl = {0};     /* Read buffer for Dept Total */
	TTLCSFLDEPTHEAD   DeptSave = {0};
    ULONG	ulOffset;           /* Offset */
    SHORT   sRetvalue;          /* Return Value */

    if (hsTtlDeptHandle < 0) {       /* Check Dept Total File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

	/* --- Read Dept File Header --- */
	{
		TTLCSDEPTHEAD    *pDepthd = pTmpBuff;    /* Pointer of Read Buffer for Dept File Header */

		if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {   
			return (sRetvalue);
		}

		DeptSave.usMaxNo = pDepthd->usMaxDept;    /* Set Max Number of Dept */
		/* --- Set Read Total Offset --- */
		switch(pTotal->uchMinorClass) {
		case CLASS_TTLCURDAY:                                 /* Current Daily File */
			DeptSave.usDeptNo = pDepthd->usCurDayDept;            /* Set No of Current Daily Dept Number */
			DeptSave.ulPositionMsc = pDepthd->ulCurDayMiscOff;    /* Set Current Daily Misc Table Offset */
			DeptSave.ulPositionInd = pDepthd->ulCurDayIndexOff;   /* Set Current Daily Index Table Offset */
			DeptSave.ulPositionBlk = pDepthd->ulCurDayBlockOff;   /* Set Current Daily Block Table Offset */
			DeptSave.ulPositionEmp = pDepthd->ulCurDayEmptOff;    /* Set Current Daily Empty Offset */
			DeptSave.ulPositionTtl = pDepthd->ulCurDayTotalOff;   /* Set Current Daily Total Table Offset */
			break;

		case CLASS_TTLSAVDAY:                                 /* Previous Daily File */
			DeptSave.usDeptNo = pDepthd->usPreDayDept;            /* Set No of Previous Daily Dept Number */
			DeptSave.ulPositionMsc = pDepthd->ulPreDayMiscOff;    /* Set Previous Daily Misc Table Offset */
			DeptSave.ulPositionInd = pDepthd->ulPreDayIndexOff;   /* Set Previous Daily Index Table Offset */
			DeptSave.ulPositionBlk = pDepthd->ulPreDayBlockOff;   /* Set Previous Daily Block Table Offset */
			DeptSave.ulPositionEmp = pDepthd->ulPreDayEmptOff;    /* Set Previous Daily Empty Offset */
			DeptSave.ulPositionTtl = pDepthd->ulPreDayTotalOff;   /* Set Previous Daily Total Table Offset */
			break;

		case CLASS_TTLCURPTD:                                 /* Current PTD File */
			if (pDepthd->ulCurPTDMiscOff == 0L) {                 /* Check Exist PTD Total File */
				return (TTL_NOTPTD);    /* Return Not PTD */
			}
			DeptSave.usDeptNo = pDepthd->usCurPTDDept;            /* Set No of Current PTD Dept Number */
			DeptSave.ulPositionMsc = pDepthd->ulCurPTDMiscOff;    /* Set Current PTD Misc Table Offset */
			DeptSave.ulPositionInd = pDepthd->ulCurPTDIndexOff;   /* Set Current PTD Index Table Offset */
			DeptSave.ulPositionBlk = pDepthd->ulCurPTDBlockOff;   /* Set Current PTD Block Table Offset */
			DeptSave.ulPositionEmp = pDepthd->ulCurPTDEmptOff;    /* Set Current PTD Empty Offset */
			DeptSave.ulPositionTtl = pDepthd->ulCurPTDTotalOff;   /* Set Current PTD Total Table Offset */
			break;

		case CLASS_TTLSAVPTD:                                 /* Previous PTD File */
			if (pDepthd->ulPrePTDMiscOff == 0L) {                 /* Check Exist PTD Total File */
				return (TTL_NOTPTD);    /* Return Not PTD */
			}
			DeptSave.usDeptNo = pDepthd->usPrePTDDept;            /* Set No of Previous PTD Dept Number */
			DeptSave.ulPositionMsc = pDepthd->ulPrePTDMiscOff;    /* Set Previous PTD Misc Table Offset */
			DeptSave.ulPositionInd = pDepthd->ulPrePTDIndexOff;   /* Set Previous PTD Index Table Offset */
			DeptSave.ulPositionBlk = pDepthd->ulPrePTDBlockOff;   /* Set Previous PTD Block Table Offset */
			DeptSave.ulPositionEmp = pDepthd->ulPrePTDEmptOff;    /* Set Previous PTD Empty Offset */
			DeptSave.ulPositionTtl = pDepthd->ulPrePTDTotalOff;   /* Set Previous PTD Total Table Offset */
			break;

		default:
			return (TTL_NOTCLASS);      /* Return Not Class */
		}
	}

    *pulPositionMsc = DeptSave.ulPositionMsc;                 /* Set Target Misc Table Position */

	{
		TTLCSDEPTMISC    *pDeptMisc = pTmpBuff;  /* Read Buffer for Dept Misc */

		/* --- Read PLU Misc Table --- */
		if ((sRetvalue = TtlDeptMiscRead(DeptSave.ulPositionMsc, pDeptMisc)) != 0) {
			return (sRetvalue);         /* Rteurn Error Status */
		}

		/* --- Copy To Interface Data Format --- */
		/* Copy Reset status, From & To Date */
		pTotal->uchResetStatus = pDeptMisc->uchResetStatus;
		pTotal->FromDate = pDeptMisc->FromDate;
		pTotal->ToDate = pDeptMisc->ToDate;
		pTotal->DEPTAllTotal = pDeptMisc->DeptAllTotal;                   /* Copy All Dept Total */
	}

    if ((DeptSave.usDeptNo == 0) && (uchMethod == TTL_SEARCH_EQU)) {  /* Check Number of Previous PTD Dept */
        return (TTL_NOTINFILE); /* Return Not In File */
    }

	/* --- Search Dept Index Table --- */
    if ((sRetvalue = TtlDeptISearch(DeptSave.ulPositionInd, DeptSave.usDeptNo,
                                   (TTLCSDEPTINDX *)&pTotal->usDEPTNumber, 
                                    pTmpBuff, &ulOffset, uchMethod)) != 0) {
                                /* Search Index Table By Dept Number */
        if ((uchMethod == TTL_SEARCH_GT) && (sRetvalue == TTL_NOTINFILE)) {
            if ((sRetvalue = TtlDeptAssin(pTotal, &DeptSave, ulOffset, pTmpBuff)) != 0) {    /* Assin Dept Total */
                return (sRetvalue); /* Return Error Status */
            }
            *pulPositionTtl = DeptSave.ulPositionTtl;
        }
        return (sRetvalue);     /* Rteurn Error Status */
    }

	/* --- Read Dept Block Table --- */
    if ((sRetvalue = TtlDeptBlockRead(DeptSave.ulPositionBlk, ulOffset, &usDeptBlk)) != 0) {
        return (sRetvalue);         /* Rteurn Error Status */
    }

    /* Read Target Dept Total Data */
    if ((sRetvalue = TtlDeptTotalRead(DeptSave.ulPositionTtl, usDeptBlk, &DeptTtl)) != 0) {
        return (sRetvalue);         /* Rteurn Error Status */
    }

    *pulPositionTtl = DeptSave.ulPositionTtl + (usDeptBlk * TTL_DEPTTOTALRECORD_SIZE);   /* Set Target Total Position */

	/* --- Copy To Interface Data Format --- */
    pTotal->DEPTTotal.lAmount = DeptTtl.lAmount;
    pTotal->DEPTTotal.lCounter = DeptTtl.sCounter;
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlCpnHeadRead(TTLCSCPNHEAD *pCpnhd)
*
*    Input:     TTLCSCPNHEAD  *pCpnhd   - Pointer of Coupon File Header Read Buffer
*
*   Output:     TTLCSCPNHEAD  *pCpnhd   - Coupon File Header
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Coupon File Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Coupon File Header.
*                             R3.0
*============================================================================
*/
SHORT TtlCpnHeadRead(TTLCSCPNHEAD *pCpnhd)
{
    return(TtlReadFile(hsTtlCpnHandle, 0L, pCpnhd, sizeof(TTLCSCPNHEAD)));   /* Read the Coupon File Header Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCpnMiscRead(ULONG ulPosition, 
*                                     TTLCSCPNMISC *pCpnMisc)
*
*    Input:     ULONG ulPosition         - Coupon Misc Total Read Position
*               TTLCSCPNMISC *pCpnMisc - Pointer of Coupon Misc Total Read Buffer
*
*   Output:     TTLCSCPNMISC *pCpnMisc  - Coupon Misc Total
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Coupon Misc Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Coupon Misc Total.
*                                R3.0
*============================================================================
*/
SHORT TtlCpnMiscRead(ULONG ulPosition, TTLCSCPNMISC *pCpnMisc)
{
    return(TtlReadFile(hsTtlCpnHandle, ulPosition, pCpnMisc, sizeof(TTLCSCPNMISC)));   /* Read the Coupon Misc Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCpnTotalRead(ULONG ulPosition, USHORT usCpnNum,
*                                     VOID *pTmpBuff, USHORT usSize)
*
*    Input:     ULONG ulPosition        - Coupon Total Read Position
*               USHORT usCpnNum        - Traget Coupon Number
*               VOID *pTmpBuff          - Pointer of Coupon Total Read Buffer
*               USHORT usSize           - Coupon Total Read Buffer Size
*
*   Output:     VOID *pTmpBuff          - Coupon Total Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Coupn Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Coupon Total.
*                                R3.0
*============================================================================
*/
SHORT TtlCpnTotalRead(ULONG ulPosition, USHORT usCpnNum, VOID *pTmpBuff, USHORT usSize)
{
    ULONG   ulPositionTtl = ulPosition + TTL_CPNMISC_SIZE + (((ULONG)usCpnNum - 1L) * (ULONG)usSize);  /* Calculate Taget Total Data Offset */
    return(TtlReadFile(hsTtlCpnHandle, ulPositionTtl, pTmpBuff, usSize));    /* Read the Total Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlreadCpn(TTLCPN *pTotal)
*
*    Input:     TTLCPN  *pTotal         - Major & Minor Class ,Coupon Number
*
*   Output:     Nothing
*    InOut:     TTLCPN  *pTotal         - Coupon Total File Data
*
** Return:      TTL_SUCCESS         - Successed the Reading Coupon Total  
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_SIZEOVER        - Max Size Over
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Coupon Total File.
*                                R3.0
*============================================================================
*/
SHORT TtlreadCpn(TTLCPN *pTotal)
{
	ULONG         ulPosition;
	TTLCSCPNHEAD  Cpnhd = {0};      /* Coupon File Header Read Buffer */
	TTLCSCPNMISC  CpnMisc = {0};    /* Coupon Misc Total Read Buffer */
    TOTAL         CpnTtl = {0};     /* Read buffer for Coupon Total */ 
	N_DATE        ClearedDate = {0};
    SHORT         sRetvalue;

    /* Get Coupon Data Position */
    sRetvalue = TtlGetCpnFilePosition(pTotal->uchMinorClass, &ulPosition, &Cpnhd);

	// zero out the buffer so that we can return something reasonable
    pTotal->uchResetStatus = 0;                    /* Copy Reset status, From & To Date */
    pTotal->FromDate = ClearedDate;                /* Copy Reset status, From & To Date */
    pTotal->ToDate = ClearedDate;                  /* Copy Reset status, From & To Date */
	pTotal->CpnTotal = CpnTtl;                     // clear the coupon totals in case of an error below.

    if (sRetvalue != 0) {
        return (sRetvalue);
    }

    if (pTotal->usCpnNumber > Cpnhd.usMaxCpn) {
        return (TTL_SIZEOVER);      /* Return Max Size Over */
    }

	/* --- Read Coupon Misc Total --- */
    if ((sRetvalue = TtlCpnMiscRead(ulPosition, &CpnMisc)) != 0) {
        return (sRetvalue);
    }

	/* --- Read Target Coupon Total --- */
     switch(pTotal->uchMinorClass) {
     case CLASS_TTLCURDAY:          /* Current Daily File */
     case CLASS_TTLSAVDAY:          /* Previous Daily File */
        if ((sRetvalue = TtlCpnTotalRead(ulPosition, pTotal->usCpnNumber, &CpnTtl, sizeof(CpnTtl))) != 0) {
            return (sRetvalue);
        }
        break;

     case CLASS_TTLCURPTD:          /* Current PTD File */
     case CLASS_TTLSAVPTD:          /* Previous PTD File */
        if ((sRetvalue = TtlCpnTotalRead(ulPosition, pTotal->usCpnNumber, &CpnTtl, sizeof(CpnTtl))) != 0) {
            return (sRetvalue);
        }
        break;

		/*  default:     Not Used */
     }

	/* --- Copy To Interface Data Format --- */
    pTotal->uchResetStatus = CpnMisc.uchResetStatus;      /* Copy Reset status, From & To Date */
    pTotal->FromDate = CpnMisc.FromDate;                  /* Copy Reset status, From & To Date */
    pTotal->ToDate = CpnMisc.ToDate;                      /* Copy Reset status, From & To Date */
    pTotal->CpnTotal = CpnTtl;                            /* Copy Target Coupon Total */
    return (TTL_SUCCESS);          /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlGetCpnFilePosition(UCHAR uchClass, ULONG *pulPosition,
*                                            TTLCSCPNHEAD   *pCpnhd)
*
*    Input:     UCHAR uchClass          - Minor Class
*               ULONG *pulPosition      - File Position
*               TTLCSCPNHEAD  *pCpnhd   - Coupon File Header Read Buffer Pointer
*
*   Output:     ULONG *pulPosition      - File Position Data
*               TTLCSCPNHEAD  *pCpnhd   - Coupon File Header Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function gets the Traget Coupon Data Position.
*                             R3.0
*============================================================================
*/
SHORT TtlGetCpnFilePosition(UCHAR uchClass, ULONG *pulPosition, TTLCSCPNHEAD  *pCpnhd)
{
    SHORT    sRetvalue;

    if (hsTtlCpnHandle < 0) {       /* Check Coupon Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CPN_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
     }

	/* --- Read Coupon File Header --- */
    if ((sRetvalue = TtlCpnHeadRead(pCpnhd)) != 0) {   /* Read Coupon File Header */
        return (sRetvalue);         /* Return Error Status */
    }

	/* --- Set Read Total Offset --- */
     switch(uchClass) {
     case CLASS_TTLCURDAY:                  /* Current Daily File */
          *pulPosition = pCpnhd->usCurDayTtlOff;
          break;

     case CLASS_TTLSAVDAY:                  /* Previous Daily File */
          *pulPosition = pCpnhd->usPreDayTtlOff;
          break;

     case CLASS_TTLCURPTD:                  /* Current PTD File */
		 if (pCpnhd->usCurPTDTtlOff == 0) {     /* Check Exist PTD Total File */
              return (TTL_NOTPTD);              /* Return Not PTD */
         }
          *pulPosition = pCpnhd->usCurPTDTtlOff;
          break;

     case CLASS_TTLSAVPTD:                   /* Previous PTD File */
		 if (pCpnhd->usPrePTDTtlOff == 0) {      /* Check Exist PTD Total File */
              return (TTL_NOTPTD);               /* Return Not PTD */
         }
          *pulPosition = pCpnhd->usPrePTDTtlOff;
          break;

     default:
          return (TTL_NOTCLASS);      /* Return Not Class */
     }
    return (TTL_SUCCESS);             /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasHeadRead(TTLCSCASHEAD *pCashd)
*
*    Input:     TTLCSCASHEAD *pCashd        - Pointer of Cashier File Header Read Buffer 
*
*   Output:     TTLCSCASHEAD *pCashd        - Cashier File Header Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Cashier Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Cashier Header.
*
*============================================================================
*/
SHORT TtlCasHeadRead(TTLCSCASHEAD *pCashd)
{
    return(TtlReadFile(hsTtlCasHandle, 0L, pCashd, sizeof(TTLCSCASHEAD)));  /* Read the Cashier Header Data */
}

/*
*============================================================================
**Synopsis:     SHORT TtlCasEmptyRead(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition        - Cashier Empty Table Position
*               VOID *pTmpBuff          - Pointer of Cashier Empty Table Data Read Buffer
*
*   Output:     VOID *pTmpBuff          - Cashier Empty Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Cashier Empty Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Cashier Empty Table.
*
*============================================================================
*/
SHORT TtlCasEmptyRead(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlReadFile(hsTtlCasHandle, ulPosition, pTmpBuff, TTL_CASEMP_SIZE));  /* Read the Cashier Empty Table */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasEmptyAssin(USHORT usMaxNo, USHORT usCasNo, 
*                                      USHORT usCashierNumber,
*                                      ULONG  ulPositionInd, ULONG ulPositionEmp,
*                                      ULONG  *pulPositionTtl, VOID *pTmpBuff)
*
*    Input:     USHORT usMaxNo               - Max Number
*               USHORT usCasNo               - Number of Cashier
*               USHORT usCashierNumber       - Cashier Number
*               ULONG  ulPositionInd         - Cashier Index Table Offset
*               ULONG  ulPositionEmp         - Cashier Empty Table Offset
*               ULONG  *pulPositionTtl       - Pointer of Cashier Total Offset 
*               VOID *pTmpBuff               - Pointer of Cashier Empty Table Data Read Buffer
*
*   Output:     ULONG  *pulPositionTtl       - Cashier Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Assin Cashier Empty Table 
*               TTL_NOTEMPTY        - Empty Table Full Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function updates the Cashier Empty Table.
*
*============================================================================
*/
SHORT TtlCasEmptyAssin(USHORT usMaxNo, USHORT usNumberOfCashiers, ULONG ulCashierNumber,
                        ULONG ulPositionInd, ULONG ulPositionEmp,
                        ULONG *pulPositionTtl, VOID *pTmpBuff)
{
    USHORT  usEmpOffset;             /* Empty Block Offset */
    ULONG   ulIndOffset;             /* Index Offset */
    SHORT   sRetvalue;
	TTLCSCASINDX    CasInd = {0};

	/*
	   Read Empty Table
	   Search Empty Table
       Update Empty Table
       Write Empty Table
	**/
    if ((sRetvalue = TtlCasEmptyRead(ulPositionEmp, pTmpBuff)) != 0) {
        return (sRetvalue);
    }
    if ((sRetvalue = TtlEmptyTableSearch(pTmpBuff, usMaxNo, &usEmpOffset)) != 0) { /* Search Empty Table */
        return (sRetvalue);
    }
    if ((sRetvalue = TtlEmptyTableUpdate(pTmpBuff, usMaxNo, usEmpOffset, TTL_EMPBIT_SET)) != 0) {
        return (sRetvalue);
    }
    if ((sRetvalue = TtlCasEmptyWrite(ulPositionEmp, pTmpBuff)) != 0) {
        return (sRetvalue);
    }

	/* --- Search Cashier Index Table, V3.3 --- */
    CasInd.ulCasNo   = ulCashierNumber;   /* Set Cashier Number */
    CasInd.usBlockNo = usEmpOffset;
    sRetvalue = TtlCasISearch(ulPositionInd, usNumberOfCashiers, &CasInd, pTmpBuff, &ulIndOffset, TTL_SEARCH_GT);

    if (sRetvalue == TTL_SUCCESS) { /* already exists */
        *pulPositionTtl = *pulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)usEmpOffset);
        return (TTL_SUCCESS);
    } else if (sRetvalue != TTL_NOTINFILE) {
        return (sRetvalue);
    }

    /* Update Index Table */
    if ((sRetvalue = TtlCasIAssin(ulPositionInd, usNumberOfCashiers, &CasInd, pTmpBuff, ulIndOffset)) != 0) {
        return (sRetvalue);
    }

    *pulPositionTtl = *pulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)usEmpOffset);

    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasAssin(UCHAR uchMinorClass, USHORT usCashierNumber,
*                                 ULONG *pulPositionTtl, VOID *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass          - Minor Class
*               USHORT usCashierNumber       - Cashier Number
*               ULONG  *pulPositionTtl       - Pointer of Cashier Total Offset
*               VOID *pTmpBuff               - Pointer of Cashier Empty Table Data Read Buffer
*
*   Output:     USHORT *pulPositionTtl       - Cashier Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Assin Cashier 
*               TTL_SIZEOVER        - Max Size Over
*               TTL_NOTEMPTY        - Empty Table Full Error
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assins the Cashier.
*
*============================================================================
*/
SHORT TtlCasAssin(UCHAR uchMinorClass, ULONG ulCashierNumber, ULONG *pulPositionTtl, VOID *pTmpBuff)
{
    TTLCSCASHEAD    *pCashd = pTmpBuff;    /* Pointer of Read Buffer for Cashier File Header */

    USHORT  usNumberCashiers;   /* Save Number of Cashier */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionEmp;      /* Save Seek Position */
    SHORT   sRetvalue;

    if (hsTtlCasHandle < 0) {                                     /* Check Cashier Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
    }

	/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {     /* Read Cashier File Header */
        return (sRetvalue);                              /* Return Error Status */
    }

	/* --- Set Read Total Offset --- */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY:                         /* Current Daily File */
        usNumberCashiers = pCashd->usCurDayCas;         /* Set Number of Current Daily Cashier */
        ulPositionInd = pCashd->ulCurDayIndexOff;       /* Set Current Daily Index Offset */
        ulPositionEmp = pCashd->ulCurDayEmptOff;        /* Set Current Daily Empty Offset */
        *pulPositionTtl = pCashd->ulCurDayTotalOff;     /* Set Current Daily Total Offset */
        break;

    case CLASS_TTLSAVDAY:                         /* Previous Daily File */
        usNumberCashiers = pCashd->usPreDayCas;        /* Set Number of Previous Daily Cashier */
        ulPositionInd = pCashd->ulPreDayIndexOff;      /* Set Previous Daily Index Offset */
        ulPositionEmp = pCashd->ulPreDayEmptOff;       /* Set Previous Daily Empty Offset */
        *pulPositionTtl = pCashd->ulPreDayTotalOff;    /* Set Previous Daily Total Offset */
        break;

    case CLASS_TTLCURPTD:                         /* Current PTD File, V3.3 */
        if (pCashd->ulCurPTDIndexOff == 0L) {          /* Check Exist PTD  Total File */
            return (TTL_NOTPTD);
        }
        usNumberCashiers = pCashd->usCurPTDCas;        /* Set Number of Current PTD Cashier */ 
        ulPositionInd = pCashd->ulCurPTDIndexOff;      /* Set Current PTD Index Offset */
        ulPositionEmp = pCashd->ulCurPTDEmptOff;       /* Set Previous PTD Empty Offset */
        *pulPositionTtl = pCashd->ulCurPTDTotalOff;    /* Set Current PTD Total Offset */
        break;

    case CLASS_TTLSAVPTD:                         /* Previous PTD File */
        if (pCashd->ulPrePTDIndexOff == 0L) {          /* Check Exist PTD  Total File */
            return (TTL_NOTPTD);
        }
        usNumberCashiers = pCashd->usPrePTDCas;        /* Set Number of Previous PTD Cashier */
        ulPositionInd = pCashd->ulPrePTDIndexOff;      /* Set Previous PTD Index Offset */
        ulPositionEmp = pCashd->ulPrePTDEmptOff;       /* Set Previous PTD Empty Offset */
        *pulPositionTtl = pCashd->ulPrePTDTotalOff;    /* Set Previous PTD Total Offset */ 
        break;

    default:
        return (TTL_NOTCLASS);
    }

    if (pCashd->usMaxCas < usNumberCashiers + 1) {
		// we have exceeded the maximum number of cashiers so it is an error.
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CASHIER_MAX_EXCEEDED);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), pCashd->usMaxCas);
        return (TTL_SIZEOVER);
    }
    
    /* Assign Previous New Cashier Total Area */
    if ((sRetvalue = TtlCasEmptyAssin(pCashd->usMaxCas, usNumberCashiers, ulCashierNumber, ulPositionInd, ulPositionEmp, pulPositionTtl, pTmpBuff)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }                             

    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {   /* Read Cashier File Header */
        return (sRetvalue);                            /* Rteurn Error Status */
    }

	/* --- Update Cashier Number --- */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        pCashd->usCurDayCas = pCashd->usCurDayCas + 1;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        pCashd->usPreDayCas = pCashd->usPreDayCas + 1;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File, V3.3 */
        pCashd->usCurPTDCas  = pCashd->usCurPTDCas + 1;
        break;

    case CLASS_TTLSAVPTD:           /* Previous PTD File, V3.3 */
        pCashd->usPrePTDCas = pCashd->usPrePTDCas + 1;
        break;

	/*  default:  Not Used */
    }

	/* --- Update Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadWrite(pCashd)) != 0) {     /* Write Cashier File Header */
        return (sRetvalue);                                  /* Rteurn Error Status */
    }
    return (TTL_SUCCESS);           /* Return Success */
}
/*                               
*============================================================================
**Synopsis:     SHORT TtlCasTotalRead(ULONG ulPosition, TTLCSCASTOTAL *pCasTotal)
*
*    Input:     ULONG ulPosition                - Cashier Total Table Position
*               TTLCSCASTOTAL *pCasTotal        - Pointer of Cashier Total Read Buffer
*
*   Output:     TTLCSCASTOTAL *pCasTotal        - Cashier Total
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Cashier Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Cashier Total.
*
*============================================================================
*/
SHORT TtlCasTotalRead(ULONG ulPosition, TTLCSCASTOTAL *pCasTotal)
{
    return(TtlReadFile(hsTtlCasHandle, ulPosition, pCasTotal, sizeof(TTLCSCASTOTAL)));    /* Read the Cashier Total Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlGetCasOffset(UCHAR uchMinorClass, 
*                                     USHORT usCashierNumber,
*                                     ULONG *pulPositionTtl
*                                     VOID *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               USHORT usCashierNumber  - Cashier Number 
*               ULONG *pulPositionTtl  - Pointer of Cashier Total Offset Set Buffer
*               VOID *pTmpBuff          - Pointer of Cashier Total Read Buffer
*
*   Output:     *pulPositionTtl          - Cashier Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed the Reading Cashier Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE   - Cashier Not Exist
*               TTL_NOTCLASS    - Set Illigal Class Code 
*
** Description  This function gets the Cashier Total Offset.
*
*============================================================================
*/
SHORT TtlGetCasOffset(UCHAR uchMinorClass, ULONG ulCashierNumber,   
                      ULONG *pulPositionTtl, VOID *pTmpBuff)
{
    TTLCSCASHEAD    *pCashd = pTmpBuff;    /* Pointer of Cashier File Header Read Buffer */

    USHORT  usCasNo;            /* Save Number of Cashier */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG	ulIndOffset;        /* Offset, V3.3 */
    SHORT   sRetvalue;
	TTLCSCASINDX    CasInd = {0};

    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
    }

	/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {    /* Read Cashier File Header */
        return (sRetvalue);         /* Return Error Status */
    }

	/* --- Set Read Total Offset --- */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY:                 /* Current Daily File */
        if (pCashd->usCurDayCas == 0) {   /* Check Number of Current Daily Cashier */
            return (TTL_NOTINFILE);       /* Return Not In File */
        }
        usCasNo = pCashd->usCurDayCas;
        ulPositionInd = pCashd->ulCurDayIndexOff;
        *pulPositionTtl = pCashd->ulCurDayTotalOff;
        break;

    case CLASS_TTLSAVDAY:                  /* Previous Daily File */
        if (pCashd->usPreDayCas == 0) {    /* Check Number of Previous Daily Cashier */
            return (TTL_NOTINFILE);        /* Return Not In File */
        }
        usCasNo = pCashd->usPreDayCas;
        ulPositionInd = pCashd->ulPreDayIndexOff;
        *pulPositionTtl = pCashd->ulPreDayTotalOff;
        break;

    case CLASS_TTLCURPTD:                       /* Current PTD File, V3.3 */
        if (pCashd->ulCurPTDIndexOff == 0) {    /* Check Exist PTD Total File */
            return (TTL_NOTPTD);
        }
        if (pCashd->usCurPTDCas == 0) {       /* Check Number of Current PTD Cashier */
            return (TTL_NOTINFILE);           /* Return Not In File */
        }
        usCasNo = pCashd->usCurPTDCas;
        ulPositionInd = pCashd->ulCurPTDIndexOff;
        *pulPositionTtl = pCashd->ulCurPTDTotalOff;
        break;

    case CLASS_TTLSAVPTD:                       /* Previous PTD File, V3.3 */
        if (pCashd->ulPrePTDIndexOff == 0) {    /* Check Exist PTD Total File */
            return (TTL_NOTPTD);
        }
        if (pCashd->usPrePTDCas == 0) {         /* Check Number of Previous PTD Cashier */
            return (TTL_NOTINFILE);             /* Return Not In File */
        }
        usCasNo = pCashd->usPrePTDCas;
        ulPositionInd = pCashd->ulPrePTDIndexOff;
        *pulPositionTtl = pCashd->ulPrePTDTotalOff;
        break;           

    default:
        return (TTL_NOTCLASS);
    }

	/* --- Search Cashier Index Table, V3.3 --- */
    CasInd.ulCasNo = ulCashierNumber;
    if ((sRetvalue = TtlCasISearch(ulPositionInd, usCasNo, &CasInd, pTmpBuff, &ulIndOffset, TTL_SEARCH_EQU)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

    *pulPositionTtl = *pulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)CasInd.usBlockNo);    /* Set Cashier Total File Offset */
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlreadCas(UCHAR uchMinorClass, USHORT usCashierNumber,
*                                VOID *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               USHORT usCashierNumber  - Cashier Number 
*               VOID *pTmpBuff          - Pointer of Cashier Total Read Buffer
*
*   Output:     TTLCSCASTOTAL *pTmpBuff - Cashier Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed Reading Cashier Total File 
*               TTL_NOTINFILE   - Cashier Not Exist
*               TTL_NOTCLASS    - Set Illigal Class Code 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Cashier Total File.
*
*============================================================================
*/
SHORT TtlreadCas(UCHAR uchMinorClass, ULONG ulCashierNumber, VOID *pTmpBuff)
{
    ULONG    ulPosition;     /* Save Seek Position */
    SHORT    sRetvalue;

    /* Get Total Offset */
    sRetvalue = TtlGetCasOffset(uchMinorClass, ulCashierNumber, &ulPosition, pTmpBuff);

	// zero out the buffer since it is used as a scratch buffer by TtlGetCasOffset()
	// Now people can depend on the values to be zero if record does not exist.
	memset (pTmpBuff, 0, sizeof(TTLCSCASTOTAL));

    if (sRetvalue == TTL_SUCCESS) {
		/* Read Cashier Total File */
		sRetvalue = TtlCasTotalRead(ulPosition, pTmpBuff);
    } else {
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_POINTER_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchMinorClass);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), (USHORT)ulCashierNumber);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
	}
    return (sRetvalue);         /* Return Error Status */
}

/*
*============================================================================
**Synopsis:     SHORT TtlCasISearch(ULONG ulPosition, USHORT usCasNo, 
*                                   TTLCSCASINDX *CasInd, VOID *pTmpBuff,
*                                   USHOT usSize, ULONG *pusOffset, 
*                                   UCHAR uchMethod)
*
*    Input:     ULONG ulPosition        - Search Index Table Position
*               USHORT usCasNo          - Number of Cashier
*               TTLCSCASINDX *CasInd    - Pointer of Target Cashier number
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHOT usSize            - Work Buffer Size
*               USHORT *pusOffset       - Pointer of Target Cashier Offset Buffer
*               UCHAR uchMethod         - Method
*
*   Output:     USHORT *Offset          - Target Cashier Offset Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Searching Cashier 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE       - Not IN FILE 
*
** Description  This function searchs the Cashier Index Table.
*
*============================================================================
*/
SHORT TtlCasISearch(ULONG ulPosition, USHORT usCasNo, TTLCSCASINDX *pCasInd,
                    VOID *pTmpBuff, ULONG *pulOffset, UCHAR uchMethod)
{
    USHORT  usNoCas;            /* Counter */
    USHORT  usUpNoCas;          /* Conuter */
    ULONG   ulReadlength;       /* Reading length of buffer */
    ULONG   ulPositionInd;      /* Pointer */
    ULONG   ulPositionPreInd;   /* Pointer */
    SHORT   sRetvalue;

	//JHHJ Updated PifReadFile 11-10-03

    usNoCas = usCasNo;
    *pulOffset = 0;
    ulPositionInd =  ulPosition;
    ulPositionPreInd = ulPosition;
    for ( ; usNoCas > TTL_CAS_TMPBUF_INDEXREAD; ) {
		ULONG   ulActualPosition,ulActualBytesRead;   /* Actual Seek Pointer */

        usUpNoCas = usNoCas / 2 + (usNoCas % 2);
        usNoCas /= 2;
        ulPositionInd += usNoCas * sizeof(TTLCSCASINDX); 
        /* Seek Pointer of Index Table */
        if (PifSeekFile(hsTtlCasHandle, ulPositionInd, &ulActualPosition) < 0) { 
            return (TTL_FILE_SEEK_ERR);
        }
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlCasHandle, pTmpBuff, sizeof(TTLCSCASINDX), &ulActualBytesRead);
        if (ulActualBytesRead != sizeof(TTLCSCASINDX)) {
            return (TTL_FILE_READ_ERR);
        }
        sRetvalue = TtlcmpCasgle(pCasInd, pTmpBuff);
        if (sRetvalue == 0) {
			ULONG	ulReadOffset;

            *pulOffset += usNoCas;
            /* read cashier index with empty block offset */
            ulReadOffset = *pulOffset;
            ulReadOffset *= sizeof(TTLCSCASINDX);
            if ((sRetvalue = TtlReadFile(hsTtlCasHandle, (ulPosition+ulReadOffset), pCasInd, sizeof(TTLCSCASINDX))) != 0) {
                return (TTL_FILE_READ_ERR);
            }
            return (TTL_SUCCESS);       /* Return SUCCESS */
        } else if (sRetvalue < 0) {
            ulPositionInd = ulPositionPreInd;
        } else {
            ulPositionPreInd = ulPositionInd;
            *pulOffset += usNoCas;
            usNoCas = usUpNoCas;
        }
    }

    if (usNoCas) {
		CHAR    *RetPointer;        /* Return Value */
		ULONG   ulActualPosition, ulActualBytesRead;   /* Actual Seek Pointer */

        /* Seek Pointer of Index Table */
        if (PifSeekFile(hsTtlCasHandle, ulPositionInd, &ulActualPosition) < 0) { 
            return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
        }
        ulReadlength = usNoCas * sizeof(TTLCSCASINDX);    /* Calculate Reading length */
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlCasHandle, pTmpBuff, ulReadlength, &ulActualBytesRead);
        if (ulActualBytesRead != ulReadlength) {
            return (TTL_FILE_READ_ERR);/* Return File Read Error */
        }
        sRetvalue = Rfl_SpBsearch(pCasInd, sizeof(TTLCSCASINDX), pTmpBuff, usNoCas, &RetPointer, TtlcmpCasgle);
        *pulOffset += (USHORT)((RetPointer - (CHAR *)pTmpBuff) / sizeof(TTLCSCASINDX));    /* ### Mod V1.0 */
        if (sRetvalue == RFL_HIT) {
			ULONG	ulReadOffset;

            /* read cashier index with empty block offset */
            ulReadOffset = *pulOffset;
            ulReadOffset *= sizeof(TTLCSCASINDX);
            if ((sRetvalue = TtlReadFile(hsTtlCasHandle, (ulPosition+ulReadOffset), pCasInd, sizeof(TTLCSCASINDX))) != 0) {
                return (TTL_FILE_READ_ERR);
            }
            return (TTL_SUCCESS);   /* Return SUCCESS */
        }
        return (TTL_NOTINFILE);     /* Return Not In File */
    }

    if (uchMethod == TTL_SEARCH_GT) {
        *pulOffset = usCasNo;
    }
    return (TTL_NOTINFILE);         /* Return Not In File */
}

/*
*============================================================================
**Synopsis:     VOID TtlAbort(USHORT usModule, USHORT ErrCode)
*
*    Input:     USHORT usModule         - Module ID
*               SHORT ErrCode          - Error Code
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Noyhing 
*
** Description  This function executes PifAbort.
*
*============================================================================
*/
#if defined(TtlAbort)
VOID   TtlAbort_Special(USHORT usModule, SHORT sErrCode);
VOID   TtlAbort_Debug(USHORT usModule, SHORT sErrCode, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "TtlAbort_Debug(): usModule = %d, sErrCode = %d, File %s, lineno = %d", usModule, sErrCode, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	TtlAbort_Special(usModule, sErrCode);
}

VOID   TtlAbort_Special(USHORT usModule, SHORT sErrCode)
#else
VOID TtlAbort(USHORT usModule, SHORT sErrCode)
#endif
{
    USHORT  usAbort;

    switch(sErrCode) {
    case TTL_FILE_SEEK_ERR:
        usAbort = FAULT_ERROR_FILE_SEEK;
        break;

    case TTL_FILE_READ_ERR:
        usAbort = FAULT_ERROR_FILE_READ;
        break;

    case TTL_FILE_OPEN_ERR:
        usAbort = FAULT_ERROR_FILE_OPEN;
        break;

    case TTL_AFFECTION_ERR:
        usAbort = FAULT_ERROR_AFFECTION;
        break;
    case TTL_BAD_LENGTH:
        usAbort = FAULT_ERROR_BAD_LENGTH;
        break;
    case TTL_FILE_HANDLE_ERR:
        usAbort = FAULT_ERROR_FILE_HANDLE;
        break;
    case TTL_BK_DENIED_NOHEAP:    /* background copy request denied (noheap) */
    case TTL_BK_DENIED_NOSTORAGE:    /* background copy request denied (nostorage) */
        usAbort = FAULT_SHORT_RESOURCE;
        break;
	case		TTL_UPD_WAI_ERROR:
		usAbort = FAULT_ERROR_TTLUPD_WAI_FILE;
		break;
	case		TTL_UPD_PLU_ERROR:
		usAbort = FAULT_ERROR_TTLUPD_PLU_FILE;
		break;
	case		TTL_UPD_DEP_ERROR:
		usAbort = FAULT_ERROR_TTLUPD_DEP_FILE;
		break;
	case		TTL_UPD_CAS_ERROR:
		usAbort = FAULT_ERROR_TTLUPD_CAS_FILE;
		break;
	case		TTL_UPD_CPN_ERROR:
		usAbort = FAULT_ERROR_TTLUPD_CPN_FILE;
		break;
    default:
        usAbort = FAULT_ERROR_FILE_WRITE;
        break;
    }
    PifAbort(usModule, usAbort);
}
/*
*============================================================================
**Synopsis:     VOID TtlLog(USHORT usModule, SHORT ErrCode, USHORT usNumber)
*
*    Input:     USHORT usModule         - Module ID
*               SHORT  sErrCode         - Error Code
*               USHORT usNumber         - Number
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Noyhing 
*
** Description  This function executes PifLog.
*
*============================================================================
*/
#if defined(TtlLog)
VOID   TtlLog_Special(USHORT usModule, SHORT sError, USHORT usNumber);
VOID   TtlLog_Debug(USHORT usModule, SHORT sError, USHORT usNumber, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "TtlLog_Debug(): usModule = %d, sError = %d, usNumber = %d, File %s, lineno = %d", usModule, sError, usNumber, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return TtlLog_Special(usModule, sError, usNumber);
}

VOID   TtlLog_Special(USHORT usModule, SHORT sError, USHORT usNumber)
#else
VOID TtlLog(USHORT usModule, SHORT sError, USHORT usNumber)
#endif
{
    USHORT  usLog;
    USHORT  usNumLog;

    if ((sError == 0) || (sError == TTL_NOTINFILE) || 
        (sError == TTL_NOT_ISSUED) || (sError == TTL_NOTZERO) ||
        (sError == TTL_NOTPTD)) {
        return;
    }

    if (sError == TTL_NOTCLASS) {
        PifLog(usModule, LOG_ERROR_TTL_FUNC_ILLIGAL);
        return;
    }

    switch(usModule) {
    case MODULE_TTL_TFL_DEPT:
    case MODULE_TTL_TFL_PLU:
    case MODULE_TTL_TFL_CPN:                    /* R3.0 */
    case MODULE_TTL_TFL_CASHIER:
    case MODULE_TTL_TFL_WAITER:
        switch(sError) {
        case TTL_SIZEOVER:
            usLog =LOG_ERROR_TTL_OVERMAX;
        case TTL_FILE_HANDLE_ERR:
            usLog = LOG_ERROR_TTL_FILE_HANDLE;
            break;
		case TTL_STATUS_DCUR:
            usLog = LOG_ERROR_TTL_SDCUR;
			break;
		case TTL_INSERT_DCUR:
            usLog = LOG_ERROR_TTL_IDCUR;
			break;
		case TTL_FIND_DCUR:
            usLog = LOG_ERROR_TTL_FDCUR;
			break;
		case TTL_UPDATE_DCUR:
            usLog = LOG_ERROR_TTL_UDCUR;
			break;
		case TTL_TSTAT_GDCUR:
            usLog = LOG_ERROR_TTL_TGDCUR;
			break;
		case TTL_TSTAT_SDCUR:
            usLog = LOG_ERROR_TTL_TSDCUR;
			break;
		case TTL_STATUS_PTD_CUR:
            usLog = LOG_ERROR_TTL_SPCUR;
			break;
		case TTL_INSERT_PTD_CUR:
            usLog = LOG_ERROR_TTL_IPCUR;
			break;
		case TTL_FIND_PTD_CUR:
            usLog = LOG_ERROR_TTL_FPCUR;
			break;
		case TTL_UPDATE_PTD_CUR:
            usLog = LOG_ERROR_TTL_UPCUR;
			break;
		case TTL_TSTAT_GPTD_CUR:
            usLog = LOG_ERROR_TTL_TGCUR;
			break;
		case TTL_TSTAT_SPTD_CUR:
            usLog = LOG_ERROR_TTL_TSPCUR;
			break;
        default:
            usLog = 0;
        }
        usNumLog = MODULE_TTL_TFL_NUM;
        break;
    case MODULE_TTL_TUM_DEPT:
    case MODULE_TTL_TUM_PLU:
    case MODULE_TTL_TUM_CPN:                    /* R3.0 */
    case MODULE_TTL_TUM_CASHIER:
    case MODULE_TTL_TUM_WAITER:
        switch(sError) {
        case TTL_SIZEOVER:
            usLog =LOG_ERROR_TTL_OVERMAX;
            break;
        case TTL_FILE_HANDLE_ERR:
            usLog = LOG_ERROR_TTL_FILE_HANDLE;
            break;
        case TTL_NOTEMPTY:
            usLog = LOG_ERROR_TTL_NOTEMPTY;
            break;
        default:
            usLog = 0;
        }
        usNumLog = MODULE_TTL_TUM_NUM;
        break;
    default:
        return;
    }
    PifLog(usModule, usLog);
    usLog = LOG_ERROR_TTL_DEPT + usNumber;
    PifLog(usNumLog, usLog);
}


/****************** New Functions (Release 3.1) BEGIN ***********************/
/*
*============================================================================
**Synopsis:     SHORT TtlIndFinHeadRead(TTLCSINDHEAD *pIndHead)
*
*    Input:     TTLCSINDHEAD *pIndHead  - Pointer of Individual File Header
*
*   Output:     TTLCSINDHEAD *pIndHead  - Individual File Header
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading File Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Individual Financial File Header.
*
*============================================================================
*/
SHORT TtlIndFinHeadRead(TTLCSINDHEAD *pIndHead)
{
    SHORT   sReturn = TtlReadFile( hsTtlIndFinHandle, 0L, pIndHead, sizeof(TTLCSINDHEAD));
    return (sReturn);
}

/*                               
*============================================================================
**Synopsis:     SHORT TtlIndFinTotalRead(ULONG ulPosition, TTLCSREGFIN *pTotal)
*
*    Input:     ULONG       ulPosition  - Individual Financial Table Position
*               TTLCSREGFIN *pTotal     - Pointer of Individual Financial
*
*   Output:     TTLCSREGFIN *pTotal     - Pointer of Individual Financial
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Individual Financial Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Inidividual Financial Total.
*
*============================================================================
*/
SHORT TtlIndFinTotalRead(ULONG ulPosition, TTLCSREGFIN *pTotal)
{
    SHORT   sReturn = TtlReadFile( hsTtlIndFinHandle, ulPosition, pTotal, sizeof(TTLCSREGFIN));  /* Read the Individual Financial */
    return (sReturn);
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinIndexRead(ULONG ulPosition, USHORT usTermNum,
*                                        VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition        - Individual Financial Index Position
*               USHORT usTermNum        - Number of Terminals
*               VOID *pTmpBuff          - Pointer of Terminal Index Table Read Buffer
*
*   Output:     VOID *pTmpBuff          - Terminal Index Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed the Reading Terminal Index Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Individual Financial Index Table.
*
*============================================================================
*/
SHORT TtlIndFinIndexRead(ULONG ulPosition, USHORT usTermNum, VOID *pTmpBuff)
{
    ULONG  ulReadlength = usTermNum * sizeof(TTLCSINDINDX);    /* Calculate Reading length */
    return(TtlReadFile(hsTtlIndFinHandle, ulPosition, pTmpBuff, ulReadlength));    /* Read the Terminal Index Table */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinISearch(VOID *pTmpBuff, USHORT usTermNo, 
*                                      USHORT usNumber, UCHAR *puchOffset,
*                                      UCHAR *puchNumber)
*
*    Input:     VOID    *pTmpBuff   - Index Table Buffer Pointer
*               USHORT  usTermNo    - Current Terminal Number
*               USHORT  usNumber    - Target Terminal Number
*               UCHAR   *puchOffset - Pointer of Target Total Offset Data
*               UCHAR   *puchNumber - Pointer of Target Terminal
*
*   Output:     UCHAR *Offset       - Target Total Offset Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed Searching Terminal
*               TTL_NOTINFILE   - Not In File Terminal
*
** Description  This function searchs the Individual Financial Index.
*
*============================================================================
*/
SHORT TtlIndFinISearch(VOID *pTmpBuff, USHORT usTermNo,
                       USHORT usNumber,  UCHAR *puchOffset, UCHAR *puchNumber)
{
	TTLCSINDINDX    IndIdx = {0}; /* Read Buffer for Terminal Index Table */
    CHAR    *RetPointer;               /* Return Value */

    IndIdx.usTerminalNo = usNumber;
    RetPointer = bsearch(&IndIdx, pTmpBuff, usTermNo, sizeof(TTLCSINDINDX), TtlcmpIndFingle);
    if (RetPointer) {
		TTLCSINDINDX    *pInd = (TTLCSINDINDX *)RetPointer;

        *puchOffset = pInd->uchBlockNo;  /* Set Total Offset */
        *puchNumber = (UCHAR)((RetPointer - (CHAR *)pTmpBuff) / sizeof(TTLCSINDINDX)); /* ### Mod V1.0 */
        return (TTL_SUCCESS);   /* Return Success */
    } else {
        return (TTL_NOTINFILE);
    }
}


/*
*============================================================================
**Synopsis:     SHORT TtlreadIndFin(UCHAR uchMinorClass, USHORT usTerminalNumber,
*                                   VOID *pTmpBuff)
*
*    Input:     UCHAR  uchMinorClass    - Minor Class
*               USHORT usTerminalNumber - Terminal Number 
*               VOID   *pTmpBuff        - Pointer of Individual Financial Buffer
*
*   Output:     VOID   *pTmpBuff        - Individual Financial Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed Reading Individual Financial
*               TTL_NOTINFILE   - Terminal Not Exist
*               TTL_NOTCLASS    - Set Illigal Class Code 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Individual Financial File
*
*============================================================================
*/
SHORT TtlreadIndFin(UCHAR uchMinorClass, USHORT usTerminalNumber, VOID *pTmpBuff)
{
    ULONG   ulPositionTtl;     /* Save Seek Position */
    SHORT   sRetvalue;

    sRetvalue = TtlGetIndFinOffset(uchMinorClass, usTerminalNumber, &ulPositionTtl, pTmpBuff);

	// zero out the buffer since it is used as a scratch buffer by TtlGetIndFinOffset()
	// Now people can depend on the values to be zero if record does not exist.
	memset(pTmpBuff, 0, sizeof(TTLCSREGFIN));
    if (sRetvalue == TTL_SUCCESS) {  /* Get Total Offset */
        /* Read Cashier Total File */
		sRetvalue =  (TtlIndFinTotalRead(ulPositionTtl, (TTLCSREGFIN *)pTmpBuff));
    } else {
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_POINTER_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchMinorClass);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), usTerminalNumber);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
	}

    return (sRetvalue);         /* Return Error Status */
}

/*
*============================================================================
**Synopsis:     SHORT TtlGetIndFinOffset(UCHAR  uchMinorClass, 
*                                        USHORT usTerminalNumber,
*                                        USHORT *pusPositionTtl
*                                        VOID   *pTmpBuff)
*
*    Input:     UCHAR  uchMinorClass    - Minor Class
*               USHORT usTerminalNumber - Terminal Number
*               USHORT *pusPositionTtl  - Pointer of Individual Financial Offset
*               VOID   *pTmpBuff        - Pointer of Individual Financial Total
*
*   Output:     *pusPositionTtl         - Individual Financial Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed the Reading Individual Financial
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE   - Terminal Not Exist
*               TTL_NOTCLASS    - Set Illigal Class Code 
*
** Description  This function gets the Individual Financial Total Offset.
*
*============================================================================
*/
SHORT TtlGetIndFinOffset(UCHAR uchMinorClass, USHORT usTerminalNumber,   
                         ULONG *pulPositionTtl, TTLCS_TMPBUF_WORKING pTmpBuff)
{
    TTLCSINDHEAD    *pIndHead = (VOID *)pTmpBuff;  /* Pointer of Individual Financial Header */

    USHORT  usTermNo;           /* Save Number of Terminal */
    USHORT  usPositionInd;      /* Save Seek Position */
    UCHAR   uchOffset;          /* Offset */
    UCHAR   uchNumber;
    SHORT   sRetvalue;

    if (hsTtlIndFinHandle < 0) {    /* Check Individual Financial Handle */
		NHPOS_ASSERT(hsTtlIndFinHandle >= 0);
        return (TTL_FILE_HANDLE_ERR);
    }

    /* --- Read Individual Financial File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Set Read Total Offset --- */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        if (pIndHead->usCurDayTerminal == 0) {    /* Check Number of Current Daily Terminal */
            return (TTL_NOTINFILE);               /* Return Not In File */
        }
        usTermNo        = pIndHead->usCurDayTerminal;
        usPositionInd   = pIndHead->usCurDayIndexOff;
        *pulPositionTtl = pIndHead->usCurDayTotalOff;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        if (pIndHead->usPreDayTerminal == 0) {    /* Check Number of Previous Daily Terminal */
            return (TTL_NOTINFILE);               /* Return Not In File */
        }
        usTermNo        = pIndHead->usPreDayTerminal;
        usPositionInd   = pIndHead->usPreDayIndexOff;
        *pulPositionTtl = pIndHead->usPreDayTotalOff;
        break;

    default:
        return (TTL_NOTCLASS);
    }

    /* --- Search Individual Financial Index Table --- */
    if ((sRetvalue = TtlIndFinIndexRead((ULONG)usPositionInd, usTermNo, pTmpBuff)) != 0) {
                                    /* Read Individual Financial Index Table */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_READ_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), usTermNo);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (sRetvalue);         /* Return Error Status */
    }

    if ((sRetvalue = TtlIndFinISearch(pTmpBuff, usTermNo, usTerminalNumber, &uchOffset, &uchNumber)) != 0) {
                                    /* Search Individual Financial Index Table */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_READ_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), usTermNo);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (sRetvalue);         /* Return Error Status */
    }

    *pulPositionTtl = *pulPositionTtl + (TTL_INDTTL_SIZE * (ULONG)uchOffset);    /* Set Individual Financial Total File Offset */
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinAssin(UCHAR uchMinorClass, USHORT usTerminalNumber,
*                                    USHORT *pusPositionTtl, VOID *pTmpBuff)
*
*    Input:     UCHAR  uchMinorClass    - Minor Class
*               USHORT usTerminalNumber - Terminal Number
*               USHORT *pusPositionTtl  - Pointer of Terminal Total Offset
*               VOID   *pTmpBuff        - Pointer of Terminal Empty Table Data Read Buffer
*                                       
*   Output:     USHORT *pusPositionTtl  - Terminal Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Assign Individual Financial
*               TTL_SIZEOVER        - Max Size Over
*               TTL_NOTEMPTY        - Empty Table Full Error
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assigns the individual financial
*
*============================================================================
*/
SHORT TtlIndFinAssin(UCHAR uchMinorClass, USHORT usTerminalNumber,
                     ULONG *pulPositionTtl, TTLCS_TMPBUF_WORKING pTmpBuff)
{

    TTLCSINDHEAD    *pIndHead = (VOID *)pTmpBuff;  /* Pointer of Read Buffer for Individual Financial */

    USHORT  usTermNo;           /* Save Number of Terminal */
    USHORT  usPositionInd;      /* Save Seek Position */
    USHORT  usPositionEmp;      /* Save Seek Position */
    SHORT   sRetvalue;

    if (hsTtlIndFinHandle < 0) {    /* Check Individual Financial Total File Handle */
		NHPOS_ASSERT(hsTtlIndFinHandle >= 0);
        return (TTL_FILE_HANDLE_ERR);
    }

    /* --- Read File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {    /* Read File Header */
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Set Read Total Offset --- */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        usTermNo        = pIndHead->usCurDayTerminal;    /* Set Number of Current Daily Terminal */
        usPositionInd   = pIndHead->usCurDayIndexOff;    /* Set Current Daily Index Offset */
        usPositionEmp   = pIndHead->usCurDayEmptOff;     /* Set Current Daily Empty Offset */
        *pulPositionTtl = pIndHead->usCurDayTotalOff;    /* Set Current Daily Total Offset */
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        usTermNo        = pIndHead->usPreDayTerminal;    /* Set Number of Previous Daily Terminal */
        usPositionInd   = pIndHead->usPreDayIndexOff;    /* Set Previous Daily Index Offset */
        usPositionEmp   = pIndHead->usPreDayEmptOff;     /* Set Previous Daily Empty Offset */
        *pulPositionTtl = pIndHead->usPreDayTotalOff;    /* Set Previous Daily Total Offset */
        break;

    default:
        return (TTL_NOTCLASS);
    }
    if (pIndHead->usMaxTerminal < usTermNo + 1) {
        return (TTL_SIZEOVER);
    }
    
    if ((sRetvalue = TtlIndFinEmptyAssin(pIndHead->usMaxTerminal,
                                         usTermNo,
                                         usTerminalNumber,
                                         usPositionInd,
                                         usPositionEmp,
                                         pulPositionTtl,
                                         pTmpBuff)) != 0) {
                                    /* Assin Previous New Terminal Total Area */
        return (sRetvalue);         /* Return Error Status */
    }                             

    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {    /* Read File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    
    /* --- Update Temrinal Number --- */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        pIndHead->usCurDayTerminal = pIndHead->usCurDayTerminal + 1;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        pIndHead->usPreDayTerminal = pIndHead->usPreDayTerminal + 1;
        break;

/*  default:  Not Used */
    }

    /* --- Update File Header --- */
    if ((sRetvalue = TtlIndFinHeadWrite(pIndHead)) != 0) {    /* Write File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinEmptyAssin(USHORT usMaxNo, USHORT usTermNo,
*                                         USHORT usTerminalNumber,
*                                         USHORT usPositionInd, USHORT usPositionEmp,
*                                         USHORT *pusPositionTtl, VOID *pTmpBuff)
*
*    Input:     USHORT usMaxNo          - Max Number
*               USHORT usTermNo         - Number of Terminal
*               USHORT usTerminalNumber - Temrinal Number
*               USHORT usPositionInd    - Terminal Index Table Offset
*               USHORT usPositionEmp    - Terminal Empty Table Offset
*               USHORT *pusPositionTtl  - Pointer of Terminal Total Offset 
*               VOID *pTmpBuff          - Pointer of Terminal Empty Table Data Read Buffer
*
*   Output:     USHORT *pusPositionTtl  - Terminal Total Offset
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Assign Terminal Empty Table 
*               TTL_NOTEMPTY        - Empty Table Full Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function updates the Individual Financial Empty Table.
*
*============================================================================
*/
SHORT TtlIndFinEmptyAssin(USHORT usMaxNo, USHORT usTermNo, USHORT usTerminalNumber,
                          USHORT usPositionInd, USHORT usPositionEmp,
                          ULONG  *pulPositionTtl, VOID *pTmpBuff)
{
    USHORT  usEmpOffset;             /* Empty Block Offset */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlIndFinEmptyRead((ULONG)usPositionEmp, pTmpBuff)) != 0) {    /* Read Empty Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlEmptyTableSearch(pTmpBuff, usMaxNo, &usEmpOffset)) != 0) {
                                    /* Search Empty Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlEmptyTableUpdate(pTmpBuff, usMaxNo, usEmpOffset, TTL_EMPBIT_SET)) != 0) {
                                    /* Update Empty Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlIndFinEmptyWrite((ULONG)usPositionEmp, pTmpBuff)) != 0) {  /* WriteEmpty Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlIndFinIndexRead((ULONG)usPositionInd, usTermNo, pTmpBuff)) != 0) {
                                    /* Read Terminal Index Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlIndFinIAssin(pTmpBuff, usMaxNo, usTermNo, usTerminalNumber, (UCHAR)usEmpOffset)) != 0) {  
                                    /* Update Terminal Index Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlIndFinIndexWrite((ULONG)usPositionInd, (USHORT)(usTermNo + 1), pTmpBuff)) != 0) { /* ### Mod (2171 for Win32) V1.0 */
                                    /* Read Terminal Index Table */
        return (sRetvalue);         /* Return Error Status */
    }

    *pulPositionTtl = *pulPositionTtl + (TTL_INDTTL_SIZE * usEmpOffset);
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinEmptyRead(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition        - Terminal Empty Table Position
*               VOID *pTmpBuff          - Pointer of Terminal Empty Table 
*
*   Output:     VOID *pTmpBuff          - Terminal Empty Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Terminal Empty Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Termianl Empty Table.
*
*============================================================================
*/
SHORT TtlIndFinEmptyRead(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlReadFile(hsTtlIndFinHandle, ulPosition, pTmpBuff, TTL_INDEMP_SIZE));    /* Read the Terminal Empty Table */
}

/*
*============================================================================
**Synopsis:     SHORT TtlreadSerTime(UCHAR uchMinorClass, UCHAR uchHourOffset,
*                                    UCHAR uchBorder, TOTAL *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               UCHAR uchOffset         - Hourly Block Offset
*               UCHAR uchBorder         - Service Time Border    
*               TOTAL *pTmpBuff         - Pointer of Service Time Data
*
*
*    Input:     UCHAR   uchMinorClass   - Minor Class
*               TOTAL   *pTmpBuff       - Pointer of Service Time Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Reading Service Time Total
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS - Set Illigal Class Code 
*
** Description  This function reads the Service Time Total.
*
*============================================================================
*/
SHORT TtlreadSerTime(UCHAR uchMinorClass, UCHAR uchHourOffset, UCHAR uchBorder,
                        TOTAL *pTmpBuff)
{
    ULONG   ulPosition = 0;         /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetSerTimeFilePosition(uchMinorClass, &ulPosition)) != 0) {    /* Get Service Time Data Position */
        return (sRetvalue);
    }

    /* --- Read Service Time Total --- */
    ulPosition += TTL_STAT_SIZE;
    ulPosition += (sizeof(TOTAL) * 3 * uchHourOffset);
    ulPosition += (sizeof(TOTAL) * uchBorder);

    sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, pTmpBuff, sizeof(TOTAL));

    return (sRetvalue);         /* Return Read Status */
}

/*
*============================================================================
**Synopsis:     SHORT TtlGetSerTimeFilePosition(UCHAR uchClass, ULONG *pulPosition)
*
*    Input:     UCHAR uchClass          - Minor Class
*               ULONG *pulPosition      - File Position
*
*   Output:     ULONG *pulPosition      - File Position Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function gets the Target Service Time Data Position.
*
*============================================================================
*/
SHORT TtlGetSerTimeFilePosition(UCHAR uchClass, ULONG *pulPosition)
{
    if (hsTtlSerTimeHandle < 0) {   /* Check Total File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

    /* --- Set Read Total Offset --- */
    switch(uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        *pulPosition = TTL_SERTIME_DAYCURR;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        *pulPosition = TTL_SERTIME_DAYCURR + TTL_SERTTL_SIZE;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
        *pulPosition = TTL_SERTIME_PTDCURR;
        break;

    case CLASS_TTLSAVPTD:           /* Previous PTD File */
        *pulPosition = TTL_SERTIME_PTDCURR + TTL_SERTTL_SIZE;
        break;

    default:
        return (TTL_NOTCLASS);      /* Return Not Class */
    }
    return (TTL_SUCCESS);           /* Return Success */
}

/****************** New Functions (Release 3.1) END ***********************/

/* --- End of Source File --- */
