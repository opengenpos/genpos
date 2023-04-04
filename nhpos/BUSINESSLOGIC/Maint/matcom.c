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
* Title       : Maintenance Common Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCOM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls common function in supervisor/program mode. 
*
*           The provided function names are as follows:  
*
*               MaintHeaderCtl()        : increase consecutive number & print header
*               MaintMakeHeader()       : print header
*               MaintMakeTrailer()      : print trailer & reset 
*               MaintFeed()             : feed control
*               MaintCopyPLU()          :
*               MaintPLUDEPTLock()      :
*               MaintPLUDEPTUnLock()    :
*               MaintPLUFileUpDate()    :
*               MaintMakeAbortKey()     : print "ABORT"
*               MaintIncreSpcCo()       : increase special counter
*               MaintGetPLUNo()         : get PLU number (cf. UifAC29EnterPLU1() )
*               MaintChkGCNo()          : check GC No. (cf. A/C No. 2,3)
*               MaintClearSetPin()      : clear and reset PIN for Empployee Id (cf. A/C No. 15)
*               MaintDisp()             : Display Control from UI
*               MaintShrTermLock()      : Terminal Lock for Shared Printer
*               MaintShrTermUnLock()    : Terminal UnLock for Shared Printer
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-22-92: J.Ikeda   : initial                                   
* Jun-24-93: J.IKEDA   : Add MaintShrTermLock() & MaintShrTermUnLock()                                   
* Jul-24-95: M.Ozawa   : Add Report on LCD Check Function
* Dec-27-99: hrkato    : Saratoga
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
;=============================================================================
**/
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
#include <rfl.h>
#include <plu.h>
#include <eept.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <cscas.h>
#include <csop.h>
#include <csstbopr.h>
#include <csgcs.h>
#include <regstrct.h>
#include <transact.h>
#include <display.h>
#include <csttl.h>
#include <prt.h>
#include <maint.h>
#include <report.h>
#include <mld.h>
#include <pifmain.h>
#include <item.h>
#include <csstbcas.h>

#include "maintram.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

USHORT      usMaintLockHnd;                 /* Lock Handler Save Area */
USHORT      usMaintACNumber;                /* action code, Saratoga */


/*
*===========================================================================
** Synopsis:    VOID MaintHeaderCtl(UCHAR uchPrgSupNo, UCHAR uchRptAdr1)
*             
*     Input:    uchPrgSupNo
*               uchRptAdr1
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*               
*
** Description: This function sets header item for super/program mode.
*===========================================================================
*/
VOID MaintHeaderCtlExt(USHORT uchPrgSupNo, UCHAR uchRptAdr1, USHORT usPrtControl)
{
    /* check operational times */
    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {
        MaintIncreSpcCo(SPCO_CONSEC_ADR);           /* increase consecutive number */
        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_PARA, uchRptAdr1, 0, 0, 0, 0, uchPrgSupNo, 0, usPrtControl);
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }
}

VOID MaintHeaderCtl(USHORT uchPrgSupNo, UCHAR uchRptAdr1)
{
	MaintHeaderCtlExt (uchPrgSupNo, uchRptAdr1, (PRT_RECEIPT | PRT_JOURNAL));
}

VOID MaintMakePrintLine (TCHAR  *pLineToPrint, USHORT usPrtControl )
{
    MAINTPRINTLINE    MaintPrintLine = {0};

    /* set major class and print control */
    MaintPrintLine.uchMajorClass = CLASS_MAINTPRINTLINE;
    MaintPrintLine.usPrtControl = usPrtControl;
	_tcsncpy (MaintPrintLine.aszPrintLine, pLineToPrint, sizeof(MaintPrintLine.aszPrintLine)/sizeof(MaintPrintLine.aszPrintLine[0]) - 1);
    PrtPrintItem(NULL, &MaintPrintLine);
}

/**fhfh
;=============================================================================
;               
;     Synopsis: VOID MaintMakeHeader( UCHAR uchMinorClass,
;                                     UCHAR uchRptAdr1,
;                                     UCHAR uchRptAdr2,
;                                     UCHAR uchSpeAdr,
;                                     UCHAR uchRptAdr3,
;                                     UCHAR uchReportType,
;                                     USHORT uchPrgSupNo,
;                                     UCHAR uchResetType, 
;                                     USHORT usPrtControl);    
;       input:  UCHAR uchMinorClass
;               UCHAR uchRptAdr1
;               UCHAR uchRptAdr2
;               UCHAR uchSpeAdr
;               UCHAR uchRptAdr3
;               UCHAR uchReportType
;               USHORT uchPrgSupNo
;               UCHAR uchResetType 
;               USHORT usPrtControl
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets maintenance header.
;        
;===============================================================================
fhfh**/

