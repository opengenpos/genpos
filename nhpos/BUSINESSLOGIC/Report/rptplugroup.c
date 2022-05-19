/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : PLU Group Summary Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application          
* Program Name: RPTCPN.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*       RptCpnDayRead()               : Read Daily Coupon Report Module
*       RptCpnPTDRead()               : Read PTD Coupon Report Module
*       RptCpnRead()                  : Read Daily and PTD Coupon report
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
*    -  -  : 00.00.01   : Waki      : initial                                   
* Jul-27-95: 03.00.03   : M.Ozawa   : Add Report on Display
* Aug-11-99: 03.05.00   : M.Teraki  : R3.5 remove WAITER_MODEL
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
============================================================================
;                      I N C L U D E     F I L E s                         
;===========================================================================
*/

#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <log.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csop.h>
#include <csstbopr.h>
#include <csttl.h>
#include <csstbttl.h>
#include <csstbstb.h>
#include <cscas.h>
#include <cswai.h>
#include <csstbwai.h>
#include <csstbcas.h>
#include <report.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"

#include "bl.h"

/*************************************************************/
LTOTAL   gTotalCpn;     /* Grand Total Calc Area */

/*
*===========================================================================
**  Synopsis: SHORT RptCpnRead(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchNumber) 
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*               UCHAR  uchType         : RPT_IND_READ  
*                                      : RPT_ALL_READ
*               UCHAR  uchNumber       : Cpn number
*
*      Output:  nothing
*
**     Return:  SUCCESS     
*               RPT_ABORTED
*               LDT_ERR_ADR     (TTL_FAIL)
*               LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
*               LDT_KEYOVER_ADR
*
**  Description:              
*       Read Daily or PTD Cpn report. 
*===========================================================================
*/
SHORT  RptPLUGroupRead(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchNumber) 
{
    
    SHORT           sReturn = SUCCESS;
    //PARAFLEXMEM      FlexMem;

    //if(uchType == RPT_IND_READ){
    //    FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;                          /* Set Major for # of Record */
    //    FlexMem.uchAddress = FLEX_CPN_ADR;                                 /* Set Address for # of Record */
    //    CliParaRead(&FlexMem);                                              /* Get # of Record */
    //    if ((uchNumber < 1) || (uchNumber > (UCHAR)FlexMem.ulRecordNumber)) {   /* Saratoga */
    //        return(LDT_KEYOVER_ADR);            
    //    }
    //}
    /* Edit Total Report */

    RptPLUGroupHeader(uchMinorClass, uchType);                              /* Call Header Func. */

    switch(uchType) {
    case RPT_IND_READ :                                 /* Individual read ? */
        sReturn = RptPLUGroupIndProc(uchMinorClass, uchNumber, uchType);
        if (sReturn == RPT_ABORTED) {
            RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
            MaintMakeAbortKey();                                    /* Print ABORTED   */
        }
        //return(sReturn);
        break;

    case RPT_EOD_ALLREAD:                               /* EOD report */
    case RPT_PTD_ALLREAD:                               /* PTD report */
    case RPT_ALL_READ:                                  /* All read ? */
        sReturn = RptPLUGroupAllProc(uchMinorClass, uchType);
        break;

    case RPT_FIN_READ:                                  /* Individual Read END */
        if(uchRptOpeCount & RPT_HEADER_PRINT){          /* Not First Operation */
            RptFeed(RPT_DEFALTFEED);                        /* Line Feed        */
            RptCpnGrandTtlPrt(&gTotalCpn, uchType);         /* Grand Total Print */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);    /* Print Trailer */
            return(sReturn);
        }
        return(sReturn);
        break;

    default:
        return(LDT_SEQERR_ADR);
                                                                
    }
    if (sReturn == RPT_ABORTED) {
        RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
        MaintMakeAbortKey();                                    /* Print ABORTED   */
    }
    if((uchType != RPT_TYPE_IND)){
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
    }
    return(sReturn);
}

