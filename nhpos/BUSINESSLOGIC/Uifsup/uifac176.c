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
* Title       : Operator/Guest Check Status Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC176.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC176Function()       : A/C No.176 Function Entry Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Nov-24-92:01.00.00    :K.You      : initial                                   
* Apr-27-95:03.00.00    :M.Waki     : Function Type Change --> Normal Called                                   
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/

#include	<tchar.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */
/*
*=============================================================================
**  Synopsis: SHORT UifAC176Function(VOID) 
*
*       Input:  nothing  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.176 Function  
*===============================================================================
*/


SHORT UifAC176Function(KEYMSG *pKeyMsg)
{
    switch(pKeyMsg->uchMsg){
    case UIM_INIT:

        MaintDisp(AC_CSOPN_RPT,                     /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  0);                               /* Lead Through Address */

        MaintOpeStatus();                           /* Operator/Guest Check Status Report */ 
        UieExit(aACEnter);
        return(SUCCESS);
    default:
        return(UifACDefProc(pKeyMsg));
    }
}


