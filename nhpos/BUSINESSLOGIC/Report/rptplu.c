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
* Title       : PLU Report Module
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application
* Program Name: RPTPLU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*               RptPLUDayRead()             : Read Daily Department Report Module
*               RptPLUPTDRead()             : Read PTD Department Report Module
*                 RptPLURead()              : Read Daily and PTD report
*                   RptPLUEditInd()         : Edit PLU individualy
*                     RptPLUIndAdj()        : Set Adjective Data to print
*                     RptPLUIndOpen()       : Set Open Data to print
*                   RptPLUEditCode()        : Edit PLU by Code
*                     RptPluCode()          : Classify NORMAL, MINUS or HASH
*                     RptCodeNamePrt()      : Print Code Number
*                       RptPLUCodeAdj()     : Set Adjective Data to print
*                       RptPLUCodeOpen()    : Set Open Data to print
*                   RptPLUEditByDEPT()      : Edit PLU by DEPT
*                     RptPLUByDEPT()        : Classify NORMAL, MINUS or HASH
*                     RptDEPTNamePrt()      : Print DEPT name
*                       RptPLUByDEPTAdj()   : Set Adjective Data to print
*                       RptPLUByDEPTOpen()  : Set Open Data to print
*                     RptPLUPrintChk1()     : Check to print each PLU element w/o MDC
*                     RptPLUPrintChk2()     : Check to print each PLU element w/ MDC
*                     RptPLUPrint()         : Print each PLU element
*                     RptPLUHeader()        : Print Header
*                     RptPLUPercent()       : Calculate PLU percent
*                     RptPLUGrandTtlPrt()   : Print Grand Total or Total
*               RptPLUDayReset()            : Reset PLU Daily report
*               RptPLUPTDReset()            : Reset PLU PTD report
*                 RptPLUReset()             : Reset PLU Daily or PTD
*               RptPLULock()                : Lock PLU File
*               RptPLUUnLock()              : UnLock PLU File
*
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev.   :   Name    : Description
* Jul-07-93 : 01.00.12   : J.IKEDA   : Maint. (Scalable is based on COUNTER
*           :            :           : prints '*****%'.)
* Aug-26-93 : 01.00.13   : J.IKEDA   : Add PrtShrPartialSend() 
* Mar-17-94 : 02.05.00   : K.You     : bug fixed S-19 (mod. RptPLUPercent) 
* Jul-26-95 : 03.00.03   : M.Ozawa   : Add Report On LCD Display
* Feb-07-96 : 03.01.00   : T.Nakahata: increase PLU/Dept control code.
* Aug-11-99 : 03.05.00   : M.Teraki  : remove WAITER_MODEL
* Dec-11-99 : 01.00.00   : hrkato    : Saratoga
*==========================================================================
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

#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
#include "log.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "maint.h"
#include "csop.h"
#include "csstbopr.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csttl.h"
#include "csstbttl.h"
#include "csstbstb.h"
#include "report.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "appllog.h"
#include "mld.h"
#include "mldsup.h"
#include "plu.h"
#include "ttl.h"
#include "PluTtlD.h"
#include "rptcom.h"
#include "csstbfcc.h"
#include "CSSTBEPT.H"

static LTOTAL    RptGTotal;            /* Store Grand Total for RPT_ALL_READ */
static UCHAR     uchRptScaleTtl;       /* Store Scalable bit: 3-Type         */
                                       /* 1) Total of PLU or MINUS in Type1  */
                                       /* 2) Total of PLU and MINUS in Type2 */
                                       /* 3) Total of PLU and MINUS in Type3 */

#define RPTNOPRTCO_MAX       30        /* if uchRptNoPrtCo exceeds this, call SerTtlReadWriteChkAndWait(RPT_WAIT_TIMER) and reset */

static UCHAR     uchRptNoPrtCo;        /* PLU line Not Printed Counter, see reset max value RPTNOPRTCO_MAX */
static UCHAR     uchRptStatus;         /* Print Status */

extern  MAINTPLU    MaintPlu;

/*
*=============================================================================
**  Synopsis: SHORT RptPLUDayRead(UCHAR uchMinorClass,
*                                 UCHAR uchType,
*                                 USHORT usNumber)
*                                 UCHAR uchResetType)
*
*       Input:  UCHAR   uchMinorClass  : CLASS_TTLCURDAY
*                                      : CLASS_TTLSAVDAY
*               UCHAR   uchType        : RPT_ALL_READ
*                                      : RPT_DEPT_READ
*                                      : RPT_CODE_READ
*                                      : RPT_PLUIND_READ
*                                      : RPT_EOD_ALLREAD
*               USHORT  usNumber       : PLU or DEPT Number
*               UCHAR   uchResetType   : 0
*                                      : RPT_ONLY_PRT_RESET
*                                      : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:
*
**  Description:
*       Read Daily PLU report.
*===============================================================================
*/
SHORT   RptPLUDayRead(UCHAR uchMinorClass, UCHAR uchType, 
                        USHORT usNumber, UCHAR uchResetType, TCHAR *puchPLUNumber)
{
    /* execute Current Daily Report */
    return(RptPLURead(uchMinorClass, uchType, usNumber, uchResetType, puchPLUNumber));
}

/*
*=============================================================================
**  Synopsis: SHORT RptPLUPTDRead( UCHAR uchMinorClass,
*                                  UCHAR uchType,
*                                  USHORT usNumber,
*                                  UCHAR uchResetType)
*
*       Input:  UCHAR   uchMinorClass  : CLASS_TTLCURPTD
*                                      : CLASS_TTLSAVPTD
*               UCHAR   uchType        : RPT_ALL_READ
*                                      : RPT_DEPT_READ
*                                      : RPT_CODE_READ
*                                      : RPT_PLUIND_READ
*                                      : RPT_PTD_ALLREAD
*               USHORT  usNumber       : PLU or DEPT Number
*               UCHAR   uchResetType   : 0
*                                      : RPT_ONLY_PRT_RESET
*                                      : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:
*
**  Description:
*       Read PTD PLU report.
*===============================================================================
*/
SHORT   RptPLUPTDRead(UCHAR uchMinorClass, UCHAR uchType,
                      USHORT usNumber, UCHAR uchResetType, TCHAR *puchPLUNumber)
{
    /* execute Current PTD Report */
    return(RptPLURead(uchMinorClass, uchType, usNumber, uchResetType, puchPLUNumber));
}

/*
*=============================================================================
**  Synopsis: SHORT RptPLURead(UCHAR uchMinorClass, UCHAR uchType, USHORT usNumber, uchResetType)
*
*       Input:  UCHAR    uchMinorClass   : CLASS_TTLCURDAY
*                                        : CLASS_TTLSAVDAY
*                                        : CLASS_TTLCURPTD
*                                        : CLASS_TTLSAVPTD
*               USHORT   uchType         : RPT_ALL_READ
*                                        : RPT_DEPT_READ
*                                        : RPT_CODE_READ
*                                        : RPT_PLUIND_READ
*                                        : RPT_FIN_READ
*                                        : RPT_EOD_ALLREAD
*                                        : RPT_PTD_ALLREAD
*               USHORT   usNumber        : PLU or DEPT Number
*               UCHAR   uchResetType   : 0
*                                      : RPT_ONLY_PRT_RESET
*                                      : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:
*
**  Description:
*       Read Daily or PTD and Current or Saved.
*       Get current Time if data is Current Read.
*       Set certain data for Header.
*       Print Header and Clear Grand total if it is first operation.
*       Print Trailer.
*===============================================================================
*/
SHORT   RptPLURead(UCHAR uchMinorClass, UCHAR uchType, 
                   USHORT usNumber, UCHAR uchResetType, TCHAR *puchPLUNumber)
{
    SHORT           sReturn = SUCCESS;

    /*----- Edit Total Report -----*/
    uchRptNoPrtCo = 0;                                  /* Clear Work Counter */
    uchRptStatus = 0;                                   /* Print Status */
    switch (uchType) {
    case RPT_ALL_READ:                                  /* All PLU by PLU# */
    case RPT_EOD_ALLREAD:                               /* Saratoga */
    case RPT_PTD_ALLREAD:                               /* Saratoga */
        sReturn = RptPLUEditAll(uchMinorClass, uchType, uchResetType);
        break;

    case RPT_PLUIND_READ:                                       /* PLU Individual Read */
		/* Analyze PLU Label & Convert (ParaPLU->aucPLUNumber : PLU number) */
		{
			SHORT         sReturn;
			PARAPLU       ParaPLU = {0};

			_tcscpy(ParaPLU.auchPLUNumber, puchPLUNumber);
			if (puchPLUNumber[0] >= _T('A')) {  /* with scan code */
				ParaPLU.uchStatus |= MAINT_SCANNER_INPUT;
			}
			sReturn = MaintPluLabelAnalysis(&ParaPLU, MaintPlu.uchEVersion);
			MaintPlu.uchEVersion = 0;
			if (sReturn != SUCCESS) {
				return(sReturn);
			}
			if (_tcsncmp(ParaPLU.auchPLUNumber, MLD_ZERO_PLU, PLU_MAX_DIGIT) == 0) {
				return(LDT_KEYOVER_ADR);
			}
			_tcsncpy(puchPLUNumber, ParaPLU.auchPLUNumber, PLU_MAX_DIGIT);
		}
        sReturn = RptPLUEditInd(uchMinorClass, uchType, puchPLUNumber);
        break;

    case RPT_CODE_READ:                                         /* PLU Read by Code */
        sReturn = RptPLUEditCode(uchMinorClass, uchType, (UCHAR)usNumber);
        break;

    case RPT_FIN_READ:                                          /* Final read */
        RptFeed(RPT_DEFALTFEED);                                /* Line Feed        */
                                                                /* Print All Total */
        RptPLUGrandTtlPrt(TRN_TTLR_ADR, &RptGTotal, 0, uchRptWgtFlag, uchType);
		// with finalizing a report we do not want to do a reset and it always succeeds.
		uchMinorClass = CLASS_TTLIGNORE;    // ensure uchMinorClass is set to skip call to SerTtlIssuedReset() below.
		sReturn = SUCCESS;
        break;

    default:                                                    /* PLU read by Dept */
        sReturn = RptPLUEditByDEPT(uchMinorClass, uchType, usNumber, uchResetType);

    }    /*----- End of Switch Loop -----*/

	if (sReturn == RPT_ABORTED) {
        RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
        MaintMakeAbortKey();                                    /* Print ABORTED   */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
        return(sReturn);
    }
                                                                /* After All read */
    if (sReturn == SUCCESS && (uchMinorClass == CLASS_TTLSAVDAY || uchMinorClass == CLASS_TTLSAVPTD)) {
		TTLPLU    TtlPlu = {0};

        TtlPlu.uchMajorClass = CLASS_TTLPLU;                    /* Set Major for Issued */
        TtlPlu.uchMinorClass = uchMinorClass;                   /* Set Minor for Issued */
                                                                /* Issued set */
        if (((sReturn = SerTtlIssuedReset(&TtlPlu, TTL_NOTRESET)) != TTL_SUCCESS) && (sReturn != TTL_FILE_HANDLE_ERR)) {
            sReturn = TtlConvertError(sReturn);
            RptPrtError(sReturn);
        }
    }

    if (uchRptOpeCount & RPT_HEADER_PRINT) {                    /* Already print header out? */
        if((uchType != RPT_EOD_ALLREAD) &&                      /* Not EOD ? */
           (uchType != RPT_PTD_ALLREAD) &&                      /* Not PTD ? */
           (uchType != RPT_PLUIND_READ) &&                          /* Not IND ? */
           (uchType != RPT_DEPT_READ) &&                        /* Not DEPT IND ? */
           (uchType != RPT_CODE_READ)) {                        /* Not CODE ? */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Trailer    */
        }
        if ((uchType == RPT_EOD_ALLREAD) ||
            (uchType == RPT_PTD_ALLREAD)) {
            RptFeed(RPT_DEFALTFEED);                            /* Feed */
        }
    }
    return(sReturn);
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUEditInd(UCHAR uchMinorClass,
*                                   UCHAR uchType,
*                                   UCHAR *puchPLUNumber);
* 
*       Input:  UCHAR   uchMinorClass   : CLASS_TTLCURDAY
*                                       : CLASS_TTLSAVDAY
*                                       : CLASS_TTLCURPTD
*                                       : CLASS_TTLSAVPTD
*               UCHAR   uchType         : RPT_PLUIND_READ
*               UCHAR   *puchPLUNumber  : PLU Number
*
*      Output:  nothing
*
**     Return:  SUCCESS        :
*               RPT_ABORTED    :
*               LDT_PROHBT_ADR : (OP_FILE_NOT_FOUND)
*               LDT_NTINFL_ADR : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR   : (OP_LOCK)
*               LDT_ERR_ADR    : (TTL_FAIL)
*               LDT_LOCK_ADR   : (TTL_LOCKED)  
*
**  Description:  
*      Get certain Data for PLU individual report.
*      Classify NORMAL, MINUS or HASH and also OPEN-NONADJECTIVE or ONEADJECTIVE.
*      Add Each Total-lAmount and lCounter.
*      RptGTotal is cleared in the function RptPLURead if only First Operation.
*===========================================================================
*/
SHORT   RptPLUEditInd(UCHAR uchMinorClass, UCHAR uchType, TCHAR *puchPLUNumber)
{
    UCHAR      uchPLUType;
    SHORT      sReturn = SUCCESS;
    TTLPLU     TtlPlu = {0};
    PLUIF      PluIf = {0};
	DEPTIF     DeptIf = {0};

    _tcsncpy(PluIf.auchPluNo, puchPLUNumber, OP_PLU_LEN);        /* Set PLU No   */
    PluIf.uchPluAdj = 1;                                                    /* Must put 1 to read 1st PLU */ 
    if ((sReturn = CliOpPluIndRead(&PluIf, 0)) != OP_PERFORM) {
        return(OpConvertError(sReturn));
    }
    TtlPlu.uchMajorClass = CLASS_TTLPLU;                                    /* Set Major for Ttl */
    TtlPlu.uchMinorClass = uchMinorClass;                                   /* Set Minor for Ttl */
    _tcsncpy(TtlPlu.auchPLUNumber, puchPLUNumber, OP_PLU_LEN);
    TtlPlu.uchAdjectNo = PluIf.uchPluAdj;                                   /* Set Adj for Ttl */
    if ((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) {              /* Get Total */    
        if (sReturn == TTL_NOTINFILE) {           
            sReturn = SUCCESS;
        } else {
            return(TtlConvertError(sReturn));
        }
    }
    RflConvertPLU(puchPLUNumber,TtlPlu.auchPLUNumber);
    RptPLUHeader(uchMinorClass, uchType, puchPLUNumber, 0);                 /* Call Header Fuc. */

    /*----- PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status) -----*/
                                                                            /* Wipe out PLU Type */
    DeptIf.usDeptNo = PluIf.ParaPlu.ContPlu.usDept;                         /* Set DEPT No */

    sReturn = CliOpDeptIndRead(&DeptIf, 0);                                 /* Get DEPT Status */
    if (sReturn == OP_PERFORM) {
        if (PluIf.ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL) {         /* Use Dept Status ? */
            PluIf.ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];    /* Copy Bit Status 0 */
            PluIf.ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];    /* Copy Bit Status 1 */
            PluIf.ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];    /* Copy Bit Status 2 */
            PluIf.ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];    /* Copy Bit Status 3 */
            /* --- increase control code at R3.1 --- */
            PluIf.ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];    /* Copy Bit Status 5 */
        } else {   /* Copy DEPT Parameter file Address 6 BIT2 to PLU Parameter file Address 7 BIT2 */
            DeptIf.ParaDept.auchControlCode[2] &= PLU_HASH;                                 /* Clear 0x04 in ...Code */
            PluIf.ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;                            /* Clear 0xfb in ...Code */
            PluIf.ParaPlu.ContPlu.auchContOther[2] |= DeptIf.ParaDept.auchControlCode[2];   /* Copy Bit Status 2 */
        }
    } else if (sReturn != OP_NOT_IN_FILE) {
        sReturn = OpConvertError(sReturn);
        RptPrtError(sReturn);
        return(sReturn);
	}

    uchPLUType = (UCHAR)(PluIf.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);/* Saratoga */
                                                                            /* Wipe out Dept No */
    if (!(PluIf.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&            /* NORMAL 3-TYPE */
       !(PluIf.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        if (uchPLUType == PLU_ADJ_TYP) {                                    /* ADJECTIVE TYPE */
            sReturn = RptPLUIndAdj(&PluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLADJ1, uchType);
        } else if (uchPLUType == PLU_NON_ADJ_TYP) {                         /* PRESET TYPE */
            RptPLUIndOpen(&PluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLPRSET, uchType);
        } else {                                                            /* OPEN TYPE */
            RptPLUIndOpen(&PluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLOPEN, uchType);
        }
    } else {                                                                /* MINUS 3-TYPE, HASH 3-TYPE */
        if (uchPLUType == PLU_ADJ_TYP) {                                    /* COUPON TYPE 2,  ONE-ADJECTIVE */
            sReturn = RptPLUIndAdj(&PluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, uchType);
        } else if (uchPLUType == PLU_NON_ADJ_TYP) {                         /* COUPON TYPE 1,  NONE-ADJECTIVE */
            RptPLUIndOpen(&PluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, uchType);
        } else {                                                            /* COUPON TYPE 2, CREDIT TYPE */
            RptPLUIndOpen(&PluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, uchType);
        }
    }
    if (sReturn != SUCCESS) {                                               /* Error Check */
        return(sReturn);
    }
    RptGTotal.lAmount += TtlPlu.PLUTotal.lAmount;                           /* Add to Grand Total */
    RptGTotal.lCounter += TtlPlu.PLUTotal.lCounter; 
    return(SUCCESS);  
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUIndAdj( PLUIF  *pPluIf,
*                                   TTLPLU *pTtlPlu, 
*                                   UCHAR uchMinor);
*               
*       Input:  PLUIF      *pPluIf   :
*               TTLPLU     *pTtlPlu  :
*               UCHAR      uchMinor  : CLASS_RPTPLU_RPTTTLADJ1  
*                                    : CLASS_RPTPLU_RPTTTLMINUSADJ1
*                                
*      Output:  nothing
*
**     Return:  SUCCESS 
*               LDT_PROHBT_ADR  : (OP_FILE_NOT_FOUND)
*               LDT_NTINFL_ADR  : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR    : (OP_LOCK) 
*
*
**  Description:  
*       Get all 5 Adjective Data.  Add to Total--lAmount and lCounter.
*       Print each Plu Adjective.
*===========================================================================
*/
SHORT   RptPLUIndAdj(PLUIF *pPluIf, TTLPLU *pTtlPlu, UCHAR uchMinor, UCHAR uchType)
{
    SHORT   sReturn;
    USHORT  usPercent = 0;
    UCHAR   uchStatus = RPT_DECIMAL_0;
    UCHAR   uchScale = 0;
    UCHAR   uchAdjNo, uchNo, uchPluProhibitVal;
	LTOTAL  AdjTtl = {0};
    TCHAR   AdjMnemo[PARA_ADJMNEMO_LEN + 1] = {0};
	TCHAR   SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};
    PLUIF   PluFile = {0};  /* to get unit price of Adj2 to Adj5 */
                                  /* other data is used in pPluIf structure */

    /*----- Classify 4 Adjective groups -----*/
    if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG1) {         /* Group 1 */
        uchNo = ADJ_GP1VAR1_ADR;
    } else if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG2) {  /* Group 2 */
        uchNo = ADJ_GP2VAR1_ADR;
    } else if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG3) {  /* Group 3 */
        uchNo = ADJ_GP3VAR1_ADR;
    } else {                                                                /* Group 4 */
        uchNo = ADJ_GP4VAR1_ADR;
    }

    /*----- Get Adjective Mnemonics -----*/
    RflGetAdj (AdjMnemo, uchNo);                                                    /* Get Adjective Mnemonics */

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&          /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {       /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemonic for scale units LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                                  /* Set Weight Flag in Static area */
        uchScale = 1;                                                       /* Set Scale Flag for usPercent */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&          /* NORMAL TYPE ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, uchScale);                       /* Calculate Percent */
    }
    uchPluProhibitVal = PLU_PROHIBIT_VAL1;
    if (RptPLUPrintChk1(pTtlPlu, pPluIf->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) == 0) {  /* Check Prohibit status */
        RptPLUPrint(uchMinor,                       /* CLASS_RPTPLU_RPTTTLADJ1 */ 
                                                    /* CLASS_RPTPLU_RPTTTLMINUSADJ1 */
                    &pTtlPlu->PLUTotal,             /* TOTAL- lAmount & lCounter */   
                    pPluIf->auchPluNo,              /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    SpeMnemo,                       /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                       /* Ajective Mnemoincs */
                    pPluIf->ParaPlu.auchPrice,      /* PresetPrice */       
                    usPercent,                      /* Percent */             
                    uchStatus,                      /* Decimal point status */
                    uchType);                       /* Report Type */

        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else {                                                /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
        AdjTtl.lAmount = pTtlPlu->PLUTotal.lAmount;             /* Store Total */
        if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
            !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
             (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter;            /* Store Wgt Amt /w Decimal point */
        } else{
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {      /* !Scalable - MDC #89 BIT 2 is 0 */
                AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 100L; /* Adjust Amt to Wgt Amt */
            } else {                                                 /* !Scalable - MDC #89 BIT 2 is 1 */ 
               AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;   /* Adjust Amt to Wgt Amt */
            }
        }
    }

    /*----- loop 2nd to 5th Adjective -----*/
    /*****======================================================================*****/
    /* Using PLU Parameter File from Address3 to Address10 that are set in function */
    /* RptPLUEditInd if PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status). */
    /* In other case, using PLU Parameter File Address6 BIT2                        */
    /* The reason to copy from pPluIf_Dep to PluFile_Dep is to get each Adjective   */
    /* unit Price                                                                   */
    /*****======================================================================*****/

    PluFile = *pPluIf;    /* Copy to other Buffer to save Copied Status */

    for (uchAdjNo = uchNo+(UCHAR)1; uchAdjNo < uchNo+(UCHAR)5; uchAdjNo++) {    
        PluFile.uchPluAdj++;                             /* Next Adjective for other work Buffer */
        uchScale = 0;
        if ((sReturn = CliOpPluIndRead(&PluFile, 0)) != OP_PERFORM) {           /* Get Plu */
            sReturn = OpConvertError(sReturn);                                  /* Convert Error */
            RptPrtError(sReturn);                                               /* Print Error */
            return(sReturn);                                                    /* Return Error */
        }
        /*----- to store new Adjective unit price -----*/
        memcpy(pPluIf->ParaPlu.auchPrice, PluFile.ParaPlu.auchPrice, sizeof(pPluIf->ParaPlu.auchPrice));  

        memset(&pTtlPlu->PLUTotal, 0, sizeof(pTtlPlu->PLUTotal));               /* Clear Total */
        _tcsncpy(pTtlPlu->auchPLUNumber, pPluIf->auchPluNo, OP_PLU_LEN);
        pTtlPlu->uchAdjectNo = PluFile.uchPluAdj;                               /* Set incremented Adj # */
        if (((sReturn = SerTtlTotalRead(pTtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE))   {
            sReturn = TtlConvertError(sReturn);                                 /* Convert Error */
            RptPrtError(sReturn);                                               /* Print Error */
            return(sReturn);                                                    /* Return Error */
        }

        uchPluProhibitVal = (UCHAR)(uchPluProhibitVal << 1);                /* Shift right 1 bit in each time */ 
        if (RptPLUPrintChk1(pTtlPlu, pPluIf->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) != 0) { 
            continue;
        }

        /*----- Get Adjective Mnemonics -----*/
        RflGetAdj (AdjMnemo, uchAdjNo);                                      /* Get Adjective Mnemonics */

        AdjTtl.lAmount += pTtlPlu->PLUTotal.lAmount;                /* Add to All Adject Amount */
        if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
            !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
             (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
            AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter;                  /* Store Wgt Amt /w Decimal point */
            uchScale = 1;
        } else{                                                         
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* !Scalable - MDC #89 BIT 2 is 0 */
                AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 100L;       /* Adjust Amt to Wgt Amt */
            } else {                                                        /* !Scalable - MDC #89 BIT 2 is 1 */
                AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 10L;        /* Adjust Amt to Wgt Amt */
            }
        }

        /*----- Calculate Percent based on MDC -----*/
        if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
           !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
            usPercent = RptPLUPercent(pTtlPlu, uchScale);                   /* Calculate Percent */
        }
        RptPLUPrint(uchMinor,                       /* CLASS_RPTPLU_RPTTTLADJ1 */
                                                    /* CLASS_RPTPLU_RPTTTLADJ2 */
                                                    /* CLASS_RPTPLU_RPTTTLMINUSADJ1 */
                                                    /* CLASS_RPTPLU_RPTTTLMINUSADJ2 */
                    &pTtlPlu->PLUTotal,             /* TOTAL- lAmount & lCounter */   
                    pPluIf->auchPluNo,              /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    SpeMnemo,                       /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                       /* Ajective Mnemoincs */
                    pPluIf->ParaPlu.auchPrice,      /* PresetPrice */       
                    usPercent,                      /* Percent */             
                    uchStatus,                      /* Decimal point status */
                    uchType);                       /* Report Type */

        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {                  /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                     /* Set 2nd Adjective Minor */
        } else if (uchMinor == CLASS_RPTPLU_PRTTTLMINUSADJ1) {      /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;                /* Set 2nd Adjective minus Minor */
        }
    }    /*----- End of For Loop -----*/
    pTtlPlu->PLUTotal = AdjTtl;
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptPLUIndOpen(PLUIF  *pPluIf,
*                                  TTLPLU *pTtlPlu,
*                                  UCHAR uchMinor);
*               
*       Input:  PLUIF       *pPluIf
*               TTLPLU      *pTtlPlu
*               UCHAR       uchMinor
*
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Set Data for individual Open PLU.
*       Print PLU element. 
*===========================================================================
*/
VOID    RptPLUIndOpen(PLUIF  *pPluIf, TTLPLU *pTtlPlu, UCHAR uchMinor, UCHAR uchType)
{
    UCHAR     uchStatus = RPT_DECIMAL_0;
    UCHAR     uchScale = 0;
    USHORT    usPercent = 0;
	WCHAR     aszMnemonics[PARA_SPEMNEMO_LEN + 1] = {0};

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&              /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {           /* Scalable ? */
        RflGetSpecMnem (aszMnemonics, SPC_LB_ADR);                              /* Get scale units mnemonic (LB/Kg) */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                     /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                          /* Decimal - 0.01 */
        } else {                                                                /* MDC #29 BIT1 is 0 */
            uchStatus = RPT_DECIMAL_1;                                           /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                                      /* Set Weight Flag in Static area */
        uchScale = 1;                                                           /* Set Scale Flag for usPercent */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&              /* NORMAL TYPE ? */
       !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, uchScale);
    }

    switch(uchMinor) {
    case CLASS_RPTPLU_PRTTTLOPEN:                   /* CLASS_RPTPLU_PRTTTLOPEN */
        /* break */
    case CLASS_RPTPLU_PRTTTLCREDIT:                 /* CLASS_RPTPLU_PRTTTLCREDIT */
        RptPLUPrint(uchMinor,                        
                    &pTtlPlu->PLUTotal,             /* TOTAL- lAmount & lCounter */             
                    pPluIf->auchPluNo,              /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    aszMnemonics,                   /* Special Mnemonics LB/Kg */             
                    0,                              /* NOT USE (Ajective Mnemoincs) */              
                    0,                              /* NOT USE (PresetPrice) */             
                    usPercent,                      /* Percent */             
                    uchStatus,                      /* Decimal point status */
                    uchType);                       /* Report Type */
        break;

    default:                                        /* case of CLASS_RPTPLU_PRTTTLPRSET */                                       
                                                    /* case of CLASS_RPTPLU_PRTTTLCOUPON */
        RptPLUPrint(uchMinor,                       /* CLASS_RPTPLU_PRTTTLPRSET */ 
                    &pTtlPlu->PLUTotal,             /* TOTAL- lAmount & lCounter */   
                    pPluIf->auchPluNo,              /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    aszMnemonics,                   /* Special Mnemonics LB/Kg */     
                    0,                              /* NOT USE (Ajective Mnemonics) */
                    pPluIf->ParaPlu.auchPrice,      /* PresetPrice */       
                    usPercent,                      /* Percent */             
                    uchStatus,                      /* Decimal point status */
                    uchType);                       /* Report Type */
        break;

    }   /*----- End of Switch Loop -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&          /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {       /* Scalable ? */
        pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter;            /* Store Wgt Amt /w Decimal point */
    } else {
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 100L; /* Adjust Amt to Wgt Amt */
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;  /* Adjust Amt to Wgt Amt */   
        }                                                                   
    }
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUEditCode(UCHAR uchMinorClass,
*                                    UCHAR uchType,
*                                    UCHAR uchCodeNumber);
*               
*       Input:  UCHAR   uchMinorClass  :
*               UCHAR   uchType        : 
*               UCHAR   uchCodeNumber  :
*
*      Output:  nothing
*
**     Return:  SUCCESS
*               RPT_ABORTED
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*                                                                       
**  Description:  
*       Edit PLU for Code group.
*===========================================================================
*/