/*
*===========================================================================
**  Synopsis: SHORT RptCpnIndProc(UCHAR uchMinorClass, 
*                                 UCHAR uchCpnNumber) 
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*               UCHAR  uchCpnNumber   : Cpn number
*
*      Output:  nothing
*
**  Return: SUCCESS      
*           RPT_ABORTED
*           LDT_ERR_ADR     (TTL_FAIL)
*           LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
* 
**  Description:  
*       individual Cpn report.
*
*===========================================================================
*/
SHORT  RptPLUGroupIndProc(UCHAR uchMinorClass, UCHAR uchGroupNumber, UCHAR uchType)
{    
    SHORT    sReturn, sTempReturn;
	RPTPLU		RptPLUGroup;
	BL_PLU_INFO		pluInfo;
	WCHAR     auchTempPLUNo[16];   // PLU number should be at least as big as OP_PLU_LEN
    LABELANALYSIS   PluLabel;
	PLUIF pluIf;
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    memset(&RptPLUGroup, 0, sizeof(RptPLUGroup));

    if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT0))) {
        RptPLUGroup.usPrintControl |=  PRT_JOURNAL;
    }
    if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT1))) {
        RptPLUGroup.usPrintControl |=  PRT_RECEIPT;
    }
	usRptPrintStatus = RptPLUGroup.usPrintControl;
                                                                                                        
    RptFeed(RPT_DEFALTFEED);                        /* Line Feed           */

    RptPLUGroup.uchMajorClass = CLASS_RPTPLU;            /* PLU (Group) Report Set */
    RptPLUGroup.uchMinorClass = CLASS_RPTPLUGROUP_PRTGRP;       /* PLU Group Number Set */
	RptPLUGroup.usReportCode = uchGroupNumber;

	memset(&pluInfo, 0, sizeof(pluInfo));
	// get the first PLU member of the selected group
	sReturn = BlFwIfIntrnlGetGroupPluInformationFirst(0, uchGroupNumber, 0, 0L, 0, &pluInfo);

	// Display the header
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPLUGroup, (USHORT)uchType);  /* Print each Element  */
        RptPLUGroup.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }

    PrtPrintItem(NULL, &RptPLUGroup);                    /* Individual Print For One Coupon */

	while (sReturn == 0)
	{
		RptPLUGroup.uchMinorClass = CLASS_RPTPLUGROUP_PRTPLU;
		RptPLUGroup.usReportCode = pluInfo.usGroupNo;
		RptPLUGroup.usDEPTNumber = pluInfo.usDeptNo;

		// check if current PLU is a repeat of the last displayed PLU
		RflConvertPLU(auchTempPLUNo, pluInfo.auchPluNo);
		if ( _tcscmp(auchTempPLUNo, RptPLUGroup.auchPLUNumber) == 0) // if same PLU is repeated, skip displaying
		{
			sReturn = BlFwIfIntrnlGetGroupPluInformationNext(&pluInfo);
			continue;
		}

		//get PLU Dept. No.
		memset(&pluIf, 0, sizeof(pluIf));
		memset(&PluLabel, 0, sizeof(PluLabel));

		RflConvertPLU(auchTempPLUNo, pluInfo.auchPluNo);

		_tcsncpy(PluLabel.aszScanPlu, auchTempPLUNo, OP_PLU_LEN);

		RflLabelAnalysis(&PluLabel);

		_tcsncpy(pluIf.auchPluNo, /*auchTempPLUNo*/PluLabel.aszLookPlu, NUM_PLU_LEN);        // Set PLU No   
		pluIf.uchPluAdj = 1;                                         // Must put 1 to read 1st PLU  
		
		if ((sTempReturn = CliOpPluIndRead(&pluIf, 0)) != OP_PERFORM) {
			return(OpConvertError(sTempReturn));
		}
		
		RptPLUGroup.usDEPTNumber = pluIf.ParaPlu.ContPlu.usDept; //set Dept No.

		RflConvertPLU(RptPLUGroup.auchPLUNumber, PluLabel.aszLookPlu);
		
		_tcsncpy(RptPLUGroup.aszMnemo, pluInfo.uchPluName, PARA_PLU_LEN);              /* Copy Mnemonics */

		if (RptCheckReportOnMld()) {
			uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPLUGroup, (USHORT)uchType);  /* Print each Element  */
			RptPLUGroup.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
		}

		PrtPrintItem(NULL, &RptPLUGroup);                    /* Individual Print For One Coupon */

		/* abort check */
		if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
			return(RPT_ABORTED);
			break;
		}
		if(RptPauseCheck() == RPT_ABORTED){
			return(RPT_ABORTED);
		}

		RptPLUGroup.PLUTotal.lCounter++;     /* Counter Set */

		sReturn = BlFwIfIntrnlGetGroupPluInformationNext(&pluInfo);
	}

    /* send print data to shared module */
    PrtShrPartialSend(RptPLUGroup.usPrintControl);

    return(SUCCESS);
}                                                                        

