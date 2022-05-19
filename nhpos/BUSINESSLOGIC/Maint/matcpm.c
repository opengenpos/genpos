#if defined(POSSIBLE_DEAD_CODE)
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
* Title       : Start / Stop CPM (SUPER/PROGRAM MODE)
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCPM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                   
*               MaintStartStopCpm()  * Start or Stop CPM
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* 11-11-93: 03.00.00  :M.Yamamoto : initial                                   
*          :          :           :                                    
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

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <pif.h>
#include <plu.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <eept.h>
#include <cpm.h>
#include <csstbcpm.h>

#include "maintram.h"


/*
*===========================================================================
** Synopsis:    SHORT MaintStartStopCpm( UCHAR uchStatus )
*               
*     Input:  uchStatus     * for SerCpmChangeStatus(uchStatus)  
*    Output:  Nothing 
*
** Return:      SUCCESS        : Successful
*               LDT_PROHBT_ADR : Prohibit Operation
*               LDT_CPM_BUSY_ADR : CPM busy
*
** Description: This Function Start or Stop Cpm Service. 
*===========================================================================
*/

SHORT MaintStartStopCpm( UCHAR uchStatus )
{

    SHORT           sReturn;
    MAINTERRORCODE  MaintErrCode;
    MAINTCPM        MaintCpm;

    MaintHeaderCtl(AC_START_STOP_CPM, RPT_ACT_ADR);         /* Make Header */

    if ((sReturn = SerCpmChangeStatus(uchStatus)) == CPM_RET_BUSY) {
        return LDT_CPM_BUSY_ADR;
    }

    /* Print Error Code */

    if (sReturn == CPM_RET_SUCCESS) {
        MaintCpm.uchMajorClass    = CLASS_MAINTCPM;
        MaintCpm.uchType          = uchStatus;
        MaintCpm.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintCpm);
        sReturn = SUCCESS;
    } else {
        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.sErrorCode = LDT_PROHBT_ADR;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintErrCode);
        sReturn = LDT_PROHBT_ADR;
    }

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */
    return(sReturn);
}

#endif
