/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintFSC Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATFSC.C
* --------------------------------------------------------------------------
* Abstract: This function reads & writes FSC.
*
*           The provided function names are as follows: 
* 
*              MaintFSCRead()   : read & display FSC data.
*              MaintFSCEdit()   : set Major FSC data in self-work area
*              MaintFSCWrite()  : write & print FSC data of self-work area.
*              MaintFSCReport() : print all FSC data report
* 
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-23-92: 00.00.01 : J.Ikeda   : initial                                   
* Jun-21-93: 01.00.12 : K.You     : add direct menu shift key.                                   
* Feb-23-95: 03.00.00 : H.Ishida  : Add Keyed Coupon Key,Coupon No Key,
*          :          :           :     Scroll-Up Key,Scroll-Down Key,
*          :          :           :     Right Arrow Key,Left Arrow Key,
*          :          :           :     Wait Key,Paid Order Recall Key
* Mar-02-95:          :           : Add FSC_KEYED_COUPON of pass
* Nov-28-95: 03.01.00 : M.Ozawa   : Set direct plu no entry
* Jan-23-98: 03.01.16 : M.Ozawa   : Correct direct plu no entry
* 
*** NCR2171 **
* Sep-06-99: 01.00.00 : M.Teraki  : initial (for Win32)
* Jan-18-00: 01.00.00 : hrkato    : Saratoga
* 
*** GenPOS
* Oct-11-17 : 02.02.02 : R.Chambers   : fix use of memset() with incorrect size, localize variables.
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

#include "ecr.h"
#include "uie.h"
#include "uifprog.h"
#include "pif.h"
#include "plu.h"
#include "csop.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
#include "csstbopr.h"
#include "fsc.h"
#include "rfl.h"
#include "mld.h"
#include "appllog.h"
#include "maintram.h" 

CONST TCHAR FARCONST  aszMldFscPlu[] = _T(" %20s\t%13s ");

static SHORT MaintFSCMnem(VOID);                               /* Saratoga */
static SHORT MaintFSCMnemClear(VOID);                          /* Saratoga */
static VOID  MaintFSCDispPlu(MAINTFSCPLU *pPlu, SHORT sNum);   /* Saratoga */


/*
*===========================================================================
** Synopsis:    SHORT MaintFSCRead( PARAFSC *pData )
*               
*     Input:    *pData         : pointer to structure for PARAFSC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays FSC data.
*===========================================================================
*/

SHORT MaintFSCRead( PARAFSC *pData )
{
    UCHAR uchAddress;
    UCHAR uchPageNo;
    CONST UCHAR uchFSCMaxAddr = MaintQueryKeyBoardFscMax();

    /* check keyboard */
    if (uchFSCMaxAddr < 1) {
        NHPOS_ASSERT_TEXT(0, "==ERROR: MaintFSCRead() called with invalid keyboard type.");
        return(LDT_PROHBT_ADR);                                /* out of address */
	}

    switch(pData->uchMinorClass){
    case CLASS_PARAFSC_INIT:
        MaintWork.FSC.uchPageNo = pData->uchPageNo;
        MaintWork.FSC.uchAddress = pData->uchAddress;
        break;

    case CLASS_PARAFSC_PAGE:
        /* check status */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
            uchPageNo = ++MaintWork.FSC.uchPageNo;
            /* check page */
            if (uchPageNo > FSC_MAX_PAGE) {                       
                uchPageNo = FSC_MIN_PAGE;                           /* initialize page */
            }
        } else {                                                    /* with data */
            uchPageNo = pData->uchPageNo;
        }

        /* check page */
        if (uchPageNo < FSC_MIN_PAGE || FSC_MAX_PAGE < uchPageNo) {
            return(LDT_KEYOVER_ADR);                                /* out of address */
        }
        MaintWork.FSC.uchPageNo = uchPageNo;
        MaintWork.FSC.uchAddress = FSC_MIN_ADR;    /* Address Initial */ 
        break;

    case CLASS_PARAFSC_ADDRESS:
        /* check status */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
            uchAddress = ++MaintWork.FSC.uchAddress;

            /* check address */
            if (uchAddress > uchFSCMaxAddr) {                       
                uchAddress = FSC_MIN_ADR;                           /* initialize address */
            }
        } else {                                                    /* with data */
            uchAddress = pData->uchAddress;
        }

        /* check address */
        if (uchAddress < FSC_MIN_ADR || uchFSCMaxAddr < uchAddress) {
            return(LDT_KEYOVER_ADR);                                /* out of address */
        }
        MaintWork.FSC.uchAddress = uchAddress;
        break;
    }
    MaintWork.FSC.uchMajorClass = pData->uchMajorClass;         /* copy major class */
    CliParaRead(&(MaintWork.FSC));                              /* call CliParaFSCRead() */

    /* 2172 */
    if (MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
		PARAPLUNOMENU   ParaPLUNoMenu = {0};

        /* read plu no. from plu# table */
        ParaPLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
        ParaPLUNoMenu.uchMenuPageNumber = MaintWork.FSC.uchPageNo;
        ParaPLUNoMenu.uchMinorFSCData = MaintWork.FSC.uchMinorFSCData;
        CliParaRead(&ParaPLUNoMenu);                            /* call ParaDiscRead() */
        _tcsncpy(MaintWork.FSC.PluNo.aszPLUNumber, ParaPLUNoMenu.PluNo.aszPLUNumber, PLU_MAX_DIGIT);
        MaintWork.FSC.PluNo.uchModStat = ParaPLUNoMenu.PluNo.uchModStat;
    }

    /* 2172 */
    if (MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_DEPT) {
		PARADEPTNOMENU   ParaDeptNoMenu = {0};

        /* read plu no. from plu# table */
        ParaDeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;
        ParaDeptNoMenu.uchMenuPageNumber = MaintWork.FSC.uchPageNo;
        ParaDeptNoMenu.uchMinorFSCData = MaintWork.FSC.uchMinorFSCData;
        CliParaRead(&ParaDeptNoMenu);                            /* call ParaDiscRead() */
        MaintWork.FSC.usDeptNo = ParaDeptNoMenu.usDeptNumber;
    }

    DispWrite(&(MaintWork.FSC));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintFSCEdit( PARAFSC *pData )
