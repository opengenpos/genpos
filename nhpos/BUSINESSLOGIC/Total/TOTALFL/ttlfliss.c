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
* Title       : TOTAL, Total Function Library  Subroutine                       
* Category    : TOTAL, NCR 2170 US Hospitality Application         
* Program Name: Ttlfliss.c                                                      
* --------------------------------------------------------------------------
* Abstract:        
*   SHORT TtlTIssuedFin()    - Set Reset Report Enable Status of Financial Total File 
*   SHORT TtlTIssuedHour()   - Set Reset Report Enable Status of Hourly Total File 
*   SHORT TtlTIssuedDept()   - Set Reset Report Enable Status of Dept Total File
*   SHORT TtlTIssuedPLU()    - Set Reset Report Enable Status of PLU Total File
*   SHORT TtlTIssuedCpn()    - Set Reset Report Enable Status of Coupon Total File, R3.0
*   SHORT TtlTIssuedCas()    - Set Reset Report Enable Status of Cashier Total File
*   SHORT TtlTIssuedWai()    - Set Reset Report Enable Status of Waiter Total File
*   SHORT TtlTIssuedSerTime()- Set Reset Report Enable Status of Service Time, R3.1
*   SHORT TtlTIssuedIndFin() - Set Reset Report Enable Status of Individual Financial, R3.1
*   SHORT TtlTTotalCheckFin()  - Check Reset report Enable Status of Financial
*   SHORT TtlTTotalCheckHour() - Check Reset report Enable Status of Hourly
*   SHORT TtlTTotalCheckDept() - Check Reset report Enable Status of Dept
*   SHORT TtlTTotalCheckPLU()  - Check Reset report Enable Status of PLU
*   SHORT TtlTTotalCheckCpn()  - Check Reset report Enable Status of Coupon, R3.0
*   SHORT TtlTTotalCheckCas()  - Check Reset report Enable Status of Cashier
*   SHORT TtlTTotalCheckWai()  - Check Reset report Enable Status of Waiter
*   SHORT TtlTTotalCheckSerTime() - Check Reset Report Enable Status of Service Time, R3.1
*   SHORT TtlTTotalCheckIndFin()  - Check Reset Report Enable Status of Individual Financial R3.1
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:K.Terai    :                                    
* Mar-03-95:03.00.00:hkato      : R3.0
* Nov-13-95:03.01.00:T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Mar-21-96:03.01.01:T.Nakahata : Cashier File Size USHORT to ULONG
* Aug-11-99:03.05.00:K.Iwata    : R3.5 Remov WAITER_MODEL
* Aug-13-99:03.05.00:K.Iwata    : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* Dec-14-99:01.00.00:hrkato     : Saratoga
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : ULONG back to USHORT of position with TtlGetSerTimeFilePosition().
* Jan-19-18 : 02.02.02 : R.Chambers : back to ULONG of position with TtlGetSerTimeFilePosition(). did it right this time.
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
#include <string.h>

#include <ecr.h>
#include <pif.h>    /* Portable Platform Interface header file */
#include <paraequ.h>
#include <csttl.h>
#include <appllog.h>
#include <ttl.h>    /* Total header file */

