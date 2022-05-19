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
* Title       : ISP Server Module, CPM Functions Source File                        
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPCPM.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           IspRecvCpm();       Entry point of cashier function handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Nov-10-93:00.00.01:H.Yamaguchi: initial                                   
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

#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <cpm.h>
#include    <eept.h>
#include    <csstbcpm.h>
#include    <csserm.h>
#include    <isp.h>
#include    <storage.h>
#include    "ispcom.h"
/* #include    "ispext.h"  */
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    IspRecvCpm(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes for Charge posting function request.
*===========================================================================
*/
VOID    IspRecvCpm(VOID)
{
    CLIREQCPM_TLY   *pReqMsgH;
    CLIRESCPM       ResMsgH;
    USHORT          usRetLen;

    pReqMsgH = (CLIREQCPM_TLY *)IspRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESCPM));

    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.sReturn       = ISP_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;

    usRetLen = sizeof(CLIRESCPM) - CLI_MAX_CPMDATA ;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case CLI_FCCPMRDRSTTLY:    /*  Read/Reset Tally  */
        
        ResMsgH.usRetLen = sizeof(CPMEPT_TALLY);
        usRetLen += sizeof(CPMEPT_TALLY);

        ResMsgH.sReturn  = SerCpmEptReadResetTally(pReqMsgH->uchType, 
                                          (CPMEPT_TALLY *)ResMsgH.auchCpmData);
        break;

    default:                 /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVCPM_INV_FUN);
        return;
        break;
    }

    IspSendResponse((CLIRESCOM *)&ResMsgH, usRetLen , NULL, 0);
}


/*===== END OF SOURCE =====*/

