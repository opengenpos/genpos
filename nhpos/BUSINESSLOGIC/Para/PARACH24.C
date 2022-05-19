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
* Title       : Parameter ParaChar24 Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARACH24.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaChar24Read()  : reads 24 CHARACTERS MNEMONICS
*               ParaChar24Write() : sets 24 CHARACTERS MNEMONICS
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
#include<string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaChar24Read( PARACHAR24 *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_CHAR24_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads 24 CHARACTERS MNEMONICS of appointed address.
;===============================================================================
**/

VOID ParaChar24Read( PARACHAR24 *pData )
{
    USHORT    i;

    i = (USHORT)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */     
	if (i >= MAX_CH24_NO ) {
		memset(&pData->aszMnemonics[0], 0x00, PARA_CHAR24_LEN * sizeof(pData->aszMnemonics[0]));
	}
	else{
		_tcsncpy(&pData->aszMnemonics[0], &Para.ParaChar24[i][0], PARA_CHAR24_LEN);
		pData->aszMnemonics[PARA_CHAR24_LEN] = '\0';
	}
}

/**
;=============================================================================
;    Synopsis:  VOID ParaChar24Write( PARACHAR24 *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_CHAR24_LEN+1]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets 24 CHARACTERS MNEMONICS in appointed address.
;===============================================================================
**/

VOID ParaChar24Write( PARACHAR24 *pData )
{
    USHORT    i;

    i = (USHORT)(pData->uchAddress - 1);  /* "-1" fits program address to RAM address */            
	if (i < MAX_CH24_NO ) {
		_tcsncpy(&Para.ParaChar24[i][0], &pData->aszMnemonics[0], PARA_CHAR24_LEN);
	}
}