/*
*===========================================================================
**  Synopsis: SHORT RptCpnAllProc(UCHAR uchMinorClass)
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*                                      : CLASS_TTLSAVDAY
*                                      : CLASS_TTLSAVPTD
*
*      Output:  nothing
*
**  Return: SUCCESS      
*           RPT_ABORTED
*           LDT_ERR_ADR     (TTL_FAIL)
*           LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
* 
**  Description:  
*       All Cpn report.
*
*===========================================================================
*/
SHORT  RptPLUGroupAllProc(UCHAR uchMinorClass, UCHAR uchType)
{    
    SHORT    sReturn, sTempReturn;
	RPTPLU		RptPLUGroup;
	PLUIF		pluIf;
	BL_PLU_INFO		pluInfo;
    USHORT    uchPLUGroupNumber;
	TCHAR     auchTempPLUNo[16];   // PLU number should be at least as big as OP_PLU_LEN
    LABELANALYSIS   PluLabel;

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

	memset(&RptPLUGroup, 0, sizeof(RptPLUGroup));

	RptPLUGroup.uchMajorClass = CLASS_RPTPLU;	/* PLU (Group) Report Set */
	RptPLUGroup.uchMinorClass = CLASS_RPTPLUGROUP_PRTGRP;	/* PLU (Group) Report Set */

    RptFeed(RPT_DEFALTFEED);                        /* Line Feed           */

    if(RptPauseCheck() == RPT_ABORTED){
        return(RPT_ABORTED);
    }

    if((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD)){
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT0))) {
            RptPLUGroup.usPrintControl |=  PRT_JOURNAL;
        }
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT1))) {
            RptPLUGroup.usPrintControl |=  PRT_RECEIPT;
        }
    }else{
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT2))) {
            RptPLUGroup.usPrintControl |=  PRT_JOURNAL;
        }
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT3))) {
            RptPLUGroup.usPrintControl |=  PRT_RECEIPT;
        }
    }    
    usRptPrintStatus = RptPLUGroup.usPrintControl;

    for (uchPLUGroupNumber = 1; uchPLUGroupNumber <= (ULONG)PLU_OEPGRP_MAX; uchPLUGroupNumber++){  /* Saratoga */

		RptPLUGroup.uchMinorClass = CLASS_RPTPLUGROUP_PRTGRP;	/* PLU (Group) Report Set */
        RptPLUGroup.usReportCode = uchPLUGroupNumber;             /* PLU Group Number Set */

		memset(&pluInfo, 0, sizeof(pluInfo));

		// get current group's first PLU info.
		sReturn = BlFwIfIntrnlGetGroupPluInformationFirst(0, uchPLUGroupNumber, 0, 0L, 0, &pluInfo);

		// if group does not contain any PLUs, skip displaying
		if ( _tcscmp(pluInfo.auchPluNo, _T("")) == 0 ) // if PLU No. is '', then no PLU exists in group
		{
			continue;
		}

		// Display header for each group
		if (RptCheckReportOnMld()) {
			uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPLUGroup, (USHORT)uchType);  /* Print each Element  */
			RptPLUGroup.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
		}

		PrtPrintItem(NULL, &RptPLUGroup);                    /* Individual Print For One Coupon */

		while (sReturn == 0)
		{
			RptPLUGroup.uchMinorClass = CLASS_RPTPLUGROUP_PRTPLU;	/* PLU (Group) Report Set */
			RptPLUGroup.usReportCode = pluInfo.usGroupNo;
			//RptPLUGroup.usDEPTNumber = pluInfo.usDeptNo;

			// check if current PLU is a repeat of the last displayed PLU
			RflConvertPLU(auchTempPLUNo, pluInfo.auchPluNo);
			if ( _tcscmp(auchTempPLUNo, RptPLUGroup.auchPLUNumber) == 0) // if same PLU is repeated, skip displaying
			{
				sReturn = BlFwIfIntrnlGetGroupPluInformationNext(&pluInfo);
				continue;
			}

			//get PLU Dept. No.
			memset(&pluIf, 0, sizeof(pluIf));
			memset(&PluLabel, 0, sizeof(PluLabel));

			RflConvertPLU(auchTempPLUNo, pluInfo.auchPluNo);

			_tcsncpy(PluLabel.aszScanPlu, auchTempPLUNo, OP_PLU_LEN);

			RflLabelAnalysis(&PluLabel);

			_tcsncpy(pluIf.auchPluNo, /*auchTempPLUNo*/PluLabel.aszLookPlu, NUM_PLU_LEN);        // Set PLU No   
			pluIf.uchPluAdj = 1;                                         // Must put 1 to read 1st PLU  
			
			if ((sTempReturn = CliOpPluIndRead(&pluIf, 0)) != OP_PERFORM) {
				return(OpConvertError(sTempReturn));
			}
			
			RptPLUGroup.usDEPTNumber = pluIf.ParaPlu.ContPlu.usDept; //set Dept No.

			RflConvertPLU(RptPLUGroup.auchPLUNumber, PluLabel.aszLookPlu);

		    _tcsncpy(RptPLUGroup.aszMnemo, pluInfo.uchPluName, PARA_PLU_LEN);              /* Copy Mnemonics */

			if (RptCheckReportOnMld()) {
				uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPLUGroup, (USHORT)uchType);  /* Print each Element  */
				RptPLUGroup.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
			}

			PrtPrintItem(NULL, &RptPLUGroup);                    /* Individual Print For One Coupon */

			/* abort check */
			if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
				return(RPT_ABORTED);
				break;
			}
			if(RptPauseCheck() == RPT_ABORTED){
				return(RPT_ABORTED);
			}


			RptPLUGroup.PLUTotal.lCounter++;//(LONG)TtlCpn.CpnTotal.sCounter;     /* Counter Set */

			sReturn = BlFwIfIntrnlGetGroupPluInformationNext(&pluInfo);
		}

        RptFeed(RPT_DEFALTFEED);                                            /* Line Feed           */
      
        /* send print data to shared module */
        PrtShrPartialSend(RptPLUGroup.usPrintControl);
    }

    return(SUCCESS);
}                                                                        

