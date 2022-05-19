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
* Program Name: MATTAX4.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TAX TABLE #4.
*                                  
*           The provided function names are as follows: 
* 
*               MaintTaxTblReadNo4()           : read & display tax table
*               MaintTaxRateEditNo4()          : edit rate
*               MaintTaxableAmountEditNo4()    : edit taxable amount 
*               MaintTaxAmountEditNo4()        : edit tax amount
*               MaintTaxableAmountErrorNo4()   : correct taxable amount
*               MaintTaxTblWriteNo4()          : write tax table in sytem ram
*               MaintTaxTblDeleteNo4()         : delete tax table from system ram
*               MaintTaxTblCalNo4()            : calculate & print tax
*               MaintTaxTblTmpReportNo4()      : print tax table report during tax table creation 
*               MaintTaxTblReportNo4()         : print tax table report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name     : Description
* Jul-07-93 : 00.00.01 : M.Suzuki   : initial                              
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
** Synopsis:    VOID MaintTaxTblReadNo4( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reads TAX TABLE No.4.
*===========================================================================
*/

VOID MaintTaxTblReadNo4(VOID)
{
    
    MaintTaxTblRead(CLASS_PARATAXTBL4);
}                                                                                              
                                                                                                  
/*                                                                                                  
*===========================================================================                   
** Synopsis:    SHORT MaintTaxRateEditNo4(MAINTTAXTBL *pData)                                  
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
** Description: This function edits tax No.4 rate.
*===========================================================================
*/

SHORT MaintTaxRateEditNo4(MAINTTAXTBL *pData)
{

    return(MaintTaxRateEdit(pData, CLASS_MAINTTAXTBL4));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountEditNo4(MAINTTAXTBL *pData)
*               
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits taxable no.4 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountEditNo4(MAINTTAXTBL *pData)
{

    return(MaintTaxableAmountEdit(pData, CLASS_MAINTTAXTBL4));
}
                                   
/*                               
*===========================================================================
** Synopsis:    SHORT MaintTaxAmountEditNo4(MAINTTAXTBL *pData)
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
** Description: This function edits tax no.4 amount.
*===========================================================================
*/

SHORT MaintTaxAmountEditNo4(MAINTTAXTBL *pData)
{

    return(MaintTaxAmountEdit(pData, (UCHAR)CLASS_MAINTTAXTBL4,
                     (USHORT)(usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len), (UCHAR)3)); /* ### Mod (2171 for Win32) V1.0 */
}    

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountErrorNo4(MAINTTAXTBL *pData)
*               
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function corrects taxable no.4 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountErrorNo4(MAINTTAXTBL *pData)
{

    return(MaintTaxableAmountError(pData, (UCHAR)CLASS_MAINTTAXTBL4,
                          (USHORT)(usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len))); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblWriteNo4( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function writes TAX TABLE No.4 in SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblWriteNo4( VOID )
{

    /* set tax table #4 length */

    MaintWork.TaxTbl.TaxData.ausTblLen[3] = ( USHORT)(uchMaintTblOff
                    - usMaintTbl1Len - usMaintTbl2Len  - usMaintTbl3Len + 1); 

    /* write tax table */

    MaintWork.TaxTbl.uchMajorClass = CLASS_PARATAXTBL4;
    CliParaWrite(&MaintWork.TaxTbl);
}                                            

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblDeleteNo4( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function delete TAX TABLE #4 from SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblDeleteNo4( VOID )
{

    MaintTaxTblDelete(CLASS_PARATAXTBL4);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxTblCalNo4(MAINTTAXTBL *pData)
*               
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount) 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function caluculate & print TAX no.4.
*===========================================================================
*/

SHORT MaintTaxTblCalNo4(MAINTTAXTBL *pData)
{

    return(MaintTaxTblCal(pData, CLASS_MAINTTAXTBL4, RFL_USTAX_4));
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblTmpReportNo4( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE REPORT during tax table creation 
*===========================================================================
*/

VOID MaintTaxTblTmpReportNo4( VOID )
{

    MaintTaxTblTmpReport((UCHAR)CLASS_MAINTTAXTBL4, (USHORT)(usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len), (UCHAR)3); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReportNo4( VOID )
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

VOID MaintTaxTblReportNo4( VOID )
{

    MaintTaxTblReport(CLASS_MAINTTAXTBL4);
}

/****** End of Source ******/
