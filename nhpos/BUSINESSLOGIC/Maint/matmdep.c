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
* Title       : MaintMajorDEPT Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATMDEPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write MAJOR DEPARTMENT MNEMONICS.
*
*           The provided function names are as follows: 
* 
*              MaintMajorDEPTRead()   : read & display MAJOR DEPARTMENT MNEMONICS
*              MaintMajorDEPTWrite()  : read & display MAJOR DEPARTMENT MNEMONICS
*              MaintMajorDEPTReport() : report & print MAJOR DEPARTMENT MNEMONICS report  
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
** Synopsis:    SHORT MaintMajorDEPTRead( PARAMAJORDEPT *pData )
*               
*     Input:    *pData         : ponter to structure for PARAMAJORDEPT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays MAJOR DEPARTMENT MNEMONICS.
*===========================================================================
*/

SHORT MaintMajorDEPTRead( PARAMAJORDEPT *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.MajorDEPT.aszMnemonics, '\0', PARA_MAJORDEPT_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.MajorDEPT.uchAddress;

        /* check address */

        if (uchAddress > MDPT_ADR_MAX) {                      
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || MDPT_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.MajorDEPT.uchMajorClass = pData->uchMajorClass;   /* copy major class */
    MaintWork.MajorDEPT.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.MajorDEPT));                        /* call ParaMajorDEPTRead() */ 
    DispWrite(&(MaintWork.MajorDEPT));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintMajorDEPTWrite( PARAMAJORDEPT *pData )
*               
*     Input:    *pData         : pointer to structure for PARAMAJORDEPT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays MAJOR DEPARTMENT MNEMONICS.
*===========================================================================
*/

SHORT MaintMajorDEPTWrite( PARAMAJORDEPT *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.MajorDEPT.aszMnemonics, pData->aszMnemonics, PARA_MAJORDEPT_LEN);
    //memcpy(MaintWork.MajorDEPT.aszMnemonics, pData->aszMnemonics, PARA_MAJORDEPT_LEN);
    CliParaWrite(&(MaintWork.MajorDEPT));                       /* call ParaMajorDEPTWrite() */

    /* control header item */

    MaintHeaderCtl(PG_MAJDEPT_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.MajorDEPT.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.MajorDEPT));

    /* set address for Display next address */

    MaintWork.MajorDEPT.uchAddress++;

    /* check address */

    if (MaintWork.MajorDEPT.uchAddress > MDPT_ADR_MAX) {
        MaintWork.MajorDEPT.uchAddress = 1;                     /* initialize address */
    }
    MaintMajorDEPTRead(&(MaintWork.MajorDEPT));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintMajorDEPTReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints MAJOR DEPARTMENT MNEMONICS report.
*===========================================================================
*/

VOID MaintMajorDEPTReport( VOID )
{

    UCHAR          i;
    PARAMAJORDEPT  ParaMajorDEPT;

    /* initialize */

/*    memset(ParaMajorDEPT.aszMnemonics, '\0', PARA_MAJORDEPT_LEN+1); */

    /* control header */

    MaintHeaderCtl(PG_MAJDEPT_MNEMO, RPT_PRG_ADR);

    /* set major class */

    ParaMajorDEPT.uchMajorClass = CLASS_PARAMAJORDEPT;

    /* set journal bit & receipt bit */

    ParaMajorDEPT.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= MDPT_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaMajorDEPT.uchAddress = i;
        CliParaRead(&ParaMajorDEPT);                            /* call ParaMajorDEPTRead() */
        PrtPrintItem(NULL, &ParaMajorDEPT);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
