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
* Title       : Parameter ParaPigRule Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAPIG.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaPigRuleRead()   : reads Pig Rule Table
*               ParaPigRuleWrite()  : sets Pig Rule Table
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-09-93:  01.00.12 : K.You      : initial                                   
*          :           :                                            
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
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>


/**
;=============================================================================
;    Synopsis:  VOID ParaPigRuleRead( PARAPIGRULE *pData )
;       input:  pData->uchAddress
;      output:  pData->ulPigRule
;
;      Return:  Nothing
;
; Description:  This function reads Pig Rule Table.
;===============================================================================
**/

VOID ParaPigRuleRead( PARAPIGRULE *pData )
{
            
    UCHAR   i;


    i = ( UCHAR)(pData->uchAddress - 1);      /* "-1" fits program address to RAM address */    

    pData->ulPigRule = ParaPigRule[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaPigRuleWrite( PARAPIGRULE *pData )
;       input:  pData->uchAddress
;               pData->ulPigRule           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Description:  This function sets Pig Rule Table.
;===============================================================================
**/

VOID ParaPigRuleWrite( PARAPIGRULE *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */  

    ParaPigRule[i] = pData->ulPigRule;

}

/****** End of Source ******/
