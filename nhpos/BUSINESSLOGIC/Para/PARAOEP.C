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
* Title       : Parameter ParaOep Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARADISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaOepRead()  : reads DISCOUNT/BONUS RATE
*               ParaOepWrite() : sets DISCOUNT/BONUS RATE
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
* 2171 for Win32
* Aug-26-99:  01.00.00 : K.Iwata  : V1.0 Initial
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
#include <string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaOepRead( PARAOEPTBL *pData )
;       input:  pData->uchAddress
;      output:  pData->uchOepRate
;
;      Return:  Nothing
;
; Descruption:  This function reads OEPOUNT/BONUS RATE.
;===============================================================================
**/

VOID ParaOepRead( PARAOEPTBL *pData )
{

    UCHAR    i,j;

    i = ( UCHAR)(pData->uchTblNumber - 1);  /* "-1" fits program address to RAM address */  
    if (pData->uchTblAddr == 0) {
		memcpy(&(pData->uchOepData[0]), &ParaOep[i][0], MAX_ONE_OEPTABLE_SIZE); /* ### Mod (2171 for Win32) V1.0 */
        return;
    }

    j = ( UCHAR)(pData->uchTblAddr - 1);    /* "-1" fits program address to RAM address */  

    pData->uchOepData[0] = ParaOep[i][j];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaOepWrite( PARAOEPTBL *pData )
;       input:  pData->uchAddress
;               pData->uchOepRate           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets OEPOUNT/BONUS RATE.
;===============================================================================
**/

VOID ParaOepWrite( PARAOEPTBL *pData )
{

    UCHAR    i,j;

    i = ( UCHAR)(pData->uchTblNumber - 1);  /* "-1" fits program address to RAM address */  
    j = ( UCHAR)(pData->uchTblAddr - 1);    /* "-1" fits program address to RAM address */  

    ParaOep[i][j] = pData->uchOepData[0];

}


