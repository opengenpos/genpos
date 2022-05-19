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
* Title       : MaintTaxTable Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATTAX2.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TAX TABLE #2.
*                                  
*           The provided function names are as follows: 
* 
*               MaintTaxTblReadNo2()           : read & display tax table
*               MaintTaxRateEditNo2()          : edit rate
*               MaintTaxableAmountEditNo2()    : edit taxable amount 
*               MaintTaxAmountEditNo2()        : edit tax amount
*               MaintTaxableAmountErrorNo2()   : correct taxable amount
*               MaintTaxTblWriteNo2()          : write tax table in system ram
*               MaintTaxTblDeleteNo2()         : delete tax table from system ram
*               MaintTaxTblCalNo2()            : calculate & print tax
*               MaintTaxTblTmpReportNo2()      : print tax table report during tax table creation 
*               MaintTaxTblReportNo2()         : print tax table report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-18-92: 00.00.01 : J.Ikeda   : initial                                   
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
/* #include <uie.h> */
/* #include <pif.h> */
/* #include <log.h> */
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <display.h> */
/* #include <regstrct.h> */
/* #include <transact.h> */
/* #include <prt.h> */
#include <csstbpar.h>
/* #include <appllog.h> */

#include "maintram.h" 


/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReadNo2( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reads TAX TABLE No.2.
*===========================================================================
*/

VOID MaintTaxTblReadNo2(VOID)
{

    MaintTaxTblRead(CLASS_PARATAXTBL2);
}                                                                                              
                                                                                                  
/*                                                                                                  
*===========================================================================                   
** Synopsis:    SHORT MaintTaxRateEditNo2(MAINTTAXTBL *pData)                                  
*                                                                                              
*     Input:    *pData          : pointer to structure for MAINTTAXTBL                         
*                  (pData->uchInteger)
*                  (pData->uchDenominator)
*                  (pData->uchNumerator)
*
*    Output:    Nothing 
*     InOut:    Nothing
*                 
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits tax no.2 rate.
*===========================================================================
*/

SHORT MaintTaxRateEditNo2(MAINTTAXTBL *pData)
{

    return(MaintTaxRateEdit(pData, CLASS_MAINTTAXTBL2));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountEditNo2(MAINTTAXTBL *pData)
*               
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits taxable no.2 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountEditNo2(MAINTTAXTBL *pData)
{

    return(MaintTaxableAmountEdit(pData, CLASS_MAINTTAXTBL2));
}
                                   
/*                               
*===========================================================================
** Synopsis:    SHORT MaintTaxAmountEditNo2(MAINTTAXTBL *pData)
*               
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->usTaxAmount)
*
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*               LDT_FLFUL_ADR   : tax table over flow
*               MAINT_2WDC_OVER : taxable amount is over 2*WDC
*
** Description: This function edits tax no.2 amount.
*===========================================================================
*/

SHORT MaintTaxAmountEditNo2(MAINTTAXTBL *pData)
{

    return(MaintTaxAmountEdit(pData, CLASS_MAINTTAXTBL2, usMaintTbl1Len, 1));
}    

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountErrorNo2(MAINTTAXTBL *pData)
*               
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function corrects taxable no.2 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountErrorNo2(MAINTTAXTBL *pData)
{

    return(MaintTaxableAmountError(pData, CLASS_MAINTTAXTBL2, usMaintTbl1Len));
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblWriteNo2( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function writes TAX TABLE No.2 in SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblWriteNo2( VOID )
{

    UCHAR       uchRestTbl;

    /* calculate rest table size */

    uchRestTbl = ( UCHAR)(MAX_TTBLDATA_SIZE - (usMaintTbl1Len
                        + usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len));

    /* set tax table #2 length */

    MaintWork.TaxTbl.TaxData.ausTblLen[1] = ( USHORT)(uchMaintTblOff - usMaintTbl1Len + 1); 

    /* modify tax table */

    memmove(&MaintWork.TaxTbl.TaxData.auchTblData[uchMaintTblOff + 1], 
            &MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len + usMaintTbl2Len + uchRestTbl], 
            usMaintTbl3Len + usMaintTbl4Len);

    /* write tax table */

    MaintWork.TaxTbl.uchMajorClass = CLASS_PARATAXTBL2;
    CliParaWrite(&MaintWork.TaxTbl);
}                                            

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblDeleteNo2( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function delete TAX TABLE #2 from SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblDeleteNo2( VOID )
{

    MaintTaxTblDelete(CLASS_PARATAXTBL2);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxTblCalNo2(MAINTTAXTBL *pData)
*               
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount) 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function caluculate & print TAX no.2.
*===========================================================================
*/

SHORT MaintTaxTblCalNo2(MAINTTAXTBL *pData)
{

    return(MaintTaxTblCal(pData, CLASS_MAINTTAXTBL2, RFL_USTAX_2));
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblTmpReportNo2( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE No.2 REPORT during tax table creation 
*===========================================================================
*/

VOID MaintTaxTblTmpReportNo2( VOID )
{

    MaintTaxTblTmpReport(CLASS_MAINTTAXTBL2, usMaintTbl1Len, 1);
}


/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReportNo2( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE NO.2 REPORT
*===========================================================================
*/

VOID MaintTaxTblReportNo2( VOID )
{

    MaintTaxTblReport(CLASS_MAINTTAXTBL2);
}

/****** End of Source ******/

