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
* Title       : Operator/Guest Check Status Report Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATOPSTS.C
* --------------------------------------------------------------------------
* Abstract: The Provided Function Names are as Follows: 
*                   
*               MaintOpeStatus() : This Function Reads Operator/Guest Check Status Report.    
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-24-92: 01.00.00 : K.You     : initial                                   
* Aug-11-99: 03.05.00 : M.Teraki  : Remove WAITER_MODEL
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

#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csop.h>
#include <csstbopr.h>
#include <cswai.h>
#include <cscas.h>
#include <csgcs.h>
#include <csstbpar.h>
#include <csstbwai.h>
#include <csstbcas.h>
#include <csstbgcf.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csttl.h>
#include <csstbttl.h>
#include <report.h>
#include <mld.h>

#include "maintram.h"


/*
*===========================================================================
** Synopsis:    VOID MaintOpeStatus( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Reads Operator/Guest Check Status Report.
*===========================================================================
*/

VOID MaintOpeStatus( VOID )
{
	extern    UCHAR   uchRptMldAbortStatus;  /* abort status by LCD R3.0 */
                    
    SHORT           sNoofGC;
	union  {
		ULONG     aulCashierNo[CAS_NUMBER_OF_MAX_CASHIER];
		USHORT    ausGcfRcvBuff[GCF_MAX_GCF_NUMBER];
	} auslRcvBuff;
	MAINTTRANS      MaintTrans = { 0 };
	MAINTOPESTATUS  MaintOpeSts = { 0 };

    /* Make Header */
    MaintHeaderCtl(AC_CSOPN_RPT, RPT_ACT_ADR);
    
    /* Preset Each Parameter */
    MaintOpeSts.uchMajorClass = CLASS_MAINTOPESTATUS;
    MaintOpeSts.uchMinorClass = 0;
    MaintOpeSts.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.uchMinorClass = 0;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Output Data to Print Module */
    /* Clear Receive Buffer */
    memset(&auslRcvBuff, 0, sizeof(auslRcvBuff));

    /* Get Opened Cashier Id */
    if (SerCasChkSignIn(&(auslRcvBuff.aulCashierNo[0])) == CAS_DURING_SIGNIN) {     /* Opened Cashier Exist */     
        MaintFeed();                                                                 
                                                                
        /* Print Special Mnemonics for Cashier */
		RflGetSpecMnem(MaintTrans.aszTransMnemo, SPC_CAS_ADR);
		if (RptCheckReportOnMld()) {
			uchRptMldAbortStatus = (UCHAR)MldDispItem(&MaintTrans, 0); /* display on LCD          */ 
			MaintTrans.usPrtControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
		}
        PrtPrintItem(NULL, &MaintTrans);

		MaintOpeSts.uchMinorClass = CLASS_PARAOPESTATUS_CASHIER;

        for (USHORT i = 0; i < CAS_NUMBER_OF_MAX_CASHIER; i++) { /* V1.0.13 */
            if ((MaintOpeSts.ulOperatorId = auslRcvBuff.aulCashierNo[i]) != 0L) { /* Opened Cashier Exist */ 
				if (RptCheckReportOnMld()) {
					uchRptMldAbortStatus = (UCHAR)MldDispItem(&MaintOpeSts, 0); /* display on LCD          */ 
					MaintOpeSts.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
				}
                PrtPrintItem(NULL, &MaintOpeSts);                           
            }
        }
    }

    /* Clear Receive Buffer */
    memset(&auslRcvBuff, 0, sizeof(auslRcvBuff));

    /* Get Locked GCF No. */
    if ((sNoofGC = SerGusReadAllLockedGCN(&(auslRcvBuff.ausGcfRcvBuff[0]), sizeof(auslRcvBuff))) > 0) {   /* Locked GC Exist */
        USHORT  usSysType = RflGetSystemType();  // fetch the system type once for use in the loop below.

		MaintFeed();
                                                                                        
        /* Print Transaction Mnemonics for GCF */
		RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_GCNO_ADR);
		if (RptCheckReportOnMld()) {
			uchRptMldAbortStatus = (UCHAR)MldDispItem(&MaintTrans, 0); /* display on LCD          */ 
			MaintTrans.usPrtControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
		}
        PrtPrintItem(NULL, &MaintTrans);

		MaintOpeSts.uchMinorClass = CLASS_PARAOPESTATUS_GCNO;
        for (SHORT i = 0; i < sNoofGC/2; i++) {
            MaintOpeSts.ulOperatorId = auslRcvBuff.ausGcfRcvBuff[i];

            /* Check CDV System. The ulOperatorID member contains a Guest Check number */
            if (usSysType != FLEX_STORE_RECALL && CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT1)) {  /* CDV System */
                MaintOpeSts.ulOperatorId *= 100L;
                MaintOpeSts.ulOperatorId += RflMakeCD(auslRcvBuff.ausGcfRcvBuff[i]);
            }
			if (RptCheckReportOnMld()) {
				uchRptMldAbortStatus = (UCHAR)MldDispItem(&MaintOpeSts, 0); /* display on LCD          */ 
				MaintOpeSts.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
			}
            PrtPrintItem(NULL, &MaintOpeSts);
        }
    }
    
    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}
