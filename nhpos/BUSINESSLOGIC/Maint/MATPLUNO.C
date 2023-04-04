/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintPLUNoMenu Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US GP Application Program
* Program Name: MATPLUNO.C
* --------------------------------------------------------------------------
* Abstract: This function read & write & report PLU No. through Action Code 4.
*           AC4 is intended to be used for keyboard type systems only and is
*           a hold over from NCR 2172 and NCR 7448 days before Touchscreen.
*
*           The provided function names are as follows:
*               MaintPLUNoMenuRead()   : read & display PLU No
*               MaintPLUNoMenuWrite()  : set PLU No
*               MaintPLUNoMenuReport() : print all dept No REPORT
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name     : Description
* Jun-23-93: 00.00.01 : M.Ozawa    : initial
* Jul-12-93: 00.00.02 : S.Liu      : Add PLU(13-Line), Scanner
*                                    and E-Version function
* Oct-12-93: 00.00.04 : M.Ozawa    : Enable to set Type 6 - 12 Label
* Oct-15-96: 02.00.00 : T.Yatuhasi : Change MaintPLUNoMenuEdit,MaintPLUNoMenuCheck
*                                    for UPC Velocity Code
* Nov-01-03: 02.00.00 : J. Hall    : fix for Touchscreen and keyboard compatibility.
* Apr-24-15: 02.02.01 : R.Chambers : source cleanup and eliminate duplicated source
*                                    new function MaintQueryKeyBoardFscMax() introduced
*                                    and replacing magic constants with defined constants.
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
#include <uie.h>
#include <pif.h>
#include <log.h>
#include <appllog.h>
#include <paraequ.h>
#include <fsc.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <display.h>
#include <transact.h>
#include <prt.h>
#include <rfl.h>
#include <csstbpar.h>

#include "maintram.h"

