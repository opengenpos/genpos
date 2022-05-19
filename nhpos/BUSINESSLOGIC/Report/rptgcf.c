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
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Guest Check File Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTGCF.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               RptGuestRead()              : Read Guest Check File
*                   RptGuestAllRead()       : Read All Guest Check File or by Waiter 
*                   RptGuestIndRead()       : Read Individual GC File
*               RptGuestReset()             : Reset Guest Check File
*                   RptGuestAllReset()      : Reset All Guest Check File or by Waiter
*                   RptGuestIndReset()      : Reset Individual GC File
*                   RptGuestOpenReset()     : Emergent GC File Close Module
*                       RptGuestAllEdit()   : Edit All GC File Report
*                       RptGuestIndEdit()   : Edit Individual GC File Report
*                       RptGuestElement()   : Format GC File Report
*               RptGuestLock()              : Lock Guest Check File
*               RptGuestUnLock()            : UnLock Guest Check File
*               RptGuestGetPrtCtl()         : Get Print Control by MDC
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name     : Description
* Jul-11-92 : 00.00.01 :  K.You     : initial                                   
* Nov-20-92 : 01.00.00 :  K.You     : Adds print/not print by MDC function                        
* Dec-16-92 : 01.00.00 :  K.You     : Fix bug for counting up twice of reset co. in
*           :          :            : drive-through system.                          
* Jun-21-93 : 01.00.12 :  J.IKEDA   : Change major class for print in RptGuestOpenReset()
*           :          :            : Change struture for training ID print in RptGuestIndEdit()
*           :          :            : Change CLASS_RPTREGFIN_PRTDGG to CLASS_RPTGUEST_PRTDGG
* Jun-25-93 : 01.00.12 : J.IKEDA    : Change MinorClass for header print in RptGuestOpenReset()
* Jun-28-93 : 01.00.12 : J.IKEDA    : Add argument-sFeedType in RptGuestIndEdit()
* Jul-07-93 : 01.00.12 : J.IKEDA    : Change define from RPT_ALL_READ to RPT_GCF_READ
*           :          :            : for Abort Key - RptGuestAllEdit() 
* Jul-09-93 : 01.00.12 : J.IKEDA    : Adds PrtCheckAbort() in RptGuestIndEdit()
* Jul-14-93 : 01.00.12 : J.IKEDA    : Distinguish BY WAITER and OTHERS in RptGuestLock/UnLock()
*           :          :            : Distinguish INDIVIDUAL and OTHERS in RptGuestUnLock()
*           :          :            : Adds GCF_FULL in RptGuestLock()
* Jul-30-93 : 01.00.12 : J.IKEDA    : |--> comment because Do not return GCF_FULL from SerGusReadLock()
* Aug-25-93 : 01.00.13 : J.IKEDA    : Add PrtShrPartialSend() in RptGuestIndEdit()
* Aug-30-93 : 01.00.13 :            : Bug Fixed E-76. (Mod. RptGuestIndEdit())
* Apr-24-95 : 03.00.00 : hkato      : R3.0(GusReadLock)
* Jun-27-95 : 03.00.00 : M.Suzuki   : R3.0(Mod. RptGuestReset)
* Jul-24-95 : 03.00.03 : M.Ozawa    : R3.0(Mod. RptGuestElement for Scroll)
* Aug-11-99 : 03.05.00 : M.Teraki   : R3.5(remove WAITER_MODEL)
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <rfl.h>
#include <appllog.h>
#include <log.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <prt.h>
#include <csstbpar.h>
#include <csop.h>
#include <csstbopr.h>
#include <cscas.h>
#include <csgcs.h>
#include <csstbcas.h>
#include <csstbgcf.h>
#include <csttl.h>
#include <csstbttl.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"
/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

D13DIGITS  lRptSubTotal;                             /* Sub Total Save Area */
D13DIGITS  lRptGndTotal;                             /* Grand Total Save Area */
        
/*
*=============================================================================
**  Synopsis: USHORT RptGuestGetPrtCtl( UCHAR uchType )
* 
*       Input:  uchType     : RPT_GCF_READ      : Read Type 
*                           : RPT_GCF_RESET     : Reset Type
*      Output:  Nothing
*
**  Return:     Print Control Status  
*
**  Description:  
*       Check Read or Reset report, and check also MDC bit.
*       Return status of those condition.  
*===============================================================================
*/
static USHORT RptGuestGetRptCtl(UCHAR uchType)
{
    USHORT  usPrintStatus = 0;

    if (uchType == RPT_GCF_READ) {                      /* Read Type Case */
        if (!(CliParaMDCCheckField (MDC_PCTL_ADR, MDC_PARAM_BIT_D))) {  /* Journal print ? MDC check */
            usPrintStatus |= PRT_JOURNAL;                               /* Set Journal print status  */
        }
        if (!(CliParaMDCCheckField (MDC_PCTL_ADR, MDC_PARAM_BIT_C))) {  /* Receipt print ? MDC check */ 
            usPrintStatus |= PRT_RECEIPT;                               /* Set Receipt print status  */
        }
    } else {                                            /* Reset Type Case */                                                              /* if Reset report ?   */
        if (!(CliParaMDCCheckField (MDC_PCTL_ADR, MDC_PARAM_BIT_B))) {  /* Journal print ? MDC check */
            usPrintStatus |= PRT_JOURNAL;                               /* Set Journal print status  */
        }
        if (!(CliParaMDCCheckField (MDC_PCTL_ADR, MDC_PARAM_BIT_A))) {  /* Receipt print ? MDC check */ 
            usPrintStatus |= PRT_RECEIPT;                               /* Set Receipt print status  */
        }
        if (usPrintStatus == 0) {
            usPrintStatus |= PRT_JOURNAL;                               /* Set Journal print status  */
        }
    }
    return(usPrintStatus);
}


