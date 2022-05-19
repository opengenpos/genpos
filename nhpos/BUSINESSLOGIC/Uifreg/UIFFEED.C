/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Feed Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFFEED.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifFeed() : Reg Feed
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Jul-23-93:01.00.12:K.You      : memory compaction                                   
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
UISEQ FARCONST aszSeqRegFeed[] = {FSC_FEED_REL, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifFeed(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Feed Module
*===========================================================================
*/
SHORT UifFeed(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegFeed);     /* open feed key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_FEED_REL) {    /* feed/release ? */
            if ((sRetStatus = UifRegInputCheck(pData,UIFREG_MAX_DIGIT2)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            UifRegData.regother.uchMajorClass = CLASS_UIFREGOTHER;  /* set feed class */
            UifRegData.regother.uchMinorClass = CLASS_UISLIPFEED;
            UifRegData.regother.lAmount = pData->SEL.INPUT.lData; /* set input amount */
            sRetStatus = ItemOtherSlipFeed(&UifRegData.regother);       /* Other Module */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