SHORT RptPLUEditCode(UCHAR uchMinorClass, 
                     UCHAR uchType, 
                     UCHAR uchCodeNumber)
{
    USHORT      usStatus;
    SHORT       sReturn;
    UCHAR       uchTtlFlag = 0, uchFlag = 0;
    PLUIF_REP   PluIf_Rep = {0};

    if ((uchCodeNumber < 1) || (uchCodeNumber > 10)) {
        return(LDT_KEYOVER_ADR);  
    }

    /* Check PLU File Exist or Not */
    PluIf_Rep.uchReport = uchCodeNumber;
    PluIf_Rep.culCounter = 0L;
    sReturn = SerOpRprtPluRead(&PluIf_Rep, 0);
    if ((sReturn == OP_EOF) || (sReturn == OP_NOT_IN_FILE)) {       /* File Not Exist */
        return(LDT_NTINFL_ADR);
    }

    uchRptScaleTtl = 0;    /* Initialize Total Flag for PLU and MINUS Total */
    for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++) {   /* Loop 3 times   */
		LTOTAL      Total = {0};

        uchFlag = 0;
        if ((sReturn = RptPLUCode(uchMinorClass, uchType, usStatus, uchCodeNumber, &Total, &uchFlag)) != SUCCESS) {
            if ((uchRptOpeCount & RPT_HEADER_PRINT) && (sReturn != RPT_ABORTED)) {
                RptPrtError(sReturn);                               /* print Error Number */ 
            }
            return(sReturn);
        }
        uchTtlFlag |= uchFlag;
        if ((usStatus == RPT_CREDIT) && (uchTtlFlag != 0)) {        /* CREDIT DEPT ?     */
            RptFeed(RPT_DEFALTFEED);                                /* Line Feed         */
                                                                    /* Print Grand Total */
            RptPLUGrandTtlPrt(TRN_GTTLR_ADR, &Total, usStatus, uchRptScaleTtl, uchType); 
        }
        if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
        }
    } /*----- End of For Loop -----*/
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUCode(UCHAR  uchMinorClass,
*                                UCHAR  uchType,
*                                USHORT usStatus,
*                                UCHAR  uchCodeNo,
*                                LTOTAL *pTotal,
*                                UCHAR  *puchFlag);
*               
*       Input:  UCHAR     uchMinorClass :
*               UCHAR     uchType       :
*               USHORT    usStatus      :
*               UCHAR     uchCodeNo     :
*               LTOTAL    *pTotal       : 
*               UCHAR     *puchFlag      :
*
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
*
**  Description:  
*       Classify DEPT to NORMAL, MINUS or HASH and OPEN-NONADJECTIVE or ONEADJECTIVE.
*       Print Sub Total.
*
*===========================================================================
*/
SHORT   RptPLUCode(UCHAR uchMinorClass, UCHAR uchType, USHORT usStatus, 
                                UCHAR uchCodeNo, LTOTAL *pTotal, UCHAR  *puchFlag)
{
    UCHAR            uchCodeFlag,
                     uchPLUType,
                     uchScaleIn,   /* To Checkscalable item is existed */
                     uchScaleOut;  /* To Store uchScaleIn Bit */
    D13DIGITS        lSubTotal;
    LONG             lSubCounter;
    SHORT            sReturn;
    USHORT           usDEPT;
    PLUIF_REP        PluIf_Rep = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    if ((usStatus == RPT_CREDIT) && !(uchRptStatus & RPT_EXIST_CREDIT)) {
        return(SUCCESS);
    } else if ((usStatus == RPT_HASH) && !(uchRptStatus & RPT_EXIST_HASH)) {
        return(SUCCESS);
    }
    PluIf_Rep.uchReport = uchCodeNo;                                /* Set Report Code */
    PluIf_Rep.culCounter = 0;                                       /* Set First Counter */
    uchCodeFlag = 0;
    lSubTotal = 0;
    lSubCounter = 0;
    uchScaleOut = 0;
                                                                    /* Get PLU No and other Data */
    while ((sReturn = SerOpRprtPluRead(&PluIf_Rep, 0)) == OP_PERFORM) {  
		DEPTIF           DeptIf = {0};
		TTLPLU           TtlPlu = {0}; 

        uchScaleIn = 0;
		TtlPlu.uchMajorClass = CLASS_TTLPLU;                            /* Set Major for Ttl */
		TtlPlu.uchMinorClass = uchMinorClass;                           /* Set Minor for Ttl */
        TtlPlu.uchAdjectNo = PluIf_Rep.uchPluAdj;           /* MUST set 1 (non-adjective) for Total */
        _tcsncpy(TtlPlu.auchPLUNumber, PluIf_Rep.auchPluNo, OP_PLU_LEN);
        RflConvertPLU(PluIf_Rep.auchPluNo,TtlPlu.auchPLUNumber);
        RptPLUHeader(uchMinorClass, uchType, PluIf_Rep.auchPluNo, 0);
        _tcsncpy(PluIf_Rep.auchPluNo,TtlPlu.auchPLUNumber, OP_PLU_LEN);
        if ((usStatus == RPT_OPEN) &&                               /* Execute 1st Code ? */
           (uchCodeFlag == 0)) {                                    /*  to check OPEN & Flag */
            RptFeed(RPT_DEFALTFEED);                                /* Line Feed        */
            RptCodeNamePrt(uchCodeNo, uchType);                     /* Code Name Print */
            uchCodeFlag = 1;                                        /* Set Flag to identify code */
        }                                                           /*  name is alredy printed */

        /*----- PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status) -----*/
        usDEPT = PluIf_Rep.ParaPlu.ContPlu.usDept; 
                                                                                    /* Wipe out PLU Type */
        DeptIf.usDeptNo = usDEPT;                                                   /* Set DEPT No */
        sReturn = CliOpDeptIndRead(&DeptIf, 0);                                     /* Get DEPT Status */
        if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE)) {
            sReturn = OpConvertError(sReturn);
            RptPrtError(sReturn);
            return(sReturn);
        }

        if (sReturn == OP_PERFORM) {
            if (PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL) {         /* Use Dept Status ? */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];    /* Copy Bit Status 0 */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];    /* Copy Bit Status 1 */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];    /* Copy Bit Status 2 */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];    /* Copy Bit Status 3 */
                /* --- increase control code at R3.1 --- */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];    /* Copy Bit Status 5 */
            } else {   /* Copy DEPT Parameter file Address 6 BIT2 to PLU Parameter file Address 7 BIT2 */
                DeptIf.ParaDept.auchControlCode[2] &= PLU_HASH;                                     /* Clear 0x04 in ...Code */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;                            /* Clear 0xfb in ...Code */
                PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] |= DeptIf.ParaDept.auchControlCode[2];   /* Copy Bit Status 2 */
            }
        } 

        /*----- Clear 0xc0 in PluIf_Rep...uchDEPT -----*/
        uchPLUType = (UCHAR)(PluIf_Rep.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);/* Saratoga */

        switch(usStatus) {
        case RPT_OPEN:         
            if (!(PluIf_Rep.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&    /* NORMAL 3-TYPE */
                !(PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {

                if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) &&    /* Get Total */      
                   (sReturn != TTL_NOTINFILE)) {                                     
                    return(TtlConvertError(sReturn));                                                  
                }
                if (uchPLUType == PLU_ADJ_TYP) {                                /* ADJECTIVE TYPE */
                    sReturn = RptPLUCodeAdj(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLADJ1, puchFlag, &uchScaleIn, uchType);
                    if (sReturn != SUCCESS) {
                        return(sReturn);
                    }
                } else if (uchPLUType == PLU_NON_ADJ_TYP) {                     /* PRESET TYPE */
                    RptPLUCodeOpen(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLPRSET, puchFlag, &uchScaleIn, uchType);
                } else {                                                        /* OPEN TYPE */
                    RptPLUCodeOpen(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLOPEN, puchFlag, &uchScaleIn, uchType);
                }
                lSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
                lSubCounter += TtlPlu.PLUTotal.lCounter; 
            } else {
                if ((PluIf_Rep.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                    !(PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {   
                    uchRptStatus |= RPT_EXIST_CREDIT;
                } else if (PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {        /* HASH 3-TYPE */
                    uchRptStatus |= RPT_EXIST_HASH;
                }
                uchRptNoPrtCo++;                       /* Count up not Print Counter */
            }
            break;

        case RPT_CREDIT:                                          
            if ((PluIf_Rep.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                !(PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {   
                                                                              
                if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) &&    /* Get Total */      
                   (sReturn != TTL_NOTINFILE)) {                                     
                    return(TtlConvertError(sReturn));                                                  
                }
                if (uchPLUType == PLU_ADJ_TYP) {                                /* COUPON TYPE 2, ONE-ADJECTIVE */
                    sReturn = RptPLUCodeAdj(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlag, &uchScaleIn, uchType);
                    if (sReturn != SUCCESS) {
                        return(sReturn);
                    }
                } else if (uchPLUType == PLU_NON_ADJ_TYP) {                     /* COUPON TYPE 1, NONE-ADJECTIVE */
                    RptPLUCodeOpen(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlag, &uchScaleIn, uchType);
                } else {                                                        /* CREDIT TYPE */
                    RptPLUCodeOpen(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlag, &uchScaleIn, uchType);
                }
                lSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
                lSubCounter += TtlPlu.PLUTotal.lCounter;                         
            } else {
                uchRptNoPrtCo++;                       /* Count up not Print Counter */
            }
            break;

        default:                                                                /* case of RPT_HASH */       
            if (PluIf_Rep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {        /* HASH 3-TYPE */
                if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) &&    /* Get Total */      
                   (sReturn != TTL_NOTINFILE)) {                                     
                    return(TtlConvertError(sReturn));                                                  
                }
                if (uchPLUType == PLU_ADJ_TYP) {                                /* ONE-ADJECTIVE */
                    sReturn = RptPLUCodeAdj(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlag, &uchScaleIn, uchType);
                    if (sReturn != SUCCESS) {
                        return(sReturn);
                    }
                } else if (uchPLUType == PLU_NON_ADJ_TYP) {                     /* NONE-ADJECTIVE */
                    RptPLUCodeOpen(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlag, &uchScaleIn, uchType);
                } else {                                                        /* OPEN TYPE  */
                    RptPLUCodeOpen(&PluIf_Rep, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlag, &uchScaleIn, uchType);
                }
                lSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
                lSubCounter += TtlPlu.PLUTotal.lCounter;
            } else {
                uchRptNoPrtCo++;                       /* Count up not Print Counter */
            }
            break;
        } /*----- End of Switch Loop -----*/
        if (UieReadAbortKey() == UIE_ENABLE) {                                  /* if Abort ?      */
            return(RPT_ABORTED);
        } 
        if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
        }
        uchScaleOut |= uchScaleIn;
        if (uchRptNoPrtCo >= RPTNOPRTCO_MAX) {
            SerTtlReadWriteChkAndWait(RPT_WAIT_TIMER);  /* Sleep */
            uchRptNoPrtCo = 0;
        }
    } /*----- End of While Loop -----*/
    if (sReturn != OP_EOF) {                                                    /* Convert Error */
        return(OpConvertError(sReturn));
    }
    if (usStatus != RPT_HASH) {                                                 /* Not HASH ? */
        RptGTotal.lAmount += lSubTotal;                                         /* Add SubTtl to static */
        RptGTotal.lCounter += lSubCounter;                                      /*  area of Grand Total */
    }
    pTotal->lAmount += lSubTotal;                                               /* Store SubTtl to local */
    pTotal->lCounter += lSubCounter;                                            /*  area of Total */
    if (*puchFlag & 0x01) {                                                     /* Print item(s) ? */
		RPTPLU           RptPlu = {0};

        RptFeed(RPT_DEFALTFEED);                                                /* Line Feed       */
        if(RptPauseCheck() == RPT_ABORTED){         /* aborted by MLD */
            return(RPT_ABORTED);
        }

        /*----- Get Special Mnemonics -----*/
        RptPlu.uchMajorClass = CLASS_RPTPLU;                                    /* Set Major to print */
        RptPlu.PLUTotal.lAmount = lSubTotal;                                    /* Set SubTotal to print */
        RptPlu.uchMinorClass = CLASS_RPTPLU_PRTTTLCNT;
        RflGetTranMnem (RptPlu.aszMnemo, TRN_STTLR_ADR);                        /* Copy SubTotal Mnemo to print */
        RptPlu.usPrintControl = usRptPrintStatus;                               /* Set Print Status */
        if ((uchScaleOut == 1) && (usStatus == RPT_OPEN)) {   
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
                RptPlu.uchStatus = RPT_DECIMAL_2;                               /* Decimal - 0.01 */
            } else {       /*----- MDC #29 BIT1 is 0 -----*/
                RptPlu.uchStatus = RPT_DECIMAL_1;                               /* Decimal - 0.1 */
            }
            RptPlu.PLUTotal.lCounter = lSubCounter;                             
        } else {                                                                 
            RptPlu.uchStatus = RPT_DECIMAL_0;                     
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 
                RptPlu.PLUTotal.lCounter = lSubCounter / 100L;                   
            } else {
                RptPlu.PLUTotal.lCounter = lSubCounter / 10L;                    
            }                                                  
        }                                                                       
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);/* Print GrandTotal */
            RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }

        PrtPrintItem(NULL, &RptPlu);                                            /* Print out */

        /* send print data to shared module */
        PrtShrPartialSend(RptPlu.usPrintControl);

    }
    if (UieReadAbortKey() == UIE_ENABLE) {                                      /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                                           /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
        return(RPT_ABORTED);
    }
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptCodeNamePrt(USHORT uchCodeNo)
*               
*       Input:  UCHAR  uchCodeNo
*
*      Output:  nothing
*
**     Return:  nothing
*
**  Description:
*       Print Code Number at the top of Code group.
*===========================================================================
*/
VOID    RptCodeNamePrt(USHORT usCodeNo, UCHAR uchType)
{   
	RPTPLU RptPlu = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    RptPlu.usReportCode = usCodeNo;                     /* Set Report Code # for Print */
    RptPlu.uchMajorClass = CLASS_RPTPLU;                /* Set Major for Print */
    RptPlu.uchMinorClass = CLASS_RPTPLU_PRTCODE;        /* Set Minor for Print */
    RptPlu.usPrintControl = usRptPrintStatus;           /* Set Print Status for Print */
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);/* Print GrandTotal */
        RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptPlu);                        /* Print out           */ 
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUCodeAdj(PLUIF_REP  *pPluIf_Rep,
*                                   TTLPLU     *pTtlPlu,    
*                                   UCHAR      uchMinor,
*                                   UCHAR      *puchFlag,
*                                   UCHAR      *puchScaleIn);
*               
*       Input:  PLUIF_REP  *pPluIf_Rep
*               TTLPLU     *pTtlPlu    
*               UCHAR      uchMinor 
*               UCHAR      *puchFlag
*               UCHAR      *puchScaleIn
*
*      Output:  nothing
*
**     Return:  LDT_PROHBT_ADR  : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR   : (OP_PLU_FULL)
*               LDT_NTINFL_ADR  : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR    : (OP_LOCK)
*
**  Description:  
*       Get all 5 Adjective Data.  Add to Total-lAmount and lCounter.
*===========================================================================
*/