VOID MaintMakeHeader( UCHAR uchMinorClass,
                      UCHAR uchRptAdr1,
                      UCHAR uchRptAdr2,
                      UCHAR uchSpeAdr,
                      UCHAR uchRptAdr3,
                      UCHAR uchReportType,
                      USHORT uchPrgSupNo,
                      UCHAR uchResetType, 
                      USHORT usPrtControl )    
{

    MAINTHEADER    MaintHeader = {0};

    /* set major class and minor class */
    MaintHeader.uchMajorClass = CLASS_MAINTHEADER;
    MaintHeader.uchMinorClass = uchMinorClass;
    
    /* set report name 1*/
    if (uchRptAdr1 != 0) {
        RflGetReportMnem (MaintHeader.aszReportName1, uchRptAdr1);
    } 

    /* set report name 2 */
    if (uchRptAdr2 != 0) {
        RflGetReportMnem (MaintHeader.aszReportName2, uchRptAdr2);
    } 

    /* set special mnemonics */
    if (uchSpeAdr != 0) {
        RflGetSpecMnem (MaintHeader.aszSpeMnemo, uchSpeAdr);
    } 

    /* set report name 3 */
    if (uchRptAdr3 != 0) {
        RflGetReportMnem (MaintHeader.aszReportName3, uchRptAdr3);
    }

    /* set report type, program/supervisor number, reset type and print control */
    MaintHeader.uchReportType = uchReportType;
    MaintHeader.uchPrgSupNo = uchPrgSupNo;
    MaintHeader.uchResetType = uchResetType;
    MaintHeader.usPrtControl = usPrtControl;

    if (uchRptAdr1 == RPT_ACT_ADR) {
        if (RptCheckReportOnMld()) {
            MaintHeader.usPrtControl &= ~PRT_RECEIPT; /* Report Only MLD */
            MldDispItem(&MaintHeader, 0);
        }
    }

    PrtPrintItem(NULL, &MaintHeader);

    /* 1 line feed */
    if (uchRptAdr1 == RPT_ACT_ADR && (uchPrgSupNo == AC_LOAN || uchPrgSupNo == AC_PICKUP)) {
		MaintLoanPickFeed(usPrtControl);
    } else {
        MaintMakeFeedExt(usPrtControl);
    }
}

/*
*===========================================================================
** Synopsis:    VOID MaintMakeTrailer(UCHAR uchMinorClass, USHORT usPrtControl)
*               
*     Input:    uchMinorClass
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*               
*
** Description: This function makes maintenance trailer.
*===========================================================================
*/