/*
*=============================================================================
**  Synopsis: SHORT RptGuestAllRead( UCHAR uchType, USHORT usNumber ) 
*               
*       Input:  uchType     : RPT_ALL_READ      : All Read Type
*                           : RPT_WAITER_READ   : Read by Waiter Type
*               usNumber    : Guest Check Number/Waiter Number         
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read All Guest Check Total File.  
*===============================================================================
*/
static SHORT RptGuestAllRead(UCHAR uchType, ULONG ulCashierNumber)
{
    SHORT           sError;
    SHORT           sGCSysStatus;
    SHORT           sNoofGC1 = 0;
    SHORT           sNoofGC2 = 0;
    USHORT          ausRcvBuff[GCF_MAX_GCF_NUMBER];

    /* Check Waiter Exist */
#if 0
    if (uchType == RPT_WAITER_READ) {
		CASIF    Arg = {0};

        Arg.ulCashierNo = ulCashierNumber;                        
        if ((sError = SerCasIndRead(&Arg)) != CAS_PERFORM) {   
            return(CasConvertError(sError));
        }
    }
#endif

    /* Precheck GC File Exist */
    if ((sError = SerGusCheckExist()) == GCF_FILE_NOT_FOUND) {  /* GCF Not Created */
        return(GusConvertError(sError));
    } 

    RptPrtStatusSet(CLASS_TTLCURDAY);                        /* Set J|R print status for Feed Ctl.*/

    /* Initialize Sub/Grand Total Save Area */
    lRptGndTotal = 0L;
    lRptSubTotal = 0L;                             

    /* Control Header */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecutive No. */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_GCF_ADR,                            /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_READ_ADR,                           /* Report Name Address */
                    uchType,                                /* Read Type */
                    AC_GCFL_READ_RPT,                       /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */
    
    
    /* Precheck GC File Exist */
    if (sError == GCF_SUCCESS) {                            /* GCF Not Exist */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return(SUCCESS);
    } /*else if (sError != GCF_EXIST) {
        RptPrtError(GusConvertError(sError));
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return(GusConvertError(sError));
    }*/

	/* Get Guest Check System Information */
	if (RflGetSystemType() == FLEX_STORE_RECALL)
	{
		if (uchType == RPT_ALL_READ)
		{
            sNoofGC1 = SerGusAllIdRead(GCF_DRIVE_THROUGH_STORE, ausRcvBuff, sizeof(ausRcvBuff));              
        }
		else
		{
            sNoofGC1 = SerGusAllIdReadBW(GCF_DRIVE_THROUGH_STORE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff));
        }
        if (sNoofGC1 == GCF_NOT_IN)
		{
            sNoofGC1 = 0;
        }
		else if (sNoofGC1 < 0)
		{                              /* Error Case */
            RptPrtError(GusConvertError(sNoofGC1));
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(GusConvertError(sNoofGC1));
        }

		/* Print Store/Recall Guest Check Total Report */
        if ((sError = RptGuestAllEdit(RPT_GCF_READ, ausRcvBuff, sNoofGC1)) == RPT_ABORTED) {
            MaintMakeAbortKey();                                        /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(SUCCESS);
        } else if (sError != SUCCESS) {
            RptPrtError(sError);
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(sError);
        }
	}
	else
	{
		if (uchType == RPT_ALL_READ)
		{
			if (( sGCSysStatus = SerGusAllIdRead(GCF_COUNTER_TYPE, ausRcvBuff, sizeof(ausRcvBuff))) < 0)
			{    /* Error Case */
			    sGCSysStatus = 0;
			}
		} 
		else
		{
			if (( sGCSysStatus = SerGusAllIdReadBW(GCF_COUNTER_TYPE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff))) < 0)
			{  /* Error Case */
				sGCSysStatus = 0;
			}
		}
    }

    if (sNoofGC1 && sGCSysStatus) {                     /* Drive Through and Counter Type File Exist */
        /* Print Sub Total */                           

        RptFeed(RPT_DEFALTFEED);                                 /* Feed */
        /* Set Date  for Print Out      */
        /* |--------------------------| */
        /* | SUB TOTAL                | */
        /* |             SZZZZZZZ9.99 | */
        /* |--------------------------| */
        RptGuestElement(RPT_GCF_READ, CLASS_RPTGUEST_PRTTTL, TRN_STTLR_ADR, 0, 0, 0, 0, 0, 0, lRptSubTotal);
        RptFeed(RPT_DEFALTFEED);                                 /* Feed */
    }

	// if Counter Type File exists, **and no Drive Thru** - CSMALL
    if (sGCSysStatus && !sNoofGC1) {                             /* Counter Type File Exist */
        /* Initialize Sub Total Save Area */
        lRptSubTotal = 0L;
   
        /* Read Guest Check Id for Counter Type */
        if (uchType == RPT_ALL_READ) {
            sNoofGC2 = SerGusAllIdRead(GCF_COUNTER_TYPE, ausRcvBuff, sizeof(ausRcvBuff));              
        } else {
            sNoofGC2 = SerGusAllIdReadBW(GCF_COUNTER_TYPE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff));
        }
        if (sNoofGC2 == GCF_NOT_IN) {
            sNoofGC2 = 0;
        } else if (sNoofGC2 < 0) {                                             /* Error Case */
            RptPrtError(GusConvertError(sNoofGC2));
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(GusConvertError(sNoofGC2));
        }
    
        /* Print Counter Guest Check Total Report */
        if ((sError = RptGuestAllEdit(RPT_GCF_READ, ausRcvBuff, sNoofGC2)) == RPT_ABORTED) {
            MaintMakeAbortKey();                                        /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(SUCCESS);
        } else if (sError != SUCCESS) {
            RptPrtError(sError);
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(sError);
        }
    
        if (sNoofGC1) {                                                 /* Drive Through GCF Exist */
            /* Print Sub Total */
            RptFeed(RPT_DEFALTFEED);                                 /* Feed */
            /* Set Date  for Print Out      */
            /* |--------------------------| */
            /* | SUB TOTAL                | */
            /* |             SZZZZZZZ9.99 | */
            /* |--------------------------| */
            RptGuestElement(RPT_GCF_READ, CLASS_RPTGUEST_PRTTTL, TRN_STTLR_ADR, 0, 0, 0, 0, 0, 0, lRptSubTotal);
        }
    }

    if (sNoofGC1 || sNoofGC2) {                                     /* Exist GC File */
        /* Print Grand Total */
        RptFeed(RPT_DEFALTFEED);                                 /* Feed */

        /* Set Date  for Print Out      */
        /* |--------------------------| */
        /* | TOTAL                    | */
        /* |             SZZZZZZZ9.99 | */
        /* |--------------------------| */
        RptGuestElement(RPT_GCF_READ, CLASS_RPTGUEST_PRTTTL, TRN_TTLR_ADR, 0, 0, 0, 0, 0, 0, lRptGndTotal);
    }

    /* Control Trailer */
    if (uchRptMldAbortStatus) {                                     /* aborted by MLD */
        MaintMakeAbortKey();                                        /* Print ABORDED */
    }
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestIndRead( USHORT usNumber ) 
*               
*       Input:  usNumber    : Guest Check Number         
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read Individual Guest Check Total File.  
*===============================================================================
*/

