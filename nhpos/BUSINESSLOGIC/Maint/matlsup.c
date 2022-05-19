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
* Title       : Load Supervisor Parameter Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATLSUP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The Provided Function Names are as Follows: 
*                   
*               MaintLoadSup() : This Function Broadcasts/Requests to Download Super Parameter.   
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-03-92: 00.00.01 : K.You     : initial                                   
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
#include <nb.h>
#include <csop.h>
#include <plu.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <csstbopr.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>

#include "maintram.h"


/*
*===========================================================================
** Synopsis:    SHORT MaintLoadSup( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      LDT_PROHBT_ADR : Prohibit Operation
*
** Description: This Function Broadcasts/Requests to Download Super Parameter.
*===========================================================================
*/
SHORT MaintLoadSup( VOID )
{
    UCHAR           i;
    SHORT           sError;
    SHORT           sReturn = SUCCESS;
    MAINTBCAS       MaintBcas;
    MAINTERRORCODE  MaintErrCode;
    PARASTOREGNO    ParaStoRegNo;
    CLIOPBCAS       BcasRegNo[CLI_ALLTRANSNO];

    /* Make Header */
    MaintHeaderCtl(AC_SUP_DOWN, RPT_ACT_ADR);

    /* preset parameter for MaintBcas */
	memset (&MaintBcas, 0, sizeof(MaintBcas));
    MaintBcas.uchMajorClass = CLASS_MAINTBCAS;
    MaintBcas.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Preset Parameter for MaintErrCode */
    MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
    MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
    
    /* Check As Master Terminal */
    if (CliCheckAsMaster() != STUB_SUCCESS) {                       /* Not As Master Case */
        /* Request Super Parameter from Master */
        if((sError = CliReqOpPara(CLI_LOAD_SUP)) != OP_PERFORM) {   
            /* Print error Code */
            sReturn = OpConvertError(sError);
            MaintErrCode.sErrorCode = sReturn;
            PrtPrintItem(NULL, &MaintErrCode);
        } else {
            MaintBcas.uchTermNo = CliReadUAddr();
            ParaStoRegNo.uchMajorClass = CLASS_PARASTOREGNO;
            ParaStoRegNo.uchAddress = SREG_NO_ADR;
            CliParaRead(&ParaStoRegNo);
            MaintBcas.usRegNo = ParaStoRegNo.usRegisterNo;
			MaintBcas.uchBcasStatus = 1;      // the request worked so default to value of 1
            PrtPrintItem(NULL, &MaintBcas);
        }
    } else {
        /* BroadCast Super Parameter to Satellite */
        if ((sError = SerChangeOpPara(CLI_LOAD_SUP, BcasRegNo)) != OP_PERFORM) {
            /* Print error Code */
            sReturn = OpConvertError(sError);
            MaintErrCode.sErrorCode = sReturn;
            PrtPrintItem(NULL, &MaintErrCode);
        } else {
            /* set register number */
            for ( i = 0; i< CLI_ALLTRANSNO; i++) {
                /* in case successful communication */
                if (BcasRegNo[i].uchBcasStatus != 0) {
                    MaintBcas.uchTermNo =  ( UCHAR)(i + 1);
                    MaintBcas.usRegNo = BcasRegNo[i].usBcasRegNo;
					MaintBcas.uchBcasStatus = BcasRegNo[i].uchBcasStatus;
                    PrtPrintItem(NULL, &MaintBcas);
                }
            }
        }
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(sReturn);
}
