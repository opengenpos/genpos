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
* Title       : MaintDisc Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATDISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report DISCOUNT RATE & BONUS RATE.
*
*           The provided function names are as follows: 
* 
*              MaintDiscRead()   : read & display DISCOUNT/BONUS RATE
*              MaintDiscWrite()  : set & print DISCOUNT/BONUS RATE
*              MaintDiscReport() : print all DISCOUNT/BONUS RATE REPORT   
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
#include <rfl.h> 
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintDiscRead( PARADISCTBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARADISCTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays DISCOUNT/BONUS RATE.
*===========================================================================
*/

SHORT MaintDiscRead( PARADISCTBL *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.DiscTbl.uchAddress;
        /* check address */
        if (uchAddress > RATE_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || RATE_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.DiscTbl.uchMajorClass = pData->uchMajorClass;     /* copy major class */
    MaintWork.DiscTbl.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.DiscTbl));                          /* call ParaDiscRead() */

	RflGetLead(MaintWork.DiscTbl.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.DiscTbl));

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintDiscWrite( PARADISCTBL *pData )
*               
*     Input:    *pData             : pointer to structure for PARADISCTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints DISCOUNT/BONUS RATE.
*===========================================================================
*/

SHORT MaintDiscWrite( PARADISCTBL *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check range */

    if (pData->uchDiscRate > MAINT_DISC_MAX) { 
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.DiscTbl.uchDiscRate = pData->uchDiscRate;
    CliParaWrite(&(MaintWork.DiscTbl));                         /* call ParaDiscWrite() */

    /* control header item */
        
    MaintHeaderCtl(AC_DISC_BONUS, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.DiscTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.DiscTbl));
    
    /* set address for display next address */

    /* check address */

    MaintWork.DiscTbl.uchAddress++;

    if (MaintWork.DiscTbl.uchAddress > RATE_ADR_MAX) {
        MaintWork.DiscTbl.uchAddress = 1;                       /* initialize address */
    }
    MaintDiscRead(&(MaintWork.DiscTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintDiscReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all DISCOUNT/BONUS RATE REPORT.
*===========================================================================
*/

VOID MaintDiscReport( VOID )
{


    UCHAR            i;
    PARADISCTBL      ParaDiscTbl;


    /* control header item */
        
    MaintHeaderCtl(AC_DISC_BONUS, RPT_ACT_ADR);
    ParaDiscTbl.uchMajorClass = CLASS_PARADISCTBL;

    /* set journal bit & receipt bit */

    ParaDiscTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= RATE_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaDiscTbl.uchAddress = i;
        CliParaRead(&ParaDiscTbl);                              /* call ParaDiscRead() */
        PrtPrintItem(NULL, &ParaDiscTbl);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

    
    
    
    
    
