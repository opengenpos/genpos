/*
*===========================================================================
* Title       : TOTAL, Total Update module Subroutine
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: Ttlumup.c
* --------------------------------------------------------------------------
* Abstract:
*   VOID  TtlWUpdateFin()               - Update Financial Total in Work Area
*   SHORT TtlCheckFinAffectClass()      - Check Affection Data for Financial
*   VOID  TtlTReadUpdateFin()           - Read Financial Total into Work Area
*   VOID  TtlTUpdateFin()               - Update Financial File
*   VOID  TtlTUpdateHour()              - Update Hourly File
*   SHORT TtlCheckHourAffectClass()     - Check Affection Data for Hourly
*   VOID  TtlWUpdateDept()              - Update Dept Toal in Work Area
*   VOID  TtlSetDeptCounter()           - Set Dept Counter
*   SHORT TtlCheckDeptAffectClass()     - Check Affection Data for Dept
*   SHORT TtlTReadUpdateDept()          - Read Dept Total into Work Area
*   SHORT TtlTUpdateDept()              - Update Dept File
*   VOID  TtlTUpdatePLU()               - Update PLU File
*   SHORT TtlCheckPLUAffectClass()      - Check Affection Data for PLU
*   VOID  TtlWUpdateCpn()               - Update Coupon Total in Work Area, R3.0
*   SHORT TtlCheckCpnAffectClass()      - Check Affection Data for Coupon,  R3.0
*   SHORT TtlTReadUpdateCpn()           - Read Coupon Total into Work Area, R3.0
*   VOID  TtlTUpdateCpn()               - Update Coupon File,               R3.0
*   VOID  TtlWUpdateCas()               - Update Cashier Total in Work Area
*   SHORT TtlCheckCasAffectClass()      - Check Affection Data for Cashier  R3.1
*   SHORT TtlCheckCasNorAffectClass()   - Check Affection Data for Cashier  R3.1
*   SHORT TtlCheckCasBarAffectClass()   - Check Affection Data for Cashier  R3.1
*   SHORT TtlTReadUpdateCas()           - Read Cashier Total into Work Area
*   VOID  TtlTUpdateCas()               - Update Cashier File
*   SHORT TtlWUpdateWai()               - Update Waiter Total in Work Area
*   SHORT TtlCheckWaiAffectClass()      - Check Affection Data for Waiter   R3.1
*   SHORT TtlCheckWaiNorAffectClass()   - Check Affection Data for Waiter   R3.1
*   SHORT TtlCheckWaiBarAffectClass()   - Check Affection Data for Waiter   R3.1
*   VOID  TtlTUpdateWai()               - Update Waiter File
*   VOID  TtlTUpdateSerTime()           - Update Service Time Total,        R3.1
*   SHORT TtlCheckSerTimeAffectClass()  - Check Affection for Service Time, R3.1
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:K.Terai    :
* Mar-07-95:03.00.00:hkato      : R3.0
* Nov-16-95:03.00.01:T.Nakahata : Bug Fixed (not affect coupon at training)
* Dec-11-95:03.01.00:T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Jan-30-96:03.01.00:hkato      : Regular Discount(w/Total Key).
* Feb-13-96:03.01.00:M.Ozawa    : Enhanced to Bartender Affection.
* Mar-21-96:03.01.01:T.Nakahata : Cashier Total Offset USHORT to ULONG
* Jul-15-96:03.01.09:M.Ozawa    : Correct Charge Tips affection to Server
* Aug-11-99:03.05.00:K.Iwata    : R3.5 (remove WAITER_MODEL)
* Aug-11-99:03.05.00:K.Iwata    : Merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include <stdlib.h>

#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <csstbfcc.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <csttl.h>
#include <ttl.h>
#include <regstrct.h>
#include <transact.h>
#include <maint.h>
#include "ttltum.h"
#include <appllog.h>
#include <rfl.h>

ULONG  ulTtlIndFinPosition = 0;    /* Individual Financial Position , R3.1 */


/*
*============================================================================
**Synopsis:     VOID TtlWUpdateFin(TTLTUNTRANQUAL *pTtlTranQualBuff,
*                                  TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUNTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing 
*

** Description  This function updates the Financial Total in Work Area.
*
*============================================================================
*/
VOID TtlWUpdateFin(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    if (TtlCheckFinAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {    /* Check Affection Data for Financial */
        return;                     /* Return */
    }

	/* --- Update Current Daily Financial Total in Work Area--- */
    TtlFinupdate(pTtlTranQualBuff, pTtlTranUpBuff, &TtlFinDayWork);    /* Update Financial Total */

	/* --- Update Current PTD Financial Total in Work Area--- */
    TtlFinupdate(pTtlTranQualBuff, pTtlTranUpBuff, &TtlFinPTDWork);    /* Update Financial Total */

    /* === Individual Financial Total (Relase 3.1) BEGIN === */
    /* --- Update Current Daily Individual Terminal Financial, R3.1 --- */
    TtlFinupdate(pTtlTranQualBuff, pTtlTranUpBuff, &TtlIndFinWork);
    /* === Individual Financial Total (Relase 3.1) END === */

    TtlUpTotalFlag |= TTL_UPTOTAL_FIN;     /* Set Financial Updated Flag */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckFinAffectClass(TTLTUNTRANQUAL *pTtlTranQualBuff, 
*                                            TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUNTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckFinAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction Open */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NEWCHECK:        /* Newcheck */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {   /* Check Training Operation */
                return (TTL_NOTAFFECT);     /* Not Affection */
            }
            break;                          /* Execute Affection */
/*      default:   Not Used */
        }
        break;
        
    case CLASS_ITEMSALES:           /* Sales */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_DEPT:            /* Dept */
        case CLASS_DEPTITEMDISC:    /* Dept W/Item Discount */
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_SETMENU:         /* Set Menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP Menu,                 R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount, R3.0 */
            if ((pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING)) {     /* Check Training Operation */
                if ((pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) ||
                    (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER) ||
                    (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS)) {               
                                    /* Check Preselect Void */
                                    /* Check Momentary Void */
                                    /* Check Credit Item */
                    return (TTL_NOTAFFECT); 
                                    /* Not Affection */
                }
            }
            break;                  /* Execute Affection */

        case CLASS_DEPTMODDISC:     /* Dept W/Modifier Discount */
        case CLASS_PLUMODDISC:      /* PLU W/Modifier discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount,   R3.0 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {                          /* Check Training Operation */
                if (((pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & ITM_MOD_DISC_SUR) == 0) ||
                    (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) ||
                    (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER)) {
                                    /* Check Discount Operation */
                                    /* Check Preselect Void */
                                    /* Check Momentary Void */
                    return (TTL_NOTAFFECT); 
                                    /* Not Affection */
                }
            }
            break;                  /* Execute Affection */
/*      default:   Not Used */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
        case CLASS_REGDISC1:        /* Regular Discount 1 */
        case CLASS_REGDISC2:        /* Regular Discount 2 */
        case CLASS_REGDISC3:        /* Regular Discount 3,  R3.1 */
        case CLASS_REGDISC4:        /* Regular Discount 4,  R3.1 */
        case CLASS_REGDISC5:        /* Regular Discount 5,  R3.1 */
        case CLASS_REGDISC6:        /* Regular Discount 6,  R3.1 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {                /* Check Training Operation */
                if (((pTtlTranUpBuff->TtlItemDisc.auchDiscStatus[1] & TTL_MDC_SURCHARGE) == 0) ||
                    (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) ||
                    (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER)) {
                                    /* Check Discount Operation */
                                    /* Check Preselect Void */
                                    /* Check Momentary Void */
                    return (TTL_NOTAFFECT); 
                                    /* Not Affection */
                }
            }
            break;

        case CLASS_CHARGETIP:       /* Charge Tips */
        case CLASS_CHARGETIP2:      /* Charge Tips, V3.3 */
        case CLASS_CHARGETIP3:      /* Charge Tips, V3.3 */
        case CLASS_AUTOCHARGETIP:   /* Charge Tips, V3.3 */
        case CLASS_AUTOCHARGETIP2:  /* Charge Tips, V3.3 */
        case CLASS_AUTOCHARGETIP3:  /* Charge Tips, V3.3 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {    /* Check Training Operation */
                return (TTL_NOTAFFECT);     /* Not Affection */
            }
            break;                          /* Execute Affection */
        }
        break;

    case CLASS_ITEMCOUPON:          /* Coupon,  R3.0 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_COMCOUPON:       /* Combination Coupon */
        case CLASS_UPCCOUPON:       /* UPC Coupon, saratoga */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {    /* Check Training Operation */
                if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ||
                    pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) {
                                              /* Check Preselect Void */
                                              /* Check Momentary Void */
                    return (TTL_NOTAFFECT);   /* Not Affection */
                }
            }
            break;
        }
        break;

    case CLASS_ITEMTOTAL:           /* Total key */
		{
			UCHAR uchStatus;

			if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {    /* Check Training Operation */
				return (TTL_NOTAFFECT); /* Not Affection */
			}
			uchStatus = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE);     /* Get Total Type */
			if ((uchStatus == PRT_SERVICE1) || (uchStatus == PRT_SERVICE2) ||     /* Check Service Total Operation */
				(uchStatus == PRT_CASINT1) || (uchStatus == PRT_CASINT2)) {       /* Check Cashier Interrupt Total Operation, R3.3 */
				return (TTL_NOTAFFECT);                     /* Not Affect Class */
			}
		}
        break;                                          /* Execute Affection */

    case CLASS_ITEMTENDER:          /* Tender */
        if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {                       /* Check Training Operation */
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;                      /* Execute Affection */

    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NOSALE:          /* No Sale */
        case CLASS_PO:              /* Paid Out */
        case CLASS_RA:              /* Received on Account */
        case CLASS_TIPSPO:          /* Tips Paid Out */
        case CLASS_TIPSDECLARED:    /* Tips Declared */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {     /* Check Training Operation */
                return(TTL_NOTAFFECT);      /* Not Affection */
            }
            break;                          /* Execute Affection */

        case CLASS_MONEY:                   /* Money,   Saratoga */
            return(TTL_NOTAFFECT); 

/*      default:   Not Used */
        }
        break;

    case CLASS_ITEMAFFECTION:       /* Affection */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_HOURLY:          /* Hourly Total */
        case CLASS_SERVICE:         /* Service Total */
        case CLASS_CANCELTOTAL:     /* Cancel Total */
        case CLASS_CASHIERCHECK:    /* Cashier Close Check */
        case CLASS_WAITERCHECK:     /* Waiter Close Check */
        case CLASS_ADDCHECKTOTAL:   /* Addcheck Total */
        case CLASS_MANADDCHECKTOTAL:/* Manual Addcheck Total */
        case CLASS_CASHIEROPENCHECK: /* Casher open check */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {     /* Check Training Operation */
                return(TTL_NOTAFFECT);    /* Not Affection if training operator */
            }
            break;

        case CLASS_TAXAFFECT:       /* Tax for Affection */
        case CLASS_SERVICE_VAT:     /* VATable Service Total, V3.3 */
        case CLASS_SERVICE_NON:     /* Non VATable Service Total, V3.3 */
        case CLASS_VATAFFECT:       /* VAT total, V3.3 */
        case CLASS_LOANAFFECT:      /* Loan,    Saratoga */
        case CLASS_PICKAFFECT:      /* Pickup,  Saratoga */
        case CLASS_LOANCLOSE:       /* Saratoga */
		case CLASS_PICKCLOSE:
		case CLASS_EPT_ERROR_AFFECT:       /* Electronic Payment error to affect totals */
			break;

        default:
			// Waiter functionality, a legacy from NHPOS and NCR 2170 hospitality was not
			// supported in the NCR 7448 NHPOS (Rel 1.4) and touch screen NHPOS (Rel 2.x).
			// Adding this comment for anyone else who is trying to understand Affectation
			// and these legacy defines that are no really functional anymore.
			//    Richard Chambers, Dec-03-2020
			//
			//     CLASS_CHARGETIPWAITER
			//     CLASS_CHARGETIPWAITER2
			//     CLASS_CHARGETIPWAITER3
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

    case CLASS_MAINTLOAN:           /* Saratoga */
    case CLASS_MAINTPICKUP:
        return (TTL_NOTAFFECT);     /* Not Affection */

