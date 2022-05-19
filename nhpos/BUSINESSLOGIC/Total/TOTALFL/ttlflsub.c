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
* Program Name: Ttlflsub.c                                                      
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlTreadFin()     - Read Financial Total File
*           SHORT TtlTreadHour()    - Read Hourly Total File
*           SHORT TtlTreadIncrHour()    - Read Hourly Total File incrementally
*           SHORT TtlTreadDept()    - Read Dept Total File
*           SHORT TtlTreadPLU()     - Read PLU Total File
*           SHORT TtlTreadCpn()     - Read Coupon Total File,        R3.0
*           SHORT TtlTreadCas()     - Read Cashier Total File
*           SHORT TtlTreadWai()     - Read Waiter Total File
*           SHORT TtlTreadSerTime() - Read Service Time Total        R3.1
*           SHORT TtlTreadIndFin()  - Read Individual Financial      R3.1
*           SHORT TtlTresetEODPTD() - Reset EOD/PTD Total File
*           SHORT TtlTresetFin()    - Reset Financial Total File
*           SHORT TtlTresetHour()   - Reset Hourly Total File
*           SHORT TtlTresetDept()   - Reset Dept Total File
*           SHORT TtlTresetPLU()    - Reset PLU Total File
*           SHORT TtlTresetCpn()    - Reset Coupon Total File,       R3.0
*           SHORT TtlTresetCas()    - Reset Cashier Total File
*           SHORT TtlTresetWai()    - Reset Waiter Total File
*           SHORT TtlTresetSerTime()- Read Service Time Total        R3.1
*           SHORT TtlTresetIndFin() - Read Individual Financial      R3.1
*           SHORT TtlTDeleteCas()   - Delete Cashier Total File
*           SHORT TtlTDeleteWai()   - Delete Waiter Total File
*           SHORT TtlTDeleteIndFin()- Delete Individual Financial    R3.1
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name      : Description
* May-06-92 : 00.00.01 : K.Terai     :                                    
* Mar-03-95 : 03.00.00 : hkato       : R3.0
* Mar-06-96 : 03.01.00 : T.Nakahata  : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Apr-23-96 : 03.01.04 : T.Nakahata  : Not reset CGG total on Ind-Fin
* May-09-96 : 03.01.05 : T.Nakahata  : Fix a glitch at TtlTresetIndFin (idx search)
* Aug-11-99 : 03.05.00 : K.Iwata     : R3.5 Remov WAITER_MODEL
* Aug-13-99 : 03.05.00 : K.Iwata     : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32 Initial
* Sep-17-99 : 01.00.00 : K.Iwata     : Changed type definition of 'D13DIGITS'
* GenPOS Rel 2.2 for Win32 and Touchscreen
* Apr-15-15 : 02.02.01 : R.Chambers  : correct problem with SavedTotals snapshot from Rel 2.1
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
#include <stddef.h>

#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <csttl.h>
#include <para.h>
#include <csstbpar.h>
#include <ttl.h>
#include <appllog.h>
#include <csop.h>
#include <..\report\rptcom.h>

N_DATE *TtlCopyNdateFromDateTime (N_DATE *pNdate, DATE_TIME *pDateTime)
{
	pNdate->usMin   = pDateTime->usMin;
	pNdate->usHour  = pDateTime->usHour;
	pNdate->usMDay  = pDateTime->usMDay;
	pNdate->usMonth = pDateTime->usMonth;
#if defined(DCURRENCY_LONGLONG)
	pNdate->usYear  = pDateTime->usYear;
#endif

	return pNdate;
}

/*
*============================================================================
**Synopsis:     TtlTreadFin(TTLREGFIN *pTotal, TTLCSREGFIN *pTmpBuff)
*
*    Input:     TTLREGFIN *pTotal       - Major & Minor Class
*               TTLCSREGFIN *pTmpBuff   - Work Buffer Pointer
*
*   Output:     TTLREGFIN *pTotal       - Financial Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reading Financial Total File 
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Financial Total File.
*
*============================================================================
*/
SHORT TtlTreadFin(TTLREGFIN *pTotal)
{
    SHORT    sRetvalue;

	NHPOS_ASSERT(sizeof(TTLREGFIN) >= sizeof(TTLCSREGFIN) + offsetof(TTLREGFIN,uchResetStatus));

	if ((sRetvalue = TtlreadFin(pTotal->uchMinorClass, (VOID *)&pTotal->uchResetStatus)) != 0) {    /* Read Financial File */
		memset (&pTotal->uchResetStatus, 0, TTL_STAT_SIZE);     // ensure memory is set to a known value on error return.
        return (sRetvalue);         /* Return Error */
    }

    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTreadHour(TTLHOURYTBL *pTotal, TTLCSHOULY *pTmpBuff)
*
*    Input:     TTLHOURYTBL *pTotal     - Major & Minor Class
*               TTLCSHOULY *pTmpBuff    - Work Buffer Pointer
*
*   Output:     TTLHOURYTBL *pTotal     - Hourly Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reading Hourly Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function reads the Hourly Total File. V3.3
*
*============================================================================
*/
SHORT TtlTreadHour(TTLHOURLY *pTotal)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT    sRetvalue;
    USHORT   i;

    if ((sRetvalue = TtlGetHourFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {     /* Get Hourly Data Position */
        return (sRetvalue);
    }

    /* read only hourly total header at first, V3.3 */
	/* --- Read Hourly Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &pTotal->uchResetStatus, TTL_STAT_SIZE)) != 0) {      /* Read Hourly Total File */
		memset (&pTotal->uchResetStatus, 0, TTL_STAT_SIZE);  // ensure that header is set to known values if error.
        return (sRetvalue);         /* Return Error Status */
    }

    ulPosition += TTL_STAT_SIZE;

    /* Copy Hourly Total, V3.3 */
    for (i = 0; i < STD_MAX_HOURBLK; i++) {
        if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &pTotal->Total[i], sizeof(HOURLY))) != 0) {  /* Read Hourly Total File */
            return (sRetvalue);         /* Return Error Status */
        } 

        ulPosition += sizeof(HOURLY);
    }

    return (TTL_SUCCESS);           /* Return Success */
}

SHORT TtlTwriteHour(TTLHOURLY *pTotal)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT    sRetvalue;
    USHORT   i;

    /* Get Hourly Data Position based on the pTotal->uchMinorClass */
    if ((sRetvalue = TtlGetHourFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {
        return (sRetvalue);
    }

    ulPosition += TTL_STAT_SIZE;  // skip the header which spcecifies reset status and From/To dates.

    /* Copy Hourly Total, V3.3 */
    for (i = 0; i < STD_MAX_HOURBLK; i++) {
        if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPosition, &pTotal->Total[i], sizeof(HOURLY))) != 0) {
            return (sRetvalue);         /* Return Error Status */
        } 

        ulPosition += sizeof(HOURLY);
    }

    return (TTL_SUCCESS);           /* Return Success */
}