SHORT  RptPLUCodeAdj(PLUIF_REP  *pPluIf_Rep,
                     TTLPLU     *pTtlPlu,
                     UCHAR      uchMinor,
                     UCHAR      *puchFlag,
                     UCHAR      *puchScaleIn,
                     UCHAR      uchType)
{
    SHORT   sReturn;
    USHORT  usPercent = 0;
    UCHAR   uchAdjNo,
            uchNo,
            uchPluProhibitVal;
    UCHAR   uchStatus = RPT_DECIMAL_0;           /* to store Decimal point status */
	LTOTAL     AdjTtl = {0};
    TCHAR      AdjMnemo[PARA_ADJMNEMO_LEN + 1] = {0};
    TCHAR      SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};
    PLUIF_REP  PluFile_Rep = {0};   /* to get unit price of Adj2 to Adj5 */
                                    /* other data is used in pPluIf_Rep structure */

    if (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG1) {          /* Group 1 */
        uchNo = ADJ_GP1VAR1_ADR;
    } else if (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG2) {   /* Group 2 */
        uchNo = ADJ_GP2VAR1_ADR;
    } else if (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG3) {   /* Group 3 */
        uchNo = ADJ_GP3VAR1_ADR;
    } else {                                                                     /* Group 4 */
        uchNo = ADJ_GP4VAR1_ADR;
    }

    /*----- Get Adjective Mnemonics -----*/
    RflGetAdj (AdjMnemo, uchNo);                                            /* Get Adjective Mnemonics */

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemo LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS and MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);                   /* Calculate Percent */
    }
    uchPluProhibitVal = PLU_PROHIBIT_VAL1;
    if (RptPLUPrintChk2(pTtlPlu, pPluIf_Rep->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) == 0) { 
        if (*puchFlag == 0x00)  {                        /* The 1st Adjective ? */
            RptFeed(RPT_DEFALTFEED);                     /* Line Feed */
        }
        RptPLUPrint(uchMinor,                            /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                  /* TOTAL- lAmount & lCounter */ 
                    pPluIf_Rep->auchPluNo,               /* PLU Number */
                    pPluIf_Rep->ParaPlu.auchPluName,     /* PLU Mnemonics */            
                    SpeMnemo,                            /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                            /* Ajective Mnemoincs */
                    pPluIf_Rep->ParaPlu.auchPrice,       /* PresetPrice */       
                    usPercent,                           /* Percent */             
                    uchStatus,                           /* Decimal point status */
                    uchType);                            /* Report Type */

        *puchFlag |= 0x01;
        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else {                                                /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
    } else {
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    }

    AdjTtl.lAmount = pTtlPlu->PLUTotal.lAmount;                             /* Add lAmount to Adjective Ttl */
    if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter;                       /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 100L;            /* Adjust Amt to Wgt Amt */
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;             /* Adjust Amt to Wgt Amt */   
        }
    }

    /*----- loop 2nd to 5th Adjective -----*/
    /*****======================================================================*****/
    /* Using PLU Parameter File from Address3 to Address10 that are set in function */
    /* RptPLUCode if PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status).    */
    /* In other case, using PLU Parameter File Address6 BIT2                        */
    /* The reason to copy from pPluIf_Dep to PluFile_Dep is to get each Adjective   */
    /* unit Price                                                                   */
    /*****======================================================================*****/

    PluFile_Rep = *pPluIf_Rep;          /* Copy to other work Buffer to save copied Status */
    for (uchAdjNo = uchNo+(UCHAR)1; uchAdjNo < uchNo+(UCHAR)5; uchAdjNo++) {      
        PluFile_Rep.uchPluAdj++;                 /* Next Adjective for other work Buffer */
        if ((sReturn = SerOpRprtPluRead(&PluFile_Rep, 0)) != OP_PERFORM) { /* Skip if file not found */
            return(OpConvertError(sReturn));
        }
        /*----- to store new Adjective unit price -----*/
        memcpy(pPluIf_Rep->ParaPlu.auchPrice, PluFile_Rep.ParaPlu.auchPrice, sizeof(pPluIf_Rep->ParaPlu.auchPrice));  

        memset(&pTtlPlu->PLUTotal, 0, sizeof(pTtlPlu->PLUTotal));           /* Clear Total */
        _tcsncpy(pTtlPlu->auchPLUNumber, pPluIf_Rep->auchPluNo, OP_PLU_LEN);
        pTtlPlu->uchAdjectNo = PluFile_Rep.uchPluAdj;                       /* Set Adjective */
        if (((sReturn = SerTtlTotalRead(pTtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
            return(TtlConvertError(sReturn));
        }
        uchPluProhibitVal = (UCHAR)(uchPluProhibitVal << 1);                    /* Shift right 1 bit in each time */
        if (RptPLUPrintChk2(pTtlPlu, pPluIf_Rep->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) != 0) { 
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
            continue;
        }

        /*----- Get Adjective Mnemonics -----*/
        RflGetAdj (AdjMnemo, uchAdjNo);                                     /* Get Adjective Mnemonics */

        AdjTtl.lAmount += pTtlPlu->PLUTotal.lAmount;                        /* Add lAmount to Adjective Ttl */
        if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&  /* Plus item PLU ? */
            !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
             (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) { /* Scalable ? */
            AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter;                  /* Store Wgt Amt /w Decimal point */
        } else{                                                             
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* !Scalable - MDC #89 BIT 2 is 0 */
                AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 100L;       /* Adjust Amt to Wgt Amt */
            } else {                                                        /* !Scalable - MDC #89 BIT 2 is 1 */  
               AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 10L;         /* Adjust Amt to Wgt Amt */   
            }
        }

        /*----- Calculate Percent based on MDC -----*/
        if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&  /* NORMAL TYPE ? */
           !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {

            usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);               /* Calculate Percent */
        }                                                                   /* the 1st Adjective ? */
        if (*puchFlag == 0x00) {                                            /* Line Feed */
            RptFeed(RPT_DEFALTFEED);
        }
        RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                    pPluIf_Rep->auchPluNo,              /* PLU Number */
                    pPluIf_Rep->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    SpeMnemo,                           /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                           /* Adjective Mnemoincs */
                    pPluIf_Rep->ParaPlu.auchPrice,      /* PresetPrice */       
                    usPercent,                          /* Percent */             
                    uchStatus,                          /* Decimal point status */
                    uchType);                           /* Report Type */

        *puchFlag |= 0x01;
        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else if (uchMinor == CLASS_RPTPLU_PRTTTLMINUSADJ1) {  /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
    } /*----- End of For Loop -----*/

    pTtlPlu->PLUTotal = AdjTtl;
    *pPluIf_Rep = PluFile_Rep;    /* Copy to other work Buffer  */
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptPLUCodeOpen(PLUIF_REP  *pPluIf_Rep,
*                                   TTLPLU     *pTtlPlu,
*                                   UCHAR uchMinor,
*                                   UCHAR *puchFlag,
*                                   UCHAR *puchScaleIn);
*               
*       Input:  PLUIF_REP   *pPluIf_Rep
*               TTLPLU      *pTtlPlu
*               UCHAR       uchMinor
*               UCHAR       *puchFlag
*               UCHAR       *puchScaleIn
*
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Get all Data for Code Open PLU.
*===========================================================================
*/

VOID  RptPLUCodeOpen(PLUIF_REP *pPluIf_Rep,
                     TTLPLU    *pTtlPlu,
                     UCHAR     uchMinor,
                     UCHAR     *puchFlag,
                     UCHAR     *puchScaleIn,
                     UCHAR     uchType)
{
    UCHAR         uchStatus = RPT_DECIMAL_0;
    USHORT        usPercent = 0;
	TCHAR         SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemo LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS and MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);
    }
    if ((pTtlPlu->PLUTotal.lAmount == 0L) && (pTtlPlu->PLUTotal.lCounter == 0L) && /* Total = 0 ? */
        (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT2))) {                /* MDC #89 BIT 2 */
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    } else {
        if (*puchFlag == 0x00) {
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed */    
        }
        switch(uchMinor) {
        case CLASS_RPTPLU_PRTTTLOPEN:                       /* CLASS_RPTPLU_PRTTTLOPEN */ 
            /* break; */
        case CLASS_RPTPLU_PRTTTLCREDIT:                     /* CLASS_RPTPLU_PRTTTLCREDIT */ 
            RptPLUPrint(uchMinor,                           
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */             
                        pPluIf_Rep->auchPluNo,              /* PLU Number */
                        pPluIf_Rep->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                           /* Special Mnemonics LB/Kg */             
                        0,                                  /* NOT USE (Ajective Mnemoincs) */              
                        0,                                  /* NOT USE (PresetPrice) */             
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Report Type */
            *puchFlag |= 0x01;
            break;

        default:                                            /* case of CLASS_RPTPLU_PRTTTLPRSET */
                                                            /* case of CLASS_RPTPLU_PRTTTLCOUPON */
            RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLPRSET */ 
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                        pPluIf_Rep->auchPluNo,              /* PLU Number */
                        pPluIf_Rep->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                           /* Special Mnemonics LB/Kg */     
                        0,                                  /* NOT USE (Ajective Mnemoincs) */
                        pPluIf_Rep->ParaPlu.auchPrice,      /* PresetPrice */       
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Report Type */
            *puchFlag |= 0x01;
            break;

        } /*----- End of Switch Loop -----*/
    }
    if (!(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Rep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter;            /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */ 
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 100L; /* Adjust Amt to Wgt Amt */       
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */ 
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;  /* Adjust Amt to Wgt Amt */         
        }                                                                      
    }
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUEditAll(UCHAR  uchMinorClass,
*                                      UCHAR  uchType,
*                                      UCHAR  uchResetType)
*               
*       Input:  UCHAR   uchMinorClass :
*               UCHAR   uchType       : RPT_DEPT_READ
*                                     : RPT_ALL_READ
*                                     : RPT_EOD_ALLREAD
*                                     : RPT_PTD_ALLREAD 
*               UCHAR   uchResetType  : 0
*                                     : RPT_ONLY_PRT_RESET
*                                     : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               LDT_KEYOVER_ADR  :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
*
**  Description:  
*       Read All PLU by PLU Number,                             Saratoga
*===========================================================================
*/
SHORT   RptPLUEditAll(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchResetType)
{
    UCHAR            uchFlag = 0;       /* To Check item is existed */
    UCHAR            uchTtlFlag = 0;    /* To store uchFlag Bit */
    UCHAR            uchScaleOut = 0;
    SHORT            sReturn;
    SHORT            sTblId;
    USHORT           usFunc;
    ULONG            ulPluRecNum;
	SHORT			 i = 0;
    DCURRENCY        lSubTotal = 0;
    LONG             lSubCounter = 0;
    BOOL             bActive;
    ECMCOND          EcmCond = {0};
    PLUIF            PluIf = {0};
	TTLPLU			 myPluTotal = {0};
	LTOTAL           Total = {0};

    if (uchType == RPT_ALL_READ && RflGetMaxRecordNumberByType(FLEX_PLU_ADR) == 0L) {
		return(LDT_KEYOVER_ADR);            
    }

    /* Check File Exist or Not */
    sReturn = SerOpPluAllRead(&PluIf, 0);
    if (sReturn == OP_EOF) /* File Not Exist */
    {      
        RptPLUHeader(uchMinorClass, uchType, PluIf.auchPluNo, uchResetType);
        return(SUCCESS);
    }

    /*uchTtlFlag = 0;*/
    uchRptScaleTtl = 0;         /* Initialize Total of PLU or MINUS Total */

    bActive = FALSE;
    usFunc = FUNC_REPORT_COND;

    if (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT2)) {
        EcmCond.fsOption = REPORT_ACTIVE;
        bActive = TRUE;
    } else {
        EcmCond.fsOption = REPORT_ACTIVE | REPORT_INACTIVE;
    }

    if (uchResetType != RPT_TYPE_ONLY_PRT)  {  /* reset report ?,  Saratoga */
        EcmCond.fsOption |= REPORT_AS_RESET;
    }

	//Release 2.X CUSTOMS SCER
	//If we are not master then we will need to read from the master
	//so that we can report on satelites. JHHJ
	if(sReturn == OP_NOT_MASTER)
	{
    BOOL     fContinue = TRUE;
    USHORT   usStatus;

	for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++)/* Loop 3times */
	{
		LONG    lCounter = -1;

		do
		{
			TTLPLUEX    myTotal = {0};

			myTotal.uchResetStatus = uchResetType;
			myTotal.uchMinorClass = uchMinorClass;
			CliRptPLURead(&myTotal);

			if(lCounter < 0) {
				//reset the counter for reading purposes.
				lCounter = myTotal.PLUAllTotal.lCounter;
			}

				i = 0;
				while((myTotal.Plu[i].auchPLUNumber[0]) && (i < 20))
				{
					if ((usStatus == RPT_CREDIT) && !(uchRptStatus & RPT_EXIST_CREDIT)) {
						fContinue = FALSE;
						break;
					} else if ((usStatus == RPT_HASH) && !(uchRptStatus & RPT_EXIST_HASH)) {
						fContinue = FALSE;
						break;
					} else {
						fContinue = TRUE;
					}

					_tcsncpy(PluIf.auchPluNo, myTotal.Plu[i].auchPLUNumber, NUM_PLU_LEN);
					if ((myTotal.Plu[i].uchAdjectNo - 1) % 5 + 1) {
						PluIf.uchPluAdj = (myTotal.Plu[i].uchAdjectNo - 1) % 5 + 1;
					} else {
						PluIf.uchPluAdj = 1;      /* dummy adjective valiation */
					}

					CliOpPluIndRead(&PluIf, 0);

					//Put the necessary information into the myPluTotal information
					//so that we can report the percentage and PLU information
					_tcsncpy(myPluTotal.auchPLUNumber, myTotal.Plu[i].auchPLUNumber, NUM_PLU_LEN);
					myPluTotal.uchAdjectNo = PluIf.uchPluAdj;
					myPluTotal.FromDate = myTotal.FromDate;
					myPluTotal.ToDate = myTotal.ToDate;
					myPluTotal.PLUAllTotal = myTotal.PLUAllTotal;
					myPluTotal.PLUTotal = myTotal.Plu[i].PLUTotal;
					
					lCounter -= myPluTotal.PLUTotal.lCounter;
					if(RptPLUByACTPLUSat(uchMinorClass, 
										  uchType,
										  uchResetType,
										  usStatus,
										  &uchFlag,
										  &lSubTotal,
										  &lSubCounter,
										  &uchScaleOut,
										  &PluIf,
										  &myPluTotal) != SUCCESS)
					  {
							/*if((sReturn == SPLU_INVALID_HANDLE) || (sReturn == SPLU_FILE_NOT_FOUND) ||
							   (sReturn == SPLU_FUNC_KILLED))*/
							if(sReturn != OP_EOF) {
								if (sReturn == RPT_ABORTED) {
									SerOpPluAbort(PluIf.husHandle, 0);
									return(sReturn);
								}

								sReturn = LDT_ERR_ADR;
								if ((uchRptOpeCount & RPT_HEADER_PRINT) && (sReturn != RPT_ABORTED)) {
									RptPrtError(sReturn);
									/*SerPluExitCritMode(usFile,0);*/
									SerOpPluAbort(PluIf.husHandle, 0);
									return(sReturn);
								}
							}
							/* OP_EOF */
					}
					i++;
			} /* Not Success */
		} while((fContinue == TRUE) && (lCounter > 0));
	}
		if (usStatus != RPT_HASH) {
			// if not a hash department PLU then add to the subtotals.
            RptGTotal.lAmount += lSubTotal;              /* Add SubTtl to static */
            RptGTotal.lCounter += lSubCounter;           /*  area of Grand Total */
        }
        Total.lAmount += lSubTotal;
        Total.lCounter += lSubCounter;

        if(uchFlag & 0x01) {
			RPTPLU           RptPlu = {0};

            RptFeed(RPT_DEFALTFEED);
            if(RptPauseCheck() == RPT_ABORTED) {
                SerOpPluAbort(PluIf.husHandle, 0);
                return(RPT_ABORTED);
            }

            RptPlu.uchMajorClass = CLASS_RPTPLU;                         /* Set Major to print */
            RptPlu.PLUTotal.lAmount = lSubTotal;                         /* Set SubTotal to print */
            RptPlu.uchMinorClass = CLASS_RPTPLU_PRTTTLCNT;
            if ((uchScaleOut == 1) && (usStatus == RPT_OPEN)) {
                if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {
                    RptPlu.uchStatus = RPT_DECIMAL_2;
                } else {
                    RptPlu.uchStatus = RPT_DECIMAL_1;
                }
                RptPlu.PLUTotal.lCounter = lSubCounter;
            } else {
                RptPlu.uchStatus = RPT_DECIMAL_0;
                if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {
                    RptPlu.PLUTotal.lCounter = lSubCounter / 100L;
                }
                else {
                    RptPlu.PLUTotal.lCounter = lSubCounter / 10L;
                }
            }
            RflGetTranMnem (RptPlu.aszMnemo, TRN_STTLR_ADR);
            RptPlu.usPrintControl = usRptPrintStatus;
            if (RptCheckReportOnMld()) {
                uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);
                RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
            }
            PrtPrintItem(NULL, &RptPlu);
            PrtShrPartialSend(RptPlu.usPrintControl);
        }
        if (UieReadAbortKey() == UIE_ENABLE) {
            SerOpPluAbort(PluIf.husHandle, 0);
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED) {
            SerOpPluAbort(PluIf.husHandle, 0);
            return(RPT_ABORTED);
        }
	} else if(!bActive) {
		USHORT   usStatus;

        for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++) {
            uchFlag = 0;
            lSubTotal = 0L;
            lSubCounter = 0L;
            PluIf.ulCounter = 0L;
            PluIf.usAdjCo = 0;

            while(1) {
                if ((usStatus == RPT_CREDIT) && !(uchRptStatus & RPT_EXIST_CREDIT)) {
                    break;
                } else if ((usStatus == RPT_HASH) && !(uchRptStatus & RPT_EXIST_HASH)) {
                    break;
                }

                if ((sReturn = RptPLUByPLU(uchMinorClass, 
                                           uchType, 
                                           /*usFile, */
                                           uchResetType,
                                           usStatus,
                                           &uchFlag,
                                           &lSubTotal,
                                           &lSubCounter,
                                           &uchScaleOut,
                                           &PluIf)) != SUCCESS) 
                {
                    /*if((sReturn == SPLU_INVALID_HANDLE) || (sReturn == SPLU_FILE_NOT_FOUND) ||
                       (sReturn == SPLU_FUNC_KILLED))*/
                    if(sReturn != OP_EOF) {
                        if (sReturn == RPT_ABORTED) {
                            SerOpPluAbort(PluIf.husHandle, 0);
                            return(sReturn);
                        }

                        sReturn = LDT_ERR_ADR;
                        if ((uchRptOpeCount & RPT_HEADER_PRINT) && (sReturn != RPT_ABORTED)) {
                            RptPrtError(sReturn);
                            SerOpPluAbort(PluIf.husHandle, 0);
                            return(sReturn);
                        }
                    } else if(sReturn == OP_EOF) {
                        if (usStatus != RPT_HASH) {                      /* Not HASH ? */
                            RptGTotal.lAmount += lSubTotal;              /* Add SubTtl to static */
                            RptGTotal.lCounter += lSubCounter;           /*  area of Grand Total */
                        }
                        Total.lAmount += lSubTotal;
                        Total.lCounter += lSubCounter;

                        if(uchFlag & 0x01) {
							RPTPLU           RptPlu = {0};

                            RptFeed(RPT_DEFALTFEED);
                            if(RptPauseCheck() == RPT_ABORTED) {
                                /*SerPluExitCritMode(usFile,0);*/
                                SerOpPluAbort(PluIf.husHandle, 0);
                                return(RPT_ABORTED);
                            }

                            RptPlu.uchMajorClass = CLASS_RPTPLU;                         /* Set Major to print */
                            RptPlu.PLUTotal.lAmount = lSubTotal;                         /* Set SubTotal to print */
                            RptPlu.uchMinorClass = CLASS_RPTPLU_PRTTTLCNT;
                            if ((uchScaleOut == 1) && (usStatus == RPT_OPEN)) {
                                if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {
                                    RptPlu.uchStatus = RPT_DECIMAL_2;
                                } else {
                                    RptPlu.uchStatus = RPT_DECIMAL_1;
                                }
                                RptPlu.PLUTotal.lCounter = lSubCounter;
                            } else {
                                RptPlu.uchStatus = RPT_DECIMAL_0;
                                if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {
                                    RptPlu.PLUTotal.lCounter = lSubCounter / 100L;
                                } else {
                                    RptPlu.PLUTotal.lCounter = lSubCounter / 10L;
                                }
                            }
                            RflGetTranMnem (RptPlu.aszMnemo, TRN_STTLR_ADR);   /* Get SubTotal Mnemo */
                            RptPlu.usPrintControl = usRptPrintStatus;
                            if (RptCheckReportOnMld()) {
                                uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);
                                RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */;
                            }
                            PrtPrintItem(NULL, &RptPlu);
                            PrtShrPartialSend(RptPlu.usPrintControl);
                        }
                        if (UieReadAbortKey() == UIE_ENABLE) {
                            SerOpPluAbort(PluIf.husHandle, 0);
                            return(RPT_ABORTED);
                        }
                        if (RptPauseCheck() == RPT_ABORTED) {
                            SerOpPluAbort(PluIf.husHandle, 0);
                            return(RPT_ABORTED);
                        }
                        break;
                        
                    }/* OP_EOF */
                }/* Not Success */
            } /*----- End of while Loop -----*/

            uchTtlFlag |= uchFlag;

            if ((usStatus == RPT_CREDIT) && (uchTtlFlag != 0)) {
                RptFeed(RPT_DEFALTFEED);
                /* Print Grand Total */
                RptPLUGrandTtlPrt(TRN_GTTLR_ADR, &Total, usStatus, uchRptScaleTtl, uchType); 
            }
            if (UieReadAbortKey() == UIE_ENABLE) {
                /*SerPluExitCritMode(usFile,0);*/
                SerOpPluAbort(PluIf.husHandle, 0);
                return(RPT_ABORTED);
            }
            if(RptPauseCheck() == RPT_ABORTED) {
                /*SerPluExitCritMode(usFile,0);*/
                SerOpPluAbort(PluIf.husHandle, 0);
                return(RPT_ABORTED);
            }
        }/*----- End of For Loop -----*/
    } else {
		USHORT   usStatus;

        switch(uchMinorClass) {
        case CLASS_TTLCURDAY:
            sTblId = PLUTOTAL_ID_DAILY_CUR;
            break;
        case CLASS_TTLSAVDAY:
            sTblId = PLUTOTAL_ID_DAILY_SAV;
            break;
        case CLASS_TTLCURPTD:
            sTblId = PLUTOTAL_ID_PTD_CUR;
            break;
        case CLASS_TTLSAVPTD:
            sTblId = PLUTOTAL_ID_PTD_SAV;
            break;
        default:
            break;
        }

        for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++) {
            uchFlag = 0;
            lSubTotal = 0L;
            lSubCounter = 0L;

            sReturn = TtlPLUOpenRec(sTblId,PLUTOTAL_SORT_PLUNO,&ulPluRecNum);
            if (sReturn == TTL_SUCCESS) {
                if (ulPluRecNum == 0) { /* TOTAL RESET */
                    TtlPLUCloseRec();
                    return(SUCCESS);
                }
            }
            if (sReturn == TTL_FAIL) {
                TtlPLUCloseRec();
                return (LDT_ERR_ADR);
            }
            
            while(1) {
                if ((usStatus == RPT_CREDIT) && !(uchRptStatus & RPT_EXIST_CREDIT)) {
                    break;
                } else if ((usStatus == RPT_HASH) && !(uchRptStatus & RPT_EXIST_HASH)) {
                    break;
                }

                if ((sReturn = RptPLUByACTPLU(uchMinorClass, 
                                              uchType,
                                              uchResetType,
                                              usStatus,
                                              &uchFlag,
                                              &lSubTotal,
                                              &lSubCounter,
                                              &uchScaleOut,
                                              &PluIf)) != SUCCESS) 
                {
                    if (sReturn == LDT_ERR_ADR) {
                        sReturn = LDT_ERR_ADR;

                        if ((uchRptOpeCount & RPT_HEADER_PRINT) && (sReturn != RPT_ABORTED)) {
                            RptPrtError(sReturn);
                        }
                        TtlPLUCloseRec();
                        SerOpPluAbort(PluIf.husHandle,0);
                        return(sReturn);
                    }
#if defined(USE_ORIGINAL)
                    else if(sReturn == TTL_EOF)
#else
                    /* --- Fix a glitch (2001/06/20)
                        The previous version of RptPLUByACTPLU returns two type
                        of return codes which have the same value (1).
                          - Cannot find the PLU paramter (LDT_NTINFL_ADR)
                          - Found the last total PLU record (TTL_EOF)
                    --- */
                    else if(sReturn == LDT_NTINFL_ADR)
                    {
                        /* --- When the PLU record is not found in PARAPLU,
                            skip it and get the next record. --- */
                        continue;
                    }
                    else if(sReturn == RPT_EOF)
#endif
                    {
                        if (usStatus != RPT_HASH) {
                            RptGTotal.lAmount += lSubTotal;              /* Add SubTtl to static */
                            RptGTotal.lCounter += lSubCounter;           /*  area of Grand Total */
                        }
                        Total.lAmount += lSubTotal;
                        Total.lCounter += lSubCounter;

                        if(uchFlag & 0x01) {
							RPTPLU           RptPlu = {0};

                            RptFeed(RPT_DEFALTFEED);
                            if(RptPauseCheck() == RPT_ABORTED) {
                                TtlPLUCloseRec();
                                SerOpPluAbort(PluIf.husHandle,0);
                                return(RPT_ABORTED);
                            }

							RptPlu.uchMajorClass = CLASS_RPTPLU;                         /* Set Major to print */
                            RptPlu.PLUTotal.lAmount = lSubTotal;                         /* Set SubTotal to print */
                            RptPlu.uchMinorClass = CLASS_RPTPLU_PRTTTLCNT;
                            if ((uchScaleOut == 1) && (usStatus == RPT_OPEN)) {
                                if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {
                                    RptPlu.uchStatus = RPT_DECIMAL_2;
                                } else {
                                    RptPlu.uchStatus = RPT_DECIMAL_1;
                                }
                                RptPlu.PLUTotal.lCounter = lSubCounter;
                            } else {
                                RptPlu.uchStatus = RPT_DECIMAL_0;
                                if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {
                                    RptPlu.PLUTotal.lCounter = lSubCounter / 100L;
                                } else {
                                    RptPlu.PLUTotal.lCounter = lSubCounter / 10L;
                                }
                            }
                            RflGetTranMnem (RptPlu.aszMnemo, TRN_STTLR_ADR);
                            RptPlu.usPrintControl = usRptPrintStatus;
                            if (RptCheckReportOnMld()) {
                                uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);
                                RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
                            }
                            PrtPrintItem(NULL, &RptPlu);
                            PrtShrPartialSend(RptPlu.usPrintControl);
                        }
                        if (UieReadAbortKey() == UIE_ENABLE) {
                            TtlPLUCloseRec();
                            SerOpPluAbort(PluIf.husHandle,0);
                            return(RPT_ABORTED);
                        }
                        if(RptPauseCheck() == RPT_ABORTED) {
                            TtlPLUCloseRec();
                            SerOpPluAbort(PluIf.husHandle,0);
                            return(RPT_ABORTED);
                        }
                        break;
                    } else {
						/* TTL EOF */
                        if (sReturn == RPT_ABORTED) {
                            TtlPLUCloseRec();
                            SerOpPluAbort(PluIf.husHandle, 0);
                            return(sReturn);
                        }

                        sReturn = LDT_ERR_ADR;
                        if ((uchRptOpeCount & RPT_HEADER_PRINT) && (sReturn != RPT_ABORTED)) {
                            RptPrtError(sReturn);
                        }
                        TtlPLUCloseRec();
                        SerOpPluAbort(PluIf.husHandle,0);
                        return(sReturn);
                    }
                }/* Not Success */        
            } /*----- End of While Loop -----*/
            
            TtlPLUCloseRec();

            uchTtlFlag |= uchFlag;
            if ((usStatus == RPT_CREDIT) && (uchTtlFlag != 0)) {
                RptFeed(RPT_DEFALTFEED);
                /* Print Grand Total */
                RptPLUGrandTtlPrt(TRN_GTTLR_ADR, &Total, usStatus, uchRptScaleTtl, uchType); 
            }
            if (UieReadAbortKey() == UIE_ENABLE) {
                SerOpPluAbort(PluIf.husHandle,0);
                return(RPT_ABORTED);
            }
            if(RptPauseCheck() == RPT_ABORTED) {
                SerOpPluAbort(PluIf.husHandle,0);
                return(RPT_ABORTED);
            }
        }/*----- End of for Loop -----*/
    }

    if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
        SerOpPluAbort(PluIf.husHandle,0);
        return(RPT_ABORTED);
    } 
    if (RptPauseCheck() == RPT_ABORTED) {
        SerOpPluAbort(PluIf.husHandle,0);
        return(RPT_ABORTED);
    }

    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptPLUOpen(PLUIF    *pPluIf,
*                               TTLPLU    *pTtlPlu,
*                               UCHAR     uchMinor,
*                               UCHAR     *puchFlag,
*                               UCHAR     *puchScaleIn);
*               
*       Input:  PRECPLU   pRecPlu
*               TTLPLU      *pTtlPlu
*               UCHAR       uchMinor
*               UCHAR       *puchFlag
*               UCHAR       *puchScaleIn
*
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Get all Data for Code Open PLU.
*===========================================================================
*/

