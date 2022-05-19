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
* Title       : MaintSlipFeddCtl Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATSLIP.C
* --------------------------------------------------------------------------
* Abstract: This function read & write & report SLIP FEED CONTROL DATA.
*                                  
*           The provided function names are as follows: 
* 
*              MaintSlipFeedCtlRead()   : read & display SLIP FEED CONTROL DATA
*              MaintSlipFeedCtlWrite()  : set & print SLIP FEED CONTROL DATA
*              MaintSlipFeedCtlReport() : print all SLIP FEED CONTROL DATA REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-10-92: 00.00.01 : J.Ikeda   : initial                                   
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
** Synopsis:    SHORT MaintSlipFeedCtlRead( PARASLIPFEEDCTL *pData )
*               
*     Input:    *pData          : pointer to structure for PARASLIPFEEDCTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays SLIP FEED CONTROL DATA.
*===========================================================================
*/
SHORT MaintSlipFeedCtlRead( PARASLIPFEEDCTL *pData )
{
    UCHAR uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.SlipFeedCtl.uchAddress;
        /* check address */
        if (uchAddress > SLIP_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || SLIP_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.SlipFeedCtl.uchMajorClass = pData->uchMajorClass; /* copy major class */
    MaintWork.SlipFeedCtl.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.SlipFeedCtl));                      /* call ParaSlipFeedCtlRead() */
    DispWrite(&(MaintWork.SlipFeedCtl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintSlipFeedCtlWrite( PARASLIPFEEDCTL *pData )
*               
*     Input:    *pData             : pointer to structure for PARASLIPFEEDCTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints SLIP FEED CONTROL.
*===========================================================================
*/
SHORT MaintSlipFeedCtlWrite( PARASLIPFEEDCTL *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check address */
    switch (MaintWork.SlipFeedCtl.uchAddress) {
    case SLIP_MAXLINE_ADR:
        /* check range */
        if (pData->uchFeedCtl > MAINT_MAXFEED_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */ 
        }
        break;

    case SLIP_NO1ST_ADR:
        /* check range */
        if (pData->uchFeedCtl > MAINT_MAXLINE_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    case SLIP_MAXVAL_ADR:
        /* check range */
        if (pData->uchFeedCtl < MAINT_VALIDATION_MIN || MAINT_VALIDATION_MAX < pData->uchFeedCtl) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    case SLIP_EJLEN_ADR:
        /* check range */
        if (pData->uchFeedCtl < MAINT_EJLEN_MIN || MAINT_EJLEN_MAX < pData->uchFeedCtl) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    case SLIP_REVJLEN_ADR:
        /* check range */
        if (MAINT_REVEJLEN_MAX < pData->uchFeedCtl) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    default:                        /* SLIP_NEARFULL_ADR: */
        /* check range */
        if (MAINT_NEARFULL_MAX < pData->uchFeedCtl) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    }
    MaintWork.SlipFeedCtl.uchFeedCtl = pData->uchFeedCtl;
    CliParaWrite(&(MaintWork.SlipFeedCtl));                     /* call ParaSlipFeedCtlWrite() */

    /* control header item */
    MaintHeaderCtl(PG_SLIP_CTL, RPT_PRG_ADR);

    /* set journal bit & receipt bit */
    MaintWork.SlipFeedCtl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.SlipFeedCtl));
    
    /* set address for display next address */
    /* check address */
    MaintWork.SlipFeedCtl.uchAddress++;

    if (MaintWork.SlipFeedCtl.uchAddress > SLIP_ADR_MAX) {
        MaintWork.SlipFeedCtl.uchAddress = 1;                   /* initialize address */
    }
    MaintSlipFeedCtlRead(&(MaintWork.SlipFeedCtl));
    return(SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID MaintSlipFeedCtlReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all SLIP FEED CONTROL REPORT.
*===========================================================================
*/
VOID MaintSlipFeedCtlReport( VOID )
{
	PARASLIPFEEDCTL      ParaSlipFeedCtl = {0};
    UCHAR                i;

    /* control header item */
    MaintHeaderCtl(PG_SLIP_CTL, RPT_PRG_ADR);
    ParaSlipFeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;

    /* set journal bit & receipt bit */
    ParaSlipFeedCtl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= SLIP_ADR_MAX; i++) {
        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaSlipFeedCtl.uchAddress = i;
        CliParaRead(&ParaSlipFeedCtl);                          /* call ParaSlipFeedCtlRead() */
        PrtPrintItem(NULL, &ParaSlipFeedCtl);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

/****** End of Source ******/    

