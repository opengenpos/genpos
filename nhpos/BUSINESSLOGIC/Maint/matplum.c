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
* Title       : Assign Set Menu Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPLUM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                                            
*               MaintMenuPLURead()      : Read & Display Set Menu
*               MaintMenuPLUEdit()      : Edit Set Menu
*               MaintMenuPLUWrite()     : Assign Set Menu
*               MaintMenuPLUReport()    : Print All Set Menu
*               MaintMenuPLUDelete()    : Delete Set Menu
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-10-92: 00.00.01 : K.You     : initial                                   
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
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbopr.h>
#include <csstbpar.h>
#include <mld.h>
#include "maintram.h" 
#include "rfl.h"

UCHAR uchMenuEVersion;

/*
*===========================================================================
** Synopsis:    SHORT MaintMenuPLURead( PARAMENUPLUTBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARAMENUPLUTBL
*    Output:    Nothing 
*
** Return:      SUCCESS          : Successful
*               LDT_KEYOVER_ADR  : Wrong Data
*
** Description: This Function Reads & Displays Set Menu.
*===========================================================================
*/

SHORT MaintMenuPLURead( PARAMENUPLUTBL *pData )
{
    SHORT           sError;
    PARAPLU         ParaPLU = {0};

    /* Check W/o Input */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* W/o Amount */
        uchMenuEVersion = 0;
        return(LDT_SEQERR_ADR);
    }

    /* PLU Number has already checked by UI */

    /* Check PLU Number */

    /* Analyze PLU Label & Convert (ParaPLU->aucPLUNumber : PLU number) */
    _tcsncpy(ParaPLU.auchPLUNumber, pData->SetPLU[0].aszPLUNumber, PLU_MAX_DIGIT+1);
    ParaPLU.uchStatus = pData->uchStatus;
    sError = MaintPluLabelAnalysis(&ParaPLU, uchMenuEVersion);
    uchMenuEVersion = 0;
    if (sError != SUCCESS) {
        return(sError);
    }
    if (_tcsncmp(ParaPLU.auchPLUNumber, MLD_ZERO_PLU, PLU_MAX_DIGIT) == 0) {
        return(LDT_KEYOVER_ADR);
    }

    /* Search Set Menu */

    memset(&MaintWork, 0, sizeof(MAINTWORK));
    MaintWork.MenuPLUTbl.uchMajorClass = pData->uchMajorClass;      /* Set Major Class */
	_tcsncpy(MaintWork.MenuPLUTbl.SetPLU[0].aszPLUNumber, ParaPLU.auchPLUNumber, PLU_MAX_DIGIT+1);
    MaintWork.MenuPLUTbl.uchAddress = 0;
    MaintWork.MenuPLUTbl.uchArrayAddr = 0;
    
    /* Set Lead Thru for Display */
	RflGetLead(MaintWork.MenuPLUTbl.aszLeadMnemonics, LDT_DATA_ADR);
           
    sError = CliParaMenuPLURead(&(MaintWork.MenuPLUTbl));
    if ((sError == SUCCESS) || (sError == LDT_NTINFL_ADR)) {
		PARAMENUPLUTBL  MldParaMenu = {0};    /* FOR MLD */

        if (MaintWork.MenuPLUTbl.uchAddress == 0) {
            return(LDT_FLFUL_ADR);
        }
        sError = SUCCESS;
        MaintWork.MenuPLUTbl.uchPageNumber = MaintGetPage();

        /******************/
        /* Display to MLD */
        /******************/
        MldParaMenu = MaintWork.MenuPLUTbl;
        MldParaMenu.uchArrayAddr = 6;
        MldDispItem(&MldParaMenu,0);

        DispWrite(&(MaintWork.MenuPLUTbl));
    }
    return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintMenuPLUEdit( PARAMENUPLUTBL *pData )
*               
*     Input:    *pData             : pointer to structure for PARAMENUPLUTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : Successful 
*               LDT_KEYOVER_ADR    : Wrong Data Error
*
** Description: This Function Edit Set Menu.
*===========================================================================
*/

