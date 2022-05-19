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
* Title       : MaintTransMnemo Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATTRAN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write TRANSACTION MNEMONICS.
*
*           The provided function names are as follows: 
* 
*              MaintTransMnemoRead()   : read & display TRANSACTION MNEMONICS
*              MaintTransMnemoWrite()  : read & display TRANSACTION MNEMONICS
*              MaintTransMnemoReport() : report & print TRANSACTION MNEMONICS report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-25-92: 00.00.01 : J.Ikeda   : initial                                   
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
** Synopsis:    SHORT MaintTransMnemoRead( PARATRANSMNEMO *pData )
*               
*     Input:    *pData          : ponter to structure for PARATRANSMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data 
*
** Description: This function reads & displays TRANSACTION MNEMONICS.
*===========================================================================
*/

SHORT MaintTransMnemoRead( PARATRANSMNEMO *pData )
{

    USHORT uchAddress;

    /* initialize */

/*    memset(MaintWork.TransMnemo.aszMnemonics, '\0', PARA_TRANSMNEMO_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.TransMnemo.uchAddress;

        /* check address */

        if (uchAddress > TRN_ADR_MAX) {                      
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || TRN_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.TransMnemo.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.TransMnemo.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.TransMnemo));                       /* call ParaTransMnemoRead() */ 
    DispWrite(&(MaintWork.TransMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTransMnemoWrite( PARATRANSMNEMO *pData )
*               
*     Input:    *pData         : pointer to structure for PARATRANSMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays TRANSACTION MNEMONICS.
*===========================================================================
*/

SHORT MaintTransMnemoWrite( PARATRANSMNEMO *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.TransMnemo.aszMnemonics, pData->aszMnemonics, PARA_TRANSMNEMO_LEN);
    //memcpy(MaintWork.TransMnemo.aszMnemonics, pData->aszMnemonics, PARA_TRANSMNEMO_LEN);
    CliParaWrite(&(MaintWork.TransMnemo));                      /* call ParaTransMnemoWrite() */

    /* control header item */

    MaintHeaderCtl(PG_TRAN_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.TransMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.TransMnemo));

    /* set address for Display next address */

    MaintWork.TransMnemo.uchAddress++;

    /* check address */

    if (MaintWork.TransMnemo.uchAddress > TRN_ADR_MAX) {
        MaintWork.TransMnemo.uchAddress = 1;                    /* initialize address */
    }
    MaintTransMnemoRead(&(MaintWork.TransMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTransMnemoReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints TRANSACTION MNEMONICS report.
*===========================================================================
*/

VOID MaintTransMnemoReport( VOID )
{

    USHORT     i;
    PARATRANSMNEMO  ParaTransMnemo;

    /* initialize */

/*    memset(ParaTransMnemo.aszMnemonics, '\0', PARA_TRANSMNEMO_LEN+1); */

    /* control header */

    MaintHeaderCtl(PG_TRAN_MNEMO, RPT_PRG_ADR);

    /* set major class */

    ParaTransMnemo.uchMajorClass = CLASS_PARATRANSMNEMO;

    /* set journal bit & receipt bit */

    ParaTransMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= TRN_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaTransMnemo.uchAddress = i;
        CliParaRead(&ParaTransMnemo);                           /* call ParaTransMnemoRead() */
        PrtPrintItem(NULL, &ParaTransMnemo);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