/*  default: Not Used */
    }
    return (TTL_AFFECT);            /* Affection */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTReadUpdateFin(UCHAR uchTerminalNo)
*
*    Input:     UCHAR   uchTerminalNo   -   Terminal Unique No
*               VOID    *pTmpBuff       -   address of temporary buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function raeds the Financial Total File.
*
*============================================================================
*/
SHORT TtlTReadUpdateFin(USHORT usTerminalNo, TTLCS_TMPBUF_WORKING pTmpBuff )
{
	DATE_TIME   DateTime = {0};       /* Date & Time Work Area */
    SHORT   sRetvalue;
    SHORT   sRetStat;
    ULONG   ulPosition = 0;

    PifGetDateTime(&DateTime);  /* Get Current Date & Time */

	memset (&TtlFinDayWork, 0, sizeof(TtlFinDayWork));
	TtlCopyNdateFromDateTime (&TtlFinDayWork.FromDate, &DateTime);

	memset (&TtlFinPTDWork, 0, sizeof(TtlFinPTDWork));
	TtlCopyNdateFromDateTime (&TtlFinPTDWork.FromDate, &DateTime);

	memset (&TtlIndFinWork, 0, sizeof(TtlIndFinWork));
	TtlCopyNdateFromDateTime (&TtlIndFinWork.FromDate, &DateTime);


    /*********************************************************************
    if ((sRetvalue = TtlreadFin(CLASS_TTLCURDAY, &TtlFinDayWork)) != 0) {
    *********************************************************************/
    if ((sRetvalue = TtlGetFinFilePosition(CLASS_TTLCURDAY, &ulPosition)) != 0) {
        return (sRetvalue);
    }

    /* Read Current Daily Financial File */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &TtlFinDayWork, TTL_FIN_SIZE)) != 0 ) {
        TtlLog(MODULE_TTL_TUM_FINANCIAL, sRetvalue, usTerminalNo);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)abs(sRetvalue));
        PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)__LINE__);
        TtlAbort(MODULE_TTL_TUM_FINANCIAL, sRetvalue);
    }
    /*********************************************************************
    if ((sRetvalue = TtlreadFin(CLASS_TTLCURPTD, &TtlFinPTDWork)) != 0) {
    **********************************************************************/
    if ((sRetvalue = TtlGetFinFilePosition(CLASS_TTLCURPTD, &ulPosition)) != 0) {
        return (sRetvalue);
    }

    /* Read Current PTD Financial File */
    if ((sRetvalue = TtlReadFile(hsTtlBaseHandle, ulPosition, &TtlFinPTDWork, TTL_FIN_SIZE)) != 0 ) {
        TtlLog(MODULE_TTL_TUM_FINANCIAL, sRetvalue, usTerminalNo);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)abs(sRetvalue));
        PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)__LINE__);
        TtlAbort(MODULE_TTL_TUM_FINANCIAL, sRetvalue);
    }
    /* ===== Individual Financial File (Release 3.1) BEGIN ===== */
    if (usTerminalNo == 0) {
        return (TTL_SUCCESS);
    }
    sRetStat = TtlGetIndFinOffset(CLASS_TTLCURDAY, usTerminalNo, &ulTtlIndFinPosition, pTmpBuff);
    switch (sRetStat) {
    case TTL_SUCCESS:
        if ((sRetvalue = TtlReadFile(hsTtlIndFinHandle, ulTtlIndFinPosition, &TtlIndFinWork, TTL_INDTTL_SIZE)) != 0) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_INDFIN, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TUM_INDFIN, sRetvalue, usTerminalNo);
            return (sRetvalue);
        }
        break;

    case TTL_NOTINFILE:             /* Not Exist IndFin File */
        if ((sRetvalue = TtlIndFinAssin(CLASS_TTLCURDAY, usTerminalNo, &ulTtlIndFinPosition, pTmpBuff)) != 0) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_INDFIN, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TUM_INDFIN, sRetvalue, usTerminalNo );
            return (sRetvalue);
        }
        memset(&TtlIndFinWork, 0, sizeof(TtlIndFinWork));
		TtlCopyNdateFromDateTime (&TtlIndFinWork.FromDate, &DateTime);
        if ((sRetvalue = TtlWriteFile(hsTtlIndFinHandle, ulTtlIndFinPosition, &TtlIndFinWork, TTL_INDTTL_SIZE)) != 0) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_INDFIN, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TUM_INDFIN, sRetvalue, usTerminalNo);
            return (sRetvalue);     /* Return Error Status */
        }
        break;

    default:
        TtlAbort(MODULE_TTL_TUM_INDFIN, sRetStat);
    }
    return (TTL_SUCCESS);           /* Return Success */
    /* ===== Individual Financial File (Release 3.1) END ===== */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTUpdateFin(USHORT usTerminalNo)
*
*    Input:     UCHAR   uchTerminalNo   - Terminal Unique Address
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Financial Total File.
*
*============================================================================
*/
SHORT TtlTUpdateFin(USHORT usTerminalNo)
{
    SHORT   sRetvalue;
    ULONG   ulPosition;

    if ((TtlUpTotalFlag & TTL_UPTOTAL_FIN) == 0) {
        return (TTL_SUCCESS);           /* Return */
    }
/* --- Update Current Daily Financial Total --- */
    /*********************************************************************
    if ((sRetvalue = TtlwriteFin(CLASS_TTLCURDAY, &TtlFinDayWork)) != 0) {
    ********************************************************************/
    if ((sRetvalue = TtlGetFinFilePosition(CLASS_TTLCURDAY, &ulPosition)) != 0) {
                                    /* Get Financial Data Position */
        return (sRetvalue);
    }

    /* Write Current Daily Financial File */
    if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPosition, &TtlFinDayWork, TTL_FIN_SIZE)) != 0 ) {
        TtlLog(MODULE_TTL_TUM_FINANCIAL, sRetvalue, usTerminalNo);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)abs(sRetvalue));
        PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)__LINE__);
        TtlAbort(MODULE_TTL_TUM_FINANCIAL, sRetvalue);
    }

/* --- Update Current PTD Financial Total --- */
    /********************************************************************
    if ((sRetvalue = TtlwriteFin(CLASS_TTLCURPTD, &TtlFinPTDWork)) != 0) {
    ********************************************************************/
    if ((sRetvalue = TtlGetFinFilePosition(CLASS_TTLCURPTD, &ulPosition)) != 0) {
                                    /* Get Financial Data Position */
        return (sRetvalue);
    }

    /* Write Current PTD Financial File */
    if ((sRetvalue = TtlWriteFile(hsTtlBaseHandle, ulPosition, &TtlFinPTDWork, TTL_FIN_SIZE)) != 0 ) {
        TtlLog(MODULE_TTL_TUM_FINANCIAL, sRetvalue, usTerminalNo);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)abs(sRetvalue));
        PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_FINANCIAL), (USHORT)__LINE__);
        TtlAbort(MODULE_TTL_TUM_FINANCIAL, sRetvalue);
    }

    /* ===== Individual Financial Total (Release 3.1) BEGIN ===== */
    /* --- Update Current Daily Individual Financial Total (R3.1) --- */
    if ((ulTtlIndFinPosition  == 0) || (usTerminalNo == 0)) {
        return (TTL_SUCCESS);           /* Return Success */
    }
    if ((sRetvalue = TtlWriteFile(hsTtlIndFinHandle, ulTtlIndFinPosition, &TtlIndFinWork, TTL_INDTTL_SIZE)) != 0) {
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TUM_INDFIN, sRetvalue);
            }
        }
        TtlLog(MODULE_TTL_TUM_INDFIN, sRetvalue, usTerminalNo);
        return (sRetvalue);     /* Return Error Status */
    }
    /* ===== Individual Financial Total (Release 3.1) END ===== */

    return (TTL_SUCCESS);           /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTUpdateHour(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                    TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                    VOID *pTmpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Hourly Total File.
*
*============================================================================
*/
SHORT TtlTUpdateHour(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff)
{
	HOURLY  HourlyTotal = {0};
    SHORT   sRetvalue, i;
       
    if (TtlCheckHourAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {  /* Check if Hourly Total Affection Data, TtlTUpdateHour() */
        return (TTL_SUCCESS);       /* Return */
    }
    HourlyTotal.lHourlyTotal = pTtlTranUpBuff->TtlItemAffect.lAmount;                         /* Set Hourly Affection Amount */
    HourlyTotal.lItemproductivityCount = (LONG)pTtlTranUpBuff->TtlItemAffect.sItemCounter;    /* Set Item Productivity Counter */
    HourlyTotal.sDayNoOfPerson = pTtlTranUpBuff->TtlItemAffect.sNoPerson;                     /* Set No of Person */
    /* V3.3 */
    for (i=0; i< TTL_BONUS_MAX; i++) {
        HourlyTotal.lBonus[i] = pTtlTranUpBuff->TtlItemAffect.lBonus[i];
    }

    /* Affect Hourly Total/Counter */
    if ((sRetvalue = TtlHourupdate(&HourlyTotal, pTtlTranUpBuff->TtlItemAffect.uchOffset, (HOURLY *)pTmpBuff)) != 0) {
        TtlLog(MODULE_TTL_TUM_HOURLY, sRetvalue, 0);
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_HOURLY), (USHORT)abs(sRetvalue));
        PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_HOURLY), (USHORT)__LINE__);
        TtlAbort(MODULE_TTL_TUM_HOURLY, sRetvalue);   /* Execute PifAbort */
    }
    return (TTL_SUCCESS);           /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckHourAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