/*
*===========================================================================
**  Synopsis: SHORT RptCpnHeader(UCHAR uchMinorClass, 
*                                 UCHAR uchType) 
*               
*       Input:  UCHAR uchMinorClass
*               UCHAR uchType
*
*      Output:  nothing
*
**  Return:     SUCCESS      :
*               RPT_ABORTED  :   
*               
**  Description:  
*       Print out header 
*===========================================================================
*/
SHORT RptPLUGroupHeader(UCHAR uchMinorClass, UCHAR uchType) 
{
    UCHAR      uchSpecMnemo,
               uchRptName,
               uchRptType,
               uchTmpType,
               uchACNo;
    //TTLCPN    TtlCpn;
	N_DATE		toDate;
	//SHORT		sReturn;

    /* Check First Operation */

    if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation ?     */
        RptPrtStatusSet(uchMinorClass);                         /* Check and set Print Status */
        memset(&gTotalCpn, 0, sizeof(gTotalCpn));               /* Grand Total Area Clear */

        /* Select Special Mnemonics for Header */

        switch(uchMinorClass) {
        case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */        
            break;
   
        case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
            break;

        case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */
            break;

        case CLASS_TTLSAVPTD :                                  /* Case of PTD RESET */
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
            uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
            break;

        default:            
            return(RPT_ABORTED);
/*            PifAbort(MODULE_RPT_ID, FAULT_INVALID_DATA); */
        }

        /* Set Report name & A/C number */

        uchRptName = RPT_READ_ADR;/*RPT_CPN_ADR*/                         /* Set A/C name  */
        if ((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)){
            uchACNo = AC_CPN_RESET_RPT;                     /* Set A/C #40   */
        }else {
            uchACNo = AC_PLUGROUP_READ_RPT;                      /* Set A/C #31 */
        }

        /* Print Header Name */
                                                        
        uchTmpType = uchType;
        if (uchType == RPT_EOD_ALLREAD || uchType == RPT_PTD_ALLREAD) {
            uchTmpType =  RPT_ALL_READ;
        }

        if ((uchType != RPT_EOD_ALLREAD) && (uchType != RPT_PTD_ALLREAD)) {                                                           
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
        }
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                        RPT_ACT_ADR,                            /* Report Name Address       */
                        uchRptName,                             /* Report Name Address       */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        uchRptType,                             /* Report Name Address       */
                        uchTmpType,                             /* Report Name Address       */
                        uchACNo,                                /* A/C Number                */
                        0,                                      /* Reset Type                */                                       
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */                                       
                                                                    
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                       /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }
        //TtlCpn.uchMajorClass = CLASS_TTLCPN;                  /* Set Major for Ttl */
        //TtlCpn.uchMinorClass = uchMinorClass;                  /* Set Minor for Ttl */
        //TtlCpn.uchCpnNumber = 1;                              /* Set Number for Ttl */
        //sReturn = SerTtlTotalRead(&TtlCpn);                              /* Get Total */
		//if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
		//	return(TtlConvertError(sReturn));                               /* Return Error */
		//}
        //if ((uchMinorClass == CLASS_TTLCURDAY) ||               /* Current Daily report ? */
        //    (uchMinorClass == CLASS_TTLCURPTD)) {               /* Current PTD report ? */
        //    PifGetDateTime(&DateTime);                          /* Get Current Date/Time */
        //    TtlCpn.ToDate.usMin = DateTime.usMin;              /* Set Period To Date    */
        //    TtlCpn.ToDate.usHour = DateTime.usHour;
        //    TtlCpn.ToDate.usMDay = DateTime.usMDay;
        //    TtlCpn.ToDate.usMonth = DateTime.usMonth;
        //}
		TtlGetNdate (&toDate);

		//RptPrtTime(TRN_PFROM_ADR, &TtlCpn.FromDate);           /* Print PERIOD FROM         */
        RptPrtTime(TRN_PTO_ADR, &toDate/*&TtlCpn.ToDate*/);               /* Print PERIOD TO           */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                       
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        uchRptOpeCount |= RPT_HEADER_PRINT;                     /* Header already prints     */
    }   
    return(SUCCESS);
}

   
/*
*===========================================================================
**  Synopsis:  VOID     RptCpnGrandTtlPrt(LTOTAL *pTotal);
*               
*               LTOTAL   *pTotal        : 
*
*      Output:  LTOTAL   *pTotal        :
*
**  Return:     nothing
*
**  Description:  
*       Print out Total 
*
*===========================================================================
*/
VOID RptPLUGroupGrandTtlPrt(LTOTAL *pTotal, UCHAR uchType)
{
    RPTCPN   RptCpn;
    PARATRANSMNEMO Mnemo;

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }
    memset(&RptCpn, 0, sizeof(RptCpn));                            
    RptCpn.uchMajorClass = CLASS_RPTCPN;
    RptCpn.uchMinorClass = CLASS_RPTCPN_TOTAL;
    RptCpn.CpnTotal.lAmount = pTotal->lAmount;                    /* Store GrandTotal */
    RptCpn.CpnTotal.lCounter = pTotal->lCounter;                  /* Store Counter    */