/*
*===========================================================================
** Synopsis:    UCHAR  MaintQueryKeyBoardFscMax (VOID)
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads the system configuration data to determine the
*               type of keyboard that is in use and then returning the maximum FSC
*               index for the keyboard to use when indexing into the menu page tables.
*===========================================================================
*/
UCHAR  MaintQueryKeyBoardFscMax (VOID)
{
    UCHAR  uchMinorFSCMax = 0;

    /* check keyboard */
    if (PifSysConfig()->uchKeyType == KEYBOARD_CONVENTION) {  // 7448 Conventional keyboard
        uchMinorFSCMax = FSC_CONV_ADR;
    } else if (PifSysConfig()->uchKeyType == KEYBOARD_MICRO) {  // 7448 Micromotion keyboard
        uchMinorFSCMax = FSC_MICRO_ADR;
    } else if (PifSysConfig()->uchKeyType == KEYBOARD_TOUCH) {  // Touchscreen terminal uses Micromotion tables
        uchMinorFSCMax = FSC_MICRO_ADR;
    } else if (PifSysConfig()->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        uchMinorFSCMax = FSC_CONV_ADR;
    } else if (PifSysConfig()->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        uchMinorFSCMax = FSC_CONV_ADR;
    } else {
        NHPOS_ASSERT_TEXT(0, "==ERROR: MaintFSCRead() called with invalid keyboard type.");
	}

	return uchMinorFSCMax;
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUNoMenuRead( PARAPLUNOMENU *pData )
*
*     Input:    *pData          : pointer to structure for PARAPLUNOMENU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Dept No.
*===========================================================================
*/
SHORT MaintPLUNoMenuRead( PARAPLUNOMENU *pData )
{
    UCHAR           uchMinorFSCData;
    UCHAR           uchMinorFSCMax;

    /* check keyboard */
    uchMinorFSCMax = MaintQueryKeyBoardFscMax ();

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* without data ? */
        uchMinorFSCData = ++MaintWork.PLUNoMenu.uchMinorFSCData;
        if (uchMinorFSCData > uchMinorFSCMax) {
            uchMinorFSCData = FSC_MIN_ADR;                    /* initialize address */
        }
    } else {                                        /* with data */
        uchMinorFSCData = pData->uchMinorFSCData;
    }

    /* check minor FSC data */
    if (uchMinorFSCData < FSC_MIN_ADR || uchMinorFSCMax < uchMinorFSCData) {
        return(LDT_KEYOVER_ADR);                    /* wrong data */
    }
    MaintWork.PLUNoMenu.uchMajorClass = pData->uchMajorClass;
    MaintWork.PLUNoMenu.uchMinorClass = pData->uchMinorClass;
    MaintWork.PLUNoMenu.uchMinorFSCData = uchMinorFSCData;
    MaintWork.PLUNoMenu.uchMenuPageNumber = uchMaintMenuPage;
    CliParaRead(&(MaintWork.PLUNoMenu));

    RflGetLead (MaintWork.PLUNoMenu.aszMnemonics, LDT_DATA_ADR);

    /* reset adjective/E-version flag */
    MaintWork.PLUNoMenu.uchStatus &= ~MAINT_DATA_INPUT;
    MaintWork.PLUNoMenu.uchStatus &= ~MAINT_DATA_INPUT2;
    DispWrite(&(MaintWork.PLUNoMenu));

    /* clear PLU# for next # entry */
    memset(MaintWork.PLUNoMenu.PluNo.aszPLUNumber, 0, sizeof(MaintWork.PLUNoMenu.PluNo.aszPLUNumber));
    MaintWork.PLUNoMenu.PluNo.uchAdjective = 0;
/*    MaintWork.PLUNoMenu.PluNo.uchModStat  &= ~MOD_STAT_EVERSION;   * R2.0 */
    MaintWork.PLUNoMenu.PluNo.uchModStat = 0;                       /* R2.0 */

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUNoMenuWrite( PARAPLUNOMENU *pData )
*
*     Input:    *pData             : pointer to structure for PARAPLUNOMENU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints PLU No.
*===========================================================================
*/
SHORT MaintPLUNoMenuWrite( PARAPLUNOMENU *pData )
{
	PARAPLU    ParaPLU = {0};
    UCHAR      ii;
    UCHAR      uchMinorFSCMax;
    UCHAR      uchStatus = 0;

    /* check input data */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* without data */
        return(LDT_SEQERR_ADR);                     /* sequence error */
    }

    if(pData->uchStatus & MAINT_SCANNER_INPUT)      /* input from scanner ? */
        ParaPLU.uchStatus |= MAINT_SCANNER_INPUT;

    _tcscpy(ParaPLU.auchPLUNumber, pData->PluNo.aszPLUNumber);

    if (MaintPLUNoMenuCheck(&ParaPLU) != SUCCESS) { /* Label analize */
        return(LDT_KEYOVER_ADR);                    /* illegal data input error */
    }

	/* check PLU No. has at least one non-zero digit. do not test check sum digit */
    for (ii = 0; ii < (PLU_MAX_DIGIT-1); ii++) {
        if (ParaPLU.auchPLUNumber[ii] > 0x30) { /* not 0 ? */
            uchStatus = 1;
        }
    }
    if (!uchStatus) {                               /* PLU# all 0 ? */
        if (MaintWork.PLUNoMenu.uchStatus & MAINT_DATA_INPUT) { /* if adjective and E-Version key */
            return(LDT_KEYOVER_ADR);    /* yes, error */
        }
        memset(&ParaPLU.auchPLUNumber, 0x00, sizeof(ParaPLU.auchPLUNumber)); /* cler ascii data */
    }

    /* check minor class */
    switch(pData->uchMinorClass) {

    case CLASS_PARAPLUNOMENU_KEYEDPLU:
        MaintWork.PLUNoMenu.uchMajorClass = pData->uchMajorClass;
        MaintWork.PLUNoMenu.uchMinorFSCData = pData->uchMinorFSCData;
        MaintWork.PLUNoMenu.uchMenuPageNumber = uchMaintMenuPage;
        /* break */

    case CLASS_PARAPLUNOMENU_PLUNO:
        _tcsncpy(MaintWork.PLUNoMenu.PluNo.aszPLUNumber, ParaPLU.auchPLUNumber, PLU_MAX_DIGIT);
		MaintWork.PLUNoMenu.PluNo.aszPLUNumber[PLU_MAX_DIGIT] = 0;  // ensure end of text string is set.
        break;

    default:
        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
    }

    MaintWork.PLUNoMenu.uchMinorClass = pData->uchMinorClass;

    CliParaWrite(&(MaintWork.PLUNoMenu));

    /* control header item */
    MaintHeaderCtl(AC_PLU_MENU, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.PLUNoMenu.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.PLUNoMenu));

    /* set minor FSC data for next display */
    MaintWork.PLUNoMenu.uchMinorFSCData++;

    /* check keyboard */
    uchMinorFSCMax = MaintQueryKeyBoardFscMax ();
    /* check address */
    if (MaintWork.PLUNoMenu.uchMinorFSCData > uchMinorFSCMax) {
        MaintWork.PLUNoMenu.uchMinorFSCData = 1;                /* initialize address */
    }
    MaintPLUNoMenuRead(&(MaintWork.PLUNoMenu));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUNoMenuEdit( PARAPLUNOMENU *pData )
