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
* Title       : Parameter ParaTaxTable Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARATTBL.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaTaxTableRead()  : reads all TAX TABLE
*               ParaTaxTableWrite() : sets all TAX TABLE
*                                                                 
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
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
;=============================================================================
**/
#include	<tchar.h>
#include	<memory.h>
#include	<string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>


/**
;=============================================================================
;    Synopsis:  VOID ParaTaxTableRead( PARATAXTBL *pData )
;       input:  Nothing
;      output:  pData->TaxData.ausTblLen[MAX_TTBLHEAD_SIZE]
;               pData->TaxData.auchTblData[MAX_TTBLEDATA_SIZE]
;
;      Return:  Nothing
;
; Descruption:  This function reads all TAX TABLE in a lump.
;===============================================================================
**/

VOID ParaTaxTableRead( PARATAXTBL *pData )
{
    memcpy(&pData->TaxData, &Para.ParaTaxTable[0], sizeof(TAXDATA)); /* ### Mod (2171 for Win32) V1.0 */
}

/**
;=============================================================================
;    Synopsis:  VOID ParaTaxTableWrite( PARATAXTBL *pData )
;       
;       input:  pData->TaxData.ausTblLen[MAX_TTBLHEAD_SIZE]
;               pData->TaxData.auchTblData[MAX_TTBLDATA_SIZE]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets all TAX TABLE in a lump.
;===============================================================================
**/

VOID ParaTaxTableWrite( PARATAXTBL *pData )
{
    memcpy(&Para.ParaTaxTable[0], &pData->TaxData, sizeof(TAXDATA)); /* ### Mod (2171 for Win32) V1.0 */
}
