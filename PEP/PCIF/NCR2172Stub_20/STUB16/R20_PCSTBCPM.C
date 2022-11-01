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
* Title       : Client/Server STUB, Charge Posting Source File
* Category    : Client/Server STUB, US Hospitality Enhance Model
* Program Name: CSSTBDFL.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerCpmReadResetTally()      * Read or Reset tally 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Oct-28-93:H.Yamaguchi: initial                                   
*          :           :                                    
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
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include    <pc2170.h>
#endif
#include    <r20_pc2172.h>

#include    <r20_pif.h>
#include    <memory.h>
#include    "csstubin.h"
#include "mypifdefs.h"

/*
*===========================================================================
** Synopsis: SHORT STUBENTRY SerCpmReadResetTally(UCHAR            uchType,  
*                                                 CPMEPT_TALLY FAR *pTally);
*                                   
*     Input:   	uchType     - Define report type 
*    OutPut:    pTally      - Set  CPM's Tally
*
** Return:      CPM_RET_SUCCESS:    CPM Tally
*  Abnormal:    CPM_RET_NOT_MASTER:              
*
** Description: This function supports to cRead or reset CPM Tally.
*===========================================================================
*/
SHORT   STUBENTRY SerCpmReadResetTally(UCHAR            uchType,  
                                       CPMEPT_TALLY FAR *pTally)
{
    CLIREQCPM_TLY  ReqMsgH;
    CLIRESCPM      ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    
    memset(&ReqMsgH, 0, sizeof(CLIREQCPM_TLY));
    memset(&ResMsgH, 0, sizeof(CLIRESCPM));
        
    ReqMsgH.uchType = uchType;
  
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCPMRDRSTTLY;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCPM_TLY);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCPM);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (CPM_RET_SUCCESS == CliMsg.sRetCode) {
        memcpy(pTally, ResMsgH.auchCpmData, ResMsgH.usRetLen);
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*===== END OF SOURCE =====*/


