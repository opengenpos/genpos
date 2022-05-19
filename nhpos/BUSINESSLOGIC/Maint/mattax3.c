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
* Program Name: MATTAX3.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TAX TABLE #3.
*                                  
*           The provided function names are as follows: 
* 
*               MaintTaxTblReadNo3()           : read & display tax table
*               MaintTaxRateEditNo3()          : edit rate
*               MaintTaxableAmountEditNo3()    : edit taxable amount 
*               MaintTaxAmountEditNo3()        : edit tax amount
*               MaintTaxableAmountErrorNo3()   : correct taxable amount
*               MaintTaxTblWriteNo3()          : write tax table in sytem ram
*               MaintTaxTblDeleteNo3()         : delete tax table from system ram
*               MaintTaxTblCalNo3()            : calculate & print tax
*               MaintTaxTblTmpReportNo3()      : print tax table report during tax table creation 
*               MaintTaxTblReportNo3()         : print tax table report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-18-92: 00.00.01 : J.Ikeda   : initial                                   
*          :          :           :                                    
*
** NCR2171 **
* Aug-26-99: 01.00.00 : M.Teraki  : initial (for 2171)
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
** Synopsis:    VOID MaintTaxTblReadNo3( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reads TAX TABLE No.3.
*===========================================================================
*/

VOID MaintTaxTblReadNo3(VOID)
{
    
    MaintTaxTblRead(CLASS_PARATAXTBL3);
}                                                                                              
                                                                                                  
/*                                                                                                  
*===========================================================================                   
** Synopsis:    SHORT MaintTaxRateEditNo3(MAINTTAXTBL *pData)                                  
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
** Description: This function edits tax no.3 rate.
*===========================================================================
*/

SHORT MaintTaxRateEditNo3(MAINTTAXTBL *pData)
{

    return(MaintTaxRateEdit(pData, CLASS_MAINTTAXTBL3));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountEditNo3(MAINTTAXTBL *pData)
*               
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits taxable no.3 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountEditNo3(MAINTTAXTBL *pData)
{

    return(MaintTaxableAmountEdit(pData, CLASS_MAINTTAXTBL3));
}
                                   
/*                               
*===========================================================================
** Synopsis:    SHORT MaintTaxAmountEditNo3(MAINTTAXTBL *pData)
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
** Description: This function edits tax no.3 amount.
*===========================================================================
*/

SHORT MaintTaxAmountEditNo3(MAINTTAXTBL *pData)
{

    return(MaintTaxAmountEdit(pData, CLASS_MAINTTAXTBL3,
                               (USHORT)(usMaintTbl1Len + usMaintTbl2Len), 2));
}    

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountErrorNo3(MAINTTAXTBL *pData)
*               
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function corrects taxable no.3 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountErrorNo3(MAINTTAXTBL *pData)
{

    return(MaintTaxableAmountError(pData, CLASS_MAINTTAXTBL3,
                                   (USHORT)(usMaintTbl1Len + usMaintTbl2Len)));
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblWriteNo3( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function writes TAX TABLE No.3 in SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblWriteNo3( VOID )
{

    UCHAR       uchRestTbl;

    /* calculate rest table size */

    uchRestTbl = ( UCHAR)(MAX_TTBLDATA_SIZE - (usMaintTbl1Len
                        + usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len));

    /* set tax table #3 length */

    MaintWork.TaxTbl.TaxData.ausTblLen[2] = ( USHORT)(uchMaintTblOff
                                       - usMaintTbl1Len - usMaintTbl2Len + 1); 

    /* modify tax table */

    memmove(&MaintWork.TaxTbl.TaxData.auchTblData[uchMaintTblOff + 1], 
            &MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len
             + usMaintTbl2Len + usMaintTbl3Len + uchRestTbl], 
            usMaintTbl4Len);

    /* write tax table */

    MaintWork.TaxTbl.uchMajorClass = CLASS_PARATAXTBL3;
    CliParaWrite(&MaintWork.TaxTbl);
}                                            

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblDeleteNo3( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function delete TAX TABLE #3 from SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblDeleteNo3( VOID )
{

    MaintTaxTblDelete(CLASS_PARATAXTBL3);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxTblCalNo3(MAINTTAXTBL *pData)
*               
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount) 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function caluculate & print TAX no.3.
*===========================================================================
*/

SHORT MaintTaxTblCalNo3(MAINTTAXTBL *pData)
{

    return(MaintTaxTblCal(pData, CLASS_MAINTTAXTBL3, RFL_USTAX_3));
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblTmpReportNo3( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE No.3 REPORT during tax table creation 
*===========================================================================
*/

VOID MaintTaxTblTmpReportNo3( VOID )
{

    MaintTaxTblTmpReport(CLASS_MAINTTAXTBL3, (USHORT)(usMaintTbl1Len + usMaintTbl2Len), 2);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReportNo3( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE REPORT
*===========================================================================
*/

VOID MaintTaxTblReportNo3( VOID )
{

    MaintTaxTblReport(CLASS_MAINTTAXTBL3);
}

/****** End of Source ******/