/*
*============================================================================
**Synopsis:     SHORT TtlTreadIncrHour(TTLHOURYTBL *pTotal, TTLCSHOULY *pTmpBuff)
*
*    Input:     TTLHOURYTBL *pTotal     - Major & Minor Class
*               TTLCSHOULY *pTmpBuff    - Work Buffer Pointer
*
*   Output:     TTLHOURYTBL *pTotal     - Hourly Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reading Hourly Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function reads the Hourly Total File Incremenatally. V3.3
*               pTotal->usBlockRead of the TTLHOURLY struct contains the iterator
*               which on the first call to this function should be set to zero (0).
*               The first call will set the management data of the time stamp info
*               and provide the first hourly bucket data.
*               Successive calls are then used to obtain the remaining buckets of data.
*               Each time the function is called, the iterator is incremented.
*============================================================================
*/
SHORT TtlTreadIncrHour(TTLHOURLY *pTotal, TTLCSHOURLY *pTmpBuff)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT    sRetvalue;

	if ((sRetvalue = TtlGetHourFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {             /* Get Hourly Data Position */
		return (sRetvalue);
	}

	if (pTotal->usBlockRead >= STD_MAX_HOURBLK ) {
		NHPOS_ASSERT_TEXT((pTotal->usBlockRead < STD_MAX_HOURBLK), "**WARNING: TtlTreadIncrHour() usBlockRead out of range.");
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_OUT_OF_RANGE);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), pTotal->usBlockRead);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
		return TTL_NOTINFILE;
	}

	/* read only hourly total header at first, if no data has been read yet V3.3 */
	if(pTotal->usBlockRead == 0) {
		/* --- Read Hourly Total --- */
		if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, pTmpBuff, TTL_STAT_SIZE)) != 0) {   /* Read Hourly Total File */
			return (sRetvalue);         /* Return Error Status */
		}

		/* --- Copy To Interface Data Format --- */
		/* Copy Reset Status, From & To Date */
		pTotal->uchResetStatus = pTmpBuff->uchResetStatus;
		pTotal->FromDate = pTmpBuff->FromDate;
		pTotal->ToDate = pTmpBuff->ToDate;

		//move the position to Total information
		ulPosition += TTL_STAT_SIZE;	
	}
	else {
		//if subsequent read into file
		ulPosition = (ulPosition + ((pTotal->usBlockRead * sizeof(HOURLY)) + TTL_STAT_SIZE));
	}

    /* Copy Hourly Total, V3.3 */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &pTotal->Total[0], sizeof(HOURLY))) != 0) {    /* Read Hourly Total File */
		return (sRetvalue);         /* Return Error Status */
    } 

	/*save the read position so we know where to begin reading at during
	the next call if all of the HOURLY info has not been read*/
	pTotal->usBlockRead++;

    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTreadDept(TTLDEPT *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLDEPT *pTotal          - Major & Minor Class, Dept Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     TTLDEPT *pTotal          - Dept Total File Data
*    InOut:     Nothing
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
SHORT TtlTreadDept(TTLDEPT *pTotal, VOID *pTmpBuff)
{
    ULONG   ulPositionMsc;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */

    return (TtlreadDept(pTotal, pTmpBuff, &ulPositionMsc, &ulPositionTtl, TTL_SEARCH_EQU));    /* Read Dept Total */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTreadCpn(TTLCPN *pTotal)
*
*    Input:     TTLCPN *pTotal         - Major & Minor Class , Coupon Number
*
*   Output:     TTLCPN *pTotal         - Coupon Total File Data
*    InOut:    Nothing
*
** Return:      TTL_SUCCESS            - Successed the Reading Coupon Total  
*                  TTL_FILE_HANDLE_ERR    - Occured File Handle Error
*                   TTL_FILE_SEEK_ERR      - Occured File Seek Error
*              TTL_FILE_READ_ERR      - Occured File Read Error
*              TTL_NOTCLASS           - Set Illigal Class Code 
*              TTL_NOTPTD             - Not Exist PTD
*              TTL_SIZEOVER           - Max Size Over
*
** Description  This function reads the Coupon Total File.           R3.0
*
*============================================================================
*/
SHORT TtlTreadCpn(TTLCPN *pTotal)
{
    return (TtlreadCpn(pTotal));    /* Read Coupon Total */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTreadCas(TTLCASHIER *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLCASHIER *pTotal      - Major & Minor Class, Cashier Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     TTLCASHIER *pTotal      - Cashier Total File Data
*
** Return:      TTL_SUCCESS         - Successed the Reading Cashier Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE       - Cashier Not Exist
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function reads the Cashier Total File and then
*               copies the returned data into the buffer provided
*               returning the status of the read attempt.
*
*============================================================================
*/
SHORT TtlTreadCas(TTLCASHIER *pTotal)
{
    SHORT    sRetValue = TTL_SUCCESS;

    /* Read Cashier File */
    sRetValue = TtlreadCas(pTotal->uchMinorClass, pTotal->ulCashierNumber, &(pTotal->uchResetStatus));

    return (sRetValue);
}

/*
*============================================================================
**Synopsis:     SHORT TtlTresetEODPTD(USHORT usMDCBit, UCHAR uchClass,
*                                     VOID *pTmpBuff)
*
*    Input:     USHORT  usMDCBit    - Execution Data
*               UCHAR   uchClass    - Minor Class
*               VOID    *pTmpBuff   - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting EOD Total  
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the EOD Total File.             R3.0
*
*============================================================================
*/
SHORT TtlTresetEODPTD(USHORT usMDCBit, UCHAR uchClass, TTLCS_TMPBUF_WORKING pTmpBuff)
{
    SHORT   sRetvalue;          /* Return Value */

	/* --- Reset Financial Total File --- */        
    if ((usMDCBit & TTL_BIT_FIN_EXE) == 0) {
        sRetvalue = TtlTresetFin(uchClass, (VOID *)pTmpBuff);
        if (sRetvalue) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
            } else {
                return (sRetvalue); /* Return Error */
            }
        }
    }

	/* --- Reset Hourly Total File --- */        
    if ((usMDCBit & TTL_BIT_HOUR_EXE) == 0) {
        sRetvalue = TtlTresetHour(uchClass, (VOID *)pTmpBuff);
        if (sRetvalue) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
            } else {
                return (sRetvalue); /* Return Error */
            }
        }
    }

	/* --- Reset Dept Total File --- */        
    if ((usMDCBit & TTL_BIT_DEPT_EXE) == 0) {
		DEPTTBL     Table[FLEX_DEPT_MAX + 1] = { 0 };  // plus one to hold the RPT_EOF record indicating end of list.

		RptDEPTTableAllLocal(CLASS_TTLSAVDAY, &Table[0]);            /* Saratoga */
		TtlWriteSavFile(CLASS_TTLDEPT, &Table[0], sizeof(Table), 0);
		
		sRetvalue = TtlTresetDept(uchClass, pTmpBuff);
        if (sRetvalue) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, 0);
            return (sRetvalue);     /* Return Error */
        }
    }

	/* --- Reset PLU Total File --- */        
    if ((usMDCBit & TTL_BIT_PLU_EXE) == 0) {
		TtlWriteSavFilePlu(CLASS_TTLSAVDAY);
        sRetvalue = TtlTresetPLU(uchClass, (VOID  *)pTmpBuff);
        if (sRetvalue) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);
            return (sRetvalue);     /* Return Error */
        }
    }

	/* --- Reset Coupon Total File, R3.0 --- */
    if ((usMDCBit & TTL_BIT_CPN_EXE) == 0) {
        sRetvalue = TtlTresetCpn(uchClass, pTmpBuff);
        if (sRetvalue) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, 0);
            if (sRetvalue != TTL_FILE_HANDLE_ERR)
				return (sRetvalue);     /* Return Error */
        }
    }

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    
    /* --- Reset Service Time Total, R3.1 --- */
    if ((usMDCBit & TTL_BIT_SERTIME_EXE) == 0) {
        sRetvalue = TtlTresetSerTime(uchClass, pTmpBuff);
        if (sRetvalue) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TFL_SERTIME, sRetvalue, 0);
            if (sRetvalue != TTL_FILE_HANDLE_ERR)
				return (sRetvalue);     /* Return Error */
        }
    }

    /* ===== New Functions (Release 3.1) END ===== */

     return (TTL_SUCCESS);          /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTresetFin(UCHAR uchClass, TTLCSREGFIN *pTmpBuff)
*
*    Input:     UCHAR uchClass          - Minor Class
*               TTLCSREGFIN *pTmpBuff   - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Financial Total 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function resets the Financial Total.            R3.0
*
*============================================================================
*/
SHORT TtlTresetFin(UCHAR uchClass, TTLCSREGFIN *pTmpBuff)
{
    ULONG  ulPosition;         /* Save Seek Position */
    ULONG  ulPositionPrev;     /* Save Seek Position */
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    D13DIGITS   CGGTotal;       /* Save Current Gross Group Total Area */
    D13DIGITS   Taxable[STD_TAX_ITEMIZERS_MAX];     /* Save Taxable Amount Area */
    SHORT    sRetvalue;
	TTLCSREGFIN SavedTotal = {0};

    if (hsTtlBaseHandle < 0) {         /* Check Base Total File Handle */
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(hsTtlBaseHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);  /* Return File Handle Error */
    }

/* --- Set Reset Total Offset --- */
    switch (uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        ulPosition = TTL_FIN_DAYCURR;
        ulPositionPrev = TTL_FIN_DAYCURR + TTL_FIN_SIZE;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
        ulPosition = TTL_FIN_PTDCURR;
        ulPositionPrev = TTL_FIN_PTDCURR + TTL_FIN_SIZE;
        break;

    default:
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_CLASS);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), uchClass);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_NOTCLASS);      /* Return Fail */
    }

/* --- Read Previous Reset Report Status(1 Byte) --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPositionPrev, pTmpBuff, sizeof(UCHAR))) != 0) {
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if (pTmpBuff->uchResetStatus & TTL_NOTRESET) {
                                    /* Check Report Issued Flag */
		NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetFin()")
        return (TTL_NOT_ISSUED);    /* Return Not Issued Report */
    }

/* --- Read Current Financial Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, pTmpBuff, TTL_FIN_SIZE)) != 0) {
        return (sRetvalue);
    }

    PifGetDateTime(&DateTime);      /* Get Current Date & Time */
	TtlCopyNdateFromDateTime (&pTmpBuff->ToDate, &DateTime);
    pTmpBuff->uchResetStatus |= TTL_NOTRESET;                       /* Set Reset Disable status */
    CGGTotal = pTmpBuff->mlCGGTotal;                        /* Save Current Gross Group Total */
    Taxable[0] = pTmpBuff->Taxable[0].mlTaxableAmount;      /* Save Taxable Amount 1 */
    Taxable[1] = pTmpBuff->Taxable[1].mlTaxableAmount;      /* Save Taxable Amount 2 */
    Taxable[2] = pTmpBuff->Taxable[2].mlTaxableAmount;      /* Save Taxable Amount 3 */
    Taxable[3] = pTmpBuff->Taxable[3].mlTaxableAmount;      /* Save Taxable Amount 4 */

	//WRITE TOTAL SAVE FILE FOR FINANCIAL TOTAL
	if((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPositionPrev, &SavedTotal, TTL_FIN_SIZE)) != 0){
		return (sRetvalue);
	}
	TtlWriteSavFile (CLASS_TTLEODRESET, &SavedTotal, sizeof(SavedTotal), 0);

	/* --- Write From Current Financial Total To Previous Financial Total --- */
    if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPositionPrev, pTmpBuff, TTL_FIN_SIZE)) != 0) {
        return (sRetvalue);
    }

	/* --- Clear Current Financial Total ---- */
    memset(pTmpBuff, 0, TTL_FIN_SIZE);                               /* Clear Buffer */
	TtlCopyNdateFromDateTime (&pTmpBuff->FromDate, &DateTime);       /* Set Start Date & Time */
    pTmpBuff->mlCGGTotal = CGGTotal;     /* Save Current Gross Group Total */
    if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT2) == 0) {  /* Check Taxable Reset Enable MDC */
        pTmpBuff->Taxable[0].mlTaxableAmount = Taxable[0];  /* ReStore Taxable Amount 1 */
        pTmpBuff->Taxable[1].mlTaxableAmount = Taxable[1];  /* ReStore Taxable Amount 2 */
        pTmpBuff->Taxable[2].mlTaxableAmount = Taxable[2];  /* ReStore Taxable Amount 3 */
        pTmpBuff->Taxable[3].mlTaxableAmount = Taxable[3];  /* ReStore Taxable Amount 4 */
    }
    return (TtlWriteFile(hsTtlBaseHandle, ulPosition, pTmpBuff, TTL_FIN_SIZE)); /* Write Financial Total */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTresetHour(UCHAR uchClass, TTLCSHOURLY *pTmpBuff)