static SHORT RptGuestIndRead(USHORT usNumber)
{
    SHORT           sError;
    TRANGCFQUAL     TranGCFQual;

    /* Check Guest Check File Exist */
    if ((sError = SerGusIndRead(usNumber, ( UCHAR *)&TranGCFQual, sizeof(TranGCFQual))) == GCF_NOT_IN) {
        return(GusConvertError(sError));
    }

    RptPrtStatusSet(CLASS_TTLCURDAY);                       /* Set J|R print status for Feed Ctl. */

    /* Control Header */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Increment Consecutive Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_GCF_ADR,                            /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_READ_ADR,                           /* Report Name Address */
                    RPT_IND_READ,                           /* Individual Read Type */
                    AC_GCFL_READ_RPT,                       /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */


    /* Print Guest Check Total Report */
    if ((sError = RptGuestIndEdit(RPT_GCF_READ, usNumber, 0)) != SUCCESS) {
        RptPrtError(sError);
    }

    /* Control Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestRead( UCHAR uchType, USHORT usNumber ) 
*               
*       Input:  uchType     : RPT_ALL_READ      : All Read Type
*                           : RPT_IND_READ      : Individual Read Type
*                           : RPT_WAITER_READ   : Read by Waiter Type
*               usNumber    : Guest Check Number/Waiter Number         
*
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read Guest Check Total File.  
*===============================================================================
*/

SHORT RptGuestRead(UCHAR uchType, USHORT usNumber)
{

    SHORT   sError;

    /* Distingush Report Type */
    switch (uchType) {
    case RPT_ALL_READ:
    case RPT_WAITER_READ:        
        sError = RptGuestAllRead(uchType, usNumber);
        break;
                                                                    
    default:                                                        /* RPT_IND_READ case */
        sError = RptGuestIndRead(usNumber);
        break;

    }
    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestAllReset( UCHAR uchType, USHORT usNumber ) 
*               
*       Input:  uchType     : RPT_ALL_RESET     : All Read and Reset Type
*                           : RPT_EOD_ALLRESET  : Read and Reset by EOD Type
*                           : RPT_PTD_ALLRESET  : Read and Reset by PTD Type
*                           : RPT_WAI_RESET     : Read and Reset by Waiter Type
*               usNumber    : Guest Check Number/Waiter Number         
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read and Reset All Guest Check Total File.  
*===============================================================================
*/
static SHORT RptGuestAllReset(UCHAR uchType, ULONG ulNumber)
{
    SHORT           sError;
    SHORT           sNoofGC1 = 0;
    SHORT           sNoofGC2 = 0;
    SHORT           sGCSysStatus;
    USHORT          ausRcvBuff[GCF_MAX_GCF_NUMBER];
    UCHAR           uchRptType;
    UCHAR           uchResetCo = 0;

    /* Precheck GC File Exist */
    if ((sError = SerGusCheckExist()) == GCF_FILE_NOT_FOUND) {  /* GCF Not Created */
        return(GusConvertError(sError));                        
    } 
            
    RptPrtStatusSet(CLASS_TTLSAVDAY);                           /* Set J|R print status for Feed Ctl. */

    /* Initialize Sub/Grand Total Save Area */
    lRptGndTotal = 0L;
    lRptSubTotal = 0L;

    /* Control Header */
    uchRptType = uchType;
    if (uchType == RPT_EOD_ALLRESET || uchType == RPT_PTD_ALLRESET ) {
        uchRptType = RPT_ALL_RESET;
    }

    if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET ) {
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
    }
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_GCF_ADR,                            /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_RESET_ADR,                          /* Report Name Address */
                    uchRptType,                             /* Report Type */
                    AC_GCFL_RESET_RPT,                      /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */
    
    /* Precheck GC File Exist */
    if (sError == GCF_SUCCESS) {                                            /* GCF Not Exist */
        if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET)  {  /* Not EOD/PTD Report */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(SUCCESS);
        } else {
            return(LDT_NTINFL_ADR);
        }
    } else if (sError != GCF_EXIST) {
        RptPrtError(GusConvertError(sError));
        if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET)  {  /* Not EOD/PTD Report */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        }
        return(GusConvertError(sError));
    }

    if (uchType == RPT_WAI_RESET) {
        if (( sGCSysStatus = SerGusAllIdReadBW(GCF_COUNTER_TYPE, ulNumber, ausRcvBuff, sizeof(ausRcvBuff))) < 0) {  /* Error Case */
            sGCSysStatus = 0;
        }
    } else {
        if (( sGCSysStatus = SerGusAllIdRead(GCF_COUNTER_TYPE, ausRcvBuff, sizeof(ausRcvBuff))) < 0) {    /* Error Case */
            sGCSysStatus = 0;
        }
    }

    /* Check Guest Check System and Guest Check Id */
    if (RflGetSystemType() == FLEX_STORE_RECALL) {                  /* Drive Through Type */
        if (uchType != RPT_WAI_RESET) {
            sNoofGC1 = SerGusAllIdRead(GCF_DRIVE_THROUGH_STORE, ausRcvBuff, sizeof(ausRcvBuff));              
        } else {
            sNoofGC1 = SerGusAllIdReadBW(GCF_DRIVE_THROUGH_STORE, ulNumber, ausRcvBuff, sizeof(ausRcvBuff));
        }
        if (sNoofGC1 == GCF_NOT_IN) {
            sNoofGC1 = 0;
        } else if (sNoofGC1 < 0) {                                              /* Error Case */
            RptPrtError(GusConvertError(sNoofGC1));
            if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET)  {  /* Not EOD/PTD Report */
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            }
            return(GusConvertError(sNoofGC1));
        }
        
        /* Print Store/Recall Guest Check Total Report */
        if ((sError = RptGuestAllEdit(RPT_GCF_RESET, ausRcvBuff, sNoofGC1)) == RPT_ABORTED) {
            MaintMakeAbortKey();                                    /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            return(SUCCESS);
        } else if (sError != SUCCESS && sError != RPT_EXE_RESET) {
            RptPrtError(sError);
            if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET)  {  /* Not EOD/PTD Report */
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            }
            return(sError);
        }

        /* Count Up Reset Counter */
        if (sError == RPT_EXE_RESET && (uchType == RPT_ALL_RESET) || (uchType == RPT_WAI_RESET)) {
            MaintIncreSpcCo(SPCO_EODRST_ADR);                       /* Count Up EOD Reset Counter */
            uchResetCo = 1;                                         /* Set Reset Co. Flag */
        }
    }

    if (sNoofGC1 && sGCSysStatus) {                     /* Drive Through and Counter Type File Exist */
        /* Print Sub Total */                           
        RptFeed(RPT_DEFALTFEED);                                 /* Feed */
        /* Set Date  for Print Out      */
        /* |--------------------------| */
        /* | SUB TOTAL                | */
        /* |             SZZZZZZZ9.99 | */
        /* |--------------------------| */
        RptGuestElement(RPT_GCF_RESET, CLASS_RPTGUEST_PRTTTL, TRN_STTLR_ADR, 0, 0, 0, 0, 0, 0, lRptSubTotal);
        RptFeed(RPT_DEFALTFEED);                                 /* Feed */
    }

	// if Counter Type File exists, and no Drive Thru - CSMALL
    if (sGCSysStatus && !sNoofGC1) {                             /* Counter Type File Exist */
        /* Initialize Sub Total Save Area */
        lRptSubTotal = 0L;
   
        /* Read Guest Check Id for Counter Type */
        if (uchType == RPT_WAI_RESET) {
            sNoofGC2 = SerGusAllIdReadBW(GCF_COUNTER_TYPE, ulNumber, ausRcvBuff, sizeof(ausRcvBuff));
        } else {
            sNoofGC2 = SerGusAllIdRead(GCF_COUNTER_TYPE, ausRcvBuff, sizeof(ausRcvBuff));              
        }
        if (sNoofGC2 == GCF_NOT_IN) {
            sNoofGC2 = 0;
        } else if (sNoofGC2 < 0) {                                             /* Error Case */
            RptPrtError(GusConvertError(sNoofGC2));
            if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET)  { /* Not EOD/PTD Report */
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            }
            return(GusConvertError(sNoofGC2));
        }
    
        /* Print Counter Guest Check Total Report */
        if ((sError = RptGuestAllEdit(RPT_GCF_RESET, ausRcvBuff, sNoofGC2)) == RPT_ABORTED) {
            MaintMakeAbortKey();                                        /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            
            return(SUCCESS);
        } else if (sError != SUCCESS && sError != RPT_EXE_RESET) {
            RptPrtError(sError);
            if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET) {   /* Not EOD/PTD Type */
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
            }
            return(sError);
        }
    
        if (sNoofGC1) {                                                 /* Drive Through GCF Exist */
            /* Print Sub Total */
            RptFeed(RPT_DEFALTFEED);                                 /* Feed */
            RptGuestElement(RPT_GCF_RESET, CLASS_RPTGUEST_PRTTTL, TRN_STTLR_ADR, 0, 0, 0, 0, 0, 0, lRptSubTotal);
        }
    }

    /* Count Up Reset COunter */
    if (sError == RPT_EXE_RESET && (uchType == RPT_ALL_RESET) || (uchType == RPT_WAI_RESET)) {
        if (!uchResetCo) {                              /* Not Count Up Reset Co. */              
            MaintIncreSpcCo(SPCO_EODRST_ADR);                   /* Count Up EOD Reset Counter */
        }
    }                                                           
                                                                
    if (sNoofGC1 || sNoofGC2) {                                 /* Exist GC File */
        /* Print Grand Total */
        RptFeed(RPT_DEFALTFEED);                                /* Feed */
        RptGuestElement(RPT_GCF_RESET, CLASS_RPTGUEST_PRTTTL, TRN_TTLR_ADR, 0, 0, 0, 0, 0, 0, lRptGndTotal);
    }

    /* Control Trailer */
    if (uchType != RPT_EOD_ALLRESET && uchType != RPT_PTD_ALLRESET) {   /* Not EOD/PTD Type */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                    
    } else {
        RptFeed(RPT_DEFALTFEED);                                     /* Feed            */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestReset( UCHAR uchType, USHORT usNumber ) 
*               
*       Input:  uchType     : RPT_ALL_RESET      : All Read and Reset Type
*                           : RPT_IND_RESET      : Individual Read and Reset Type
*                           : RPT_WAITER_RESET   : Read and Reset by Waiter Type
*                           : RPT_EOD_ALLRESET   : Read and Reset by EOD Type
*                           : RPT_PTD_ALLRESET   : Read and Reset by PTD Type
*                           : RPT_OPEN_RESET     : Emergent Guest Check File Close Module
*               usNumber    : Guest Check Number/Waiter Number         
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read and Reset Guest Check Total File.  
*===============================================================================
*/
static SHORT RptGuestReset(UCHAR uchType, ULONG ulNumber)
{
    SHORT   sError;
	USHORT  usGuestCheck = (USHORT)ulNumber;

    /* Distingush Report Type */
    switch (uchType) {
    case RPT_ALL_RESET:
    case RPT_EOD_ALLRESET:
    case RPT_PTD_ALLRESET:
    case RPT_WAI_RESET:
        sError = RptGuestAllReset(uchType, ulNumber);
        if (uchType != RPT_WAI_RESET) {                 /* Add R3.0 */
            SerGusResetDeliveryQueue();                 /* Reset Delivery Queue */
        }
        break;

    case RPT_IND_RESET:
        sError = RptGuestIndReset(usGuestCheck);
        break;

    default:                                                    /* RPT_OPEN_RESET case */
        sError = RptGuestOpenReset(usGuestCheck);
        break;

    }
    return(sError);
}

SHORT RptGuestResetByCashier( ULONG ulCashierNumber )
{
	UCHAR uchType = RPT_WAI_RESET;

	return RptGuestReset(uchType, ulCashierNumber);
}

SHORT RptGuestResetByGuestCheck( UCHAR uchType, USHORT usGuestCheckNo )
{
	ULONG  ulNumber = usGuestCheckNo;

	return RptGuestReset(uchType, ulNumber);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestIndReset( USHORT usNumber ) 
*               
*       Input:  usNumber    : Guest Check Number         
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read and Reset Individual Guest Check Total File.  
*===============================================================================
*/
SHORT RptGuestIndReset(USHORT usNumber)
{
    SHORT   sError;

    RptPrtStatusSet(CLASS_TTLSAVDAY);                       /* Set J|R print status for Feed Ctl. */
 
    /* Control Header */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Increment Consecutive Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_GCF_ADR,                            /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_RESET_ADR,                          /* Report Name Address */
                    RPT_IND_RESET,                          /* Reset Individual Type */
                    AC_GCFL_RESET_RPT,                      /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */


    /* Print Guest Check Total Report */
    if ((sError = RptGuestIndEdit(RPT_GCF_RESET, usNumber, 0)) != SUCCESS && sError != RPT_EXE_RESET) {
        RptPrtError(sError);
    }

    /* Control Trailer and Reset Counter */
    if (sError == RPT_EXE_RESET)  {                             /* Executed Reset Report */
        MaintIncreSpcCo(SPCO_EODRST_ADR);                       /* Count Up EOD Reset Counter */
    }
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestOpenReset( USHORT usNumber ) 
*               
*       Input:  usNumber    : Guest Check Number         
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Emergent Guest Check File Close Module.  
*===============================================================================
*/
SHORT RptGuestOpenReset(USHORT usNumber)
{
    SHORT           sError;
	RPTGUEST        RptGuest = {0};
    UCHAR           uchCDV;
    UCHAR           uchMinorClass;

    /* RptPrtStatusSet(CLASS_TTLSAVDAY);                        Set J|R print status */
    
    /* Emergent Guest Check File Close */
    if ((sError = SerGusResetReadFlag(usNumber)) != SUCCESS) {
        return(GusConvertError(sError));
    }

    /* Control Header */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Increment Consecutive Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_PARA,                 /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    0,                                      /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    0,                                      /* Report Name Address */
                    0,                                      /* Reset Type */
                    AC_EMG_GCFL_CLS,                        /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */
    
    /* Check CDV System */
    uchMinorClass = CLASS_RPTGUEST_PRTCHKNOWOCD;
    uchCDV = 0;
    if (RflGetSystemType() != FLEX_STORE_RECALL && CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT1)) {         /* CDV System */
        uchMinorClass = CLASS_RPTGUEST_PRTCHKNOWCD;
        uchCDV = RflMakeCD(usNumber);       
    }

    /* Set Major/Minor Class */
    RptGuest.uchMajorClass = CLASS_RPTOPENGCF;
    RptGuest.uchMinorClass = uchMinorClass;

    /* Set Print Control by MDC */
    RptGuest.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
	RflGetTranMnem (RptGuest.aszMnemo, TRN_GCNO_ADR);

    /* Set Guest Check Number/Cdv */
    RptGuest.usGuestNo = usNumber;
    RptGuest.uchCdv = uchCDV;
    RptGuest.ulCashierId = 0;

    /* Output Data to Print Module */
    PrtPrintItem(NULL, &RptGuest);

    /* RptFeed(RPT_DEFALTFEED);                              Feed Line */

    /* Control Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestAllEdit( UCHAR uchType,
*                                    USHORT ausRcvBuff[],
*                                    SHORT sNoofGC ) 
*               
*       Input:  uchType     : Report Type
*               ausRcvBuff  : Address of Guest Check Id Saved Area
*               sNoofGC     : Number of Guest Check          
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description: Prepare to Print Guest Check Total File.  
*===============================================================================
*/
SHORT RptGuestAllEdit( UCHAR  uchType,
                       USHORT ausRcvBuff[],
                       SHORT  sNoofGC )
{
    USHORT  i;
    SHORT   sError = SUCCESS;
    SHORT   sReturn = SUCCESS;


    /* Print Each Guest Check Total File */  
    for (i = 0; i < sNoofGC/sizeof(USHORT); i++) {
        if (uchType == RPT_GCF_READ) {                              /* All Read Case */
            if (UieReadAbortKey() == UIE_ENABLE) {                  /* Aborted */
                return(RPT_ABORTED);                                
            }
            if(RptPauseCheck() == RPT_ABORTED){
                return(RPT_ABORTED);
            }
        }
        if ((sError = RptGuestIndEdit(uchType, ausRcvBuff[i], i)) == RPT_EXE_RESET) {
            sReturn = RPT_EXE_RESET;
        }

        if (sError == RPT_ABORTED) {
            return(sError);
        }
    }
    if (sReturn == RPT_EXE_RESET) {
        return(sReturn);
    }
    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestIndEdit( UCHAR uchType, USHORT usGCNumber, SHORT sFeedType ) 
*               
*       Input:  uchType     : RPT_GCF_READ      : Read Type
*                           : RPT_GCF_RESET     : Read and Reset Type
*               usGCNumber  : Guest Check Number         
*               sFeedType   : The number of Operation
*                                           
*      Output:  Nothing
*
**  Return: SUCCESS         : Not Execute Reset Report
*           RPT_EXE_RESET   : Execute Reset Report
*
**  Description: Print One Guest Check Total File.  
*===============================================================================
*/
SHORT RptGuestIndEdit( UCHAR uchType, USHORT usGCNumber, SHORT sFeedType )
{
    SHORT           sError;
    SHORT           sReturn = SUCCESS;
    TRANGCFQUAL     TranGCFQual = {0};
    UCHAR           uchMinorClass;
    UCHAR           uchSpeAddr;
    UCHAR           uchCDV;
	UCHAR			uchSeat;

    /* Check Guest Check File Exist */
    if ((sError = SerGusIndRead(usGCNumber, ( UCHAR *)&TranGCFQual, sizeof(TranGCFQual))) == GCF_NOT_IN) {
        return(SUCCESS);
    } else if (sError < 0) {
        return(GusConvertError(sError));                                                                             
    }

    if (sFeedType) {
        RptFeed(RPT_DEFALTFEED);                    /* Feed Line */
        if (uchRptMldAbortStatus) return(SUCCESS);  /* aborted by MLD */
    }

    /* Print Training Mnemonics */
    if (sError && (TranGCFQual.fsGCFStatus & GCFQUAL_TRAINING)) {                   /* Training Mode */
		RPTGUEST        RptGuest = {0};

        RptGuest.uchMajorClass = CLASS_RPTGUEST;
        RptGuest.uchMinorClass = CLASS_RPTGUEST_PRTMNEMO;
        RptGuest.usPrintControl = RptGuestGetRptCtl(uchType);
		RflGetSpecMnem (RptGuest.aszSpeMnemo, SPC_TRNGID_ADR);
        RptGuest.uchStatus = uchType;

		if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptGuest, 0); /* display on LCD          */ 
            RptGuest.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptGuest);
    }

	/* Special Display for Pre-Authorization */
	uchMinorClass = CLASS_RPTGUEST_PRTCHKNOWOCD;
	for (uchSeat = 0; uchSeat <= NUM_SEAT_CARD; uchSeat++) {
		if (TranGCFQual.auchAcqRefData[uchSeat][0] != '\0') {
			uchMinorClass = CLASS_RPTGUEST_PRTCHKNOWOCD_PREAUTH;
			break;
		} 		
	}
	
    /* Check CDV System */
    uchCDV = 0;
    if (RflGetSystemType() != FLEX_STORE_RECALL && CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT1)) {         /* CDV System */
        uchMinorClass = CLASS_RPTGUEST_PRTCHKNOWCD;
        if (!sError) {                                          /* Record Size is 0 */
            uchCDV = RflMakeCD(usGCNumber);       
        } else {
            uchCDV = TranGCFQual.uchCdv;       
        }
    }

    if (!sError) {              /* Record Size is 0 */
        /* Print Guest Check Number */
		/* | CHK# 999999              | */
        RptGuestElement(uchType, uchMinorClass, TRN_GCNO_ADR, 0, usGCNumber, uchCDV, 0, 0, 0, 0L);

        /* Check Report Type */
        if (uchType == RPT_GCF_RESET) {
            /* send print data to shared printer module */
            PrtShrPartialSend(RptGuestGetRptCtl(uchType));                

            /* check if printer trouble occurs */
            if (PrtCheckAbort() == PRT_ABORTED) {
                return(RPT_ABORTED);
            } else {
                SerGusDelete(usGCNumber);           /* Reset Guest Check File */
                return(RPT_EXE_RESET);
            }
        }
        return(SUCCESS);
    } else {
		ULONG   ulCashWaitNo;

        /* Check Cashier or Waiter */
        uchSpeAddr = SPC_CAS_ADR;
        ulCashWaitNo = TranGCFQual.ulCashierID;

        /* Print Guest Check Number and Waiter Number */
		/* | CHK# 999999     WAIT 999 | */
        RptGuestElement(uchType, uchMinorClass, TRN_GCNO_ADR, uchSpeAddr, TranGCFQual.usGuestNo, uchCDV, ulCashWaitNo, 0, 0, 0L);
    }

    if (!CliParaMDCCheck(MDC_RPTGCF1_ADR, ODD_MDC_BIT1)) {      /* Print Time Opened Case */                    
		/* | OPENED           Z9:99AM | */
        RptGuestElement(uchType, CLASS_RPTGUEST_PRTCHKOPEN, TRN_TOPEN_ADR, 0, 0, 0, 0, TranGCFQual.auchCheckOpen, 0, 0L);
    }

    if (!CliParaMDCCheck(MDC_RPTGCF1_ADR, ODD_MDC_BIT2)) {      /* Print Table Number Case */                    
		/* | TABLE #              ZZ9 | */
        RptGuestElement(uchType, CLASS_RPTGUEST_PRTTBL, 0, SPC_TBL_ADR, 0, 0, 0, 0, TranGCFQual.usTableNo, 0L);
    }

    if (!CliParaMDCCheck(MDC_RPTGCF1_ADR, ODD_MDC_BIT3)) {      /* Print Number of Person Case */                    
		/* | #.PERSON             ZZ9 | */
        RptGuestElement(uchType, CLASS_RPTGUEST_PRTPERSON, TRN_PSN_ADR, 0, 0, 0, 0, 0, TranGCFQual.usNoPerson, 0L);
    }

    if (!CliParaMDCCheck(MDC_RPTGCF1_ADR, EVEN_MDC_BIT0)) {     /* Print Old Waiter Number Case */                    
		/* | OLD WAI#             999 | */
		RptGuestElement(uchType, CLASS_RPTGUEST_PRTPERSON, TRN_OLDWT_ADR, 0, 0, 0, TranGCFQual.ulPrevCashier, 0, 0, 0L);
    }

    if (!CliParaMDCCheck(MDC_RPTGCF1_ADR, EVEN_MDC_BIT1)) {     /* Print Transferred Blance Case */                    
		/* | TRA FROM    SZZZZZZZ9.99 | */
		RptGuestElement(uchType, CLASS_RPTGUEST_PRTDGG, TRN_TRNSB_ADR, 0, 0, 0, 0, 0, 0, TranGCFQual.lTranBalance);
    }

    /* Print Current Blance */
	/* | CHECK TL    SZZZZZZZ9.99 | */
    RptGuestElement(uchType, CLASS_RPTGUEST_PRTDGG, TRN_CURB_ADR, 0, 0, 0, 0, 0, 0, TranGCFQual.lCurBalance);

    /* Add Sub Total and Grand Total */
    if (!(TranGCFQual.fsGCFStatus & GCFQUAL_TRAINING)) {                /* Not Training Mode */
        lRptSubTotal += TranGCFQual.lCurBalance;                        
        lRptGndTotal += TranGCFQual.lCurBalance;
    }

    /* Check Report Type */
    if (uchType == RPT_GCF_RESET) {
        /* send print data to shared printer module */
        PrtShrPartialSend(RptGuestGetRptCtl(uchType));                

        /* check if printer trouble occurs */
        if (PrtCheckAbort() == PRT_ABORTED) {
            return(RPT_ABORTED);
        } else {
            SerGusDelete(usGCNumber);           /* Reset Guest Check File */
            sReturn = RPT_EXE_RESET;            /* Set Execute Reset Status */
        }
    }
    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: VOID RptGuestElement( UCHAR  uchType,
*                                   UCHAR  uchMinorClass,
*                                   UCHAR  uchTransAddr,
*                                   UCHAR  uchSpeAddr,
*                                   USHORT usGCNumber,
*                                   UCHAR  uchCdv,
*                                   USHORT usCashWaitNo,
*                                   UCHAR  *puchCheckOpen,
*                                   USHORT usNumber,
*                                   LONG   lTotal ) 
*                   
*       Input: uchType          : Read/Reset Type
*              uchMinorClass    : Minor Class
*              uchTransAddr     : Transaction Mnemonics Address
*              uchSpeAddr       : Special Mnemonics Address
*              usGCNumber       : Guest Check Number
*              uchCdv           : Cdv
*              usCashWaitNo     : Cashier/Waiter Number
*              *puchCheckOpen   : Pointer for Checked Open Save Area
*              usNumber         : Number of Person/Old Waiter Number
*              lTotal           : Total
*                                 
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Format Print Format of Guest Check Report.  
*===============================================================================
*/
VOID RptGuestElement( UCHAR  uchType,
                      UCHAR  uchMinorClass,
                      USHORT usTransAddr,
                      UCHAR  uchSpeAddr,
                      USHORT usGCNumber,
                      UCHAR  uchCdv,
                      ULONG  ulCashWaitNo,
                      UCHAR  *puchCheckOpen,
                      USHORT usNumber,
                      D13DIGITS   d13Total )
{
	RPTGUEST            RptGuest = {0};

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

    /* Preset Each Parameter */
    RptGuest.uchMajorClass = CLASS_RPTGUEST;
    /* Set Print Control by MDC */
    RptGuest.usPrintControl = RptGuestGetRptCtl(uchType);

    /* set "uchType" for distinguishing A/C No. in thermal module */
    RptGuest.uchStatus = uchType;

    /* Distingush Minor Class */
    switch (uchMinorClass) {
	case CLASS_RPTGUEST_PRTCHKNOWOCD_PREAUTH:
    case CLASS_RPTGUEST_PRTCHKNOWCD:                                /* Set Date  for Print Out      */
    case CLASS_RPTGUEST_PRTCHKNOWOCD:                               /* |--------------------------| */
                                                                    /* | CHK# 999999     WAIT 999 | */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    
        RflGetTranMnem (RptGuest.aszMnemo, usTransAddr);            /* |--------------------------| */
                                                                    /* | CHK# 999999              | */
                                                                    /* |--------------------------| */

        if (uchSpeAddr) {                       /* Set Special Mnemonics Case */                        
            /* Get and Set Special Mnemonics */
			RflGetSpecMnem (RptGuest.aszSpeMnemo, uchSpeAddr);
        }

        /* Set Guest Check Number/Cdv/Cashier/Waiter Number */
        RptGuest.usGuestNo = usGCNumber;
        RptGuest.uchCdv = uchCdv;
        RptGuest.ulCashierId = ulCashWaitNo;

        /* Set Minor Class */
        RptGuest.uchMinorClass = uchMinorClass;
        break;

    case CLASS_RPTGUEST_PRTCHKOPEN:                                 /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | OPENED           Z9:99AM | */
        RflGetTranMnem (RptGuest.aszMnemo, usTransAddr);            /* |--------------------------| */

        /* Set Checked Open */
        memcpy(RptGuest.auchCheckOpen, puchCheckOpen, sizeof(RptGuest.auchCheckOpen));
        
        /* Set Minor Class */
        RptGuest.uchMinorClass = CLASS_RPTGUEST_PRTCHKOPEN;
        break;

    case CLASS_RPTGUEST_PRTPERSON:                                  /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | #.PERSON             ZZ9 | */
        RflGetTranMnem (RptGuest.aszMnemo, usTransAddr);            /* |--------------------------| */

        /* Set Number of Person/Table Number and the operator id */
        RptGuest.usNoPerson = usNumber;
        RptGuest.ulCashierId = ulCashWaitNo;

        /* Set Minor Class */
        RptGuest.uchMinorClass = CLASS_RPTGUEST_PRTPERSON;
        break;

    case CLASS_RPTGUEST_PRTTBL:                                     /* Set Date  for Print Out      */
        /* Get and Set Special Mnemonics */                         /* |--------------------------| */
                                                                    /* | TABLE #              ZZ9 | */
        RflGetSpecMnem (RptGuest.aszSpeMnemo, uchSpeAddr);          /* |--------------------------| */

        /* Set Number of Person/Table Number */
        RptGuest.usNoPerson = usNumber;

        /* Set Minor Class */
        RptGuest.uchMinorClass = CLASS_RPTGUEST_PRTTBL;
        break;

    case CLASS_RPTGUEST_PRTTTL:                                     /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | SUB TOTAL                | */
        RflGetTranMnem (RptGuest.aszMnemo, usTransAddr);            /* |             SZZZZZZZ9.99 | */
                                                                    /* |--------------------------| */

        /* Set Total Data */
        RptGuest.lCurBalance = d13Total;

        /* Set Minor Class */
        RptGuest.uchMinorClass = CLASS_RPTGUEST_PRTTTL;
        break;

    case CLASS_RPTGUEST_PRTDGG:                                     /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | TRA FROM    SZZZZZZZ9.99 | */
        RflGetTranMnem (RptGuest.aszMnemo, usTransAddr);            /* |--------------------------| */

        /* Set Total */
        RptGuest.lCurBalance = d13Total;

        /* Set Minor Class */
        RptGuest.uchMinorClass = CLASS_RPTGUEST_PRTDGG;
        break;

    default:
/*        PifAbort(MODULE_RPT_ID, FAULT_INVALID_DATA); */
        return;
    }

    /* Output Data to Print Module */
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptGuest, 0); /* display on LCD          */ 
        RptGuest.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptGuest);
}

