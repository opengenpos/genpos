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
* Title       : MaintChar24 Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCH24.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write 24 CHARACTERS MNEMONICS.
*
*           The provided function names are as follows: 
* 
*              MaintChar24Read()   : read & display 24 CHARACTERS MNEMONICS
*              MaintChar24Write()  : read & display 24 CHARACTERS MNEMONICS
*              MaintChar24Report() : report & print 24 CHARACTERS MNEMONICS report  
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
** Synopsis:    SHORT MaintChar24Read( PARACHAR24 *pData )
*               
*     Input:    *pData         : ponter to structure for PARACHAR24
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays 24 CHARACTERS MNEMONICS.
*===========================================================================
*/

SHORT MaintChar24Read( PARACHAR24 *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.Char24.aszMnemonics, '\0', PARA_CHAR24_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.Char24.uchAddress;

        /* check address */

        if (uchAddress > CH24_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || CH24_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.Char24.uchMajorClass = pData->uchMajorClass;      /* copy major class */
    MaintWork.Char24.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.Char24));                           /* call ParaChar24Read() */ 
    DispWrite(&(MaintWork.Char24));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintChar24Write( PARACHAR24 *pData )
*               
*     Input:    *pData         : pointer to structure for PARACHAR24
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays 24 CHARACTERS MNEMONICS.
*===========================================================================
*/

SHORT MaintChar24Write( PARACHAR24 *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.Char24.aszMnemonics, pData->aszMnemonics, PARA_CHAR24_LEN);
    //memcpy(MaintWork.Char24.aszMnemonics, pData->aszMnemonics, PARA_CHAR24_LEN);
    CliParaWrite(&(MaintWork.Char24));                          /* call ParaChar24Write() */

    /* control header item */

    MaintHeaderCtl(PG_CHA24_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.Char24.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.Char24));

    /* set address for Display next address */

    MaintWork.Char24.uchAddress++;

    /* check address */

    if (MaintWork.Char24.uchAddress > CH24_ADR_MAX) {
        MaintWork.Char24.uchAddress = 1;                        /* initialize address */
    }
    MaintChar24Read(&(MaintWork.Char24));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintChar24Report( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints 24 CHARACTERS MNEMONICS report.
*===========================================================================
*/

VOID MaintChar24Report( VOID )
{

    UCHAR       i;
    PARACHAR24  ParaChar24;

    /* initialize */

/*    memset(ParaChar24.aszMnemonics, '\0', PARA_CHAR24_LEN+1); */

    /* control header */

    MaintHeaderCtl(PG_CHA24_MNEMO, RPT_PRG_ADR);

    /* set major class */

    ParaChar24.uchMajorClass = CLASS_PARACHAR24;

    /* set journal bit & receipt bit */

    ParaChar24.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= CH24_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaChar24.uchAddress = i;
        CliParaRead(&ParaChar24);                               /* call ParaChar24Read() */
        PrtPrintItem(NULL, &ParaChar24);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
