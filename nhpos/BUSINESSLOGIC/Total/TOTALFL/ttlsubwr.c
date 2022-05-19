/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Subroutine for File Write                         
* Category    : TOTAL, NCR 2170 US Hospitality Application         
* Program Name: Ttlsubwr.c                                                      
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlWriteFile()    - Write File
*           VOID  TtlNullWrite()    - Write Null Data
*           SHORT TtlwriteFin()     - Write Financial Total
*           SHORT TtlwriteHour()    - Write Hourly Total
*           SHORT TtlDeptHeadWrite() - Write Dept File Header  
*           SHORT TtlDeptAllWrite() - Write Dept All of Total 
*           SHORT TtlDeptTotalWrite() - Write Dept Total
*           SHORT TtlCpnHeadWrite() - Write Coupon File Header       R3.0
*           SHORT TtlCpnTotalWrite() - Write Coupon Total            R3.0
*           SHORT TtlCasHeadWrite() - Write Cashier File Header
*           SHORT TtlCasIndexWrite() - Write Cashier Index Table
*           SHORT TtlCasEmptyWrite() - Write Cashier Empty Table
*           SHORT TtlCasTotalWrite() - Write Cashier Total
*           SHORT TtlEmptyTableUpdate() - Update PLU,Cashier,Waiter Empty Table
*           SHORT TtlIndFinHeadWrite()  - Write Individual Financial Header, R3.1
*           SHORT TtlIndFinIndexWrite() - Write Individual Financial Index,  R3.1
*           SHORT TtlIndFinEmptyWrite() - Write Individual Financial Emply Table, R3.1
*           SHORT TtlIndFinTotalWrite() - Write Individual Financial Total R3.1
*           SHORT TtlIndFinIAssin()     - Assign Terminal Index Table,  R3.1
*           SHORT TtlIndFinIErase()     - Erase Terminal Index Table,   R3.1
*           int   TtlcmpIndFing()       - Compare Terminal Number,      R3.1
*           SHORT TtlwriteSerTime()     - Write Service Time Total,     R3.1
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name     : Description
* May-06-92 : 00.00.01 : K.Terai    :                                    
* Mar-03-95 : 03.00.00 : hkato      : R3.0
* Nov-13-95 ; 03.01.00 : T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Aug-11-99 : 03.05.00 : K.Iwata    : R3.5 Remov WAITER_MODEL
* Aug-13-99 : 03.05.00 : K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : removed __DEL_2172 code for PLU totals.
* Aug-29-17 : 02.02.02 : R.Chambers : moved TtlDateWrite() from here.
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
#include <tchar.h>
#include <memory.h>
#include <math.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
#include <rfl.h>
/* #include <search.h> */
#include <csttl.h>
#include <ttl.h>
#include <appllog.h>

static SHORT TtlcmpIndFing(VOID  *p1, VOID *p2); /* ### Mod (2171 for Win32) V1.0 */