*                                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                  Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckHourAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                              TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction open */
    case CLASS_ITEMSALES:           /* Sales */
    case CLASS_ITEMDISC:            /* Discount */
    case CLASS_ITEMCOUPON:          /* Coupon,     R3.0 */
    case CLASS_ITEMTOTAL:           /* Total key */
    case CLASS_ITEMTENDER:          /* Tender */
    case CLASS_ITEMMISC:            /* Misc. Transaction */
        return (TTL_NOTAFFECT);     /* Not Affection */

    case CLASS_ITEMAFFECTION:       /* Affection */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_HOURLY:          /* Hourly Total */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {     /* Check Training Operation */
                return(TTL_NOTAFFECT);  /* Not Affection if training operator */
            }
            break;
        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;
/*  default: Not Used */
    }
    return (TTL_AFFECT);            /* Affection */
}

/*
*============================================================================
**Synopsis:     SHORT TtlTUpdateDept(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                   TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                   VOID *pTmpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Dept Total File. 2172
*============================================================================
*/
SHORT TtlTUpdateDept(TTLTUMTRANQUAL *pTtlTranQualBuff,
                     TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff)
{
    SHORT      sSign;              /* Buffer for Calculate Sign */
    DCURRENCY  lAmount;            /* Buffer for Calculate Amount */
    LONG       lCounter;           /* Buffer for Counter */
    USHORT     usStatus;           /* Buffer for Affection Status */
    UCHAR      i;
    SHORT      sError;
    SHORT      sRetvalue;
	TCHAR      auchDummy[NUM_PLU_LEN] = {0};

    sError = 0;
    if (TtlCheckDeptAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {
        return (TTL_SUCCESS);       /* Return */
    }
    usStatus = 0;                   /* Set Status */
                                    /* Preselect Void (TTL_TUP_PVOID) */
                                    /* Training       (TTL_TUP_TRAIN) */
                                    /* Momentary Void (TTL_TUP_MVOID) */
    sSign = 1;                      /* Set Sign */
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_PVOID) {       /* Check Preselect Void */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_PVOID;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {   /* Check transaction return */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_TRETURN;  /* Set Preselect Void Status */
        usStatus |= TTL_TUP_IGNORE;   /* Set ignore data status to be reset only if this ia returned item */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {    /* Check Training Operation */
        usStatus |= TTL_TUP_TRAIN;  /* Set Training Status */
    }

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMSALES:           /* Sales */
        if (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER) {
                                          /* Check VOID Modifier */
            sSign *= -1;                  /* Set Sign */
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
			} else {
				usStatus |= TTL_TUP_MVOID;    /* Set Momentary Void Status */
			}
        }

		if (pTtlTranUpBuff->TtlItemSales.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */
		}

        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_PLU:                         /* Saratoga */
        case CLASS_PLUITEMDISC:                 /* Saratoga */
        case CLASS_DEPT:             /* Dept */
        case CLASS_DEPTITEMDISC:     /* Dept W/Item Discount */
        case CLASS_SETMENU:         /* Set menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP menu,                  R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount,  R3.0 */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {  /* Check Credit Dept/PLU Status */
                usStatus |= TTL_TUP_CREDT;    /* Set Credit Dept/PLU Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_HASH) {
				/* Check Hash Dept/PLU Status. See the comments titled What is a Hash Department in rptdept.c */
                usStatus |= TTL_TUP_HASH;     /* Set Hash Dept/PLU Status */
            }
            
            TtlSetDeptCounter(pTtlTranUpBuff, &usStatus, &lCounter);   /* Set Dept Counter */
			if((!ParaMDCCheck(MDC_DOUBLE1_ADR, EVEN_MDC_BIT1)
			   || !ParaMDCCheck(MDC_TRIPLE1_ADR, EVEN_MDC_BIT1))
				   && (pTtlTranUpBuff->TtlItemSales.lDiscountAmount < 0))
		   {
			   sRetvalue = TtlDeptupdate(usStatus,
                             pTtlTranUpBuff->TtlItemSales.usDeptNo,
                             (pTtlTranUpBuff->TtlItemSales.lProduct +
							 pTtlTranUpBuff->TtlItemSales.lDiscountAmount),
                             lCounter,
                             pTmpBuff);
			   
		   }else sRetvalue = TtlDeptupdate(usStatus,
                             pTtlTranUpBuff->TtlItemSales.usDeptNo,
                             pTtlTranUpBuff->TtlItemSales.lProduct,
                             lCounter,
                             pTmpBuff);
            
            if (sRetvalue != 0) {
                                    /* Update Main Dept Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_PLU, sRetvalue);
                                    /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_DEPT, sRetvalue,
                       (USHORT)pTtlTranUpBuff->TtlItemSales.usDeptNo);
                                    /* Record Log */
                sError = 1;
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & 
                 PLU_SCALABLE) {
                                    /* Check Scale Item */
                if (pTtlTranUpBuff->TtlItemSales.usLinkNo) {    /* saratoga */
                    if (lCounter >= 0) {
                        lCounter = 1L;
                    } else {
                        lCounter = -1L;
                    }
                } else {
                    break;
                }
            }
            for (i = 0; i < (pTtlTranUpBuff->TtlItemSales.uchCondNo
                 + pTtlTranUpBuff->TtlItemSales.uchChildNo        /* R3.0 */
                 + pTtlTranUpBuff->TtlItemSales.uchPrintModNo); i++) {
                if ( _tcsncmp(pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) {   /* 2172 */
                /* if ( pTtlTranUpBuff->TtlItemSales.Condiment[i].usPLUNo == 0 ) { */
                    continue;
                }
                usStatus &= ~TTL_TUP_CREDT;
                                    /* Reset Credit Status */
                /* Calculate Condiment Product,             R3.0 */
                lAmount = (DCURRENCY)sSign * labs(lCounter) * pTtlTranUpBuff->TtlItemSales.Condiment[i].lUnitPrice;

                if (pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[0] & PLU_MINUS) {
                    usStatus |= TTL_TUP_CREDT;      /* Set Credit Dept/PLU Status */
                }
                if ((sRetvalue = TtlDeptupdate(usStatus, pTtlTranUpBuff->TtlItemSales.Condiment[i].usDeptNo, 
                          lAmount,
                          lCounter,
                          pTmpBuff)) != 0) {
                                    /* Update Condiment PLU Product */
                    if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                        if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                            TtlAbort(MODULE_TTL_TUM_DEPT, sRetvalue);
                                    /* Execute PifAbort */
                        }
                    }
                    TtlLog(MODULE_TTL_TUM_DEPT, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemSales.Condiment[i].usDeptNo);
                    sError = 1;
                }
            }
            break;                  /* End Affection */

        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount,   R3.0 */
        case CLASS_DEPTMODDISC:     /* Saratoga */
            if ((sRetvalue = TtlDeptupdate(usStatus,
                             pTtlTranUpBuff->TtlItemSales.usDeptNo, 
                             pTtlTranUpBuff->TtlItemSales.lDiscountAmount, 
                             0L,
                             pTmpBuff)) != 0) {
                                    /* Update Modifier Discount Dept Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_DEPT, sRetvalue);     /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_DEPT, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemSales.usDeptNo);
                sError = 1;
            }
            break;
/*      default: Not Used */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
            if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) { /* Check VOID Modifier */
				if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;    /* Set Momentary Void Status */
				}
            }
            if ((sRetvalue = TtlDeptupdate(usStatus, pTtlTranUpBuff->TtlItemDisc.usDeptNo, pTtlTranUpBuff->TtlItemDisc.lAmount, 0L, pTmpBuff)) != 0) {
                                    /* Update Item Discount Dept Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_DEPT, sRetvalue);    /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_DEPT, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemDisc.usDeptNo);
                sError = 1;
            }
            break;                  /* End Affection */
/*      default: Not Used */
        }

    case CLASS_ITEMCOUPON:            /* Coupon, 2172 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_UPCCOUPON:       /* UPC Coupon */
            if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) { /* Check VOID Modifier */
				if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
				}
            }
            if ((sRetvalue = TtlDeptupdate(usStatus,
                              pTtlTranUpBuff->TtlItemCoupon.usDeptNo, 
                              pTtlTranUpBuff->TtlItemCoupon.lAmount, 
                              0L,
                              pTmpBuff)) != 0) {
                                    /* Update Item Discount Dept Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_DEPT, sRetvalue);    /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_DEPT, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemDisc.usDeptNo);
                sError = 1;
            }
            break;                  /* End Affection */
/*      default: Not Used */
        }
        break;
    }
    if (sError) {
        return (TTL_FAIL);          /* Return Fail */
    } else {
        return (TTL_SUCCESS);       /* Return Success */
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlWUpdateDept(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                    TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function updates the Dept Total in Work Area.  Saratoga
*============================================================================
*/
VOID    TtlWUpdateDept(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    UCHAR      auchTmpBuf[512];
    SHORT      sSign;              /* Buffer for Calculate Sign */
    DCURRENCY  lAmount;            /* Buffer for Calculate Amount */
    LONG       lCounter;           /* Buffer for Counter */
    USHORT     usStatus;           /* Buffer for Affection Status */
    UCHAR      i;
	TCHAR      auchDummy[NUM_PLU_LEN] = {0};

    memset(auchTmpBuf, 0, sizeof(auchTmpBuf));

    if (TtlCheckDeptAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {
        return;                     /* Return */
    }
    usStatus = 0;                   /* Set Status */
                                    /* Preselect Void (TTL_TUP_PVOID) */
                                    /* Training       (TTL_TUP_TRAIN) */
                                    /* Momentary Void (TTL_TUP_MVOID) */
    sSign = 1;                      /* Set Sign */
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_PVOID) {      /* Check Preselect Void */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_PVOID;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {   /* Check transaction return */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_TRETURN;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {   /* Check Training Operation */
        usStatus |= TTL_TUP_TRAIN;  /* Set Training Status */
    }
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMSALES:           /* Sales */
        if (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER) {  /* Check VOID Modifier */
            sSign *= -1;                         /* Set Sign */
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
			} else {
				usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
			}
        }
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_DEPT:            /* Dept */
        case CLASS_DEPTITEMDISC:    /* Dept W/Item Discount */
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_SETMENU:         /* Set menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP menu,        R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount, R3.0 */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {
                                    /* Check Credit Dept/PLU Status */
                usStatus |= TTL_TUP_CREDT;    /* Set Credit Dept/PLU Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_HASH) {
                /* Check Hash Dept/PLU Status. See the comments titled What is a Hash Department in rptdept.c */
                usStatus |= TTL_TUP_HASH;     /* Set Hash Dept/PLU Status */
            }

            TtlSetDeptCounter(pTtlTranUpBuff, &usStatus, &lCounter);
                                    /* Set Dept Counter */
            TtlDeptupdate(usStatus,
                          pTtlTranUpBuff->TtlItemSales.usDeptNo, 
                          pTtlTranUpBuff->TtlItemSales.lProduct,
                          lCounter,
                          auchTmpBuf);  /* Saratoga */
                                    /* Update Main Dept Product */

            if ((pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_DEPT) ||
                (pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_DEPTITEMDISC) ||
                (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {
                                    /* Check Scale Item */
                if (pTtlTranUpBuff->TtlItemSales.usLinkNo) {    /* saratoga */
                    if (lCounter >= 0) {
                        lCounter = 1L;
                    } else {
                        lCounter = -1L;
                    }
                } else {
                    break;
                }
            }
            for (i = 0; i < (pTtlTranUpBuff->TtlItemSales.uchCondNo
                + pTtlTranUpBuff->TtlItemSales.uchChildNo
                + pTtlTranUpBuff->TtlItemSales.uchPrintModNo); i++) {

                if ( _tcsncmp(pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) {   /* 2172 */
                    continue;
                }
                usStatus &= ~TTL_TUP_CREDT;      /* Reset Credit Status */

                lAmount = (DCURRENCY)sSign * labs(lCounter) *  pTtlTranUpBuff->TtlItemSales.Condiment[i].lUnitPrice;    /* Calculate Condiment Product */
                if (pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[0] & PLU_MINUS) {
                    usStatus |= TTL_TUP_CREDT;   /* Set Credit Dept/PLU Status */
                }
                TtlDeptupdate(usStatus,
                          pTtlTranUpBuff->TtlItemSales.Condiment[i].usDeptNo, 
                          lAmount,
                          lCounter,
                          auchTmpBuf);
                                    /* Update Condiment Dept Product */
            }
            break;                  /* End Affection */

        case CLASS_DEPTMODDISC:     /* Dept W/Modifier Discount */
        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount, R3.0 */
            TtlDeptupdate(usStatus, pTtlTranUpBuff->TtlItemSales.usDeptNo, pTtlTranUpBuff->TtlItemSales.lDiscountAmount, 0L, auchTmpBuf);  /* Update Modifier Discount Dept Product */
            break;
/*      default: Not Used */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
            if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
				if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
				}
            }
            TtlDeptupdate(usStatus, pTtlTranUpBuff->TtlItemDisc.usDeptNo, pTtlTranUpBuff->TtlItemDisc.lAmount, 0L, auchTmpBuf);           /* Update Item Discount Dept Product */
            break;                  /* End Affection */
/*      default: Not Used */
        }

    case CLASS_ITEMCOUPON:            /* Coupon, 2172 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_UPCCOUPON:       /* UPC Coupon */
            if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) { /* Check VOID Modifier */
				if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
				}
            }
            TtlDeptupdate(usStatus, pTtlTranUpBuff->TtlItemCoupon.usDeptNo, pTtlTranUpBuff->TtlItemCoupon.lAmount, 0L, auchTmpBuf);       /* Update UPC Coupon Dept Product */
            break;                  /* End Affection */
/*      default: Not Used */
        }
        
        break;
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlSetDeptCounter(TTLTUMTRANUPDATE *pTtlTranUpBuff, 
*                                      USHORT *pusStatus,
*                                      LONG *plCounter)
*
*    Input:     TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               USHORT *pusStatus                - Pointer of Status
*               LONG *plCounter                  - Pointer of Counter
*
*   Output:     USHORT *pusStatus                - Status
*               LONG *plCounter                  - Counter
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function Set Dept counter data.
*
*============================================================================
*/
VOID TtlSetDeptCounter(TTLTUMTRANUPDATE *pTtlTranUpBuff, USHORT *pusStatus,
                       LONG *plCounter)
{

    if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) { /* Check Scalable */
        *pusStatus |= TTL_TUP_SCALE;                                /* Set Scalable Dept/PLU Status */
        if (pTtlTranUpBuff->TtlItemSales.lQTY < 0) {
            *plCounter = -1L;
        } else {
            *plCounter = 1L;
        }
    } else {
        *plCounter = (pTtlTranUpBuff->TtlItemSales.lQTY / 1000L);   /* Set Quantity */
    }
}
            
