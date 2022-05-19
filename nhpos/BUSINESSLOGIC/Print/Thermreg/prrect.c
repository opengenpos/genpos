/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Error Correct for Charge Posting                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrREcT.c                                                      
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*                       
*      PrtErrCorr()     : print error correct for charge posting
*      PrtErrCorr_EJ()  : error correct( electic journal )
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-18-93 : 02.00.02 : K.You      : initial                                   
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-25, 31 (mod. PrtErrCorr_J)                                   
* May-17-94 : 02.05.00 : Yoshiko.Jim: S-017 corr. (mod. PrtErrCorr_J())                                   
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
/* #include <para.h> */
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include <prt.h>
#include "prtrin.h"
#include "prrcolm_.h"
#include <rfl.h>

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtErrCorr(ITEMTENDER *pItem);      
*                                           
*   Input  : ITEMTENDER         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints error correct for charge posting
*===========================================================================
*/
VOID PrtErrCorr(ITEMTENDER *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */

    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
        PrtErrCorr_EJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtErrCorr_EJ(ITEMTENDER *pItem);      
*                               
*   Input  : ITEMTENDER         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints error correct for charge posting(electric journal)
*===========================================================================
*/
VOID  PrtErrCorr_EJ(ITEMTENDER *pItem)
{
    USHORT  i;

    PrtEJMnem(TRN_EC_ADR, PRT_DOUBLE);              /* E/C with double wide */
    PrtEJOffTend(pItem->fbModifier);                /* Cp off line          */
    PrtEJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
    PrtEJNumber(pItem->aszNumber);                                 /* number line          */
    PrtEJCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID);        /* room #, guest ID     */
    PrtEJAmtMnem(RflChkTendAdr(pItem), pItem->lTenderAmount);      /* E/C amount line      */
    PrtEJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);  /* folio#, post trans#  */
    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtEJCPRspMsgText(pItem->aszCPMsgText[i]);  /* Response Msg         */
    }
}

/***** End Of Source *****/
