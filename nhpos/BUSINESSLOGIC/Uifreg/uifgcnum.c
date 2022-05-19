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
* Title       : Reg GC Number Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFGCNUM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifGCNumberEntry() : Reg GC Number Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Jan-12-95:        :hkato      : R3.0
* Sep-21-98:03.03.00:M.Ozawa    : Enhanced to new guest check sequence
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
UISEQ FARCONST aszSeqRegGCNumberEntry[] = {FSC_GUEST_CHK_NO, FSC_SCANNER, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifGCNumberEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg GC Number Entry Module
*===========================================================================
*/
SHORT UifGCNumberEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;
	TCHAR   aszMyUifRegNumber[NUM_NUMBER];

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegGCNumberEntry);    /* open GC number entry key sequence */
        break;

    case UIM_INPUT:
		if (pData->SEL.INPUT.uchMajor == FSC_VIRTUALKEYEVENT || pData->SEL.INPUT.uchMajor == FSC_SCANNER) {
			if ((sRetStatus = UifCheckScannerData(pData)) != UIF_SUCCESS) {
				return(sRetStatus);
			}
			_tcsncpy(aszMyUifRegNumber, aszUifRegNumber, NUM_NUMBER);
		}
        if (pData->SEL.INPUT.uchMajor == FSC_GUEST_CHK_NO) {   /* guest check number ? */
			if (pData->SEL.INPUT.lData < 1 && CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT0) == 0) {
				// if a guest check number was not entered and if the auto generate is turned off
				// then we need to request a guest check number from the user.
				UIFDIADATA  WorkUI;
				ItemTransDispGCFNo(TRN_GCNO_ADR);
				memset(&WorkUI, 0, sizeof(UIFDIADATA));
				if (UifDiaGuestNo(&WorkUI) != UIF_SUCCESS) {
					return(UIF_CANCEL);
				}       

				pData->SEL.INPUT.lData = WorkUI.ulData;
			}

            if ((sRetStatus = UifRegInputCheck(pData,UIFREG_MAX_DIGIT6)) != SUCCESS) {      /* check input data, V3.3 */
            /* if ((sRetStatus = UifRegInputCheck(pData,UIFREG_MAX_DIGIT2)) != SUCCESS) {  */
                return(sRetStatus);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void,number */
                return(sRetStatus);
            }
//			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
//			if(sRet != SUCCESS)
//			{
//				return sRet;
//			}
            UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set recall class */
            UifRegData.regtransopen.uchMinorClass = CLASS_UINEWKEYSEQ;      /* V3.3 */
            /* UifRegData.regtransopen.uchMinorClass = CLASS_UIRECALL; */
            UifRegData.regtransopen.ulAmountTransOpen = (ULONG)pData->SEL.INPUT.lData;  /* set input amount */
            sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL || sRetStatus == UIF_DELIV_RECALL) {
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
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

