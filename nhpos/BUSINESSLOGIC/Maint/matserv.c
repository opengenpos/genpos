/*     
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Maintenance Service Time Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATSERV.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Service Time Border
*
*           The provided function names are as follows: 
* 
*              MaintServiceTimeRead()   : read & display Service Time Border
*              MaintServiceTimeWrite()  : set & print Service Time Border
*              MaintServiceTimeReport() : print all Service Time Border
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-30-95: 03.01.00 : M.Ozawa   : initial                                   
* Aug-27-98: 03.03.00 : hrkato    : V3.3
* Aug-16-99: 03.05.00 : K.Iwata   : V3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
** Synopsis:    SHORT MaintServiceTimeRead( PARASERVICETIME *pData )
*               
*     Input:    *pData          : pointer to structure for PARASERVICETIME
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Service Time Border
*===========================================================================
*/

SHORT MaintServiceTimeRead( PARASERVICETIME *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.DiscTbl.uchAddress;

        /* check address */
        if (uchAddress > SERVTIME_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || SERVTIME_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.ServTime.uchMajorClass = pData->uchMajorClass;     /* copy major class */
    MaintWork.ServTime.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.ServTime));                          /* call ParaServiceTimeRead() */

    /* Set Leadthru Data */
	RflGetLead(MaintWork.ServTime.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.ServTime));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintServiceTimeWrite( PARASERVICETIME *pData )
*               
*     Input:    *pData             : pointer to structure for PARASERVICETIME
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints Service Time Border
*===========================================================================
*/

SHORT MaintServiceTimeWrite( PARASERVICETIME *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check range */
    if (pData->usBorder > MAINT_SERVTIME_MAX) { 
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.ServTime.usBorder = pData->usBorder;
    CliParaWrite(&(MaintWork.ServTime));                         /* call ParaServiceTimeWrite() */

    /* control header item */
    MaintHeaderCtl(AC_SERVICE_MAINT, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.ServTime.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.ServTime));
    
    /* set address for display next address */
    /* check address */
    MaintWork.ServTime.uchAddress++;

    if (MaintWork.ServTime.uchAddress > SERVTIME_ADR_MAX) {
        MaintWork.ServTime.uchAddress = 1;                       /* initialize address */
    }
    MaintServiceTimeRead(&(MaintWork.ServTime));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintServiceTimeReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Service Time Border REPORT.
*===========================================================================
*/

VOID MaintServiceTimeReport( VOID )
{
    UCHAR            i;
	PARASERVICETIME  ParaServTime = { 0 };

    /* control header item */
    MaintHeaderCtl(AC_SERVICE_MAINT, RPT_ACT_ADR);
    ParaServTime.uchMajorClass = CLASS_PARASERVICETIME;

    /* set journal bit & receipt bit */
    ParaServTime.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= SERVTIME_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaServTime.uchAddress = i;
        CliParaRead(&ParaServTime);                              /* call ParaServiceTimeRead() */
        PrtPrintItem(NULL, &ParaServTime);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}



/* --- End of Source --- */