VOID MaintMakeTrailerExt(UCHAR uchMinorClass, USHORT usPrtControl)
{
    MAINTTRAILER     MaintTrailer = {0};
    PARASTOREGNO     ParaStoRegNo = {0};
    DATE_TIME        DateTime;

    /* set major class */
    MaintTrailer.uchMajorClass = CLASS_MAINTTRAILER;
    MaintTrailer.uchMinorClass = uchMinorClass;

    /* distinguish program mode and supervisor mode */
    if (uchMinorClass == CLASS_MAINTTRAILER_PRTPRG) {
        /* set program counter mnemonics */
        RflGetSpecMnem (MaintTrailer.aszPrgCountMnemo, SPC_PRGCO_ADR);

        /* read program counter */
        MaintTrailer.usPrgCount = MaintCurrentSpcCo(SPCO_PRG_ADR);
    } else { /* supervisor mode */
        /* set supervisor number */
        MaintTrailer.ulSuperNumber = ulMaintSupNumber;
    }
    
    /* set EOD reset counter mnemonics */
    RflGetSpecMnem(MaintTrailer.aszDayRstMnemo, SPC_DAIRST_ADR);

    /* set PTD reset counter mnemonics */
    RflGetSpecMnem(MaintTrailer.aszPTDRstMnemo, SPC_PTDRST_ADR);

    /* read EOD reset counter */
    MaintTrailer.usDayRstCount = MaintCurrentSpcCo(SPCO_EODRST_ADR);

    /* set PTD reset counter */
    MaintTrailer.usPTDRstCount = MaintCurrentSpcCo(SPCO_PTDRST_ADR);

    /* set store/register number */
    ParaStoRegNo.uchMajorClass = CLASS_PARASTOREGNO;
    CliParaRead(&ParaStoRegNo);  /* call ParaStoRegNoRead() */

    MaintTrailer.usStoreNumber = ParaStoRegNo.usStoreNo;
    MaintTrailer.usRegNumber = ParaStoRegNo.usRegisterNo;

    /* set consecutive number */
    MaintTrailer.usConsCount = MaintCurrentSpcCo(SPCO_CONSEC_ADR);

    /* set time */
    PifGetDateTime(&DateTime);
    MaintTrailer.usMin = DateTime.usMin;
    MaintTrailer.usHour = DateTime.usHour; 
    MaintTrailer.usMDay = DateTime.usMDay;
    MaintTrailer.usMonth = DateTime.usMonth;
    MaintTrailer.usYear = DateTime.usYear;

    /* set print control */
    MaintTrailer.usPrtControl = usPrtControl;   /* set journal bit & receipt bit */

    /* print trailer */
    if (uchMinorClass == CLASS_MAINTTRAILER_PRTSUP) {
        if (RptCheckReportOnMld()) {
            MaintTrailer.usPrtControl &=  ~PRT_RECEIPT;
            MldDispItem(&MaintTrailer, 0);
        }
    }
    PrtPrintItem(NULL,&MaintTrailer);
    /* reset */
    uchMaintOpeCo = MAINT_1STOPERATION;

}

VOID MaintMakeTrailer(UCHAR uchMinorClass)
{
	MaintMakeTrailerExt (uchMinorClass, (PRT_JOURNAL | PRT_RECEIPT));
}


/*
*=============================================================================
**  Synopsis: VOID MaintMakeFeedExt(USHORT usPrtControl)
*               
*       Input:  nothing 
*      Output:  nothing
*
**  Return: nothing 
*
**  Description:  
*===============================================================================
*/
VOID    MaintMakeFeedExt(USHORT usPrtControl)
{
    ITEMOTHER JRFeed = {0};

    JRFeed.uchMajorClass = CLASS_ITEMOTHER;
    JRFeed.uchMinorClass = CLASS_INSIDEFEED;
    JRFeed.lAmount = MAINT_DEFAULT_FEED;
    JRFeed.fsPrintStatus = usPrtControl;

    if (RptCheckReportOnMld()) {
        MldDispItem(&JRFeed,(USHORT)MAINT_DEFAULT_FEED);             
        return; /* Not Print Receipt */
    }
    PrtPrintItem(NULL,&JRFeed);             
}  

VOID    MaintFeed(VOID)
{
	MaintMakeFeedExt(PRT_RECEIPT);
} 

/*
*=============================================================================
**  Synopsis: VOID  MaintLoanPickFeed(VOID)
*               
*       Input:  nothing 
*      Output:  nothing
*
**  Return: nothing 
*
**  Description:                                                    Saratoga
*===============================================================================
*/
VOID  MaintLoanPickFeed(USHORT usPrtControl)
{
    ITEMOTHER   JRFeed = {0};

    JRFeed.uchMajorClass    = CLASS_ITEMOTHER;
    JRFeed.uchMinorClass    = CLASS_LP_INSIDEFEED;
    JRFeed.lAmount          = MAINT_DEFAULT_FEED;
    JRFeed.fsPrintStatus    = usPrtControl;

    if (RptCheckReportOnMld()) {
        MldDispItem(&JRFeed,(USHORT)MAINT_DEFAULT_FEED);
        return;
    }

    PrtPrintItem(NULL, &JRFeed);
}

