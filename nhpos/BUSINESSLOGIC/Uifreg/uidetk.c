/*
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
* Title       : ETK Job Code Compulsory Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDETK.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifDiaETK()     : ETK Job Code Compulsory Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-18-93:01.00.12: K.You     : initial                                   
*          :        :           :                                    
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
#include <regstrct.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegDiaETK[] = {{UifDiaETKEntry, CID_DIAETKENTRY},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaETK(UIFDIADATA *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg ETK Job Code Compulsory
*===========================================================================
*/

SHORT UifDiaETK(UIFDIADATA *pData, USHORT usEcho)
{

    SHORT   sRetStatus;
    USHORT  usModeStatus, usCtrlDevice;

    UieCreateDialog();                              /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);      /* disable mode change */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    UieInitMenu(aChildRegDiaETK);                   /* open ETK job code entry */
    if (usEcho == UIFREG_NO_ECHO) {         /* echo back as if server entry, V3.3 */
        UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_ID_DIGITS); /* no echo back */
    } else {
		UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_ID_DIGITS);  /* set echo back */
	}

    for (;;) {
		KEYMSG  Data = {0};

        UieGetMessage(&Data, 0x0000);                       /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);       /* check key */
        if (sRetStatus == UIFREG_ABORT) {           /* cancel ? */
            break;
        }
        else if (sRetStatus == UIF_DIA_SUCCESS) {    
            *pData = UifRegDiaWork;    /* set job code */
            sRetStatus = UIF_SUCCESS;       
            break;
        }
    }

    UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_INPUT);     /* set no echo back */
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieModeChange(usModeStatus);                    /* restore mode change status */
    UieDeleteDialog();                              /* delete dialog */
    return(sRetStatus);
}

