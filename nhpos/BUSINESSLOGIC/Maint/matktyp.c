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
* Title       : MaintTtlKeyTyp Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATKTYP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TOTAL KEY TYPE.
*
*           The provided function names are as follows: 
*                          
*              MaintTtlKeyTypRead()   : read & display TOTAL KEY TYPE
*              MaintTtlKeyTypWrite()  : set & print TOTAL KEY TYPE
*              MaintTtlKeyTypReport() : print all TOTAL KEY TYPE REPORT   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-10-92: 00.00.01 : J.Ikeda   : initial                                   
* Nov-18-92: 01.00.00 : K.You     : Changed Max Value of Total Type (for GCA)                                    
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
** Synopsis:    SHORT MaintTtlKeyTypRead( PARATTLKEYTYP *pData )
*               
*     Input:    *pData          : pointer to structure for PARATTLKEYTYP
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays TOTAL KEY TYPE.
*===========================================================================
*/
SHORT MaintTtlKeyTypRead( PARATTLKEYTYP *pData )
{

    UCHAR uchAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.TtlKeyTyp.uchAddress;

        /* check address */
        if (uchAddress > TLTY_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < 1 || TLTY_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.TtlKeyTyp.uchMajorClass = pData->uchMajorClass;   /* copy major class */
    MaintWork.TtlKeyTyp.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.TtlKeyTyp));                        /* call ParaTtlKeyTypRead() */
    DispWrite(&(MaintWork.TtlKeyTyp));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTtlKeyTypWrite( PARATTLKEYTYP *pData )
*               
*     Input:    *pData             : pointer to structure for PARATTLKEYTYP
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints TOTAL KEY TYPE.
*===========================================================================
*/

SHORT MaintTtlKeyTypWrite( PARATTLKEYTYP *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check range */

    if (pData->uchTypeIdx != 0) {
        if (pData->uchTypeIdx < MAINT_TLTY_MIN || pData->uchTypeIdx > MAINT_TLTY_MAX) {
            return(LDT_KEYOVER_ADR);                                /* wrong data */
        }
		
		//All of these values can be used.
        /*if (pData->uchTypeIdx == 19 ||
            pData->uchTypeIdx == 29 ||
            pData->uchTypeIdx == 39 ||
            pData->uchTypeIdx == 49 ||  
            pData->uchTypeIdx == 59 || 
            pData->uchTypeIdx == 69) { 
           // return(LDT_KEYOVER_ADR);                                /* wrong data 
        }*/
    }
    MaintWork.TtlKeyTyp.uchTypeIdx = pData->uchTypeIdx;
    CliParaWrite(&(MaintWork.TtlKeyTyp));                       /* call ParaTtlKeyTypWrite() */

    /* control header item */
        
    MaintHeaderCtl(PG_TTLKEY_TYP, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.TtlKeyTyp.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.TtlKeyTyp));
    
    /* set address for display next address */

    /* check address */

    MaintWork.TtlKeyTyp.uchAddress++;
    
    if (MaintWork.TtlKeyTyp.uchAddress > TLTY_ADR_MAX) {
        MaintWork.TtlKeyTyp.uchAddress = 1;                     /* initialize address */
    }
    MaintTtlKeyTypRead(&(MaintWork.TtlKeyTyp));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTtlKeyTypReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all TOTAL KEY TYPE REPORT.
*===========================================================================
*/

VOID MaintTtlKeyTypReport( VOID )
{
    UCHAR            i;
    PARATTLKEYTYP    ParaTtlKeyTyp;

    /* control header item */
        
    MaintHeaderCtl(PG_TTLKEY_TYP, RPT_PRG_ADR);
    ParaTtlKeyTyp.uchMajorClass = CLASS_PARATTLKEYTYP;

    /* set journal bit & receipt bit */

    ParaTtlKeyTyp.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= TLTY_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaTtlKeyTyp.uchAddress = i;
        CliParaRead(&ParaTtlKeyTyp);                              /* call ParaTtlKeyTypRead() */
        PrtPrintItem(NULL, &ParaTtlKeyTyp);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

    
    
    
    
    
