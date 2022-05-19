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
* Title       : Load All Parameter Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATLALL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The Provided Function Names are as Follows: 
*                   
*               MaintLoadAll() : This Function Broadcasts/Requests to Download All Parameter.   
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
#include	<stdio.h>

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
** Synopsis:    SHORT MaintLoadAll( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      LDT_PROHBT_ADR : Prohibit Operation
*
** Description: This Function Broadcasts/Requests to Download All Parameter.
*===========================================================================
*/
SHORT MaintLoadAll( ULONG ulLoadAllFlags )
{
    UCHAR           i;
    SHORT           sError;
    SHORT           sReturn = SUCCESS;
    MAINTBCAS       MaintBcas;
    MAINTERRORCODE  MaintErrCode;
    PARASTOREGNO    ParaStoRegNo;
    CLIOPBCAS       BcasRegNo[CLI_ALLTRANSNO];

    /* Make Header */
    MaintHeaderCtl(AC_ALL_DOWN, RPT_ACT_ADR);

    /* preset parameter for MaintBcas */
	memset (&MaintBcas, 0, sizeof(MaintBcas));
    MaintBcas.uchMajorClass = CLASS_MAINTBCAS;
    MaintBcas.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT);

    /* Preset Parameter for MaintErrCode */
    MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
    MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
    
    /* Check As Master Terminal */
    if (CliCheckAsMaster() != STUB_SUCCESS) {                       /* Not As Master Case */
        /* Request All Parameter from Master */
		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: AC75 Initiated.  Requesting Parameters from Master.");
        if((sError = CliReqOpPara(CLI_LOAD_ALL)) != OP_PERFORM) {   
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

		if (ulLoadAllFlags == CLI_LOAD_AC75_ARG_LAY) {
			NHPOS_NONASSERT_NOTE("==STATE", "         AC75 Initiated.  Requesting Layouts as well.");
			if((sError = CliReqOpPara(CLI_LOAD_LAY)) != OP_PERFORM) {   
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
		}

		if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
			SHORT  sStatus = 0;

			sError = OpReqFile(_T("PARAM$PL"), 0);
			if (sError != OP_PERFORM) {
				sReturn = OpConvertError(sError);
				MaintErrCode.sErrorCode = sReturn;
				PrtPrintItem(NULL, &MaintErrCode);
			}

			sError = OpReqFile(_T("PARAMCAS"), 0);
			if (sError != OP_PERFORM) {
				sReturn = OpConvertError(sError);
				MaintErrCode.sErrorCode = sReturn;
				PrtPrintItem(NULL, &MaintErrCode);
			}

			sError = OpReqFile(_T("PARAMDEP"), 0);
			if (sError != OP_PERFORM) {
				sReturn = OpConvertError(sError);
				MaintErrCode.sErrorCode = sReturn;
				PrtPrintItem(NULL, &MaintErrCode);
			}

			sError = OpReqFile(_T("PARAMETK"), 0);
			// the ETK file is an optional file in that it may or
			// may not exist depending on the P1 provisioning for
			// the number of employees.  If there is an error,
			// just ignore the error and move on.
//			if (sError != OP_PERFORM) {
//				sReturn = OpConvertError(sError);
//				MaintErrCode.sErrorCode = sReturn;
//				PrtPrintItem(NULL, &MaintErrCode);
//				sReturn = SUCCESS;
//			}

			sError = OpReqFile(_T("PARAMIDX"), 0);
			if (sError != OP_PERFORM) {
				sReturn = OpConvertError(sError);
				MaintErrCode.sErrorCode = sReturn;
				PrtPrintItem(NULL, &MaintErrCode);
			}
		}
		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: AC75 Complete.");
    } else {
        /* BroadCast All Parameter to Satellite */
		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: AC75 Initiated.  Broadcasting Parameters.");
        if ((sError = SerChangeOpPara(CLI_LOAD_ALL, BcasRegNo)) != OP_PERFORM) {   
            /* Print error Code */
            sReturn = OpConvertError(sError);
            MaintErrCode.sErrorCode = sReturn;
            PrtPrintItem(NULL, &MaintErrCode);
        } else {
            /* report on broadcast results */
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: AC75 Complete.");
            for (i = 0; i < CLI_ALLTRANSNO; i++) {
                /* in case successful communication */ 
                if (BcasRegNo[i].uchBcasStatus != 0) {
					char  xBuff[128];

                    MaintBcas.uchTermNo = ( UCHAR)(i + 1);
                    MaintBcas.usRegNo = BcasRegNo[i].usBcasRegNo;
					MaintBcas.uchBcasStatus = BcasRegNo[i].uchBcasStatus;
                    PrtPrintItem(NULL, &MaintBcas);
					sprintf (xBuff, "       AC75 Term No: %d, RegNo %d, BcasStatus %d.", MaintBcas.uchTermNo, MaintBcas.usRegNo, MaintBcas.uchBcasStatus);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
            }
        }
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(sReturn);
}


