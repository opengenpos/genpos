/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Foreign Out Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFFOROT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifForeignOut() : Reg Foreign Out
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial
* Jul-20-93:01.00.12:K.You      : bug fixed (mod. UifForeignOut)
* Jul-23-93:01.00.12:K.You      : memory compaction
* Dec-06-99:01.00.00:hrkato     : Saratoga
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
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "rfl.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"
#include "BlFWif.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegForeignOut[] = {FSC_FOREIGN, 0};
UISEQ FARCONST aszSeqRegForeignOutEC[] = {FSC_EC, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifForeignOut(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Foreign Out Module
*===========================================================================
*/
SHORT UifForeignOut(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegForeignOut);       /* open foreign out key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_FOREIGN) {   /* foreign ? */
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_FC_MAX)) { /* check foreign # */
                return(LDT_PROHBT_ADR);
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                if (!pData->SEL.INPUT.lData) {
                    return(LDT_SEQERR_ADR);
                }
                UifRegData.regtender.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            _tcsncpy(UifRegData.regtender.aszNumber, aszUifRegNumber, NUM_NUMBER);
			RflEncryptByteString ((UCHAR *)&(UifRegData.regtender.aszNumber[0]), sizeof(UifRegData.regtender.aszNumber));
            UifRegData.regtender.uchMajorClass = CLASS_UIFREGTENDER;    /* set foreign class */
            UifRegData.regtender.uchMinorClass = 
                (UCHAR)(pData->SEL.INPUT.uchMinor + CLASS_UIFOREIGN1 - 1);
            UifRegData.regtender.lTenderAmount = (LONG)pData->SEL.INPUT.lData;  /* set input amount */
            sRetStatus = ItemTenderEntry(&UifRegData.regtender);   /* Tender Modele */
            UifRegWorkClear();                              /* clear work area */
            switch (sRetStatus) {
            case UIF_CANCEL:                            /* cancel */
                UieReject();                            /* send reject to parent */
                break;

            case UIF_SUCCESS:                           /* success */
                flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                UieAccept();                            /* send accepted to parent */
                break;

            case LDT_TAKETL_ADR:                        /* buffer full */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                return(sRetStatus);

            case UIF_FINALIZE:                          /* finalize */
                flUifRegStatus |= UIFREG_FINALIZE;      /* set finalize status */
                UieAccept();                            /* send accepted to parent */
				UifRegClearDisplay(0);					/* clear work area */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                break;

            case UIF_CAS_SIGNOUT:                       /* cashier sign out */
                flUifRegStatus |= (UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE);   /* set buffer full and finalize status */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                            /* send accepted to parent */
                break;

            case UIF_WAI_SIGNOUT:                       /* waiter sign out */
                flUifRegStatus |= (UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE);    /* set buffer full and finalize status */
                UieAccept();                            /* send accepted to parent */
                break;

            case UIF_SEAT_TRANS:                            /* 5/24/96 End of Seat Trans,  R3.1 */
                flUifRegStatus |= UIFREG_BASETRANS;         /* Goto Base Trans Seq */
                flUifRegStatus &= ~(UIFREG_SEATTRANS | UIFREG_TYPE2_TRANS);
                UieAccept();
                break;

            default:                                        
                return(sRetStatus);
            }
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifForeignOutEC(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Foreign Out E/C Module
*===========================================================================
*/
SHORT UifForeignOutEC(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegForeignOutEC);       /* open foreign EC key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_EC) {      /* error correct ? */
            if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            /* UifRegData.regother.uchMajorClass = CLASS_UIFREGOTHER;  set error correct class 
            UifRegData.regother.uchMinorClass = CLASS_UIEC; */
            sRetStatus = ItemOtherECor();                   /* Other Module */
            UifRegWorkClear();                              /* clear work area */
            if ((sRetStatus == UIF_CANCEL) || (sRetStatus == UIF_SUCCESS)) {    /* cancel or success ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            return(sRetStatus);
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/