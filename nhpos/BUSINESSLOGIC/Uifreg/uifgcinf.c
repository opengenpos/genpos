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
* Title       : Reg GC Information Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFGCINF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifGCInformation() : Reg GC Information
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Jul-26-93:01.00.12:K.You      : memory compaction
* Jul-20-95:03.00.00:hkato      : R3.0
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
#include <uie.h>
#include <fsc.h>
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
UISEQ FARCONST aszSeqRegGCInformation[] = {FSC_TABLE_NO,
                                           FSC_NUM_PERSON, FSC_ALPHA, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifGCInformation(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg GC Information Module
*===========================================================================
*/
SHORT UifGCInformation(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegGCInformation);    /* open GC information key sequence */
        break;

    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case    FSC_TABLE_NO:                                /* table number ? */
            /* --- Saratoga, Nov/27/2000 --- */
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set modifier major class */
            UifRegData.regmodifier.uchMinorClass = CLASS_UITABLENO; /* set table number minor class */
            UifRegData.regmodifier.usTableNo = (USHORT)pData->SEL.INPUT.lData;   /* set tab;e number */
            if (uchUifRegVoid != 0) {
                UifRegData.regmodifier.fbModifier |= VOID_MODIFIER;
            }
            /* --- Saratoga, Nov/27/2000 --- */
            sRetStatus = ItemModTable(&UifRegData.regmodifier);    
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
            break;

        case    FSC_NUM_PERSON:                              /* number of person */
            /* --- Nov/27/2000 --- */
            if ((sRetStatus = UifRegInputZeroCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {
                return(sRetStatus);
            }
            UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;
            UifRegData.regmodifier.uchMinorClass = CLASS_UINOPERSON;
            UifRegData.regmodifier.usNoPerson = (USHORT)pData->SEL.INPUT.lData;
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                UifRegData.regmodifier.fbInputStatus = INPUT_0_ONLY;
            }
            sRetStatus = ItemModPerson(&UifRegData.regmodifier);
            UifRegWorkClear();
            if (sRetStatus == UIF_CANCEL) {
                UieReject();
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {
                flUifRegStatus |= UIFREG_BUFFERFULL;
                UieAccept();
                return(sRetStatus);
            }
            if (sRetStatus != UIF_SUCCESS) {
                return(sRetStatus);
            }
            UieAccept();
            break;
            /* --- Nov/27/2000 --- */

        case   FSC_ALPHA:                               /* Alpha, R3.0 */
            if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;
            UifRegData.regmodifier.uchMinorClass = CLASS_UIALPHANAME;
            sRetStatus = ItemModAlphaName();
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
            break;
        }
        break;

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/