/*
*============================================================================
**Synopsis:     SHORT TtlCheckDeptAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckDeptAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                              TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction open */
    case CLASS_ITEMTOTAL:           /* Total key */
    case CLASS_ITEMTENDER:          /* Tender */
    case CLASS_ITEMMISC:            /* misc. Transaction */
    case CLASS_ITEMAFFECTION:       /* Affection */
        return (TTL_NOTAFFECT);     /* Not Affection */
        
    case CLASS_ITEMSALES:           /* Sales */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_DEPT:            /* Dept */
        case CLASS_DEPTITEMDISC:    /* Dept W/Item Discount */
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_SETMENU:         /* Set Menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP Menu,                 R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount, R3.0 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {   /* Check Training Operation */
                return(TTL_NOTAFFECT);    /* Not Affection if training operator */
            }
            break;                  /* Execute Affection */

        case CLASS_DEPTMODDISC:     /* Dept W/Modifier Discount */
        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount,   R3.0 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {   /* Check Training Operation */
                return(TTL_NOTAFFECT);    /* Not Affection if training operator */
            }
            if((pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & (ITM_MOD_DISC_SUR | ITM_MOD_AFFECT_DEPT_PLU)) == 0) {
                                         /* Check Discount Operation */
                                         /* Check Not Affection Option */
                return(TTL_NOTAFFECT);   /* Not Affection */
            }
            break;                       /* Execute Affection */

/*      default:    Not Used */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {  /* Check Training Operation */
                return(TTL_NOTAFFECT);    /* Not Affection if training operator */
            }
            if ((pTtlTranUpBuff->TtlItemDisc.auchDiscStatus[1] & (TTL_MDC_SURCHARGE + TTL_MDC_NOTAFFECTDEPTPLU)) == 0) {
                                          /* Check Discount Operation */
                                          /* Check Not Affection Option */
                return(TTL_NOTAFFECT);    /* Not Affection */
            }
            break;                        /* Execute Affection */
        default:
            return (TTL_NOTAFFECT);       /* Not Affection */
        }
        break;

    case CLASS_ITEMCOUPON:          /* Coupon,  2172 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_UPCCOUPON:       /* UPC Coupon, saratoga */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {   /* Check Training Operation */
                return(TTL_NOTAFFECT);     /* Not Affection if training operator */
            }
            if (pTtlTranUpBuff->TtlItemCoupon.usDeptNo == 0) { /* Check if valid Dept Number */
                return(TTL_NOTAFFECT);    /* Not Affection */
            }
            break;                        /* Execute Affection */

        default:
            return (TTL_NOTAFFECT);       /* Not Affection */
        }
        break;