VOID  RptPLUOpen(PLUIF    *pPluIf,
                 TTLPLU    *pTtlPlu,
                 UCHAR     uchMinor,
                 UCHAR     *puchFlag,
                 UCHAR     *puchScaleIn,
                 UCHAR     uchType)
{
    UCHAR     uchStatus = RPT_DECIMAL_0;
    USHORT    usPercent = 0;
	TCHAR     SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemo LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS and MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);
    }
    if ((pTtlPlu->PLUTotal.lAmount == 0L) && (pTtlPlu->PLUTotal.lCounter == 0L) && /* Total = 0 ? */
        (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT2))) {                /* MDC #89 BIT 2 */
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    } else {
        if (*puchFlag == 0x00) {
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed */    
        }
        switch(uchMinor) {
        case CLASS_RPTPLU_PRTTTLOPEN:                       /* CLASS_RPTPLU_PRTTTLOPEN */ 
            /* break; */
        case CLASS_RPTPLU_PRTTTLCREDIT:                     /* CLASS_RPTPLU_PRTTTLCREDIT */ 
            RptPLUPrint(uchMinor,                           
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */             
                        pPluIf->auchPluNo,              /* PLU Number */
                        pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                       /* Special Mnemonics LB/Kg */             
                        0,                                  /* NOT USE (Ajective Mnemoincs) */              
                        0,                                  /* NOT USE (PresetPrice) */             
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Report Type */
            *puchFlag |= 0x01;
            break;

        default:                                            /* case of CLASS_RPTPLU_PRTTTLPRSET */
            RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLPRSET */ 
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                        pPluIf->auchPluNo,              /* PLU Number */
                        pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                       /* Special Mnemonics LB/Kg */     
                        0,                                  /* NOT USE (Ajective Mnemoincs) */
                        pPluIf->ParaPlu.auchPrice,      /* PresetPrice */       
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Report Type */
            *puchFlag |= 0x01;
            break;
        } /*----- End of Switch Loop -----*/
    }
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter;            /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */ 
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 100L; /* Adjust Amt to Wgt Amt */       
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */ 
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;  /* Adjust Amt to Wgt Amt */         
        }                                                                      
    }
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUAdj(PLUIF      *pPluIf,
*                               TTLPLU     *pTtlPlu,    
*                               UCHAR      uchMinor,
*                               UCHAR      *puchFlag,
*                               UCHAR      *puchScaleIn);
*               
*       Input:  PRECPLU    pRecPlu
*               TTLPLU     *pTtlPlu    
*               UCHAR      uchMinor 
*               UCHAR      *puchFlag
*               UCHAR      *puchScaleIn
*
*      Output:  nothing
*
**     Return:  LDT_PROHBT_ADR  : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR   : (OP_PLU_FULL)
*               LDT_NTINFL_ADR  : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR    : (OP_LOCK)
*
**  Description:  
*       Get all 5 Adjective Data.  Add to Total-lAmount and lCounter.
*===========================================================================
*/

SHORT  RptPLUAdj(PLUIF      *pPluIf,
                 TTLPLU     *pTtlPlu,
                 UCHAR      uchMinor,
                 UCHAR      *puchFlag,
                 UCHAR      *puchScaleIn,
                 UCHAR      uchType)
{
    SHORT   sReturn;
    USHORT  usPercent = 0;
    UCHAR   uchAdjNo,
            uchNo,
            uchPluProhibitVal;
    UCHAR   uchStatus = RPT_DECIMAL_0;           /* to store Decimal point status */
	LTOTAL  AdjTtl = {0};
    TCHAR   AdjMnemo[PARA_ADJMNEMO_LEN + 1] = {0};
    TCHAR   SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};
    PLUIF   PluFile_IF = {0};   /* to get unit price of Adj2 to Adj5 */
                                 /* other data is used in pPluIf_Rep structure */

    if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG1) {          /* Group 1 */
        uchNo = ADJ_GP1VAR1_ADR;
    } else if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG2) {   /* Group 2 */
        uchNo = ADJ_GP2VAR1_ADR;
    } else if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG3) {   /* Group 3 */
        uchNo = ADJ_GP3VAR1_ADR;
    } else {                                                                     /* Group 4 */
        uchNo = ADJ_GP4VAR1_ADR;
    }

    /*----- Get Adjective Mnemonics -----*/
    RflGetAdj (AdjMnemo, uchNo);                                        /* Get Adjective Mnemonics */

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemo LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS and MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);                   /* Calculate Percent */
    }
    uchPluProhibitVal = PLU_PROHIBIT_VAL1;
    if (RptPLUPrintChk2(pTtlPlu, pPluIf->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) == 0) { 
        if (*puchFlag == 0x00)  {                        /* The 1st Adjective ? */
            RptFeed(RPT_DEFALTFEED);                     /* Line Feed */
        }

        RptPLUPrint(uchMinor,                            /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                  /* TOTAL- lAmount & lCounter */ 
                    pPluIf->auchPluNo,               /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,     /* PLU Mnemonics */            
                    SpeMnemo,                        /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                        /* Ajective Mnemoincs */
                    pPluIf->ParaPlu.auchPrice,       /* PresetPrice */       
                    usPercent,                           /* Percent */             
                    uchStatus,                           /* Decimal point status */
                    uchType);                            /* Report Type */
        if (UieReadAbortKey() == UIE_ENABLE) {
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED) {
            return(RPT_ABORTED);
        }

        *puchFlag |= 0x01;
        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else {                                                /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }

    } else {
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    }
    AdjTtl.lAmount = pTtlPlu->PLUTotal.lAmount;                             /* Add lAmount to Adjective Ttl */
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter;                       /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 100L;            /* Adjust Amt to Wgt Amt */
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;             /* Adjust Amt to Wgt Amt */   
        }
    }

    /*----- loop 2nd to 5th Adjective -----*/
    /*****======================================================================*****/
    /* Using PLU Parameter File from Address3 to Address10 that are set in function */
    /* RptPLUCode if PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status).    */
    /* In other case, using PLU Parameter File Address6 BIT2                        */
    /* The reason to copy from pPluIf_Dep to PluFile_Dep is to get each Adjective   */
    /* unit Price                                                                   */
    /*****======================================================================*****/


    PluFile_IF = *pPluIf;   /* Copy to other work Buffer  */
    _tcsncpy(PluFile_IF.auchPluNo,pTtlPlu->auchPLUNumber,OP_PLU_LEN);/*checks PLU label*/

                                                             /* to save copied Status */
    for (uchAdjNo = uchNo+(UCHAR)1; uchAdjNo < uchNo+(UCHAR)5; uchAdjNo++) {      
        PluFile_IF.uchPluAdj++;                 /* Next Adjective for other work Buffer */
        if ((sReturn = SerOpPluAllRead(&PluFile_IF, 0)) != OP_PERFORM) { /* Skip if file not found */
            return(OpConvertError(sReturn));
        }
        
        /*----- to store new Adjective unit price -----*/
        memcpy(pPluIf->ParaPlu.auchPrice, PluFile_IF.ParaPlu.auchPrice, sizeof(pPluIf->ParaPlu.auchPrice));  

        memset(&pTtlPlu->PLUTotal, 0, sizeof(pTtlPlu->PLUTotal));           /* Clear Total */
        _tcsncpy(pTtlPlu->auchPLUNumber, PluFile_IF.auchPluNo, OP_PLU_LEN);
        pTtlPlu->uchAdjectNo = PluFile_IF.uchPluAdj;                       /* Set Adjective */
        if (((sReturn = SerTtlTotalRead(pTtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
            return(TtlConvertError(sReturn));
        }
        uchPluProhibitVal = (UCHAR)(uchPluProhibitVal << 1);                    /* Shift right 1 bit in each time */
        if (RptPLUPrintChk2(pTtlPlu, pPluIf->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) != 0) { 
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
            continue;
        }

        /*----- Get Adjective Mnemonics -----*/
        RflGetAdj (AdjMnemo, uchAdjNo);                                     /* Get Adjective Mnemonics */

        AdjTtl.lAmount += pTtlPlu->PLUTotal.lAmount;                        /* Add lAmount to Adjective Ttl */
        if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&  /* Plus item PLU ? */
            !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
             (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) { /* Scalable ? */
            AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter;                  /* Store Wgt Amt /w Decimal point */
        } else{                                                             
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* !Scalable - MDC #89 BIT 2 is 0 */
                AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 100L;       /* Adjust Amt to Wgt Amt */
            } else {                                                        /* !Scalable - MDC #89 BIT 2 is 1 */  
               AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 10L;         /* Adjust Amt to Wgt Amt */   
            }
        }

        /*----- Calculate Percent based on MDC -----*/
        if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&  /* NORMAL TYPE ? */
           !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {

            usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);               /* Calculate Percent */
        }                                                                   /* the 1st Adjective ? */
        if (*puchFlag == 0x00) {                                            /* Line Feed */
            RptFeed(RPT_DEFALTFEED);
        }
        RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                    pPluIf->auchPluNo,              /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    SpeMnemo,                       /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                       /* Adjective Mnemoincs */
                    pPluIf->ParaPlu.auchPrice,      /* PresetPrice */       
                    usPercent,                          /* Percent */             
                    uchStatus,                          /* Decimal point status */
                    uchType);                           /* Report Type */

        *puchFlag |= 0x01;
        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else if (uchMinor == CLASS_RPTPLU_PRTTTLMINUSADJ1) {  /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
    } /*----- End of For Loop -----*/

    pTtlPlu->PLUTotal = AdjTtl;
    *pPluIf = PluFile_IF;    /* Copy to other work Buffer  */
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptPLUACTOpen(PLUIF     *pPluIf,
*                                  TTLPLU    *pTtlPlu,
*                                  UCHAR     uchMinor,
*                                  UCHAR     *puchFlag,
*                                  UCHAR     *puchScaleIn);
*               
*       Input:  PRECPLU   pRecPlu
*               TTLPLU      *pTtlPlu
*               UCHAR       uchMinor
*               UCHAR       *puchFlag
*               UCHAR       *puchScaleIn
*
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Get all Data for Code Open PLU.
*===========================================================================
*/

VOID  RptPLUACTOpen(PLUIF     *pPluIf,
                    TTLPLU    *pTtlPlu,
                    UCHAR     uchMinor,
                    UCHAR     *puchFlag,
                    UCHAR     *puchScaleIn,
                    UCHAR     uchType)
{
    UCHAR      uchStatus = RPT_DECIMAL_0;
    USHORT     usPercent = 0;
	TCHAR      SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemo LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS and MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);
    }
    if ((pTtlPlu->PLUTotal.lAmount == 0L) && (pTtlPlu->PLUTotal.lCounter == 0L) && /* Total = 0 ? */
        (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT2))) {                /* MDC #89 BIT 2 */
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    } else {
        if (*puchFlag == 0x00) {
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed */    
        }
        switch(uchMinor) {
        case CLASS_RPTPLU_PRTTTLOPEN:                       /* CLASS_RPTPLU_PRTTTLOPEN */ 
            /* break; */
        case CLASS_RPTPLU_PRTTTLCREDIT:                     /* CLASS_RPTPLU_PRTTTLCREDIT */ 
            RptPLUPrint(uchMinor,                           
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */             
                        pPluIf->auchPluNo,              /* PLU Number */
                        pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                       /* Special Mnemonics LB/Kg */             
                        0,                                  /* NOT USE (Ajective Mnemoincs) */              
                        0,                                  /* NOT USE (PresetPrice) */             
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Report Type */
            *puchFlag |= 0x01;
            break;

        default:                                            /* case of CLASS_RPTPLU_PRTTTLPRSET */
            RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLPRSET */ 
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                        pPluIf->auchPluNo,              /* PLU Number */
                        pPluIf->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                       /* Special Mnemonics LB/Kg */     
                        0,                                  /* NOT USE (Ajective Mnemoincs) */
                        pPluIf->ParaPlu.auchPrice,      /* PresetPrice */       
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Report Type */
            *puchFlag |= 0x01;
            break;

        } /*----- End of Switch Loop -----*/
    }
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter;            /* Store Wgt Amt /w Decimal point */
    } else {
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */ 
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 100L; /* Adjust Amt to Wgt Amt */       
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */ 
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;  /* Adjust Amt to Wgt Amt */         
        }                                                                      
    }
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUACTAdj(PLUIF     *pPluIf,
*                                  TTLPLU     *pTtlPlu,    
*                                  UCHAR      uchMinor,
*                                  UCHAR      *puchFlag,
*                                  UCHAR      *puchScaleIn);
*               
*       Input:  PRECPLU    pRecPlu
*               TTLPLU     *pTtlPlu    
*               UCHAR      uchMinor 
*               UCHAR      *puchFlag
*               UCHAR      *puchScaleIn
*
*      Output:  nothing
*
**     Return:  LDT_PROHBT_ADR  : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR   : (OP_PLU_FULL)
*               LDT_NTINFL_ADR  : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR    : (OP_LOCK)
*
**  Description:  
*       Get all 5 Adjective Data.  Add to Total-lAmount and lCounter.
*===========================================================================
*/