/*
*=============================================================================
**  Synopsis: SHORT RptGuestLock( UCHAR uchType, USHORT usNumber ) 
*               
*       Input:  uchType     : RPT_IND_LOCK      : Individual Lock Type
*                           : RPT_ALL_LOCK      : All Lock Type
*                           : RPT_WAI_LOCK      : Lock by Waiter Type
*               usNumber    : Guest Check Number/Waiter Number         
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Lock Guest Check Total File.  
*===============================================================================
*/
SHORT RptGuestLockByCashier( ULONG ulCashierNumber )
{
    SHORT           sError;
    USHORT          ausRcvBuff[1];
	CASIF           Arg = {0};

    /* Check Waiter Exist */
    Arg.ulCashierNo = ulCashierNumber;                        
    if ((sError = SerCasIndRead(&Arg)) != CAS_PERFORM) {   
        return(CasConvertError(sError));
    }

    if ((sError = SerGusAllLock()) != GCF_SUCCESS) { 
        SerCasIndUnLock(ulCashierNumber);        /* Unlock Waiter */
        SerCasAllUnLock();                       /* Unlock Cashier */
        return(GusConvertError(sError));
    }                                         

    if (RflGetSystemType() == FLEX_STORE_RECALL) {                  /* Drive Through Type */
        if ((sError = SerGusAllIdReadBW(GCF_DRIVE_THROUGH_STORE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff))) >= 0) {
            return(SUCCESS);
        }
    } else {
        if ((sError = SerGusAllIdReadBW(GCF_COUNTER_TYPE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff))) >= 0) {
            return(SUCCESS);
        }
    }
    SerCasAllUnLock();                          /* Unlock Cashier */
    SerGusAllUnLock(); 
    return(GusConvertError(sError));
}