/*
*============================================================================
**Synopsis:     TtlTIssuedFin(TTLREGFIN *pTotal, TTLCSREGFIN *pTmpBuff,
*                                UCHAR uchStat, UCHAR uchMethod)
*
*    Input:     TTLREGFIN *pTotal   - Major & Minor Class
*               VOID *pTmpBuff      - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Setting Reset Enable Flag of 
*                                     Financial Total File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Status of Financial Total File.
*
*============================================================================
*/
SHORT TtlTIssuedFin(TTLREGFIN *pTotal, TTLCSREGFIN *pTmpBuff, 
                            UCHAR uchStat, UCHAR uchMethod)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetFinFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {    /* Get Financial Data Position */
        return (sRetvalue);
    }

    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR))) != 0) {
                                    /* Read Reset Report Status of Financial Total */
        return (sRetvalue);         /* Return Error Status */
    }
    if (uchMethod == TTL_CHECKFLAG) {
        if  (pTmpBuff->uchResetStatus & uchStat) {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedFin()")
                return (TTL_NOT_ISSUED);
        } else {
                return (TTL_ISSUED);
        }
    } 

	/* --- Set or Reset Report Status of Financial Total --- */
    if (uchMethod == TTL_SETFLAG) {
        pTmpBuff->uchResetStatus |= uchStat;     /* Set Report Status */
    } else {
        pTmpBuff->uchResetStatus &= ~uchStat;    /* Reset Report Status */
    }
    return (TtlWriteFile(hsTtlBaseHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR)));  /* Write Reset Report Status of Financial Total */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTIssuedHour(TTLHOURYTBL *pTotal, TTLCSHOULY *pTmpBuff,
*                                       UCHAR uchStat, UCHAR uchMethod)
*
*    Input:     TTLHOURYTBL *pTotal - Major & Minor Class
*               VOID *pTmpBuff      - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the setting Reset Enable Flag of 
*                                     Hourly Total File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Enable Flag of Hourly Total File.
*
*============================================================================
*/
SHORT TtlTIssuedHour(TTLHOURLY *pTotal, TTLCSHOURLY *pTmpBuff,
                        UCHAR uchStat, UCHAR uchMethod)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetHourFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {  /* Get Hourly Data Position */
        return (sRetvalue);
    }

    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR))) != 0) {
                                    /* Read Reset Report Status of Hourly Total File */
        return (sRetvalue);         /* Return Error Status */
    }
    if (uchMethod == TTL_CHECKFLAG) {
        if  (pTmpBuff->uchResetStatus & uchStat) {
			if (CliParaMDCCheckField(MDC_EOD1_ADR, MDC_PARAM_BIT_A)) {
				// hourly activity report not executed
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED_MDC: TtlTIssuedHour(). MDC_EOD1_ADR (91.A) disabled.")
				return (TTL_NOT_ISSUED_MDC);
			} else {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedHour()")
				return (TTL_NOT_ISSUED);
			}
        } else {
                return (TTL_ISSUED);
        }
    }

	/* --- Set Reset Report Status of Hourly Total --- */
    if (uchMethod == TTL_SETFLAG) {
        pTmpBuff->uchResetStatus |= uchStat;    /* Set Report Status */
    } else {
        pTmpBuff->uchResetStatus &= ~uchStat;   /* Reset Report Status */
    }
    return (TtlWriteFile(hsTtlBaseHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR)));  /* Write Reset Report Status of Hourly Total File */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTIssuedDept(TTLDEPT *pTotal, VOID *pTmpBuff, 
*                                      BOOL fIssued)
*
*    Input:     TTLDEPT *pTotal      - Major & Minor Class, Dept Number
*               VOID *pTmpBuff      - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Setting Reset Enable Flag of 
*                                     Dept Total File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Enable Flag of Dept Total File.
*
*============================================================================
*/
SHORT TtlTIssuedDept(TTLDEPT *pTotal, VOID *pTmpBuff,
                       UCHAR uchStat, UCHAR uchMethod)
{
    TTLCSDEPTHEAD    * const pDepthd = pTmpBuff;       /* Set Dept File Header set Buffer Address */
    TTLCSDEPTMISC    * const pDeptMisc = pTmpBuff;     /* Set DEPT Misc Toatl Read Buffer Address */

    ULONG   ulPositionMsc;      /* Save Seek Position */
    SHORT   sRetvalue;

    if (hsTtlDeptHandle < 0) {         /* Check Dept Total File Handle */
        return (TTL_FILE_HANDLE_ERR);  /* Return File Handle Error */
    }

	/* --- Read Dept File Header --- */
    if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {    /* Read Dept File Header */
        return (sRetvalue);         /* Return Error Status */
    }

	/* --- Set Reset Report Status Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:                        /* Current Daily File */
        ulPositionMsc = pDepthd->ulCurDayMiscOff;
        break;

    case CLASS_TTLSAVDAY:                        /* Previous Daily File */
        ulPositionMsc = pDepthd->ulPreDayMiscOff;
        break;

    case CLASS_TTLCURPTD:                        /* Current PTD File */
        if (pDepthd->ulCurPTDMiscOff == 0L) {    /* Check Exist PTD Total File */
            return (TTL_NOTPTD);                 /* Return Not Exist PTD */
        }
        ulPositionMsc = pDepthd->ulCurPTDMiscOff;
        break;

    case CLASS_TTLSAVPTD:                        /* Previous PTD File */
        if (pDepthd->ulPrePTDMiscOff == 0L) {    /* Check Exist PTD Total File */
            return (TTL_NOTPTD);                 /* Return Not Exist PTD */
        }
        ulPositionMsc = pDepthd->ulPrePTDMiscOff;
        break;

    default:
        return (TTL_NOTCLASS);      /* Return Not Class */
    }

    if ((sRetvalue = TtlReadFile(hsTtlDeptHandle, ulPositionMsc, (VOID *)&pDeptMisc->uchResetStatus, sizeof(UCHAR))) != 0) {
                                    /* Write Reset Report Status of Dept Total File */
        return (sRetvalue);         /* Return Error Status */
    }
    if (uchMethod == TTL_CHECKFLAG) {
        if  (pDeptMisc->uchResetStatus & uchStat) {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedDept()")
                return (TTL_NOT_ISSUED);
        } else {
                return (TTL_ISSUED);
        }
    }

	/* --- Set Reset Report Status of Dept Total --- */
    if (uchMethod == TTL_SETFLAG) {
        pDeptMisc->uchResetStatus |= uchStat;     /* Set Report Status */
    } else {
        pDeptMisc->uchResetStatus &= ~uchStat;    /* Reset Report Status */
    }
    return (TtlWriteFile(hsTtlDeptHandle, ulPositionMsc, (VOID *)&pDeptMisc->uchResetStatus, sizeof(UCHAR))); /* Write Reset Report Status of Dept Total File */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTIssuedCpn(TTLCPN *pTotal, VOID *pTmpBuff,
*                                       BOOL fIssued)
*
*    Input:    TTLCPN *pTotal      - Major & Minor Class , Coupon Number
*              VOID *pTmpBuff      - Pointer of Work Buffer
*              UCHAR uchStat       - Status
*              UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:    Nothing
*
** Return:      TTL_SUCCESS         - Successed the setting Reset Enable Flag of 
*                                     Coupon Total  
*              TTL_NOTCLASS        - Set Illigal Class No
*              TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*                  TTL_FILE_SEEK_ERR   - Occured File Seek Error
*              TTL_FILE_READ_ERR   - Occured File Read Error
*                                   
** Description  This function sets the Reset Enable Flag of Coupon Total File.  R3.0
*
*============================================================================
*/
SHORT TtlTIssuedCpn(TTLCPN *pTotal, TTLCS_TMPBUF_WORKING pTmpBuff,
                        UCHAR uchStat, UCHAR uchMethod)
{
    TTLCSCPNHEAD    * const pCpnhd = (VOID *)pTmpBuff;    /* Pointer of Coupon File Header set Buffer */
    TTLCSCPNMISC    * const pCpnMisc = (VOID *)pTmpBuff;  /* Pointer of Coupon Misc Total set Buffer */
    ULONG           ulPosition;     /* Seek Position */
    SHORT           sRetvalue;

    if ((sRetvalue = TtlGetCpnFilePosition(pTotal->uchMinorClass, &ulPosition, pCpnhd)) != 0) {    /* Get Coupon Data Position */
        return (sRetvalue);
    }

    if ((sRetvalue = TtlReadFile(hsTtlCpnHandle, ulPosition, &pCpnMisc->uchResetStatus, sizeof(UCHAR))) != 0) {    /* Read Reset Report Status of coupon Total File */
        return (sRetvalue);         /* Return Error Status */
    }

    if (uchMethod == TTL_CHECKFLAG) {
        if  (pCpnMisc->uchResetStatus & uchStat) {
			if (CliParaMDCCheckField(MDC_EOD2_ADR, MDC_PARAM_BIT_A)) {
				// hourly activity report not executed
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED_MDC: TtlTIssuedCpn(). MDC_EOD2_ADR (92.A) disabled.")
				return (TTL_NOT_ISSUED_MDC);
			} else {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedCpn()")
                return (TTL_NOT_ISSUED);
			}
        } else {
                return (TTL_ISSUED);
        }
    }

	/* --- Set Reset Report Status of Coupon Total --- */
    if (uchMethod == TTL_SETFLAG) {
        pCpnMisc->uchResetStatus |= uchStat;      /* Set Report Status */
    } else {
        pCpnMisc->uchResetStatus &= ~uchStat;     /* Reset Report Status */
    }
    return (TtlWriteFile(hsTtlCpnHandle, ulPosition, &pCpnMisc->uchResetStatus, sizeof(UCHAR)));  /* Write Reset Report Status of Coupon Total File */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTIssuedCas(TTLCASHIER *pTotal, VOID *pTmpBuff, 
*                                      BOOL fIssued)
*
*    Input:     TTLCASHIER *pTotal      - Major & Minor Class, Cashier Number
*               VOID *pTmpBuff      - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the setting Reset Enable Flag of 
*                                     Cashier Total File 
*               TTL_NOTINFILE       - Cashier Not In File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Enable Flag of 
*               Cashier Total File.
*
*============================================================================
*/
SHORT TtlTIssuedCas(TTLCASHIER *pTotal, TTLCS_TMPBUF_WORKING pTmpBuff,
                       UCHAR uchStat, UCHAR uchMethod)
{
    TTLCSCASHEAD    * const pCashd = (VOID  *)pTmpBuff;    /* Pointer of Cashier File Header set Buffer */
    TTLCSCASTOTAL   * const pCasTtl = (VOID  *)pTmpBuff;   /* Pointer of Cashier Total set Buffer */ 
	TTLCSCASINDX    CasInd = {0};

    USHORT  usNumberOfCashiers; /* Save Cashier Number */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */
    ULONG   ulPosition;
    ULONG   ulIndOffset;
    USHORT  usOffset;           /* Offset, V3.3 */
    SHORT   sRetvalue;
    UCHAR   uchFlag;

    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
    }

	/* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {    /* Read Cashier File Header */
        return (sRetvalue);                             /* Return Error Status */
    }

	/* --- Set Reset Report Status Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:                 /* Current Daily File */
        if (pCashd->usCurDayCas == 0) {   /* Check Current Cashier Number */
            return (TTL_NOTINFILE);       /* Return Not In File */
        }
        usNumberOfCashiers = pCashd->usCurDayCas;
        ulPositionInd = pCashd->ulCurDayIndexOff;
        ulPositionTtl = pCashd->ulCurDayTotalOff;
        break;

    case CLASS_TTLSAVDAY:                 /* Previous Daily File */
        if (pCashd->usPreDayCas == 0) {   /* Check Previous Cashier Number */
            return (TTL_NOTINFILE);       /* Return Not In File */
        }
        usNumberOfCashiers = pCashd->usPreDayCas;
        ulPositionInd = pCashd->ulPreDayIndexOff;
        ulPositionTtl = pCashd->ulPreDayTotalOff;
        break;

    case CLASS_TTLCURPTD:                       /* Current PTD File, V3.3 */
        if (pCashd->ulCurPTDIndexOff == 0L) {   /* Check Exist PTD Total File */
            return (TTL_NOTPTD);                /* Return Not Exist PTD */
        }
        if (pCashd->usCurPTDCas == 0) {         /* Check Current Cashier Number */
            return (TTL_NOTINFILE);             /* Return Not In File */
        }
        usNumberOfCashiers = pCashd->usCurPTDCas;
        ulPositionInd = pCashd->ulCurPTDIndexOff;
        ulPositionTtl = pCashd->ulCurPTDTotalOff;
        break;

    case CLASS_TTLSAVPTD:                       /* Previous PTD File, V3.3 */
        if (pCashd->ulPrePTDIndexOff == 0L) {   /* Check Exist PTD Total File */
            return (TTL_NOTPTD);                /* Return Not Exist PTD */
        }
        if (pCashd->usPrePTDCas == 0) {         /* Check Current Cashier Number */
            return (TTL_NOTINFILE);             /* Return Not In File */
        }
        usNumberOfCashiers = pCashd->usPrePTDCas;
        ulPositionInd = pCashd->ulPrePTDIndexOff;
        ulPositionTtl = pCashd->ulPrePTDTotalOff;
        break;

    default:
        return (TTL_NOTCLASS);      /* Return Not Class */
    }

	/* --- Search Cashier Index Table --- */
    if ((uchMethod == TTL_CHECKFLAG) && (pTotal->ulCashierNumber == 0)) {
		USHORT  i;

        for (i = 0; i < usNumberOfCashiers; i++) {
            /* --- Search Cashier Index Table, V3.3 --- */
			/* Read Reset Report Status of CashierTotal File */
            if ((sRetvalue = TtlReadFile(hsTtlCasHandle, ulPositionInd, &CasInd, sizeof(CasInd))) != 0) {
                return (sRetvalue); /* Return Error Status */
            }
            usOffset = CasInd.usBlockNo;            /* Set Total Offset */
            ulPosition = ulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)usOffset);

            /* Read Reset Report Status of CashierTotal File */
            if ((sRetvalue = TtlReadFile(hsTtlCasHandle, ulPosition, &uchFlag, sizeof(UCHAR))) != 0) {
                return (sRetvalue); /* Return Error Status */
            }
            if (uchFlag & uchStat) {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedCas()")
                return (TTL_NOT_ISSUED);
            }
            ulPositionInd += sizeof(CasInd);
        }
        return (TTL_ISSUED);
    } 

	/* --- Search Cashier Index Table, V3.3 --- */
    CasInd.ulCasNo = pTotal->ulCashierNumber;
    if ((sRetvalue = TtlCasISearch(ulPositionInd, usNumberOfCashiers, &CasInd, pTmpBuff, &ulIndOffset, TTL_SEARCH_EQU)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    usOffset = CasInd.usBlockNo;    /* set empty table offset */

	/* --- Set Reset Report Status of Cashier Total --- */
    ulPosition = ulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)usOffset);
    /* Read Reset Report Status of CashierTotal File */
    if ((sRetvalue = TtlReadFile(hsTtlCasHandle, ulPosition, &uchFlag, sizeof(UCHAR))) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }
    if (uchMethod == TTL_CHECKFLAG) {
        if  (uchFlag & uchStat) {
			NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedCas()")
            return (TTL_NOT_ISSUED);
        } else {
            return (TTL_ISSUED);
        }
    }  
    if (uchMethod == TTL_SETFLAG) {
        uchFlag |= uchStat;            /* Set Report Status */
    } else {
        uchFlag &= ~uchStat;           /* Reset Report Status */
    }
    return (TtlWriteFile(hsTtlCasHandle, ulPosition, &uchFlag, sizeof(UCHAR)));  /* Write Reset Report Status of Cashier Total File */
}
/*
*============================================================================
**Synopsis:     TtlTTotalCheckFin(TTLREGFIN *pTotal, TTLCSREGFIN *pTmpBuff)
*
*    Input:     TTLREGFIN *pTotal       - Major & Minor Class
*               TTLCSREGFIN *pTmpBuff   - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Financial Total is 0
**              TTL_NOTZERO     - Financial Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks that the Financial Total is 0.
*
*============================================================================
*/
SHORT TtlTTotalCheckFin(TTLREGFIN *pTotal, TTLCSREGFIN *pTmpBuff)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetFinFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {   /* Get Financial Data Position - TtlTTotalCheckFin() */
        return (sRetvalue);
    }

	/* --- Check Reset Report Status of Financial Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR))) != 0) {   /* Read Reset Report Status of Financial Total */
        return (sRetvalue);         /* Return Error Status */
    }
    return(TtlStringCheck(&pTmpBuff->mlCGGTotal, (TTL_FIN_SIZE - sizeof(UCHAR) - (sizeof(N_DATE) * 2)), 0));
}