SHORT  RptPLUACTAdj(PLUIF     *pPluIf,
                    TTLPLU     *pTtlPlu,
                    UCHAR      uchMinor,
                    UCHAR      *puchFlag,
                    UCHAR      *puchScaleIn,
                    UCHAR      uchType)
{
    USHORT  usPercent = 0;
    UCHAR   uchNo,
            uchPluProhibitVal;
    UCHAR   uchStatus = RPT_DECIMAL_0;           /* to store Decimal point status */
	TCHAR   AdjMnemo[PARA_ADJMNEMO_LEN + 1] = {0};
    TCHAR   SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};

    if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG1) {          /* Group 1 */
        uchNo = ADJ_GP1VAR1_ADR;
    } else if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG2) {   /* Group 2 */
        uchNo = ADJ_GP2VAR1_ADR;
    } else if (pPluIf->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG3) {   /* Group 3 */
        uchNo = ADJ_GP3VAR1_ADR;
    } else {                                                                     /* Group 4 */
        uchNo = ADJ_GP4VAR1_ADR;
    }

    /*----- Get Adjective Mnemonics -----*/ 
    uchNo += pTtlPlu->uchAdjectNo - 1;
    RflGetAdj(AdjMnemo, uchNo);                                                    /* Get Adjective Mnemonics */

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                              /* Get Mnemo LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS and MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);                   /* Calculate Percent */
    }

    if (pTtlPlu->uchAdjectNo == 1) {
        uchPluProhibitVal = PLU_PROHIBIT_VAL1;
    } else {
        uchPluProhibitVal = PLU_PROHIBIT_VAL1;
        uchPluProhibitVal = (UCHAR)(uchPluProhibitVal << pTtlPlu->uchAdjectNo);
    }

    if (RptPLUPrintChk2(pTtlPlu, pPluIf->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) == 0) { 
        if (*puchFlag == 0x00)  {                        /* The 1st Adjective ? */
            RptFeed(RPT_DEFALTFEED);                     /* Line Feed */
        }
        RptPLUPrint(uchMinor,                            /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                  /* TOTAL- lAmount & lCounter */ 
                    pPluIf->auchPluNo,                   /* PLU Number */
                    pPluIf->ParaPlu.auchPluName,         /* PLU Mnemonics */            
                    SpeMnemo,                            /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                            /* Ajective Mnemoincs */
                    pPluIf->ParaPlu.auchPrice,           /* PresetPrice */       
                    usPercent,                           /* Percent */             
                    uchStatus,                           /* Decimal point status */
                    uchType);                            /* Report Type */

        *puchFlag |= 0x01;

        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else {                                                /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
    } else {
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    }

/*    AdjTtl.lAmount = pTtlPlu->PLUTotal.lAmount;                             /* Add lAmount to Adjective Ttl */

    if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter;                       /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 100L;
/*            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 100L;            /* Adjust Amt to Wgt Amt */
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;
/*            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;             /* Adjust Amt to Wgt Amt */   
        }
    }

/*    memcpy(pPluIf, &PluFile_IF, sizeof(PLUIF_REP));    /* Copy to other work Buffer  */

    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUEditByDEPT(UCHAR  uchMinorClass,
*                                      UCHAR  uchType,
*                                      USHORT usDEPTNumber,
*                                      UCHAR  uchResetType)
*               
*       Input:  UCHAR   uchMinorClass :
*               UCHAR   uchType       : RPT_DEPT_READ
*                                     : RPT_ALL_READ
*                                     : RPT_EOD_ALLREAD
*                                     : RPT_PTD_ALLREAD 
*               UCHAR   uchDEPTNumber :
*               UCHAR   uchResetType  : 0
*                                     : RPT_ONLY_PRT_RESET
*                                     : RPT_PRT_RESET
*
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               LDT_KEYOVER_ADR  :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
*
**  Description:  
*       Edit PLU by DEPT individualy or all.
*===========================================================================
*/
SHORT   RptPLUEditByDEPT(UCHAR uchMinorClass, UCHAR uchType, USHORT usDEPTNumber, UCHAR uchResetType)
{
    USHORT           usStatus;
    UCHAR            uchTtlFlag = 0;    /* To store uchFlag Bit */
    SHORT            sReturn;
    LTOTAL           Total = {0};
    PLUIF_DEP        PluIf_Dep = {0};
	DEPTIF           DeptIf = {0};

    /* saratoga */
    DeptIf.usDeptNo = usDEPTNumber;                                             /* Set DEPT No */
    if ((sReturn = CliOpDeptIndRead(&DeptIf, 0)) != OP_PERFORM) {               /* Get DEPT Status */
        sReturn = OpConvertError(sReturn);
        return(sReturn);
    }

    /* Pre-Set for Checking PLU File Exsit */
    PluIf_Dep.usDept = usDEPTNumber;
    PluIf_Dep.culCounter = 0L;
    sReturn = SerOpDeptPluRead (&PluIf_Dep, 0x1000);
    if ((sReturn == OP_EOF) || (sReturn == OP_NOT_IN_FILE)) {
        return(LDT_NTINFL_ADR);
    }

    uchRptScaleTtl = 0;                 /* Initialize Total Flag of PLUS and MINUS Total */
    /*uchTtlFlag = 0;*/
    for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++) {       /* Loop 3 times */
		UCHAR      uchFlag = 0;       /* To Check item is existed */

		if ((sReturn = RptPLUByDEPT(uchMinorClass, uchType, usStatus, usDEPTNumber, &Total, &uchFlag, uchResetType)) != SUCCESS) {
            if ((uchRptOpeCount & RPT_HEADER_PRINT) && (sReturn != RPT_ABORTED)) {
                RptPrtError(sReturn);
            }
            return(sReturn);
        }
        uchTtlFlag |= uchFlag;
        if ((usStatus == RPT_CREDIT) && (uchTtlFlag != 0)){  /* Credit DEPT ?     */
            RptFeed(RPT_DEFALTFEED);                                    /* Line Feed         */
                                                                        /* Print Grand Total */
            RptPLUGrandTtlPrt(TRN_GTTLR_ADR, &Total, usStatus, uchRptScaleTtl, uchType);
        }
        if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?      */
            return(RPT_ABORTED);                       
        }                                                         
        if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
        }
    } /*----- End of For Loop -----*/

    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  SHORT    RptPLUByPLU(UCHAR  uchMinorClass,
*                                  UCHAR  uchType,
*                                  USHORT usFile,
*                                  UCHAR  uchResetType)
*               
*       Input:  UCHAR      uchMinorClass  :
*               UCHAR      uchType        : 
*               USHORT     usFile         :
*               UCHAR      uchResetType   :
*
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
**  Description:  
*       Check Error before printing HEADER.  If there is an Error, 
*       it prints Error on R/J papers.  Classify DEPT to OPEN, 
*       CREDIT or HASH. Print Sub Total.                        Saratoga
*===========================================================================
*/
SHORT   RptPLUByPLU(UCHAR uchMinorClass, UCHAR uchType, 
                    /*USHORT usFile,*/ UCHAR uchResetType, USHORT usStatus,
                    UCHAR * puchFlg, DCURRENCY * plSubTotal, LONG * plSubCounter,
                    UCHAR * puchScaleOut, PLUIF * pPluIf)
{
    SHORT               sReturn;
    UCHAR               uchScaleIn = 0;      /* To Check scalable item is existed */
    USHORT              usDEPT;
    UCHAR               uchPLUType;
    TTLPLU              TtlPlu = {0};
    RECPLU              RecPlu = {0};
	DEPTIF              DeptIf = {0};
    TCHAR               auchPluNo[NUM_PLU_LEN] = {0};

    if (uchRptMldAbortStatus) {
		/* aborted by MLD */
        return (RPT_ABORTED);
    }
    
    if((sReturn = SerOpPluAllRead(pPluIf, 0)) != OP_PERFORM) {
        return(sReturn); 
    }

    TtlPlu.uchMajorClass = CLASS_TTLPLU;
    TtlPlu.uchMinorClass = uchMinorClass;
    _tcsncpy(TtlPlu.auchPLUNumber, pPluIf->auchPluNo, OP_PLU_LEN);
    TtlPlu.uchAdjectNo = pPluIf->uchPluAdj;
    if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
        return(TtlConvertError(sReturn));
    }

    _tcsncpy(auchPluNo, pPluIf->auchPluNo, NUM_PLU_LEN);  /* save plu no, before convert */
    RflConvertPLU(pPluIf->auchPluNo,TtlPlu.auchPLUNumber);
    RptPLUHeader(uchMinorClass, uchType, pPluIf->auchPluNo, uchResetType);
    _tcsncpy(pPluIf->auchPluNo, auchPluNo, NUM_PLU_LEN);  /* recover plu no */
    if (uchType == RPT_DEPT_READ) {
        RptFeed(RPT_DEFALTFEED);
    }

/*    RptFeed(RPT_DEFALTFEED);                                            / Line Feed       */
    if(RptPauseCheck() == RPT_ABORTED) {
		/* aborted by MLD */
        return(RPT_ABORTED);
    }

    /*----- PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status) -----*/
    usDEPT = pPluIf->ParaPlu.ContPlu.usDept;
                                                                                    /* Wipe out PLU Type */
    DeptIf.usDeptNo = usDEPT;                                                      /* Set DEPT No */
    sReturn = CliOpDeptIndRead(&DeptIf, 0);                                        /* Get DEPT Status */
    if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE)) {
        sReturn = OpConvertError(sReturn);
        RptPrtError(sReturn);
        return(sReturn);
    }

    if (sReturn == OP_PERFORM) {
        if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL) {          /* Use Dept Status ? */
            pPluIf->ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];    /* Copy Bit Status 0 */
            pPluIf->ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];    /* Copy Bit Status 1 */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];    /* Copy Bit Status 2 */
            pPluIf->ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];    /* Copy Bit Status 3 */
            pPluIf->ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];    /* Copy Bit Status 5 */
        } else {   /* Copy DEPT Parameter file Address 6 BIT2 to PLU Parameter file Address 7 BIT2 */
            DeptIf.ParaDept.auchControlCode[2] &= PLU_HASH;                /* Clear 0x04 in ...Code */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;                            /* Clear 0xfb in ...Code */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] |= DeptIf.ParaDept.auchControlCode[2];    /* Copy Bit Status 2 */
        }
    }

    /*----- Clear 0xc0 in PluIf_Rep...uchDEPT -----*/
    uchPLUType = (UCHAR)(pPluIf->ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);/* Saratoga */

    switch(usStatus) {
    case RPT_OPEN:
         if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&    /* NORMAL 3-TYPE */
                !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH))
		 {
             if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
                 return(TtlConvertError(sReturn));                                                  
             }

             RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */

             if (uchPLUType == PLU_ADJ_TYP) {                                 /* ADJECTIVE TYPE */
                 sReturn = RptPLUAdj(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLADJ1, puchFlg, &uchScaleIn, uchType);
                 if (sReturn != SUCCESS) {
                     return(sReturn);
                 }
             } else if (uchPLUType == PLU_NON_ADJ_TYP) {                        /* PRESET TYPE */
                 RptPLUOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLPRSET, puchFlg, &uchScaleIn, uchType);
             } else {                                                         /* OPEN TYPE */
                 RptPLUOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLOPEN, puchFlg, &uchScaleIn, uchType);
             }
             *plSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
             *plSubCounter += TtlPlu.PLUTotal.lCounter; 
         } else {
             if ((pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                    !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
             {
                 uchRptStatus |= RPT_EXIST_CREDIT;
             }
             else if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)      /* HASH 3-TYPE */
             {
                 uchRptStatus |= RPT_EXIST_HASH;
             }
             uchRptNoPrtCo++;                       /* Count up not Print Counter */
         }
         break;

    case RPT_CREDIT:
        if ((pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
        {           
            if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
                return(TtlConvertError(sReturn));                                                  
            }
            
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
            
            if (uchPLUType == PLU_ADJ_TYP) {                                 /* COUPON TYPE 2 */
                sReturn = RptPLUAdj(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlg, &uchScaleIn, uchType);
                if (sReturn != SUCCESS) {
                    return(sReturn);
                }
            } else if (uchPLUType == PLU_NON_ADJ_TYP) {                        /* COUPON TYPE 1 */
                RptPLUOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlg, &uchScaleIn, uchType);
            } else {                                                        /* CREDIT TYPE */
                RptPLUOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlg, &uchScaleIn, uchType);
            }
            *plSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
            *plSubCounter += TtlPlu.PLUTotal.lCounter;                         
        } else {
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
        }
        break;

    default:                                                                /* case of RPT_HASH */       
        if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {        /* HASH 3-TYPE */
            if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
                return(TtlConvertError(sReturn));                                                  
            }
            
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
            
            if (uchPLUType == PLU_ADJ_TYP) {                                 /* ONE-ADJECTIVE */
                sReturn = RptPLUAdj(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlg, &uchScaleIn, uchType);
                if (sReturn != SUCCESS) {
                    return(sReturn);
                }
            } else if (uchPLUType == PLU_NON_ADJ_TYP) {                      /* NONE-ADJECTIVE */
                RptPLUOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlg, &uchScaleIn, uchType);
            } else {                                                         /* OPEN TYPE  */
                RptPLUOpen(pPluIf,  &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlg, &uchScaleIn, uchType);
            }
            *plSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
            *plSubCounter += TtlPlu.PLUTotal.lCounter;
        } else {
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
        }
        break;
    } /*----- End of Switch Loop -----*/

    if (UieReadAbortKey() == UIE_ENABLE) {
        return(RPT_ABORTED);
    } 
    if(RptPauseCheck() == RPT_ABORTED) {
        return(RPT_ABORTED);
    }
    *puchScaleOut |= uchScaleIn;
    if (uchRptNoPrtCo >= RPTNOPRTCO_MAX) {
        SerTtlReadWriteChkAndWait(RPT_WAIT_TIMER);  /* Sleep */
        uchRptNoPrtCo = 0;
    }

    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  SHORT    RptPLUByACTPLU(UCHAR  uchMinorClass,
*                                  UCHAR  uchType,
*                                  USHORT usFile,
*                                  UCHAR  uchResetType)
*               
*       Input:  UCHAR      uchMinorClass  :
*               UCHAR      uchType        : 
*               USHORT     usFile         :
*               UCHAR      uchResetType   :
*
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
**  Description:  
*       Check Error before printing HEADER.  If there is an Error, 
*       it prints Error on R/J papers.  Classify DEPT to OPEN, 
*       CREDIT or HASH. Print Sub Total.                        Saratoga
*===========================================================================
*/
SHORT   RptPLUByACTPLU(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchResetType, USHORT usStatus,
                       UCHAR * puchFlg, DCURRENCY * plSubTotal, LONG * plSubCounter,
                       UCHAR * puchScaleOut, PLUIF * pPluIf)
{
    UCHAR               uchScaleIn;      /* To Check scalable item is existed */
    UCHAR               uchPLUType;
    SHORT               sReturn;
    USHORT              usDEPT;
    TTLPLU              TtlPlu = {0};
	DEPTIF              DeptIf = {0};
    BOOL                bEof = FALSE;

    if (uchRptMldAbortStatus) {                 
        return (RPT_ABORTED);
    }

    sReturn = TtlPLUGetRec(&TtlPlu);
    if (sReturn != TTL_SUCCESS) {
        if(sReturn == TTL_EOF) {
            bEof = TRUE;
        } else {
            if(sReturn == TTL_FAIL) /* to avoid to duplicate PRT_ABORTED */
            {
                sReturn = LDT_ERR_ADR;
            }
            return sReturn;
        }
    }

    _tcsncpy(pPluIf->auchPluNo,TtlPlu.auchPLUNumber,OP_PLU_LEN);
    pPluIf->uchPluAdj = TtlPlu.uchAdjectNo;
    
    sReturn = CliOpPluIndRead(pPluIf, 0);
    if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE)) {
#if defined(USE_ORIGINAL)
        return(OpConvertError(sReturn));
#else
        /* --- Fix a glitch (2001/06/20)
            The function should not return "TTL_EOF" which is defined as 1.
            It is the same value as LDT_NTINFL_ADR which is returned when
            the PLU record is not found in PARAPLU file.
        --- */
        sReturn = OpConvertError( sReturn );
        if ( bEof && ( sReturn == LDT_NTINFL_ADR )) {
            sReturn = RPT_EOF;
        }
        return ( sReturn );
#endif
    }

    uchScaleIn = 0;

    RflConvertPLU(pPluIf->auchPluNo,TtlPlu.auchPLUNumber);
    RptPLUHeader(uchMinorClass, uchType, pPluIf->auchPluNo, uchResetType);
    if (uchType == RPT_DEPT_READ) {
        RptFeed(RPT_DEFALTFEED);
    }

