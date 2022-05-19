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
* Title       : Assign GC No. Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATGCNO.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                   
*               MaintGuestNoWrite()     : This Function Assign GC No.   
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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
#include <plu.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <paraequ.h>
#include <para.h>
#include <rfl.h>
#include <csstbpar.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>

#include "maintram.h"


/*
*===========================================================================
** Synopsis:    SHORT MaintGuestNoWrite( PARAGUESTNO *pData )
*               
*     Input:  *pKeyMsg         : Pointer to Structure for PARAGUESTNO  
*    Output:  Nothing 
*
** Return:      SUCCESS        : Successful
*               LDT_PROHBT_ADR : Prohibit Operation
*
** Description: This Function Assign GC No. 
*===========================================================================
*/

SHORT MaintGuestNoWrite( PARAGUESTNO *pData )                                                     
{
    SHORT           sReturn = SUCCESS;
    SHORT           sError;
	MAINTERRORCODE  MaintErrCode = { 0 };
	PARAGUESTNO     ParaGuestNo = { 0 };
	PARAFLEXMEM     ParaFlexMem = { 0 };

    /* Check W/o Data */
    if (pData->uchStatus == MAINT_WITHOUT_DATA) {       /* W/o Data */
        return(LDT_SEQERR_ADR);
    }

    /* Ckeck Data */
    if (!pData->usGuestNumber) {                        /* Input Data is 0 */                        
        return(LDT_KEYOVER_ADR);
    }
                                                        
    /* Check System and Check Data */
    /* Get Guest Check System Information */
    ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    ParaFlexMem.uchAddress = FLEX_GC_ADR;
    CliParaRead(&ParaFlexMem);

    /* Check Guest Check System and Guest Check Id */

    if (ParaFlexMem.uchPTDFlag == FLEX_STORE_RECALL) {                  /* Drive Through Type */
        if (pData->usGuestNumber > GCF_MAX_DRIVE_NUMBER) {              /* Out of Range */
            return(LDT_KEYOVER_ADR);
        }
    }

    MaintHeaderCtl(AC_GCNO_ASSIN, RPT_ACT_ADR);         /* Make Header */

    /* Check GC File Exist */
    if ((sError = SerGusCheckExist()) == GCF_SUCCESS) {
        SerGusAssignNo(pData->usGuestNumber);           /* Assign Guest Check No. */

        /* Print Guest Check No. */
        ParaGuestNo.uchMajorClass = CLASS_PARAGUESTNO;
        ParaGuestNo.usGuestNumber = pData->usGuestNumber;
        ParaGuestNo.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
		RflGetTranMnem(ParaGuestNo.aszMnemonics, TRN_GCNO_ADR);
        PrtPrintItem(NULL, &ParaGuestNo);
    } else {
        sReturn = LDT_GCFNTEMPT_ADR;

        /* Print Error Code */
        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.sErrorCode = sReturn;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintErrCode);
    }

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */

    return(sReturn);
}



























