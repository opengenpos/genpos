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
* Title       : Reg Guest Check No for Check Transfer Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFCHKNO.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifCheckNo() : Reg Guest Check No for Check Transfer
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
#include <string.h>
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
UISEQ FARCONST aszSeqRegCheckNo[] = {FSC_GUEST_CHK_NO, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifCheckNo(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Guest Check No For Check Transfer Module
*===========================================================================
*/
SHORT UifCheckNo(KEYMSG *pData)
{
    SHORT   sRetStatus, sRet;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCheckNo);  /* open check no key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_GUEST_CHK_NO) { /* guest check no ? */ 
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRet != SUCCESS)
			{
				return sRet;
			}
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;   /* set check no class */
            UifRegData.regmisc.uchMinorClass = CLASS_UICHECKTRANSFER;
            _tcsncpy(UifRegData.regmisc.aszGuestNo, pData->SEL.INPUT.aszKey, (size_t)pData->SEL.INPUT.uchLen);  /* set guest check number */
            //memcpy(UifRegData.regmisc.aszGuestNo, pData->SEL.INPUT.aszKey, (size_t)pData->SEL.INPUT.uchLen);  /* set guest check number */
            sRetStatus = ItemMiscCheckTrans(&UifRegData.regmisc);   /* Misc Module */
            UifRegWorkClear();                          /* clear work area */
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