/*    RptFeed(RPT_DEFALTFEED);                                            / Line Feed       */
    if(RptPauseCheck() == RPT_ABORTED) {
        return(RPT_ABORTED);
    }

    /*----- PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status) -----*/
	if (sReturn == OP_NOT_IN_FILE) {
		memset(&(pPluIf->ParaPlu), 0, sizeof(OPPLU_PARAENTRY));
	} else {
	    usDEPT = pPluIf->ParaPlu.ContPlu.usDept;

    	memset(&DeptIf, 0, sizeof(DeptIf));                                         /* Clear Buffer */
	    DeptIf.usDeptNo = usDEPT;                                                   /* Set DEPT No */
    	sReturn = CliOpDeptIndRead(&DeptIf, 0);                                     /* Get DEPT Status */
	    if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE))
    	{
        	sReturn = OpConvertError(sReturn);
	        RptPrtError(sReturn);
    	    return(sReturn);
    	}
    }

    if (sReturn == OP_PERFORM) {
        if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL) {         /* Use Dept Status ? */
            pPluIf->ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];    /* Copy Bit Status 0 */
            pPluIf->ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];    /* Copy Bit Status 1 */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];    /* Copy Bit Status 2 */
            pPluIf->ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];    /* Copy Bit Status 3 */
            /* --- increase control code at R3.1 --- */
            pPluIf->ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];    /* Copy Bit Status 5 */
        } else {
			/* Copy DEPT Parameter file Address 6 BIT2 to PLU Parameter file Address 7 BIT2 */
            DeptIf.ParaDept.auchControlCode[2] &= PLU_HASH;                                     /* Clear 0x04 in ...Code */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;                            /* Clear 0xfb in ...Code */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] |= DeptIf.ParaDept.auchControlCode[2];   /* Copy Bit Status 2 */
        }
    }
    /*----- Clear 0xc0 in PluIf_Rep...uchDEPT -----*/
    uchPLUType = (UCHAR)(pPluIf->ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);/* Saratoga */

    switch(usStatus) {
    case RPT_OPEN:
         if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&    /* NORMAL 3-TYPE */
                !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
         {
            
             RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
             
             if (uchPLUType == PLU_ADJ_TYP)  {                                /* ADJECTIVE TYPE */
                 sReturn = RptPLUACTAdj(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLADJ1, puchFlg, &uchScaleIn, uchType);
                 if (sReturn != SUCCESS) {
                     return(sReturn);
                 }
             } else if (uchPLUType == PLU_NON_ADJ_TYP) {                        /* PRESET TYPE */
                 RptPLUACTOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLPRSET, puchFlg, &uchScaleIn, uchType);
             } else  {                                                           /* OPEN TYPE */
                 RptPLUACTOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLOPEN, puchFlg, &uchScaleIn, uchType);
             }
             *plSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
             *plSubCounter += TtlPlu.PLUTotal.lCounter; 
         } else {
             if ((pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                    !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
             {
                 uchRptStatus |= RPT_EXIST_CREDIT;
             } else if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {     /* HASH 3-TYPE */
                 uchRptStatus |= RPT_EXIST_HASH;
             }
             uchRptNoPrtCo++;                       /* Count up not Print Counter */
         }
         break;

    case RPT_CREDIT:
        if ((pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
        {           
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
             
            if (uchPLUType == PLU_ADJ_TYP)  {                                /* COUPON TYPE 2 */
                sReturn = RptPLUACTAdj(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlg, &uchScaleIn, uchType);
                if (sReturn != SUCCESS) {
                    return(sReturn);
                }
            } else if (uchPLUType == PLU_NON_ADJ_TYP)  {                       /* COUPON TYPE 1 */
                RptPLUACTOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlg, &uchScaleIn, uchType);
            } else {                                                        /* CREDIT TYPE */
            
                RptPLUACTOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlg, &uchScaleIn, uchType);
            }
            *plSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
            *plSubCounter += TtlPlu.PLUTotal.lCounter;                         
        } else {
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
        }
        break;

    default:                                                                /* case of RPT_HASH */       
        if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)         /* HASH 3-TYPE */
        {                                                                             
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
             
            if (uchPLUType == PLU_ADJ_TYP)  {                                /* ONE-ADJECTIVE */
                sReturn = RptPLUACTAdj(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlg, &uchScaleIn, uchType);
                if (sReturn != SUCCESS) {
                    return(sReturn);
                }
            } else if (uchPLUType == PLU_NON_ADJ_TYP) {                      /* NONE-ADJECTIVE */
                RptPLUACTOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlg, &uchScaleIn, uchType);
            } else {                                                         /* OPEN TYPE  */
                RptPLUACTOpen(pPluIf, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlg, &uchScaleIn, uchType);
            }
            *plSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
            *plSubCounter += TtlPlu.PLUTotal.lCounter;
        } else {
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
        }
        break;
    } /*----- End of Switch Loop -----*/

    if (UieReadAbortKey() == UIE_ENABLE) {
        return(RPT_ABORTED);
    } 
    if(RptPauseCheck() == RPT_ABORTED) {
        return(RPT_ABORTED);
    }
    *puchScaleOut |= uchScaleIn;
    if (uchRptNoPrtCo >= RPTNOPRTCO_MAX) {
        SerTtlReadWriteChkAndWait(RPT_WAIT_TIMER);  /* Sleep */
        uchRptNoPrtCo = 0;
    }

    if(bEof) {
        bEof = FALSE;
#if defined (USE_ORIGINAL)
        return (TTL_EOF);
#else
        /* --- Fix a glitch (2001/06/20)
            The function should not return "TTL_EOF" which is defined as 1.
            It is the same value as LDT_NTINFL_ADR which is returned when
            the PLU record is not found in PARAPLU file.
        --- */
        return (RPT_EOF);
#endif
	} else {
        return(SUCCESS);
	}
}
/*
*===========================================================================
**  Synopsis:  SHORT    RptPLUByACTPLU(UCHAR  uchMinorClass,
*                                  UCHAR  uchType,
*                                  USHORT usFile,
*                                  UCHAR  uchResetType)
*               
*       Input:  UCHAR      uchMinorClass  :
*               UCHAR      uchType        : 
*               USHORT     usFile         :
*               UCHAR      uchResetType   :
*
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
**  Description:  
*       Check Error before printing HEADER.  If there is an Error, 
*       it prints Error on R/J papers.  Classify DEPT to OPEN, 
*       CREDIT or HASH. Print Sub Total.                        Saratoga
*===========================================================================
*/
SHORT   RptPLUByACTPLUSat(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchResetType, USHORT usStatus,
                       UCHAR * puchFlg, DCURRENCY * plSubTotal, LONG * plSubCounter,
                       UCHAR * puchScaleOut, PLUIF * pPluIf, TTLPLU *ttlPlu)
{
    SHORT               sReturn;
    UCHAR               uchScaleIn = 0;      /* To Check scalable item is existed */
    USHORT              usDEPT;
    UCHAR               uchPLUType;
	DEPTIF              DeptIf = {0};
    BOOL                bEof = FALSE;

    if (uchRptMldAbortStatus) {                 
        return (RPT_ABORTED);
    }

    RptPLUHeader(uchMinorClass, uchType, pPluIf->auchPluNo, uchResetType);
    if (uchType == RPT_DEPT_READ) {
        RptFeed(RPT_DEFALTFEED);
    }

/*    RptFeed(RPT_DEFALTFEED);                                            / Line Feed       */
    if(RptPauseCheck() == RPT_ABORTED) {                                 /* aborted by MLD */
        return(RPT_ABORTED);
    }

    /*----- PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status) -----*/
	    usDEPT = pPluIf->ParaPlu.ContPlu.usDept;

	    DeptIf.usDeptNo = usDEPT;                                                   /* Set DEPT No */
    	sReturn = CliOpDeptIndRead(&DeptIf, 0);                                     /* Get DEPT Status */
	    if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE)) {
        	sReturn = OpConvertError(sReturn);
	        RptPrtError(sReturn);
    	    return(sReturn);
    	}

    if (sReturn == OP_PERFORM) {
        if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL) {
            pPluIf->ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];    /* Copy Bit Status 0 */
            pPluIf->ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];    /* Copy Bit Status 1 */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];    /* Copy Bit Status 2 */
            pPluIf->ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];    /* Copy Bit Status 3 */
            /* --- increase control code at R3.1 --- */
            pPluIf->ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];    /* Copy Bit Status 5 */
        } else {
			/* Copy DEPT Parameter file Address 6 BIT2 to PLU Parameter file Address 7 BIT2 */
            DeptIf.ParaDept.auchControlCode[2] &= PLU_HASH;                                     /* Clear 0x04 in ...Code */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;                            /* Clear 0xfb in ...Code */
            pPluIf->ParaPlu.ContPlu.auchContOther[2] |= DeptIf.ParaDept.auchControlCode[2];   /* Copy Bit Status 2 */
        }
    }
    /*----- Clear 0xc0 in PluIf_Rep...uchDEPT -----*/
    uchPLUType = (UCHAR)(pPluIf->ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);/* Saratoga */

    switch(usStatus) {
    case RPT_OPEN:
         if (!(pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&    /* NORMAL 3-TYPE */
                !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
         {
             RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
             
             if (uchPLUType == PLU_ADJ_TYP) {                                 /* ADJECTIVE TYPE */
                 sReturn = RptPLUACTAdj(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLADJ1, puchFlg, &uchScaleIn, uchType);
                 if (sReturn != SUCCESS) {
                     return(sReturn);
                 }
             } else if (uchPLUType == PLU_NON_ADJ_TYP) {                        /* PRESET TYPE */
                 RptPLUACTOpen(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLPRSET, puchFlg, &uchScaleIn, uchType);
             } else {                                                            /* OPEN TYPE */
                 RptPLUACTOpen(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLOPEN, puchFlg, &uchScaleIn, uchType);
             }
             *plSubTotal += ttlPlu->PLUTotal.lAmount;                           /* Add to Sub Total */
             *plSubCounter += ttlPlu->PLUTotal.lCounter; 
         } else {
             if ((pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                    !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
             {
                 uchRptStatus |= RPT_EXIST_CREDIT;
             } else if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {     /* HASH 3-TYPE */
                 uchRptStatus |= RPT_EXIST_HASH;
             }
             uchRptNoPrtCo++;                       /* Count up not Print Counter */
         }
         break;

    case RPT_CREDIT:
        if ((pPluIf->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                !(pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) 
        {           
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
             
            if (uchPLUType == PLU_ADJ_TYP) {                                 /* COUPON TYPE 2 */
                sReturn = RptPLUACTAdj(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlg, &uchScaleIn, uchType);
                if (sReturn != SUCCESS) 
                {
                    return(sReturn);
                }
            } else if (uchPLUType == PLU_NON_ADJ_TYP) {                        /* COUPON TYPE 1 */
                RptPLUACTOpen(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlg, &uchScaleIn, uchType);
            } else  {                                                       /* CREDIT TYPE */
                RptPLUACTOpen(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlg, &uchScaleIn, uchType);
            }
            *plSubTotal += ttlPlu->PLUTotal.lAmount;                           /* Add to Sub Total */
            *plSubCounter += ttlPlu->PLUTotal.lCounter;                         
        } else {
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
        }
        break;

    default:                                                                /* case of RPT_HASH */       
        if (pPluIf->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {        /* HASH 3-TYPE */
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed       */
             
            if (uchPLUType == PLU_ADJ_TYP) {                                 /* ONE-ADJECTIVE */
                sReturn = RptPLUACTAdj(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlg, &uchScaleIn, uchType);
                if (sReturn != SUCCESS) {
                    return(sReturn);
                }
            } else if (uchPLUType == PLU_NON_ADJ_TYP) {                      /* NONE-ADJECTIVE */
                RptPLUACTOpen(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlg, &uchScaleIn, uchType);
            } else  {                                                        /* OPEN TYPE  */
                RptPLUACTOpen(pPluIf, ttlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlg, &uchScaleIn, uchType);
            }
            *plSubTotal += ttlPlu->PLUTotal.lAmount;                           /* Add to Sub Total */
            *plSubCounter += ttlPlu->PLUTotal.lCounter;
        } else {
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
        }
        break;
    } /*----- End of Switch Loop -----*/

    if (UieReadAbortKey() == UIE_ENABLE) {
        return(RPT_ABORTED);
    } 
    if(RptPauseCheck() == RPT_ABORTED) {
        return(RPT_ABORTED);
    }
    *puchScaleOut |= uchScaleIn;
    if (uchRptNoPrtCo >= RPTNOPRTCO_MAX) {
        SerTtlReadWriteChkAndWait(RPT_WAIT_TIMER);  /* Sleep */
        uchRptNoPrtCo = 0;
    }

    if(bEof) {
        bEof = FALSE;
#if defined (USE_ORIGINAL)
        return (TTL_EOF);
#else
        /* --- Fix a glitch (2001/06/20)
            The function should not return "TTL_EOF" which is defined as 1.
            It is the same value as LDT_NTINFL_ADR which is returned when
            the PLU record is not found in PARAPLU file.
        --- */
        return (RPT_EOF);
#endif
    } else {
        return(SUCCESS);
	}
}
/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUByDEPT(UCHAR  uchMinorClass,
*                                  UCHAR  uchType,
*                                  USHORT usStatus,
*                                  USHORT usDEPTNo,
*                                  LTOTAL *Total,
*                                  UCHAR  *puchFlag,
*                                  UCHAr  uchResetType);
*               
*       Input:  UCHAR      uchMinorClass  :
*               UCHAR      uchType        : 
*               USHORT     usStatus       :
*               UCHAR      uchDEPTNo      :
*               LTOTAL     *Total         :    
*               UCHAR      *puchFlag      :
*               UCHAR      uchResetType   :
*
*      Output:  nothing
*
**     Return:  SUCCESS          :
*               RPT_ABORTED      :
*               LDT_PROHBT_ADR   : (OP_FILE_NOT_FOUND)
*               LDT_FLFUL_ADR    : (OP_FLU_FULL)
*               LDT_NTINFL_ADR   : (OP_NOT_IN_FILE)
*               LDT_LOCK_ADR     : (OP_LOCK)
*               ???????          : (OP_EOF) ONLY INDIVIDUAL
*               LDT_ERR_ADR      : (TTL_FAIL)
*               LDT_LOCK_ADR     : (TTL_LOCKED) 
*
**  Description:  
*       Check Error before printing HEADER.  If there is an Error, it prints Error
*       on R/J papers.  Classify DEPT to OPEN, CREDIT or HASH.
*       Print Sub Total.
*===========================================================================
*/
SHORT   RptPLUByDEPT(UCHAR uchMinorClass, UCHAR uchType, USHORT usStatus, 
                     USHORT usDEPTNo, LTOTAL *Total, UCHAR *puchFlag, UCHAR uchResetType)
{
    UCHAR            uchPLUType,
                     uchScaleOut;     /* To Store uchScaleIn Bit */
    SHORT            sReturn;
    DCURRENCY        lSubTotal;
    LONG             lSubCounter;
    TTLPLU           TtlPlu = {0};
    PLUIF_DEP        PluIf_Dep = {0};
    
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    if ((usStatus == RPT_CREDIT) && !(uchRptStatus & RPT_EXIST_CREDIT)) {
        return(SUCCESS);
    } else if ((usStatus == RPT_HASH) && !(uchRptStatus & RPT_EXIST_HASH)) {
        return(SUCCESS);
    }

    /*----- Initialize Data -----*/
    PluIf_Dep.usDept = usDEPTNo;

    /*----- Check Error -----*/
    if (uchType == RPT_DEPT_READ) {
        if ((sReturn = SerOpDeptPluRead(&PluIf_Dep, 0x1000)) != OP_PERFORM) {
            return(OpConvertError(sReturn));
        }
    } else {
        if ((sReturn = SerOpDeptPluRead(&PluIf_Dep, 0x1000)) != OP_PERFORM) {
            if ((sReturn == OP_EOF) || (sReturn == OP_NOT_IN_FILE)) {
                return(SUCCESS);
            } else {
                return(OpConvertError(sReturn));
            }
        }
    }            
    TtlPlu.uchMajorClass = CLASS_TTLPLU;                                    /* Set Major for Ttl */
    TtlPlu.uchMinorClass = uchMinorClass;                                   /* Set Minor for Ttl */
    _tcsncpy(TtlPlu.auchPLUNumber, PluIf_Dep.auchPluNo, OP_PLU_LEN);
    RflConvertPLU(PluIf_Dep.auchPluNo,TtlPlu.auchPLUNumber);
    TtlPlu.uchAdjectNo = PluIf_Dep.uchPluAdj;                               /* Adjective must be 1 */
    if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {                                     
        return(TtlConvertError(sReturn));                                                  
    }
    RptPLUHeader(uchMinorClass, uchType, PluIf_Dep.auchPluNo, uchResetType);
    _tcsncpy(PluIf_Dep.auchPluNo,TtlPlu.auchPLUNumber, OP_PLU_LEN);
    if ((uchType == RPT_DEPT_READ) && (usStatus == RPT_OPEN)) {
        RptFeed(RPT_DEFALTFEED);
        RptDEPTNamePrt(usDEPTNo, uchType);
    }

    lSubTotal = 0L;
    lSubCounter = 0L;
    uchScaleOut = 0;
    PluIf_Dep.culCounter = 0L;                                                  /* Set Counter */
    while ((sReturn = SerOpDeptPluRead(&PluIf_Dep, 0)) == OP_PERFORM) {
		DEPTIF      DeptIf = {0};
		UCHAR       uchScaleIn = 0;      /* To Check scalable item is existed */

        memset(&TtlPlu.PLUTotal, 0, sizeof(TtlPlu.PLUTotal));
        _tcsncpy(TtlPlu.auchPLUNumber, PluIf_Dep.auchPluNo, OP_PLU_LEN);
        TtlPlu.uchAdjectNo = PluIf_Dep.uchPluAdj;                               /* Adjective must be 1 */
                                                                                /* Clear 0xc0 in ...uchDEPT */
        /*----- PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status) -----*/
        DeptIf.usDeptNo = usDEPTNo;
        sReturn = CliOpDeptIndRead(&DeptIf, 0);                                 /* Get DEPT Status */
        if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE)) {
            return(OpConvertError(sReturn));
        }

        if (sReturn == OP_PERFORM) {
            if (PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] & PLU_USE_DEPTCTL) {     /* Use Dept Status ? */
                                                                                    /* Copy Bit Status 0 */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];
                                                                                    /* Copy Bit Status 1 */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];
                                                                                    /* Copy Bit Status 2 */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];
                                                                                    /* Copy Bit Status 3 */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];
                /* --- increase control code at R3.1 --- */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];

            } else {   /* Copy DEPT Parameter file Address 6 BIT2 to PLU Parameter file Address 7 BIT2 */
                                                                                    /* Clear 0x04 in ...Code */
                DeptIf.ParaDept.auchControlCode[2] &= PLU_HASH;
                                                                                    /* Clear 0xfb in ...Code */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;
                                                                                    /* Copy Bit Status 2 */
                PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] |= DeptIf.ParaDept.auchControlCode[2];
            }
        }
                                                                                /* Clear 0xc0 in ...uchDEPT */
        uchPLUType = (UCHAR)(PluIf_Dep.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG);/* Saratoga */
        switch(usStatus) {
        case RPT_OPEN:         
            if (!(PluIf_Dep.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&    /* NORMAL 3-TYPE */
                !(PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {

                if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) &&          
                   (sReturn != TTL_NOTINFILE)) {                                     
                    return(TtlConvertError(sReturn));                                                  
                }
                if (uchPLUType == PLU_ADJ_TYP) {                                /* ADJECTIVE TYPE */  
                    sReturn = RptPLUByDEPTAdj(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLADJ1, puchFlag, uchType, usDEPTNo, &uchScaleIn);    
                    if (sReturn != SUCCESS) {
                        return(sReturn);
                    }
                } else if (uchPLUType == PLU_NON_ADJ_TYP) {                     /* PRESET TYPE */ 
                    RptPLUByDEPTOpen(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLPRSET, puchFlag, uchType, usDEPTNo, &uchScaleIn);
                } else {   
                    RptPLUByDEPTOpen(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLOPEN, puchFlag, uchType, usDEPTNo, &uchScaleIn);
                }
                lSubTotal += TtlPlu.PLUTotal.lAmount;                           /* Add to Sub Total */
                lSubCounter += TtlPlu.PLUTotal.lCounter;

            } else {
                if ((PluIf_Dep.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&     /* MINUS 3-TYPE */
                    !(PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {   
                    uchRptStatus |= RPT_EXIST_CREDIT;
                } else if (PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {        /* HASH 3-TYPE */
                    uchRptStatus |= RPT_EXIST_HASH;
                }
                uchRptNoPrtCo++;                       /* Count up not Print Counter */
            }
            break;
                                                                             
        case RPT_CREDIT:        
            if ((PluIf_Dep.ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) && /* MINUS 3-TYPE */
                !(PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {   

                if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) &&          
                   (sReturn != TTL_NOTINFILE)) {                                     
                    return(TtlConvertError(sReturn));                                                  
                }
                if (uchPLUType == PLU_ADJ_TYP) {                            /* COUPON TYPE 2, ONE-ADJECTIVE */
                    sReturn = RptPLUByDEPTAdj(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlag, uchType, usDEPTNo, &uchScaleIn);    
                    if (sReturn != SUCCESS) {
                        return(sReturn);
                    }
                } else if (uchPLUType == PLU_NON_ADJ_TYP) {                 /* COUPON TYPE 1, NONE-ADJECTIVE */
                    RptPLUByDEPTOpen(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlag, uchType, usDEPTNo, &uchScaleIn);  
                } else {                                                    /* CREDIT TYPE */ 
                    RptPLUByDEPTOpen(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlag, uchType, usDEPTNo, &uchScaleIn);       
                }
                lSubTotal += TtlPlu.PLUTotal.lAmount;                       /* Add to Sub Total */
                lSubCounter += TtlPlu.PLUTotal.lCounter;
            } else {
                uchRptNoPrtCo++;                       /* Count up not Print Counter */
            }                                                                 
            break;
                                                                              
        default:                                                            /* case of RPT_HASH */
            if (PluIf_Dep.ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) {    /* HASH 3-TYPE */

                if (((sReturn = SerTtlTotalRead(&TtlPlu)) != TTL_SUCCESS) &&          
                   (sReturn != TTL_NOTINFILE)) {                                     
                    return(TtlConvertError(sReturn));                                                  
                }
                if (uchPLUType == PLU_ADJ_TYP) {                            /* ADJECTIVE, NONE-ADJECTIVE */
                    sReturn = RptPLUByDEPTAdj(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLMINUSADJ1, puchFlag, uchType, usDEPTNo, &uchScaleIn);      
                    if (sReturn != SUCCESS) {
                        return(sReturn);
                    }
                } else if (uchPLUType == PLU_NON_ADJ_TYP) {                 /* COUPON, ONE-ADJECTIVE */
                    RptPLUByDEPTOpen(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLCOUPON, puchFlag, uchType, usDEPTNo, &uchScaleIn);       
                } else {                                                    /* CREDIT */    
                    RptPLUByDEPTOpen(&PluIf_Dep, &TtlPlu, CLASS_RPTPLU_PRTTTLCREDIT, puchFlag, uchType, usDEPTNo, &uchScaleIn);       
                }                                                           
                lSubTotal += TtlPlu.PLUTotal.lAmount;                      
                lSubCounter += TtlPlu.PLUTotal.lCounter;
            } else {
                uchRptNoPrtCo++;                       /* Count up not Print Counter */
            }
            break;
        }   /*----- End of Switch Loop -----*/
        if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?      */
            return(RPT_ABORTED);
        } 
        if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
        }
        uchScaleOut |= uchScaleIn;
        if (uchRptNoPrtCo >= RPTNOPRTCO_MAX) {
            SerTtlReadWriteChkAndWait(RPT_WAIT_TIMER);  /* Sleep */
            uchRptNoPrtCo = 0;
        }
    }    /*----- End of While Loop -----*/

    if (sReturn != OP_EOF) {
        return(OpConvertError(sReturn));
    }
    if (usStatus != RPT_HASH) {                                             /* Not HASH ? */
        RptGTotal.lAmount += lSubTotal;                                     /* Add SubTtl to Grand Ttl */
        RptGTotal.lCounter += lSubCounter;
    }

    Total->lAmount += lSubTotal;
    Total->lCounter += lSubCounter;
    if (*puchFlag & 0x01) {
		RPTPLU           RptPlu = {0};

        RptFeed(RPT_DEFALTFEED);                                            /* Line Feed       */
        if(RptPauseCheck() == RPT_ABORTED){                                 /* aborted by MLD */
            return(RPT_ABORTED);
        }

        RptPlu.uchMajorClass = CLASS_RPTPLU;                                /* Set Major to print */
        RptPlu.uchMinorClass = CLASS_RPTPLU_PRTTTLCNT;
        RptPlu.PLUTotal.lAmount = lSubTotal;                                /* Set SubTotal to print */
        if ((uchScaleOut == 1) && (usStatus == RPT_OPEN)){                  /* one of items is Scalable? */
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* MDC #29 BIT1 is 1 */
                RptPlu.uchStatus = RPT_DECIMAL_2;                           /* Decimal - 0.01 */
            } else {                                                        /* MDC #29 BIT1 is 0 */
                RptPlu.uchStatus = RPT_DECIMAL_1;                           /* Decimal - 0.1 */
            }
            RptPlu.PLUTotal.lCounter = lSubCounter;
        } else {                                                     /* if none of items is non_Scalable ? */
            RptPlu.uchStatus = RPT_DECIMAL_0;                               /* no Decimal */
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* MDC #89 BIT 2 is 1 */
                RptPlu.PLUTotal.lCounter = lSubCounter / 100L;              /* Divide by 100 */
            } else {                                                        /* MDC #89 BIT 2 is 0 */
                RptPlu.PLUTotal.lCounter = lSubCounter / 10L;               /* Divide by 10 */
            }
        }
        RflGetTranMnem (RptPlu.aszMnemo, TRN_STTLR_ADR);          /* Copy xSubTtl Mnemo to print */
        RptPlu.usPrintControl = usRptPrintStatus;
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);/* Print GrandTotal */
            RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptPlu);                                        /* Print out */

        /* send print data to shared module */
        PrtShrPartialSend(RptPlu.usPrintControl);

    }
    if (UieReadAbortKey() == UIE_ENABLE) {                                  /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                                       /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
        return(RPT_ABORTED);
    }
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptDEPTNamePrt(UCHAR uchDEPTNo)
*               
*       Input:  UCHAR  uchDEPTNo
*
*      Output:  nothing
*
**     Return:  nothing
*
**  Description:  
*       Print out Dept name at the top of each DEPT group.
*       If no DEPT Number, print blank Dept name.
*===========================================================================
*/
VOID    RptDEPTNamePrt(USHORT usDEPTNo, UCHAR uchType)
{                   
    DEPTIF  Dpi = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    Dpi.usDeptNo = usDEPTNo;            
    if (CliOpDeptIndRead(&Dpi, 0) != OP_PERFORM) {              /* Get Mnemonics */
        RptCodeNamePrt(usDEPTNo, uchType);
    } else {
		RPTPLU  RptPlu = {0};

        _tcsncpy(RptPlu.aszMnemo, Dpi.ParaDept.auchMnemonic, PARA_DEPT_LEN);  /* Copy Mnemo */
        RptPlu.uchMajorClass = CLASS_RPTPLU;                    /* Set Major for Print */
        RptPlu.uchMinorClass = CLASS_RPTPLU_PRTDEPT;            /* Set Minor for Print */
        RptPlu.usDEPTNumber = usDEPTNo;
        RptPlu.usPrintControl = usRptPrintStatus;
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);/* Print GrandTotal */
            RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptPlu);                            /* Print out           */ 
    }
}