*
*    Input:     UCHAR uchClass          - Minor Class
*               TTLCSHOURLY *pTmpBuff   - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Hourly Total 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the Hourly Total.  V3.3
*
*============================================================================
*/
SHORT TtlTresetHour(UCHAR uchClass, TTLCSHOURLY *pTmpBuff)
{
    ULONG  ulPosition;         /* Save Seek Position */
    ULONG  ulPositionPrev;     /* Save Seek Position */
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    SHORT    sRetvalue;
    USHORT  usOffset;
	TTLCSHOURLY SavedTotal = {0};
	UCHAR	uchSavedIndex = 0;
    
    if (hsTtlBaseHandle < 0) {      /* Check Base Total File Handle */
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(hsTtlBaseHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);   /* Return File Handle Error */
    }
    
/* --- Set Reset Total Offset --- */
    switch (uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        ulPosition = TTL_HOUR_DAYCURR;
        ulPositionPrev = TTL_HOUR_DAYCURR + TTL_HOUR_SIZE;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
        ulPosition = TTL_HOUR_PTDCURR;
        ulPositionPrev = TTL_HOUR_PTDCURR + TTL_HOUR_SIZE;
        break;

    default:
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_CLASS);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), uchClass);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_NOTCLASS);          /* Return Fail */
    }

	/* --- Read Previous Reset Report Status(1 Byte) --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPositionPrev, pTmpBuff, sizeof(UCHAR))) != 0) {
        return (sRetvalue);          /* Rteurn Error Status */
    }
    if (pTmpBuff->uchResetStatus & TTL_NOTRESET) {    /* Check Report Issued Flag */
		NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetHour()")
        return (TTL_NOT_ISSUED);    /* Return Not Issued Report */
    }

    /* read and write only hourly total header at first, V3.3 */
    usOffset = TTL_STAT_SIZE;   /* reset status + from date + to date */

	/* --- Read Current Hourly Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, pTmpBuff, TTL_STAT_SIZE)) != 0) {
        return (sRetvalue);
    }
/*
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, (ULONG)ulPosition, 
                                 (VOID *)pTmpBuff, TTL_HOUR_SIZE)) != 0) {
        return (sRetvalue);
    }
*/
    PifGetDateTime(&DateTime);      /* Get Current Date & Time */
	TtlCopyNdateFromDateTime (&pTmpBuff->ToDate, &DateTime);
    pTmpBuff->uchResetStatus |= TTL_NOTRESET;
                                    /* Set Reset Disable status */
	/* --- Write From Current Hourly Total To Previous Hourly Total --- */
    if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPositionPrev, pTmpBuff, TTL_STAT_SIZE)) != 0) {
        return (sRetvalue);
    }