/*
*============================================================================
**Synopsis:     SHORT TtlWriteFile(USHORT hsHandle, ULONG ulPosition,
                                   VOID *pTmpBuff, USHORT usSize)
*
*    Input:     USHORT hsHandle         - File Handle
*               ULONG ulPosition        - Write File Position
*               VOID *pTmpBuff          - Pointer of Write File Data Buffer
*               USHORT usSize           - Write Buffer Size
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Writing Traget  
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Traget.
*
*============================================================================
*/
#if defined(TtlWriteFile)
SHORT   TtlWriteFile_Special(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize);
SHORT   TtlWriteFile_Debug(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize, char *aszFilePath, int nLineNo)
{
	int    iLen = 0;
	char   xBuffer[256];
	SHORT  sResult;

	sResult = TtlWriteFile_Special(hsHandle, ulPosition, pTmpBuff, ulSize);

	if (sResult != TTL_SUCCESS) {
		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf (xBuffer, "TtlWriteFile_Debug(): sResult = %d, File %s, lineno = %d", sResult, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sResult;
}

SHORT   TtlWriteFile_Special(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize)
#else
SHORT TtlWriteFile(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize)
#endif
{
    ULONG   ulActualPosition;   /* Actual Seek Pointer */
 
    if (hsHandle < 0) {                /* Check for valid File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

	if (PifSeekFile(hsHandle, ulPosition, &ulActualPosition) < 0) {  /* Seek to file Write position */
        return (TTL_FILE_SEEK_ERR);

    }
    PifWriteFile(hsHandle, pTmpBuff, ulSize); 
    return (TTL_SUCCESS);
}
/*
*============================================================================
**Synopsis:     VOID  TtlNullWrite(SHORT hsHandle, ULONG ulLength 
*                                  VOID *pTmpBuff, USHORT usSize)
*
*    Input:     SHORT hsHandle          - File Handle
*               ULONG ulLength          - Null Write Length
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHORT usSize           - Work Buffer Size
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing 
*
** Description  This function writes the Null data to Total File.
*
*============================================================================
*/
VOID TtlNullWrite(SHORT hsHandle, ULONG ulLength, VOID *pTmpBuff,
                  USHORT usSize)
{
    ULONG   ulNullwTmp;         /* Number of Temporary buffer Null data Counter */
    ULONG   ulNullwrest;        /* Rest Number of Temporary Null data Counter */
    USHORT  i;

    memset(pTmpBuff, 0, usSize); /* Clear Work */
    ulNullwTmp = ulLength / (ULONG)usSize;
    ulNullwrest = ulLength % (ULONG)usSize;
    for (i = 0; i < ulNullwTmp; i++) {
        PifWriteFile(hsHandle, pTmpBuff, usSize);  /* Write Null Data(Temporary Buffer Size) */
    }
    if (ulNullwrest) {
        PifWriteFile(hsHandle, pTmpBuff, (USHORT)ulNullwrest);  /* Write Null Data(Rest) */
    }
}
                                           
/*
*============================================================================
**Synopsis:     TtlwriteFin(UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               TTLCSREGFIN *pTmpBuff   - Pointer of Financial Read Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writting Financial Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function writes the Financial Total File.
*
*============================================================================
*/
SHORT TtlwriteFin(UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    sRetvalue = TtlGetFinFilePosition(uchMinorClass, &ulPosition);  /* Get Financial Data Position */
    if (sRetvalue != 0) {
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_POINTER_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchMinorClass);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (sRetvalue);
    }

	/* --- Write Financial Total --- */
    return (TtlWriteFile(hsTtlBaseHandle, ulPosition, pTmpBuff, TTL_FIN_SIZE));
}
/*
*============================================================================
**Synopsis:     SHORT TtlwriteHour(UCHAR uchMinorClass, UCHAR uchOffset,
*                                  HOURLY *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               UCHAR uchOffset         - Hourly Block Offset
*               HOULY *pTmpBuff         - Pointer of Hourly Read Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writeing Hourly Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function writes the Hourly Total File.
*
*============================================================================
*/
SHORT TtlwriteHour(UCHAR uchMinorClass, UCHAR uchOffset, HOURLY *pTmpBuff)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    sRetvalue = TtlGetHourFilePosition(uchMinorClass, &ulPosition);    /* Get Financial Data Position */
    if (sRetvalue  != 0) {
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_POINTER_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchMinorClass);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM), uchOffset);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (sRetvalue);
    }

	/* --- Write Hourly Total to Hourly Total File --- */
    ulPosition = ulPosition + TTL_STAT_SIZE + (sizeof(HOURLY) * uchOffset);
    return (TtlWriteFile(hsTtlBaseHandle, ulPosition, pTmpBuff, sizeof(HOURLY)));
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptHeadWrite(TTLCSDEPTHEAD *pDepthd)
*
*    Input:     TTLCSDEPTHEAD *pDepthd    - Pointer of Dept File Header Write Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writing Dept Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Dept Header.  
*
*============================================================================
*/
SHORT TtlDeptHeadWrite(TTLCSDEPTHEAD *pDepthd)
{
    return(TtlWriteFile(hsTtlDeptHandle, 0L, pDepthd, sizeof(TTLCSDEPTHEAD)));
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptMiscWrite(ULONG ulPosition, TTLCSDEPTMISC *DeptMisc)
*
*    Input:     ULONG ulPosition        - Dept Misc Total Position
*               TTLCSDEPTMISC *DeptMisc   - Pointer of Dept Misc Total Write Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writing Dept Misc Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Dept Block Table.
*
*============================================================================
*/
SHORT TtlDeptMiscWrite(ULONG ulPosition, TTLCSDEPTMISC *pDeptMisc)
{
    return(TtlWriteFile(hsTtlDeptHandle, ulPosition, pDeptMisc, sizeof(TTLCSDEPTMISC)));
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptGTotalWrite(ULONG ulPosition, VOID *plTotal)
*
*    Input:     ULONG ulPosition        - Dept All Total Position
*               VOID *plTotal           - Pointer of Dept All Total Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writting Dept Gross(All) Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Dept Gross(All) Total.
*
*============================================================================
*/
SHORT TtlDeptGTotalWrite(ULONG ulPosition, LTOTAL *plTotal)
{
    ULONG   ulPositionAllTtl;          /* Seek Position */

    ulPositionAllTtl = ulPosition + (ULONG)TTL_STAT_SIZE;  /* Calculate Target Total Data Offset */
    return(TtlWriteFile(hsTtlDeptHandle, ulPositionAllTtl, plTotal, sizeof(LTOTAL)));
                                    /* Write the Dept Gross(All) Total Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptEmptyWrite(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition        - Dept Empty Table Position
*               VOID *pTmpBuff          - Pointer of Dept Empty Table Write Data Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Writing Dept Empty Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Dept Empty Table.
*
*============================================================================
*/
SHORT TtlDeptEmptyWrite(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlWriteFile(hsTtlDeptHandle, ulPosition, pTmpBuff, TTL_DEPTEMP_SIZE));
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptTotalWrite(ULONG ulPosition, USHORT usDeptBlk,
*                                      TOTAL *pTotal)
*
*    Input:     ULONG ulPosition         - Dept TOTAL Table Position
*               USHORT usDeptBlk          - Target Dept Total Block Data
*               TOTAL *pTotal            - Pointer of Dept Total Data Buffer 
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writting Dept Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Dept Total.
*
*============================================================================
*/
SHORT TtlDeptTotalWrite(ULONG ulPosition, USHORT usDeptBlk, TOTAL *pTotal)
{
    ULONG   ulPositionTtl;          /* Seek Position */

    ulPositionTtl = ulPosition + ((ULONG)usDeptBlk * sizeof(TOTAL));
                                    /* Calculate Taget Total Data Offset */
    return(TtlWriteFile(hsTtlDeptHandle, ulPositionTtl, pTotal, sizeof(TOTAL)));
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptIAssin(ULONG ulPosition, USHORT usDeptNo, 
*                                  TTLCSDEPTINDX *pDeptInd, VOID *pTmpBuff,
*                                  ULONG ulOffset)
*
*    Input:     ULONG ulPosition        - Search Index Table Position
*               USHORT usDeptNo          - Number of Dept
*               TTLCSDEPTINDX *DeptInd    - Pointer of Target Dept number
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHORT usOffset         - Dept Offset
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Assin Dept Index 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assins the Dept Index Table.
*
*============================================================================
*/
SHORT TtlDeptIAssin(ULONG ulPosition, USHORT usDeptNo, TTLCSDEPTINDX *pDeptInd, 
                   VOID *pTmpBuff, ULONG ulOffset)
{
    TTLCSDEPTINDX    *pDeptMove = pTmpBuff;
    TTLCSDEPTINDX    DeptIndex;
    ULONG   ulPositionIndStart;
    ULONG   ulActualPosition;
//    ULONG   ulLength;        
    USHORT	usMoveDeptNo;
    SHORT   sRetvalue;
	ULONG	ulActualBytesRead;

    DeptIndex = *pDeptInd;
    usMoveDeptNo = usDeptNo - (USHORT)ulOffset; /* Set Move Number of Dept */
    ulPositionIndStart = ulPosition + (ulOffset * sizeof(TTLCSDEPTINDX));
                                    /* Set Moving Start Position */ 
    for (;usMoveDeptNo >= TTL_DEPT_TMPBUF_INDEXREAD;) {
        if (PifSeekFile(hsTtlDeptHandle, ulPositionIndStart, &ulActualPosition) < 0) { 
                                    /* Seek Pointer of Dept Index Table */
            return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
        }
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlDeptHandle, (pDeptMove+1), (sizeof(TTLCSDEPTINDX) * (TTL_DEPT_TMPBUF_INDEXREAD - 1)), &ulActualBytesRead);
        if (ulActualBytesRead != (sizeof(TTLCSDEPTINDX) * (TTL_DEPT_TMPBUF_INDEXREAD - 1))) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
        *pDeptMove = DeptIndex;         /* Set Insertion Dept Number */
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlDeptHandle, &DeptIndex, sizeof(TTLCSDEPTINDX), &ulActualBytesRead);
                                    /* Save Overwrited Dept Number */
        if (ulActualBytesRead != sizeof(TTLCSDEPTINDX)) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
        if ((sRetvalue = TtlWriteFile(hsTtlDeptHandle, ulPositionIndStart, pDeptMove, sizeof(TTLCSDEPTINDX) * (TTL_DEPT_TMPBUF_INDEXREAD))) != 0) {
            return (sRetvalue);     /* Return Error Status */
        }
                                    /* Write Index Table */
        ulPositionIndStart += sizeof(TTLCSDEPTINDX) * (TTL_DEPT_TMPBUF_INDEXREAD); /* Update Start Position */
        usMoveDeptNo -= TTL_DEPT_TMPBUF_INDEXREAD;                                 /* Update Move Number of Dept */
    }
    if (PifSeekFile(hsTtlDeptHandle, ulPositionIndStart, &ulActualPosition) < 0) { 
                                    /* Seek Pointer of Dept Index Table */
        return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
    }
    if (usMoveDeptNo) {
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile((USHORT)hsTtlDeptHandle, (pDeptMove+1), (sizeof(TTLCSDEPTINDX) * usMoveDeptNo), &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */
        if (ulActualBytesRead != sizeof(TTLCSDEPTINDX) * usMoveDeptNo) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
    }
    *pDeptMove = DeptIndex;
                                    /* Set Insertion Dept Number */
    return (TtlWriteFile(hsTtlDeptHandle, ulPositionIndStart, pDeptMove, sizeof(TTLCSDEPTINDX) * (usMoveDeptNo + 1))); /* ### Mod (2171 for Win32) V1.0 */
                                    /* Write Index Table */
}
/*
*============================================================================
**Synopsis:     SHORT TtlDeptBAssin(ULONG ulPosition, USHORT usDeptNo, 
*                                  USHORT usTtlOffset, VOID *pTmpBuff,
*                                  USHORT usOffset)
*
*    Input:     ULONG ulPosition        - Search Index Table Position
*               USHORT usDeptNo          - Number of Dept
*               USHORT *pusTtlOffset    - Pointer of Target Dept Total Offset
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHORT usOffset         - Target Dept Offset 
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Assin Dept Block 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assins the Dept Block Table.
*
*============================================================================
*/
SHORT TtlDeptBAssin(ULONG ulPosition, USHORT usDeptNo, ULONG ulTtlOffset, 
                   VOID *pTmpBuff, ULONG ulOffset)
{
    USHORT  *pDeptMove = pTmpBuff;
    ULONG   ulPositionIndStart;
    ULONG   ulActualPosition;
//    ULONG   ulLength;        
    USHORT	usMoveDeptNo;
    USHORT  usTotalBlock;
    SHORT   sRetvalue;
	ULONG	ulActualBytesRead;

    usTotalBlock = (USHORT)ulTtlOffset;
    usMoveDeptNo = usDeptNo - (USHORT)ulOffset;    /* Set Move Number of Dept */
    ulPositionIndStart = ulPosition + (ulOffset * sizeof(USHORT));  /* Set Moving Start Position */ 
    for (;usMoveDeptNo >= TTL_DEPT_TMPBUF_BLOCKREAD;) {
        if (PifSeekFile(hsTtlDeptHandle, ulPositionIndStart, &ulActualPosition) < 0) { 
                                    /* Seek Pointer of Dept Block Table */
            return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
        }
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlDeptHandle, (pDeptMove+1), (sizeof(USHORT) * (TTL_DEPT_TMPBUF_BLOCKREAD - 1)), &ulActualBytesRead);
        if (ulActualBytesRead != (sizeof(USHORT) * (TTL_DEPT_TMPBUF_BLOCKREAD - 1))) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
        *pDeptMove = usTotalBlock;      /* Set Insertion Dept Block */
		//Change made in PifReadFile, see piffile.c for new usage
        PifReadFile(hsTtlDeptHandle, &usTotalBlock, sizeof(USHORT), &ulActualBytesRead);
                                    /* Save Overwrited Dept Block */
        if (ulActualBytesRead != sizeof(USHORT)) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }

        sRetvalue = TtlWriteFile(hsTtlDeptHandle, ulPositionIndStart,pDeptMove, sizeof(USHORT) * (TTL_DEPT_TMPBUF_BLOCKREAD));
        if (sRetvalue != 0) {  /* ### Mod (2171 for Win32) V1.0 */
            return (sRetvalue);     /* Return Error Status */
        }
                                    /* Write Block Table */
        ulPositionIndStart += sizeof(USHORT) * (TTL_DEPT_TMPBUF_BLOCKREAD);  /* Update Start Position */
        usMoveDeptNo -= TTL_DEPT_TMPBUF_BLOCKREAD;                           /* Update Move Number of Dept */
    }
    if (PifSeekFile(hsTtlDeptHandle, ulPositionIndStart, &ulActualPosition) < 0) { 
                                    /* Seek Pointer of Dept Index Table */
        return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
    }
    if (usMoveDeptNo) {
		//Changed made in PifReadFile, see piffile.c for new usage
       PifReadFile(hsTtlDeptHandle, (pDeptMove + 1), (sizeof(USHORT) * usMoveDeptNo), &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */
        if (ulActualBytesRead != sizeof(USHORT) * usMoveDeptNo) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
    }
    *pDeptMove = usTotalBlock;      /* Set Insertion Dept Block */
    return (TtlWriteFile(hsTtlDeptHandle, ulPositionIndStart, pDeptMove, (sizeof(USHORT) * (usMoveDeptNo + 1))));  /* ### Mod (2171 for Win32) V1.0 */
                                    /* Write Block Table */
}


/*
*============================================================================
**Synopsis:     SHORT TtlCpnHeadWrite(TTLCSCPNHEAD *pCpnhd)
*
*    Input:     TTLCSCPNHEAD  *pCpnhd  - Pointer of Coupon File Header Data Buffer
*
*   Output:     Nothing
*    InOut:    Nothing
*
** Return:      TTL_SUCCESS         - Successed Writting Coupon File Header 
*                  TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Coupon File Header.         R3.0
*
*============================================================================
*/
SHORT TtlCpnHeadWrite(TTLCSCPNHEAD *pCpnhd)
{
    return(TtlWriteFile(hsTtlCpnHandle, 0L, pCpnhd, TTL_CPNHEAD_SIZE));    /* Write the Header Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCpnTotalWrite(ULONG ulPosition, UCHAR uchCpnNumber, 
*                                       VOID *pTotal, USHORT usSize)
*
*    Input:     ULONG ulPosition        - Coupon Total First Position
*              UCHAR uchCpnNumber     - Coupon Number
*              VOID *pTotal            - Pointer of Coupon Total Data
*              USHORT usSize           - Coupon Total Write Buffer Size
*
*   Output:     Nothing
*    InOut:    Nothing
*
** Return:      TTL_SUCCESS         - Successed Writting Coupon Total 
*                  TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Coupon Total.               R3.0
*
*============================================================================
*/
SHORT TtlCpnTotalWrite(ULONG ulPosition, UCHAR uchCpnNumber, 
                        VOID *pTotal, USHORT usSize)
{
     ULONG   ulPositionTtl;          /* Seek Position */

    ulPositionTtl = ulPosition + ((ULONG)uchCpnNumber - 1L) * (ULONG)usSize;     /* Calculate Taget Total Data Offset */
    return(TtlWriteFile(hsTtlCpnHandle, ulPositionTtl, pTotal, usSize));         /* Write Coupon Total Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasHeadWrite(TTLCSCASHEAD *Cashd)
*
*    Input:     TTLCSCASHEAD *Cashd     - Pointer of Cashier File Header Write Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writing Cashier Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Cashier Header.
*
*============================================================================
*/
SHORT TtlCasHeadWrite(TTLCSCASHEAD *pCashd)
{
    return(TtlWriteFile(hsTtlCasHandle, 0L, pCashd, TTL_CASHEAD_SIZE));     /* Write the Header Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasEmptyWrite(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition        - Cashier Empty Table Position
*               VOID *pTmpBuff          - Pointer of Cashier Empty Table Write Data Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Writing Cashier Empty Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Cashier Empty Table.
*
*============================================================================
*/
SHORT TtlCasEmptyWrite(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlWriteFile(hsTtlCasHandle, ulPosition, pTmpBuff, TTL_CASEMP_SIZE));    /* Write the Cashier Empty Data */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasTotalWrite(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition, VOID *pTmpBuff
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Writing Cashier Total 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Cashier Total.
*
*============================================================================
*/
SHORT TtlCasTotalWrite(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlWriteFile(hsTtlCasHandle, ulPosition, pTmpBuff, TTL_CASTTL_SIZE));    /* Write the Cashier Total Data */
}

/*
*============================================================================
**Synopsis:     SHORT TtlCasIAssin(ULONG ulPosition, USHORT usCasNo, 
*                                  TTLCSCASINDX *pCasInd, VOID *pTmpBuff,
*                                  ULONG ulOffset)
*
*    Input:     ULONG ulPosition        - Search Index Table Position
*               USHORT usCasNo          - Number of Cashier records (number of Cashiers)
*               TTLCSCASINDX *CasInd    - Pointer of Target Cashier number
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHORT usOffset         - Cashier Offset
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Assin Cashier Index 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assins the Cashier Index Table.
*
*============================================================================
*/
SHORT TtlCasIAssin(ULONG ulPosition, USHORT usCasNo, TTLCSCASINDX *pCasInd, 
                   VOID *pTmpBuff, ULONG ulOffset)
{
    TTLCSCASINDX    *pCasMove = pTmpBuff;
    TTLCSCASINDX    CasIndex;
    ULONG   ulPositionIndStart;
    ULONG   ulActualPosition;
    USHORT  usMoveCasNo;
    SHORT   sRetvalue;
	ULONG	ulActualBytesRead;
	//added for PifReadFile

    CasIndex = *pCasInd;

    usMoveCasNo = usCasNo - (USHORT)ulOffset;                             /* Set Move Number of Cashier */
    ulPositionIndStart = ulPosition + (ulOffset * sizeof(TTLCSCASINDX));  /* Set Moving Start Position */ 
    for (;usMoveCasNo >= TTL_CAS_TMPBUF_INDEXREAD;) {
        if (PifSeekFile(hsTtlCasHandle, ulPositionIndStart, &ulActualPosition) < 0) {  /* Seek Pointer of Cashier Index Table */
            return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
        }
		//Changed made in PifReadFile, , see piffile.c for new usage
        PifReadFile(hsTtlCasHandle, (pCasMove+1), (sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD - 1)), &ulActualBytesRead);
        if (ulActualBytesRead != (sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD - 1))) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
        *pCasMove = CasIndex;              /* Set Insertion Cashier Number */
		//Changed made in PifReadFile, , see piffile.c for new usage
        PifReadFile(hsTtlCasHandle, &CasIndex, sizeof(TTLCSCASINDX), &ulActualBytesRead);  /* Save Overwrited Cashier Number */
        if (ulActualBytesRead != sizeof(TTLCSCASINDX)) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
        if ((sRetvalue = TtlWriteFile(hsTtlCasHandle, ulPositionIndStart, pCasMove, sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD))) != 0) {
            return (sRetvalue);     /* Return Error Status */
        }
                                    /* Write Index Table */
        ulPositionIndStart += sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD);    /* Update Start Position */
        usMoveCasNo -= TTL_CAS_TMPBUF_INDEXREAD;                                    /* Update Move Number of Cashier */
    }
    if (PifSeekFile(hsTtlCasHandle, ulPositionIndStart, &ulActualPosition) < 0) {   /* Seek Pointer of Cashier Index Table */
        return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
    }
    if (usMoveCasNo) {
		//Changed made in PifReadFile, , see piffile.c for new usage
        PifReadFile(hsTtlCasHandle, (pCasMove + 1), (sizeof(TTLCSCASINDX) * usMoveCasNo), &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */
        if (ulActualBytesRead != sizeof(TTLCSCASINDX) * usMoveCasNo) {
            return (TTL_FILE_READ_ERR); /* Return File Read Error */
        }
    }
    *pCasMove = CasIndex;                                                    /* Set Insertion Cashier Number */
    return (TtlWriteFile(hsTtlCasHandle, ulPositionIndStart, pCasMove, (sizeof(TTLCSCASINDX) * (usMoveCasNo + 1))));       /* Write Index Table */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasIErase(ULONG ulPosition, USHORT usCasNo, 
*                                  VOID *pTmpBuff, ULONG ulOffset)
*
*    Input:     ULONG ulPosition        - Search Index Table Position
*               USHORT usCasNo          - Number of Cashier
*               VOID *pTmpBuff          - Pointer of Work Buffer
*               USHORT usOffset         - Cashier Offset
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Assin Cashier Index 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function assins the Cashier Index Table.
*
*============================================================================
*/
SHORT TtlCasIErase(ULONG ulPosition, USHORT usCasNo, VOID *pTmpBuff, ULONG ulOffset)
{
    TTLCSCASINDX    *pCasMove = pTmpBuff;
    TTLCSCASINDX    CasInd;
    ULONG   ulPositionIndStart;
    ULONG   ulActualPosition;
//    USHORT	usLength;        
    USHORT	usMoveCasNo;
    SHORT   sRetvalue;
	ULONG ulActualBytesRead;
	//JHHJ 11-7 Added for PifReadFile

    usMoveCasNo = usCasNo - (USHORT)ulOffset-1;   /* Set Move Number of Cashier */

    if ((LONG)usMoveCasNo < 0) {
        return (TTL_SIZEOVER);      /* Return Size Over */
    }

    if (usMoveCasNo) {
        ulPositionIndStart = ulPosition + (ulOffset * sizeof(TTLCSCASINDX));   /* Set Moving Start Position */ 
        for (;usMoveCasNo >= TTL_CAS_TMPBUF_INDEXREAD;) {
            if (PifSeekFile(hsTtlCasHandle, ulPositionIndStart+sizeof(TTLCSCASINDX), &ulActualPosition) < 0) { 
                                        /* Seek Pointer of Cashier Index Table */
                return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
            }
			//11-10-03 Change made in PifReadFile, see piffile.c for new usage
            PifReadFile(hsTtlCasHandle, pCasMove, (sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD)), &ulActualBytesRead);
            if (ulActualBytesRead !=  (sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD))) {
                return (TTL_FILE_READ_ERR); /* Return File Read Error */
            }
            if ((sRetvalue = TtlWriteFile(hsTtlCasHandle, ulPositionIndStart, pCasMove, sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD))) != 0) {
                return (sRetvalue);     /* Return Error Status */
            }
                                        /* Write Index Table */
            ulPositionIndStart += sizeof(TTLCSCASINDX) * (TTL_CAS_TMPBUF_INDEXREAD);  /* Update Start Position */
            usMoveCasNo -= TTL_CAS_TMPBUF_INDEXREAD;      /* Update Move Number of Cashier */
        }
        if (PifSeekFile(hsTtlCasHandle, ulPositionIndStart + sizeof(TTLCSCASINDX), &ulActualPosition) < 0) { 
                                        /* Seek Pointer of Cashier Index Table */
            return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
        }
        if (usMoveCasNo) {
			//JHHJ 11-7 Change to PifReadFile, see piffile.c for new usage
           PifReadFile(hsTtlCasHandle, pCasMove, (sizeof(TTLCSCASINDX) * usMoveCasNo), &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */
            if (ulActualBytesRead != sizeof(TTLCSCASINDX) * usMoveCasNo) {
                return (TTL_FILE_READ_ERR); /* Return File Read Error */
            }
            if ((sRetvalue = TtlWriteFile(hsTtlCasHandle, ulPositionIndStart, pCasMove, (sizeof(TTLCSCASINDX) * (usMoveCasNo)))) != 0) {  /* ### Mod (2171 for Win32) V1.0 */
                                        /* Write Index Table */
                return (sRetvalue);     /* Return Error Status */
            }
        }
    }

    /* clear last index */
    ulPositionIndStart = ulPosition +  (ULONG)((usCasNo-1) * sizeof(TTLCSCASINDX));
    memset(&CasInd, 0, sizeof(CasInd));
    return (TtlWriteFile(hsTtlCasHandle, ulPositionIndStart, &CasInd, sizeof(CasInd)));
                                                /* Write Index Table */
}
/*
*============================================================================
**Synopsis:     SHORT TtlEmptyTableUpdate(USHORT *pTmpBuff, USHORT usMaxNo,
*                                   USHORT usOffset, USHORT usMethod)
*
*    Input:     USHORT *pTmpBuff        - Pointer of Empty Table Data
*               USHORT usMaxNo          - Empty Table Max Number
*               USHORT usOffset         - Target Total Offset 
*               USHORT usMethod         - Update Method (0 = Set, 0 != Reset)
*
*   Output:     USHORT *pTmpBuff        - Upadte Empty Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed Updateing Cashier Empty Table 
*               TTL_SIZEOVER    - Failed Updateing Cashier Empty Table
*
** Description  This function Updates the Empty Table.
*
*============================================================================
*/
SHORT TtlEmptyTableUpdate(USHORT *pTmpBuff, USHORT usMaxNo, USHORT usOffset, 
                          USHORT usMethod)
{
    if (usMaxNo <= usOffset) {
        return (TTL_SIZEOVER);      /* Return Size Over */
    }
    pTmpBuff = pTmpBuff + (usOffset / 16);
    if (usMethod == TTL_EMPBIT_SET) {
        *pTmpBuff = *pTmpBuff | (1 << (usOffset % 16));     /* Set Empty Table */
    } else {
        *pTmpBuff = *pTmpBuff & (~(1 << (usOffset % 16)));  /* Reset Empty Table */
    }
    return (TTL_SUCCESS);           /* Return Success */
}


/************** New Functions (Release 3.1) BEGIN **************************/
/*
*============================================================================
**Synopsis:     SHORT TtlIndFinHeadWrite(TTLCSINDHEAD *pIndHead)
*
*    Input:     TTlCSINDHEAD pIndHead - Pointer of Individual File Header
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writting File Header 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Individual Financial File Header.  
*
*============================================================================
*/
SHORT TtlIndFinHeadWrite(TTLCSINDHEAD *pIndHead)
{
    return (TtlWriteFile( hsTtlIndFinHandle, 0L, pIndHead, TTL_INDHEAD_SIZE));
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinIndexWrite(ULONG ulPosition, USHORT usTermNum,
*                                         VOID *pTmpBuff)
*
*    Input:     ULONG   ulPosition  - Terminal Index Table Position
*               USHORT  usTermNum   - Current Terminal Number
*               VOID    *pTmpBuff   - Pointer of Terminal Index Table 
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writing Terminal Index Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Writes the Terminal Index Table.
*
*============================================================================
*/
SHORT TtlIndFinIndexWrite(ULONG ulPosition, USHORT usTermNum, VOID *pTmpBuff)
{
    ULONG  ulWritelength;       /* Writeing length */

    ulWritelength = usTermNum * sizeof(TTLCSINDINDX); /* Calculate Writeing length */
    return(TtlWriteFile(hsTtlIndFinHandle, ulPosition, pTmpBuff, ulWritelength));
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinEmptyWrite(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition    - Terminal Empty Table Position
*               VOID *pTmpBuff      - Pointer of Terminal Empty Table
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Writing Terminal Empty Table 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Terminal Empty Table.
*
*============================================================================
*/
SHORT TtlIndFinEmptyWrite(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlWriteFile(hsTtlIndFinHandle, ulPosition, pTmpBuff, TTL_INDEMP_SIZE));
                                    /* Write the Individual Financial Empty Data */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinTotalWrite(ULONG ulPosition, VOID *pTmpBuff)
*
*    Input:     ULONG ulPosition, VOID *pTmpBuff
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Writing Individual Financial 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Individual Financial Total.
*
*============================================================================
*/
SHORT TtlIndFinTotalWrite(ULONG ulPosition, VOID *pTmpBuff)
{
    return(TtlWriteFile(hsTtlIndFinHandle, ulPosition, pTmpBuff, TTL_INDTTL_SIZE));
                                    /* Write the Individual Financial */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinIAssin(VOID *pTmpBuff, USHORT usTermNum,
*                                     USHORT usNumber,  UCHAR uchOffset)
*
*    Input:     VOID *pTmpBuff      - Pointer of Index Table Data Buffer
*               USHORT usTermNum    - Current Terminal Number
*               USHORT usNumber     - Assign Terminal Number
*               UCHAR Offset        - Target Total Offset
*
*   Output:     VOID *pTmpBuff          - Assined Index Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed the Assin Index Terminal
*               TTL_SIZEOVER    - Max Size Over
*
** Description  This function Assins the Terminal Index Table.
*
*============================================================================
*/
SHORT TtlIndFinIAssin(VOID *pTmpBuff, USHORT usMaxNo, USHORT usTermNo,
                        USHORT usNumber,  UCHAR uchOffset)
{
    TTLCSINDINDX    *pInd;      /* Pointer Index Table */
	TTLCSINDINDX    IndIdx = {0};     /* Terminal Index Data */
    CHAR    *RetPointer;               /* Return Value */
    USHORT  i;
    SHORT   sRetvalue;

    if (usMaxNo < usTermNo + 1) {
        return (TTL_SIZEOVER);
    }

	IndIdx.usTerminalNo = usNumber;
    if (usTermNo) {
        sRetvalue = Rfl_SpBsearch(&IndIdx,
                                  sizeof(TTLCSINDINDX), 
                                  pTmpBuff,
                                  usTermNo,
                                  (VOID **)&RetPointer, 
                                  TtlcmpIndFing);

        pInd = (TTLCSINDINDX *)RetPointer;
        if (sRetvalue != RFL_TAIL_OF_TABLE) {
            i = (USHORT)((RetPointer - (CHAR *)pTmpBuff) / sizeof(TTLCSINDINDX)); /* ### Mod V1.0 */
            memmove(pInd + 1, pInd, (usTermNo - i) * sizeof(TTLCSINDINDX));
        }
    } else {
        pInd = (TTLCSINDINDX *)pTmpBuff;
    }
    pInd->usTerminalNo = usNumber;
    pInd->uchBlockNo   = uchOffset;
    return (TTL_SUCCESS);   /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlIndFinIErase(VOID *pTmpBuff, USHORT usTermNum,
*                                     UCHAR uchOffset)
*
*    Input:     VOID   *pTmpBuff    - Pointer of Index Table Data Buffer
*               USHORT usTermNum    - Current Terminal Number
*               UCHAR  uchOffset    - Target Total Offset
*
*   Output:     VOID *pTmpBuff      - Eraseed Index Table Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Successed the Erase Index Terminal 
*               TTl_SIZEOVER    - Failed the Erase Index Terminal
*
** Description  This function erases the Terminal Index Table.
*
*============================================================================
*/
SHORT TtlIndFinIErase(VOID *pTmpBuff, USHORT usTermNum, UCHAR uchOffset)
{
    TTLCSINDINDX    *pIndIdx = (TTLCSINDINDX *)pTmpBuff + uchOffset;       /* Pointer of Read Buffer for Index Table */
    TTLCSINDINDX    *pIndLastIdx = (TTLCSINDINDX *)pTmpBuff + (usTermNum -  1);   /* Pointer of Read Buffer for Index Table */
    ULONG           ulMovelength;

    if (usTermNum <= (USHORT)uchOffset) {
        return (TTL_SIZEOVER);      /* Return Size Over */
    }
    ulMovelength = (usTermNum - (USHORT)uchOffset - 1) * sizeof(TTLCSINDINDX);
    memmove(pIndIdx, pIndIdx + 1, ulMovelength);
    memset( pIndLastIdx, 0, sizeof(TTLCSINDINDX));  /* Clear Last Terminal */
    return (TTL_SUCCESS);   /* Return Success */
}

/*
*============================================================================
**Synopsis:     int TtlcmpIndFing(TTLCSINDINDX  *elem1, TTLCSINDINDX *elem2)
*
*    Input:     TTLCSINDINDX  *elem1 - Terminal Number 1
*               TTLCSINDINDX  *elem2 - Terminal Number 2
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
static SHORT TtlcmpIndFing(VOID  *p1, VOID *p2) /* ### Mod (2171 for Win32) V1.0 */
{
    TTLCSINDINDX  *elem1 = p1, *elem2 = p2;    /* ### Add (2171 for Win32) V1.0 */

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
**Synopsis:     SHORT TtlwriteSerTime(UCHAR uchMinorClass, UCHAR uchHourOffset,
*                                     UCHAR uchBorder, TOTAL *pTmpBuff)
*
*    Input:     UCHAR uchMinorClass     - Minor Class
*               UCHAR uchOffset         - Hourly Block Offset
*               UCHAR uchBorder         - Service Time Border    
*               TOTAL *pTmpBuff         - Pointer of Service Time
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Writing Service Time Total
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function writes the Service Time Total.
*
*============================================================================
*/
SHORT TtlwriteSerTime(UCHAR uchMinorClass, UCHAR uchHourOffset,
                        UCHAR uchBorder, TOTAL *pTmpBuff)
{
    ULONG   ulPosition = 0;         /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetSerTimeFilePosition(uchMinorClass, &ulPosition)) != 0) {
                                    /* Get Financial Data Position */
        return (sRetvalue);
    }

    /* --- Write Service Time Total --- */
    ulPosition += TTL_STAT_SIZE;
    ulPosition += (sizeof(TOTAL) * 3 * uchHourOffset);
    ulPosition += (sizeof(TOTAL) * uchBorder);
    
    sRetvalue = TtlWriteFile(hsTtlSerTimeHandle, ulPosition, pTmpBuff, sizeof(TOTAL));    /* Write Service Time Total */

    return (sRetvalue);
}

/************** New Functions (Release 3.1) END **************************/

/* End of File */
