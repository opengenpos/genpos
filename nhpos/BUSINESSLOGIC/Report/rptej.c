/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Electronic Journal Read Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTEJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               RptEJRead()                 : Read EJ File
*               RptEJReset()                : Reset EJ File
*               RptEJReverse()              : Edit and Print EJ File for Reverse Func.
*               RptEJClusterReset()         : Reset all EJ file in cluster
*                   Rpt_EJEdit()            : Edit and Print EJ Read File
*                   Rpt_EJChgNULL()         : Change from NULL Data to Space Char. Data
*                   Rpt_EJChkShr()          : Check Shared System or Not
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Feb-01-93: 00.00.01 : K.YOU     : initial                                   
* Jul-08-93: 01.00.12 : J.IKEDA   : Adds PrtCheckAbort() in Rpt_EJEdit() 
*          :          :           : Chg MinorClass from CLASS_MAINTTRAILER_NOTSEND
*          :          :           : to CLASS_MAINTTRAILER_SENDSHR in RptEJReset()
* Jul-12-93: 01.00.12 : K.Nakajima: Change "PmgFont()" to "PrtChangeFont()" 
* Jul-20-93: 01.00.12 : K.You     : add Rpt_EJChkShared(). 
* Aug-25-93: 01.00.12 : K.You     : bug fixed S-6.( mod. RptEJRead, RptEJReset, RptEJReverse
*          :          :           : del. Rpt_EJChkShared())
* Sep-07-93: 02.00.00 : K.You     : add Rpt_EJChkShr(), bug fixed E-85.( mod. RptEJRead,
*          :          :           : RptEJReset, RptEJReverse)
* Sep-08-93: 02.00.00 : K.You     : bug fixed E-87.( mod. RptEJReverse() )
* Aug-09-95: 03.00.04 : M.Ozawa   : display EJ report on LCD
* Dec-06-95: 03.01.00 : M.Ozawa   : Reset EJ by EOD
* Apr-23-95: 03.01.04 : M.Ozawa   : bug fixed of Rpt_EJConfirmation()
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

#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csop.h>
#include <csttl.h>
#include <report.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <ej.h>
#include <csstbej.h>
#include <pmg.h>
#include <csstbstb.h>
#include <csstbpar.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>
#include "rfl.h"

#include "rptcom.h"
SHORT Rpt_EJEdit_Print_SingleColumn( VOID );