/*
    if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, (ULONG)ulPositionPrev, 
                                  (VOID *)pTmpBuff, TTL_HOUR_SIZE)) != 0) {
        return (sRetvalue);
    }
*/
	/* --- Clear Current Hourly Total ---- */
    memset(pTmpBuff, 0, TTL_STAT_SIZE);                              /* Clear Buffer */
	TtlCopyNdateFromDateTime (&pTmpBuff->FromDate, &DateTime);
    if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPosition, pTmpBuff, TTL_STAT_SIZE)) != 0) {
        return (sRetvalue);
    }

	if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPositionPrev, &SavedTotal, TTL_STAT_SIZE)) != 0)
	{
		return (sRetvalue);
	}

    ulPosition += TTL_STAT_SIZE;
    ulPositionPrev += TTL_STAT_SIZE;

    /* read and write each hourly total block, V3.3 */
    while(usOffset < TTL_HOUR_SIZE) {
        /* read from current total */
        if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, pTmpBuff, sizeof(HOURLY))) != 0) {
            return (sRetvalue);
        }

		if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPositionPrev, &SavedTotal.Total[uchSavedIndex], sizeof(HOURLY))) != 0)
		{
			return (sRetvalue);
		}
        /* write to previous total */
        if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPositionPrev, pTmpBuff, sizeof(HOURLY))) != 0) {
            return (sRetvalue);
        }

        /* clear current total */
        memset(pTmpBuff, 0, sizeof(HOURLY));
        if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPosition, pTmpBuff, sizeof(HOURLY))) != 0) {
            return (sRetvalue);
        }

        ulPosition     += sizeof(HOURLY);
        ulPositionPrev += sizeof(HOURLY);
        usOffset       += sizeof(HOURLY);
		uchSavedIndex ++;
    }

	//WRITE TOTAL SAVE FILE FOR HOURLY before it gets overwritten
	TtlWriteSavFile(CLASS_TTLHOURLY, &SavedTotal, sizeof(SavedTotal),0);

    return (sRetvalue);
}
/*
*============================================================================
**Synopsis:     SHORT TtlTresetDept(UCHAR uchClass, VOID *pTmpBuff)
*
*    Input:     UCHAR uchClass      - Minor Class
*               VOID *pTmpBuff      - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Dept Total File 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the Dept Total File.
*
*============================================================================
*/
SHORT TtlTresetDept(UCHAR uchClass, VOID *pTmpBuff)
{
    TTLCSDEPTHEAD    *pDepthd = pTmpBuff;    /* Pointer of Read Buffer for Dept File Header */
    TTLCSDEPTMISC    *pDeptMisc = pTmpBuff;  /* Read Buffer for Dept Misc */

    USHORT  usMaxDeptNo;         /* Save Dept Max Number */
    ULONG   ulDeptTotalSize;     /* Save Dept Total Size */
    ULONG   ulPositionMsc;      /* Save Seek Position */
    ULONG   ulPositionMscPrev;  /* Save Seek Position */
    ULONG   ulPositionIndPrev;  /* Save Seek Position */
    ULONG   ulPositionBlkPrev;  /* Save Seek Position */
    ULONG   ulPositionEmpPrev;  /* Save Seek Position */
    ULONG   ulPositionTtlPrev;  /* Save Seek Position */
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    SHORT    sRetvalue; 

    if (hsTtlDeptHandle < 0) {       /* Check Dept Total File Handle */
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(hsTtlDeptHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR); /* Return File Handle Error */
    }

	/* --- Read Dept File Header --- */
    if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {   /* Read Dept File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }

	/* --- Set Read Total Offset --- */
    switch(uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
                                    /* Previous Daily File */
        ulPositionMsc = pDepthd->ulCurDayMiscOff;
        ulPositionMscPrev = pDepthd->ulPreDayMiscOff;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
                                    /* Previous PTD File */
        if (pDepthd->ulCurPTDMiscOff == 0L) {  /* Check Exist PTD  Total File */
            return (TTL_NOTPTD);
        }
        ulPositionMsc = pDepthd->ulCurPTDMiscOff;
        ulPositionMscPrev = pDepthd->ulPrePTDMiscOff;
        break;

    default:
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_CLASS);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), uchClass);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_NOTCLASS);
    }
    usMaxDeptNo = pDepthd->usMaxDept;        /* Set Number of Max Dept */

	/* --- Read Previous Dept Misc Table --- */
    if ((sRetvalue = TtlDeptMiscRead(ulPositionMscPrev, pDeptMisc)) != 0) {    /* Read Dept Misc Total */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if (pDeptMisc->uchResetStatus & TTL_NOTRESET) {                            /* Check Report Issued Flag */
		NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetDept()")
        return (TTL_NOT_ISSUED);    /* Return Not Issued Report */
    }

	//WRITE TOTAL SAVE FILE FOR DEPARTMENT before it gets overwritten
    ulDeptTotalSize = (ULONG)((sizeof(TTLCSDEPTINDX) * (ULONG)usMaxDeptNo) + (sizeof(USHORT) * (ULONG)usMaxDeptNo) + TTL_DEPTEMP_SIZE + (sizeof(TOTAL) * (ULONG)usMaxDeptNo));  /* Caluculate Dept Total Size */
	
	/* --- Previous Dept Total File Initialize --- */
    memset(pDeptMisc, 0, TTL_DEPTMISC_SIZE);                      /* Clear Buffer */
    PifGetDateTime(&DateTime);                                    /* Get Current Date & Time */
	TtlCopyNdateFromDateTime (&pDeptMisc->FromDate, &DateTime);   /* Set Start Date & Time */
    if ((sRetvalue = TtlDeptMiscWrite(ulPositionMscPrev, pDeptMisc)) != 0) {  /* Write Previous Dept Misc Table */
        return (sRetvalue);         /* Return Error Status */
    }
    

    TtlNullWrite(hsTtlDeptHandle, ulDeptTotalSize, pTmpBuff, TTL_TMPBUF_SIZE);   /* Set NULL Code Previous Dept File */

	/* --- Read Current Dept Misc Table --- */
    if ((sRetvalue = TtlDeptMiscRead(ulPositionMsc, pDeptMisc)) != 0) {    /* Read Dept Misc Total */
        return (sRetvalue);         /* Rteurn Error Status */
    }

	/* --- Set Current Dept Misc Table --- */
	TtlCopyNdateFromDateTime (&pDeptMisc->ToDate, &DateTime);               /* Set End Date & Time */
    pDeptMisc->uchResetStatus |= TTL_NOTRESET;                              /* Set Reset Disable status */
    if ((sRetvalue = TtlDeptMiscWrite(ulPositionMsc, pDeptMisc)) != 0) {    /* Write Current Dept Misc Table */
        return (sRetvalue);         /* Return Error Status */
    }

	/* --- Read Dept File Header --- */
    if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {        /* Read Dept File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }

	/* --- Set Dept File Header Data --- */
    switch(uchClass) {

    /** --- Change Dept Daily Current <-> Previous Offset --- **/ 
    case CLASS_TTLCURDAY:           /* Current Daily File */
                                    /* Previous Daily File */
        ulPositionMscPrev = pDepthd->ulPreDayMiscOff;
        ulPositionIndPrev = pDepthd->ulPreDayIndexOff;
        ulPositionBlkPrev = pDepthd->ulPreDayBlockOff;
        ulPositionEmpPrev = pDepthd->ulPreDayEmptOff;
        ulPositionTtlPrev = pDepthd->ulPreDayTotalOff;

        pDepthd->usPreDayDept = pDepthd->usCurDayDept;          /* Set Number of Previous Daily Dept */
        pDepthd->ulPreDayMiscOff = pDepthd->ulCurDayMiscOff;       /* Set Daily Previous Dept Misc Total Offset */
        pDepthd->ulPreDayIndexOff = pDepthd->ulCurDayIndexOff;     /* Set Daily Previous Dept Index Offset */
        pDepthd->ulPreDayBlockOff = pDepthd->ulCurDayBlockOff;     /* Set Daily Previous Dept Block Offset */
        pDepthd->ulPreDayEmptOff = pDepthd->ulCurDayEmptOff;       /* Set Daily Previous Dept Empty Offset */
        pDepthd->ulPreDayTotalOff = pDepthd->ulCurDayTotalOff;     /* Set Daily Previous Dept Total Offset */

        pDepthd->usCurDayDept = 0;                              /* Set Number of Current Daily Dept */
        pDepthd->ulCurDayMiscOff = ulPositionMscPrev;             /* Set Daily Current Dept Misc Total Offset */
        pDepthd->ulCurDayIndexOff = ulPositionIndPrev;            /* Set Daily Current Dept Index Offset */
        pDepthd->ulCurDayBlockOff = ulPositionBlkPrev;            /* Set Daily Current Dept Block Offset */
        pDepthd->ulCurDayEmptOff = ulPositionEmpPrev;             /* Set Daily Current Dept Empty Offset */
        pDepthd->ulCurDayTotalOff = ulPositionTtlPrev;            /* Set Daily Current Dept Total Offset */

        break;


    /** --- Change Dept Daily Current <-> Previous Offset --- **/ 
    case CLASS_TTLCURPTD:           /* Current PTD File */
                                    /* Previous PTD File */
        ulPositionMscPrev = pDepthd->ulPrePTDMiscOff;
        ulPositionIndPrev = pDepthd->ulPrePTDIndexOff;
        ulPositionBlkPrev = pDepthd->ulPrePTDBlockOff;
        ulPositionEmpPrev = pDepthd->ulPrePTDEmptOff;
        ulPositionTtlPrev = pDepthd->ulPrePTDTotalOff;
        pDepthd->usPrePTDDept = pDepthd->usCurPTDDept;          /* Set Number of Previous PTD Dept */
        pDepthd->ulPrePTDMiscOff = pDepthd->ulCurPTDMiscOff;      /* Set Daily Previous Dept Misc Total Offset */
        pDepthd->ulPrePTDIndexOff = pDepthd->ulCurPTDIndexOff;    /* Set Daily Previous Dept Index Offset */
        pDepthd->ulPrePTDBlockOff = pDepthd->ulCurPTDBlockOff;    /* Set Daily Previous Dept Block Offset */
        pDepthd->ulPrePTDEmptOff = pDepthd->ulCurPTDEmptOff;      /* Set Daily Previous Dept Empty Offset */
        pDepthd->ulPrePTDTotalOff = pDepthd->ulCurPTDTotalOff;    /* Set Daily Previous Dept Total Offset */

        pDepthd->usCurPTDDept = 0;                              /* Set Number of Current PTD Dept */
        pDepthd->ulCurPTDMiscOff = ulPositionMscPrev;             /* Set PTD Current Dept Misc Total Offset */
        pDepthd->ulCurPTDIndexOff = ulPositionIndPrev;            /* Set PTD Current Dept Index Offset */
        pDepthd->ulCurPTDBlockOff = ulPositionBlkPrev;            /* Set PTD Current Dept Block Offset */
        pDepthd->ulCurPTDEmptOff = ulPositionEmpPrev;             /* Set PTD Current Dept Empty Offset */
        pDepthd->ulCurPTDTotalOff = ulPositionTtlPrev;            /* Set PTD Current Dept Total Offset */

        break;

/*  default:   Not Used */
    }
/* --- Write Dept File Header --- */
    return (TtlDeptHeadWrite(pDepthd));/* Write Dept File Header */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTresetCpn(UCHAR uchClass, VOID *pTmpBuff)
*
*    Input:     UCHAR uchClass      - Minor Class
*               VOID *pTmpBuff      - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Coupon Total 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the Coupon Total.               R3.0
*
*============================================================================
*/
SHORT TtlTresetCpn(UCHAR uchClass, VOID *pTmpBuff)
{
     TTLCSCPNHEAD    *pCpnhd = pTmpBuff;     /* Pointer of Coupon File Header Read Buffer */
    TTLCSCPNMISC   *pCpnMisc = pTmpBuff;   /* Pointer of Coupon Misc Total Read Buffer */
    ULONG  ulPosition;         /* Save Seek Position */
    ULONG  ulPositionPrev;     /* Save Seek Position */
    USHORT  usCpnFileSize;      /* Coupon Total File Size */
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    SHORT    sRetvalue;

    if (hsTtlCpnHandle < 0) {       /* Check Coupon Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CPN_FILE_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(hsTtlCpnHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
     }
    
/* --- Read Coupon File Header --- */
    if ((sRetvalue = TtlCpnHeadRead(pCpnhd)) != 0) {    /* Read Coupon File Header */
        return (sRetvalue);          /* Rteurn Error Status */
    }

/* --- Set Read Total Offset --- */
    switch (uchClass) {
     case CLASS_TTLCURDAY:          /* Current Daily File */
        ulPosition = pCpnhd->usCurDayTtlOff;
        ulPositionPrev = pCpnhd->usPreDayTtlOff;
        usCpnFileSize = TTL_CPNMISC_SIZE + (pCpnhd->usMaxCpn * sizeof(TOTAL));
        break;

    case CLASS_TTLCURPTD:                    /* Current PTD File */
        if(pCpnhd->usCurPTDTtlOff == 0) {    /* Check Exist PTD Total File */
            return (TTL_NOTPTD);    /* Return Fail */
        }
        ulPosition = pCpnhd->usCurPTDTtlOff;
        ulPositionPrev = pCpnhd->usPrePTDTtlOff;
        usCpnFileSize = TTL_CPNMISC_SIZE + (pCpnhd->usMaxCpn * sizeof(TOTAL));
        break;

     default:
          return (TTL_NOTCLASS);
     }

	/* --- Read Previous Reset Report Status(1 Byte) --- */
    if ((sRetvalue = TtlCpnMiscRead(ulPositionPrev, pCpnMisc)) != 0) {
          return (sRetvalue);          /* Rteurn Error Status */
     }
    if (pCpnMisc->uchResetStatus & TTL_NOTRESET) {           /* Check Report Issued Flag */
		  NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetCpn()")
          return (TTL_NOT_ISSUED);    /* Return Not Issued Report */
     }

	/* --- Read All Current Coupon Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlCpnHandle, ulPosition, pTmpBuff, usCpnFileSize)) != 0) {
          return (sRetvalue);
    }
    PifGetDateTime(&DateTime);      /* Get Current Date & Time */
	TtlCopyNdateFromDateTime (&pCpnMisc->ToDate, &DateTime);  /* Set Reset Date & Time */
    pCpnMisc->uchResetStatus |= TTL_NOTRESET;   /* Set Reset Disable status */

	//WRITE TOTAL SAVE FILE FOR COUPON before it gets overwritten
	/* --- Write From Current Coupon Total To Previous Coupon Total --- */
    if ((sRetvalue = TtlWriteFile(hsTtlCpnHandle, ulPositionPrev, pTmpBuff, usCpnFileSize)) != 0) {
          return (sRetvalue);
    }

	{
		TTLCSCPNMISC *pMisc = pTmpBuff;              /* Daily Coupon Status, R3.0 */
		TOTAL        *pTotal = (VOID *)((TTLCSCPNMISC *)pTmpBuff + 1);  /* Coupon Total */

//		TtlWriteSavFile(CLASS_TTLCPN, pTmpBuff, usCpnFileSize, 0);
		TtlSavCpnSavFileBuff(pTotal, pCpnhd->usMaxCpn);
	}

	/* --- Clear Current Coupon Total ---- */
    memset(pTmpBuff, 0, usCpnFileSize);                         /* Clear Buffer */
	TtlCopyNdateFromDateTime (&pCpnMisc->FromDate, &DateTime);  /* Set Start Date & Time */
    return (TtlWriteFile(hsTtlCpnHandle, ulPosition, pTmpBuff, usCpnFileSize));
}
/*
*============================================================================
**Synopsis:     SHORT TtlTResetCas(TTLCASHIER *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLCASHIER *pTotal  - Minor Class, Cashier Number
*               VOID *pTmpBuff      - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Cashier Total File 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTINFILE       - Not IN FILE Cashier
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the Cashier Total File.
*
*============================================================================
*/
SHORT TtlTresetCas(TTLCASHIER *pTotal, VOID *pTmpBuff)
{
    TTLCSCASHEAD    *pCashd = pTmpBuff;    /* Pointer of Read Buffer for Cashier File Header */
    ULONG           ulPositionInd;      /* Save Seek Position */
    ULONG           ulPositionTtl;      /* Save Seek Position */
    ULONG           ulPositionIndPrev;  /* Save Seek Position */
    ULONG           ulPositionEmpPrev;  /* Save Seek Position */
    ULONG           ulPositionTtlPrev;  /* Save Seek Position */
    ULONG           ulIndOffset;

    DATE_TIME    DateTime;       /* Date & Time Work Area */
    TTLCASHIER   TtlCashierWork;         /* Work Buffer for Delete Cashier interface */

    UCHAR   uchMinorClass;
    USHORT  usCurCasNo = 0;         /* Save Numner of Current Cashier */
    USHORT  usPreCasNo = 0;         /* Save Numner of Previous Cashier */
    USHORT  usOffset = 0;           /* Offset, V3.3 */
    USHORT  usPrevOffset = 0;       /* Offset, V3.3 */
    SHORT   sCasCurState = 0;      /* Cashier Exist status */
    SHORT   sCasPreState = 0;      /* Cashier Exist status */
    SHORT    sRetvalue;

    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(hsTtlCasHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);    /* Return File Handle Error */
    }

	/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {    /* Read Cashier File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }

	/* --- Set Read Total Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
                                    /* Previous Daily File */
        uchMinorClass  = CLASS_TTLSAVDAY;
        usCurCasNo = pCashd->usCurDayCas;               /* Set Numner of Current Cashier */ 
        usPreCasNo = pCashd->usPreDayCas;               /* Set Numner of Previous Cashier */ 
        ulPositionInd = pCashd->ulCurDayIndexOff;       /* Set Current Daily Index Offset */
        ulPositionTtl = pCashd->ulCurDayTotalOff;       /* Set Current Daily Total Offset */
        ulPositionIndPrev = pCashd->ulPreDayIndexOff;   /* Set Previous Daily Index Offset */
        ulPositionEmpPrev = pCashd->ulPreDayEmptOff;    /* Set Previous Daily Empty Offset */
        ulPositionTtlPrev = pCashd->ulPreDayTotalOff;   /* Set Previous Daily Total Offset */
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File, V3.3 */
                                    /* Previous PTD File */
        if (pCashd->ulCurPTDIndexOff == 0L) {      /* Check Exist PTD  Total File */
            return (TTL_NOTPTD);
        }
        uchMinorClass  = CLASS_TTLSAVPTD;
        usCurCasNo = pCashd->usCurPTDCas;               /* Set Number of Current PTD Cashier */ 
        usPreCasNo = pCashd->usPrePTDCas;               /* Set Number of Previous PTD Cashier */
        ulPositionInd = pCashd->ulCurPTDIndexOff;       /* Set Current PTD Index Offset */
        ulPositionTtl = pCashd->ulCurPTDTotalOff;       /* Set Current PTD Total Offset */
        ulPositionIndPrev = pCashd->ulPrePTDIndexOff;   /* Set Previous PTD Index Offset */
        ulPositionTtlPrev = pCashd->ulPrePTDTotalOff;   /* Set Previous PTD Total Offset */ 
        break;

    default:
        return (TTL_NOTCLASS);
    }

	/* --- Search Previous Cashier Index Table --- */
    sCasPreState = TTL_NOTINFILE;      /* Set Not Exist Previous Cashier Status then if found change it */
    if (usPreCasNo) {
		TTLCSCASINDX    CasInd = {0};
        /* --- Search Cashier Index Table, V3.3 --- */
        CasInd.ulCasNo = pTotal->ulCashierNumber;
        sCasPreState = TtlCasISearch(ulPositionIndPrev, usPreCasNo, &CasInd, pTmpBuff, &ulIndOffset, TTL_SEARCH_EQU);   /* Search Cashier Index Table */
        if (sCasPreState != TTL_SUCCESS && sCasPreState != TTL_NOTINFILE) {
			/* Return Error Status if an error other than not in file */
            return (sCasPreState);         /* Return Error Status */
        }
        usPrevOffset = CasInd.usBlockNo;    /* set empty table block no offset */
    }

	/* --- Search Current Cashier Index Table --- */
    sCasCurState = TTL_NOTINFILE;          /* Set Not Exist Current Cashier Status then if found change it */
    if (usCurCasNo) {
		TTLCSCASINDX    CasInd = {0};
        /* --- Search Cashier Index Table, V3.3 --- */
        CasInd.ulCasNo = pTotal->ulCashierNumber;
        sCasCurState = TtlCasISearch(ulPositionInd, usCurCasNo, &CasInd, pTmpBuff, &ulIndOffset, TTL_SEARCH_EQU);    /* Search Cashier Index Table */
        if (sCasCurState != TTL_SUCCESS && sCasCurState != TTL_NOTINFILE) {
			/* Return Error Status if an error other than not in file */
            return (sCasCurState);         /* Return Error Status */
        }
        usOffset = CasInd.usBlockNo;    /* set empty table block no offset */
    }

    TtlCashierWork = *pTotal; /* Copy Major & Minor Class, Cashier Number */

	{
		TTLCSCASTOTAL   *pCasTtl = pTmpBuff;   /* Pointer of Cashier Total Read Buffer */ 

		/* --- Previous Cashier Control --- */
		if (sCasPreState == TTL_SUCCESS) {            /* Exist Previous Cashier Total */
			TTLCASHIER	    SavedTotal = {0};
			ulPositionTtlPrev = ulPositionTtlPrev + (sizeof(TTLCSCASTOTAL) * (ULONG)usPrevOffset);
			if ((sRetvalue = TtlCasTotalRead(ulPositionTtlPrev, pCasTtl)) != 0) {    /* Read Cashier Total File */
				return (sRetvalue);     /* Rteurn Error Status */
			}

			// make a copy of the current cashier total and save into the saved total backup file.
			SavedTotal.ulCashierNumber = pTotal->ulCashierNumber;
			memcpy(&SavedTotal.uchResetStatus, &pCasTtl->uchResetStatus, sizeof(TTLCSCASTOTAL));                //WRITE TOTAL SAVE FILE FOR CASHIER
			TtlWriteSavFile(CLASS_TTLCASHIER, &SavedTotal, sizeof(SavedTotal), 0);
			if (pCasTtl->uchResetStatus & TTL_NOTRESET) {                                                    /* Check Report Issued Flag */
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetCas()")
				return (TTL_NOT_ISSUED);      /* Return Fail */
			}
		} else {
			/* Not Exist Previous Cashier Total */
			if ((sRetvalue = TtlCasAssin(uchMinorClass, pTotal->ulCashierNumber, &ulPositionTtlPrev, pCasTtl)) != 0) {   /* Assin Previous New Cashier Total Area */
				return (sRetvalue); /* Rteurn Error Status */
			}
		}

		PifGetDateTime(&DateTime);  /* Get Current Date & Time */
		/* --- Current Cashier Control --- */
		if (sCasCurState == TTL_SUCCESS) {                                                     /* Exist Current Cashier Total */
			ulPositionTtl = ulPositionTtl + (sizeof(TTLCSCASTOTAL) * (ULONG)usOffset);
			if ((sRetvalue = TtlCasTotalRead(ulPositionTtl, pCasTtl)) != 0) {                           /* Read Cashier Total File */
				return (sRetvalue);     /* Rteurn Error Status */
			}

			TtlCopyNdateFromDateTime (&pCasTtl->ToDate, &DateTime);                   /* Set Reset Date & Time */
			pCasTtl->uchResetStatus |= TTL_NOTRESET;

			if ((sRetvalue = TtlWriteFile(hsTtlCasHandle, ulPositionTtlPrev, pCasTtl, sizeof(TTLCSCASTOTAL))) != 0) {  /* Write Cashier Total */
				return (sRetvalue);     /* Return Error Status */
			}

			TtlCashierWork.uchMinorClass = pTotal->uchMinorClass;    /* Set Current Minor Class */
			if ((sRetvalue = TtlTDeleteCas(&TtlCashierWork, pCasTtl)) != 0) {    /* Delete Current Daily Cashier */
				return (sRetvalue);     /* Return Error Status */
			}
		} else {
			/* Not Exist Current Cashier Total */
			memset(pCasTtl, 0, sizeof(TTLCSCASTOTAL));                           /* Clear Cashier Total */
			pCasTtl->uchResetStatus |= TTL_NOTRESET;
			TtlCopyNdateFromDateTime (&pCasTtl->FromDate, &DateTime);
			TtlCopyNdateFromDateTime (&pCasTtl->ToDate, &DateTime);

			if ((sRetvalue = TtlWriteFile(hsTtlCasHandle, ulPositionTtlPrev, pCasTtl, sizeof(TTLCSCASTOTAL))) != 0) {  /* Write Cashier Total */
				return (sRetvalue);     /* Return Error Status */
			}
		}
	}
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTDeleteCas(TTLCASHIER *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLCASHIER *pTotal  - Minor Class, Cashier Number
*               VOID *pTmpBuff      - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Deletting Cashier Total File 
*               TTL_NOTINFILE       - Cashier Not In File
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Deletes the Cashier Total File.
*
*============================================================================
*/
SHORT TtlTDeleteCas(TTLCASHIER *pTotal, VOID *pTmpBuff)
{
    TTLCSCASHEAD    *pCashd = pTmpBuff;    /* Pointer of Read Buffer for Cashier File Header */
	TTLCSCASINDX    CasInd = {0};

    USHORT  usMaxNo;
    USHORT  usCasNo;            /* Save Number of Cashier */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionEmp;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */
    USHORT  usOffset;           /* Offset, V3.3 */
    ULONG  ulIndOffset;
    SHORT   sRetvalue;

    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)abs(hsTtlCasHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);  /* Return File Handle Error */
    }

/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {   
                                    /* Read Cashier File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }

/* --- Set Read Total Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
                                    /* Previous Daily File */
        if (pCashd->usCurDayCas == 0) {   /* Check Number of Current Cashier */
            return (TTL_NOTINFILE); /* Return Not In File */
        }
        usCasNo = pCashd->usCurDayCas;
        ulPositionInd = pCashd->ulCurDayIndexOff;   /* Set Current Daily Index Offset */ 
        ulPositionEmp = pCashd->ulCurDayEmptOff;    /* Set Current Daily Empty Offset */
        ulPositionTtl = pCashd->ulCurDayTotalOff;   /* Set Current Daily Total Offset */
        break;

    case CLASS_TTLSAVDAY:                /* Previous Daily File */
        if (pCashd->usPreDayCas == 0) {  /* Check Number of Previous Cashier */
            return (TTL_NOTINFILE);      /* Return Not In File */
        }
        usCasNo = pCashd->usPreDayCas;
        ulPositionInd = pCashd->ulPreDayIndexOff;   /* Set Previous Daily Index Offset */ 
        ulPositionEmp = pCashd->ulPreDayEmptOff;    /* Set Previous Daily Empty Offset */
        ulPositionTtl = pCashd->ulPreDayTotalOff;   /* Set Previous Daily Total Offset */
        break;

    case CLASS_TTLCURPTD:                      /* Current PTD File, V3.3 */
        if (pCashd->ulCurPTDIndexOff == 0L) {  /* Check Exist PTD  Total File */
            return (TTL_NOTPTD);               /* Return Not Exist PTD */
        }
        if (pCashd->usCurPTDCas == 0) {  /* Check Number of Current PTD Cashier */
            return (TTL_NOTINFILE);      /* Return Not In File */
        }
        usCasNo = pCashd->usCurPTDCas;               /* Set Number of Current PTD Cashier */
        ulPositionInd = pCashd->ulCurPTDIndexOff;    /* Set Currnet PTD Index Offset */
        ulPositionEmp = pCashd->ulCurPTDEmptOff;     /* Set Current PTD Empty Offset */
        ulPositionTtl = pCashd->ulCurPTDTotalOff;    /* Set Current PTD Total Offset */
        break;

    case CLASS_TTLSAVPTD:                       /* Previous PTD File, V3.3 */
        if (pCashd->ulPrePTDIndexOff == 0L) {   /* Check Exist PTD  Total File */
            return (TTL_NOTPTD);
        }
        if (pCashd->usPrePTDCas == 0) {         /* Check Number of Previous PTD Cashier */
            return (TTL_NOTINFILE);             /* Return Not In File */
        }
        usCasNo = pCashd->usPrePTDCas;               /* Set Number of Previous PTD Cashier */
        ulPositionInd = pCashd->ulPrePTDIndexOff;    /* Set Previous PTD Index Offset */
        ulPositionEmp = pCashd->ulPrePTDEmptOff;     /* Set Previous PTD Empty Offset */
        ulPositionTtl = pCashd->ulPrePTDTotalOff;    /* Set Previous PTD Total Offset */
        break;

    default:
        return (TTL_NOTCLASS);
    }
    usMaxNo = pCashd->usMaxCas;
/* --- Search & Erase Cashier Index Table --- */
    CasInd.ulCasNo = pTotal->ulCashierNumber;
    if ((sRetvalue = TtlCasISearch(ulPositionInd, usCasNo, &CasInd, pTmpBuff, &ulIndOffset, TTL_SEARCH_EQU)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    if ((sRetvalue = TtlCasIErase(ulPositionInd, usCasNo, pTmpBuff, ulIndOffset)) != 0) {
                                    /* Update Index Table */
        return (sRetvalue);         /* Return Error Status */
    }
    usOffset = CasInd.usBlockNo;    /* set empty table block no. */

/* --- Erase Cashier Empty Table --- */
    if ((sRetvalue = TtlCasEmptyRead(ulPositionEmp, pTmpBuff)) != 0) {
                                    /* Read Cashier Empty Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if ((sRetvalue = TtlEmptyTableUpdate(pTmpBuff, usMaxNo, usOffset, TTL_EMPBIT_RESET)) != 0) {
                                    /* Erase Cashier Empty Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if ((sRetvalue = TtlCasEmptyWrite(ulPositionEmp, pTmpBuff)) != 0) {
                                    /* Write Cashier Empty Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }

/* --- Clear Cashier Total ---- */
    memset(pTmpBuff, 0, TTL_CASTTL_SIZE);   
                                    /* Clear Buffer */
    ulPositionTtl = ulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)usOffset);
    if ((sRetvalue = TtlWriteFile(hsTtlCasHandle, ulPositionTtl, pTmpBuff, TTL_CASTTL_SIZE)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {   /* Read Cashier File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }
/* --- Update Cashier Number --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
                                    /* Previous Daily File */
        pCashd->usCurDayCas = pCashd->usCurDayCas - 1;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        pCashd->usPreDayCas = pCashd->usPreDayCas - 1;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File, V3.3 */
        pCashd->usCurPTDCas  = pCashd->usCurPTDCas - 1;
        break;

    case CLASS_TTLSAVPTD:           /* Previous PTD File */
        pCashd->usPrePTDCas = pCashd->usPrePTDCas - 1;
        break;

/*  default:  Not Used */
    }

/* --- Update Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadWrite(pCashd)) != 0) {       /* Write Cashier File Header */
        return (sRetvalue);                                 /* Rteurn Error Status */
    }

    return (TTL_SUCCESS);           /* Return Success */
}
/****************** New Functions (Release 3.1) BEGIN **********************/
/*
*============================================================================
**Synopsis:     TtlTreadSerTime(TTLSERTIME *pTotal, TTLCSSERTIME *pTmpBuff)
*
*    Input:     TTLSERTIME   *pTotal    - Major & Minor Class
*               TTLCSSERTIME *pTmpBuff  - Work Buffer Pointer
*
*   Output:     TTLSERTIME   *pTotal    - Service Time Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Service Time Total File 
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Service Time Total File.
*
*============================================================================
*/
SHORT TtlTreadSerTime(TTLSERTIME *pTotal)
{
    ULONG    ulPosition = 0;         /* Seek Position */
    SHORT    sRetvalue;
    USHORT   i;

    if ((sRetvalue = TtlGetSerTimeFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {    /* Get Service Time Data Position */
        return (sRetvalue);
    }

    /* --- Read Service Time Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, &pTotal->uchResetStatus, TTL_STAT_SIZE)) != 0) {    /* Read Service Time Total File */
		memset (&pTotal->uchResetStatus, 0, TTL_STAT_SIZE);     // ensure memory is set to a known value on error return.
        return (sRetvalue);         /* Return Error Status */
    }

    ulPosition += TTL_STAT_SIZE;

    /* Copy Service Total, V3.3 */
    for (i = 0; i < STD_MAX_HOURBLK; i++) {
        if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, &pTotal->ServiceTime[i][0], sizeof(TOTAL) * 3)) != 0) {    /* Read Hourly Total File */
            return (sRetvalue);         /* Return Error Status */
        } 

        ulPosition += sizeof(TOTAL) * 3;
    }

	return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTresetSerTime(UCHAR uchClass, TTLCSSERTIME *pTmpBuff)
*
*    Input:     UCHAR        uchClass   - Minor Class
*               TTLCSSERTIME *pTmpBuff  - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Service Time
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function resets the Service Time Total
*
*============================================================================
*/
SHORT TtlTresetSerTime(UCHAR uchClass, TTLCSSERTIME *pTmpBuff)
{
    ULONG        ulPosition;         /* Save Seek Position */
    ULONG        ulPositionPrev;     /* Save Seek Position */
    DATE_TIME    DateTime;       /* Date & Time Work Area */
	TTLCSSERTIME SavedTotal = {0};
    SHORT        sRetvalue;
	USHORT	     usSavedIndex = 0;

    if (hsTtlSerTimeHandle < 0) {      /* Check Service Time Total File Handle */
		TtlWriteSavFile (CLASS_TTLSERVICE, &SavedTotal, sizeof(SavedTotal), 0);
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(hsTtlSerTimeHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);  /* Return File Handle Error */
    }

    /* --- Set Reset Total Offset --- */
    switch (uchClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        ulPosition     = TTL_SERTIME_DAYCURR;
        ulPositionPrev = TTL_SERTIME_DAYCURR + TTL_SERTTL_SIZE;
        break;

    case CLASS_TTLCURPTD:           /* Current PTD File */
        ulPosition     = TTL_SERTIME_PTDCURR;
        ulPositionPrev = TTL_SERTIME_PTDCURR + TTL_SERTTL_SIZE;
        break;

    default:
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_CLASS);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), uchClass);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_NOTCLASS);      /* Return Fail */
    }

    /* --- Read Previous Reset Report Status(1 Byte) --- */
    if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPositionPrev, pTmpBuff, sizeof(UCHAR))) != 0) {
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if (pTmpBuff->uchResetStatus & TTL_NOTRESET) {    /* Check Report Issued Flag */
		NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetSerTime()")
        return (TTL_NOT_ISSUED);    /* Return Not Issued Report */
    }

    /* --- Read Current Service Time Total --- */
    /* read and write only hourly total header at first, V3.3 */
    /* reset status + from date + to date */
    if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, pTmpBuff, TTL_STAT_SIZE)) != 0) {
        return (sRetvalue);
    }

	PifGetDateTime(&DateTime);      /* Get Current Date & Time */
	TtlCopyNdateFromDateTime (&pTmpBuff->ToDate, &DateTime);       /* TtlTresetSerTime() Set Reset Date & Time */
    pTmpBuff->uchResetStatus |= TTL_NOTRESET;                      /* TtlTresetSerTime() Set Reset Disable status */

	SavedTotal.ToDate = pTmpBuff->ToDate;
	SavedTotal.FromDate = pTmpBuff->FromDate;

	/* --- Write From Current Service Time Total To Previous --- */
    if ((sRetvalue = TtlWriteFile(hsTtlSerTimeHandle, ulPositionPrev, pTmpBuff, TTL_STAT_SIZE)) != 0) {
        return (sRetvalue);
    }

	/* --- Clear Current Hourly Total ---- */
    memset(pTmpBuff, 0, TTL_STAT_SIZE);                           /* Clear Buffer */
	TtlCopyNdateFromDateTime (&pTmpBuff->FromDate, &DateTime);    /* Set Reset Date & Time */
    if ((sRetvalue = TtlWriteFile(hsTtlSerTimeHandle, ulPosition, pTmpBuff, TTL_STAT_SIZE)) != 0) {
        return (sRetvalue);
    }

    ulPosition += TTL_STAT_SIZE;
    ulPositionPrev += TTL_STAT_SIZE;

    /* read and write each service total block, V3.3 */
    for (usSavedIndex = 0; usSavedIndex < 48; usSavedIndex++) {
		TOTAL  ServiceTimeTotal[3];

        /* read from current total */
        if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, ServiceTimeTotal, sizeof(TOTAL)*3)) != 0) {
            return (sRetvalue);
        }
		if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPositionPrev, &SavedTotal.ServiceTime[usSavedIndex], sizeof(TOTAL)*3)) != 0)
		{
			return (sRetvalue);
		}
        /* write to previous total */
        if ((sRetvalue = TtlWriteFile(hsTtlSerTimeHandle, ulPositionPrev, ServiceTimeTotal, sizeof(TOTAL)*3)) != 0) {
            return (sRetvalue);
        }

        /* clear current total */
        memset(ServiceTimeTotal, 0, sizeof(ServiceTimeTotal));
        if ((sRetvalue = TtlWriteFile(hsTtlSerTimeHandle, ulPosition, ServiceTimeTotal, sizeof(TOTAL)*3)) != 0) {
            return (sRetvalue);
        }

        ulPosition     += sizeof(TOTAL)*3;
        ulPositionPrev += sizeof(TOTAL)*3;
    }

	//WRITE TOTAL SAVE FILE FOR HOURLY before it gets overwritten
	TtlWriteSavFile (CLASS_TTLSERVICE, &SavedTotal, sizeof(SavedTotal), 0);

	return (sRetvalue);
}

