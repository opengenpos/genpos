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
* Title       : MaintPrtModi Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPRTM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write PRINT MODIFIER MNEMONICS.
*
*           The provided function names are as follows: 
* 
*              MaintPrtModiRead()   : read & display PRINT MODIFIER MNEMONICS
*              MaintPrtModiWrite()  : read & display PRINT MODIFIER MNEMONICS
*              MaintPrtModiReport() : report & print PRINT MODIFIER MNEMONICS report 
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
** Synopsis:    SHORT MaintPrtModiRead( PARAPRTMODI *pData )
*               
*     Input:    *pData         : ponter to structure for PARAPRTMODI
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays PRINT MODIFIER MNEMONICS.
*===========================================================================
*/

SHORT MaintPrtModiRead( PARAPRTMODI *pData )
{
    UCHAR uchAddress;

    /* check status */
    if(pData->uchStatus & MAINT_WITHOUT_DATA) {
		/* without data so increment to next address and do rollover to beginning if at max */
        uchAddress = ++MaintWork.PrtModi.uchAddress;
        if (uchAddress > MAX_PRTMODI_NO) {
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */
    if (uchAddress < 1 || MAX_PRTMODI_NO < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.PrtModi.uchMajorClass = pData->uchMajorClass;     /* copy major class */
    MaintWork.PrtModi.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.PrtModi));                          /* call ParaPrtModiRead() */ 

    DispWrite(&(MaintWork.PrtModi));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPrtModiWrite( PARAPRTMODI *pData )
*               
*     Input:    *pData         : pointer to structure for PARAPRTMODI
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays PRINT MODIFIER MNEMONICS.
*===========================================================================
*/

SHORT MaintPrtModiWrite( PARAPRTMODI *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.PrtModi.aszMnemonics, pData->aszMnemonics, PARA_PRTMODI_LEN);
    CliParaWrite(&(MaintWork.PrtModi));                         /* call ParaPrtModiWrite() */

    /* control header item */
    MaintHeaderCtl(PG_PRT_MNEMO, RPT_PRG_ADR);

    MaintWork.PrtModi.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.PrtModi));

    /* set address for display next address */
    MaintWork.PrtModi.uchAddress++;
    if (MaintWork.PrtModi.uchAddress > MAX_PRTMODI_NO) {
        MaintWork.PrtModi.uchAddress = 1;                       /* initialize address */
    }

    MaintPrtModiRead(&(MaintWork.PrtModi));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPrtModiReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints PRINT MODIFIER MNEMONICS report.
*===========================================================================
*/

VOID MaintPrtModiReport( VOID )
{
    USHORT       i;
	PARAPRTMODI  ParaPrtModi = { 0 };

    /* control header */
    MaintHeaderCtl(PG_PRT_MNEMO, RPT_PRG_ADR);

    ParaPrtModi.uchMajorClass = CLASS_PARAPRTMODI;
    ParaPrtModi.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* get data at every address and print it */
    for (i = 1; i <= MAX_PRTMODI_NO; i++) {
        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaPrtModi.uchAddress = i;
        CliParaRead(&ParaPrtModi);                              /* call ParaPrtModiRead() */
        PrtPrintItem(NULL, &ParaPrtModi);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