/*
*=============================================================================
**  Synopsis: SHORT RptEJRead( VOID ) 
*               
*       Input:  Nothing      
*
*      Output:  nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Read Electronic Journal File.  
*===============================================================================
*/
SHORT RptEJRead( VOID )
{
    UCHAR   uchUAddr;       /* Terminal Unique Address, R3.1 */

    /* Check System */
	//SR 601, we check to see if the user has chosen to display 
	//on the terminal rather than print the information.  If 
	//they choose to show it on the display, we dont need to 
	//check for printers JHHJ
	if(!RptCheckReportOnMld())
	{
		if (PrtGetStatus() != PRT_THERMAL_PRINTER) {        /* Not Thermal System */
	        return(LDT_PROHBT_ADR);
		}

		if (CliEJGetStatus() == EJ_THERMAL_NOT_EXIST) {     /* Not Thermal System */
			if (!Rpt_EJChkShr()) {              /* Not Shared Printer System */
				return(LDT_PROHBT_ADR);
			}
		}
	}
    usRptPrintStatus = 0;                                                        /* Clear print status  */

    /* Control Header */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecutive No. */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_EJ_ADR,                             /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_READ_ADR,                           /* Report Name Address */
                    0,                                      /* Read Type */
                    AC_EJ_READ_RPT,                         /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */
    
    /* Edit and Print EJ Read File */
    uchUAddr = CliReadUAddr();
    RptRegFinPrtTermNumber((USHORT)uchUAddr, RPT_IND_READ);   /* print out terminal no. */
    Rpt_EJEdit(RPT_EJ_READ, uchUAddr);
    
    /* Control Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptEJReset( VOID ) 
*               
*       Input:  Nothing
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Reset Electronic Journal File.  
*===============================================================================
*/
SHORT RptEJReset( VOID )
{
    UCHAR   uchUAddr;       /* Terminal Unique Address, R3.1 */

    /* Display Leadthrough */
    MaintDisp(AC_EJ_RESET_RPT, 0, 0, 0, 0, 0, 0L, 0);

    /* Check System */
    if (PrtGetStatus() != PRT_THERMAL_PRINTER) {        /* Not Thermal System */
        return(LDT_PROHBT_ADR);
    }

    if (CliEJGetStatus() == EJ_THERMAL_NOT_EXIST) {     /* Not Thermal System */
        if (!Rpt_EJChkShr()) {              /* Not Shared Printer System */
            return(LDT_PROHBT_ADR);
        }
    }
    usRptPrintStatus = 0;                                                        /* Clear print status  */

    /* Control Header */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecutive No. */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_EJ_ADR,                             /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_RESET_ADR,                          /* Report Name Address */
                    0,                                      /* Read Type */
                    AC_EJ_RESET_RPT,                        /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */
    
    /* Edit and Print EJ Read File */
    uchUAddr = CliReadUAddr();

    RptRegFinPrtTermNumber((USHORT)uchUAddr, RPT_IND_RESET);   /* print out terminal no. */
    if (Rpt_EJEdit(RPT_EJ_RESET, uchUAddr) == PRT_ABORTED) {
        return(RPT_ABORTED);
    }
    
    /* Reset EJ File */
    CliEJClear(uchUAddr);

    MaintIncreSpcCo(SPCO_EODRST_ADR);                       /* Count Up Daily Reset Co. */

    /* Control Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);

}

/*
*=============================================================================
**  Synopsis: SHORT RptEJClusterReset( VOID ) 
*               
*       Input:  Nothing
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Reset Electronic Journal File by EOD.  
*===============================================================================
*/
SHORT RptEJClusterReset( VOID )
{
    USHORT usTermNo, usNoOfTerm;
    SHORT  sReturn;

    /* Check System */
    if (PrtGetStatus() != PRT_THERMAL_PRINTER) {        /* Not Thermal System */
        return(LDT_PROHBT_ADR);
    }

    if (CliEJGetStatus() == EJ_THERMAL_NOT_EXIST) {     /* Not Thermal System */
        if (!Rpt_EJChkShr()) {              /* Not Shared Printer System */
            return(LDT_PROHBT_ADR);
        }
    }

    /* Edit and Print EJ Read File */
    usNoOfTerm = RflGetNoTermsInCluster();

    for (usTermNo = 1; usTermNo <= usNoOfTerm; usTermNo++) {
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        usRptPrintStatus = 0;                                                        /* Clear print status  */

        /* Display Terget Terminal No. */
        MaintDisp(AC_EJ_RESET_RPT, 0, 0, 0, 0, 0, (LONG)usTermNo, 0);

        /* Control Header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_EJ_ADR,                             /* Report Name Address */
                        0,                                      /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        0,                                      /* Read Type */
                        AC_EJ_RESET_RPT,                        /* A/C Number */
                        0,                                      /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */

        RptRegFinPrtTermNumber(usTermNo, RPT_IND_RESET);   /* print out terminal no. */
        sReturn = Rpt_EJEdit(RPT_EJ_CLUSTER_RESET, (UCHAR)usTermNo);

        if (sReturn == SUCCESS) {   /* successed to read */
            /* Reset EJ File */
            for (;;) {
                sReturn = CliEJClear((UCHAR)usTermNo);
                if (sReturn) {
                    sReturn = Rpt_EJConfirmation(sReturn, LDT_EJRESET_ADR);
                    if (sReturn) {
                        if (sReturn == LDT_EJRESET_ADR) {
                            Rpt_EJPrtError(sReturn);
                        }
                        break;
                    }
                    /* retry */
                } else {
                    break;  /* successed to reset */
                }
            }
        } else if (sReturn == LDT_EJREAD_ADR) {
			char  xBuff[128];
			sprintf (xBuff, "==ERROR: RptEJClusterReset() RPT_EJ_CLUSTER_RESET error usTermNo %d.", usTermNo);
			NHPOS_ASSERT_TEXT((sReturn != LDT_EJREAD_ADR), xBuff);
            Rpt_EJPrtError(sReturn);
        }
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptEJReverse( VOID ) 
*               
*       Input:  Nothing
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Edit and Print Electronic Journal File for Reverse Function.  
*===============================================================================
*/
SHORT RptEJReverse( VOID )
{
	TCHAR           auchRBuff[EJ_COLUMN + 1] = {0};
    SHORT           sError;
    RPTEJ           EJEdit = {0};
    EJ_READ         EJRead;

    /* Check System */
    if (PrtGetStatus() != PRT_THERMAL_PRINTER) {        /* Not Thermal System */
        return(LDT_PROHBT_ADR);
    }

    if (CliEJGetStatus() == EJ_THERMAL_NOT_EXIST) {     /* Not Thermal System */
        if (!Rpt_EJChkShr()) {              /* Not Shared Printer System */
            return(LDT_PROHBT_ADR);
        }
    }

    /* Check Intside Transaction */
    if ((uchMaintOpeCo & MAINT_ALREADYOPERATE) ||
        (uchRptOpeCount & RPT_HEADER_PRINT)) {                      /* in case of Inside Transaction */
        return(LDT_PROHBT_ADR);
    }

    /* check if terminal lock is successful */
    if (MaintShrTermLockSup(0) != SUCCESS) {       /* In case of SYSTEM BUSY */
        return(SUCCESS);                        
    }

    /* Initialize Work */
    memset(&EJEdit, '\0', sizeof(RPTEJ));
    memset(auchRBuff, '\0', sizeof(auchRBuff));

    /* Set Major Class and Print Control */
    EJEdit.uchMajorClass = CLASS_RPTEJ;
    EJEdit.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);

    /* Get First EJ Record from EJ File */
    EJRead.usSize = sizeof(auchRBuff) - sizeof(auchRBuff[0]); //ESMITH EJ PRINT
	//EJRead.usSize = sizeof(auchRBuff) - 1;                                  
    if ((sError = CliEJReverse(&EJRead, auchRBuff, MINOR_EJ_NEW)) < 0) {    /* Error Case */ 
        MaintShrTermUnLock();                           /* Terminal Unlock */
        return(EJConvertError(sError));                                          
    }
                                        
    if (Rpt_EJChkShr()) {           /* Shared Printer System */
        /* Start Shared Printer */
        PrtShrInitReverse();
    }

    /* Change Number of Printer Column from 42 to 56 */
    PrtChangeFont(PRT_FONT_B); 
 
    /* Print First EJ Record */
    EJEdit.uchMinorClass = CLASS_RPTEJ_REVERSE;                                                      
    Rpt_EJChgNULL(auchRBuff, sizeof(auchRBuff) - sizeof(auchRBuff[0])); //ESMITH EJ PRINT /* Check and Change NULL Data */
    _tcsncpy(EJEdit.aszLineData, auchRBuff, EJ_COLUMN);
    PrtPrintItem(NULL, &EJEdit);

    /* Check End Report */
    if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {       /* End Report */
        PrtChangeFont(PRT_FONT_A);              /* Change Number of Printer Column from 56 to 42 */ 
        MaintFeedRec(MAINT_JOUKEYDEF_FEED);
        PrtShrEnd();
        MaintShrTermUnLock();                           /* Terminal Unlock */
        return(SUCCESS);
    }

    /* Print EJ Record from Second Record to Last Record */
    for ( ;; ) {
        /* Initialize Buffer */    
        memset(auchRBuff, '\0', sizeof(auchRBuff));
        memset(EJEdit.aszLineData, '\0', EJ_REPORT_WIDE + 1);

        /* Get EJ records from EJ File */
        EJRead.usSize = sizeof(auchRBuff) - sizeof(auchRBuff[0]); //ESMITH EJ PRINT
        sError = CliEJReverse(&EJRead, auchRBuff, 0);

        /* Check Error Status */
        if (sError < 0) {                       /* Error Case */               
            PrtChangeFont(PRT_FONT_A);          /* Change Number of Printer Column from 56 to 42 */ 
            PrtShrEnd();
            MaintShrTermUnLock();               /* Terminal Unlock */
            return(EJConvertError(sError));
        }

        /* Print EJ Record */
        EJEdit.uchMinorClass = CLASS_RPTEJ_REVERSE;                                      
        Rpt_EJChgNULL(auchRBuff, sizeof(auchRBuff) - sizeof(auchRBuff[0])); //ESMITH EJ PRINT   /* Check and Change NULL Data */
        _tcsncpy(EJEdit.aszLineData, auchRBuff, EJ_COLUMN);
        PrtPrintItem(NULL, &EJEdit);

        /* Check End Report */
        if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {   /* End Report */
            PrtChangeFont(PRT_FONT_A);          /* Change Number of Printer Column from 56 to 42 */ 
            MaintFeedRec(MAINT_JOUKEYDEF_FEED);
            PrtShrEnd();
            MaintShrTermUnLock();                           /* Terminal Unlock */
            return(SUCCESS);                                
        }
    }
}

/*
*=============================================================================
**  Synopsis: SHORT Rpt_EJEdit( SHORT sType ) 
*               
*       Input:  sType   : RPT_EJ_READ   : Read Type
*                       : RPT_EJ_RESET  : Reset Type
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted  
*
**  Description: Edit and Print Electronic Journal File.  
*===============================================================================
*/
SHORT Rpt_EJEdit( SHORT sType, UCHAR uchTermNo )
{
    if (RptCheckReportOnMld()) {
        return (Rpt_EJEdit_Display());    /* display on LCD */
    } else {
        return (Rpt_EJEdit_Print( sType, uchTermNo ));      /* print on receipt */
		//return(Rpt_EJEdit_Print_SingleColumn());   //For future use w/ Zebra RW 220 and other short line length printers
    }
}

/*
*=============================================================================
**  Synopsis: SHORT Rpt_EJEdit( SHORT sType ) 
*               
*       Input:  sType   : RPT_EJ_READ   : Read Type
*                       : RPT_EJ_RESET  : Reset Type
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted  
*
**  Description: Edit and Print Electronic Journal File.  
*===============================================================================
*/
SHORT Rpt_EJEdit_Print( SHORT sType, UCHAR uchUAddr  )
{
    TCHAR       auchRBuff[4][EJ_REPORT_WIDE] = {0};
    SHORT       sError;
    SHORT       i;
	RPTEJ       EJEdit = {0};
    EJ_READ     EJRead, EJReadSave;

    /* Set Major Class and Print Control */
    EJEdit.uchMajorClass = CLASS_RPTEJ;
    EJEdit.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);

    /* Get First EJ Record from EJ File */
    EJRead.usSize = sizeof(auchRBuff[0]);                                   
    for (;;) {
        sError = CliEJRead(uchUAddr, &EJRead, auchRBuff[0], MINOR_EJ_NEW);

        if (sError >= 0) break; /* successed to read */

        sError = Rpt_EJConfirmation(sError, LDT_EJREAD_ADR);
        if (sError) {    /* ignore if no data existed */
            return(sError);
        }
        /* retry */
    }

    /* Print Page No. */
    EJEdit.uchMinorClass = CLASS_RPTEJ_PAGE;
    EJEdit.usPageNo = 1;
    EJEdit.usTtlPageNo = EJRead.usPage;
    PrtPrintItem(NULL, &EJEdit);

    /* Change Number of Printer Column from 42 to 56 */
    PrtChangeFont(PRT_FONT_B); 
 
    /* Print First EJ Record */
    EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;                                
    Rpt_EJChgNULL(auchRBuff[0], sizeof(auchRBuff[0]));          /* Check and Change NULL Data */
    //_tcsncpy(EJEdit.aszLineData, auchRBuff[0], EJ_REPORT_WIDE);
    memcpy(EJEdit.aszLineData, auchRBuff[0], sizeof(auchRBuff[0]));
    PrtPrintItem(NULL, &EJEdit);

    /* Check End Report */
    if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {       /* End Report */
        PrtChangeFont(PRT_FONT_A);              /* Change Number of Printer Column from 56 to 42 */ 
        return(SUCCESS);
    }

    /* Print EJ Record from Second Record to Last Record */
    for ( ;; ) {
        /* Check Abort Key */
        if (sType == RPT_EJ_READ) {
            /* check if abort key */
            if (UieReadAbortKey() == UIE_ENABLE) {      /* Depress Abort key */
                MaintMakeAbortKey();
                PrtChangeFont(PRT_FONT_A);      /* Change Number of Printer Column from 56 to 42 */ 
                return(RPT_ABORTED);
            }
        }

        /* copy previous read offset for retry */
        memcpy(&EJReadSave, &EJRead, sizeof(EJ_READ));

        /* Initialize Buffer */    
        memset(auchRBuff, '\0', sizeof(auchRBuff));

        /* Get EJ records from EJ File */
        EJRead.usSize = sizeof(auchRBuff[0]);

        for (;;) {
            sError = CliEJRead(uchUAddr, &EJRead, auchRBuff[0], 0);

            if (sError >= 0) break; /* successed to read */

            /* Check Error Status */
            sError = Rpt_EJConfirmation(sError, LDT_EJREAD_ADR);
            if (!sError) {  /* retry */
                memcpy(&EJRead, &EJReadSave, sizeof(EJ_READ));  /* retry by previous offset */
                continue;
            } else {
                PrtChangeFont(PRT_FONT_A);      /* Change Number of Printer Column from 56 to 42 */ 
                return (sError);
            }
        }

        /* Print EJ Record */
        for (i = 0; i < sError; i++) {
            EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;                    
            Rpt_EJChgNULL(auchRBuff[i], sizeof(auchRBuff[0]));          /* Check and Change NULL Data */
            memset(EJEdit.aszLineData, '\0', EJ_REPORT_WIDE + 1);
            _tcsncpy(EJEdit.aszLineData, auchRBuff[i], EJ_REPORT_WIDE);
            PrtPrintItem(NULL, &EJEdit);

            /* check if printer trouble occurs */
            if (PrtCheckAbort() == PRT_ABORTED) {
                PrtChangeFont(PRT_FONT_A);      /* Change Number of Printer Column from 56 to 42 */ 
                return(PRT_ABORTED);
            }
        }

        /* Check End Page */
        if ((EJRead.fchFlag & EJFG_PAGE_END) && !(EJRead.fchFlag & EJFG_FINAL_PAGE)) {   /* End Page */

            PrtChangeFont(PRT_FONT_A);          /* Change Number of Printer Column from 56 to 42 */ 

            MaintMakeTrailer(CLASS_MAINTTRAILER_NOTSEND);   /* Control Trailer */       

            /* Print Next Page No. */
            RptRegFinPrtTermNumber((USHORT)uchUAddr, RPT_EJ_CLUSTER_RESET);   /* print out terminal no. */

            EJEdit.uchMinorClass = CLASS_RPTEJ_PAGE;
            EJEdit.usPageNo += 1;
            /* EJEdit.usTtlPageNo = EJRead.usPage; */
            PrtPrintItem(NULL, &EJEdit);
            PrtChangeFont(PRT_FONT_B);          /* Change Number of Printer Column from 42 to 56 */ 
        }

        /* Check End Report */
        if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {   /* End Report */
           PrtChangeFont(PRT_FONT_A);          /* Change Number of Printer Column from 56 to 42 */ 
           return(SUCCESS);
        }
    }
}


/*
*=============================================================================
**  Synopsis: SHORT Rpt_EJEdit_Print_SingleColumn( VOID ) 
*               
*       Input:  Nothing
*
*      Output:  The formatting from the receipt displayed on the LCD
				that is printed in a single column is printed on the 
				actual receipt.  This is in order to fix the AC70 
				Supervisor report that was printing the original two 
				columns thus causing word wrapping and excess lines and 
				spaces.
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted  
*
**  Description: Edit and Display on LCD Electronic Journal File.  
*===============================================================================
*/
SHORT Rpt_EJEdit_Print_SingleColumn( VOID )
{
    TCHAR       auchRBuff[4][EJ_REPORT_WIDE];
    SHORT       sError;
    RPTEJ       EJEdit;
    EJ_READ     EJRead;

    /* Initialize Work */
    memset(&EJEdit, '\0', sizeof(RPTEJ));
    memset(auchRBuff, '\0', sizeof(auchRBuff));

    /* Set Major Class and Print Control */
    EJEdit.uchMajorClass = CLASS_RPTEJ;
    EJEdit.usPrintControl = PRT_JOURNAL;

    /* Get First EJ Record from EJ File */
    EJRead.usSize = sizeof(auchRBuff[0]);                                   
    if ((sError = CliEJRead1Line(&EJRead, auchRBuff[0], MINOR_EJ_NEW)) < 0) {    /* Error Case */ 
        return(EJConvertError(sError));
    }

    /* Print First EJ Record */
    EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;                                
    _tcsncpy(EJEdit.aszLineData, auchRBuff[0], EJ_REPORT_WIDE);
	//uchRptMldAbortStatus = (UCHAR)MldDispItem(&EJEdit, 0);
    PrtPrintItem(NULL, &EJEdit);

    /* Check End Report */
    if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {       /* End Report */
        return(SUCCESS);
    }

    /* Print EJ Record from Second Record to Last Record */
    for ( ;; ) {
        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {      /* Depress Abort key */
            MaintMakeAbortKey();
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
            MaintMakeAbortKey();
            return(RPT_ABORTED);
        }

        /* Initialize Buffer */    
        memset(auchRBuff, '\0', sizeof(auchRBuff));

        /* Get EJ records from EJ File */
        EJRead.usSize = sizeof(auchRBuff);
        sError = CliEJRead1Line(&EJRead, auchRBuff[0], 0);

        /* Check Error Status */
        if (sError < 0) {                   /* Error Case */               
            return(EJConvertError(sError));
        }

        /* Print EJ Record */
        EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;                    
		tcharnset(EJEdit.aszLineData,'\0', EJ_REPORT_WIDE + 1);
        _tcsncpy(EJEdit.aszLineData, auchRBuff[0], EJ_REPORT_WIDE);
        //uchRptMldAbortStatus = (UCHAR)MldDispItem(&EJEdit, 0);
		PrtPrintItem(NULL, &EJEdit);

        if(RptPauseCheck() == RPT_ABORTED){
            MaintMakeAbortKey();
            return(RPT_ABORTED);
        }

        /* Check End Report */
        if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {   /* End Report */
			return(SUCCESS);
        }
    }
}

/*
*=============================================================================
**  Synopsis: SHORT Rpt_EJEdit_Display( SHORT sType ) 
*               
*       Input:  sType   : RPT_EJ_READ   : Read Type
*                       : RPT_EJ_RESET  : Reset Type
*
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted  
*
**  Description: Edit and Display on LCD Electronic Journal File.  
*===============================================================================
*/
SHORT Rpt_EJEdit_Display( VOID )
{
    TCHAR       auchRBuff[4][EJ_REPORT_WIDE];
    SHORT       sError;
    RPTEJ       EJEdit;
    EJ_READ     EJRead;


    /* Initialize Work */
    memset(&EJEdit, '\0', sizeof(RPTEJ));
    memset(auchRBuff, '\0', sizeof(auchRBuff));

    /* Set Major Class and Print Control */
    EJEdit.uchMajorClass = CLASS_RPTEJ;
    EJEdit.usPrintControl = PRT_JOURNAL;

    /* Get First EJ Record from EJ File */
    EJRead.usSize = sizeof(auchRBuff[0]);                                   
    if ((sError = CliEJRead1Line(&EJRead, auchRBuff[0], MINOR_EJ_NEW)) < 0) {    /* Error Case */ 
        return(EJConvertError(sError));
    }

    /* Print First EJ Record */
    EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;                                
    _tcsncpy(EJEdit.aszLineData, auchRBuff[0], EJ_REPORT_WIDE);
    uchRptMldAbortStatus = (UCHAR)MldDispItem(&EJEdit, 0);

    /* Check End Report */
    if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {       /* End Report */
        return(SUCCESS);
    }

    /* Print EJ Record from Second Record to Last Record */
    for ( ;; ) {
        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {      /* Depress Abort key */
            MaintMakeAbortKey();
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
            MaintMakeAbortKey();
            return(RPT_ABORTED);
        }

        /* Initialize Buffer */    
        memset(auchRBuff, '\0', sizeof(auchRBuff));

        /* Get EJ records from EJ File */
        EJRead.usSize = sizeof(auchRBuff);
        sError = CliEJRead1Line(&EJRead, auchRBuff[0], 0);

        /* Check Error Status */
        if (sError < 0) {                   /* Error Case */               
            return(EJConvertError(sError));
        }

        /* Print EJ Record */
        EJEdit.uchMinorClass = CLASS_RPTEJ_LINE;                    
		tcharnset(EJEdit.aszLineData,'\0', EJ_REPORT_WIDE + 1);
        _tcsncpy(EJEdit.aszLineData, auchRBuff[0], EJ_REPORT_WIDE);
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&EJEdit, 0);

        if(RptPauseCheck() == RPT_ABORTED){
            MaintMakeAbortKey();
            return(RPT_ABORTED);
        }

        /* Check End Report */
        if ((EJRead.fchFlag & EJFG_FINAL_PAGE) && (EJRead.fchFlag & EJFG_PAGE_END)) {   /* End Report */
            return(SUCCESS);
        }
    }
}

/*
*=============================================================================
**  Synopsis: VOID Rpt_EJChgNULL( UCHAR *pData, UCHAR uchLen ) 
*               
*       Input:  uchLen  : Length of Buffer
*      Output:  Nothing
*       InOut:  *pData  : Pointer for Buffer to be Changed
*
**  Return: Nothing  
*
**  Description: Change Buffer Data from NULL to Space Char.  
*===============================================================================
*/
VOID Rpt_EJChgNULL( TCHAR *pData, UCHAR uchLen )
{
    UCHAR   i;
    
    for (i = 0; i < uchLen; i++) {
        if (*(pData + i) == 0) {
            *(pData + i) = _T(' ');
        }
    }
}


/*
*=============================================================================
**  Synopsis: UCHAR Rpt_EJChkShr( VOID ) 
*               
*       Input:  Nothing
*
*      Output:  Nothing
*
**  Return: 0               : Not Shared System  
*           Not 0           : Shared System  
*
**  Description: Check Shared System or Not.  
*===============================================================================
*/
UCHAR Rpt_EJChkShr( VOID )
{
    UCHAR           uchReturn;
    PARASHAREDPRT   ParaSharedPrt;

    /* check shared printer system */
    ParaSharedPrt.uchMajorClass = CLASS_PARASHRPRT;
    ParaSharedPrt.uchAddress = CliReadUAddr();
    ParaSharedPrt.uchAddress = ( UCHAR)2 * ParaSharedPrt.uchAddress - ( UCHAR)1;
    CliParaRead(&ParaSharedPrt);

    uchReturn = ParaSharedPrt.uchTermNo;
                                                            
    if ((ParaSharedPrt.uchTermNo > RPT_NO_OF_TERM)) {       /* out of terminal number */
        uchReturn = 0;
    }

    return(uchReturn);
}


/*
*=============================================================================
**  Synopsis: UCHAR Rpt_EJConfirmation(SHORT sError, SHORT sErrorType ) 
*               
*       Input:  SHORT sError    -EJ read/reset error
*               SHORT sErrorType-LDT_EJREAD_ADR/LDT_EJRESET_ADR
*
*      Output:  Nothing
*
**  Return: 0               : SUCCESS
*           Not 0           : Error
*           LDT_EJREAD_ADR    Canceled by User
*           LDT_EJRESET_ADR
*
**  Description: Check Error & Create Dialog for retry/abort.
*===============================================================================
*/
SHORT Rpt_EJConfirmation(SHORT sError, SHORT sErrorType)
{
    USHORT  usRet;

    /* return if error code of EJRead()/EJClear() */
    if (sError == EJ_FILE_NOT_EXIST) {
        return (LDT_PROHBT_ADR);
    } else if (sError == EJ_NOTHING_DATA) {
        return (LDT_NTINFL_ADR);
    }

/*****  not use EJConvertError() because if R/J model, always returns success
    if ((sError == EJ_FILE_NOT_EXIST) || (sError == EJ_NOTHING_DATA)) {
        return(EJConvertError(sError));                                          
    }
*****/

    usRet = UieErrorMsg(sErrorType, UIE_ABORT); /* create dialog */

    /* if abort key entered, skip to read/reset */
    if (usRet == FSC_CANCEL || usRet == FSC_P5) {
        return(sErrorType);
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT Rpt_EJPrtError( SHORT sError ) 
*               
*       Input:  usError         : Error Code    
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Print Error Code for Report.  
*===============================================================================
*/
SHORT Rpt_EJPrtError( SHORT sError )
{
    MAINTERRORCODE  MaintErrCode;

    /* Print error Code */
    MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
    MaintErrCode.sErrorCode = sError;
    MaintErrCode.usPrtControl = PRT_RECEIPT|PRT_JOURNAL;
    PrtPrintItem(NULL, &MaintErrCode);

    return(sError);
}

/****** End of Source ******/

