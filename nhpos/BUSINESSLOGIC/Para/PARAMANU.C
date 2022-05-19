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
* Title       : Parameter ParaManuAltKitch Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAMANU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaManuAltKitchRead()  : reads KITCHEN PRINTER ALTERNATIVE LIST
*               ParaManuAltKitchWrite() : sets KITCHEN PRINTER ALTERNATIVE
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
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
==============================================================================
**/
#include	<tchar.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaManuAltKitchRead( PARAALTKITCH *pData )
;       input:  pData->uchSrcPtr
;      output:  pData->uchDesPtr
;
;      Return:  Nothing
;
; Descruption:  This function reads KITCHEN PRINTER ALTERNATIVE LIST.
;===============================================================================
**/

VOID ParaManuAltKitchRead( PARAALTKITCH *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchSrcPtr - 1);    /* "-1" fits program address to RAM address */ 

    pData->uchDesPtr = ParaManuAltKitch[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaManuAltKitchWrite( PARAALTKITCH *pData )
;       input:  pData->uchSrcPtr
;               pData->uchDesPtr           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets KITCHEN PRINTER ALTERNATIVE.
;===============================================================================
**/

VOID ParaManuAltKitchWrite( PARAALTKITCH *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchSrcPtr - 1);    /* "-1" fits program address to RAM address */

    ParaManuAltKitch[i] = pData->uchDesPtr;

}