/*    memset(&Mnemo.aszMnemonics, '\0', sizeof(Mnemo.aszMnemonics)); */
    Mnemo.uchMajorClass = CLASS_PARATRANSMNEMO;                     /* Set Major Class  */
    Mnemo.uchAddress = TRN_TTLR_ADR;                                /* Set Address      */
    CliParaRead(&Mnemo);                                            /* Get Mnemonics    */
    _tcsncpy(RptCpn.aszMnemo, Mnemo.aszMnemonics, PARA_CPN_LEN);    /* Copy Mnemonics   */
    RptCpn.usPrintControl = usRptPrintStatus;
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptCpn, (USHORT)uchType);  /* Print each Element  */
        RptCpn.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptCpn);                                   /* Print GrandTotal */

    /* send print data to shared module */
    PrtShrPartialSend(RptCpn.usPrintControl);
}


/*
*=============================================================================
**  Synopsis: SHORT RptCpnReset(UCHAR uchMinorClass, UCHAR uchType) 
*               
*       Input:  uchMinorClass   : CLASS_TTLSAVDAY
*                               : CLASS_TTLSAVPTD
*               uchType         : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful 
*               LDT_ERR_ADR         : (TTL_NOT_ISSUED)
*
**  Description:  
*        Reset Daily or PTD COUPON File.
*        
*===============================================================================
*/
SHORT RptPLUGroupReset(UCHAR uchMinorClass, UCHAR uchType)
{
    UCHAR   uchSpecReset,
            uchSpecMnemo;
    SHORT   sReturn;
/*    CPNIF   CpnIf; */
    TTLCPN  TtlCpn;

    /*----- Reset All Coupon Total -----*/

    TtlCpn.uchMajorClass = CLASS_TTLCPN;
    TtlCpn.uchMinorClass = uchMinorClass;
    if ((sReturn = SerTtlIssuedCheck(&TtlCpn, TTL_NOTRESET)) != TTL_ISSUED) {
        return(TtlConvertError(sReturn));
    }

    if (uchMinorClass == CLASS_TTLSAVDAY){
        TtlCpn.uchMinorClass = CLASS_TTLCURDAY;
    }else{
        TtlCpn.uchMinorClass = CLASS_TTLCURPTD;
    }
    
    if ((sReturn = SerTtlTotalReset(&TtlCpn,0)) != TTL_SUCCESS) {   
        return(TtlConvertError(sReturn));
    }

    if (uchMinorClass == CLASS_TTLSAVDAY) {
        uchSpecReset = SPCO_EODRST_ADR;                         /* Set Daily Reset Counter */
    } else {
        uchSpecReset = SPCO_PTDRST_ADR;                         /* Set PTD Reset Counter */
    }

    MaintIncreSpcCo(uchSpecReset);                        /* Count Up Reset Counter */

    /*----- Only Reset Case -----*/

    if (uchType == RPT_ONLY_RESET) {

        if (uchMinorClass == CLASS_TTLSAVDAY) {
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        } else {
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        }
        /*----- Print Header Name -----*/
                                                        
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_CPN_ADR,                            /* Report Name Address */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        0,                                      /* Not Print */
                        AC_CPN_RESET_RPT,                       /* A/C Number */
                        uchType,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

                                                                   
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCpnLock(VOID) 
*               
*       Input:  nothing
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful 
*               LDT_LOCK_ADR        : (CAS_ALREADY_LOCK)
*               LDT_ERR_ADR         : (CAS_DURING_SIGNIN) 
*
**  Description:  
*       Lock Cashier and Waiter File.
*       If File couldn't Lock, Unlock the File(s).    
*===============================================================================
*/
SHORT RptPLUGroupLock(VOID)
{
    SHORT  sReturn;

    if (((sReturn = SerCasAllLock()) != CAS_PERFORM) &&
        (sReturn != CAS_FILE_NOT_FOUND)) {
        return(CasConvertError(sReturn));                           /* Return Error */
    }

    return(SUCCESS);                                                /* Return SUCCESS */
}

/*
*=============================================================================
**  Synopsis: VOID RptCpnUnLock(VOID) 
*               
*       Input:  nothing
*      Output:  nothing
*
**     Return:  nothing 
*
**  Description:  
*       UnLock Cashier and Waiter File.    
*===============================================================================
*/
VOID RptPLUGroupUnLock(VOID)
{
    /*----- Unlock files -----*/

    SerCasAllUnLock();                      /* Unlock All Cashier File */
}

/* --- End of Source File --- */