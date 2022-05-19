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
* Title       : Disconnect Master or Backup Master ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATDISM.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                   
*               MaintDisconMas()    : This Function Disconnect Master or Backup    
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-20-92: 00.00.01 : K.You     : initial                                   
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

#include <ecr.h>
#include <plu.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <csstbbak.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>

#include "maintram.h"


/*
*===========================================================================
** Synopsis:    SHORT MaintDisconMas( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      SUCCESS        : Successful
*               LDT_PROHBT_ADR : Prohibit Operation
*               LDT_ERR_ADR    : Other Error
*
** Description: This Function Disconnect Master or Backup Master.
*===========================================================================
*/

SHORT MaintDisconMas( VOID )                                                     
{

    SHORT           sError;
    SHORT           sReturn = SUCCESS;
    MAINTERRORCODE  MaintErrCode;


    /* Execute Disconnect Master /Backup Master */

    sError = SerChangeInqStat();

    /* lock target shared printer at this point, in case of target is down at AC85, 12/08/01 */
	if (MaintShrTermLockSup(AC_DISCON_MAST) != SUCCESS) {        /* In case of SYSTEM BUSY */
	    return(SUCCESS);
    }
    MaintHeaderCtl(AC_DISCON_MAST, RPT_ACT_ADR);            /* Make Header */

	if (sError != STUB_SUCCESS) {
        sReturn = CstConvertError(sError);  

        /* Print Error Code */

        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.sErrorCode = sReturn;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintErrCode);
	}
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */
    return(sReturn);
}

SHORT MaintForceInquiry ( VOID)
{
    SHORT           sReturn = SUCCESS;

	SerForceInqStatus ();

    /* lock target shared printer at this point, in case of target is down at AC85, 12/08/01 */
	if (MaintShrTermLockSup(AC_FORCE_MAST) != SUCCESS) {        /* In case of SYSTEM BUSY */
	    return(SUCCESS);
    }
    MaintHeaderCtl(AC_FORCE_MAST, RPT_ACT_ADR);            /* Make Header */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */
    return(sReturn);
}


























