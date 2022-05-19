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
* Title       : MaintLeadThru Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATLEAD.C
* --------------------------------------------------------------------------
* Abstract: This function read & write LEAD THROUGH.
*
*           The provided function names are as follows: 
* 
*              MaintLeadThruRead()   : read & display LEAD THROUGH
*              MaintLeadThruWrite()  : set & print LEAD THROUGH
*              MaintLeadThruReport() : print all LEAD THROUGH report
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 : J.Ikeda   : initial                                   
*          :          :           :                                    
** GenPOS **
* Sep-15-17 : 02.02.02 : R.Chambers  : replace LDT_ADR_MAX with MAX_LEAD_NO to eliminate a define.
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
#include <tchar.h>
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
** Synopsis:    SHORT MaintLeadThruRead( PARALEADTHRU *pData )
*               
*     Input:    *pData         : ponter to structure for PARALEADTHRU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays LEADTHRU.
*===========================================================================
*/
SHORT MaintLeadThruRead( PARALEADTHRU *pData )
{
    UCHAR uchAddress;

    /* check status */
    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.LeadThru.uchAddress;

        /* check address */
        if (uchAddress > MAX_LEAD_NO) {                         
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */
    if (uchAddress < 1 || MAX_LEAD_NO < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.LeadThru.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.LeadThru.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.LeadThru));                         /* call ParaLeadThruRead() */ 
    DispWrite(&(MaintWork.LeadThru));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintLeadThruWrite( PARALEADTHRU *pData )
*               
*     Input:    *pData         : pointer to structure for PARALEADTHRU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function sets & prints LEAD THROUGH.
*===========================================================================
*/
SHORT MaintLeadThruWrite( PARALEADTHRU *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.LeadThru.aszMnemonics, pData->aszMnemonics, PARA_LEADTHRU_LEN);
    CliParaWrite(&(MaintWork.LeadThru));                        /* call ParaLeadThruWrite() */

    /* control header item */
    MaintHeaderCtl(PG_LEAD_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */
    MaintWork.LeadThru.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.LeadThru));

    /* set address for Display next address */
    MaintWork.LeadThru.uchAddress++;

    /* check address */
    if (MaintWork.LeadThru.uchAddress > MAX_LEAD_NO) {
        MaintWork.LeadThru.uchAddress = 1;                      /* initialize address */
    }
    MaintLeadThruRead(&(MaintWork.LeadThru));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintLeadThruReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all LEAD THROUGH report.
*===========================================================================
*/
VOID MaintLeadThruReport( VOID )
{
    UCHAR         i;

    /* control header */
    MaintHeaderCtl(PG_LEAD_MNEMO, RPT_PRG_ADR);

    /* set data at every address */
    for (i = 1; i <= MAX_LEAD_NO; i++) {
		PARALEADTHRU  ParaLeadThru = {0};

		ParaLeadThru.uchMajorClass = CLASS_PARALEADTHRU;
		ParaLeadThru.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaLeadThru.uchAddress = i;
        CliParaRead(&ParaLeadThru);                             /* call ParaLeadThruRead() */
        PrtPrintItem(NULL, &ParaLeadThru);
    }

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