*
*     Input:    *pData             : pointer to structure for PARAPLUNOMENU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets PLU adjective No.
*===========================================================================
*/
SHORT MaintPLUNoMenuEdit( PARAPLUNOMENU *pData )
{

    switch (pData->uchMinorClass){
#if 0
    case CLASS_PARAPLUNOMENU_ADJECTIVE:
        if (pData->PluNo.uchAdjective == 0) {
           return(LDT_KEYOVER_ADR);
        }
        if (pData->PluNo.uchAdjective > MAX_ADJECTIVE_NO) {
           return(LDT_KEYOVER_ADR);
        }
        MaintWork.PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_ADJECTIVE;

        /* clear adjective, if same adjective key is depressed */

        if (MaintWork.PLUNoMenu.PluNo.uchAdjective == pData->PluNo.uchAdjective) {

            MaintWork.PLUNoMenu.PluNo.uchAdjective = 0;
            MaintWork.PLUNoMenu.uchStatus &= ~MAINT_DATA_INPUT;
        } else {
            MaintWork.PLUNoMenu.PluNo.uchAdjective = pData->PluNo.uchAdjective;
            MaintWork.PLUNoMenu.uchStatus |= MAINT_DATA_INPUT;
        }
        break;
#endif
    case CLASS_PARAPLUNOMENU_EVERSION:
        MaintWork.PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_EVERSION;

        /* clear E-VERSION, if E-VERSION key is depressed repeatedly */
        if (MaintWork.PLUNoMenu.PluNo.uchModStat & MOD_STAT_EVERSION) {

/*            MaintWork.PLUNoMenu.PluNo.uchModStat &= ~MOD_STAT_EVERSION; * R2.0 */
            MaintWork.PLUNoMenu.PluNo.uchModStat = 0;                    /* R2.0 */
            MaintWork.PLUNoMenu.uchStatus &= ~MAINT_DATA_INPUT2;
        } else {
/*            MaintWork.PLUNoMenu.PluNo.uchModStat |= MOD_STAT_EVERSION;  * R2.0 */
            MaintWork.PLUNoMenu.PluNo.uchModStat = MOD_STAT_EVERSION;    /* R2.0 */
            MaintWork.PLUNoMenu.uchStatus |= MAINT_DATA_INPUT2;
        }
        break;

#if 0
    /* R2.0 Start */
    case CLASS_PARAPLUNOMENU_VELOCITY:
        /* if Velocity # is entered, check E-Version Status */
        if ((pData->uchStatus & MAINT_WITHOUT_DATA) == 0) {
            if (MaintWork.PLUNoMenu.PluNo.uchModStat & MOD_STAT_EVERSION) {
                return(LDT_KEYOVER_ADR);    /* illegal data input error */
            }
        }

        MaintWork.PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_VELOCITY;

        /* clear VELOCITY, if VELOCITY key is depressed repeatedly */

        if (MaintWork.PLUNoMenu.PluNo.uchModStat & MOD_STAT_VELOCITY) {

            if (pData->uchStatus & MAINT_WITHOUT_DATA) { /* not reset if # is entered */
                MaintWork.PLUNoMenu.PluNo.uchModStat = 0;
                MaintWork.PLUNoMenu.uchStatus &= ~MAINT_DATA_INPUT2;
            }
        } else {
            MaintWork.PLUNoMenu.PluNo.uchModStat = MOD_STAT_VELOCITY;
            MaintWork.PLUNoMenu.uchStatus |= MAINT_DATA_INPUT2;
        }

        /* if Velocity # is entered, return without display */
        if ((pData->uchStatus & MAINT_WITHOUT_DATA) == 0) {
            return(SUCCESS);
        }
        break;
    /* R2.0 End   */
#endif

    default:
        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
    }

    /* Set Leadthru Data */
    RflGetLead (MaintWork.PLUNoMenu.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.PLUNoMenu));
    return(SUCCESS);

}
/*
*===========================================================================
** Synopsis:    VOID MaintPLUNoMenuReport( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all PLU No. REPORT.
*===========================================================================
*/
VOID MaintPLUNoMenuReport( VOID )
{
    UCHAR            i;
    UCHAR            uchMinorFSCMax;

    /* control header item */
    MaintHeaderCtl(AC_PLU_MENU, RPT_ACT_ADR);

    /* check keyboard */
    uchMinorFSCMax = MaintQueryKeyBoardFscMax ();

    /* set data every pages */
    for (i = 1; i <= FSC_MAX_PAGE; i++) {
       PARAPLUNOMENU    ParaPLUNoMenu = {0};
       UCHAR    j;

       ParaPLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
       ParaPLUNoMenu.uchMenuPageNumber = i;
       ParaPLUNoMenu.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
       for (j = FSC_MIN_ADR; j <= uchMinorFSCMax; j++) {  /* set data at every minor FSC data */
            if (UieReadAbortKey() == UIE_ENABLE) {  /* check abort key */
                MaintMakeAbortKey();
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
                return;
            }
            ParaPLUNoMenu.uchMinorFSCData = j;
            CliParaRead(&ParaPLUNoMenu);
            PrtPrintItem(NULL, &ParaPLUNoMenu);
        }
    }
    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPLUNoMenuRedisplay( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays PLU No. after Clear Key.
*===========================================================================
*/
VOID MaintPLUNoMenuRedisplay( VOID )
{
    MaintWork.PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_ADDRESS;
    MaintPLUNoMenuRead(&(MaintWork.PLUNoMenu));
}


/*
*===========================================================================
** Synopsis:    SHORT MaintPLUNoMenuCheck( PARAPLU * ParaPLU )
*
**    Input:    *ParaPLU       : pointer to structure for PARAPLU
*    Output:    Nothing
*     InOut:    Nothing
*
**  Return:     SUCCESS        : successful
**              LDT_SEQERR_ADR : sequence error
**              LDT_KEYOVER_ADR: wrong data
**              sReturn        : error status
*
** Description: This function checks PLU No. which is read from scanner.
*===========================================================================
*/
SHORT MaintPLUNoMenuCheck( PARAPLU *pParaPLU )
{
    SHORT           sReturn;
	LABELANALYSIS   LabelAnalysis = {0};
    
#if 0
    /* R2.0 Start */
    if (MaintWork.PLUNoMenu.PluNo.uchModStat & (UCHAR)MOD_STAT_EVERSION) {
        if (MaintWork.PLUNoMenu.PluNo.uchModStat & (UCHAR)MOD_STAT_VELOCITY) {
            return(LDT_KEYOVER_ADR);    /* illegal data input error */
        }
    }
    if (MaintWork.PLUNoMenu.PluNo.uchModStat & (UCHAR)MOD_STAT_VELOCITY) {
        if (pParaPLU->uchStatus & MAINT_SCANNER_INPUT) {
            return(LDT_KEYOVER_ADR);    /* illegal data input error */
        }

        if (strlen(pParaPLU->aszPLUNum) > 6) {
            return(LDT_KEYOVER_ADR);    /* illegal data input error */
        }
    }
    /* R2.0 End   */
#endif

    _tcscpy(LabelAnalysis.aszScanPlu, pParaPLU->auchPLUNumber);

    if (MaintWork.PLUNoMenu.PluNo.uchModStat & (UCHAR)MOD_STAT_EVERSION) {
        LabelAnalysis.fchModifier|=LA_EMOD;     /* Set E-Version status */
    }
    
#if 0
    /* R2.0 Start */
    if (MaintWork.PLUNoMenu.PluNo.uchModStat & (UCHAR)MOD_STAT_VELOCITY) {
        LabelAnalysis.fchModifier|=LA_UPC_VEL;      /* Set Velocity status */
    }
    /* R2.0 End   */
#endif

    if (pParaPLU->uchStatus & MAINT_SCANNER_INPUT)
        LabelAnalysis.fchModifier|=LA_SCANNER;  /* Set Scanner Input status */

    sReturn = RflLabelAnalysis(&LabelAnalysis);
    if (sReturn == LABEL_ERR) {
        return(LDT_KEYOVER_ADR);    /* illegal data input error */
    }

/**********
    if ((sReturn == LABEL_ERR) ||
            ((LabelAnalysis.uchType == LABEL_RANDOM) &&
            (LabelAnalysis.uchLookup == LA_NOT_LOOKUP))) {  / PLU label analyze /
        return(LDT_KEYOVER_ADR);    / illegal data input error /
    }
***********/

    /* convert scaner entry data to manual entry data */
    if (pParaPLU->uchStatus & MAINT_SCANNER_INPUT) {
        memset(pParaPLU->auchPLUNumber, 0, sizeof(pParaPLU->auchPLUNumber));
        sReturn = RflConvScan2Manual (pParaPLU->auchPLUNumber, LabelAnalysis.aszScanPlu);
        if ((sReturn == MOD_STAT_EVERSION) ||
           (LabelAnalysis.fchModifier & LA_EMOD)) {
            MaintWork.PLUNoMenu.PluNo.uchModStat |= (UCHAR)MOD_STAT_EVERSION;
        }
    }

    return (SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID MaintPLUNoMenuReplace( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function relocate PLU No. position in PLU No. table.
*===========================================================================
*/
VOID MaintPLUNoMenuReplace( VOID )
{
    UCHAR           i, j, uchPLUKey;
    USHORT          usRetLen;
    PARAPLUNOMENU   ParaPLUNoMenu;
    PARAFSC         ParaFSC;
    PLUNO           PLUNo[MAX_FSC_NO];

    ParaPLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
    ParaFSC.uchMajorClass = CLASS_PARAFSC;

    /* set data every menu pages, really only applies to keyboard type systems */
    for (i = 1; i <= FSC_MAX_PAGE; i++) {       // SR 466, if we use the total number of menu pages,
												// we begin to go out of the bounds of ParaPluNoMenu, because
												// there is not enough size for the last two pages, due to them
												// being touch screens. JHHJ see MAX_PLUKEY_SIZE define.

        CliParaAllRead(CLASS_PARAPLUNOMENU, (UCHAR *)PLUNo,
                    (USHORT)(FSC_MAX_ADR * sizeof(PLUNO)),
                    (USHORT)(FSC_MAX_ADR * sizeof(PLUNO) * (i-1)),
                    &usRetLen);

        ParaFSC.uchPageNo = i;
        ParaPLUNoMenu.uchMenuPageNumber = i;

        /* set data at every minor FSC data */
        for (j = 1; j <= FSC_MAX_ADR; j++) {
            ParaFSC.uchAddress = j;
            ParaPLUNoMenu.uchMinorFSCData = j;
            CliParaRead(&ParaFSC);

            if ((ParaFSC.uchMajorFSCData + UIE_FSC_BIAS) == FSC_KEYED_PLU) {
                /* relocate plu no. */
                uchPLUKey = ParaFSC.uchMinorFSCData;
                if (uchPLUKey) uchPLUKey--;
                ParaPLUNoMenu.PluNo = PLUNo[uchPLUKey];
                CliParaWrite(&ParaPLUNoMenu);

                ParaFSC.uchMinorFSCData = j;
                CliParaWrite(&ParaFSC);
            } else {
                /* clear plu no. */
                memset(&ParaPLUNoMenu.PluNo, 0, sizeof(PLUNO));
                CliParaWrite(&ParaPLUNoMenu);
            }
        }
    }

}

/************************************************
**  END OF PROGRAM                             **
************************************************/