/*  default: Not Used */
    
    }
    return (TTL_AFFECT);                  /* Affection */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTUpdatePLU(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                   TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                   VOID *pTmpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the PLU Total File.
*
*============================================================================
*/
SHORT TtlTUpdatePLU(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff)
{
    SHORT      sSign;              /* Buffer for Calculate Sign */
    DCURRENCY  lAmount;            /* Buffer for Calculate Amount */
    LONG       lCounter;           /* Buffer for Counter */
    USHORT     usStatus;           /* Buffer for Affection Status */
    UCHAR      i;
    UCHAR      uchAdjectNo;
    SHORT      sError;
    SHORT      sRetvalue;
	TCHAR      auchDummy[NUM_PLU_LEN] = {0};

    sError = 0;
    if (TtlCheckPLUAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {
        return (TTL_SUCCESS);       /* Return */
    }
    usStatus = 0;                   /* Set Status */
                                    /* Preselect Void (TTL_TUP_PVOID) */
                                    /* Training       (TTL_TUP_TRAIN) */
                                    /* Momentary Void (TTL_TUP_MVOID) */
    sSign = 1;                      /* Set Sign */
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_PVOID) {       /* Check Preselect Void */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_PVOID;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {   /* Check transaction return */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_TRETURN;  /* Set Preselect Void Status */
        usStatus |= TTL_TUP_IGNORE;   /* Set ignore data status to be reset only if this ia returned item */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {    /* Check Training Operation */
        usStatus |= TTL_TUP_TRAIN;  /* Set Training Status */
    }
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMSALES:           /* Sales */
        if (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER) { /* Check VOID Modifier */
            sSign *= -1;                         /* Set Sign */
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
			} else {
				usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
			}
        }

		if (pTtlTranUpBuff->TtlItemSales.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */
		}

		switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_SETMENU:         /* Set menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP menu,                  R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount,  R3.0 */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {
                                    /* Check Credit Dept/PLU Status */
                usStatus |= TTL_TUP_CREDT;         /* Set Credit Dept/PLU Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_HASH) {
                /* Check Hash Dept/PLU Status. See the comments titled What is a Hash Department in rptdept.c */
                usStatus |= TTL_TUP_HASH;          /* Set Hash Dept/PLU Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                                    /* Check Scalable */
                usStatus |= TTL_TUP_SCALE;     /* Set Scalable Dept/PLU Status */
                if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & ITM_AFFECT_DEV_BY_10) {
                                    /* Check Scale Weight Value */
                    lCounter = (pTtlTranUpBuff->TtlItemSales.lQTY / 10L);    /* 0.01 LB/Kg */
                } else {
                    lCounter = (pTtlTranUpBuff->TtlItemSales.lQTY / 100L);   /* 0.1 LB/Kg */
                }
            } else {
                lCounter = (pTtlTranUpBuff->TtlItemSales.lQTY / 1000L);
                                    /* Set Quantity */
			}
			//SR 47 Double/Triple Affect PLU Total
           if((!ParaMDCCheck(MDC_DOUBLE1_ADR, EVEN_MDC_BIT1)
			   || !ParaMDCCheck(MDC_TRIPLE1_ADR, EVEN_MDC_BIT1))
				   && (pTtlTranUpBuff->TtlItemSales.lDiscountAmount < 0))
		   {
			   sRetvalue = TtlPLUupdate(usStatus,
                             pTtlTranUpBuff->TtlItemSales.auchPLUNo,
                             pTtlTranUpBuff->TtlItemSales.uchAdjective,
                             (pTtlTranUpBuff->TtlItemSales.lProduct + 
							 pTtlTranUpBuff->TtlItemSales.lDiscountAmount),
                             lCounter,
                             pTmpBuff);

		   } else 
		   {
#ifdef CREATE_PLUTOTAL_MAX
#pragma message("code snippet below used to create a large database, MUST DECLARE ULONG	j;\z")
			   for( j =1; j < 100000; j ++)
			   {

				   _ltot(j, auchDummy, 10);

				   TtlPLUupdate(usStatus, auchDummy,1, j, lCounter,pTmpBuff);
			   }
#endif



			   sRetvalue = TtlPLUupdate(usStatus,
                             pTtlTranUpBuff->TtlItemSales.auchPLUNo,
                             pTtlTranUpBuff->TtlItemSales.uchAdjective,
                             pTtlTranUpBuff->TtlItemSales.lProduct,
                             lCounter,
                             pTmpBuff);
		   }
                                    /* Update Main PLU Product */

		   if (sRetvalue != 0)
		   {
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_PLU, sRetvalue);    /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_PLU, sRetvalue, 1234);
                /*TtlLog(MODULE_TTL_TUM_PLU, sRetvalue,
                       (USHORT)pTtlTranUpBuff->TtlItemSales.auchPLUNo);*/
                                    /* Record Log */
                sError = 1;
            }


            /* for link plu, saratoga */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                                    /* Check Scale Item */
                if (pTtlTranUpBuff->TtlItemSales.usLinkNo) {
                    if (lCounter >= 0) {
                        lCounter = 1L;
                    } else {
                        lCounter = -1L;
                    }
                } else {
                    break;
                }
            }
            
            for (i = 0; i < (pTtlTranUpBuff->TtlItemSales.uchCondNo
                 + pTtlTranUpBuff->TtlItemSales.uchChildNo        /* R3.0 */
                 + pTtlTranUpBuff->TtlItemSales.uchPrintModNo); i++) {
                if ( _tcsncmp(pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) {   /* 2172 */
                /* if ( pTtlTranUpBuff->TtlItemSales.Condiment[i].usPLUNo == 0 ) { */
                    continue;
                }
                usStatus &= ~TTL_TUP_CREDT;
                                    /* Reset Credit Status */
                if ((pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_SETMENU) ||
                    (pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_OEPPLU) ||                             /* R3.0 */
                    (pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_OEPITEMDISC) ||                        /* R3.0 */
                    (pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_SETITEMDISC)) {
                                    /* Check Set Menu Affection */
                    uchAdjectNo = pTtlTranUpBuff->TtlItemSales.Condiment[i].uchAdjective;     /* Adjective # is used Condiment */
                } else {
                    if ((pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.uchItemType & TTL_COND_ADJ_MASK) == TTL_COND_ADJECT) {
                        uchAdjectNo = pTtlTranUpBuff->TtlItemSales.uchAdjective;               /* Adjective # is used Main Adjective # */ 
                    } else {
                        uchAdjectNo = pTtlTranUpBuff->TtlItemSales.Condiment[i].uchAdjective;  /* Adjective # is used Condiment */
                    }
                }
                /* Calculate Condiment Product,             R3.0 */
                lAmount = (DCURRENCY)sSign * labs(lCounter) * pTtlTranUpBuff->TtlItemSales.Condiment[i].lUnitPrice;

                if (pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[0] & PLU_MINUS) {
                    usStatus |= TTL_TUP_CREDT;  /* Set Credit Dept/PLU Status */
                }

                if ((sRetvalue = TtlPLUupdate(usStatus,
                          pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo, 
                          uchAdjectNo,
                          lAmount,
                          lCounter,
                          pTmpBuff)) != 0) {
                                    /* Update Condiment PLU Product */
                    if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                        if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                            TtlAbort(MODULE_TTL_TUM_PLU, sRetvalue);    /* Execute PifAbort */
                        }
                    }
                    TtlLog(MODULE_TTL_TUM_PLU, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo);
                    sError = 1;
                }

            }
            break;                  /* End Affection */

        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount,   R3.0 */

            if ((sRetvalue = TtlPLUupdate(usStatus,
                             pTtlTranUpBuff->TtlItemSales.auchPLUNo, 
                             pTtlTranUpBuff->TtlItemSales.uchAdjective,
                             pTtlTranUpBuff->TtlItemSales.lDiscountAmount, 
                             0L,
                             pTmpBuff)) != 0) {
                                    /* Update Modifier Discount PLU Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_PLU, sRetvalue);    /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_PLU, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemSales.auchPLUNo);
                sError = 1;
            }

            break;
/*      default: Not Used */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
		if (pTtlTranUpBuff->TtlItemDisc.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */
		}

		if (usStatus & TTL_TUP_IGNORE)
			break;   // skip this discount data
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
            if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
				if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
				}
            }

            if ((sRetvalue = TtlPLUupdate(usStatus, pTtlTranUpBuff->TtlItemDisc.auchPLUNo, pTtlTranUpBuff->TtlItemDisc.uchAdjective,
                              pTtlTranUpBuff->TtlItemDisc.lAmount, 0L, pTmpBuff)) != 0) {
                                    /* Update Item Discount PLU Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_PLU, sRetvalue);    /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_PLU, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemDisc.auchPLUNo);
                sError = 1;
            }

            break;                  /* End Affection */
/*      default: Not Used */
        }
        break;

    case CLASS_ITEMCOUPON:          /* Coupon,  2172 */

		if (pTtlTranUpBuff->TtlItemCoupon.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */
		}

		switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_UPCCOUPON:       /* UPC Coupon, saratoga */
            if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
				if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
				}
            }

            if ((sRetvalue = TtlPLUupdate(usStatus,
                              pTtlTranUpBuff->TtlItemCoupon.auchPLUNo, 
                              pTtlTranUpBuff->TtlItemCoupon.uchAdjective,
                              pTtlTranUpBuff->TtlItemCoupon.lAmount, 
                              0L,
                              pTmpBuff)) != 0) {
                                    /* Update UPC Coupon PLU Product */
                if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                    if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                        TtlAbort(MODULE_TTL_TUM_PLU, sRetvalue);     /* Execute PifAbort */
                    }
                }
                TtlLog(MODULE_TTL_TUM_PLU, sRetvalue, (USHORT)pTtlTranUpBuff->TtlItemCoupon.auchPLUNo);
                sError = 1;
            }

            break;                  /* End Affection */