/*
*============================================================================
**Synopsis:     SHORT TtlTreadIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLREGFIN *pTotal   - Major & Minor Class, Terminal Number
*               VOID      *pTmpBuff - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     TTLREGFIN *pTotal   - Individual Financial Data
*
** Return:      TTL_SUCCESS         - Successed the Reading Individual Financial
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTINFILE       - Cashier Not Exist
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function reads the Individual Financial.
*
*============================================================================
*/
SHORT TtlTreadIndFin(TTLREGFIN *pTotal)
{
    SHORT    sRetValue;

	NHPOS_ASSERT(sizeof(TTLREGFIN) >= sizeof(TTLCSREGFIN) + offsetof(TTLREGFIN,uchResetStatus));

    sRetValue = TtlreadIndFin(pTotal->uchMinorClass, pTotal->usTerminalNumber, &pTotal->uchResetStatus);
    return (sRetValue);
}

/*
*============================================================================
**Synopsis:     SHORT TtlTresetIndFin(USHORT usMDCBit,
*                                       TTLREGFIN *pTotal, VOID *pTmpBuff)
*
*    Input:     USHORT  usMDCBit    -   indicate reset at EOD or AC233
*               TTLREGFIN *pTotal   - Minor Class, Terminal Number
*               VOID      *pTmpBuff - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Reseting Individual Financial
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_NOTINFILE       - Not IN FILE Terminal
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Resets the Individual Financail File.
*
*============================================================================
*/
SHORT TtlTresetIndFin(USHORT usMDCBit, TTLREGFIN *pTotal, VOID *pTmpBuff)
{
    TTLCSINDHEAD    *pIndHead = pTmpBuff;  /* Pointer of Read Buffer for File Header */
    TTLCSREGFIN     *pIndTtl = pTmpBuff;   /* Pointer of Individual Financial Total */
	TTLCSREGFIN		SavedTotal = {0};

    USHORT  usMaxNo;
    USHORT  usCurTermNo;        /* Save Numner of Current Termianl */
    USHORT  usPreTermNo;        /* Save Numner of Previous Terminal */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */
    ULONG   ulPositionIndPrev;  /* Save Seek Position */
    USHORT  usPositionEmpPrev;  /* Save Seek Position */
    ULONG   ulPositionTtlPrev;  /* Save Seek Position */
    UCHAR   uchOffset = 0;          /* Offset */
    UCHAR   uchPrevOffset = 0;      /* Offset */
    UCHAR   uchNumber;
    USHORT  usTermCurState;     /* Terminal Exist status */
    USHORT  usTermPreState;     /* Terminal Exist status */
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    UCHAR   uchWork[4];         /* Work Buffer for Delete Termianl interface */
    SHORT   sRetvalue;
    DCURRENCY   lNetTtlSave;
    D13DIGITS   CGGTotal;       /* Save Current Gross Group Total Area */
    D13DIGITS   Taxable[STD_TAX_ITEMIZERS_MAX];     /* Save Taxable Amount Area */

    if (hsTtlIndFinHandle < 0) {            /* Check Individual Financial Handle */
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(hsTtlIndFinHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);       /* Return File Handle Error */
    }

    /* --- Read File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {  /* Read File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    usMaxNo = pIndHead->usMaxTerminal;

    /* --- Set Read Total Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
                                    /* Previous Daily File */
        usCurTermNo       = pIndHead->usCurDayTerminal;    /* Set Number of Current Terminal */ 
        usPreTermNo       = pIndHead->usPreDayTerminal;    /* Set Number of Previous Terminal */ 
        ulPositionInd     = pIndHead->usCurDayIndexOff;    /* Set Current Daily Index Offset */
        ulPositionTtl     = pIndHead->usCurDayTotalOff;    /* Set Current Daily Total Offset */
        ulPositionIndPrev = pIndHead->usPreDayIndexOff;    /* Set Previous Daily Index Offset */
        usPositionEmpPrev = pIndHead->usPreDayEmptOff;     /* Set Previous Daily Empty Offset */
        ulPositionTtlPrev = pIndHead->usPreDayTotalOff;    /* Set Previous Daily Total Offset */
        break;

    default:
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_CLASS);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), pTotal->uchMinorClass);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_NOTCLASS);
    }

    /* --- Search Previous Terminal Index Table --- */
    if (usPreTermNo) {
        usTermPreState = 0;         /* Set Exist Previous Terminal Status */
        if ((sRetvalue = TtlIndFinIndexRead(ulPositionIndPrev, usPreTermNo, pTmpBuff)) != 0) {  /* Read Terminal Index Table */
            return (sRetvalue);     /* Rteurn Error Status */
        }

        if(TtlIndFinISearch(pTmpBuff, usPreTermNo, pTotal->usTerminalNumber, &uchPrevOffset, &uchNumber)) {
                                    /* Search Terminal Index Table */
            usTermPreState = 1;     /* Set Not Exist Previous Terminal Status */
        }
    } else {
        usTermPreState = 1;         /* Set Not Exist Previous Terminal Status */
    }

    /* --- Search Current Terminal Index Table --- */
    if (usCurTermNo) {
        usTermCurState = 0;         /* Set Exist Current Terminal Status */
        if ((sRetvalue = TtlIndFinIndexRead(ulPositionInd, usCurTermNo, pTmpBuff)) != 0) { /* Read Terminal Index Table */
            return (sRetvalue);     /* Rteurn Error Status */
        }

        if(TtlIndFinISearch(pTmpBuff, usCurTermNo, pTotal->usTerminalNumber, &uchOffset, &uchNumber)) { /* Search Terminal Index Table */
            usTermCurState = 1;         /* Set Not Exist Current Terminal Status */
        }
    } else {
        usTermCurState = 1;             /* Set Not Exist Current Terminal Status */
    }

    memcpy(&uchWork[0], pTotal, 4);     /* Copy Major & Minor Class, Terminal Number */

    /* --- Previous Individual Financial Control --- */
    if (usTermPreState == 0) {
        /* Exist Previous Terminal Total */
        /* Read Individual Financial Total File */
        ulPositionTtlPrev = ulPositionTtlPrev + (TTL_INDTTL_SIZE * (ULONG)uchPrevOffset);
        if ((sRetvalue = TtlIndFinTotalRead(ulPositionTtlPrev, pIndTtl)) != 0) {
			PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_RESET_READ);
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(sRetvalue));
			PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
            return (sRetvalue);     /* Rteurn Error Status */
        }

        if (pIndTtl->uchResetStatus & TTL_NOTRESET) {   /* Check Report Issued Flag */
			NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTresetIndFin()")
            return (TTL_NOT_ISSUED);      /* Return Fail */
        }
    } else {
                                    /* Not Exist Previous Terminal Total */
        if ((sRetvalue = TtlIndFinAssin(CLASS_TTLSAVDAY, pTotal->usTerminalNumber, &ulPositionTtlPrev, pTmpBuff)) != 0) {
                                    /* Assin Previous New Terminal Total Area */
            return (sRetvalue); /* Rteurn Error Status */
        }
    }

    PifGetDateTime(&DateTime);  /* Get Current Date & Time */
    
    /* --- Current Individual Financial Control --- */
    if (usTermCurState == 0) {
        /* Exist Current Individual Financial Total */
        /* Read Individual Financial Total File */
        ulPositionTtl = ulPositionTtl + (TTL_INDTTL_SIZE * (ULONG)uchOffset);
        if ((sRetvalue = TtlIndFinTotalRead(ulPositionTtl, pIndTtl)) != 0) {
			PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_RESET_READ);
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(sRetvalue));
			PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
            return (sRetvalue);     /* Return Error Status */
        }

		TtlCopyNdateFromDateTime (&pIndTtl->ToDate, &DateTime);   /* TtlTresetIndFin() Set Reset Date & Time */
        CGGTotal = pIndTtl->mlCGGTotal;                           /* Save Current Gross Group Total */
        Taxable[0] = pIndTtl->Taxable[0].mlTaxableAmount;         /* Save Taxable Amount 1 */
        Taxable[1] = pIndTtl->Taxable[1].mlTaxableAmount;         /* Save Taxable Amount 2 */
        Taxable[2] = pIndTtl->Taxable[2].mlTaxableAmount;         /* Save Taxable Amount 3 */
        Taxable[3] = pIndTtl->Taxable[3].mlTaxableAmount;         /* Save Taxable Amount 4 */

        if ( usMDCBit & TTL_BIT_INDFIN_EXE ) { /* reset at ind-fin reset */
            lNetTtlSave = pIndTtl->lNetTotal;
        } else {                               /* reset at EOD (AC99) */
            lNetTtlSave = 0L;
        }
        pIndTtl->uchResetStatus |= TTL_NOTRESET;                 /* TtlTresetIndFin() Set Reset Disable status */

		//WRITE TOTAL SAVE FILE FOR IND FINANCIAL before it gets overwritten
        /* Read Individual Financial Total File */
        if ((sRetvalue = TtlIndFinTotalRead(ulPositionTtlPrev, &SavedTotal)) != 0) {
            return (sRetvalue);     /* Rteurn Error Status */
        }
		// update the to date to the latest and then write the totals to the SavedTotals file.
		// we want to make sure that TtlWriteSavFile() does the TtlCreateSavFile() in order to
		// ensure that everything is initialized properly before starting to snapshot the
		// totals to the SavedTotals file.  US Customs ran into a field issue that has existed
		// for some time in which the writing out of Operator Totals could result in GenPOS
		// crash if several End of Days are done with a large number of Operators (~200).
		//    Richard Chambers, Apr-15-2015
		TtlWriteSavFile(CLASS_TTLINDFIN, &SavedTotal, sizeof(SavedTotal), pTotal->usTerminalNumber);

        /* Write Individual Fianancial Total */
        if ((sRetvalue = TtlWriteFile(hsTtlIndFinHandle, ulPositionTtlPrev, pTmpBuff, TTL_INDTTL_SIZE)) != 0) {
			PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_RESET_WRITE);
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(sRetvalue));
			PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
            return (sRetvalue);     /* Return Error Status */
        }

        memset( pIndTtl, 0x00, sizeof(*pIndTtl) );
		TtlCopyNdateFromDateTime (&pIndTtl->FromDate, &DateTime);    /* TtlTresetIndFin() Set Reset Date & Time */
        pIndTtl->mlCGGTotal = CGGTotal;                              /* Save Current Gross Group Total */
        if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT2) == 0) {     /* Check Taxable Reset Enable MDC */
            pIndTtl->Taxable[0].mlTaxableAmount = Taxable[0];        /* ReStore Taxable Amount 1 */
            pIndTtl->Taxable[1].mlTaxableAmount = Taxable[1];        /* ReStore Taxable Amount 2 */
            pIndTtl->Taxable[2].mlTaxableAmount = Taxable[2];        /* ReStore Taxable Amount 3 */
            pIndTtl->Taxable[3].mlTaxableAmount = Taxable[3];        /* ReStore Taxable Amount 4 */
        }

        pIndTtl->lNetTotal = lNetTtlSave;

		/* Write Individual Fianancial Total */
        if ((sRetvalue = TtlWriteFile(hsTtlIndFinHandle, ulPositionTtl, pIndTtl, TTL_INDTTL_SIZE)) != 0) {
			PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_RESET_WRITE);
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(sRetvalue));
			PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
            return (sRetvalue);     /* Return Error Status */
        }
    } else {
		//There is no total information for the current ind-fin total
		//so we put blank information into the location.  Not Exist Current Ind-Fin Total
        memset(pTmpBuff, 0, TTL_INDTTL_SIZE );                            /* Clear Ind-Fin Total */
		TtlCopyNdateFromDateTime (&pIndTtl->FromDate, &DateTime);         /* TtlTresetIndFin() Set Reset Date & Time */
		TtlCopyNdateFromDateTime (&pIndTtl->ToDate, &DateTime);           /* TtlTresetIndFin() Set Reset Date & Time */
        pIndTtl->uchResetStatus |= TTL_NOTRESET;

		sRetvalue = TtlIndFinTotalRead (ulPositionTtlPrev, &SavedTotal);

		SavedTotal.FromDate = pIndTtl->FromDate;
		SavedTotal.ToDate = pIndTtl->ToDate;
		TtlWriteSavFile (CLASS_TTLINDFIN, &SavedTotal, sizeof(SavedTotal), pTotal->usTerminalNumber);

		/* Write Individual Fianancial Total */
        if ((sRetvalue = TtlWriteFile(hsTtlIndFinHandle, ulPositionTtlPrev, pTmpBuff, TTL_INDTTL_SIZE )) != 0) {
			PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_RESET_WRITE);
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(sRetvalue));
			PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
            return (sRetvalue);     /* Return Error Status */
        }
    }

    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTDeleteIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLREGFIN *pTotal   - Minor Class, Terminal Number
