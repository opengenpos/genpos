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
* Title       : Parameter ParaAutoAltKitch Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAAUTO.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaAutoAltKitchRead()  : reads KITCHEN PRINTER ALTERNATIVE LIST
*               ParaAutoAltKitchWrite() : sets KITCHEN PRINTER ALTERNATIVE
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.00 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
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
;    Synopsis:  VOID ParaAutoAltKitchRead( PARAALTKITCH *pData )
;       input:  pData->uchSrcPtr
;      output:  pData->uchDesPtr
;
;      Return:  Nothing
;
; Descruption:  This function reads KITCHEN PRINTER ALTERNATIVE LIST.
;===============================================================================
**/

VOID ParaAutoAltKitchRead( PARAALTKITCH *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchSrcPtr - 1);      /* "-1" fits program address to RAM address */

    pData->uchDesPtr = ParaAutoAltKitch[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaAutoAltKitchWrite( PARAALTKITCH *pData )
;       input:  pData->uchSrcPtr
;               pData->uchDesPtr           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets KITCHEN PRINTER ALTERNATIVE.
;===============================================================================
**/

VOID ParaAutoAltKitchWrite( PARAALTKITCH *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchSrcPtr - 1);     /* "-1" fits program address to RAM address */
                                     
    ParaAutoAltKitch[i] = pData->uchDesPtr;

}