/*
*===========================================================================
** Synopsis:    VOID  MaintPLUCopy( VOID )
*               
*     Input:    Nothing 
*    Output:    Nothing          
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Copy DEPT Status to PLU Status 
*===========================================================================
*/
SHORT MaintPLUCopy()
{
    DEPTIF  DeptIf = {0};
    PARAPLU MldParaPlu = {0};                                    /* For MLD */
    SHORT   sReturn;
    UCHAR   i;

    /* Get DEPT Status */
    DeptIf.usDeptNo = MaintWork.PLU.usDEPTNumber;                 /* Set DEPT Number */
    if ((sReturn = CliOpDeptIndRead(&DeptIf, usMaintLockHnd)) != OP_PERFORM) {
        return(OpConvertError(sReturn));
    }

    /* Copy DEPT Status to Own PLU Status Save Area */
    memcpy(MaintWork.PLU.auchPLUStatus, DeptIf.ParaDept.auchControlCode, OP_PLU_SHARED_STATUS_LEN);
    MaintWork.PLU.auchPLUStatus[5] = DeptIf.ParaDept.auchControlCode[4];  /* CRT No5 - No 8 see ItemSendKdsSetCRTNo() */

    MldParaPlu = MaintWork.PLU;
    MldParaPlu.usACNumber   = AC_PLU_MAINT;
    for(i = PLU_FIELD3_ADR ; i <= PLU_FIELD10_ADR ; i++){
		MldParaPlu.uchFieldAddr = i;
		MldDispItem(&MldParaPlu,0);
    }
    MldParaPlu.uchFieldAddr = PLU_FIELD18_ADR;  /* R3.1 */
    MldDispItem(&MldParaPlu,0);
    MldParaPlu.uchFieldAddr = PLU_FIELD19_ADR;  /* Saratoga */
    MldDispItem(&MldParaPlu,0);
    return(SUCCESS);

}


/*
*===========================================================================
** Synopsis:    SHORT  MaintPLUDEPTLock( VOID )
*               
*     Input:    Nothing 
*    Output:    Nothing          
*
** Return:      SUCCESS         : Successful
*               LDT_LOCK_ADR    : During Lock
*
** Description: Lock PLU/DEPT File and Save This Lock Handle to Own Save Area
*===========================================================================
*/
SHORT MaintPLUDEPTLock()
{
    SHORT   sError;
    
    if ((sError = SerOpLock()) != OP_LOCK) {                /* Success */
        usMaintLockHnd = sError;                            /* Save Lock Handle */
        return(SUCCESS);
    }
    return(LDT_LOCK_ADR);
}

/*
*===========================================================================
** Synopsis:    VOID  MaintPLUDEPTUnLock( VOID )
*               
*     Input:    Nothing 
*    Output:    Nothing          
*
** Return:      Nothing
*
** Description: UnLock PLU/DEPT File 
*===========================================================================
*/
VOID MaintPLUDEPTUnLock()
{
    SerOpUnLock();
    usMaintLockHnd = 0;
}

/*
*===========================================================================
** Synopsis:    VOID  MaintPLUFileUpDate( VOID )
*               
*     Input:    Nothing 
*    Output:    Nothing          
*
** Return:      Nothing
*
** Description: Update PLU File Index 
*===========================================================================
*/
VOID MaintPLUFileUpDate()
{
    SerOpPluFileUpdate(usMaintLockHnd);                 /* Ignore Error Case */
}


/*
*===========================================================================
** Synopsis:    VOID MaintMakeAbortKey( VOID )
;       input:  Nothing
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function makes abort mnemonic.
*===========================================================================
*/
VOID  MaintMakeAbortKey( VOID )
{            
    MAINTTRANS     MaintTrans = {0};

    /* intialize */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    RflGetTranMnem (MaintTrans.aszTransMnemo, TRN_ABORT_ADR);
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );  
    if (RptCheckReportOnMld()) {
        MldDispItem(&MaintTrans, 1);    /* abort status for scroll control */
        MaintTrans.usPrtControl &= ~PRT_RECEIPT;  
    }
    PrtPrintItem(NULL, &MaintTrans);
}

