/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Function Library Subroutine
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: Ttlflini.c
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*           SHORT TtlDateWrite()    - Write Date Data
*           SHORT TtlBaseInit()     - Initialize Base Total File
*           SHORT TtlDeptInit()     - Initialize Dept Total File
*           SHORT TtlPLUInit()      - Initialize PLU Total File
*           SHORT TtlCasInit()      - Initialize Cashier Total File
*           SHORT TtlUpdateInit()   - Initialize Update Total File
*           SHORT TtlIndFinInit()   - Initialize Individual Financial (R3.1)
*           SHORT TtlSerTimeInit()  - Initialize Service Time File    (R3.1)
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:K.Terai    :
* Mar-14-94:00.00.04:K.You      : add flex GC file feature.(mod. TtlUpdateInit)
* Mar- 3-95:03.00.00:hkato      : R3.0
* Dec-11-95:03.01.00:T.Nakahata : R3.1 Initial
*   Add Service Time Total (Daily/PTD) and Individual Terminal Financial (Daily)
*   Increase Regular Discount ( 2 => 6 )
*   Add Net Total at Register Financial Total
*   Add Some Discount and Void Elements at Server Total
* Mar-22-96:03.01.01:T.Nakahata : Cashier total offset USHORT to ULONG
* Aug-11-99:03.05.00:K.Iwata    : R3.5 Remove WAITER_MODEL
* Aug-13-99:03.05.00:K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : removed __DEL_2172 code for PLU totals.
* Aug-29-17 : 02.02.02 : R.Chambers : moved TtlDateWrite() to here and made static.
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
#include <math.h>
#include <memory.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <csttl.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <ttl.h>
#include <rfl.h>
#include <appllog.h>

/*
*============================================================================
**Synopsis:     SHORT  TtlDateWrite(SHORT hsHandle, ULONG ulPosition)
*
*    Input:     SHORT hsHandle              - File Handle
*               ULONG ulPosition            - Date Write Position
*               
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed Writing Date Data 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function writes the Date data to Total File.
*
*============================================================================
*/
static SHORT TtlDateWrite(SHORT hsHandle, ULONG ulPosition)
{
    DATE_TIME   DateTime;       /* Date & Time Work Area */

    PifGetDateTime(&DateTime);      /* Get Current Date & Time */
    return (TtlWriteFile(hsHandle, ulPosition, (VOID *)&DateTime.usMin, sizeof(N_DATE)));  /* Write Date To Total File */
}

/*
*============================================================================
**Synopsis:     SHORT TtlBaseInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Base Total File 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Initializes the Base Total File.
*
*============================================================================
*/
SHORT TtlBaseInit(TTLCS_TMPBUF_WORKING pTmpBuff)
{
    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   sRetvalue;

    if ((sRetvalue = PifSeekFile(hsTtlBaseHandle, 0L, &ulActualPosition)) < 0) {  /* TtlBaseInit() Seek First Pointer of Base Total File */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
    }

/* --- Financial Total File Initialize --- */
    TtlNullWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_SIZE * 4), pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 
                                    /* Set NULL Code Current Daily Finanial */
                                    /* Set NULL Code Previous Daily Finanial */
                                    /* Set NULL Code Current PTD Finanial */
                                    /* Set NULL Code Previous PTD Finanial */

/* --- Hourly Total File Initialize --- */
    TtlNullWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_SIZE * 4), pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 
                                    /* Set NULL Code Current Daily Hourly */
                                    /* Set NULL Code Previous Daily Hourly */
                                    /* Set NULL Code Current PTD Hourly */
                                    /* Set NULL Code Previous PTD Hourly */

