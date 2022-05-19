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
* Title       : MaintCtlTblMenu Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCTLT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report CONTROL MENU PAGE NO.
*
*           The provided function names are as follows: 
* 
*              MaintCtlTblMenuRead()   : read & display CONTROL MENU PAGE NO.
*              MaintCtlTblMenuWrite()  : set & print CONTROL MENU PAGE NO.
*              MaintCtlTblMenuReport() : print all CONTROL MENU PAGE NO.   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-10-92: 00.00.01 : J.Ikeda   : initial                                   
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

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "rfl.h"
#include "csstbpar.h"
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintCtlTblMenuRead( PARACTLTBLMENU *pData )
*               
*     Input:    *pData          : pointer to structure for PARACTLTBLMENU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays CONTROL MENU PAGE NO.
*===========================================================================
*/

SHORT MaintCtlTblMenuRead( PARACTLTBLMENU *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.CtlTblMenu.uchAddress;

        /* check address */
        if (uchAddress > CTLPG_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || CTLPG_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.CtlTblMenu.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.CtlTblMenu.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.CtlTblMenu));                       /* call ParaCtlTblMenuRead() */

	RflGetLead(MaintWork.CtlTblMenu.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.CtlTblMenu));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCtlTblMenuWrite( PARACTLTBLMENU *pData )
*               
*     Input:    *pData             : pointer to structure for PARACTLTBLMENU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints CONTROL MENU PAGE NO.
*===========================================================================
*/

SHORT MaintCtlTblMenuWrite( PARACTLTBLMENU *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /* check address */
    
    switch (MaintWork.CtlTblMenu.uchAddress) {
    case CTLPG_STAND_ADR:
    /* if (MaintWork.CtlTblMenu.uchAddress == CTLPG_STAND_ADR) { */
        
        /* check range */

        if (pData->uchPageNumber < MAINT_CTLPG_MIN || MAINT_CTLPG_MAX < pData->uchPageNumber) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    case CTLPG_ADJ_PAGE1_ADR: /* 2172 */
    case CTLPG_ADJ_PAGE2_ADR:
    case CTLPG_ADJ_PAGE3_ADR:
    case CTLPG_ADJ_PAGE4_ADR:
    case CTLPG_ADJ_PAGE5_ADR:
    case CTLPG_ADJ_PAGE6_ADR:
    case CTLPG_ADJ_PAGE7_ADR:
    case CTLPG_ADJ_PAGE8_ADR:
    case CTLPG_ADJ_PAGE9_ADR:
        
        /* check range */
        if (MAINT_CTLPG_ADJ_MAX < pData->uchPageNumber) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    default:
    /* } else { */

        /* check range */

        if ((pData->uchPageNumber != MAINT_CTLPG_ALLOW) && (pData->uchPageNumber != MAINT_CTLPG_PROHIBIT)) { 
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;
    }
    MaintWork.CtlTblMenu.uchPageNumber = pData->uchPageNumber;
    CliParaWrite(&(MaintWork.CtlTblMenu));                      /* call ParaCtlTblMenuWrite() */

    /* control header item */
        
    MaintHeaderCtl(AC_SET_CTLPG, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.CtlTblMenu.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.CtlTblMenu));
    
    /* set address for display next address */

    /* check address */

    MaintWork.CtlTblMenu.uchAddress++;

    if (MaintWork.CtlTblMenu.uchAddress > CTLPG_ADR_MAX) {
        MaintWork.CtlTblMenu.uchAddress = 1;                    /* initialize address */
    }
    MaintCtlTblMenuRead(&(MaintWork.CtlTblMenu));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintCtlTblMenuReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all CONTROL MENU PAGE NO.
*===========================================================================
*/

VOID MaintCtlTblMenuReport( VOID )
{
    UCHAR            i;
    PARACTLTBLMENU   ParaCtlTblMenu;


    /* control header item */
        
    MaintHeaderCtl(AC_SET_CTLPG, RPT_ACT_ADR);
    ParaCtlTblMenu.uchMajorClass = CLASS_PARACTLTBLMENU;

    /* set journal bit & receipt bit */

    ParaCtlTblMenu.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= CTLPG_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaCtlTblMenu.uchAddress = i;
        CliParaRead(&ParaCtlTblMenu);                           /* call ParaCtlTblMenuRead() */
        PrtPrintItem(NULL, &ParaCtlTblMenu);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

/* --- End of Source File --- */