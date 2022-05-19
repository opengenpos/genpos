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
* Title       : MaintActCodeSec Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATACT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report ACTION CODE SECURITY.
*
*           The provided function names are as follows: 
* 
*              MaintActCodeSecRead()   : read & display ACTION CODE SECURITY
*              MaintActCodeSecWrite()  : set & print ACTION CODE SECURITY data
*              MaintActCodeSecReport() : print all ACTION CODE SECURITY REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-12-92: 00.00.01 : J.Ikeda   : initial                                   
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

/**
==============================================================================
;    Static Area Declaration                          
=============================================================================
**/

UCHAR uchMaintByteSec;

/*
*===========================================================================
** Synopsis:    SHORT MaintActCodeSecRead( PARAACTCODESEC *pData )
*               
*     Input:    *pData            : pointer to structure for PARAACTCODESEC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : successful
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function reads & displays ACTCODESEC.
*===========================================================================
*/

SHORT MaintActCodeSecRead( PARAACTCODESEC *pData )
{

    USHORT  usAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */
        usAddress = ++MaintWork.ActCodeSec.usAddress;

        /* check address */

        if (usAddress > ACSC_ADR_MAX) {                   
            usAddress = 1;                                          /* initialize address */
        }
    } else {                                                        /* with data */
        usAddress = pData->usAddress;
    }

    /* check address */

    if (usAddress < 1 || ACSC_ADR_MAX < usAddress) {
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }            
    MaintWork.ActCodeSec.uchMajorClass = pData->uchMajorClass;      /* copy major class */
    MaintWork.ActCodeSec.usAddress = usAddress; 
    CliParaRead(&(MaintWork.ActCodeSec));                           /* call CliParaActCodeSecRead() */
    uchMaintByteSec = MaintWork.ActCodeSec.uchSecurityBitPatern;

    /* check odd/even number */

    if (!(MaintWork.ActCodeSec.usAddress & 0x0001)) {              /* in case of even address */
        MaintWork.ActCodeSec.uchSecurityBitPatern >>= 4;
    } else {                                                        /* in case of odd address */
        MaintWork.ActCodeSec.uchSecurityBitPatern &= 0x0F;
    }
    DispWrite(&(MaintWork.ActCodeSec));
    return(SUCCESS);
}    

/*
*===========================================================================
** Synopsis:    SHORT MaintActCodeSecWrite( PARAACTCODESEC *pData )
*               
*     Input:    *pData                   : pointer to structure for PARAACTCODESEC
*               uchMaintByteSec          : saved 1 byte ACTION CODE SECURITY data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : successful
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints ACTION CODE SECURITY data.
*===========================================================================
*/

SHORT MaintActCodeSecWrite( PARAACTCODESEC *pData )
{

    UCHAR uchSecBitData;


    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */
        return(LDT_SEQERR_ADR);                                     /* sequence error */
    }

    /* check data */

    if (MaintChkBinary(pData->aszMDCString, 4) == LDT_KEYOVER_ADR) {
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }

    /* convert string data into bit pattern */

    uchSecBitData = MaintAtoStatus(pData->aszMDCString);

    /* set bit data */

    if (!(MaintWork.ActCodeSec.usAddress & 0x0001)) {              /* in case of even address */
        uchMaintByteSec &= 0x0F;
        uchMaintByteSec |= uchSecBitData << 4;
    } else {                                                        /* in case of odd address */
        uchMaintByteSec &= 0xF0;
        uchMaintByteSec |= uchSecBitData;
    }
    MaintWork.ActCodeSec.uchSecurityBitPatern = uchMaintByteSec;
    CliParaWrite(&(MaintWork.ActCodeSec));                          /* call ParaActCodeSecWrite() */

    /* control header item */

    MaintHeaderCtl(PG_ACT_SEC, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.ActCodeSec.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);
    MaintWork.ActCodeSec.uchSecurityBitPatern = uchSecBitData;
    PrtPrintItem(NULL, &(MaintWork.ActCodeSec));

    /* set address for display next address */

    MaintWork.ActCodeSec.usAddress++;

    /* check address */

    if (MaintWork.ActCodeSec.usAddress > ACSC_ADR_MAX) {
        MaintWork.ActCodeSec.usAddress = 1;                         /* initialize address */
    }
    MaintActCodeSecRead(&(MaintWork.ActCodeSec));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintActCodeSecReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function prints all ACTION CODE SECURITY data report.
*===========================================================================
*/

VOID MaintActCodeSecReport( VOID )
{

    UCHAR            i;
    PARAACTCODESEC   ParaActCodeSec;


    /* control header item */
    
    MaintHeaderCtl(PG_ACT_SEC, RPT_PRG_ADR);

    /* set major class */

    ParaActCodeSec.uchMajorClass = CLASS_PARAACTCODESEC;

    /* set journal bit & receipt bit */

    ParaActCodeSec.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= ACSC_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                      /* depress abort key */
            MaintMakeAbortKey();
            break;
        }

        /* print odd address data */

        ParaActCodeSec.usAddress = i;                              /* in case of odd address */
        CliParaRead(&(ParaActCodeSec));                             /* call ParaMDCRead() */
        ParaActCodeSec.uchSecurityBitPatern &= 0x0F;
        PrtPrintItem(NULL, &ParaActCodeSec);

        /* print even address data */

        ParaActCodeSec.usAddress = ++i;                            /* in case of even address */
        if (i > ACSC_ADR_MAX) break;    /* not print if max address is odd */
        CliParaRead(&(ParaActCodeSec));                             /* call ParaMDCRead() */
        ParaActCodeSec.uchSecurityBitPatern &= 0xF0;
        ParaActCodeSec.uchSecurityBitPatern >>= 4;
        PrtPrintItem(NULL, &ParaActCodeSec);

    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