/*
*===========================================================================
** Synopsis:    VOID MaintIncreSpcCo(UCHAR uchCoAddress)
;       input:  uchCoAddress
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function special counter.
*===========================================================================
*/
USHORT MaintIncreSpcCo(UCHAR uchCoAddress)
{
    PARASPCCO ParaSpcCo = {0};

    ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
    ParaSpcCo.uchAddress = uchCoAddress;                    
    CliParaRead(&ParaSpcCo);                                     /* call ParaSpcCoRead() */

    /* distinguish address */
    switch(uchCoAddress) {
    case SPCO_CONSEC_ADR:
        /* check range */
        if (ParaSpcCo.usCounter >= MAX_SPCO_CONSEC) {
            ParaSpcCo.usCounter = 1;
        } else {
            ParaSpcCo.usCounter++;
        }   
        break;

    case SPCO_PRG_ADR:
        /* check range */
        if (ParaSpcCo.usCounter >= MAX_SPCO_PRG) {
            ParaSpcCo.usCounter = 1;
        } else {
            ParaSpcCo.usCounter++;
        }
        break;
    
    case SPCO_EODRST_ADR:
        /* check range */
        if (ParaSpcCo.usCounter >= MAX_SPCO_EODRST) {
            ParaSpcCo.usCounter = 1;
        } else {
            ParaSpcCo.usCounter++;
        }
        break;

    default:    /* SPCO_PTDRST_ADR */
        /* check range */
        if (ParaSpcCo.usCounter >= MAX_SPCO_PTDRST) {
            ParaSpcCo.usCounter = 1;
        } else {
            ParaSpcCo.usCounter++;
        }
        break;
    }
    CliParaWrite(&ParaSpcCo);

    CliParaSaveFarData();          // MaintIncreSpcCo() - Update the memory resident database persistent store
	return ParaSpcCo.usCounter;
}

USHORT MaintCurrentSpcCo(UCHAR uchCoAddress)
{
    PARASPCCO ParaSpcCo = {0};

    ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
    ParaSpcCo.uchAddress = uchCoAddress;                    
    CliParaRead(&ParaSpcCo);                                     /* call ParaSpcCoRead() */

	return ParaSpcCo.usCounter;
}

