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
* Dec-24-23:  02.04.00 : R.Chambers : added range checks, new ReadAll,WriteAll functions
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
    UCHAR    i = ( UCHAR)(pData->uchSrcPtr - 1);    /* "-1" fits program address to RAM address */ 

    pData->uchDesPtr = 0;

    if (i < MAX_DEST_SIZE) pData->uchDesPtr = ParaManuAltKitch[i];
}

UCHAR* ParaManuAltKitchReadAll(UCHAR   paraManuAltKitch[MAX_DEST_SIZE]) {
    memmove(paraManuAltKitch, ParaManuAltKitch, sizeof(paraManuAltKitch));
    return paraManuAltKitch;
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
    UCHAR    i = ( UCHAR)(pData->uchSrcPtr - 1);    /* "-1" fits program address to RAM address */

    if (i < MAX_DEST_SIZE) ParaManuAltKitch[i] = pData->uchDesPtr;
}


VOID ParaManuAltKitchWriteAll(UCHAR   paraManuAltKitch[MAX_DEST_SIZE]) {
    memmove(ParaManuAltKitch, paraManuAltKitch, sizeof(ParaManuAltKitch));
}