/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckHour(TTLHOURYTBL *pTotal, 
*                                       TTLCSHOULY *pTmpBuff)
*
*    Input:     TTLHOURYTBL *pTotal     - Major & Minor Class
*               TTLCSHOULY *pTmpBuff    - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Hourly Total is 0
**              TTL_NOTZERO     - Hourly Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks that Hourly Total is 0.
*
*============================================================================
*/
SHORT TtlTTotalCheckHour(TTLHOURLY *pTotal, TTLCSHOURLY *pTmpBuff)
{
    ULONG   ulPosition;         /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetHourFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {    /* Get Hourly Data Position - TtlTTotalCheckHour() */
        return (sRetvalue);
    }

	/* --- Check Reset Report Status of Hourly Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, (VOID *)&pTmpBuff->uchResetStatus, sizeof(UCHAR))) != 0) { /* Read Reset Report Status of Hourly Total File */
        return (sRetvalue);         /* Return Error Status */
    }

    return(TtlStringCheck((VOID *)&pTmpBuff->Total, sizeof(pTmpBuff->Total), 0));
}
/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckDept(TTLDEPT *pTotal, VOID *pTmpBuff) 
*
*    Input:     TTLDEPT *pTotal          - Major & Minor Class, Dept Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Dept Total is 0
**              TTL_NOTZERO     - Dept Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks the Dept Total is 0.
*
*============================================================================
*/
SHORT TtlTTotalCheckDept(TTLDEPT *pTotal, VOID *pTmpBuff)
{
    SHORT   sRetvalue;

    if (hsTtlDeptHandle < 0) {            /* Check Dept Total File Handle */
        return (TTL_FILE_HANDLE_ERR);     /* Return File Handle Error */
    }

    if (pTotal->usDEPTNumber == 0) {
		TTLCSDEPTHEAD    *pDepthd = (VOID *)pTmpBuff;        /* Set Dept File Header set Buffer Address */

		/* --- Read Dept File Header --- */
        if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {   /* Read Dept File Header */
            return (sRetvalue);                              /* Return Error Status */
        }

		/* --- Set Reset Report Status Offset --- */
        switch(pTotal->uchMinorClass) {
        case CLASS_TTLCURDAY:                /* Current Daily File */
            if (pDepthd->usCurDayDept) {     /* Check No of Current Daily Dept Number */
                return (TTL_NOTZERO);
            }
            break;

        case CLASS_TTLSAVDAY:                /* Previous Daily File */
            if (pDepthd->usPreDayDept) {     /* Check No of Previous Daily Dept Number */
                return (TTL_NOTZERO);
            }
            break;

        case CLASS_TTLCURPTD:                         /* Current PTD File */
            if (pDepthd->ulCurPTDMiscOff == 0L) {     /* Check Exist PTD Total File */
                return (TTL_NOTPTD);                  /* Return Not Exist PTD */
            }
            if (pDepthd->usCurPTDDept) {              /* Check No of Current PTD Dept Number */
                return (TTL_NOTZERO);
            }
            break;

        case CLASS_TTLSAVPTD:                         /* Previous PTD File */
            if (pDepthd->ulPrePTDMiscOff == 0L) {     /* Check Exist PTD Total File */
                return (TTL_NOTPTD);                  /* Return Not Exist PTD */
            }
            if (pDepthd->usPrePTDDept) {              /* Check No of Current PTD Dept Number */
                return (TTL_NOTZERO);
            }
            break;

        default:
            return (TTL_NOTCLASS);
        }
    } else {
		TTLDEPT  DeptTtl = *pTotal;

        sRetvalue = TtlTreadDept(&DeptTtl, pTmpBuff);      /* Read Target Dept */
        switch (sRetvalue) {
        case TTL_SUCCESS:
            if ((DeptTtl.DEPTTotal.lAmount != 0L) || (DeptTtl.DEPTTotal.lCounter != 0L)) {   /* Check Traget Total */
                return (TTL_NOTZERO);
            }
            break;

        case TTL_NOTINFILE:
            break;

        default:
            return (sRetvalue);
        }
    }
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckCpn(TTLCPN *pTotal, VOID *pTmpBuff)
*
*    Input:     TTLCPN *pTotal         - Major & Minor Class , Dept Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Cpn Total is 0
**              TTL_NOTZERO     - Cpn Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks the Cpn Total is 0.             R3.0
*
*============================================================================
*/
SHORT TtlTTotalCheckCpn(TTLCPN *pTotal, VOID *pTmpBuff)
{
	TTLCSCPNHEAD    * const pCpnhd = pTmpBuff;    /* Pointer of Coupon File Header Read Buffer */
	ULONG           ulPosition;            /* Seek Position */
	USHORT          usSize;
	SHORT           sRetvalue;

    if ((sRetvalue = TtlGetCpnFilePosition(pTotal->uchMinorClass, &ulPosition, pCpnhd)) != 0) {  /* Get Dept Data Position */
        return (sRetvalue);
    }
    if (pTotal->usCpnNumber > pCpnhd->usMaxCpn) {
        return (TTL_SIZEOVER);      /* Return Size Over */
    }

    if (pTotal->usCpnNumber) {
		TOTAL     CpnTtl = {0};     /* Read buffer for Daily Coupon Total */

		/* --- Read Target Coupon Total --- */
        switch(pTotal->uchMinorClass) {
        case CLASS_TTLCURDAY:      /* Current Daily File */
        case CLASS_TTLSAVDAY:      /* Previous Daily File */
			if (TtlCpnTotalRead(ulPosition, pTotal->usCpnNumber, &CpnTtl, sizeof(TOTAL))) {    /* Read Coupon Total - CLASS_TTLCURDAY, CLASS_TTLSAVDAY */
				return (TTL_FAIL);  /* Return Fail */
			}
			break;

        case CLASS_TTLCURPTD:      /* Current PTD File */
        case CLASS_TTLSAVPTD:      /* Previous PTD File */
			if (TtlCpnTotalRead(ulPosition,  pTotal->usCpnNumber, &CpnTtl, sizeof(TOTAL))) {  /* Read Coupon Total - CLASS_TTLCURPTD, CLASS_TTLSAVPTD */
				return (TTL_FAIL);  /* Return Fail */
			}
			break;

		default:
			return (TTL_FAIL);  /* Return Fail */
			break;
        }
        if ((CpnTtl.lAmount != 0L) || (CpnTtl.sCounter != 0L)) {
            return (TTL_NOTZERO);
        }
        return (TTL_SUCCESS);           /* Return Success */
    } else {
		/* --- All Coupon Total Read --- */
        ulPosition += TTL_CPNMISC_SIZE;
		usSize = 0;
        switch(pTotal->uchMinorClass) {
        case CLASS_TTLCURDAY:      /* Current Daily File */
        case CLASS_TTLSAVDAY:      /* Previous Daily File */
            usSize = (sizeof(TOTAL) * pCpnhd->usMaxCpn);
            break;

        case CLASS_TTLCURPTD:      /* Current PTD File */
        case CLASS_TTLSAVPTD:      /* Previous PTD File */
            usSize = (sizeof(TOTAL) * pCpnhd->usMaxCpn);
            break;
        }

        while (usSize != 0) {
			USHORT    usTmpSize = TTL_TMPBUF_SIZE;

            if (usSize < TTL_TMPBUF_SIZE) {
                usTmpSize = usSize;
            }
            if (TtlReadFile(hsTtlCpnHandle, ulPosition, pTmpBuff, usTmpSize)) {
                return (TTL_FAIL);      /* Return Fail */
            }
            if ((sRetvalue = TtlStringCheck(pTmpBuff, usTmpSize, 0)) != TTL_SUCCESS) {
                return(sRetvalue);
            }
            usSize -= usTmpSize;
            ulPosition += usTmpSize;
        }
        return (TTL_SUCCESS);           /* Return Success */
    }
}
/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckCas(TTLCASHIER *pTotal, VOID *pTmpBuff) 
*
*    Input:     TTLCASHIER *pTotal      - Major & Minor Class, Cashier Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Cashier Total is 0
**              TTL_NOTZERO     - Cashier Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks the Cashier Total is 0.
*
*============================================================================
*/
SHORT TtlTTotalCheckCas(TTLCASHIER *pTotal, VOID *pTmpBuff)
{
    SHORT   sRetvalue;

    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
    }

    if (pTotal->ulCashierNumber == 0) {
		TTLCSCASHEAD    *pCashd = (VOID  *)pTmpBuff;         /* Set Cashier File Header set Buffer Address */

		/* --- Read Cashier File Header --- */
        if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {     /* Read Cashier File Header - TtlTTotalCheckCas() */
            return (sRetvalue);                              /* Return Fail */
        }

/* --- Set Reset Report Status Offset --- */
        switch(pTotal->uchMinorClass) {
        case CLASS_TTLCURDAY:                  /* Current Daily File */
            if (pCashd->usCurDayCas) {         /* Check Current Cashier Number */
                return (TTL_NOTZERO);          /* Return Not Zero */
            }
            break;

        case CLASS_TTLSAVDAY:                  /* Previous Daily File */
            if (pCashd->usPreDayCas) {         /* Check Previous Cashier Number */
                return (TTL_NOTZERO);          /* Return Not Zero */
            }
            break;

        case CLASS_TTLCURPTD:                           /* Current PTD File, V3.3 */
            if (pCashd->ulCurPTDIndexOff == 0L) {       /* Check Exist PTD Total File */
                return (TTL_NOTPTD);                    /* Return Not Exist PTD */
            }
            if (pCashd->usCurPTDCas) {                  /* Check Current Cashier Number */
                return (TTL_NOTZERO);                   /* Return Not In File */
            }
            break;

        case CLASS_TTLSAVPTD:                           /* Previous PTD File */
            if (pCashd->ulPrePTDIndexOff == 0) {        /* Check Exist PTD Total File */
                return (TTL_NOTPTD);                    /* Return Not Exist PTD */
            }
            if (pCashd->usPrePTDCas) {                  /* Check Current Cashier Number */
                return (TTL_NOTZERO);                   /* Return Not In File */
            }
            break;

        default:
            return (TTL_NOTCLASS);
        }
    } else {
        sRetvalue = TtlreadCas(pTotal->uchMinorClass, pTotal->ulCashierNumber, pTmpBuff);     /* Read Cashier File */
        switch (sRetvalue) {
        case TTL_SUCCESS:
            return (TTL_NOTZERO);

        case TTL_NOTINFILE:
            break;

        default:
            return (sRetvalue);     /* Return Error */
        }
    }
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlStringCheck(VOID *pStrBuff, USHORT usLength, 
*                                    UCHAR uchCmpChar)
*
*    Input:     VOID *pStrBuff    - starting address for the memory area to check
*               USHORT usLength   - number of bytes to check
*               UCHAR uchCmpChar  - comparison value to compare against
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS - Successed String Check  
*               TTl_NOTZERO - Failed String Check
*
** Description  This function performs a bitwise OR operation over an area of
*               memory and then compares the result with the specified comparison
*               value.
*
*               The primary purpose for this function is to determine if a region
*               of memory is all binary zeros. It is used in a number of places
*               when processing totals to determine if a particular total should
*               appear in reports or not depending on MDC settings for the report.
*============================================================================
*/
SHORT TtlStringCheck(VOID *pStrBuff, USHORT usLength, UCHAR uchCmpChar)
{
    USHORT  *pusCmpBuff = pStrBuff;
    UCHAR   *puchCmpBuff;
    USHORT  usCmpWork = 0;
    UCHAR   uchCmpWork = 0;
    USHORT  i;

	// scan the memory area an unsigned short per access building
	// a bitwise OR of all memory locations. Odd numbered range
	// is handled following this.
    for ( i = 0; i < (usLength / 2) ; i++, pusCmpBuff++) {
        usCmpWork |= *pusCmpBuff;    
    }

	// OR together most significant and least significant bytes of the
	// USHORT to combine both bytes into a single byte.
    puchCmpBuff = (UCHAR *)&usCmpWork;
    uchCmpWork = (*puchCmpBuff | *(puchCmpBuff+1));

	// if range is odd then we have an additional byte to process.
    if (usLength % 2) {
        puchCmpBuff = (UCHAR *)pusCmpBuff;
        uchCmpWork |= *puchCmpBuff;
    }

	// compare our generate bitwise OR over the memory range against the comparison byte
	// and let the caller know if they match or not.
    if (uchCmpWork == uchCmpChar) {
        return (TTL_SUCCESS);
    } else {
        return (TTL_NOTZERO);
    }
}

/************************ New Functions (Release 3.1) BEGIN ****************/

/*
*============================================================================
**Synopsis:     TtlTIssuedSerTime(TTLSERTIME *pTotal, TTLCSSERTIME *pTmpBuff,
*                                 UCHAR uchStat, UCHAR uchMethod)
*
*    Input:     TTLSERTIME *pTotal   - Major & Minor Class
*               TTLCSSERTIME *pTmpBuff - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the Setting Reset Enable Flag of 
*                                     Financial Total File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Status of Service Time Total.
*
*============================================================================
*/
SHORT TtlTIssuedSerTime(TTLSERTIME *pTotal, TTLCSSERTIME *pTmpBuff, 
                            UCHAR uchStat, UCHAR uchMethod)
{
    ULONG   ulPosition = 0;             /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetSerTimeFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {  /* Get Service Time Data Position */
        return (sRetvalue);
    }

    if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR))) != 0) {    /* Read Reset Report Status of Service Time */
        return (sRetvalue);         /* Return Error Status */
    }
    if (uchMethod == TTL_CHECKFLAG) {
        if  (pTmpBuff->uchResetStatus & uchStat) {
			NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedSerTime()")
            return (TTL_NOT_ISSUED);
        } else {
            return (TTL_ISSUED);
        }
    }  
    /* --- Set or Reset Report Status of Service Time Total --- */
    if (uchMethod == TTL_SETFLAG) {
        pTmpBuff->uchResetStatus |= uchStat;     /* Set Report Status */
    } else {
        pTmpBuff->uchResetStatus &= ~uchStat;    /* Reset Report Status */
    }
    sRetvalue = TtlWriteFile(hsTtlSerTimeHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR) );     /* Update the report status for Service Time Total */
    return (sRetvalue);
}


