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
* Program Name: CSSTBCPM.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           CliCpmSendMessage()         * Send request to Host
*           CliCpmReceiveMessage()      * Receive from Host
*           CstCpmGetUpdNo()            * Get current Seq No.
*           SerCpmChangeStatus()        * Change status
*           SerCpmEptReadResetTally()   * Read or Reset tally 
*           CliCpmGetAsMasUaddr()       * Get master unique address
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Oct-28-93:H.Yamaguchi: initial                                   
* Jan-10-94:H.Yamaguchi: Change Type of SerCpmEptReadResetTally 
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
#include	<tchar.h>

#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csstbstb.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <cpm.h>
#include    <eept.h>
#include    <csstbcpm.h>
#include    "csstubin.h"

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;


/*
*===========================================================================
** Synopsis:    SHORT SerCpmChangeStatus(UCHAR uchState)
*                                   
*     Input:    uchState     - Define Control 
*    OutPut:    Nothing
*
** Return:      CPM_RET_SUCCESS:    OK
*   Abnormal    CPM_RET_PORT_NONE:  
*               CPM_RET_NOT_MASTER:      
*
** Description:  This function supports to change sattus.
*===========================================================================
*/
SHORT   SerCpmChangeStatus(UCHAR uchState)  
{
	return(CPM_RET_PORT_NONE);
}

/*
*===========================================================================
** Synopsis:    SHORT SerCpmEptReadResetTally(UCHAR        uchType,  
*                                             CPMEPT_TALLY *pTally);
*                                   
*     Input:    uchType     - Define report type 
*    OutPut:    pTally      - Set  CPM's Tally
*
** Return:      CPM_RET_SUCCESS:    CPM Tally
*  Abnormal:    CPM_RET_PORT_NONE:  Not provide CPM/EPT
*
** Description: This function supports to cRead or reset CPM Tally.
*===========================================================================
*/
SHORT   SerCpmEptReadResetTally(UCHAR uchType,  CPMEPT_TALLY *pTally)
{
        return(CPM_RET_PORT_NONE);
}

/*
*===========================================================================
** Synopsis:    UCHAR   CliCpmGetAsMasUaddr(VOID)
*                                   
*     Input:    Nothing 
*    OutPut:    Nothing
*
** Return:      CLI_TGT_MASTER :If Master works as Master.
*               CLI_TGT_BMASTER:If Master works as B-Master.
*               CLI_TGT_MASTER :If not exist Master, then return Master.
*
** Description: This function supports to get as Master terminal address.
*===========================================================================
*/
UCHAR   CliCpmGetAsMasUaddr(VOID)
{
    UCHAR   uchUMA  = CLI_TGT_MASTER;
    USHORT  fsComStatus = CstComReadStatus();

    if ((CLI_IAM_MASTER) && (fsComStatus & CLI_STS_M_UPDATE)) {
        return (uchUMA);
    }
    if ((CLI_IAM_BMASTER) && (fsComStatus & CLI_STS_BM_UPDATE)) {
        if (0 == (fsComStatus & CLI_STS_M_UPDATE)) {
            uchUMA = CLI_TGT_BMASTER; 
        }
    }
    return (uchUMA);
}

/*===== END OF SOURCE =====*/