/* --- Set Daily Current From Date & Time of Financial File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_DAYCURR + TTL_FIN_FMDATE))) != 0) {
                                    /* Set Daily From Date of Current Financial Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set Daily Previous From/To Date & Time of Financial File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_DAYCURR + TTL_FIN_SIZE + TTL_FIN_FMDATE))) != 0) {
                                    /* Set Daily From Date of Previous Financial Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_DAYCURR + TTL_FIN_SIZE + TTL_FIN_TODATE))) != 0) {
                                    /* Set Daily To Date of Previous Financial Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set PTD Current From Date & Time of Financial File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_PTDCURR + TTL_FIN_FMDATE))) != 0) {
                                    /* Set PTD From Date of Current Financial Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set PTD Previous From/To Date & Time of Financial File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_PTDCURR + TTL_FIN_SIZE + TTL_FIN_FMDATE))) != 0) {
                                    /* Set PTD From Date of Previous Financial Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_FIN_PTDCURR + TTL_FIN_SIZE + TTL_FIN_TODATE))) != 0) {
                                    /* Set PTD From Date of Previous Financial Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set Daily Current Date & Time of Current Hourly File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_DAYCURR + TTL_HOUR_FMDATE))) != 0) {
                                    /* Set Daily From Date of Current Hourly Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set Daily Previous From/To Date & Time of Current Hourly File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_DAYCURR + TTL_HOUR_SIZE + TTL_HOUR_FMDATE))) != 0) {
                                    /* Set Daily From Date of Previous Hourly Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_DAYCURR + TTL_HOUR_SIZE + TTL_HOUR_TODATE))) != 0) {
                                    /* Set Daily To Date of Previous Hourly Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set PTD Current Date & Time of Current Hourly File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_PTDCURR + TTL_HOUR_FMDATE))) != 0) {
                                    /* Set PTD From Date of Current Hourly Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set PTD Previous From/To Date & Time of Current Hourly File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_PTDCURR + TTL_HOUR_SIZE + TTL_HOUR_FMDATE))) != 0) {
                                    /* Set PTD From Date of Previous Hourly Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlBaseHandle, (ULONG)(TTL_HOUR_PTDCURR + TTL_HOUR_SIZE + TTL_HOUR_TODATE))) != 0) {
                                    /* Set PTD To Date of Previous Hourly Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_BASEINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlDeptInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Dept Total File 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Initializes the Dept Total File. 2172
*
*============================================================================
*/
SHORT TtlDeptInit(TTLCS_TMPBUF_WORKING pTmpBuff)
{
    TTLCSDEPTHEAD    *pDepthd = (VOID *)pTmpBuff;    /* Pointer of Read Buffer for Dept File Header */
    ULONG   ulDailycursize;     /* Daily Current Dept Total Table Size */
    ULONG   ulPTDcursize;       /* PTD Current Dept Total Table Size */
    ULONG   ulFileSize;         /* Dept Total File Size */
    ULONG   ulCurDayMisc;
    ULONG   ulPreDayMisc;
    ULONG   ulCurPTDMisc;
    ULONG   ulPrePTDMisc;
    SHORT   sRetvalue;

/* --- Read Dept File Header --- */
    if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {   /* Read Dept File Header */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    ulDailycursize = (TTL_DEPTMISC_SIZE + 
                      (sizeof(TTLCSDEPTINDX) * (ULONG)pDepthd->usMaxDept) +
                      (TTL_DEPTBLOCKRECORD_SIZE * (ULONG)pDepthd->usMaxDept) +
                      TTL_DEPTEMP_SIZE +
                      (TTL_DEPTTOTALRECORD_SIZE * (ULONG)pDepthd->usMaxDept));
                                    /* Caluculate Daily Dept Total Size */
    if (pDepthd->ulCurPTDMiscOff != 0) {
        ulPTDcursize = ulDailycursize;  /* Caluculate PTD Dept Total Size */
    } else {
        ulPTDcursize = 0L;
    }
    ulFileSize = (ulDailycursize * 2) + (ulPTDcursize * 2);
    pDepthd->usCurDayDept = 0;        /* Set Number of Current Daily Dept */
    pDepthd->usPreDayDept = 0;        /* Set Number of Previous Daily Dept */
    pDepthd->usCurPTDDept = 0;        /* Set Number of Current PTD Dept */
    pDepthd->usPrePTDDept = 0;        /* Set Number of Prevoius PTD Dept */
    ulCurDayMisc = pDepthd->ulCurDayMiscOff;    /* Save Current Daily Misc Offset */
    ulPreDayMisc = pDepthd->ulPreDayMiscOff;    /* Save Previous Daily Misc Offset */
    ulCurPTDMisc = pDepthd->ulCurPTDMiscOff;    /* Save Current PTD Misc Offset */
    ulPrePTDMisc = pDepthd->ulPrePTDMiscOff;    /* Save Previous PTD Misc Offset */

	/* --- Write Dept File Header --- */
    if ((sRetvalue = TtlDeptHeadWrite(pDepthd)) != 0) {  
                                    /* Write Dept File Header */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
        
/* --- Dept Total File Initialize --- */
    TtlNullWrite(hsTtlDeptHandle, ulFileSize, pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 
                                    /* Set NULL Code Cuurent Daily Dept */
                                    /* Set NULL Code Previous Daily Dept */
                                    /* Set NULL Code Current PTD Dept */
                                    /* Set NULL Code Previous PTD Dept */

/* --- Set Daily Start Date & Time of Current Dept File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlDeptHandle, (ulCurDayMisc + TTL_DEPT_FMDATE))) != 0) {
                                    /* Set From Date of Current Daily Dept Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set Daily Start/End Date & Time of Previous Dept File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlDeptHandle, (ulPreDayMisc + TTL_DEPT_FMDATE))) != 0) {
                                    /* Set From Date of Current Daily Dept Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlDeptHandle, (ulPreDayMisc + TTL_DEPT_TODATE))) != 0) {
                                    /* Set To Date of Current Daily Dept Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set PTD Start Date & Time of Current Dept File --- */
    if (ulPTDcursize != 0L) {
        if ((sRetvalue = TtlDateWrite(hsTtlDeptHandle, (ulCurPTDMisc + TTL_DEPT_FMDATE))) != 0) {
                                    /* Set PTD From & To Date of Current Daily Dept Total */
			PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
			PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
            return (sRetvalue);         /* Return Error Status */
        }

/* --- Set PTD Start/End Date & Time of Previous Dept File --- */
        if ((sRetvalue = TtlDateWrite(hsTtlDeptHandle, (ulPrePTDMisc + TTL_DEPT_FMDATE))) != 0) {
                                    /* Set PTD From Date of Current Daily Dept Total */
			PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
			PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
            return (sRetvalue);         /* Return Error Status */
        }
        if ((sRetvalue = TtlDateWrite(hsTtlDeptHandle, (ulPrePTDMisc + TTL_DEPT_TODATE))) != 0) {
                                    /* Set PTD To Date of Current Daily Dept Total */
			PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_DEPTINIT);
			PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
            return (sRetvalue);         /* Return Error Status */
        }
    }
    return (TTL_SUCCESS);           /* Return Success */
}


/*
*============================================================================
**Synopsis:     SHORT TtlCpnInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Coupon Total File 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Initializes the Coupon Total File.     R3.0
*
*============================================================================
*/
SHORT TtlCpnInit(TTLCS_TMPBUF_WORKING pTmpBuff)
{
    TTLCSCPNHEAD    *pCpnhd = (VOID *)pTmpBuff;    /* Pointer of Coupon File Header */
    USHORT  usDailycursize;     /* Daily Current Coupon Total Table Size */
    USHORT  usPTDcursize;       /* PTD Current Coupon Total Table Size */
    USHORT  usFileSize;         /* Coupon Total File Size */
    SHORT   sRetvalue;

/* --- Read Coupon File Header --- */
    if ((sRetvalue = TtlCpnHeadRead(pCpnhd)) != 0) {
                                    /* Read Coupon File Header */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

    usDailycursize = (TTL_CPNMISC_SIZE + (sizeof(TOTAL) * pCpnhd->usMaxCpn));     /* Caluculate Daily Coupon Total Size */
    if (pCpnhd->usCurPTDTtlOff != 0) {       
        usPTDcursize = (TTL_CPNMISC_SIZE + (sizeof(TOTAL) * pCpnhd->usMaxCpn));   /* Caluculate PTD Coupon Total Size */
    } else {
        usPTDcursize = 0;
    }
    usFileSize = (usDailycursize * 2) + (usPTDcursize * 2);
                                    /* Calculate Coupon ToTal Size */

/* --- Coupon Total File Initialize --- */
    TtlNullWrite(hsTtlCpnHandle, (ULONG)usFileSize, pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 
                                    /* Set NULL Code Cuurent Daily Coupon */
                                    /* Set NULL Code Previous Daily Coupon */
                                    /* Set NULL Code Current PTD Coupon */
                                    /* Set NULL Code Previous PTD Coupon */

/* --- Set Daily Start Date & Time of Current Coupon File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlCpnHandle, (ULONG)(TTL_CPNHEAD_SIZE + TTL_CPN_FMDATE))) != 0) {
                                    /* Set Daily From Date of Current Daily Coupon Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set Daily Start/End Date & Time of Previous Coupon File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlCpnHandle, (ULONG)(TTL_CPNHEAD_SIZE + usDailycursize + TTL_CPN_FMDATE))) != 0) {
                                    /* Set Daily From Date of Previous Daily Coupon Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

    if ((sRetvalue = TtlDateWrite(hsTtlCpnHandle, (ULONG)(TTL_CPNHEAD_SIZE + usDailycursize + TTL_CPN_TODATE))) != 0) {
                                    /* Set Daily TO Date of Previous Daily Coupon Total */
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Set PTD Start Date & Time of Current Coupon File --- */
    if (usPTDcursize != 0) {
        if ((sRetvalue = TtlDateWrite(hsTtlCpnHandle, (ULONG)(TTL_CPNHEAD_SIZE + (usDailycursize * 2) + TTL_CPN_FMDATE))) != 0) {
                                    /* Set PTD From & To Date of Current Daily Coupon Total */
			PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
			PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
            return (sRetvalue);     /* Return Error Status */
        }

/* --- Set PTD Start/End Date & Time of Previous Coupon File --- */
        if ((sRetvalue = TtlDateWrite(hsTtlCpnHandle, (ULONG)(TTL_CPNHEAD_SIZE + (usDailycursize * 2) + usPTDcursize + TTL_CPN_FMDATE))) != 0) {
                                    /* Set PTD From Date of Previous Daily Coupon Total */
			PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
			PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
            return (sRetvalue);     /* Return Error Status */
        }
        if ((sRetvalue = TtlDateWrite(hsTtlCpnHandle, (ULONG)(TTL_CPNHEAD_SIZE + (usDailycursize * 2) + usPTDcursize + TTL_CPN_TODATE))) != 0) {
                                    /* Set PTD TO Date of Previous Daily Coupon Total */
			PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_CPNINIT);
			PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
            return (sRetvalue);     /* Return Error Status */
        }
    }
     return (TTL_SUCCESS);          /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCasInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Cashier Total File 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Initializes the Cashier Total File. V3.3
*
*============================================================================
*/
SHORT TtlCasInit(TTLCS_TMPBUF_WORKING pTmpBuff)
{
    TTLCSCASHEAD    *pCashd = (VOID *)pTmpBuff;    /* Pointer of Read Buffer for Cashier File Header */
    ULONG   ulDailycursize;     /* Daily Current Cashier Total Table Size */
    ULONG   ulPTDcursize;       /* PTD Current Waiter Total Table Size */
    ULONG   ulFileSize;         /* Cashier Total File Size */
    SHORT   sRetvalue;

/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {  /* Read Cashier File Header */
        return (sRetvalue);         /* Return Error Status */
    }
    ulDailycursize = (ULONG)((sizeof(TTLCSCASINDX) * (ULONG)pCashd->usMaxCas) + TTL_CASEMP_SIZE + (TTL_CASTTL_SIZE * (ULONG)pCashd->usMaxCas));
                                    /* Caluculate Daily Cashier Total Size */
    /* cashier PTD, V3.3 */
    if (pCashd->ulCurPTDIndexOff) {
        ulPTDcursize = ulDailycursize;      /* Caluculate PTD Cashier Total Size */
    } else {
        ulPTDcursize = 0L;
    }
    ulFileSize = (ulDailycursize * 2L) + (ulPTDcursize * 2L);
    pCashd->usCurDayCas = 0;        /* Set Number of Current Daily Cashier */
    pCashd->usPreDayCas = 0;        /* Set Number of Previous Daily Cashier */
    pCashd->usCurPTDCas = 0;        /* Set Number of Current PTD Cashier */
    pCashd->usPrePTDCas = 0;        /* Set Number of Previous PTD Cashier */
/* --- Update Cashier File Header --- */ 
    if ((sRetvalue = TtlCasHeadWrite(pCashd)) != 0) {   /* Write Cashier File Header */
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Cashier Total File Initialize --- */
    TtlNullWrite(hsTtlCasHandle, (ULONG)ulFileSize, pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 
                                    /* Set NULL Code Cuurent Daily Cashier */
                                    /* Set NULL Code Previous Daily Cashier */
                                    /* Set NULL Code Current PTD Cashier */
                                    /* Set NULL Code Previous PTD Cashier */

    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlUpdateInit(VOID *TmpBuff, USHORT usNoofItem)
*
*    Input:     VOID    *pTmpBuff - Pointer of Work Buffer
*               USHORT  usNoofItem - Number of Item for Item Storage 
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Update Total File 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*
** Description  This function Initializes the Update Total File. 2172
*
*============================================================================
*/
SHORT TtlUpdateInit(TTLCS_TMPBUF_WORKING pTmpBuff, ULONG ulNoofItem)
{

    ULONG       ulActualPosition;   /* Actual Seek Pointer */
    ULONG       ulFileSize;

	NHPOS_NONASSERT_TEXT("==NOTE:  TtlUpdateInit() called to initialize TOTALUPD.");

    if (PifSeekFile(hsTtlUpHandle, 0L, &ulActualPosition) < 0) {  /* Seek First Pointer of Update Total File */
        return (TTL_FILE_SEEK_ERR); /* Return File Seek Error */
    }

    memset (&TtlUphdata, 0, TTL_UPHEAD_SIZE);
	memset (&TtlUphDBdata, 0, TTL_UPHEAD_SIZE);

    /* calculate file size */
    if ((ulFileSize = TrnCalStoSize(ulNoofItem, FLEX_ITEMSTORAGE_ADR)) < TTL_UPDATE_FILESIZE) {
        ulFileSize = TTL_UPDATE_FILESIZE;
    }

    /* Update Total File Initialize */
    TtlNullWrite(hsTtlUpHandle, ulFileSize, pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING));  /* Set NULL Code Update Temp Buffer */
    
    return (TTL_SUCCESS);           /* Return Success */
}

/******************** New Functions (Release 3.1) BEGIN ********************/
/*
*============================================================================
**Synopsis:     SHORT TtlIndFinInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Base Total File 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description
*   This function Initializes the Individual Financial Total File (Daily).
*============================================================================
*/
SHORT TtlIndFinInit(TTLCS_TMPBUF_WORKING pTmpBuff)
{
    TTLCSINDHEAD *pIndFinhd = (VOID *)pTmpBuff;    /* Pointer of Individual Financial Header */
    ULONG   ulDailycursize;     /* Daily Current Total Table Size */
    ULONG   ulFileSize;         /* Individual Financial Total File Size */
    SHORT   sRetvalue;

    /* --- Read File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndFinhd)) != 0) {
        return (sRetvalue);
    }

    ulDailycursize = ((ULONG)(sizeof(TTLCSINDINDX) * pIndFinhd->usMaxTerminal) + (ULONG)TTL_INDEMP_SIZE + 
                      (ULONG)(TTL_INDTTL_SIZE * pIndFinhd->usMaxTerminal));
    ulFileSize     = (ulDailycursize * 2);

    pIndFinhd->usCurDayTerminal = 0;    /* Set Number of Current Daily Terminal */
    pIndFinhd->usPreDayTerminal = 0;    /* Set Number of Previous Daily Terminal */

    /* --- Update Individual File Header --- */
    if ((sRetvalue = TtlIndFinHeadWrite(pIndFinhd)) != 0) {  
        return (sRetvalue);
    }

    /* --- Individual Financial Total File Initialize --- */
    TtlNullWrite(hsTtlIndFinHandle, ulFileSize, pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 

    return (TTL_SUCCESS);
}


/*
*============================================================================
**Synopsis:     SHORT TtlSerTimeInit(VOID *TmpBuff)
*
*    Input:     VOID *pTmpBuff      - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Initialize Service Time 
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description
*   This function Initializes the Service Time Total File (Daily/PTD).
*============================================================================
*/
SHORT TtlSerTimeInit(TTLCS_TMPBUF_WORKING pTmpBuff)
{
    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   sRetvalue;

    if ((sRetvalue = PifSeekFile(hsTtlSerTimeHandle, 0L, &ulActualPosition)) < 0) { 
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (TTL_FILE_SEEK_ERR);
    }

    /* --- Service Time Total File Initialize --- */
    TtlNullWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTTL_SIZE * 4), pTmpBuff, sizeof(TTLCS_TMPBUF_WORKING)); 

    /* --- Set Daily Current From Date & Time of Service Time File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTIME_DAYCURR + TTL_SERTIME_FMDATE))) != 0) {
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);
    }

    /* --- Set Daily Previous From/To Date & Time of Service Time File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTIME_DAYCURR + TTL_SERTTL_SIZE + TTL_SERTIME_FMDATE))) != 0) {
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTIME_DAYCURR + TTL_SERTTL_SIZE + TTL_SERTIME_TODATE))) != 0) {
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Set PTD Current From Date & Time of Service Time File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTIME_PTDCURR + TTL_SERTIME_FMDATE))) != 0) {
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Set PTD Previous From/To Date & Time of Service Time File --- */
    if ((sRetvalue = TtlDateWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTIME_PTDCURR + TTL_SERTTL_SIZE + TTL_SERTIME_FMDATE))) != 0) {
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlDateWrite(hsTtlSerTimeHandle, (ULONG)(TTL_SERTIME_PTDCURR + TTL_SERTTL_SIZE + TTL_SERTIME_TODATE))) != 0) {
		PifLog (MODULE_TTL_TFL, LOG_ERROR_TTL_SERINIT);
		PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TFL), (USHORT)abs(sRetvalue));
        return (sRetvalue);         /* Return Error Status */
    }

    return (TTL_SUCCESS);           /* Return Success */
}

/******************** New Functions (Release 3.1) END ***********************/

/* End of File */