SHORT RptGuestLockByGuestCheck( UCHAR uchType, USHORT usGuestCheckNo )
{
    SHORT     sError;

    if (uchType == RPT_IND_LOCK) {                      /* Individual Lock Type */
        if ((sError = SerGusReadLock(usGuestCheckNo, NULL, 0, GCF_SUPER)) != GCF_SUCCESS) {  /* R3.0 */
            return(GusConvertError(sError));
        }
    } else {                                            /* All/By Waiter All Lock Type */
        if ((sError = SerGusAllLock()) != GCF_SUCCESS) { 
            SerCasAllUnLock();                       /* Unlock Waiter */
            SerCasAllUnLock();                       /* Unlock Cashier */
            return(GusConvertError(sError));
        }                                         
    }

    if ((sError = SerGusCheckExist()) == GCF_SUCCESS) { /* GCF Not Exist or Other Error */
        SerCasAllUnLock();                              /* Unlock Cashier */

        if (uchType == RPT_IND_LOCK) {                  /* Individual Lock Type */
            SerGusResetReadFlag(usGuestCheckNo);
        } else {                                        /* All/By Waiter All UnLock Type */
            SerGusResetDeliveryQueue();                 /* Reset Delivery Queue */
            SerGusAllUnLock(); 
        }
        return(LDT_NTINFL_ADR);
    }
    return(SUCCESS);
}

