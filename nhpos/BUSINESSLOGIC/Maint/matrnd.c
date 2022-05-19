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
* Title       : MaintRound Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATRND.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report ROUNDING DATA.
*
*           The provided function names are as follows: 
* 
*              MaintRoundRead()   : read & display ROUNDING DATA
*              MaintRoundWrite()  : set & print ROUNDING DATA
*              MaintRoundReport() : print all ROUNDING DATA REPORT   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-29-92: 00.00.01 : K.You     : initial                                   
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
/* #include <pif.h> */
/* #include <log.h> */
/* #include <appllog.h> */
#include "paraequ.h"
#include "para.h"
#include "rfl.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"

#include "maintram.h" 
                                              
/*
*===========================================================================
** Synopsis:    SHORT MaintRoundRead( PARAROUNDTBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARAROUNDTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays ROUNDING DATA.
*===========================================================================
*/

SHORT MaintRoundRead( PARAROUNDTBL *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.RoundTbl.uchAddress;

        /* check address */
        if (uchAddress > RND_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || RND_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.RoundTbl.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.RoundTbl.uchAddress = uchAddress;

    /* Set Minor Class */
    if (uchAddress & 0x01) {                                    /* Delimitor, Modules Case */
        MaintWork.RoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_DELI;
    } else {                                                    /* Position Case */
        MaintWork.RoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_POSI;
    }
    CliParaRead(&(MaintWork.RoundTbl));                         /* call ParaRoundRead() */

	RflGetLead(MaintWork.RoundTbl.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.RoundTbl));

    if (uchAddress & 0x01) {                                    /* Delimitor, Modules Case */
        return(MAINT_RND_DELI);
    } else {                                                    /* Position Case */
        return(MAINT_RND_POSI);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintRoundWrite( PARAROUNDTBL *pData )
*               
*     Input:    *pData             : pointer to structure for PARAROUNDTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints ROUNDING DATA.
*===========================================================================
*/

SHORT MaintRoundWrite( PARAROUNDTBL *pData )
{
    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /* check address */

    if (MaintWork.RoundTbl.uchMinorClass != pData->uchMinorClass) {     /* in case of even address */
        return(LDT_SEQERR_ADR);                                         
    }

    /* check minor class */

    switch(pData->uchMinorClass) {
    case CLASS_PARAROUNDTBL_DELI:

        /* delimit & modules has already checked by UI */

        MaintWork.RoundTbl.uchRoundDelimit = pData->uchRoundDelimit;
        MaintWork.RoundTbl.uchRoundModules = pData->uchRoundModules;
        break;

    case CLASS_PARAROUNDTBL_POSI:

        /* check round position range */

        if (pData->chRoundPosition < MAINT_POSITION_MIN || pData->chRoundPosition > MAINT_POSITION_MAX) {
            return(LDT_KEYOVER_ADR);                            
        }
        MaintWork.RoundTbl.chRoundPosition = pData->chRoundPosition;
        break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);
    }

    CliParaWrite(&(MaintWork.RoundTbl));                      

    /* control header item */
        
    MaintHeaderCtl(AC_ROUND, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.RoundTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.RoundTbl));
    
    /* set address for display next address */

    /* check address */

    MaintWork.RoundTbl.uchAddress++;

    if (MaintWork.RoundTbl.uchAddress > RND_ADR_MAX) {
        MaintWork.RoundTbl.uchAddress = 1;                      /* initialize address */
    }
    MaintRoundRead(&(MaintWork.RoundTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintRoundReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all ROUNDING DATA REPORT.
*===========================================================================
*/

VOID MaintRoundReport( VOID )
{
    UCHAR           i;
	PARAROUNDTBL    ParaRoundTbl = { 0 };


    /* control header item */
        
    MaintHeaderCtl(AC_ROUND, RPT_ACT_ADR);
    ParaRoundTbl.uchMajorClass = CLASS_PARAROUNDTBL;

    /* set journal bit & receipt bit */

    ParaRoundTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= RND_ADR_MAX; i += 2) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaRoundTbl.uchAddress = i;
        CliParaRead(&ParaRoundTbl);                             /* call ParaRoundRead() */
        ParaRoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_DELI;
        PrtPrintItem(NULL, &ParaRoundTbl);
        ParaRoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_POSI;
        ParaRoundTbl.uchAddress++;
        PrtPrintItem(NULL, &ParaRoundTbl);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