SHORT MaintMenuPLUEdit( MAINTMENUPLUTBL *pData )
{

    UCHAR               uchArrayAddr;
    SHORT               sPrintFlag = 0;
    PLUIF               PLUIf = { 0 };
    MAINTMENUPLUTBL     MaintMenuPLU = { 0 };
    PARAMENUPLUTBL      MldParaMenu = { 0 };
    PARAPLU             ParaPLU = {0};
    SHORT               sError;

    /* Check Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_PARAMENUPLU_READ:                                    /* Operate Address Case */
        /* Check Status */                                          
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
            uchArrayAddr = ++MaintWork.MenuPLUTbl.uchArrayAddr;

            /* Check Array Address */
            if (MAX_SETPLU_DATA <= uchArrayAddr) { 
                uchArrayAddr = 1;                                   /* Initialize Array Address */
            } 
        } else {                                                    /* W/ Amount */
            uchArrayAddr = pData->uchArrayAddr;
        }

        /* Check Array Address */
        if ((uchArrayAddr == 0) || (MAX_SETPLU_DATA <= uchArrayAddr)) {
            return(LDT_KEYOVER_ADR);                                
        }
        MaintWork.MenuPLUTbl.uchArrayAddr = uchArrayAddr;
        break;

    case CLASS_PARAMENUPLU_PLUWRITE:                                /* Operate PLU No. Case */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
            uchMenuEVersion = 0;
            return(LDT_SEQERR_ADR);
        }

        /* PLU Number has already checked by UI */
        /* Analyze PLU Label & Convert (ParaPLU->aucPLUNumber : PLU number) */
        _tcsncpy(ParaPLU.auchPLUNumber, pData->SetPLU.aszPLUNumber, PLU_MAX_DIGIT+1);
        ParaPLU.uchStatus = pData->uchStatus;
        sError = MaintPluLabelAnalysis(&ParaPLU, uchMenuEVersion);
        uchMenuEVersion = 0;
        if (sError != SUCCESS) {
            return(sError);
        }
        if (_tcsncmp(ParaPLU.auchPLUNumber, MLD_ZERO_PLU, PLU_MAX_DIGIT) == 0) {
            memset(ParaPLU.auchPLUNumber, 0, sizeof(ParaPLU.auchPLUNumber));
        }

        _tcsncpy(MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].aszPLUNumber, ParaPLU.auchPLUNumber, PLU_MAX_DIGIT);
        MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].uchAdjective = 0;

        /******************/
        /* Display to MLD */
        /******************/
        MldParaMenu = MaintWork.MenuPLUTbl;
        MldDispItem(&MldParaMenu,0);

        break;

    case CLASS_PARAMENUPLU_ADJWRITE:                                /* Operate Adjective Case */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
            if (!MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].uchAdjective) {     /* PLU Adjective Not Exist */ 
                pData->SetPLU.uchAdjective = 0;
            } else {
                pData->SetPLU.uchAdjective = MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].uchAdjective;
            }
        }

        /* Check Adjective Number */
        if (MAX_ADJECTIVE_NO < pData->SetPLU.uchAdjective) {         /* Out of Range */
            return(LDT_KEYOVER_ADR);                                                                    
        }

        MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].uchAdjective = pData->SetPLU.uchAdjective;

        /* Control Header Item */
        if (uchMaintOpeCo & MAINT_ALREADYOPERATE) {              /* in case of not 1st operation */
            sPrintFlag = 1;
        }
        MaintHeaderCtl(AC_SET_PLU, RPT_ACT_ADR);

        /* Get PLU Mnemonics */

        _tcsncpy(PLUIf.auchPluNo, MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].aszPLUNumber, PLU_MAX_DIGIT);
        PLUIf.uchPluAdj = 1;
        memset(MaintMenuPLU.aszPLUMnemonics, '\0', sizeof(MaintMenuPLU.aszPLUMnemonics));
        if (CliOpPluIndRead(&PLUIf, usMaintLockHnd) == OP_PERFORM) {
            _tcsncpy(MaintMenuPLU.aszPLUMnemonics, PLUIf.ParaPlu.auchPluName, PARA_PLU_LEN);
        }

        /* Print Set Menu */
        MaintMenuPLU.uchMajorClass = CLASS_PARAMENUPLUTBL;
        MaintMenuPLU.uchArrayAddr = MaintWork.MenuPLUTbl.uchArrayAddr;
        _tcsncpy(MaintMenuPLU.SetPLU.aszPLUNumber, MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].aszPLUNumber, PLU_MAX_DIGIT);
        MaintMenuPLU.SetPLU.uchAdjective = MaintWork.MenuPLUTbl.SetPLU[MaintWork.MenuPLUTbl.uchArrayAddr].uchAdjective;
        MaintMenuPLU.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        if (sPrintFlag && (MaintMenuPLU.uchArrayAddr == 0)) {
            MaintFeed();
        }

        /******************/
        /* Display to MLD */
        /******************/
        MldParaMenu = MaintWork.MenuPLUTbl;
        MldDispItem(&MldParaMenu,0);

        PrtPrintItem(NULL, &MaintMenuPLU);
        
        /* Advanced Next Array Address */
        if ((++MaintWork.MenuPLUTbl.uchArrayAddr) >= MAX_SETPLU_DATA) {
            MaintWork.MenuPLUTbl.uchArrayAddr = 1;
        }
        break;

    case CLASS_PARAMENUPLU_PAGE:                                    /* Operate Page Case */
        /* Check W/o Amount Case */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount Case */
            uchMaintMenuPage++;
            if (uchMaintMenuPage > MAX_PAGE_NO) {               
                uchMaintMenuPage = 1;                               /* Initialize Menu Page */
            }
        } else {                                                    /* W/ Amount Case */
            if (pData->uchPageNumber == 0 || pData->uchPageNumber > MAX_PAGE_NO) {  /* Over Digit */
                return(LDT_KEYOVER_ADR);                                            
            }

            /* Save Menu Page Data */
            uchMaintMenuPage = pData->uchPageNumber;
        }
        break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);
    }

    /* Set Lead Thru for Display */
    if (pData->uchMinorClass == CLASS_PARAMENUPLU_PLUWRITE) {
		RflGetLead(MaintWork.MenuPLUTbl.aszLeadMnemonics, LDT_DATA_ADR);
	} else {
		RflGetLead(MaintWork.MenuPLUTbl.aszLeadMnemonics, LDT_NUM_ADR);
    }

    MaintWork.MenuPLUTbl.uchPageNumber = MaintGetPage();
    DispWrite(&(MaintWork.MenuPLUTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintMenuPLUWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Sets Set Menu Data.
*===========================================================================
*/

VOID MaintMenuPLUWrite( VOID )
{
    /* Set Set Menu to Parameter Module */
    CliParaMenuPLUWrite(SET_DATA, &(MaintWork.MenuPLUTbl));                        
}

/*
*===========================================================================
** Synopsis:    VOID MaintMenuPLUReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing 
*
** Description: This Function Prints All Set Menu Data.
*===========================================================================
*/

VOID MaintMenuPLUReport( VOID )
{
    UCHAR               i;
    UCHAR               j;
    SHORT               sPrintFlag = 0;
    PLUIF               PLUIf = { 0 };
    PARAMENUPLUTBL      ParaMenuPLU = { 0 };
	MAINTMENUPLUTBL     MaintMenuPLU = { 0 };

    /* Control Header Item */
        
    MaintHeaderCtl(AC_SET_PLU, RPT_ACT_ADR);

    ParaMenuPLU.uchMajorClass = CLASS_PARAMENUPLUTBL;                       /* Set Major Class */
    MaintMenuPLU.uchMajorClass = CLASS_PARAMENUPLUTBL;                      /* Set Major Class */

    /* Set Journal Bit & Receipt Bit */

    MaintMenuPLU.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Print Data for Each Address */

    for (i = 1; i <= MAX_SET_MENU; i++) {
        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {                              /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }
        ParaMenuPLU.uchAddress = i;
        CliParaMenuPLURead(&ParaMenuPLU);

        /* Check Parent PLU Exist */
        /* Print Each PLU of Set Menu */
        if (_tcsncmp(ParaMenuPLU.SetPLU[0].aszPLUNumber, MLD_NO_DISP_PLU_DUMMY, PLU_MAX_DIGIT) == 0) {                   /* Not Exist */
            continue;                                                                   
        }
        if (sPrintFlag == 0 ) {
            sPrintFlag = 1;
        } else {
            MaintFeed();
        }
        for (j = 0; j < MAX_SETPLU_DATA; j++) {

            memset(MaintMenuPLU.aszPLUMnemonics, '\0', sizeof(MaintMenuPLU.aszPLUMnemonics));        /* Initialize Buffer */

            /* Get PLU Mnemonics */
            _tcsncpy(PLUIf.auchPluNo, ParaMenuPLU.SetPLU[j].aszPLUNumber, PLU_MAX_DIGIT);
            PLUIf.uchPluAdj = 1;
            if (CliOpPluIndRead(&PLUIf, usMaintLockHnd) == OP_PERFORM) {
                _tcsncpy(MaintMenuPLU.aszPLUMnemonics, PLUIf.ParaPlu.auchPluName, PARA_PLU_LEN);
            }
            MaintMenuPLU.uchArrayAddr = j;
            _tcsncpy(MaintMenuPLU.SetPLU.aszPLUNumber, ParaMenuPLU.SetPLU[j].aszPLUNumber, PLU_MAX_DIGIT);
            MaintMenuPLU.SetPLU.uchAdjective = ParaMenuPLU.SetPLU[j].uchAdjective;
            PrtPrintItem(NULL, &MaintMenuPLU);
        }
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}
    
/*
*===========================================================================
** Synopsis:    SHORT MaintMenuPLUDelete( PARAMENUPLUTBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARAMENUPLUTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : Successful
*               LDT_KEYOVER_ADR  : Wrong Data Error
*
** Description: This Function Prints and Deletes Set Menu.
*===========================================================================
*/

SHORT MaintMenuPLUDelete( PARAMENUPLUTBL *pData )
{
    UCHAR           i;
    SHORT           sError;
    PLUIF           PLUIf = { 0 };
    MAINTTRANS      MaintTrans = { 0 };
    PARAMENUPLUTBL  ParaMenuPLU = { 0 };
    MAINTMENUPLUTBL MaintMenuPLU = { 0 };
	PARAPLU         ParaPLU = { 0 };

    /* Initialize Work Buffer */

    /* PLU Number has already checked by UI */

    /* Check PLU Number */

    /* Analyze PLU Label & Convert (ParaPLU->aucPLUNumber : PLU number) */
    _tcsncpy(ParaPLU.auchPLUNumber, pData->SetPLU[0].aszPLUNumber, PLU_MAX_DIGIT+1);
    ParaPLU.uchStatus = pData->uchStatus;
    sError=MaintPluLabelAnalysis(&ParaPLU, uchMenuEVersion);
    uchMenuEVersion = 0;
    if (sError != SUCCESS) {
        return(sError);
    }
    if (_tcsncmp(ParaPLU.auchPLUNumber, MLD_ZERO_PLU, PLU_MAX_DIGIT) == 0) {
        return(LDT_KEYOVER_ADR);
    }

    /* Search PLU Number from Set Menu */
    ParaMenuPLU.uchMajorClass = pData->uchMajorClass;           /* Set Major Class */
    _tcsncpy(ParaMenuPLU.SetPLU[0].aszPLUNumber, ParaPLU.auchPLUNumber, PLU_MAX_DIGIT);
    ParaMenuPLU.uchAddress = 0;
    if ((sError = CliParaMenuPLURead(&ParaMenuPLU)) != SUCCESS) {
        return(sError);
    }

    /* Print "DELETE" */
    if (uchMaintOpeCo & MAINT_ALREADYOPERATE) {              /* in case of not 1st operation */
        MaintFeed();
    } else {
        MaintHeaderCtl(AC_SET_PLU, RPT_ACT_ADR);
    }
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
	RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_DEL_ADR);
    PrtPrintItem(NULL, &MaintTrans);

    /* Print Each PLU of Set Menu */
    MaintMenuPLU.uchMajorClass = CLASS_PARAMENUPLUTBL;
    MaintMenuPLU.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    for (i = 0; i < MAX_SETPLU_DATA; i++) {
        memset(MaintMenuPLU.aszPLUMnemonics, '\0', sizeof(MaintMenuPLU.aszPLUMnemonics));         /* Initialize Buffer */
                                                                            
        /* Get PLU Mnemonics */
        _tcsncpy(PLUIf.auchPluNo, ParaMenuPLU.SetPLU[i].aszPLUNumber, PLU_MAX_DIGIT);

        /* Check PLU type by uchAdjective Field */

        /* if ((PLUIf.uchPluAdj = ParaMenuPLU.SetPLU[i].uchAdjective) == 0) {  
            PLUIf.uchPluAdj = 1;
        } */

        PLUIf.uchPluAdj = 1;
        if (CliOpPluIndRead(&PLUIf, usMaintLockHnd) == OP_PERFORM) {
            _tcsncpy(MaintMenuPLU.aszPLUMnemonics, PLUIf.ParaPlu.auchPluName, PARA_PLU_LEN);
        }
        MaintMenuPLU.uchArrayAddr = i;
        _tcsncpy(MaintMenuPLU.SetPLU.aszPLUNumber, ParaMenuPLU.SetPLU[i].aszPLUNumber, PLU_MAX_DIGIT);
        MaintMenuPLU.SetPLU.uchAdjective = ParaMenuPLU.SetPLU[i].uchAdjective;
        PrtPrintItem(NULL, &MaintMenuPLU);
    }

    /* Delete Set Menu */
    memset(&(ParaMenuPLU.SetPLU), 0, sizeof(ParaMenuPLU.SetPLU));
    CliParaMenuPLUWrite(RESET_DATA, &ParaMenuPLU);                        

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintMenuPLUModifier( UCHAR uchMinorClass )
*
*     Input:    *pData              : pointer to structure for MAINTPLU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*
** Description: This function controls modifier status.
*===========================================================================
*/
SHORT MaintMenuPLUModifier( UCHAR uchMinorClass )
{

    switch(uchMinorClass) {
    case CLASS_PARAPLU_EVERSION:
        uchMenuEVersion = (UCHAR)((uchMenuEVersion & MOD_STAT_EVERSION) ? 0 : MOD_STAT_EVERSION);   /* toggle control */
    case CLASS_PARAPLU_SET_EV:
        uchMenuEVersion |= MOD_STAT_EVERSION;
        break;

    case CLASS_PARAPLU_RESET_EV:
        uchMenuEVersion &= ~MOD_STAT_EVERSION;
        break;

    default:
        break;
    }
    return(SUCCESS);
}