/*      default: Not Used */
        }
        break;
    }
    if (sError) {
        return (TTL_FAIL);          /* Return Fail */
    } else {
        return (TTL_SUCCESS);       /* Return Success */
    }
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckPLUAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                            TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckPLUAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
	TCHAR   auchDummy[NUM_PLU_LEN] = {0};

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction open */
    case CLASS_ITEMTOTAL:           /* Total key */
    case CLASS_ITEMTENDER:          /* Tender */
    case CLASS_ITEMMISC:            /* misc. Transaction */
    case CLASS_ITEMAFFECTION:       /* Affection */
        return (TTL_NOTAFFECT);     /* Not Affection */
        
    case CLASS_ITEMSALES:           /* Sales */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_SETMENU:         /* Set Menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP Menu,                  R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount,  R3.0 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {
                                    /* Check Training Operation */
                return(TTL_NOTAFFECT);   /* Not Affection if training operator */
            }
            break;                       /* Execute Affection */

        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount, R3.0 */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {
                                         /* Check Training Operation */
                return(TTL_NOTAFFECT);   /* Not Affection if training operator */
            }
            if((pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & (ITM_MOD_DISC_SUR + ITM_MOD_AFFECT_DEPT_PLU)) == 0) {
                                         /* Check Discount Operation */
                                         /* Check Not Affection Option */
                return(TTL_NOTAFFECT);   /* Not Affection */
            }
            break;                       /* Execute Affection */

        default:
            return (TTL_NOTAFFECT);      /* Not Affection */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {
                                         /* Check Training Operation */
                return(TTL_NOTAFFECT);   /* Not Affection if training operator */
            }
            if ((pTtlTranUpBuff->TtlItemDisc.auchDiscStatus[1] & (TTL_MDC_SURCHARGE | TTL_MDC_NOTAFFECTDEPTPLU)) == 0) {
                                          /* Check Discount Operation */
                                          /* Check Not Affection Option */
                return(TTL_NOTAFFECT);    /* Not Affection */
            }
            if ( _tcsncmp(pTtlTranUpBuff->TtlItemDisc.auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) { /* 2172 */
                                    /* Check PLU Number */
                return(TTL_NOTAFFECT);   /* Not Affection if PLU number not specified */
            }
            break;                  /* Execute Affection */
        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

        case CLASS_ITEMCOUPON:          /* Coupon,  R3.0 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_UPCCOUPON:       /* UPC Coupon, saratoga */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {
                                               /* Check Training Operation */
                return(TTL_NOTAFFECT);         /* Not Affection if training operator */
            }
            if ( _tcsncmp(pTtlTranUpBuff->TtlItemCoupon.auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) { /* 2172 */
                                         /* Check PLU Number */
                return(TTL_NOTAFFECT);   /* Not Affection */
            }
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;
/*  default: Not Used */
    }
    return (TTL_AFFECT);            /* Affection */
}
/*
*============================================================================
**Synopsis:     VOID TtlWUpdateCpn(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                    TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function updates the Coupon Total in Work Area.
*                             R3.0
*============================================================================
*/
VOID TtlWUpdateCpn(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    SHORT   sSign;              /* Buffer for Calculate Sign */
    USHORT  usStatus;           /* Buffer for Affection Status */

    if (TtlCheckCpnAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == 
        TTL_NOTAFFECT) {
        return;                     /* Return */
    }
    usStatus = 0;                   /* Set Status */
                                    /* Preselect Void (TTL_TUP_PVOID) */
                                    /* Training       (TTL_TUP_TRAIN) */
                                    /* Momentary Void (TTL_TUP_MVOID) */
    sSign = 1;                      /* Set Sign */
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_PVOID) {       /* Check Preselect Void */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_PVOID;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {   /* Check transaction return */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_TRETURN;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {    /* Check Training Operation */
        usStatus |= TTL_TUP_TRAIN;  /* Set Training Status */
    }

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMCOUPON:          /* Coupon */
        if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
            sSign *= -1;                         /* Set Sign */
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
			} else {
				usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
			}
        }
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_COMCOUPON:       /* Combination Coupon */
            /* === Fix a glitch (11/16/95, Rel3.1) BEGIN === */
            TtlCpnupdate(usStatus, pTtlTranUpBuff->TtlItemCoupon.uchCouponNo, pTtlTranUpBuff->TtlItemCoupon.lAmount, sSign);
            /* TtlCpnupdate(pTtlTranUpBuff->TtlItemCoupon.uchCouponNo, 
                         pTtlTranUpBuff->TtlItemCoupon.lAmount,
                         sSign); */
            /* === Fix a glitch (11/16/95, Rel3.1) END === */
            break;                  /* End Affection */
        case CLASS_UPCCOUPON:       /* UPC Coupon, saratoga */
            /* not affect to combination coupon file */
            break;
        }
        break;
    }
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCpnAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*                             R3.0
*============================================================================
*/
SHORT TtlCheckCpnAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                              TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction open */
    case CLASS_ITEMSALES:           /* Sales */
    case CLASS_ITEMTOTAL:           /* Total key */
    case CLASS_ITEMTENDER:          /* Tender */
    case CLASS_ITEMMISC:            /* misc. Transaction */
    case CLASS_ITEMAFFECTION:       /* Affection */
        return (TTL_NOTAFFECT);     /* Not Affection */

    case CLASS_ITEMCOUPON:          /* Coupon */    
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_COMCOUPON:       /* Combination Coupon */
            if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) { /* Check Training Operation */
                return(TTL_NOTAFFECT);   /* Not Affection if training operator */
            }
            break;                       /* Execute Affection */
        case CLASS_UPCCOUPON:            /* UPC Coupon, saratoga */
            /* not affect to combination coupon file */
            break;
        }
        break;
    }
    return (TTL_AFFECT);            /* Affection */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTReadUpdateCpn()
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed the Updating Coupon Total  
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_SIZEOVER        - Max Size Over
*
** Description  This function updates the Coupon Total File.
*                          R3.0
*============================================================================
*/
SHORT TtlTReadUpdateCpn(VOID)
{
     TTLCSCPNHEAD   Cpnhd;
    ULONG  ulSize;
    SHORT   sRetvalue;

	memset (&TtlCpnDayWork, 0, sizeof(TtlCpnDayWork));
	memset (&TtlCpnPTDWork, 0, sizeof(TtlCpnPTDWork));

    if (hsTtlCpnHandle < 0) {       /* Check Coupon Total File Handle */
		// removed these logs which end up being repeated a lot for installations not using coupons.
//		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CPN_FILE_HANDLE);
//		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
    }
	/* --- Read Coupon File Header --- */
                                    /* Set Coupon File Header Read Buffer Address */
    if ((sRetvalue = TtlCpnHeadRead(&Cpnhd)) != 0) {
                                    /* Read Coupon File Header */
        return (sRetvalue);         /* Return Error Status */
    }

	/* Current Daily File */
    ulSize = TTL_CPNMISC_SIZE + sizeof(TOTAL) * Cpnhd.usMaxCpn;
    if ((sRetvalue = TtlReadFile(hsTtlCpnHandle, (ULONG)(Cpnhd.usCurDayTtlOff), &TtlCpnDayWork, ulSize)) != 0) {
                                    /* Read Coupon Total */
        return (sRetvalue);         /* Rteurn Error Status */
    }

	/* Current PTD File */
    if (Cpnhd.usCurPTDTtlOff == 0) {       
                                    /* Check Exist PTD Total File */
        return(TTL_SUCCESS);        /* Return Not PTD */
     }
    ulSize = TTL_CPNMISC_SIZE + sizeof(TOTAL) * Cpnhd.usMaxCpn;
    if ((sRetvalue = TtlReadFile(hsTtlCpnHandle, (ULONG)(Cpnhd.usCurPTDTtlOff), &TtlCpnPTDWork, ulSize)) != 0) {
                                    /* Read Coupon Total */
        return (sRetvalue);         /* Rteurn Error Status */
    }

     return (TTL_SUCCESS);          /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTUpdateCpn()
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed the Updating Coupon Total  
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*               TTL_SIZEOVER        - Max Size Over
*
** Description  This function updates the Coupon Total File.
*                             R3.0
*============================================================================
*/
SHORT TtlTUpdateCpn(VOID)
{
     TTLCSCPNHEAD   Cpnhd;
    USHORT  usSize;
    SHORT   sRetvalue;

    if  ((TtlUpTotalFlag & TTL_UPTOTAL_CPN) == 0) {
        return (TTL_SUCCESS);           /* Return */
    }
    if (hsTtlCpnHandle < 0) {       /* Check Coupon Total File Handle */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CPN_FILE_HANDLE);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
     }
	/* --- Read Coupon File Header --- */
                                    /* Set Coupon File Header Read Buffer Address */
    if ((sRetvalue = TtlCpnHeadRead(&Cpnhd)) != 0) {
                                    /* Read Coupon File Header */
        return (sRetvalue);         /* Return Error Status */
    }


	/* Current Daily File */
    usSize = sizeof(TTLCSCPNMISC) + sizeof(TOTAL) * Cpnhd.usMaxCpn;

    if ((sRetvalue = TtlWriteFile(hsTtlCpnHandle, (ULONG)(Cpnhd.usCurDayTtlOff), &TtlCpnDayWork, usSize)) != 0) {
                                /* Write Coupon Total */
        return (sRetvalue);     /* Rteurn Error Status */
    }

	/* Current PTD File */
    if (Cpnhd.usCurPTDTtlOff == 0) {       
                                    /* Check Exist PTD Total File */
        return(TTL_SUCCESS);        /* Return Not PTD */
     }
    usSize = sizeof(TTLCSCPNMISC) + sizeof(TOTAL) * Cpnhd.usMaxCpn;

    if ((sRetvalue = TtlWriteFile(hsTtlCpnHandle, (ULONG)(Cpnhd.usCurPTDTtlOff), &TtlCpnPTDWork, usSize)) != 0) {
                                /* Write Coupon Total */
        return (sRetvalue);     /* Rteurn Error Status */
    }

     return (TTL_SUCCESS);          /* Return Success */
}

/*
*============================================================================
**Synopsis:     VOID TtlWUpdateCas(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                   TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function updates the Cashier Total in Work Area.
*
*============================================================================
*/
VOID TtlWUpdateCas(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff)
{

    if (TtlCheckCasAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {    /* Check Affection Data */
        return;                     /* Return Success */
    }
    if (pTtlTranQualBuff->TtlTranModeQual.ulCashierID == 0) {                           /* if Cashier ID is zero then we just return with no update */
        return;                     /* Return */
    }
	TtlCasDayWork.uchResetStatus |= TTL_NOTRESET;
    TtlCasupdate(pTtlTranQualBuff, pTtlTranUpBuff, &TtlCasDayWork);    /* Update Cashier Daily Total */
	TtlCasPTDWork.uchResetStatus |= TTL_NOTRESET;
    TtlCasupdate(pTtlTranQualBuff, pTtlTranUpBuff, &TtlCasPTDWork);    /* Update Cashier PTD Total, V3.3 */
    TtlUpTotalFlag |= TTL_UPTOTAL_CAS;                                 /* Set Cashier Total Updated Flag */         
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCasAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                            TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckCasAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    if (pTtlTranQualBuff->TtlTranModeQual.fsModeStatus & MODEQUAL_BARTENDER) {
        /* Bartender Operation */
        return(TtlCheckCasBarAffectClass(pTtlTranQualBuff, pTtlTranUpBuff));
    } else {
        /* Cashier Operation */
        return(TtlCheckCasNorAffectClass(pTtlTranQualBuff, pTtlTranUpBuff));
    }
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCasNorAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                            TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckCasNorAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    UCHAR   uchStatus;
    ULONG   ulCashierNumber;

    if (!((pTtlTranQualBuff->TtlTranModeQual.ulCashierID != 0) && (pTtlTranQualBuff->TtlTranModeQual.ulWaiterID == 0))) {
                                    /* Cashier Operation */
            return (TTL_NOTAFFECT); /* Not Affection */
    }

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction Open */
    case CLASS_ITEMSALES:           /* Sales */
    case CLASS_ITEMDISC:            /* Discount */
    case CLASS_ITEMCOUPON:          /* Coupon,     R3.0 */
    case CLASS_ITEMTENDER:          /* Tender */
        break;                      /* Execute Affection */

    case CLASS_ITEMTOTAL:           /* Total key */
        uchStatus = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE);   /* Get Total Type */
        if ((uchStatus == PRT_SERVICE1) || (uchStatus == PRT_SERVICE2) ||     /* Check Service Total Operation */
            (uchStatus == PRT_CASINT1) || (uchStatus == PRT_CASINT2)) {       /* Check Cashier Interrupt Total Operation, R3.3 */
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;                      /* Execute Affection */

    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NOSALE:          /* No Sale */
        case CLASS_PO:              /* Paid Out */
        case CLASS_RA:              /* Received on Account */
        case CLASS_TIPSPO:          /* Tips Paid Out */
        case CLASS_TIPSDECLARED:    /* Tips Declared, V3.3 */
        case CLASS_MONEY:           /* Money Declaration,   Saratoga */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

    case CLASS_ITEMAFFECTION:       /* affection */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_HOURLY:          /* Hourly Total */
        case CLASS_SERVICE:         /* Service Total */
        case CLASS_CANCELTOTAL:     /* Cancel Total */
        case CLASS_CASHIERCHECK:    /* Cashier Close Check */
        case CLASS_ADDCHECKTOTAL:   /* Addcheck Total */
        case CLASS_MANADDCHECKTOTAL:/* Manual Addcheck Total */
        case CLASS_CASHIEROPENCHECK: /* Casher open check */
        case CLASS_TAXAFFECT:       /* Tax for Affection */
        case CLASS_SERVICE_VAT:     /* VATable Service Total, V3.3 */
        case CLASS_SERVICE_NON:     /* Non VATable Service Total, V3.3 */
        case CLASS_VATAFFECT:       /* VAT total, V3.3 */
        case CLASS_LOANAFFECT:      /* Loan,    Saratoga */
        case CLASS_PICKAFFECT:      /* Pickup,  Saratoga */
        case CLASS_LOANCLOSE:       /* Loan,    Saratoga */
        case CLASS_PICKCLOSE:       /* Pickup,  Saratoga */
		case CLASS_EPT_ERROR_AFFECT:       /* Electronic Payment error to affect totals */
			break;

        case CLASS_NEWWAITER:           /* Check Transfer New Cashier, V3.3 */
        case CLASS_OLDWAITER:           /* Check Transfer Old Cashier, V3.3 */
            if ((ulCashierNumber = pTtlTranUpBuff->TtlItemAffect.ulId) == 0) {
                                        /* Set Cashier Number */
                return (TTL_NOTAFFECT); /* Not Affection if no cashier number */
            }
            if (ulCashierNumber != pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
                return (TTL_NOTAFFECT); /* Not Affection */
            }
            break;

        default:
			// Waiter functionality, a legacy from NHPOS and NCR 2170 hospitality was not
			// supported in the NCR 7448 NHPOS (Rel 1.4) and touch screen NHPOS (Rel 2.x).
			// Adding this comment for anyone else who is trying to understand Affectation
			// and these legacy defines that are no really functional anymore.
			//    Richard Chambers, Dec-03-2020
			//
			//     CLASS_CHARGETIPWAITER
			//     CLASS_CHARGETIPWAITER2
			//     CLASS_CHARGETIPWAITER3
			return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;
/*  default: Not Used */
    }
    return (TTL_AFFECT);            /* Affection */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCasBarAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                            TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckCasBarAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                             TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    UCHAR   uchStatus;

    if (!((pTtlTranQualBuff->TtlTranModeQual.ulCashierID != 0) && 
          (pTtlTranQualBuff->TtlTranModeQual.ulWaiterID != 0))) {
                                    /* Bartender Operation */
            return (TTL_NOTAFFECT); /* Not Affection */
    }

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction Open */
    case CLASS_ITEMSALES:           /* Sales */
    case CLASS_ITEMDISC:            /* Discount */
    case CLASS_ITEMCOUPON:          /* Coupon,     R3.0 */
    case CLASS_ITEMOTHER:           /* Post Receipt */
        return (TTL_NOTAFFECT); /* Not Affection */

    case CLASS_ITEMTENDER:          /* Tender */
        break;                      /* Execute Affection */

    case CLASS_ITEMTOTAL:           /* Total key */
        uchStatus = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE);  /* Get Total Type */
        if ((uchStatus == PRT_SERVICE1) || (uchStatus == PRT_SERVICE2) ||     /* Check Service Total Operation */
            (uchStatus == PRT_CASINT1) || (uchStatus == PRT_CASINT2)) {       /* Check Cashier Interrupt Total Operation, R3.3 */
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;                      /* Execute Affection */

    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NOSALE:          /* No Sale */
        case CLASS_PO:              /* Paid Out */
        case CLASS_RA:              /* Received on Account */
        case CLASS_TIPSPO:          /* Tips Paid Out */
        case CLASS_MONEY:           /* Money,   Saratoga */
        /* case CLASS_TIPSDECLARED:    / Tips Declared */
            break;                  /* Execute Affection */

        case CLASS_TIPSDECLARED:    /* Tips Declared */
        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

    case CLASS_ITEMAFFECTION:       /* affection */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_HOURLY:          /* Hourly Total */
        case CLASS_SERVICE:         /* Service Total */
        case CLASS_CANCELTOTAL:     /* Cancel Total */
        case CLASS_CASHIERCHECK:    /* Cashier Close Check */
        case CLASS_ADDCHECKTOTAL:   /* Addcheck Total */
        case CLASS_MANADDCHECKTOTAL:/* Manual Addcheck Total */
        case CLASS_CASHIEROPENCHECK: /* Casher open check */
            break;

        case CLASS_TAXAFFECT:       /* Tax for Affection */
        case CLASS_SERVICE_VAT:     /* VATable Service Total, V3.3 */
        case CLASS_SERVICE_NON:     /* Non VATable Service Total, V3.3 */
        case CLASS_VATAFFECT:       /* VAT total, V3.3 */
        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;
/*  default: Not Used */
    }
    return (TTL_AFFECT);            /* Affection */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTReadUpdateCas(USHORT usCashierNumber VOID *pTmpBuff)