USHORT MaintResetSpcCo(UCHAR uchCoAddress)
{
	USHORT    usCounter;
    PARASPCCO ParaSpcCo = {0};

    ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
    ParaSpcCo.uchAddress = uchCoAddress;                    
    CliParaRead(&ParaSpcCo);                                     /* call ParaSpcCoRead() */
	usCounter = ParaSpcCo.usCounter;
    ParaSpcCo.usCounter = 0;
    CliParaWrite(&ParaSpcCo);

    CliParaSaveFarData();          // MaintResetSpcCo() - Update the memory resident database persistent store
	return ParaSpcCo.usCounter;
}
/*
*===========================================================================
** Synopsis:    VOID MaintGetPLUNo(UCHAR uchMinorFSCData, UCHAR *pauchPLUNumber)
;       input:  uchMinorFSCData
;      output:  &PLUNumber
;      Return:  LDT_KEYOVER_ADR  : wrong data
;
; Descruption:  This function gets PLU number.
*===========================================================================
*/
SHORT MaintGetPLUNo(UCHAR uchMinorFSCData, TCHAR *pauchPLUNumber, UCHAR *puchModStat)
{
    PARAPLUNOMENU ParaPLUNoMenu = {0};

    /* check minor FSC data */
    if (uchMinorFSCData < FSC_MIN_ADR || FSC_MICRO_ADR < uchMinorFSCData) {
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }

    ParaPLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU; 
    ParaPLUNoMenu.uchMinorFSCData = uchMinorFSCData;
    ParaPLUNoMenu.uchMenuPageNumber = uchMaintMenuPage;
    CliParaRead(&(ParaPLUNoMenu));                                  /* call ParaPLUNoMenuRead() */

    _tcsncpy(pauchPLUNumber, ParaPLUNoMenu.PluNo.aszPLUNumber, PLU_MAX_DIGIT);
    *puchModStat = ParaPLUNoMenu.PluNo.uchModStat;
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   MaintGetDEPTNo(UCHAR uchMinorFSCData, USHORT *pusDeptNumber)
;       input:  uchMinorFSCData
;      output:  &DEPTNumber
;      Return:  LDT_KEYOVER_ADR  : wrong data
;
; Descruption:  This function gets DEPT number.
*===========================================================================
*/
SHORT MaintGetDEPTNo(UCHAR uchMinorFSCData, USHORT *pusDeptNumber)    
{
    PARADEPTNOMENU ParaDeptNoMenu = {0};

    /* check minor FSC data */
    if (uchMinorFSCData < FSC_MIN_ADR || FSC_MICRO_ADR < uchMinorFSCData) {
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }

    ParaDeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU; 
    ParaDeptNoMenu.uchMinorFSCData = uchMinorFSCData;
    ParaDeptNoMenu.uchMenuPageNumber = uchMaintMenuPage;
    CliParaRead(&(ParaDeptNoMenu));                                  /* call ParaDEPTNoMenuRead() */

    /* DEPT number range check */
    if (ParaDeptNoMenu.usDeptNumber < 1 || MAX_DEPT_NO < ParaDeptNoMenu.usDeptNumber) {
        return(LDT_KEYOVER_ADR);                                     /* wrong data */
    }
    *pusDeptNumber = ParaDeptNoMenu.usDeptNumber;
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    MaintGetFSC(UCHAR *puchMajorFSC, UCHAR *puchMinorFSC)
;       input:  
;      output:  
;      Return:  
;
; Descruption:  This function gets FSC data.                Saratoga
*===========================================================================
*/
VOID    MaintGetFSC(UCHAR *puchMajorFSC, UCHAR *puchMinorFSC)
{
    *puchMajorFSC = MaintWork.FSC.uchMajorFSCData;
    *puchMinorFSC = MaintWork.FSC.uchMinorFSCData;
}

/*
*===========================================================================
** Synopsis:    SHORT MaintChkGCNo( ULONG ulGCNumber, USHORT usRtnGCNumber )
*               
*     Input:    ulGCNumber       : Guest Check Number to Checked
*    Output:    Nothing 
*     InOut:    *usRtnGCNumber   : Pointer to Guest Check Number W/o CDV Saved Area 
*
** Return:      SUCCESS          : Successful
*               LDT_KEYOVER_ADR  : CDV Error or Out of Range
*
** Description: Check GC No. with CDV If CDV Check Needs Case and Return GC No W/o CDV. 
*===========================================================================
*/
SHORT MaintChkGCNo(ULONG ulGCNumber, USHORT *usRtnGCNumber)
{
    /* Check CDV System */
    if (RflGetSystemType() != FLEX_STORE_RECALL)
	{
		RflGcfNo  gcfCd = RflPerformGcfCd (ulGCNumber);
		if (gcfCd.sRetStatus != RFL_SUCCESS) {
			return gcfCd.sRetStatus;
		}
		*usRtnGCNumber = gcfCd.usGcNumber;
        return (SUCCESS);
	}

    /* R3.3 */
    if (ulGCNumber < 1 || TRANSOPEN_GCNO(GCF_MAX_CHK_NUMBER) < ulGCNumber ) {
		/* Out of Range check for CLASS_CASINTOPEN modified guest check number as well */
        return(LDT_KEYOVER_ADR);
    }

    *usRtnGCNumber = ( USHORT)ulGCNumber;
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintClearSetPin( ULONG ulEmployeeId, ULONG ulEmployeePin )
*               
*     Input:    ulEmployeeId    : Employee Id of Operator Record to clear the PIN
*     Input:    ULEmployeePin   : new PIN, use 0 to clear the existing PIN
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      SUCCESS          : Successful
*               LDT_KEYOVER_ADR  : Unmatch Error
*
** Description: Check Supervisor Number. 
*===========================================================================
*/
SHORT MaintClearSetPin (ULONG ulEmployeeId, ULONG ulEmployeePin)
{
	SHORT  sRetStatus = SUCCESS;
	CASIF  casRec = {0};

	casRec.ulCashierNo = ulEmployeeId;
	if(SerCasIndRead(&casRec) == CAS_PERFORM){
		casRec.ulCashierSecret = ulEmployeePin;
		sRetStatus = SerCasAssign(&casRec);
	} else {
		sRetStatus = CAS_NOT_IN_FILE;
	}

    return(sRetStatus);
}
/*
*=============================================================================
**  Synopsis:    VOID MaintDisp(USHORT usPGACNumber, UCHAR uchDispType,
*                               UCHAR uchPageNumber, UCHAR uchPTDType,
*                               UCHAR uchRptType, UCHAR uchResetType,
*                               ULONG ulAmount,UCHAR uchLeadThruAddr) 
*
*       Input:  usPGACNumber    : Program/AC Number
*               uchDispType     : Display Type
*               uchPageNumber   : Page Number
*               uchPTDType      : PTD Type
*               uchRptType      : Report Type
*               uchResetType    : Reset Type
*               ulAmount        : Input Data 
*               uchLeadThruAddr : Lead Through Address 
*      Output:  nothing
*
**  Return:     nothing 
*               
**  Description: Display Common Routine 
*===============================================================================
*/
VOID MaintDisp(USHORT usPGACNo,           /* A/C Number such as AC_EOD_RPT */
               UCHAR  uchDispType,        /* Display type such as CLASS_MAINTDSPCTL_aaaaa */
               UCHAR  uchPageNo,          /* Page Number */
               UCHAR  uchPTD,             /* PTD Type */
               UCHAR  uchRpt,             /* Report Type */
               UCHAR  uchReset,           /* Reset Type such as RPT_TYPE_ONLY_PRT */
               ULONG  ulAmount,           /* Amount Data if showing ammount or default to 0 */
               UCHAR  uchLeadThruAddr)    /* Lead Through Address such as LDT_RESET_ADR */
{
    MAINTDSPCTL     MaintDspCtl = {0};

    uchPageNo = uchMaintMenuPage;
    MaintDspCtl.uchMinorClass = uchDispType;                /* Set Minor Class */
    MaintDspCtl.uchMajorClass = CLASS_MAINTDSPCTL;          /* Set Major Calss */
    MaintDspCtl.usPGACNumber = usPGACNo;                    /* Set AC Number */
    MaintDspCtl.usDescriptor = 0;                           /* Set Descriptor */
    MaintDspCtl.uchPageNumber = uchPageNo;                  /* Set Page Number */
    MaintDspCtl.uchPTDType = uchPTD;                        /* Set PTD Type */
    MaintDspCtl.uchRptType = uchRpt;                        /* Set Report Type */
    MaintDspCtl.uchResetType = uchReset;                    /* Set Reset Type */
    MaintDspCtl.ulAmount = ulAmount;                        /* Set Amount */
	if (uchLeadThruAddr) {
		RflGetLead (MaintDspCtl.aszMnemonics, uchLeadThruAddr); /* Set Lead Through Mnemonics */
	}
        
    DispWrite(&MaintDspCtl);                                /* Display */

}
    
/*
*===========================================================================
** Synopsis:    VOID MaintInit( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      Nothing
*
** Description: Clear Maintenance Module Work. 
*===========================================================================
*/
VOID MaintInit(VOID)
{
    memset(&MaintWork, 0, sizeof(MAINTWORK));    
    uchMaintOpeCo = MAINT_1STOPERATION;
    uchMaintEtkCtl = 0;     /* for ETK assignment/maintenance */

	// Initialize the RFL subsystem number of decimal places and decimal separator.
	RflGetSetDecimalCommaType ();
}


/*
*===========================================================================
** Synopsis:    SHORT MaintShrTermLockSup(USHORT usSupNo)
*               
*     Input:    usSupNo      : super number
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      SUCCESS      : success terminal lock
*               FSC_CANCEL   : stop retry for lock
*               FSC_P5       : stop retry for lock
*
** Description:   Read TERMINAL No. with Shared Printer.
*                 Lock Terminal.
*===========================================================================
*/
SHORT MaintShrTermLockSup(USHORT usSupNo)
{
    UCHAR  uchPrtStatus = 0;

    if (usSupNo == AC_DISCON_MAST) {
        uchPrtStatus |= MAINT_NOT_PRINT;
    } 

    if (usSupNo == AC_FORCE_MAST) {
        uchPrtStatus |= MAINT_NOT_PRINT;
    } 

    if (usSupNo == AC_ALL_DOWN) {   /* for BM setting, saratoga */
        uchPrtStatus |= MAINT_NOT_PRINT;
    } 

    if (usSupNo == AC_COPY_TTL) {
        uchPrtStatus |= MAINT_NOT_PRINT;
    } 
    return( PrtShrTermLock(uchPrtStatus) );
}

/*
*===========================================================================
** Synopsis:    SHORT MaintShrTermLockProg(USHORT usPrgNo)
*               
*     Input:    usPrgNo      : program mode number
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      SUCCESS      : success terminal lock
*               FSC_CANCEL   : stop retry for lock
*               FSC_P5       : stop retry for lock
*
** Description:   Read TERMINAL No. with Shared Printer.
*                 Lock Terminal.
*===========================================================================
*/
SHORT MaintShrTermLockProg(USHORT usPrgNo)
{
    UCHAR uchPrtStatus = 0;

    if (usPrgNo == PG_SHR_PRT) {
        uchPrtStatus |= MAINT_NOT_PRINT;
    }
    if (usPrgNo == PG_MDC) {    /* for BM setting, saratoga */
        uchPrtStatus |= MAINT_NOT_PRINT;
    }
    return( PrtShrTermLock(uchPrtStatus) );
}

/*
*===========================================================================
** Synopsis:    VOID MaintShrTermUnLock( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      Nothing
*
** Description:   Read Terminal No. with Shared Printer.
*                 Unlock Terminal.
*===========================================================================
*/
VOID MaintShrTermUnLock( VOID )
{
    PrtShrTermUnLock();
}    

/*
*===========================================================================
** Synopsis:    VOID MaintShrTermUnLock( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      Nothing
*
** Description:   Read Terminal No. with Shared Printer.
*                 Unlock Terminal.
*===========================================================================
*/ //ESMITH PRTFILE
VOID MaintSetPrintMode( UCHAR uchMode )
{
    PrtSetPrintMode(uchMode);	
}    

/*
*==========================================================================
**  Synopsis:   UCHAR   MaintResetLog(UCHAR uchAction)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     MAINT_SUCCESS           0  : Print Response Text success
*               MAINT_UNSOLI_SUSPEND    8  : Suspend printing unsoli report data
*
*   Description:    Print or suspend PC i/F Reset Log.          V3.3
*==========================================================================
*/
UCHAR   MaintResetLog(UCHAR uchAction)
{
    if ((uchMaintOpeCo & MAINT_ALREADYOPERATE) || (uchRptOpeCount & RPT_HEADER_PRINT)) {      /* in case of inside transaction */
        return(LDT_PROHBT_ADR);
    } else {
		ItemOtherResetLogTrnItem (uchAction, ulMaintSupNumber);
        return(SUCCESS);
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   MaintPowerDownLog(UCHAR uchLog)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Print Power Down Log.                       Saratoga
*==========================================================================
*/
SHORT   MaintPowerDownLog(UCHAR uchLog)
{
    ITEMOTHER       TrnOther = {0};
    DATE_TIME       WorkDate = {0};

    /* --- During Transaction --- */
    /* if ((uchMaintOpeCo & MAINT_ALREADYOPERATE) || (uchRptOpeCount & RPT_HEADER_PRINT)) { */
        TrnOther.uchMajorClass = CLASS_ITEMOTHER;
        TrnOther.uchMinorClass = CLASS_POWERDOWNLOG;
        TrnOther.uchAction     = uchLog;

        PifGetDateTime(&WorkDate);
        TrnOther.uchYear  = (UCHAR)(WorkDate.usYear % 100);
        TrnOther.uchMonth = (UCHAR)WorkDate.usMonth;
        TrnOther.uchDay   = (UCHAR)WorkDate.usMDay;
        TrnOther.uchHour  = (UCHAR)WorkDate.usHour;
        TrnOther.uchMin   = (UCHAR)WorkDate.usMin;

        TrnItem(&TrnOther);
    /* } */

    return(SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   MaintSupASK(UCHAR uchFSC)
*
*   Input:      uchFSC :minor class
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     MAINT_SUCCESS
*               Leadthru No  (if error)
*
*   Description:    Print ASK report data for Super             Saratoga
*==========================================================================
*/
SHORT   MaintSupASK(UCHAR uchFSC)
{
    SHORT               sStatus;
    UIFREGTENDER        UifTender = {0};
    ITEMTENDER          Tender = {0};

    /* --- Set Command Code --- */
    if (uchFSC < EXTFSC_ASK_REPORT16) {                   /* check range against EXTFSC_ASK_REPORT16 Inquiry Type */
        return(LDT_SEQERR_ADR);
    }

    TrnICPOpenFile();
    UifTender.uchMinorClass = uchFSC;

    sStatus = ItemTendEPTComm(EEPT_FC2_ASK, &UifTender, &Tender, ulMaintSupNumber);
    if (sStatus == SUCCESS || sStatus == -2) {      /* -2:ITM_REJECT_ASK */
		MAINTSPHEADER   Work;           /* Work area for Trail Print */
		TCHAR           auchDummy[PARA_CASHIER_LEN + 1] = {0};

        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

        /* --- Added for Slip Print Control --- */
        MaintSPHeadTrailSet(&Work, 0, auchDummy);
        MaintSPTrailCtl( &Work );

        sStatus = SUCCESS;
    }
    TrnICPCloseFile();
    UieModeChange(UIE_ENABLE);

    return(sStatus);
}

/***** End of Source *****/