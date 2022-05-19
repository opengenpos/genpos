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
* Title       : Reg ETK Time Out Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFETKOT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifETKTimeOut() : Reg ETK Time Out 
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-18-93:01.00.12: K.you     : initial                                   
* Jul-23-93:01.00.12: K.you     : memory compaction                                   
* Nov-10-95:03.01.00: hkato     : R3.1
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <log.h>
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <uic.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <display.h>
#include <uireg.h>
#include <appllog.h>
#include "uiregloc.h"
#include <ConnEngineObjectIf.h>

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegETKTimeOut[] = {FSC_TIME_OUT, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifETKTimeOut(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg ETK Time Out Module
*===========================================================================
*/

SHORT UifETKTimeOut(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLockStatus = 0;

    switch (pData->uchMsg) {
    case UIM_INIT:                                  
        UieOpenSequence(aszSeqRegETKTimeOut);       /* register key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_TIME_OUT) {     
			UIFDIAEMPLOYEE  UifDiaEmployee;

            /* JMASON -- If MSR required prompt for supervisor intervention */
            if(CliParaMDCCheck(MDC_MSRSIGNIN_ADR,ODD_MDC_BIT3))
			{
				if ( ItemSPVInt(LDT_SUPINTR_ADR) != 0 )                         //success
					return( UIF_CANCEL );
			}

			memset (&UifDiaEmployee, 0, sizeof(UifDiaEmployee));

			UifDiaEmployee.ulSwipeEmployeeId = (ULONG)pData->SEL.INPUT.lData;
			if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT9)) != SUCCESS) {
				if ((sRetStatus = UifEmployeNumherForSignTime (&UifDiaEmployee)) != SUCCESS) {
	                return(sRetStatus);
				}
			}

            if (!husUifRegHandle) {             /* not during transaction lock */
				// check and set the keyboard and transaction lock.  this lock is used
				// to ensure that Cashier can not Sign-in if there is a remote application
				// that has locked the keyboard for actions that must not be done in
				// parallel to Cashier actions.
                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }
                usLockStatus = 1;
            }

			if (pData->SEL.INPUT.lData != 0) {
				UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
			}

            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  
            UifRegData.regmisc.uchMinorClass = CLASS_UIETKOUT;                   
            UifRegData.regmisc.ulEmployeeNo = UifDiaEmployee.ulSwipeEmployeeId;
			UifRegData.regmisc.uchJobCode = 0;
			UifRegData.regmisc.sTimeInStatus = 0;
			UifRegData.regmisc.ulStatusFlags = UifDiaEmployee.ulStatusFlags;
            sRetStatus = ItemMiscETK(&UifRegData.regmisc);

            UifRegWorkClear();                          /* clear work area */
            if (usLockStatus) {                         /* check transaction lock */
                if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                    PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                }
                husUifRegHandle = 0;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                if (sRetStatus == UIF_CANCEL) {         /* abort ? */
                    UieReject();                        
                    return (SUCCESS);
                }
                return(sRetStatus);
            }
            UieAccept();                                
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));               
    }
    return (SUCCESS);
}

/****** End of Source ******/