*
*    Input:     USHORT  usCashierNumber          - Cashier Number
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Cashier Total File. V3.3
*
*============================================================================
*/
SHORT TtlTReadUpdateCas(ULONG ulCashierNumber, VOID *pTmpBuff, UCHAR uchMinorClass)
{
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    SHORT   sRetStat;
    SHORT   sRetvalue;
    ULONG   *pulPosition;
    TTLCSCASTOTAL   *pCasWork;

    /* --- Update Current Daily/PTD Cashier Total, V3.3 --- */
    if (uchMinorClass == CLASS_TTLCURDAY) {
        pulPosition = &ulTtlCasDayPosition;
        pCasWork    = &TtlCasDayWork;
    } else {
        pulPosition = &ulTtlCasPTDPosition;
        pCasWork    = &TtlCasPTDWork;
    }

    PifGetDateTime(&DateTime);  /* Get Current Date & Time */
    memset(pCasWork, 0, sizeof(TTLCSCASTOTAL));     /* Clear Cashier Total */
	TtlCopyNdateFromDateTime (&(pCasWork->FromDate), &DateTime);

    sRetStat = TtlGetCasOffset(uchMinorClass, ulCashierNumber, pulPosition, pTmpBuff);
                                    /* Get Total Offset */
    switch (sRetStat) {
    case TTL_SUCCESS:               /* Exist Cashier File */
        sRetvalue = TtlCasTotalRead(*pulPosition, pCasWork);
        if (sRetvalue != 0) {
                                    /* Read Cashier Total File */
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);    /* Execute PifAbort */
                }
            }
            TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
            return (sRetvalue);     /* Return Error Status */
        }
        break;

    case TTL_NOTINFILE:             /* Not Exist Cashier File */
        if ((sRetvalue = TtlCasAssin(uchMinorClass, ulCashierNumber, pulPosition, pTmpBuff)) != 0) {
                                    /* Assin Cashier Total */
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);    /* Execute PifAbort */
                }
            }
            TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
            return (sRetvalue);     /* Return Error Status */
        }
        memset(pCasWork, 0, sizeof(TTLCSCASTOTAL));   /* Clear Cashier Total */
        PifGetDateTime(&DateTime);
		TtlCopyNdateFromDateTime (&(pCasWork->FromDate), &DateTime);          /* Set From Date */
        if ((sRetvalue = TtlCasTotalWrite(*pulPosition, pCasWork)) != 0) {    /* Write Current Daily Cashier File */
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);    /* Execute PifAbort */
                }
            }
            TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
            return (sRetvalue);     /* Return Error Status */
        }
        break;

    case TTL_NOTPTD:
        return (TTL_SUCCESS);       /* Return */

    default:
        TtlAbort(MODULE_TTL_TUM_CASHIER, sRetStat);
                                    /* Execute PifAbort */
    }

    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlTUpdateCas(USHORT usCashierNumber)
*
*    Input:     USHORT  usCashierNumber          - Cashier Number
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Cashier Total File.
*
*============================================================================
*/
SHORT TtlTUpdateCas(ULONG ulCashierNumber, UCHAR uchMinorClass)
{
    SHORT   sRetvalue;
    ULONG    *pulPosition;
    TTLCSCASTOTAL   *pCasWork;

    if ((TtlUpTotalFlag & TTL_UPTOTAL_CAS) == 0) {
        return (TTL_SUCCESS);           /* Return */
    }

    /* --- Update Current Daily/PTD Cashier Total, V3.3 --- */
    if (uchMinorClass == CLASS_TTLCURDAY) {
        pulPosition = &ulTtlCasDayPosition;
        pCasWork    = &TtlCasDayWork;
    } else {
        pulPosition = &ulTtlCasPTDPosition;
        pCasWork    = &TtlCasPTDWork;
    }

    if (*pulPosition  == 0UL) {
        return (TTL_SUCCESS);           /* Return Success */
    }

    /* Write Current Daily Cashier File */
    if ((sRetvalue = TtlCasTotalWrite(*pulPosition, (VOID *)pCasWork)) != 0) {
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);
            }
        }
        TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
        return (sRetvalue);
    }
    return (TTL_SUCCESS);
}

/*
*============================================================================
**Synopsis:     SHORT TtlWUpdateCasWai(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                   TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                   VOID *pTmpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Cashier(Surrogate) Total File. V3.3
*
*============================================================================
*/
SHORT TtlWUpdateCasWai(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff)
{
    ULONG    ulCashierNumber;   /* Cashier Number */
    SHORT    sRetStat;

    if (TtlCheckCasWaiAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {
                                    /* Check Affection Data */
        return (TTL_SUCCESS);       /* Return */
    }

    /* --- Check affectable as waiter operation or not --- */

    switch (pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_TIPSPO:          /* Tips Paid Out */
            if ((ulCashierNumber = pTtlTranUpBuff->TtlItemMisc.ulID) == 0) {
                                    /* Set Cashier Number */
                return (TTL_SUCCESS); /* Return */
            }
            if (ulCashierNumber == pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
                return (TTL_SUCCESS); /* Return */
            }
            break;

        default:
            if ((ulCashierNumber =  pTtlTranQualBuff->TtlTranModeQual.ulWaiterID) == 0) {
                                    /* Set Cashier Number */
                return (TTL_SUCCESS); /* Return */
            }
            if (ulCashierNumber == pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
                return (TTL_SUCCESS); /* Return */
            }
            break;
        }
        break;

    case CLASS_ITEMAFFECTION:
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_CHARGETIPWAITER:     /* Charge Tip Amount for Cashier */
        case CLASS_CHARGETIPWAITER2:     /* Charge Tip Amount for Cashier, V3.3 */
        case CLASS_CHARGETIPWAITER3:     /* Charge Tip Amount for Cashier, V3.3 */
            if (pTtlTranQualBuff->TtlTranModeQual.fsModeStatus & MODEQUAL_BARTENDER) {
                if ((ulCashierNumber = pTtlTranQualBuff->TtlTranModeQual.ulWaiterID) == 0) {
                                        /* Set Cashier Number */
                    return (TTL_SUCCESS); /* Return */
                }
            } else {
                if ((ulCashierNumber = pTtlTranUpBuff->TtlItemAffect.ulId) == 0) {
                                        /* Set Cashier Number */
                    return (TTL_SUCCESS); /* Return */
                }
            }
            if (ulCashierNumber == pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
                return (TTL_SUCCESS); /* Return */
            }
            break;

        case CLASS_NEWWAITER:           /* Check Transfer New Cashier */
        case CLASS_OLDWAITER:           /* Check Transfer Old Cashier */
            if ((ulCashierNumber = pTtlTranUpBuff->TtlItemAffect.ulId) == 0) {
                                       /* Set Cashier Number */
                return (TTL_SUCCESS); /* Return */
            }
            if (ulCashierNumber == pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
                return (TTL_SUCCESS); /* Return */
            }
            break;

        default:
            if ((ulCashierNumber = pTtlTranQualBuff->TtlTranModeQual.ulWaiterID) == 0) {
                                    /* Set Cashier Number */
                return (TTL_SUCCESS); /* Return */
            }
            if (ulCashierNumber == pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
                return (TTL_SUCCESS); /* Return */
            }
        }
        break;
    default:
        if ((ulCashierNumber = pTtlTranQualBuff->TtlTranModeQual.ulWaiterID)
             == 0) {                /* Set Cashier Number */
                return (TTL_SUCCESS); /* Return */
        }
        if (ulCashierNumber == pTtlTranQualBuff->TtlTranModeQual.ulCashierID) {
            return (TTL_SUCCESS); /* Return */
        }
        break;
    }

    if (TtlUphdata.uchResetFlag) {
        return (TTL_SUCCESS);           /* Return Success */

    } 

    /* V3.3 */
    /* --- Update Current Daily Cashier Total --- */
    sRetStat = TtlWUpdateCasWaiSub(pTtlTranQualBuff,pTtlTranUpBuff, pTmpBuff, ulCashierNumber, CLASS_TTLCURDAY);
    if (sRetStat) {
        return(sRetStat);
    }
    /* --- Update Current PTD Cashier Total --- */
    sRetStat = TtlWUpdateCasWaiSub(pTtlTranQualBuff,pTtlTranUpBuff, pTmpBuff, ulCashierNumber, CLASS_TTLCURPTD);
    if (sRetStat) {
        return(sRetStat);
    }
    return (TTL_SUCCESS);           /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT TtlWUpdateCasWaiSub(USHORT usCashierNumber VOID *pTmpBuff)
