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
* Title       : MaintSpeMnemo Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATSPEM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write SPECIAL MNEMONICS.
*
*           The provided function names are as follows: 
*                   
*              MaintSpeMnemoRead()   : read & display SPECIAL MNEMONICS
*              MaintSpeMnemoWrite()  : read & display SPECIAL MNEMONICS
*              MaintSpeMnemoReport() : report & print SPECIAL MNEMONICS report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 : J.Ikeda   : initial                                   
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
#include "csstbpar.h"
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintSpeMnemoRead( PARASPEMNEMO *pData )
*               
*     Input:    *pData         : ponter to structure for PARASPEMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays SPECIAL MNEMONICS.
*===========================================================================
*/

SHORT MaintSpeMnemoRead( PARASPEMNEMO *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.SpeMnemo.aszMnemonics, '\0', PARA_SPEMNEMO_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.SpeMnemo.uchAddress;

        /* check address */

        if (uchAddress > SPC_ADR_MAX) {                      
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || SPC_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.SpeMnemo.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.SpeMnemo.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.SpeMnemo));                         /* call ParaSpeMnemoRead() */ 
    DispWrite(&(MaintWork.SpeMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintSpeMnemoWrite( PARASPEMNEMO *pData )
*               
*     Input:    *pData         : pointer to structure for PARASPEMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays SPECIAL MNEMONICS.
*===========================================================================
*/

SHORT MaintSpeMnemoWrite( PARASPEMNEMO *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.SpeMnemo.aszMnemonics, pData->aszMnemonics, PARA_SPEMNEMO_LEN);
    //memcpy(MaintWork.SpeMnemo.aszMnemonics, pData->aszMnemonics, PARA_SPEMNEMO_LEN);
    CliParaWrite(&(MaintWork.SpeMnemo));                        /* call ParaSpeMnemoWrite() */

    /* control header item */

    MaintHeaderCtl(PG_SPEC_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.SpeMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.SpeMnemo));

    /* set address for display next address */

    MaintWork.SpeMnemo.uchAddress++;

    /* check address */

    if (MaintWork.SpeMnemo.uchAddress > SPC_ADR_MAX) {
        MaintWork.SpeMnemo.uchAddress = 1;                      /* initialize address */
    }
    MaintSpeMnemoRead(&(MaintWork.SpeMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSpeMnemoReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints SPECIAL MNEMONICS report.
*===========================================================================
*/

VOID MaintSpeMnemoReport( VOID )
{

    UCHAR     i;
    PARASPEMNEMO  ParaSpeMnemo;

    /* initialize */

/*    memset(ParaSpeMnemo.aszMnemonics, '\0', PARA_SPEMNEMO_LEN+1); */

    /* control header */

    MaintHeaderCtl(PG_SPEC_MNEMO, RPT_PRG_ADR);

    /* set major class */

    ParaSpeMnemo.uchMajorClass = CLASS_PARASPECIALMNEMO;

    /* set journal bit & receipt bit */

    ParaSpeMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= SPC_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaSpeMnemo.uchAddress = i;
        CliParaRead(&ParaSpeMnemo);                             /* call ParaSpeMnemoRead() */
        PrtPrintItem(NULL, &ParaSpeMnemo);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