#if 0
SHORT RptGuestLock( UCHAR uchType, ULONG ulCashierNumber )
{
    SHORT           sError;
    USHORT          ausRcvBuff[1];
    CASIF           Arg;
    PARAFLEXMEM     ParaFlexMem;

    /* Check Waiter Exist */
    if (uchType == RPT_WAI_LOCK) {
        Arg.ulCashierNo = ulCashierNumber;                        
        if ((sError = SerCasIndRead(&Arg)) != CAS_PERFORM) {   
            return(CasConvertError(sError));
        }
    }


#if 0	// Do not need to lock file, so AC3 can be run when operators
		//	signed in on other terminals. CSMALL

    /* Lock All Waiter/Cashier File */
    if (uchType == RPT_ALL_LOCK) {

        if (((sError = SerCasAllLock()) != CAS_PERFORM) &&
            (sError != CAS_FILE_NOT_FOUND)) {
            return(CasConvertError(sError));
        }
    } else if (uchType == RPT_WAI_LOCK) {

        if (((sError = SerCasAllLock()) != CAS_PERFORM) &&
            (sError != CAS_FILE_NOT_FOUND)) {
            return(CasConvertError(sError));
        }
    }
#endif
    
	/* Distingush Lock Type */
    if (uchType == RPT_IND_LOCK) {                      /* Individual Lock Type */
        if ((sError = SerGusReadLock(ulCashierNumber, NULL, 0, GCF_SUPER)) != GCF_SUCCESS) {  /* R3.0 */
      /**        && (sError != GCF_FULL)) {          **/
            return(GusConvertError(sError));
        }
    } else {                                            /* All/By Waiter All Lock Type */
        if ((sError = SerGusAllLock()) != GCF_SUCCESS) { 
            if (uchType == RPT_WAI_LOCK) {
               SerCasIndUnLock(ulCashierNumber);        /* Unlock Waiter */
            } else {
               SerCasAllUnLock();                       /* Unlock Waiter */
            }
            SerCasAllUnLock();                          /* Unlock Cashier */
            return(GusConvertError(sError));
        }                                         
    }

    /* Precheck GC File Exist */
    if (uchType == RPT_WAI_LOCK) {
        /* Get Guest Check System Information */
        ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
        ParaFlexMem.uchAddress = FLEX_GC_ADR;
        CliParaRead(&ParaFlexMem);

        if (ParaFlexMem.uchPTDFlag == FLEX_STORE_RECALL) {                  /* Drive Through Type */
            if ((sError = SerGusAllIdReadBW(GCF_DRIVE_THROUGH_STORE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff))) >= 0) {
                return(SUCCESS);
            }
        } else {
            if ((sError = SerGusAllIdReadBW(GCF_COUNTER_TYPE, ulCashierNumber, ausRcvBuff, sizeof(ausRcvBuff))) >= 0) {
                return(SUCCESS);
            }
        }
        SerCasAllUnLock();                          /* Unlock Cashier */
        SerGusAllUnLock(); 
        return(GusConvertError(sError));
    }

    if ((sError = SerGusCheckExist()) == GCF_SUCCESS) { /* GCF Not Exist or Other Error */

        SerCasAllUnLock();                          /* Unlock Cashier */

        if (uchType == RPT_IND_LOCK) {                  /* Individual Lock Type */
            SerGusResetReadFlag(ulCashierNumber);
        } else {                                        /* All/By Waiter All UnLock Type */
            if (uchType != RPT_WAI_RESET) {             /* Add R3.0 */
                SerGusResetDeliveryQueue();             /* Reset Delivery Queue */
            }
            SerGusAllUnLock(); 
        }
        return(LDT_NTINFL_ADR);
    }
    return(SUCCESS);
}
#endif