*               VOID      *pTmpBuff - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Deletting Individual Financial
*               TTL_NOTINFILE       - Terminal Not In File
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function Deletes the Individual Financial.
*
*============================================================================
*/
SHORT TtlTDeleteIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff)
{
    TTLCSINDHEAD *pIndHead = pTmpBuff;     /* Pointer of Read Buffer for File Header */

    USHORT  usMaxNo;
    USHORT  usTermNo;           /* Save Number of Terminals */
    USHORT  usPositionInd;      /* Save Seek Position */
    USHORT  usPositionEmp;      /* Save Seek Position */
    USHORT  usPositionTtl;      /* Save Seek Position */
    UCHAR   uchOffset;          /* Offset */
    UCHAR   uchNumber;
    SHORT   sRetvalue;

    if (hsTtlIndFinHandle < 0) {         /* Check Individual Financial File Handle */
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)abs(hsTtlIndFinHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);    /* Return File Handle Error */
    }

    /* --- Read File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {   /* Read File Header */
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Set Read Total Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        if (pIndHead->usCurDayTerminal == 0) {         /* Check Number of Current Terminal */
            return (TTL_NOTINFILE); /* Return Not In File */
        }
        usTermNo      = pIndHead->usCurDayTerminal;
        usPositionInd = pIndHead->usCurDayIndexOff;    /* Set Current Daily Index Offset */ 
        usPositionEmp = pIndHead->usCurDayEmptOff;     /* Set Current Daily Empty Offset */
        usPositionTtl = pIndHead->usCurDayTotalOff;    /* Set Current Daily Total Offset */
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        if (pIndHead->usPreDayTerminal == 0) {         /* Check Number of Previous Terminal */
            return (TTL_NOTINFILE); /* Return Not In File */
        }
        usTermNo      = pIndHead->usPreDayTerminal;
        usPositionInd = pIndHead->usPreDayIndexOff;    /* Set Previous Daily Index Offset */ 
        usPositionEmp = pIndHead->usPreDayEmptOff;     /* Set Previous Daily Empty Offset */
        usPositionTtl = pIndHead->usPreDayTotalOff;    /* Set Previous Daily Total Offset */
        break;

    default:
		PifLog(MODULE_TTL_TFL_FINANCIAL, LOG_ERROR_TOTALUM_TTL_BAD_CLASS);
		PifLog(MODULE_DATA_VALUE(MODULE_TTL_TFL_FINANCIAL), pTotal->uchMinorClass);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_FINANCIAL), (USHORT)__LINE__);
        return (TTL_NOTCLASS);
    }
    usMaxNo = pIndHead->usMaxTerminal;

    /* --- Search & Erase Terminal Index Table --- */
    if ((sRetvalue = TtlIndFinIndexRead((ULONG)usPositionInd, usTermNo, pTmpBuff)) != 0) {         /* Read Terminal Index Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }

    if(TtlIndFinISearch(pTmpBuff, usTermNo, pTotal->usTerminalNumber, &uchOffset, &uchNumber)) {   /* Search Terminal Index Table */
        return (TTL_NOTINFILE);     /* Return Fail */
    }
    if ((sRetvalue = TtlIndFinIErase(pTmpBuff, usTermNo, uchNumber)) != 0) {   /* Erase Terminal Index Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }
 
    if ((sRetvalue = TtlIndFinIndexWrite((ULONG)usPositionInd, usTermNo, pTmpBuff)) != 0) {   /* Write Terminal Index Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }

    /* --- Erase Terminal Empty Table --- */
    if ((sRetvalue = TtlIndFinEmptyRead((ULONG)usPositionEmp, pTmpBuff)) != 0) {  /* Read Terminal Empty Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if ((sRetvalue = TtlEmptyTableUpdate(pTmpBuff, usMaxNo, (USHORT)uchOffset, TTL_EMPBIT_RESET)) != 0) {
                                    /* Erase Terminal Empty Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    if ((sRetvalue = TtlIndFinEmptyWrite((ULONG)usPositionEmp, pTmpBuff)) != 0) {  /* Write Terminal Empty Table */
        return (sRetvalue);         /* Rteurn Error Status */
    }

    /* --- Clear Individual Financial Total ---- */
    memset(pTmpBuff, 0, TTL_CASTTL_SIZE);   
                                    /* Clear Buffer */
    usPositionTtl = usPositionTtl + (TTL_INDTTL_SIZE * uchOffset);
    if ((sRetvalue = TtlWriteFile(hsTtlIndFinHandle, (ULONG)usPositionTtl, pTmpBuff, TTL_INDTTL_SIZE)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Read File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {   /* Read File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }
    /* --- Update Terminal Number --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:           /* Current Daily File */
        pIndHead->usCurDayTerminal = pIndHead->usCurDayTerminal - 1;
        break;

    case CLASS_TTLSAVDAY:           /* Previous Daily File */
        pIndHead->usPreDayTerminal = pIndHead->usPreDayTerminal - 1;
        break;

/*  default:  Not Used */
    }

    /* --- Update File Header --- */
    if ((sRetvalue = TtlIndFinHeadWrite(pIndHead)) != 0) {    /* Write File Header */
        return (sRetvalue);         /* Rteurn Error Status */
    }

    return (TTL_SUCCESS);           /* Return Success */
}

/****************** New Functions (Release 3.1) END **********************/

/* End of File */