/*
*===========================================================================
**  Synopsis:  SHORT  RptPLUByDEPTAdj(PLUIF_DEP  *pPluIf_Dep,
*                                     TTLPLU     *pTtlPlu,
*                                     UCHAR      uchMinor,
*                                     UCHAR      *puchFlag,
*                                     UCHAR      uchType,
*                                     USHORT     usDEPTNo,
*                                     UCHAR      *puchScaleIn);
*               
*       Input:  PLUIF_DEP  *pPluIf_Dep
*               TTLPLU     *pTtlPlu 
*               UCHAR      uchMinor 
*               UCHAR      *puchFlag
*               UCHAR      uchType
*               UCHAR      uchDEPTNo
*               UCHAR      *puchScaleIn
*
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Get all 5 Adjective Data.  Add to Total-lAmount and lCounter.
*===========================================================================
*/
SHORT   RptPLUByDEPTAdj(PLUIF_DEP *pPluIf_Dep, TTLPLU *pTtlPlu, UCHAR uchMinor,
                        UCHAR *puchFlag, UCHAR uchType, USHORT usDEPTNo, UCHAR *puchScaleIn)
{
    SHORT   sReturn;
    USHORT  usPercent = 0;
    UCHAR   uchStatus = RPT_DECIMAL_0;
    UCHAR   uchAdjNo,
            uchNo,
            uchPluProhibitVal;
	LTOTAL  AdjTtl = {0};
	TCHAR   AdjMnemo[PARA_ADJMNEMO_LEN + 1] = {0};
	TCHAR   SpeMnemo[PARA_SPEMNEMO_LEN + 1] = {0};
    PLUIF_DEP     PluFile_Dep;  /* to get unit price of Adj2 to Adj5 */
                                /* other data is used in pPluIf_Dep structure */

    /*----- Classify 4 Adjective groups -----*/
    if (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG1) {          /* Group 1 */
        uchNo = ADJ_GP1VAR1_ADR;
    } else if (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG2) {   /* Group 2 */
        uchNo = ADJ_GP2VAR1_ADR;
    } else if (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[3] & PLU_USE_ADJG3) {   /* Group 3 */
        uchNo = ADJ_GP3VAR1_ADR;
    } else {                                                                     /* Group 4 */
        uchNo = ADJ_GP4VAR1_ADR;
    }

    /*----- Get Adjective Mnemonics -----*/
    RflGetAdj (AdjMnemo, uchNo);                                                /* Get Adjective Mnemonics */

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&          /* Plus item PLU ? */
        !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {       /* Scalable ? */
        RflGetSpecMnem (SpeMnemo, SPC_LB_ADR);                                  /* Get Mnemonic for scale units, LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                     /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                          /* Decimal - 0.01 */
        } else {                                                                /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                           /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS or MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&          /* NORMAL TYPE ? */
       !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);
    }
    uchPluProhibitVal = PLU_PROHIBIT_VAL1;
    if (RptPLUPrintChk2(pTtlPlu, pPluIf_Dep->ParaPlu.ContPlu.auchContOther[4], uchPluProhibitVal) == 0) { 
        if (*puchFlag == 0x00) {
            RptFeed(RPT_DEFALTFEED);
            if (uchType != RPT_DEPT_READ) {
                RptDEPTNamePrt(usDEPTNo, uchType);
                RptFeed(RPT_DEFALTFEED);
            }
        }
        RptPLUPrint(uchMinor,                            /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                  /* TOTAL- lAmount & lCounter */   
                    pPluIf_Dep->auchPluNo,               /* PLU Number */
                    pPluIf_Dep->ParaPlu.auchPluName,     /* PLU Mnemonics */            
                    SpeMnemo,                            /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                            /* Ajective Mnemoincs */
                    pPluIf_Dep->ParaPlu.auchPrice,       /* PresetPrice */       
                    usPercent,                           /* Percent */             
                    uchStatus,                           /* Decimal point status */
                    uchType);                            /* Repot Type */
        *puchFlag |= 0x01;
        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else {                                                /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
    } else {
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    }
    AdjTtl.lAmount = pTtlPlu->PLUTotal.lAmount;
    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&   /* Plus item PLU ? */
        !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {/* Scalable ? */
        AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter;                    /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {              /* !Scalable - MDC #89 BIT 2 is 0 */ 
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 100L;         /* Adjust Amt to Wgt Amt */       
        } else {                                                         /* !Scalable - MDC #89 BIT 2 is 1 */ 
            AdjTtl.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;          /* Adjust Amt to Wgt Amt */         
        }
    }

    /*----- loop 2nd to 5th Adjective -----*/
    /*****======================================================================*****/
    /* Using PLU Parameter File from Address3 to Address10 that are set in function */
    /* RptPLUByDEPT if PLU Parameter file Address 7 BIT3 is 1 (Using DEPT status).  */
    /* In other case, using PLU Parameter File Address6 BIT2                        */
    /* The reason to copy from pPluIf_Dep to PluFile_Dep is to get each Adjective   */
    /* unit Price                                                                   */
    /*****======================================================================*****/
    PluFile_Dep = *pPluIf_Dep;   /* Copy to other work Buffer */
                                                             /* to save copied Status */
    for (uchAdjNo = uchNo+(UCHAR)1; uchAdjNo < uchNo+(UCHAR)5; uchAdjNo++) {       /* Loop 2 to 5 */
        PluFile_Dep.uchPluAdj++;                             /* Next Adjective for work Buffer */
        if ((sReturn = SerOpDeptPluRead(&PluFile_Dep, 0)) != OP_PERFORM) {  /* Skip if not perform */
            return(OpConvertError(sReturn));
        }
        /*----- to store new Adjective unit price -----*/
        memcpy(pPluIf_Dep->ParaPlu.auchPrice, PluFile_Dep.ParaPlu.auchPrice, sizeof(pPluIf_Dep->ParaPlu.auchPrice));  

        memset(&pTtlPlu->PLUTotal, 0, sizeof(pTtlPlu->PLUTotal));
        _tcsncpy(pTtlPlu->auchPLUNumber, pPluIf_Dep->auchPluNo, OP_PLU_LEN);
        pTtlPlu->uchAdjectNo = PluFile_Dep.uchPluAdj;
        if (((sReturn = SerTtlTotalRead(pTtlPlu)) != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
            return(TtlConvertError(sReturn));
        }
        uchPluProhibitVal = (UCHAR)(uchPluProhibitVal << 1);             /* Shift right 1 bit in each time */
        if (RptPLUPrintChk2(pTtlPlu, pPluIf_Dep->ParaPlu.ContPlu.auchContOther[4], 
                            uchPluProhibitVal) != 0) { 
            uchRptNoPrtCo++;                       /* Count up not Print Counter */
            continue;
        }
        /*----- Get Adjective Mnemonics -----*/
        RflGetAdj (AdjMnemo, uchAdjNo);                                                /* Get Adjective Mnemonics */

        AdjTtl.lAmount += pTtlPlu->PLUTotal.lAmount;
        if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&  /* Plus item PLU ? */
            !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
             (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) { /* Scalable ? */ 
            AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter;                  /* Store Wgt Amt /w Decimal point */
        } else{                                                            
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* !Scalable - MDC #89 BIT 2 is 0 */       
                AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 100L;       /* Adjust Amt to Wgt Amt */        
            } else {                                                        /* !Scalable - MDC #89 BIT 2 is 1 */        
               AdjTtl.lCounter += pTtlPlu->PLUTotal.lCounter * 10L;         /* Adjust Amt to Wgt Amt */         
            }
        }

        /*----- Calculate Percent based on MDC -----*/
        if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&  /* NORMAL TYPE ? */
           !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
            usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);
        }
        if ((uchType != RPT_DEPT_READ) && (*puchFlag == 0x00)) {
            RptFeed(RPT_DEFALTFEED);
            RptDEPTNamePrt(usDEPTNo, uchType);
            RptFeed(RPT_DEFALTFEED);
        }
        RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLOPEN */ 
                    &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                    pPluIf_Dep->auchPluNo,              /* PLU Number */
                    pPluIf_Dep->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                    SpeMnemo,                           /* Special Mnemonics LB/Kg */     
                    AdjMnemo,                           /* Ajective Mnemonics */
                    pPluIf_Dep->ParaPlu.auchPrice,      /* Preset Price */       
                    usPercent,                          /* Percent */             
                    uchStatus,                          /* Decimal point status */
                    uchType);                           /* Repot Type */

        *puchFlag |= 0x01;
        if (uchMinor == CLASS_RPTPLU_PRTTTLADJ1) {              /* 1st Adjective ? */
            uchMinor = CLASS_RPTPLU_PRTTTLADJ2;                 /* Set 2nd Adjective Minor */
        } else if (uchMinor == CLASS_RPTPLU_PRTTTLMINUSADJ1) {  /* 1st Adjective minus ? */
            uchMinor = CLASS_RPTPLU_PRTTTLMINUSADJ2;            /* Set 2nd Adjective minus Minor */
        }
    } /*----- End of For Loop -----*/
    pTtlPlu->PLUTotal = AdjTtl;
    *pPluIf_Dep = PluFile_Dep;    /* Need to copy to other work Buffer */
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  VOID  RptPLUByDEPTOpen(PLUIF_DEP  *pPluIf_Dep,
*                                     TTLPLU     *pTtlPlu, 
*                                     UCHAR uchMinor,
*                                     UCHAR *puchFlag,
*                                     UCHAR uchType,
*                                     UCHAR uchDEPTNo,
*                                     UCHAR *puchScaleIn);
*               
*       Input:  PLUIF_DEP   *pPluIf_Dep
*               TTLPLU      *pTtlPlu
*               UCHAR       uchMinor
*               UCHAR       *puchFlag
*               UCHAR       uchType
*               USHORT      usDEPTNo
*               UCHAR       *puchScaleIn
*
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Get all Open Data.
*       Prepare to print PLU element.
*===========================================================================
*/
VOID    RptPLUByDEPTOpen(PLUIF_DEP *pPluIf_Dep, TTLPLU *pTtlPlu, UCHAR uchMinor,
                UCHAR *puchFlag, UCHAR uchType, USHORT usDEPTNo, UCHAR *puchScaleIn)
{
    UCHAR    uchStatus = RPT_DECIMAL_0;
    USHORT   usPercent = 0;
	TCHAR    SpeMnemo [PARA_SPEMNEMO_LEN + 1] = {0};

    /*----- Get Special Mnemonics -----*/
    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */
        RflGetSpecMnem(SpeMnemo, SPC_LB_ADR);                               /* Get Mnemonic for LB/Kg */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* MDC #29 BIT1 is 1 */
            uchStatus = RPT_DECIMAL_2;                                      /* Decimal - 0.01 */
        } else {                                                            /* MDC #29 BIT1 is 0 */
           uchStatus = RPT_DECIMAL_1;                                       /* Decimal - 0.1 */
        }
        uchRptWgtFlag = 1;                                          /* Set Wgt Flag for All GTtl */
        uchRptScaleTtl = 1;                                         /* Set Wgt Flag for PLUS or MINUS Ttl */
        *puchScaleIn = 1;                                           /* Set Wgt Flag for SubTotal */
    }

    /*----- Calculate Percent based on MDC -----*/
    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* NORMAL TYPE ? */
       !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH)) {
        usPercent = RptPLUPercent(pTtlPlu, *puchScaleIn);
    }

    if ((pTtlPlu->PLUTotal.lAmount == 0L) && (pTtlPlu->PLUTotal.lCounter == 0L) &&  /* Total = 0 ? */
        (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT2))) {                    /* MDC # 89 BIT 2 */
        uchRptNoPrtCo++;                       /* Count up not Print Counter */
    } else {
        if (*puchFlag == 0x00) {
            RptFeed(RPT_DEFALTFEED);
            if (uchType != RPT_DEPT_READ) {
                RptDEPTNamePrt(usDEPTNo, uchType);
                RptFeed(RPT_DEFALTFEED);
            }
        }
        switch(uchMinor) {
        case CLASS_RPTPLU_PRTTTLOPEN:                       /* CLASS_RPTPLU_PRTTTLOPEN */
            /* break; */
        case CLASS_RPTPLU_PRTTTLCREDIT:                     /* CLASS_RPTPLU_PRTTTLCREDIT */
            RptPLUPrint(uchMinor,                            
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */             
                        pPluIf_Dep->auchPluNo,              /* PLU Number */
                        pPluIf_Dep->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                           /* Special Mnemonics LB/Kg */             
                        0,                                  /* NOT USE (Ajective Mnemoincs) */              
                        0,                                  /* NOT USE (PresetPrice) */             
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Repot Type */
            *puchFlag |= 0x01;
            break;

        default:                                            /* case of CLASS_RPTPLU_PRTTTLPRSET */ 
                                                            /* case of CLASS_RPTPLU_PRTTTLCOUPON */
            RptPLUPrint(uchMinor,                           /* CLASS_RPTPLU_PRTTTLPRSET */ 
                        &pTtlPlu->PLUTotal,                 /* TOTAL- lAmount & lCounter */   
                        pPluIf_Dep->auchPluNo,              /* PLU Number */
                        pPluIf_Dep->ParaPlu.auchPluName,    /* PLU Mnemonics */            
                        SpeMnemo,                           /* Special Mnemonics LB/Kg */     
                        0,                                  /* NOT USE (Ajective Mnemoincs) */
                        pPluIf_Dep->ParaPlu.auchPrice,      /* PresetPrice */       
                        usPercent,                          /* Percent */             
                        uchStatus,                          /* Decimal point status */
                        uchType);                           /* Repot Type */
            *puchFlag |= 0x01;
            break;
        }
    }
    if (!(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[0] & PLU_MINUS) &&      /* Plus item PLU ? */
        !(pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_HASH) &&    
         (pPluIf_Dep->ParaPlu.ContPlu.auchContOther[2] & PLU_SCALABLE)) {   /* Scalable ? */ 
        pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter;            /* Store Wgt Amt /w Decimal point */
    } else{
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {                 /* !Scalable - MDC #89 BIT 2 is 0 */
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 100L; /* Adjust Amt to Wgt Amt */             
        } else {                                                            /* !Scalable - MDC #89 BIT 2 is 1 */        
            pTtlPlu->PLUTotal.lCounter = pTtlPlu->PLUTotal.lCounter * 10L;  /* Adjust Amt to Wgt Amt */         
        }                                                                            
    }
}

/*
*===========================================================================
**  Synopsis:  UCHAR  RptPLUPrintChk1(TTLPLU     *pTtlPlu,
*                                     UCHAR      uchContOther,
*                                     UCHAR      uchPluProhibitVal)
*               
*       Input:  TTLPLU     *pTtlPlu 
*               UCHAR      uchContOther
*               UCHAR      uchPluProhibitVal  
*
*      Output:  nothing
*
**     Return:  0      : Success
*               -1     : Fail
*
**  Description:  
*       Check the 2-condition to print out or not print out .
*       This is only for 5-Adjective.
*       1. Total: lAmount and lCounter are 0. 
*       2. if Total is 0 then check Adjective is Prohibit.
*===========================================================================
*/

UCHAR RptPLUPrintChk1(TTLPLU  *pTtlPlu, UCHAR uchContOther, UCHAR uchPluProhibitVal)
{
    if ((pTtlPlu->PLUTotal.lAmount == 0L) && (pTtlPlu->PLUTotal.lCounter == 0L)) {  /* Total = 0 ? */
        if (uchContOther & uchPluProhibitVal) {
            return -1;                                                              /* Not Print out ? */
        }
    }
    return 0;
}