/*
*=============================================================================
**  Synopsis: SHORT RptGuestUnLock( UCHAR uchType, USHORT usNumber ) 
*               
*       Input:  uchType     : RPT_IND_UNLOCK    : Individual UnLock Type
*                           : RPT_ALL_UNLOCK    : All UnLock Type
*                           : RPT_WAI_UNLOCK    : UnLock by Waiter Type
*               usNumber    : Guest Check Number/Waiter Number         
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Unlock Guest Check Total File.  
*===============================================================================
*/
SHORT   RptGuestUnlockByCashier( ULONG ulCashierNumber )
{
    SerCasIndUnLock(ulCashierNumber);
    SerCasAllUnLock();
    SerGusAllUnLock(); 
    return(SUCCESS);
}

SHORT   RptGuestUnlockByGuestCheck( UCHAR uchType, USHORT usGuestCheckNo )
{
    SerCasAllUnLock();
    if (uchType == RPT_IND_UNLOCK) {                    /* Individual UnLock Type */
		SerGusResetReadFlag(usGuestCheckNo);
	}
    SerCasAllUnLock();
    SerGusAllUnLock(); 
    return(SUCCESS);
}

#if 0
SHORT RptGuestUnLock(UCHAR uchType, ULONG ulCashierNumber)
{
    /* Unlock Waiter */
    if (uchType == RPT_WAI_UNLOCK) {
        SerCasIndUnLock(ulCashierNumber);
    } else {
        SerCasAllUnLock();
    }

    /* Unlock CASHIER/GCF */
    if (uchType == RPT_IND_UNLOCK) {                    /* Individual UnLock Type */
        SerGusResetReadFlag(ulCashierNumber);
    } else {                                            /* All/By Waiter All UnLock Type */
        SerCasAllUnLock();
        SerGusAllUnLock(); 
    }
    return(SUCCESS);
}
#endif

  
/*
*=============================================================================
**  Synopsis: USHORT RptGuestGetPrtCtl( UCHAR uchType )
* 
*       Input:  uchType     : RPT_GCF_READ      : Read Type 
*                           : RPT_GCF_RESET     : Reset Type
*      Output:  Nothing
*
**  Return:     Print Control Status  
*
**  Description:  
*       Check Read or Reset report, and check also MDC bit.
*       Return status of those condition.  
*===============================================================================
*/
USHORT RptGuestOptimizeFile ()
{
	RPTGUEST            RptGuest = {0};
	CLIGUS_TALLY_HEAD   TallyHead;

   MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_GCF_ADR,                            /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_FILE_ADR,                           /* Report Name Address */
                    1,                                /* Read Type */
                    AC_GCFL_READ_RPT,                       /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */

	//Call to the guest check subsystem to
	//uptade tallies and counts.
	GusOptimizeGcfDataFile (&TallyHead);

    /* Preset Each Parameter 
		Set Print Control by MDC
		set "uchType" for distinguishing A/C No. in thermal module
	 */
    RptGuest.uchMajorClass = CLASS_RPTGUEST;
	RptGuest.uchMinorClass = CLASS_RPTGUEST_FILE_OPTIMIZED;
	RflGetTranMnem (RptGuest.aszMnemo, TRN_OPTIMIZE_FILE_ADR);
    RptGuest.usPrintControl = RptGuestGetRptCtl(RPT_GCF_READ);
    RptGuest.uchStatus = RPT_GCF_READ;

	/* Output Data to Print Module */
    PrtPrintItem(NULL, &RptGuest);

	MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

	return 0;
}

USHORT RptGuestReportOnFile ()
{
	CLIGUS_TALLY_INDEX  *pTallyArray = 0;
	USHORT              usArrayCount = 0;
	RPTGUEST            RptGuest = {0};
	CLIGUS_TALLY_HEAD   TallyHead = {0};

	// pTallyArray = (CLIGUS_TALLY_INDEX *)_alloca (sizeof(CLIGUS_TALLY_INDEX) * usArrayCount);
	GusWalkGcfDataFile (&TallyHead, pTallyArray, &usArrayCount);

    /* Preset Each Parameter
		Set Print Control by MDC
		set "uchType" for distinguishing A/C No. in thermal module
	 */
    RptGuest.uchMajorClass = CLASS_RPTGUEST;
	RptGuest.uchMinorClass = CLASS_RPTGUEST_FILEINFO;
	_tcscpy (RptGuest.aszSpeMnemo, _T("GcfXXt"));
    RptGuest.usPrintControl = RptGuestGetRptCtl(RPT_GCF_READ);
    RptGuest.uchStatus = RPT_GCF_READ;

	/* Output Data to Print Module */
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptGuest, 0); /* display on LCD          */ 
        RptGuest.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }

    PrtPrintItem(NULL, &RptGuest);

	return 0;
}