*               
*     Input:    *pData          : pointer to structure for PARAFSC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : extend fsc do not exist 
*                               : successful
*               MAINT_FSC_EXIST : extend fsc exist 
*               LDT_KEYOVER_ADR : wrong data
*               LDT_SEQERR_ADR  : sequence error
*
** Description: This function sets Major FSC data in self-work area.
*===========================================================================
*/
SHORT MaintFSCEdit( PARAFSC *pData )
{
    SHORT      sReturn;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);
    }

    /* check minor class */
    switch(pData->uchMinorClass) {
    case CLASS_PARAFSC_MAJOR:
        /* check major FSC data */
        if (pData->uchMajorFSCData > FSC_MAJOR_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        MaintWork.FSC.uchMajorFSCData = pData->uchMajorFSCData; /* set major FSC data */
        MaintWork.FSC.uchMinorFSCData = 0;
        memset(MaintWork.FSC.PluNo.aszPLUNumber, 0, sizeof(MaintWork.FSC.PluNo.aszPLUNumber));
        MaintWork.FSC.usDeptNo = 0;

        /* check extend fsc */
        if ((1 <= pData->uchMajorFSCData && pData->uchMajorFSCData <= FSC_MINOR_MAX)
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_PRT_DEMAND)
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_PRESET_AMT)
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_KEYED_COUPON)
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_KEYED_STRING)
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_SEAT)            /* R3.1 */
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_CHARGE_TIPS)     /* V3.3 */
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_ASK)     /* 2172 */
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_ADJ_SHIFT)     /* 2172 */
            || ((pData->uchMajorFSCData + UIE_FSC_BIAS) == FSC_D_MENU_SHIFT)) { 

            /* display */
            DispWrite(&(MaintWork.FSC));
            /* --- Display Target PLUs on LCD,  Saratoga --- */
            if (MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
                if ((sReturn = MaintFSCMnem()) == SUCCESS) {
                    DispWrite(&(MaintWork.FSC));
                    return(MAINT_FSC_EXIST);
                } else {
                    return(sReturn);
                }
            }
            return(MAINT_FSC_EXIST);

        } else {                                                /* extend FSC exist */
            /* --- Display Target PLUs on LCD,  Saratoga --- */
            if (MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
                if ((sReturn = MaintFSCMnem()) == SUCCESS) {
                    return(SUCCESS);
                } else {
                    return(sReturn);
                }
            }
            return(SUCCESS);      
        }

    case CLASS_PARAFSC_MINOR2:
        /* saratoga */
        if ((MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS) == FSC_KEYED_DEPT) {

            if (pData->uchMinorFSCData > FSC_DEPT_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }

            /* write fsc table */
            MaintWork.FSC.uchMinorFSCData = pData->uchMinorFSCData; /* set minor FSC data */
            return(MAINT_FSC_EXIST);

        }
        
        if ((MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS) != FSC_KEYED_PLU) {
            return (LDT_SEQERR_ADR);
        }
        /* break */ /* not break */
        
    case CLASS_PARAFSC_MINOR:
        if ((MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS) != FSC_KEYED_PLU) {
            /* check minor FSC minimum data */
            if (pData->uchMinorFSCData < 1) {
                return(LDT_KEYOVER_ADR);
            }
            /* verify UCHAR range */
            if (pData->uchMinorFSCData > 0xFF) {
                return(LDT_KEYOVER_ADR);
            }
        }

        /* distinguish major FSC and set minor FSC */
        switch(MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS) {  /* UIE_FSC_BIAS=0x40 */
        case FSC_KEYED_PLU:                                     /* Keyed PLU key */
			{
				UCHAR           ii;
				UCHAR           uchStatus = 0;
				LABELANALYSIS   LabelAnalysis = {0};

            _tcscpy(LabelAnalysis.aszScanPlu, pData->PluNo.aszPLUNumber);

            if (pData->uchMinorClass == CLASS_PARAFSC_MINOR2) {
                LabelAnalysis.fchModifier |= LA_EMOD;     /* Set E-Version status */
            }
            if(pData->uchStatus & MAINT_SCANNER_INPUT) {    /* input from scanner ? */
                LabelAnalysis.fchModifier |= LA_SCANNER;      /* Set Scanner Input status */
            }
            sReturn = RflLabelAnalysis(&LabelAnalysis);

            if (sReturn == LABEL_ERR) {
                return(LDT_KEYOVER_ADR);    /* illegal data input error */
            }
            
            /* convert scaner entry data to manual entry data */
            if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                memset(MaintWork.FSC.PluNo.aszPLUNumber, 0, sizeof(MaintWork.FSC.PluNo.aszPLUNumber));
                sReturn = RflConvScan2Manual(MaintWork.FSC.PluNo.aszPLUNumber, LabelAnalysis.aszScanPlu);

                if ((sReturn == MOD_STAT_EVERSION) || (LabelAnalysis.fchModifier & LA_EMOD)) {
                    MaintWork.FSC.PluNo.uchModStat |= (UCHAR)MOD_STAT_EVERSION;
                }

            } else {
                _tcsncpy(MaintWork.FSC.PluNo.aszPLUNumber, pData->PluNo.aszPLUNumber, PLU_MAX_DIGIT);
                if (pData->uchMinorClass == CLASS_PARAFSC_MINOR2) {
                    MaintWork.FSC.PluNo.uchModStat |= (UCHAR)MOD_STAT_EVERSION;
                } else {
                    MaintWork.FSC.PluNo.uchModStat &= ~(UCHAR)MOD_STAT_EVERSION;
                }
            }
            for (ii = 0; ii < (PLU_MAX_DIGIT - 1); ii++) {            /* check PLU No. */
                if (MaintWork.FSC.PluNo.aszPLUNumber[ii] > 0x30) { /* not 0 ? */
                    uchStatus = 1;
                }
            }
            if (!uchStatus) {                               /* PLU# all 0 ? */
                memset(MaintWork.FSC.PluNo.aszPLUNumber, 0x00, sizeof(MaintWork.FSC.PluNo.aszPLUNumber)); /* cler TCHAR sized PLU number data */
            }
            pData->uchMinorFSCData = MaintWork.FSC.uchAddress;
			}
            break;

        case FSC_KEYED_DEPT:                                    /* Keyed Department key */
            if (pData->usDeptNo > MAX_DEPT_NO) {               /* Saratoga */
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.FSC.usDeptNo = pData->usDeptNo;
            pData->uchMinorFSCData = MaintWork.FSC.uchAddress;
            break;

        case FSC_PRINT_MODIF:                                   /* Print Modifier */
            if (pData->uchMinorFSCData > FSC_PM_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_ADJECTIVE:                                     /* Adjective key */
            if (pData->uchMinorFSCData > FSC_ADJ_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_TENDER:                                        /* Tender key */
			/* temporary hardcoded 11, change to MAINT_TEND_MAX when implementing full
			tender database change JHHJ 3-16-04*/	
            if (pData->uchMinorFSCData > FSC_TEND_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_FOREIGN:                                       /* Foreign Currency key */
            if (pData->uchMinorFSCData > FSC_FC_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;
    
        case FSC_TOTAL:                                         /* Total key */
			/* temporary hardcoded 9, change to FSC_TOTAL_MAX when implementing full
			tender database change JHHJ 3-24-04*/
            if (pData->uchMinorFSCData > FSC_TOTAL_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */                
            }
            break;

        case FSC_ADD_CHECK:                                     /* Add Check key */
            if (pData->uchMinorFSCData > FSC_ADDCHK_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;
    
        case FSC_ITEM_DISC:                                     /* Item Discount key */
            if (pData->uchMinorFSCData > FSC_IDISC_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_REGULAR_DISC:                                  /* Regular Discount key */
            if (pData->uchMinorFSCData > FSC_RDISC_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_TAX_MODIF:                                     /* Tax Modifier key */
            if (pData->uchMinorFSCData > FSC_TAXMD_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_PRT_DEMAND:                                    /* Print on Demand key */
            if (pData->uchMinorFSCData > FSC_PRTDMD_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_PRESET_AMT:                                    /* Preset Amount key */
            if (pData->uchMinorFSCData > FSC_PREAMT_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_D_MENU_SHIFT:                                  /* Direct Menu Shift key */
            if (pData->uchMinorFSCData > FSC_DMSFT_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

       case FSC_KEYED_COUPON:                                   /* Keyed Coupon key */
            if (pData->uchMinorFSCData > FLEX_CPN_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_KEYED_STRING:                                  /* Keyed STRING key */
            if (pData->uchMinorFSCData > FSC_STRING_MAX) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_SEAT:
            if (pData->uchMinorFSCData > FSC_SEAT_MAX) {        /* Seat No. key R3.1 */
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_CHARGE_TIPS:
            if (pData->uchMinorFSCData > FSC_CHGTIP_MAX) {      /* Charge Tips key V3.3 */
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_ASK:
            if (pData->uchMinorFSCData > FSC_ASK_MAX) {         /* Ask key 2172 */
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        case FSC_ADJ_SHIFT:
            if (pData->uchMinorFSCData > FSC_ADJSFT_MAX) {      /* Adjective Shift key 2172 */
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            break;

        default:
            PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_SUP_ERROR);                                              

        }
        MaintWork.FSC.uchMinorFSCData = pData->uchMinorFSCData; /* set minor FSC data */
        return(SUCCESS);                                    
    }
    return (LDT_SEQERR_ADR);
}

/*
*===========================================================================
** Synopsis:    VOID MaintFSCWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function writes & prints FSC data of self-work area.
*===========================================================================
*/

VOID MaintFSCWrite( VOID )
{
    CONST UCHAR uchFSCMaxAddr = MaintQueryKeyBoardFscMax();

    /* check keyboard */
    if (uchFSCMaxAddr < 1) {
        NHPOS_ASSERT_TEXT(0, "==ERROR: MaintFSCWrite() called with invalid keyboard type.");
        return(LDT_PROHBT_ADR);                                /* out of address */
    }

    /* write data */
    CliParaWrite(&(MaintWork.FSC));

    /* set plu no. on plu# table */
    /* correct at 1/23/98 */
    if (MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
		PARAPLUNOMENU   ParaPLUNoMenu = {0};

		ParaPLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
		ParaPLUNoMenu.uchMenuPageNumber = MaintWork.FSC.uchPageNo;
		ParaPLUNoMenu.uchMinorFSCData = MaintWork.FSC.uchAddress;
        ParaPLUNoMenu.PluNo.uchModStat = MaintWork.FSC.PluNo.uchModStat;
        _tcsncpy(ParaPLUNoMenu.PluNo.aszPLUNumber, MaintWork.FSC.PluNo.aszPLUNumber, PLU_MAX_DIGIT+1);
		CliParaWrite(&ParaPLUNoMenu);         /* CLASS_PARAPLUNOMENU - update keyed PLU */
    }

    /* saratoga */
    if (MaintWork.FSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_DEPT) {
		PARADEPTNOMENU   ParaDeptNoMenu = {0};

        ParaDeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;
        ParaDeptNoMenu.uchMenuPageNumber = MaintWork.FSC.uchPageNo;
        ParaDeptNoMenu.uchMinorFSCData = MaintWork.FSC.uchMinorFSCData;
        ParaDeptNoMenu.usDeptNumber = MaintWork.FSC.usDeptNo;
        CliParaWrite(&ParaDeptNoMenu);                           /* call ParaPLUNoMenuWrite() */
    }
    
    /* control header item */
    MaintHeaderCtl(PG_FSC, RPT_PRG_ADR);

    /* set journal bit & receipt bit */
    MaintWork.FSC.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.FSC));

    /* set address for display next address */
    MaintWork.FSC.uchAddress++;          

    /* check address */
    if (MaintWork.FSC.uchAddress > uchFSCMaxAddr) {             
        MaintWork.FSC.uchAddress = FSC_MIN_ADR;                 /* initialize address */
    }
    MaintFSCRead(&(MaintWork.FSC));
}

/*
*===========================================================================
** Synopsis:    VOID MaintFSCReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all FSC data report.
*===========================================================================
*/

VOID MaintFSCReport( VOID )
{
    UCHAR        i,j;
    SHORT        uchAbortFlug;
    CONST UCHAR  uchFSCMaxAddr = MaintQueryKeyBoardFscMax();
	PARAFSC      ParaFSC = {0};

    /* check keyboard */
    if (uchFSCMaxAddr < 1) {
        NHPOS_ASSERT_TEXT(0, "==ERROR: MaintFSCReport() called with invalid keyboard type.");
        return(LDT_PROHBT_ADR);                                /* out of address */
    }

    /* control header item */
    MaintHeaderCtl(PG_FSC, RPT_PRG_ADR);

    /* set data at every page */
    ParaFSC.uchMajorClass = CLASS_PARAFSC;
    ParaFSC.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    for (i = FSC_MIN_PAGE; i <= FSC_MAX_PAGE; i++) {
        ParaFSC.uchPageNo = i;      /* set Page Number */

        /* set data at every address */
        for (j = FSC_MIN_ADR; j <= uchFSCMaxAddr; j++) {

            /* check abort key */
            if ((uchAbortFlug = UieReadAbortKey()) == UIE_ENABLE) {  /* depress abort key */
                MaintMakeAbortKey();
                break;
            }
            ParaFSC.uchAddress = j;
            CliParaRead(&ParaFSC);                                  /* call ParaFscRead() */

            if (ParaFSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
				PARAPLUNOMENU   ParaPLUNoMenu = {0};

                /* read plu no. from plu# table */
                ParaPLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
                ParaPLUNoMenu.uchMenuPageNumber = i;
                ParaPLUNoMenu.uchMinorFSCData = ParaFSC.uchMinorFSCData;
                CliParaRead(&ParaPLUNoMenu);                            /* call ParaDiscRead() */
                _tcsncpy(ParaFSC.PluNo.aszPLUNumber, ParaPLUNoMenu.PluNo.aszPLUNumber, PLU_MAX_DIGIT);
                ParaFSC.PluNo.uchModStat = ParaPLUNoMenu.PluNo.uchModStat;
            }

            if (ParaFSC.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_DEPT) {
				PARADEPTNOMENU   ParaDeptNoMenu = {0};

                /* read dept no. from dept# table */
                ParaDeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;
                ParaDeptNoMenu.uchMenuPageNumber = i;
                ParaDeptNoMenu.uchMinorFSCData = ParaFSC.uchMinorFSCData;
                CliParaRead(&ParaDeptNoMenu);                            /* call ParaDiscRead() */
                ParaFSC.usDeptNo = ParaDeptNoMenu.usDeptNumber;
            }

            PrtPrintItem(NULL, &ParaFSC);
        }
        if(uchAbortFlug == UIE_ENABLE){
            break;
        }
        MaintFeed();
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}

/*
*===========================================================================
** Synopsis:    SHORT   MaintFSCMnem(VOID)
*                                     
*     Input:    
*    Output:    nothing 
*** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Compulsory Display for Mnemonic Entry.          Saratoga
*===========================================================================
*/
static SHORT   MaintFSCMnem(VOID)
{
    SHORT           sStatus, sNum = 0;
    USHORT          usRecLen, usHandle;
    ULONG           i, ulLimit;
    REGDISPMSG      Disp = {0};
    UIFPGDIADATA    UI = {0};
    RECPLU          RecPlu = {0};
    ECMCOND         EcmCond = {0};
    MAINTFSCPLU     Plu[20] = {0};

	{
		PARATRANSHALO   HALO = {0};

		HALO.uchMajorClass = CLASS_PARATRANSHALO;
		HALO.uchAddress = HALO_PLU_DIGIT_ADR;
		CliParaRead(&HALO);

		if (HALO.uchHALO == 0) {                    /* not use this function */
			return(SUCCESS);
		} else if (HALO.uchHALO > 139) {            /* Max PLU Digits */
			ulLimit = 139;
		} else {
			ulLimit = HALO.uchHALO;
		}
	}

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CUSTOMER;
    RflGetTranMnem (Disp.aszMnemonic, TRN_MNEMONIC_ADR);
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    flDispRegDescrControl |= COMPLSRY_CNTRL | CRED_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL | CRED_CNTRL;

    DispWrite(&Disp);

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    if (UifPGDiaMnem(&UI) != SUCCESS) {
        memset(&Disp, 0, sizeof(REGDISPMSG));
        Disp.uchMajorClass = CLASS_UIFREGMODIFIER;
        Disp.uchMinorClass = CLASS_UITABLENO;
        Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
        DispWrite(&Disp);
                                                
        flDispRegDescrControl &= ~CRED_CNTRL; 
        flDispRegKeepControl &= ~CRED_CNTRL; 
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
        return(SUCCESS);
    }

    EcmCond.aucCond[0] = (UCHAR)0;               /* request all plu */
    EcmCond.usLength  = 0;
    EcmCond.fsOption = REPORT_ACTIVE | REPORT_INACTIVE;
    sStatus = SerPluEnterCritMode(FILE_PLU, FUNC_REPORT_COND, &usHandle, &EcmCond);
    if (0 != sStatus) {
        return(LDT_ERR_ADR);
    }

    i=0;
    do {
        sStatus = SerPluReportRecord(usHandle, &RecPlu, &usRecLen);
        if (sStatus == SPLU_END_OF_FILE) {
            if (sNum != 0) {
                MaintFSCDispPlu(Plu, sNum);
                if (MaintFSCMnemClear() != SUCCESS) {
                    break;
                }
            }
            break;
        }
        if (i >= FLEX_PLU_MAX) {
            break;
        }
        if (_tcsncmp(RecPlu.aucMnemonic, UI.aszMnemonics, _tcslen(UI.aszMnemonics)) == 0) {
            _tcsncpy(Plu[sNum].auchPLUNumber, RecPlu.aucPluNumber, PLU_NUMBER_LEN);
            _tcsncpy(Plu[sNum].aszMnemonics, RecPlu.aucMnemonic, PLU_MNEMONIC_LEN);
            sNum++;
        }
        if (sNum == 20) {
            MaintFSCDispPlu(Plu, sNum);
            if (MaintFSCMnemClear() != SUCCESS) {
                break;
            }
            memset(&Plu, 0, sizeof(Plu));
            sNum = 0;
        }
        i++;
    } while (SPLU_COMPLETED == sStatus);

    MldDisplayInit(MLD_SCROLL_1, 0);
    SerPluExitCritMode(usHandle, 0);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    MaintFSCDispPlu(MAINTFSCPLU *pPlu, SHORT sNum)
*                                     
*     Input:    
*    Output:    nothing 
*** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Display 20 Plu on LCD.                          Saratoga
*===========================================================================
*/
static VOID    MaintFSCDispPlu(MAINTFSCPLU *pPlu, SHORT sNum)
{
	TCHAR   auchPlu[PLU_MAX_DIGIT + 2] = {0};
    USHORT  i;

    for (i = 0; i < sNum; i++, pPlu++) {
        RflConvertPLU(auchPlu, pPlu->auchPLUNumber);
        MldPrintf(MLD_SCROLL_1, MLD_SCROLL_APPEND, aszMldFscPlu, pPlu->aszMnemonics, auchPlu);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   MaintFSCMnemClear(VOID)
*                                     
*     Input:    
*    Output:    nothing 
*** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Request Clear Key.                              Saratoga
*===========================================================================
*/
static SHORT   MaintFSCMnemClear(VOID)
{
	REGDISPMSG      Disp = {0};

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CLEAR;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    RflGetLead (Disp.aszMnemonic, LDT_CLEAR_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL | CRED_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL | CRED_CNTRL;
    DispWrite(&Disp);

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    return(UifPGDiaClear());
}

/* --- End of Source File --- */