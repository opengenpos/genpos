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
* Title       : MaintCashABAssign Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCASH.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report CASHIER NO. of A/B KEY.
*
*           The provided function names are as follows: 
* 
*              MaintCashABAssignRead()   : read & display CASHIER NO. of A/B KEY
*              MaintCashABAssignWrite()  : set & print CASHIER NO. of A/B KEY
*              MaintCashABAssignReport() : print all CASHIER NO. of A/B KEY REPORT   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-11-92: 00.00.01 : J.Ikeda   : initial                                   
* Nov-20-92: 01.00.00 : K.You     : Delete 0 limitation for Cash Number                                   
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
#include <rfl.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintCashABAssignRead( PARACASHABASSIGN *pData )
*               
*     Input:    *pData          : pointer to structure for PARACASHABASSIGN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays CASHIER NO. of A/B KEY.
*===========================================================================
*/
SHORT MaintCashABAssignRead( PARACASHABASSIGN *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.CashABAssign.uchAddress;

        /* check address */
        if (uchAddress > ASN_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || ASN_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.CashABAssign.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.CashABAssign.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.CashABAssign));                     /* call ParaCashABAssignRead() */

	RflGetLead (MaintWork.CashABAssign.aszMnemonics, LDT_ID_ADR);
    DispWrite(&(MaintWork.CashABAssign));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCashABAssignWrite( PARACASHABASSIGN *pData )
*               
*     Input:    *pData             : pointer to structure for PARACASHABASSIGN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints CASHIER NO. of A/B KEY.
*===========================================================================
*/

SHORT MaintCashABAssignWrite( PARACASHABASSIGN *pData )
{

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /* cashier number has already checked by UI */
    MaintWork.CashABAssign.uchAddress = pData->uchAddress;
    MaintWork.CashABAssign.ulCashierNo = pData->ulCashierNo;
    CliParaWrite(&(MaintWork.CashABAssign));                    /* call ParaCashABAssignWrite() */

    /* control header item */
    MaintHeaderCtl(AC_CASH_ABKEY, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.CashABAssign.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.CashABAssign));
    
    /* set address for display next address */

    /* check address */
    MaintWork.CashABAssign.uchAddress++;

    if (MaintWork.CashABAssign.uchAddress > ASN_ADR_MAX) {
        MaintWork.CashABAssign.uchAddress = 1;                  /* initialize address */
    }
    MaintCashABAssignRead(&(MaintWork.CashABAssign));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintCashABAssignReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all CASHIER NO. of A/B KEY REPORT.
*===========================================================================
*/

VOID MaintCashABAssignReport( VOID )
{
    UCHAR            i;
	PARACASHABASSIGN ParaCashABAssign = {0};

    /* control header item */
    MaintHeaderCtl(AC_CASH_ABKEY, RPT_ACT_ADR);
    ParaCashABAssign.uchMajorClass = CLASS_PARACASHABASSIGN;

    /* set journal bit & receipt bit */
    ParaCashABAssign.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= ASN_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaCashABAssign.uchAddress = i;
        CliParaRead(&ParaCashABAssign);                         /* call ParaCashABAssignRead() */
        PrtPrintItem(NULL, &ParaCashABAssign);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

    
    
    
    
    
