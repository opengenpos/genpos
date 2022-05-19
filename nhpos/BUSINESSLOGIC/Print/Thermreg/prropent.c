/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  Newcheck Reorder                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrROpenT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemTransOpen() : print items specified class "CLASS_ITEMTRANSOPEN"
*      PrtDispTransOpen() : display items specified class "CLASS_ITEMTRANSOPEN"
*      PrtDispTransOpenForm() : display items specified class "CLASS_ITEMTRANSOPEN"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-20-93 : 01.00.12 : R.Itoh     : add PrtDispTransOpen()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDispTransOpen()                                   
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDispTransOpen() 
* May-02-95 : 03.00.00 : T.Nakahata : add flex drive thru feature
* Aug-12-99 : 03.05.00 : K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDispTransOpenForm()                                   
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- 2170 local------**/
#include	<tchar.h>
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h> 
/* #include<para.h> */
#include<prt.h>
#include<dfl.h>
#include"prtrin.h"
#include"prtdfl.h"

/*
*===========================================================================
** Format  : VOID   PrtItemTransOpen(TRANINFORMATION  *pTran,
*                                     ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints new check.
*===========================================================================
*/
VOID   PrtItemTransOpen(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem)
{
        
    /* -- check "kp only"(prints to kp or take to kitchen) -- */
    if ( !(fsPrtStatus & PRT_KPONLY) ) {

        /*  set up no-print, compulsory print           */
        /* "fsPrtCompul" & "fsPrtNoPrtMask" initialized */
        /*  in TPM(PrcTran_.c)                          */

        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            if ((auchPrtSeatCtl[0] == 0) ||     /* at individual seat# print */
                (auchPrtSeatCtl[1] == 0)) {

                if ( (fsPrtCompul & fsPrtNoPrtMask) & PRT_SLIP ) {    
                    PrtChkCurSlip(pTran);          /* get current slip line and feed */
                }

                if ( !(pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_PROM)) {
                    PrtPromotion(pTran, pItem->fsPrintStatus);
                }
                if (pTran->TranCurQual.fsCurStatus & CURQUAL_TRAINING) {
                    PrtTraining(pTran, pItem->fsPrintStatus);
                }

                if (pTran->TranGCFQual.fsGCFStatus & GCFQUAL_PVOID) {
                    PrtPVoid(pTran, pItem->fsPrintStatus);
                }

                if (pTran->TranGCFQual.fsGCFStatus & GCFQUAL_TRETURN) {
                    PrtPReturn(pTran, pItem->fsPrintStatus);
                }
            }
        }
    }

    switch (pItem->uchMinorClass) {
        case CLASS_CASHIER:
        case CLASS_CASINTOPEN:              /* R3.3 */
        case CLASS_CASINTRECALL:            /* R3.3 */
            PrtCasIn(pTran, pItem);         /* cashier IN */
            break;

        case CLASS_NEWCHECK:                /* new check */
            PrtNewChk(pTran, pItem);
            break;

        case CLASS_REORDER:                 /* reorder */
        case CLASS_MANUALPB:                /* manual P/B */
            PrtReorder(pTran, pItem);
            break;

        case CLASS_ADDCHECK:                /* addcheck */
            PrtAddChk1(pTran, pItem);
            break;

		case CLASS_OPEN_TRANSBATCH:
        case CLASS_STORE_RECALL:            /* store/recall (R3.0) */
            PrtStoreRecall( pTran, pItem );
            break;

        default:
            break;
    }
}

/*
*===========================================================================
** Format  : VOID   PrtDispTransOpen(TRANINFORMATION  *pTran,
*                                     ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays new check.
*===========================================================================
*/
VOID   PrtDispTransOpen(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem)
{
        
    switch (pItem->uchMinorClass) {

		case CLASS_OPEN_TRANSBATCH:
        case CLASS_REORDER:                 /* reorder */
        case CLASS_MANUALPB:                /* manual P/B */
            PrtDflReorder(pTran, pItem);
            break;

        case CLASS_OPENCHECKTRANSFER:
            /* -- set display initial data -- */
            PrtDflInit(pTran);

        default:
            break;
    }

}

/*
*===========================================================================
** Format  : VOID   PrtDispTransOpen(TRANINFORMATION  *pTran,
*                                     ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays new check.
*===========================================================================
*/
USHORT   PrtDispTransOpenForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;
    
    switch (pItem->uchMinorClass) {

        case CLASS_CASHIER:
        case CLASS_CASINTOPEN:              /* R3.3 */
        case CLASS_CASINTRECALL:            /* R3.3 */
		case CLASS_OPEN_TRANSBATCH:
        case CLASS_REORDER:                 /* reorder */
        case CLASS_ADDCHECK:
        case CLASS_MANUALPB:                /* manual P/B */
        case CLASS_NEWCHECK:                /* new check */
        case CLASS_STORE_RECALL:            /* store/recall (R3.0) */
            usLine = PrtDflReorderForm(pTran, pItem, puchBuffer);
            break;
/*
        case CLASS_OPENCHECKTRANSFER:
            / -- set display initial data -- /
            PrtDflInit(pTran);
*/
            usLine = 0;
        default:
            usLine = 0;
            break;
    }
    
    return usLine;

}

/***** End Of Source *****/
