/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Waiter Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFWAI.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifWaiter()    : Reg Waiter
*               UifBartender() : Reg Bartender
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Jul-01-93:01.00.12:K.You      : add post receipt feature.                                    
* Jul-20-93:01.00.12:K.You      : add EJ reverse feature.                                    
* Nov-09-95:03.01.00:hkato      : R3.1
* Aug-11-99:03.05.00:K.Iwata    : R3.5 (remove WAITER_MODEL)
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
UIMENU FARCONST aChildRegBarSignIn[] = {{UifBartenderSignIn, CID_BARTENDERSIGNIN},
                                        {NULL, 0}};
UIMENU FARCONST aChildRegBarTran[] = {{UifBartenderSignOut, CID_BARTENDERSIGNOUT},
                                      {UifTransVoid, CID_TRANSVOID},
                                      {UifWICTran, CID_WICTRAN},        /* Saratoga */
                                      {UifSales, CID_SALES},
                                      {UifDeclared, CID_DECLARED},
                                      {UifChangeComp, CID_CHANGECOMP},
                                      {UifReceipt, CID_RECEIPT},
                                      {UifPostReceipt, CID_POSTRECEIPT},
                                      {UifReverse, CID_REVERSE},
                                      {NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifBartender(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Bartender Module
*===========================================================================
*/
SHORT UifBartender(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegBarSignIn);        /* open waiter sign in */
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_WAITERSIGNOUT) {    /* waiter sign out status on ? */
            flUifRegStatus &= ~(UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE |
                                UIFREG_BUFFERFULL);     /* reset status */
            UieCtrlDevice(UIE_DIS_SCANNER);             /* V3.3 */
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        UieCtrlDevice(UIE_ENA_SCANNER);                 /* V3.3 */
        UieNextMenu(aChildRegBarTran);                  /* open all children except waiter sign in */
        break;

    case UIM_QUIT:
        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set waiter sign out class */
        UifRegData.regopeclose.uchMinorClass = CLASS_UIMODELOCK;
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