*
*    Input:     USHORT  usCashierNumber          - Cashier Number
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Cashier Total File. V3.3
*
*============================================================================
*/
SHORT TtlWUpdateCasWaiSub(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff,
                    ULONG ulCashierNumber, UCHAR uchMinorClass)
{
    ULONG       ulPositionTtl;     /* Save Seek Position */
    DATE_TIME   DateTime;       /* Date & Time Work Area */
    TTLCSCASTOTAL *pCasTotal;   /* Pointer Cashier Total */
    SHORT       sRetStat;
    SHORT       sRetvalue;

    pCasTotal = (TTLCSCASTOTAL *)pTmpBuff;
    sRetStat = TtlGetCasOffset(uchMinorClass, ulCashierNumber, &ulPositionTtl, pTmpBuff);    /* Get Total Offset */
    switch (sRetStat) {
    case TTL_SUCCESS:                                                          /* Exist Cashier File.   Read Cashier Total File */
        if ((sRetvalue = TtlCasTotalRead(ulPositionTtl, pTmpBuff)) != 0) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
            return (sRetvalue);
        }
        break;

    case TTL_NOTINFILE:                                                       /* Not Exist Cashier File.   Assin Cashier Total */
        if ((sRetvalue = TtlCasAssin(uchMinorClass, ulCashierNumber, &ulPositionTtl, pTmpBuff)) != 0) {
            if (sRetvalue <= TTL_FILE_OPEN_ERR) {
                if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                    TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);
                }
            }
            TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
            return (sRetvalue);
        }
        PifGetDateTime(&DateTime);                 /* Get Current Date & Time */
        memset(pTmpBuff, 0, TTL_CASTTL_SIZE);      /* Clear cashier Total */
		TtlCopyNdateFromDateTime (&pCasTotal->FromDate, &DateTime);  /* Set From Date */
        break;

    case TTL_NOTPTD:
        return (TTL_SUCCESS);

    default:
        TtlAbort(MODULE_TTL_TUM_CASHIER, sRetStat);
    }

    TtlCasupdate(pTtlTranQualBuff, pTtlTranUpBuff, (TTLCSCASTOTAL *)pTmpBuff);   /* Update Cashier Total */
    /* Write Current Daily Cashier File */
	((TTLCSCASTOTAL *)pTmpBuff)->uchResetStatus |= TTL_NOTRESET;
    if ((sRetvalue = TtlCasTotalWrite(ulPositionTtl, pTmpBuff)) != 0) {
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TUM_CASHIER, sRetvalue);
            }
        }
        TtlLog(MODULE_TTL_TUM_CASHIER, sRetvalue, (USHORT)ulCashierNumber);
        return (sRetvalue);
    }
    return (TTL_SUCCESS);
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCasWaiAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
*                                               TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                 Qualifier Data
*
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection., V3.3
*
*============================================================================
*/
SHORT TtlCheckCasWaiAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                                    TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    if (pTtlTranQualBuff->TtlTranModeQual.fsModeStatus & MODEQUAL_BARTENDER) {
        /* Bartender Operation */
        return(TtlCheckCasWaiBarAffectClass(pTtlTranUpBuff));
    } else {
        /* Normal Operation */
        return(TtlCheckCasWaiNorAffectClass(pTtlTranUpBuff));
    }
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCasWaiBarAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection. V3.3
*
*============================================================================
*/
SHORT TtlCheckCasWaiBarAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff)
{

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction Open */
    case CLASS_ITEMSALES:           /* Sales */
    case CLASS_ITEMCOUPON:          /* Coupon,        R3.0 */
    case CLASS_ITEMDISC:            /* Discount */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_CHARGETIP:       /* Charge Tips */
        case CLASS_CHARGETIP2:      /* Charge Tips, V3.3 */
        case CLASS_CHARGETIP3:      /* Charge Tips, V3.3 */
        case CLASS_AUTOCHARGETIP:   /* Charge Tips, V3.3 */
        case CLASS_AUTOCHARGETIP2:  /* Charge Tips, V3.3 */
        case CLASS_AUTOCHARGETIP3:  /* Charge Tips, V3.3 */
            return (TTL_NOTAFFECT); /* Not Affect Class */
        default:
            break;
        }
        break;                      /* Execute Affection */


    case CLASS_ITEMTENDER:          /* Tender */
    case CLASS_ITEMTOTAL:           /* Total key */
        return (TTL_NOTAFFECT); /* Not Affect Class */

    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_TIPSPO:          /* Tips Paid Out */
        case CLASS_TIPSDECLARED:    /* Tips Declared */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

    case CLASS_ITEMAFFECTION:       /* Affection */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_HOURLY:          /* Hourly Total */
        case CLASS_WAITERCHECK:     /* Waiter Close Check */
        case CLASS_NEWWAITER:       /* Check Transfer New Waiter */
        case CLASS_OLDWAITER:       /* Check Transfer Old Waiter */
        case CLASS_CHARGETIPWAITER: /* Charge Tip Amount for Waiter */
        case CLASS_CHARGETIPWAITER2: /* Charge Tip Amount for Waiter, V3.3 */
        case CLASS_CHARGETIPWAITER3: /* Charge Tip Amount for Waiter, V3.3 */
        case CLASS_TAXAFFECT:       /* Tax for Affection */
        case CLASS_SERVICE_VAT:     /* VATable Service Total, V3.3*/
        case CLASS_SERVICE_NON:     /* Non VATable Service Total, V3.3 */
        case CLASS_VATAFFECT:       /* VAT total, V3.3 */
            break;

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;
/*  default: Not Used */
    }
    return (TTL_AFFECT);            /* Affection */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckCasWaiNorAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                 Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection. V3.3
*
*============================================================================
*/
SHORT TtlCheckCasWaiNorAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff)
{

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_TIPSPO:          /* Tips Paid Out */
            return (TTL_AFFECT);            /* Affection */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }

    case CLASS_ITEMAFFECTION:       /* Affection */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_CHARGETIPWAITER: /* Charge Tip Amount for Waiter */
        case CLASS_CHARGETIPWAITER2: /* Charge Tip Amount for Waiter, V3.3 */
        case CLASS_CHARGETIPWAITER3: /* Charge Tip Amount for Waiter, V3.3 */
        case CLASS_NEWWAITER:       /* Check Transfer New Waiter */
        case CLASS_OLDWAITER:       /* Check Transfer Old Waiter */
            return (TTL_AFFECT);            /* Affection */
            break;

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

    case CLASS_ITEMOTHER:           /* Post Receipt Co. affection, V3.3 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_POSTRECEIPT:     /* Post Receipt */
        case CLASS_POSTRECPREBUF:   /* Post Receipt for Precheck */
        case CLASS_PARKING:         /* Parking Print */
            return (TTL_AFFECT);            /* Affection */
            break;

        default:
            return (TTL_NOTAFFECT); /* Not Affection */
        }
        break;

/*  default: Not Used */
    }
    return (TTL_NOTAFFECT); /* Not Affection */
}

/*************** New Functions (Release 3.1) BEGIN ****************************/
/*
*============================================================================
**Synopsis:     SHORT TtlTUpdateSerTime(TTLTUMTRANQUAL   *pTtlTranQualBuff, 
*                                       TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                       VOID             *pTmpBuff)
*
*    Input:     TTLTUMTRANQUAL   *pTtlTranQualBuff  - Pointer of Transaction 
*                                                       Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff    - Pointer of Transaction
*                                                       Item Data
*               VOID             *pTmpBuff          - Pointer of Work Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Affection 
*               TTl_FAIL            - Failed Affection
*
** Description  This function updates the Service Time Total File.
*
*============================================================================
*/
SHORT TtlTUpdateSerTime(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                        TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff)
{
    TOTAL   ServiceTime;
    UCHAR   uchHourOffset;
    UCHAR   uchTimeBorder;
    SHORT   sRetvalue;
       
    if (TtlCheckSerTimeAffectClass(pTtlTranQualBuff, pTtlTranUpBuff) == TTL_NOTAFFECT) {        /* Check Affection Data */
        return (TTL_SUCCESS);       /* Return */
    }

    ServiceTime.lAmount  = pTtlTranUpBuff->TtlItemAffect.lAmount;
    ServiceTime.sCounter = pTtlTranUpBuff->TtlItemAffect.sItemCounter;

    uchHourOffset = ( UCHAR )pTtlTranUpBuff->TtlItemAffect.uchOffset;
    uchTimeBorder = ( UCHAR )pTtlTranUpBuff->TtlItemAffect.ulId;

    if ((sRetvalue = TtlSerTimeupdate(&ServiceTime, uchHourOffset, uchTimeBorder, (TOTAL *)pTmpBuff)) != 0) {
                                    /* Affect Service Time Total/Counter */
        TtlAbort(MODULE_TTL_TUM_SERTIME, sRetvalue);    /* Execute PifAbort if affection failed */
    }
    return (TTL_SUCCESS);           /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckSerTimeAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
*                                                TTLTUMTRANUPDATE *pTtlTranUpBuff)
*
*    Input:     TTLTUMTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                  Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data 
*
** Description  This function checks Class of Transaction Data for affection.
*
*============================================================================
*/
SHORT TtlCheckSerTimeAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff,
                                 TTLTUMTRANUPDATE *pTtlTranUpBuff)
{
    SHORT   sAffection = TTL_NOTAFFECT;
    
    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {

    case CLASS_ITEMAFFECTION:       /* Affection Class */

        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {

        case CLASS_SERVICETIME:         /* Service Time Class */
            /* --- Is operator the normal operator ? --- */
            if ( ! (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING)) {
                sAffection = TTL_AFFECT;
            }
            break;
            
        default:
            break;
        }
    default:
        break;
    }

    return (sAffection);
}


/*************** New Functions (Release 3.1) END ****************************/

/* ===== End of File (TTLUMUP.C) ===== */