/*
*============================================================================
**Synopsis:     SHORT TtlTIssuedIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff,
*                                       UCAHR uchStat, UCHAR uchMethod)
*
*    Input:     TTLREGFIN *pTotal   - Major & Minor Class
*               VOID      *pTmpBuff - Pointer of Work Buffer
*               UCHAR uchStat       - Status
*               UCHAR uchMethod     - Method
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the setting Reset Enable Flag of 
*                                     Individual Financial Total File 
*               TTL_NOTINFILE       - Terminal Not In File 
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function sets the Reset Enable Flag of 
*               Individual Financial Total File.
*
*============================================================================
*/
SHORT TtlTIssuedIndFin(TTLREGFIN *pTotal, TTLCS_TMPBUF_WORKING pTmpBuff,
                       UCHAR uchStat, UCHAR uchMethod)
{
    TTLCSINDHEAD    * const pIndHead = (VOID  *)pTmpBuff;  /* Pointer of Individual Financial Header set Buffer */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */
    ULONG   ulPosition;
    USHORT  usTermNo;           /* Save Terminal Number */
    SHORT   sRetvalue;
    UCHAR   uchNumber;
    UCHAR   uchOffset;          /* Offset */
    UCHAR   uchFlag;

    if (hsTtlIndFinHandle < 0) {    /* Check Individual Financial Total File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

    /* --- Read Cashier File Header --- */
    if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {     /* Read File Header - TtlTIssuedIndFin() */
        return (sRetvalue);         /* Return Error Status */
    }

    /* --- Set Reset Report Status Offset --- */
    switch(pTotal->uchMinorClass) {
    case CLASS_TTLCURDAY:                            /* Current Daily File */
        if (pIndHead->usCurDayTerminal == 0) {       /* Check Current Terminal Number */
            return (TTL_NOTINFILE);                  /* Return Not In File */
        }
        usTermNo      = pIndHead->usCurDayTerminal;
        ulPositionInd = pIndHead->usCurDayIndexOff;
        ulPositionTtl = pIndHead->usCurDayTotalOff;
        break;

    case CLASS_TTLSAVDAY:                            /* Saved Daily File */
        if (pIndHead->usPreDayTerminal == 0) {       /* Check Saved Terminal Number */
            return (TTL_NOTINFILE);                  /* Return Not In File */
        }
        usTermNo      = pIndHead->usPreDayTerminal;
        ulPositionInd = pIndHead->usPreDayIndexOff;
        ulPositionTtl = pIndHead->usPreDayTotalOff;
        break;

    default:
        return (TTL_NOTCLASS);      /* Return Not Class */
    }

    /* --- Search Terminal Index Table --- */
    if ((sRetvalue = TtlIndFinIndexRead(ulPositionInd, usTermNo, pTmpBuff)) != 0) {    /* Read Terminal Index Table */
        return (sRetvalue);         /* Return Error Status */
    }
    if ((uchMethod == TTL_CHECKFLAG) && (pTotal->usTerminalNumber == 0)) {
		TTLCSINDINDX    *pIndIdx = (VOID *)pTmpBuff;               /* Set Seach Buffer Address */
		USHORT  i;

        for (i = 0; i < usTermNo; i++, pIndIdx++) {
            uchOffset = pIndIdx->uchBlockNo;              /* Set Total Offset */
            ulPosition = ulPositionTtl + (TTL_INDTTL_SIZE * (USHORT)uchOffset);
            if ((sRetvalue = TtlReadFile(hsTtlIndFinHandle, ulPosition, &uchFlag, sizeof(UCHAR))) != 0) {     /* Read Reset Report Status of Individual Financial */
                return (sRetvalue); /* Return Error Status */
            }
            if (uchFlag & uchStat) {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedIndFin()")
                return (TTL_NOT_ISSUED);
            }
        }
        return (TTL_ISSUED);
    } 

    if (TtlIndFinISearch(pTmpBuff, usTermNo, pTotal->usTerminalNumber, &uchOffset, &uchNumber)) {      /* Search Terminal Index Table */
        return (TTL_NOTINFILE);     /* Return Not In File */
    }

    /* --- Set Reset Report Status of Cashier Total --- */
    ulPosition = ulPositionTtl + (TTL_INDTTL_SIZE * (USHORT)uchOffset);
    if ((sRetvalue = TtlReadFile(hsTtlIndFinHandle, ulPosition, &uchFlag, sizeof(UCHAR))) != 0) {   /* Read Reset Report Status of Individual Financial - uchResetStatus */
        return (sRetvalue);         /* Return Error Status */
    }
    if (uchMethod == TTL_CHECKFLAG) {
        if  (uchFlag & uchStat) {
				NHPOS_ASSERT_TEXT(0, "TTL_NOT_ISSUED: TtlTIssuedIndFin()")
                return (TTL_NOT_ISSUED);
        } else {
                return (TTL_ISSUED);
        }
    }  
    if (uchMethod == TTL_SETFLAG) {
        uchFlag |= uchStat;           /* Set Report Status */
    } else {
        uchFlag &= ~uchStat;          /* Reset Report Status */
    }
    sRetvalue = TtlWriteFile(hsTtlIndFinHandle, ulPosition, &uchFlag, sizeof(UCHAR));  /* Update the Report Status of Individual Financial - uchResetStatus */

    return (sRetvalue);
}


/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckSerTime(TTLSERTIME   *pTotal, 
*                                           TTLCSSERTIME *pTmpBuff)
*
*    Input:     TTLSERTIME    *pTotal   - Major & Minor Class
*               TTLCSSERTIME *pTmpBuff  - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Hourly Total is 0
**              TTL_NOTZERO     - Hourly Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks that Service Time Total is 0.
*
*============================================================================
*/
SHORT TtlTTotalCheckSerTime(TTLSERTIME *pTotal, TTLCSSERTIME *pTmpBuff)
{
    ULONG   ulPosition = 0;             /* Seek Position */
    SHORT   sRetvalue;

    if ((sRetvalue = TtlGetSerTimeFilePosition(pTotal->uchMinorClass, &ulPosition)) != 0) {    /* Get Service Time Data Position */
        return (sRetvalue);
    }

    /* --- Check Reset Report Status of Service Time Total --- */
    if ((sRetvalue = TtlReadFile(hsTtlSerTimeHandle, ulPosition, &pTmpBuff->uchResetStatus, sizeof(UCHAR))) != 0) {  /* Read Reset Report Status of Service Time File */
        return (sRetvalue);         /* Return Error Status */
    }

    sRetvalue = TtlStringCheck(&pTmpBuff->ServiceTime, sizeof(pTmpBuff->ServiceTime), 0);
    return (sRetvalue);
}


/*
*============================================================================
**Synopsis:     SHORT TtlTTotalCheckIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff) 
*
*    Input:     TTLREGFIN *pTotal       - Major & Minor Class, Terminal Number
*               VOID *pTmpBuff          - Work Buffer Pointer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     - Individual Financial Total is 0
**              TTL_NOTZERO     - Individual Financial Total is not 0
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function checks the Individual Financial Total is 0.
*
*============================================================================
*/
SHORT TtlTTotalCheckIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff)
{
    SHORT   sRetvalue;

    if (hsTtlIndFinHandle < 0) {    /* Check Individual Financial Total File Handle */
        return (TTL_FILE_HANDLE_ERR);
    }

    if (pTotal->usTerminalNumber == 0) {
		TTLCSINDHEAD    *pIndHead = (VOID  *)pTmpBuff;    /* Set File Header set Buffer Address */

        /* --- Read Individual Financial File Header --- */
        if ((sRetvalue = TtlIndFinHeadRead(pIndHead)) != 0) {   /* Read File Header */
            return (sRetvalue);     /* Return Fail */
        }

        /* --- Set Reset Report Status Offset --- */
        switch(pTotal->uchMinorClass) {
        case CLASS_TTLCURDAY:       /* Current Daily File */
            if (pIndHead->usCurDayTerminal) {  /* Check Current Terminal Number */
                return (TTL_NOTZERO); /* Return Not Zero */
            }
            break;

        case CLASS_TTLSAVDAY:       /* Previous Daily File */
            if (pIndHead->usPreDayTerminal) {  /* Check Previous Terminal Number */
                return (TTL_NOTZERO); /* Return Not Zero */
            }
            break;

        default:
            return (TTL_NOTCLASS);
        }
    } else {
        sRetvalue = TtlreadIndFin(pTotal->uchMinorClass, pTotal->usTerminalNumber, pTmpBuff);   /* Read Individual Financial File */
        switch (sRetvalue) {
        case TTL_SUCCESS:
            return (TTL_NOTZERO);

        case TTL_NOTINFILE:
            break;

        default:
            return (sRetvalue);     /* Return Error */
        }
    }
    return (TTL_SUCCESS);           /* Return Success */
}

/*
*==========================================================================
**  Synopsis:   BOOL TtlTotalCheckPLUNo( UCHAR *puchPluNo )
*
*   Input   :   UCHAR *puchPluNo    - PLU Number
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - All PLU Read.
*                       FALSE   - Specified PLU read
*
**  Description:    This function checks PLU No "0".            Saratoga
*==========================================================================
*/
BOOL    TtlTotalCheckPLUNo(TCHAR *puchPluNo)
{
    if ( puchPluNo[0] == 0 && puchPluNo[1] == 0 && puchPluNo[2] == 0 && puchPluNo[3] == 0 &&
        puchPluNo[4] == 0 && puchPluNo[5] == 0 && puchPluNo[6] == 0 && puchPluNo[7] == 0 &&
        puchPluNo[8] == 0 && puchPluNo[9] == 0 && puchPluNo[10] == 0 && puchPluNo[11] == 0 &&
        puchPluNo[12] == 0 ) {
        return ( TRUE );
    } else {
        return ( FALSE );
    }
}

/* --- End of Source File --- */