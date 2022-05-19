/*
*===========================================================================
* Title       : Parameter ParaLeadThru Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARALEAD.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaLeadThruRead()  : reads LEADTHROUGH of appointed address
*               ParaLeadThruWrite() : sets LEADTHROUGH in appointed address
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
#include<string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaLeadThruRead( PARALEADTHRU *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_LEADTHRU_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads LEADTHROUGH of appointed address.
;===============================================================================
**/
VOID ParaLeadThruRead( PARALEADTHRU *pData )
{
    USHORT    i;

	NHPOS_ASSERT(pData->uchAddress > 0);

    i = (USHORT)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           
	if (i >= MAX_LEAD_NO ) {
		memset(&pData->aszMnemonics[0], 0x00, PARA_LEADTHRU_LEN * sizeof(pData->aszMnemonics[0]));
	}
	else{
		_tcsncpy(&pData->aszMnemonics[0], &Para.ParaLeadThru[i][0], PARA_LEADTHRU_LEN);
		pData->aszMnemonics[PARA_LEADTHRU_LEN] = '\0';    // ensure zero termination of string.
	}
}                    

/**
;=============================================================================
;    Synopsis:  VOID ParaLeadThruWrite( PARALEADTHRU *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_LEADTHRU_LEN+1]
;      output:  Nothing
;      
;      Return:  Nothing
;
; Descruption:  This function sets LEADTHROUGH in appointed address.
;===============================================================================
**/

VOID ParaLeadThruWrite( PARALEADTHRU *pData )
{
    USHORT   i;

	NHPOS_ASSERT(pData->uchAddress > 0);

    i = ( USHORT)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           
	if (i < MAX_LEAD_NO) {
		_tcsncpy(&Para.ParaLeadThru[i][0], &pData->aszMnemonics[0], PARA_LEADTHRU_LEN);
	}
}
