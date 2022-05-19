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
* Title       : MLD For Employee Number Assign Module  ( SUPER MODE )
* Category    : MLD, NCR 2170 US Hospitality Application Program    
* Program Name: MLDEMP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls Employee No.
*
*    The provided function names are as follows: 
* 
*        MldParaEmpNoDisp()  : Display parameter in Employee to MLD.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-95: 03.00.00 : T.Satoh   : initial                                   
* Nov-27-95: 03.01.00 : M.Ozawa   : Change display format
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
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <csttl.h>
#include <csstbttl.h>
#include <appllog.h>

#include <report.h>
#include <mld.h>
#include <mldsup.h> 

#include "maintram.h" 

/*
*===========================================================================
*   MLD Format of Employee Number Maintenance Modeule
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID MldParaEmpNoDisp(PARAEMPLOYEENO *pEmp, USHORT usClear)
*               
*     Input:    PARAEMPLOYEENO *pEmp
*               UCHAR          uchClear ( 0 : During Edit, 1 : When Read )
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/

VOID MldParaEmpNoDisp(PARAEMPLOYEENO *pEmp, USHORT usClear)
{
	TCHAR       aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

    /********************************************************/
    /* Clear Scroll Display and Display Item telop,         */
    /*                      when ONLY after Parameter Read. */
    /********************************************************/
    MldScrollClear(MLD_SCROLL_1);

	/* --- get transaction mnemonics --- */
	RflGetTranMnem(aszTranMnem, TRN_JOB_ADR);
    MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP,&(aszMldAC152[1][0]), pEmp->aszMnemonics, aszTranMnem);

    /*********************/
    /* Display Parameter */
    /*********************/
    MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+1,&(aszMldAC152[2][0]),
                 pEmp->ulEmployeeNo,(USHORT)(pEmp->uchJobCode1),
                    (USHORT)(pEmp->uchJobCode2),(USHORT)(pEmp->uchJobCode3));
    return;

    usClear = usClear;
}
