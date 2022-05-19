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
* Title       : MaintAdjMnemo Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATADJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write ADJECTIVE MNEMONICS.
*
*           The provided function names are as follows: 
* 
*              MaintAdjMnemoRead()   : read & display ADJECTIVE MNEMONICS
*              MaintAdjMnemoWrite()  : read & display ADJECTIVE MNEMONICS
*              MaintAdjMnemoReport() : report & print ADJECTIVE MNEMONICS report  
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

#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintAdjMnemoRead( PARAADJMNEMO *pData )
*               
*     Input:    *pData         : ponter to structure for PARAADJMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays ADJECTIVE MNEMONICS.
*===========================================================================
*/

SHORT MaintAdjMnemoRead( PARAADJMNEMO *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.AdjMnemo.aszMnemonics, '\0', PARA_ADJMNEMO_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.AdjMnemo.uchAddress;

        /* check address */

        if (uchAddress > ADJ_ADR_MAX) {                     
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || ADJ_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.AdjMnemo.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.AdjMnemo.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.AdjMnemo));                         /* call ParaAdjMnemoRead() */ 
    DispWrite(&(MaintWork.AdjMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintAdjMnemoWrite( PARAADJMNEMO *pData )
*               
*     Input:    *pData         : pointer to structure for PARAADJMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays ADJECTIVE MNEMONICS.
*===========================================================================
*/

SHORT MaintAdjMnemoWrite( PARAADJMNEMO *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.AdjMnemo.aszMnemonics, pData->aszMnemonics, PARA_ADJMNEMO_LEN);
    //memcpy(MaintWork.AdjMnemo.aszMnemonics, pData->aszMnemonics, PARA_ADJMNEMO_LEN);
    CliParaWrite(&(MaintWork.AdjMnemo));                        /* call ParaAdjMnemoWrite() */

    /* control header item */

    MaintHeaderCtl(PG_ADJ_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.AdjMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.AdjMnemo));

    /* set address for Display next address */

    MaintWork.AdjMnemo.uchAddress++;

    /* check address */

    if (MaintWork.AdjMnemo.uchAddress > ADJ_ADR_MAX) {
        MaintWork.AdjMnemo.uchAddress = 1;                      /* initialize address */
    }
    MaintAdjMnemoRead(&(MaintWork.AdjMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintAdjMnemoReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints ADJECTIVE MNEMONICS report.
*===========================================================================
*/

VOID MaintAdjMnemoReport( VOID )
{

    UCHAR     i;
    PARAADJMNEMO  ParaAdjMnemo;

    /* initialize */

/*    memset(ParaAdjMnemo.aszMnemonics, '\0', PARA_ADJMNEMO_LEN+1); */

    /* control header */

    MaintHeaderCtl(PG_ADJ_MNEMO, RPT_PRG_ADR);

    /* set major class */

    ParaAdjMnemo.uchMajorClass = CLASS_PARAADJMNEMO;

    /* set journal bit & receipt bit */

    ParaAdjMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= ADJ_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaAdjMnemo.uchAddress = i;
        CliParaRead(&ParaAdjMnemo);                             /* call ParaAdjMnemoRead() */
        PrtPrintItem(NULL, &ParaAdjMnemo);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
