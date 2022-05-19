/*     
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintAutoCoupon Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPAMT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Preset Amount Cash Tender.
*                                  
*           The provided function names are as follows: 
*                                           
*              MaintAutoCouponRead()      : read & display Preset Amount Cash Tender
*              MaintAutoCouponWrite()     : set & print Preset Amount Cash Tender
*              MaintAutoCouponReport()    : print all Preset Amount Cash Tender  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-02-93: 01.00.12 : K.You     : initial                                   
*
* Sep-06-19: 02.03.01 : R.Chambers : removed unused function MaintAutoCouponReport().
*
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
#include <prt.h>
#include <rfl.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintAutoCouponRead( PARAAUTOCPN *pData )
*               
*     Input:    *pData          : pointer to structure for PARAAUTOCPN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data 
*
** Description: This function reads & displays Preset Amount Cash Tender.
*===========================================================================
*/

SHORT MaintAutoCouponRead( PARAAUTOCPN *pData )
{
    /* check address */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
       pData->uchAddress = ++MaintWork.AutoCoupon.uchAddress;

        /* check address */
        if (pData->uchAddress > AUTOCPN_AMT_ADR_MAX) { 
            pData->uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        pData->uchAddress = pData->uchAddress;
    }

    if (pData->uchAddress < 1 || AUTOCPN_AMT_ADR_MAX < pData->uchAddress) {        
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }
    MaintWork.AutoCoupon.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.AutoCoupon.uchAddress = pData->uchAddress;
    CliParaRead(&(MaintWork.AutoCoupon)); 
	
    /* Set Leadthru Data */
	RflGetLead (MaintWork.AutoCoupon.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.AutoCoupon));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintAutoCouponWrite( PARAAUTOCPN *pData )
*               
*     Input:    *pData             : pointer to structure for PARAAUTOCPN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints Preset Amount Cash Tender.
*===========================================================================
*/

SHORT MaintAutoCouponWrite( PARAAUTOCPN *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  preset amount has already checked by UI */

    MaintWork.AutoCoupon.ulAmount = pData->ulAmount;
    CliParaWrite(&(MaintWork.AutoCoupon));                      

    /* control header item */
        
    MaintHeaderCtl(PG_PRESET_AMT, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.AutoCoupon.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.AutoCoupon));
    
    /* set address for display next address */

    /* check address */

    MaintWork.AutoCoupon.uchAddress++;

    if (MaintWork.AutoCoupon.uchAddress > AUTOCPN_AMT_ADR_MAX) {
        MaintWork.AutoCoupon.uchAddress = 1;                    /* initialize address */
    }
    MaintAutoCouponRead(&(MaintWork.AutoCoupon));
    return(SUCCESS);
}

/****** End of Source ******/    
    
    
    
    
