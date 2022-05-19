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
* Title       : MaintSecurityNo Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATSECU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function read & write & report SECURITY NO.
*                                  
*           The provided function names are as follows: 
* 
*              MaintSecurityNoRead()   : read & display SECURITY NO
*              MaintSecurityNoWrite()  : set & print SECURITY NO
*              MaintSecurityNoReport() : print all SECURITY NO REPORT
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
** Synopsis:    SHORT MaintSecurityNoRead( PARASECURITYNO *pData )
*               
*     Input:    *pData          : pointer to structure for PARASECURITYNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays SECURITY NO.
*===========================================================================
*/

SHORT MaintSecurityNoRead( PARASECURITYNO *pData )
{

    UCHAR uchAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.SecurityNo.uchAddress;

        /* check address */

        if (uchAddress > SEC_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 

    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < 1 || SEC_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.SecurityNo.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.SecurityNo.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.SecurityNo));                       /* call ParaSecurityNoRead() */
    DispWrite(&(MaintWork.SecurityNo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintSecurityNoWrite( PARASECURITYNO *pData )
*               
*     Input:    *pData             : pointer to structure for PARASECURITYNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints SECURITY NO.
*===========================================================================
*/

SHORT MaintSecurityNoWrite( PARASECURITYNO *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  security number has already checked by UI */

    MaintWork.SecurityNo.usSecurityNumber = pData->usSecurityNumber;
    CliParaWrite(&(MaintWork.SecurityNo));                      /* call ParaSecurityNoWrite() */

    /* control header item */
        
    MaintHeaderCtl(PG_SEC_PROG, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.SecurityNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.SecurityNo));
    
    /* set address for display next address */

    /* check address */

    MaintWork.SecurityNo.uchAddress++;

    if (MaintWork.SecurityNo.uchAddress > SEC_ADR_MAX) {
        MaintWork.SecurityNo.uchAddress = 1;                    /* initialize address */
    }
    MaintSecurityNoRead(&(MaintWork.SecurityNo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSecurityNoReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all SECURITY NO.
*===========================================================================
*/

VOID MaintSecurityNoReport( VOID )
{


    UCHAR               i;
    PARASECURITYNO      ParaSecurityNo;


    /* control header item */
        
    MaintHeaderCtl(PG_SEC_PROG, RPT_PRG_ADR);
    ParaSecurityNo.uchMajorClass = CLASS_PARASECURITYNO;

    /* set journal bit & receipt bit */

    ParaSecurityNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= SEC_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaSecurityNo.uchAddress = i;
        CliParaRead(&ParaSecurityNo);                           /* call ParaSecurityNoRead() */
        PrtPrintItem(NULL, &ParaSecurityNo);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}
