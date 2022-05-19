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
* Title       : MaintTransHALO Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATHALO.C
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Tansaction HALO.
*
*           The provided function names are as follows: 
* 
*              SHORT MaintTransHALORead()   : read & display HALO
*              SHORT MaintTransHALOWrite()  : set & print HALO
*              VOID  MaintTransHALOReport() : print all HALO REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-23-92: 00.00.01 : J.Ikeda   : initial                                   
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
#include <tchar.h>

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
** Synopsis:    SHORT MaintTransHALORead( PARATRANSHALO *pData )
*               
*     Input:    *pData  : pointer to structure for PARATRANSHALO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: read data
*               LDT_SEQERR_ADR: error
*
** Description: This function reads & displays TRANSACTION HALO.
*===========================================================================
*/
SHORT MaintTransHALORead( PARATRANSHALO *pData )
{
    UCHAR uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.TransHALO.uchAddress;
        if (uchAddress > MAX_HALOKEY_NO) { 
            uchAddress = HALO_TEND1_ADR;  /* initialize address */
        } 
    } else { /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (HALO_TEND1_ADR <= uchAddress && uchAddress <= MAX_HALOKEY_NO) {
        MaintWork.TransHALO.uchMajorClass = pData->uchMajorClass;
        MaintWork.TransHALO.uchAddress = uchAddress;
        CliParaRead(&(MaintWork.TransHALO));    /* call ParaTransHALORead() */
        DispWrite(&(MaintWork.TransHALO));
        return(SUCCESS);
    } else {
        return(LDT_KEYOVER_ADR);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTransHALOWrite( PARATRANSHALO *pData )
*               
*     Input:    *pData  : pointer to structure for PARATRANSHALO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: read data
*               LDT_SEQERR_ADR: error
*
** Description: This function sets & prints TRANSACTION HALO.
*===========================================================================
*/
SHORT MaintTransHALOWrite( PARATRANSHALO *pData )
{
    /* check status */
    if (!pData->uchStatus & MAINT_WITHOUT_DATA) { /* with data */
        /*  check range */
        if (pData->uchHALO <= MAINT_HALO_MAX) { 
            MaintWork.TransHALO.uchHALO = pData->uchHALO;
        } else {
            return(LDT_KEYOVER_ADR);   /* wrong data */
        }

        CliParaWrite(&(MaintWork.TransHALO));   /* call ParaTransHALOWrite() */

        /* control header item */
        MaintHeaderCtl(PG_TRAN_HALO, RPT_PRG_ADR);

        /* set journal bit & receipt bit */
        MaintWork.TransHALO.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        PrtPrintItem(NULL, &(MaintWork.TransHALO));

        /* set address for display next address */
        /* check address */
        MaintWork.TransHALO.uchAddress++;
        if (MaintWork.TransHALO.uchAddress > MAX_HALOKEY_NO) {
            MaintWork.TransHALO.uchAddress = HALO_TEND1_ADR; /* initialize address */
        }

        MaintTransHALORead(&(MaintWork.TransHALO));
        return(SUCCESS);
    } else { /* without data */
        return(LDT_SEQERR_ADR);    /* sequence error */
    }
}

/*
*===========================================================================
** Synopsis:    VOID MaintTransHALOReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all TRANSACTION HALO REPORT.
*===========================================================================
*/
VOID MaintTransHALOReport( VOID )
{
    UCHAR            i;
    PARATRANSHALO    ParaTransHalo;

    /* control header item */
    MaintHeaderCtl(PG_TRAN_HALO, RPT_PRG_ADR);

    ParaTransHalo.uchMajorClass = CLASS_PARATRANSHALO;
    ParaTransHalo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = HALO_TEND1_ADR; i <= MAX_HALOKEY_NO; i++) {
        /* check abort key */
        if (UieReadAbortKey() == UIE_DISABLE) { /* do not depress abort key */
            ParaTransHalo.uchAddress = i;
            CliParaRead(&ParaTransHalo);       /* call ParaTransHALORead() */
            PrtPrintItem(NULL, &ParaTransHalo);
        } else { /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