/*
*===========================================================================
**  Synopsis:  UCHAR  RptPLUPrintChk2(TTLPLU     *pTtlPlu,
*                                    UCHAR      uchContOther,
*                                    UCHAR      uchPluProhibitVal)
*               
*       Input:  TTLPLU     *pTtlPlu 
*               UCHAR      uchContOther
*               UCHAR      uchPluProhibitVal  
*
*      Output:  nothing
*
**     Return:  0      : Success
*               -1     : Fail
*
**  Description:  
*       Check the 3-condition to print out or not print out .
*       This is only for 5-Adjective.
*       1. Total: lAmount and lCounter are 0. 
*       2. if Total is 0 then check Adjective is Prohibit.
*       3. if Total is 0 and not Prohibit then check MDC #89 BIT2.  
*===========================================================================
*/
UCHAR RptPLUPrintChk2(TTLPLU  *pTtlPlu, UCHAR uchContOther, UCHAR uchPluProhibitVal)
{
    if ((pTtlPlu->PLUTotal.lAmount == 0L) && (pTtlPlu->PLUTotal.lCounter == 0L)) {  /* Total = 0 ? */
        if (uchContOther & uchPluProhibitVal) {
            return -1;                                                          /* Not Print out ? */
        } else {
            if (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT2)) {              /* MDC #89 BIT 2 */
                return -1;
            }
        }
    }
    return 0;
}

/*
*===========================================================================
**  Synopsis:  VOID RptPLUPrint(UCHAR  uchMinor,
*                               TOTAL  *PLUTotal,
*                               UCHAR  *puchPLUNumber,
*                               UCHAR  aszMnemo,
*                               UCHAR  aszSpeMnemo,
*                               UCHAR  aszAdjMnemo,
*                               UCHAR  auchPrice,
*                               USHORT usPercent, 
*                               UCHAR  uchStatus)    
*               
*       Input:  UCHAR       uchMinor    :   CLASS_RPTPLU_PRTTTLOPEN
*                                       :   CLASS_RPTPLU_PRTTTLCREDIT
*                                       :   CLASS_RPTPLU_PRTTTLPRSET
*                                       :   CLASS_RPTPLU_PRTTTLCOUPON
*                                       :   CLASS_RPTPLU_RPTTTLADJ1
*                                       :   CLASS_RPTPLU_RPTTTLMINUSADJ1        
*               TOTAL       *PLUTotal   :
*               USHORT      usPLUNumber :
*               UCHAR       aszMnemo    :
*               UCHAR       aszSpeMnemo :
*               UCHAR       aszAdjMnemo :
*               UCHAR       auchPrice   :   Unit Price
*               USHORT      usPercent   :
*               UCHAR       uchStatus   :
*
*               
*      Output:  nothing
*
**     Return:  DptPlu.PLUTotal (lAmount and lCounter)
*
**  Description:  
*       Print out each PLU element.
*===========================================================================
*/
VOID    RptPLUPrint(UCHAR uchMinor, LTOTAL *PLUTotal, TCHAR  *puchPLUNumber, TCHAR  aszMnemo[PARA_PLU_LEN+1],
            TCHAR  aszSpeMnemo[PARA_SPEMNEMO_LEN+1], TCHAR  aszAdjMnemo[PARA_ADJMNEMO_LEN+1],
            UCHAR  auchPrice[3], USHORT usPercent, UCHAR  uchStatus, UCHAR  uchType)
{
	RPTPLU    RptPlu = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    RptPlu.uchMajorClass = CLASS_RPTPLU;                                /* Set Major to print */
    RptPlu.uchMinorClass = uchMinor;                                    /* Set Minor to print */
    RptPlu.PLUTotal.lAmount = PLUTotal->lAmount;                        /* Set lAmount to print */
    RptPlu.PLUTotal.lCounter = PLUTotal->lCounter;                      /* Set lCounter to print */
    RflConvertPLU(RptPlu.auchPLUNumber, puchPLUNumber);
    _tcsncpy(RptPlu.aszMnemo, aszMnemo, PARA_PLU_LEN);                    /* Copy Mnemonics */
    _tcsncpy(RptPlu.aszSpeMnemo, aszSpeMnemo, PARA_SPEMNEMO_LEN);         /* Copy Special Mnemo */
    RptPlu.uchStatus = uchStatus;                                       /* Set Decimal point status */
    RptPlu.usPrintControl = usRptPrintStatus;                           /* Set Print statua */

    switch(uchMinor) {
    case CLASS_RPTPLU_PRTTTLOPEN:                                       /* Set Percent to print */
        RptPlu.usPercent = usPercent;
        /* break;  */
    case CLASS_RPTPLU_PRTTTLCREDIT:
        break;

    case CLASS_RPTPLU_PRTTTLPRSET:
        RptPlu.usPercent = usPercent;                                   /* Set Percent to print */
        /* break; */
    case CLASS_RPTPLU_PRTTTLCOUPON:
		// RflConv3bto4b(&RptPlu.lPresetPrice, auchPrice);               /* Copy Preset Price */
		RptPlu.lPresetPrice = (LONG)RflFunc3bto4b(auchPrice);
        break;

    case CLASS_RPTPLU_PRTTTLADJ1:
        /* break; */
    case CLASS_RPTPLU_PRTTTLADJ2:
        RptPlu.usPercent = usPercent;                                 /* Set Percent to print */
        /* break; */
    default:                                                          /* case of CLASS_RPTPLU_PRTTTLMINUSADJ1 */
                                                                      /* case of CLASS_RPTPLU_PRTTTLMINUSADJ2 */ 
        _tcsncpy(RptPlu.aszAdjMnemo, aszAdjMnemo, PARA_ADJMNEMO_LEN);                 /* Copy Adjective Mnemo */
		// RflConv3bto4b(&RptPlu.lPresetPrice, auchPrice);               /* Copy Preset Price */
		RptPlu.lPresetPrice = (LONG)RflFunc3bto4b(auchPrice);
		break;
    }
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);/* Print GrandTotal */
        RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptPlu);

    /* send print data to shared module */
    PrtShrPartialSend(RptPlu.usPrintControl);
}

/*
*=============================================================================
**  Synopsis: VOID RptPLUHeader(UCHAR uchMinorClass, UCHAR uchType, USHORT usPluNo, UCHAR uchResetType) 
*               
*       Input:  UCHAR    uchMinorClass   : CLASS_TTLCURDAY
*                                        : CLASS_TTLSAVDAY
*                                        : CLASS_TTLCURPTD
*                                        : CLASS_TTLSAVPTD
*               USHORT   uchType         : RPT_ALL_READ
*                                        : RPT_DEPT_READ
*                                        : RPT_CODE_READ
*                                        : RPT_PLUIND_READ
*                                        : RPT_FIN_READ
*                                        : RPT_EOD_ALLREAD
*                                        : RPT_PTD_ALLREAD
*               USHORT   usPluNo         : PLU Number
*               UCHAR   uchResetType     : 0
*                                        : RPT_ONLY_PRT_RESET
*                                        : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:  nothing 
*
**  Description:
*       Read Daily or PTD and Current or Saved.
*       Get current Time if data is Current Read.
*       Set certain data for Header.
*       Print Header and Clear Grand total if it is first operation.
*===============================================================================
*/
VOID    RptPLUHeader(UCHAR uchMinorClass, UCHAR uchType, TCHAR *puchPluNo, UCHAR uchResetType)
{
    UCHAR           uchSpecMnemo;
	UCHAR			uchRptType, uchTmpType, uchACNo;
	TTLPLU          TtlPlu = {0};
	SHORT			sReturn;

    /*----- Check First Operation -----*/
    RptPrtStatusSet(uchMinorClass);                             /* Check and set Print Status */

    if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation Case */                   
        memset(&RptGTotal, 0, sizeof(RptGTotal));               /* Clear Grand Total */

        /*----- Select Special Mnemonics for Header -----*/
        switch(uchMinorClass) {
        case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */        
            uchACNo = AC_PLUSALE_READ_RPT;
            uchTmpType = uchType;
            break;
   
        case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
            uchACNo = AC_PLU_RESET_RPT;
            uchTmpType = 0;                                     /* NOT print Report Type */
            break;

        case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
            uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */
            uchACNo = AC_PLUSALE_READ_RPT;
            uchTmpType = uchType;
            break;

        case CLASS_TTLSAVPTD :                                  /* Case of PTD RESET */
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
            uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
            uchACNo = AC_PLU_RESET_RPT;
            uchTmpType = 0;                                     /* NOT print Report Type */
            break;

        default:            
/*            PifAbort(MODULE_RPT_ID, FAULT_INVALID_DATA); */
            return;
        }

        /*----- Print Header Name -----*/
        if ((uchType != RPT_EOD_ALLREAD) && (uchType != RPT_PTD_ALLREAD)) {                                                           
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
        }
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                        RPT_ACT_ADR,                            /* Report Name Address       */
                        RPT_PLU_ADR,                            /* Report Name Address       */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        uchRptType,                             /* Report Name Address       */
                        uchTmpType,                             /* Report Type               */
                        uchACNo,                                /* A/C Number                */
                        uchResetType,                           /* Reset Type                */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
        TtlPlu.uchMajorClass = CLASS_TTLPLU;
        TtlPlu.uchMinorClass = uchMinorClass;
        _tcsncpy(TtlPlu.auchPLUNumber, puchPluNo, OP_PLU_LEN);

		sReturn = SerTtlTotalRead(&TtlPlu);              
	    if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
		    PifLog(MODULE_PLU, sReturn);
		}
        if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD)) {
			TtlGetNdate (&TtlPlu.ToDate);      /* Get Current Date/Time and  Set Period To Date */
        }
        RptPrtTime(TRN_PFROM_ADR, &TtlPlu.FromDate);            /* Print PERIOD FROM         */
        RptPrtTime(TRN_PTO_ADR, &TtlPlu.ToDate);                /* Print PERIOD TO           */
        if (uchType == RPT_PLUIND_READ) {
            RptFeed(RPT_DEFALTFEED);
        }

        uchRptOpeCount |= RPT_HEADER_PRINT;                     /* Reset Already Print Header */
    }
}

/*
*===========================================================================
**  Synopsis:  USHORT     RptPLUPercent(TTLPLU  *pTtlPlu, UCHAR uchScale)
*               
*       Input:    TTLPLU  *pTtlPlu     : 
*
*      Output:  nothing
*
**     Return:  Data of Percent
*           
**  Description:  
*       Check MDC #89 BIT 3 and Calculate Percent depend on 2-condition.
*       Devided PLU Amount by All PLU Amount or devided PLU Amount by All PLU
*       Counter. 
*       
*===========================================================================
*/
    
USHORT  RptPLUPercent(TTLPLU *pTtlPlu, UCHAR uchScale)
{
    D13DIGITS  Product;

    if (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT3)) {                  /* MDC #89 check */
    /*----- A Numerator is less than 0 ?   OR A Denominator is less than or equal 0 ? -----*/
        if ((pTtlPlu->PLUTotal.lAmount < 0L) || (pTtlPlu->PLUAllTotal.lAmount <= 0L)) {
            return(RPT_OVERFLOW);
        }
		Product = pTtlPlu->PLUTotal.lAmount;
		Product *= 100000L;
		Product /= pTtlPlu->PLUAllTotal.lAmount;
    } else {
		LONG  lCounter;

		/*----- A Numerator is less than 0 ?   OR A Denominator is less than or equal 0 ? -----*/
        if ((pTtlPlu->PLUTotal.lCounter < 0L) || (pTtlPlu->PLUAllTotal.lCounter <= 0L)) {
            return(RPT_OVERFLOW);
        }
        if (uchScale) {                                                     /* if Scalable */
            if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* MDC #29 BIT1 is 1 */  
                lCounter = (pTtlPlu->PLUTotal.lCounter + 50L) / 100L;       /* Divide by 100 */
            } else {                                                        /* MDC #29 BIT1 is 0 */
                lCounter = (pTtlPlu->PLUTotal.lCounter + 5L) / 10L;         /* Divide by 10 */ 
            }
            return(RPT_OVERFLOW);                                           /* Print ******% */
        } else {
            lCounter = pTtlPlu->PLUTotal.lCounter;
        }
		Product = lCounter;
		Product *= 100000L;
		Product /= pTtlPlu->PLUAllTotal.lCounter;
    }

    Product = (Product + 5L) / 10L;
    if (Product > 32767L) {
        return(RPT_OVERFLOW);                                               /* Print ******% */
    } else {
        return((USHORT)Product);
    }
}

/*
*===========================================================================
**  Synopsis:  VOID     RptPLUGrandTtlPrt(UCHAR uchTransAddr
*                                         LTOTAL *lTotal,
*                                         USHORT usStatus,
*                                         UCHAR  uchWeight);
*               
*       Input:  UCHAR    uchTransAddr   : TRN_TTR_ADR  --- Plus Total
*                                       : TRN_MTTLR_ADR -- Minus Total
*                                       : TRN_HTTLR_ADR -- Hash Total
*                                       : TRN_GTTLR_ADR -- Grand Total (Plus + Minus)
*               LONG     *lTotal        : 
*               USHORT   usStatus       :
*               UCHAR    uchWeight      :   uchRptWgtFlag - use for All Grand Total
*                                       :   uchRptScaleTtl 
*                                              - use for Total of PLU and MINUS Type1,Type2.
*                                              - use for Total of HASH type3.
*
*      Output:  nothing
*
**     Return:  nothing
*
**  Description:  
*       Print out Total or GrandTotal depends on the condition of 3nd formal
*       parameter : Total is Total, RptGTotal is Grand Total.
*===========================================================================
*/

VOID RptPLUGrandTtlPrt(USHORT usTransAddr,
                       LTOTAL *pTotal, 
                       USHORT usStatus,
                       UCHAR  uchWeigt,
                       UCHAR  uchType)
{
	RPTPLU   RptPlu = {0};
                                        
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    RptPlu.uchMajorClass = CLASS_RPTPLU;
    RptPlu.uchMinorClass = CLASS_RPTPLU_PRTTTLCNT;
    RptPlu.PLUTotal.lAmount = pTotal->lAmount;                          /* Store GrandTotal */
    if ((uchWeigt == 1) && (usStatus != RPT_HASH)) {                    /* Scalable ? */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* MDC #29 BIT1 is 1 */
            RptPlu.uchStatus = RPT_DECIMAL_2;                           /* Decimal - 0.01 */
        } else {                                                        /* MDC #29 BIT1 is 0 */
            RptPlu.uchStatus = RPT_DECIMAL_1;                           /* Decimal - 0.1 */
        }
        RptPlu.PLUTotal.lCounter = pTotal->lCounter;
    } else {                                                            /* Not Scalable ? */
        RptPlu.uchStatus = RPT_DECIMAL_0;
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1)) {             /* MDC #89 BIT 2 */
            RptPlu.PLUTotal.lCounter = pTotal->lCounter / 100L;   
        } else {
            RptPlu.PLUTotal.lCounter = pTotal->lCounter / 10L;      
        }
    }

	RflGetTranMnem (RptPlu.aszMnemo, usTransAddr);                      /* Copy Mnemonics   */
    RptPlu.usPrintControl = usRptPrintStatus;                           /* Set Print statua */
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptPlu, (USHORT)uchType);/* Print GrandTotal */
        RptPlu.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptPlu);                                        /* Print GrandTotal */

    /* send print data to shared module */
    PrtShrPartialSend(RptPlu.usPrintControl);
}

/*
*=============================================================================
**  Synopsis: SHORT RptPLUDayReset(UCHAR uchFunc) 
*               
*       Input:  uchFunc     : RPT_PRT_RESET ------ Report and Reset
*                           : RPT_ONLY_RESET ----- Only Reset
*      Output:  nothing
*
**     Return:  SUCCESS         : Successful 
*               LDT_ERR_ADR     : (TTL_NOT_ISSUED)
*
**  Description:  
*       Reset Daily PLU File.
*===============================================================================
*/

SHORT RptPLUDayReset(UCHAR uchFunc) 
{
    /* execute Current Daily Reset Report */

    return(RptPLUReset(CLASS_TTLCURDAY, uchFunc));
}

/*
*=============================================================================
**  Synopsis: SHORT RptPLUPTDReset(UCHAR uchFunc) 
*               
*       Input:  uchFunc     : RPT_PRT_RESET  ------ Report and Reset
*                           : RPT_ONLY_RESET  ----- Only Reset
*      Output:  nothing
*
**     Return:  SUCCESS         : Successful 
*               LDT_ERR_ADR     : (TTL_NOT_ISSUED)
*
**  Description:  
*       Reset PTD PLU File.
*===============================================================================
*/

SHORT RptPLUPTDReset(UCHAR uchFunc) 
{
    /* execute Current PTD Reset Report */

    return(RptPLUReset(CLASS_TTLCURPTD, uchFunc));
}

/*
*=============================================================================
**  Synopsis: SHORT RptPLUReset(UCHAR uchMinorClass, UCHAR uchFunc) 
*               
*       Input:  uchMinorClass   : CLASS_TTLSAVDAY
*                               : CLASS_TTLSAVPTD
*               uchFunc         : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful 
*               LDT_ERR_ADR         : (TTL_NOT_ISSUED)
*
**  Description:  
*        Reset Daily or PTD PLU File.
*        
*===============================================================================
*/

SHORT RptPLUReset(UCHAR uchMinorClass, UCHAR uchFunc) 
{
    UCHAR   uchSpecReset,
            uchSpecMnemo;
    SHORT   sReturn;
    PLUIF   PluIf = {0};
	TTLPLU  TtlPlu = {0}, TtlPlu2 = {0};

    /*----- Reset All PLU Total -----*/
    TtlPlu.uchMajorClass = CLASS_TTLPLU;
    TtlPlu.uchMinorClass = uchMinorClass;
    TtlPlu2 = TtlPlu;
    if (uchMinorClass == CLASS_TTLCURDAY) {
        TtlPlu2.uchMinorClass = CLASS_TTLSAVDAY;
    } else if (uchMinorClass == CLASS_TTLCURPTD) {
        TtlPlu2.uchMinorClass = CLASS_TTLSAVPTD;
    }
    if ((sReturn = SerTtlIssuedCheck(&TtlPlu2, TTL_NOTRESET)) != TTL_ISSUED) {
        return(TtlConvertError(sReturn));
    }

    if (uchFunc == RPT_ONLY_RESET) {
        TtlPlu.uchResetStatus |= TTL_NOTRESET;
    }

    if (uchFunc == RPT_DELETE) {
		return (LDT_PROHBT_ADR);
		
		/*** not use delete function, by DanP comment, 06/29/01 ***
    	if ((sReturn = SerTtlTotalDelete(&TtlPlu)) != TTL_SUCCESS) {   
        	return(TtlConvertError(sReturn));
    	}
    	***/
	} else {
	    if ((sReturn = SerTtlTotalReset(&TtlPlu,0)) != TTL_SUCCESS) {   
    	    return(TtlConvertError(sReturn));
    	}
    }
    if (uchMinorClass == CLASS_TTLCURDAY) {
        uchSpecReset = SPCO_EODRST_ADR;                         /* Set Daily Reset Counter */
    } else {
        uchSpecReset = SPCO_PTDRST_ADR;                         /* Set PTD Reset Counter */
    }
    PluIf.ulCounter = 0L;
    if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
        SerOpPluAbort(PluIf.husHandle,0);
        MaintIncreSpcCo(uchSpecReset);                          /* Count Up Reset Counter */
    }

    /*----- Only Reset Case -----*/

    if ((uchFunc == RPT_ONLY_RESET) || (uchFunc == RPT_DELETE)) {

        if (uchMinorClass == CLASS_TTLCURDAY) {
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        } else {
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        }
        /*----- Print Header Name -----*/
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecuteve Counter */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_PLU_ADR,                            /* Report Name Address */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        0,                                      /* Not Print */
                        AC_PLU_RESET_RPT,                       /* A/C Number */
                        uchFunc,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
                                                                   
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptPLULock(VOID) 
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

SHORT RptPLULock(VOID)    
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
**  Synopsis: VOID RptPLUUnLock(VOID) 
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

VOID  RptPLUUnLock(VOID)    
{
    /*----- Unlock PLU file -----*/

    SerCasAllUnLock();                      /* Unlock All Cashier File */
}

/*
*=============================================================================
**  Synopsis: VOID RptPLUOptimize(VOID) 
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
SHORT   RptPLUDayOptimize(UCHAR uchFunc)
{
    return(RptPLUOptimize(CLASS_TTLCURDAY, uchFunc));
}

SHORT   RptPLUPTDOptimize(UCHAR uchFunc)
{
    return(RptPLUOptimize(CLASS_TTLCURPTD, uchFunc));
}

SHORT   RptPLUOptimize(UCHAR uchMinorClass, UCHAR uchFunc)
{
	SHORT           sReturn;
    UCHAR           uchSpecMnemo;

    /* Execute Copy Total Counter File */
    if (uchMinorClass == CLASS_TTLCURDAY) {
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
    } else {
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
    }
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecuteve Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_PLU_ADR,                            /* Report Name Address */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        0,                          /* Report Name Address */
                        0,                                      /* Not Print */
                        AC_PLU_RESET_RPT,                       /* A/C Number */
                        uchFunc,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */


	sReturn = SerTtlPLUOptimize(uchMinorClass);

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */

    return(sReturn);
}

/**** End of File ****/