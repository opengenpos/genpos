/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintTend Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US Hospitality Application Program
* Program Name: MATTEND.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Tender Parameter
*
*           The provided function names are as follows: 
* 
*              SHORT MaintTendRead()   : read & display Tender Parameter
*              SHORT MaintTendWrite()  : set & print Tender Parameter
*              VOID  MaintTendReport() : print all Tender Parameter REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Aug-04-98: 03.03.00 : hrkato    : V3.3 (Fast Finalize Improvement)
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
** Synopsis:    SHORT   MaintTendRead(PARATEND *pData)
*               
*     Input:    *pData  : pointer to structure for PARATEND
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: read data
*               LDT_SEQERR_ADR: error
*
** Description: This function reads & displays Tender Parameter.
*===========================================================================
*/
SHORT   MaintTendRead(PARATEND *pData)
{
    UCHAR   uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */
        uchAddress = ++MaintWork.Tend.uchAddress;
        if (uchAddress > TEND_ADR_MAX) { 
            uchAddress = TEND_NO1_ADR;
        }

    } else {                                                /* with data */
        uchAddress = pData->uchAddress;
    }
    /* check address */
    if (TEND_NO1_ADR <= uchAddress && uchAddress <= TEND_ADR_MAX) {
        MaintWork.Tend.uchMajorClass = pData->uchMajorClass;
        MaintWork.Tend.uchAddress = uchAddress;
        CliParaRead(&MaintWork.Tend);
        DispWrite(&MaintWork.Tend);
        return(SUCCESS);

    } else {
        return(LDT_KEYOVER_ADR);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   MaintTendWrite(PARATEND *pData)
*               
*     Input:    *pData  : pointer to structure for PARATEND
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: read data
*               LDT_SEQERR_ADR: error
*
** Description: This function sets & prints Tender Key Parameter.
*===========================================================================
*/
SHORT   MaintTendWrite(PARATEND *pData)
{
    /* check status */
    if (!pData->uchStatus & MAINT_WITHOUT_DATA) { /* with data */

        /*  check range */
		/* temporary hardcoded 11, change to MAINT_TEND_MAX when implementing full
		tender database change JHHJ 3-16-04*/
        if (pData->uchTend <= STD_TENDER_MAX) {
            MaintWork.Tend.uchTend = pData->uchTend;

        } else {
            return(LDT_KEYOVER_ADR);   /* wrong data */
        }
        CliParaWrite(&MaintWork.Tend);
        
        /* control header item */
        MaintHeaderCtl(PG_TEND_PARA, RPT_PRG_ADR);

        /* set journal bit & receipt bit */
        MaintWork.Tend.usPrintControl = PRT_JOURNAL | PRT_RECEIPT;
        PrtPrintItem(NULL, &MaintWork.Tend);

        /* set address for display next address */
        /* check address */
        MaintWork.Tend.uchAddress++;
        if (MaintWork.Tend.uchAddress > TEND_ADR_MAX) {
            MaintWork.Tend.uchAddress = TEND_NO1_ADR;
        }
        MaintTendRead(&MaintWork.Tend);
        return(SUCCESS);

    } else { /* without data */
        return(LDT_SEQERR_ADR);    /* sequence error */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    MaintTendReport(VOID)
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Tender Parameter REPORT.
*===========================================================================
*/
VOID    MaintTendReport(VOID)
{
    UCHAR           i;
    PARATEND        ParaTend;

    /* control header item */
    MaintHeaderCtl(PG_TEND_PARA, RPT_PRG_ADR);
    ParaTend.uchMajorClass = CLASS_PARATEND;

    /* set journal bit & receipt bit */
    ParaTend.usPrintControl = PRT_JOURNAL | PRT_RECEIPT;

    /* set data at every address */
    for (i = TEND_NO1_ADR; i <= TEND_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_DISABLE) { /* do not depress abort key */
            ParaTend.uchAddress = i;
            CliParaRead(&ParaTend);
            PrtPrintItem(NULL, &ParaTend);

        } else {                                /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}

/* --- End of Source --- */
