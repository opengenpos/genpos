/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Cashier Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFCAS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifCashier() : Reg Cashier
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-07-92:00.00.01:M.Suzuki   : initial                                   
* Jul-01-93:01.00.12:K.You      : add post receipt feature.                                    
* Jul-20-93:01.00.12:K.You      : add EJ reverse feature.                                    
* Dec-07-93:02.00.03:K.You      : del order display feature.                                    
* Jan-12-95:        :hkato      : R3.0, Recover order display feature.                                    
* Jun-02-98:03.03.00:M.Ozawa    : add cashier interrupt key.
* Jul-31-98:03.03.00:hrkato     : V3.3 (New Key Sequence)
* Aug-11-99:03.05.00:K.Iwata    : R3.5 (remove WAITER_MODEL)
* Nov-25-99:01.00.00:hrkato     : Money
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
#include <ecr.h>
#include <pif.h>
#include <uie.h>
#include <pifmain.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegCasSignIn[] = {{UifCashierSignIn, CID_CASHIERSIGNIN},
                                        {UifCasLockSignInOut, CID_CASLOCKSIGNINOUT},/* V3.3 */
                                        {NULL, 0}};
UIMENU FARCONST aChildRegCasTran[] = {{UifCashierSignOut, CID_CASHIERSIGNOUT},
                                      {UifTransVoid, CID_TRANSVOID},
                                      {UifWICTran, CID_WICTRAN},        /* Saratoga */
                                      {UifSales, CID_SALES},
                                      {UifNewCheck, CID_NEWCHECK},
                                      {UifNewKeySeq, CID_NEWKEYSEQ},    /* V3.3 */
                                      {UifReorder, CID_REORDER},
                                      {UifAddCheck, CID_ADDCHECK},
                                      {UifSingleTrans, CID_SINGLETRANS},
                                      {UifChkTransfer, CID_CHKTRANSFER},    /* V3.3 */
                                      {UifChangeComp, CID_CHANGECOMP},
                                      {UifReceipt, CID_RECEIPT},
                                      {UifBartender, CID_BARTENDER},    /* R3.1 */
                                      {UifReverse, CID_REVERSE},
                                      {UifPostReceipt, CID_POSTRECEIPT},
                                      {UifDeclared, CID_DECLARED},      /* V3.3 */
                                      {UifCasLockSignInOut, CID_CASLOCKSIGNINOUT},/* V3.3 */
                                      {UifCashierWaiterMSR, CID_CASHIERWAITERMSR},    /* R3.1 */
                                      {UifMoneyDec,         CID_MONEYDEC        },  /* Saratoga */
											{UifTender, CID_TENDER},                /* GenPOS Rel 2.2.1 for MDC_CHK_NO_PURCH, uchIsNoCheckOn */
                                      {NULL, 0}};

UIMENU FARCONST aChildRegCasIntSignIn[] = {{UifCasIntSignIn, CID_CASINTSIGNIN}, /* R3.3 */
                                        {NULL, 0}};
UIMENU FARCONST aChildRegCasIntTran[] = {{UifCasIntSignInOutMSR, CID_CASINTSIGNINOUTMSR},
                                      {UifTransVoid, CID_TRANSVOID},
                                      {UifWICTran, CID_WICTRAN},        /* Saratoga */
                                      {UifSales, CID_SALES},
                                      {UifChangeComp, CID_CHANGECOMP},
                                      {UifSingleTrans, CID_SINGLETRANS},
                                      {UifReceipt, CID_RECEIPT},
                                      {UifPostReceipt, CID_POSTRECEIPT},
                                      {UifReverse, CID_REVERSE},
                                      {NULL, 0}};

UIMENU FARCONST aChildRegCasIntRecallTran[] = {{UifCasIntSignInOutMSR, CID_CASINTSIGNINOUTMSR},
                                      {UifSales, CID_SALES},
                                      {UifReceipt, CID_RECEIPT},
                                      {UifReverse, CID_REVERSE},
                                      {NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifCashier(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Module
*===========================================================================
*/
SHORT UifCashier(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegCasSignIn);                /* open cashier sign in */
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_CASHIERSIGNOUT) {   /* cashier sign out status on ? */
            flUifRegStatus &= ~(UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE |
                                UIFREG_WAITERSIGNOUT | /* V3.3 */
                                UIFREG_BUFFERFULL);     /* reset status */
            UieCtrlDevice(UIE_DIS_SCANNER);             /* V3.3 */
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        UieNextMenu(aChildRegCasTran);                  /* open all children except casier sign in */
        break;

    case UIM_QUIT:
        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
        UifRegData.regopeclose.uchMinorClass = CLASS_UIMODELOCK;
		UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MODE;
        ItemSignOut(&UifRegData.regopeclose);           /* cashier sign out Modele */
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        return(SUCCESS);

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifCasInt(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Interrupt Module, R3.3
*===========================================================================
*/
SHORT UifCasInt(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegCasIntSignIn);             /* open cashier sign in */
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_CASHIERSIGNOUT) {   /* cashier sign out status on ? */
            flUifRegStatus &= ~(UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE |
                                UIFREG_CASINTSIGNIN |   /* R3.3 */
                                UIFREG_CASINTRECALL |
                                UIFREG_CASINTNEWIN |
                                UIFREG_BUFFERFULL);     /* reset status */
            UieCtrlDevice(UIE_DIS_SCANNER);             /* V3.3 */
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        flUifRegStatus &= ~UIFREG_CASINTNEWIN;          /* reset new sign-in status */
        if (flUifRegStatus & UIFREG_CASINTRECALL) {     /* cashier interrupt recall ? */

            UieNextMenu(aChildRegCasIntRecallTran);     /* open children for recall trans. */

        } else {
            UieNextMenu(aChildRegCasIntTran);           /* open all children */
        }
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        break;

    case UIM_QUIT:
        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
        UifRegData.regopeclose.uchMinorClass = CLASS_UIMODELOCK;
		UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MODE;
        ItemSignOut(&UifRegData.regopeclose);           /* cashier sign out Modele */
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        return(SUCCESS);

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/* --- End of Source --- */
