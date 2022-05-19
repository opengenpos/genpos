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
* Title       : MaintReportName Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATREPO.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write REPORT NAME.
*
*           The provided function names are as follows: 
* 
*              MaintReportNameRead()   : read & display REPORT NAME
*              MaintReportNameWrite()  : read & display REPORT NAME
*              MaintReportNameReport() : report & print REPORT NAME REPORT  
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
** Synopsis:    SHORT MaintReportNameRead( PARAREPORTNAME *pData )
*               
*     Input:    *pData         : ponter to structure for PARAREPORTNAME
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays REPORT NAME.
*===========================================================================
*/

SHORT MaintReportNameRead( PARAREPORTNAME *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.ReportName.aszMnemonics, '\0', PARA_REPORTNAME_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.ReportName.uchAddress;

        /* check address */

        if (uchAddress > RPT_ADR_MAX) {                   
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || RPT_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.ReportName.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.ReportName.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.ReportName));                       /* call ParaReportNameRead() */ 
    DispWrite(&(MaintWork.ReportName));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintReportNameWrite( PARAREPORTNAME *pData )
*               
*     Input:    *pData         : pointer to structure for PARAREPORTNAME
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function sets & prints REPORT NAME.
*===========================================================================
*/

SHORT MaintReportNameWrite( PARAREPORTNAME *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.ReportName.aszMnemonics, pData->aszMnemonics, PARA_REPORTNAME_LEN);
    //memcpy(MaintWork.ReportName.aszMnemonics, pData->aszMnemonics, PARA_REPORTNAME_LEN);
    CliParaWrite(&(MaintWork.ReportName));                      /* call ParaReportNameWrite() */

    /* control header item */

    MaintHeaderCtl(PG_RPT_NAME, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.ReportName.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.ReportName));

    /* set address for Display next address */

    MaintWork.ReportName.uchAddress++;

    /* check address */

    if (MaintWork.ReportName.uchAddress > RPT_ADR_MAX) {
        MaintWork.ReportName.uchAddress = 1;                    /* initialize address */
    }
    MaintReportNameRead(&(MaintWork.ReportName));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintReportNameReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints REPORT NAME report.
*===========================================================================
*/

VOID MaintReportNameReport( VOID )
{

    UCHAR           i;
    PARAREPORTNAME  ParaReportName;

    /* initialize */

/*    memset(ParaReportName.aszMnemonics, '\0', PARA_REPORTNAME_LEN+1); */

    /* control header */

    MaintHeaderCtl(PG_RPT_NAME, RPT_PRG_ADR);

    /* set major class */

    ParaReportName.uchMajorClass = CLASS_PARAREPORTNAME;

    /* set journal bit & receipt bit */

    ParaReportName.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= RPT_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaReportName.uchAddress = i;
        CliParaRead(&ParaReportName);                           /* call ParaReportNameRead() */
        PrtPrintItem(NULL, &ParaReportName);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
