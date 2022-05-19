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
* Title       : MaintPromotion Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPROM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function reads & writes SALES PROMOTION HEADER.
*
*           The provided function names are as follows: 
* 
*              MaintPromotionRead()   : read & display SALES PROMOTION HEADER
*              MaintPromotionWrite()  : read & display SALES PROMOTION HEADER
*              MaintPromotionReport() : report & print SALES PROMOTION HEADER REPORT
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
** Synopsis:    SHORT MaintPromotionRead( PARAPROMOTION *pData )
*               
*     Input:    *pData         : ponter to structure for PARAPROMOTION
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays SALES PROMOTION HEADER.
*===========================================================================
*/

SHORT MaintPromotionRead( PARAPROMOTION *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.Promotion.aszProSales, '\0', PARA_PROMOTION_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.Promotion.uchAddress;

        /* check address */

        if (uchAddress > PRM_ADR_MAX) {                         
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || PRM_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.Promotion.uchMajorClass = pData->uchMajorClass;   /* copy major class */
    MaintWork.Promotion.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.Promotion));                        /* call ParaPromotionRead() */ 
    DispWrite(&(MaintWork.Promotion));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPromotionWrite( PARAPROMOTION *pData )
*               
*     Input:    *pData         : pointer to structure for PARAPROMOTION
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function sets & prints SALES PROMOTION HEADER.
*===========================================================================
*/

SHORT MaintPromotionWrite( PARAPROMOTION *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.Promotion.aszProSales, pData->aszProSales, PARA_PROMOTION_LEN);
    //memcpy(MaintWork.Promotion.aszProSales, pData->aszProSales, PARA_PROMOTION_LEN);
    CliParaWrite(&(MaintWork.Promotion));                       /* call ParaPromotionWrite() */

    /* control header item */

    MaintHeaderCtl(AC_SALES_PROMOTION, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.Promotion.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.Promotion));

    /* set address for Display next address */

    MaintWork.Promotion.uchAddress++;

    /* check address */

    if (MaintWork.Promotion.uchAddress > PRM_ADR_MAX) {
        MaintWork.Promotion.uchAddress = 1;                     /* initialize address */
    }
    MaintPromotionRead(&(MaintWork.Promotion));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPromotionReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all SALES PROMOTION HEADER REPORT.
*===========================================================================
*/

VOID MaintPromotionReport( VOID )
{

    UCHAR          i;
    PARAPROMOTION  ParaPromotion;

    /* initialize */

/*    memset(ParaPromotion.aszProSales, '\0', PARA_PROMOTION_LEN+1); */

    /* control header */

    MaintHeaderCtl(AC_SALES_PROMOTION, RPT_ACT_ADR);

    /* set major class */

    ParaPromotion.uchMajorClass = CLASS_PARAPROMOTION;

    /* set journal bit & receipt bit */

    ParaPromotion.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= PRM_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaPromotion.uchAddress = i;
        CliParaRead(&ParaPromotion);                            /* call ParaPromotionRead() */
        PrtPrintItem(NULL, &ParaPromotion);
    }

    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}
















