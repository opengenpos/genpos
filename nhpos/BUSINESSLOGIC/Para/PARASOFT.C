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
* Title       : Parameter ParaSoftChk Module                         
* Category    : User Interface For Supervisor, NCR 2170 ENH US Hospitality Application         
* Program Name: PARASOFT.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSoftChkMsgRead()  : reads MNEMONICS for SOFT CHECK
*               ParaSoftChkMsgWrite() : sets MNEMONICS for SOFT CHECK
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Aug-23-93: J.IKEDA   : initial                                   
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaSoftChkMsgRead( PARASOFTCHK *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_SOFTCHK_LEN+1]
;
;      Return:  Nothing
;
; Description:  This function reads MNEMONICS for SOFT CHECK from the memory resident
;               database. We ensure the mnemonic copied is zero terminated and the
;               receiving buffer must be sized to PARA_SOFTCHK_LEN + 1 characters
;               in length.
;===============================================================================
**/
VOID ParaSoftChkMsgRead( PARASOFTCHK *pData )
{
    USHORT    i = pData->uchAddress - 1;               /* "-1" fits program address to RAM address */     

    pData->aszMnemonics[0] = '\0';                     // initialize destination to an empty string
	if (i < MAX_SOFTCHK_NO) {
		_tcsncpy(pData->aszMnemonics, ParaSoftChk[i], PARA_SOFTCHK_LEN);
	} else {
		NHPOS_ASSERT(i < MAX_SOFTCHK_NO);
	}
    pData->aszMnemonics[PARA_SOFTCHK_LEN] = '\0';      // ensure zero terminated string with max count of characters.
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSoftChkMsgWrite( PARASOFTCHK *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_SOFTCHK_LEN+1]
;      output:  Nothing
;
;      Return:  Nothing
;
; Description:  This function sets MNEMONICS for SOFT CHECK into the memory resident
;               database. We copy a maximum of PARA_SOFTCHK_LEN characters from the
;               argument into the memory resident database.
;
;               If the mnemonic is the full character count then it is not stored
;               zero terminated and reading from the memory resident database must
;               accommodate this possibility.
;===============================================================================
**/
VOID ParaSoftChkMsgWrite( PARASOFTCHK *pData )
{
    USHORT    i = pData->uchAddress - 1;  /* "-1" fits program address to RAM address */            

	if (i < MAX_SOFTCHK_NO) {
		_tcsncpy(ParaSoftChk[i], pData->aszMnemonics, PARA_SOFTCHK_LEN);
	} else {
		NHPOS_ASSERT(i < MAX_SOFTCHK_NO);
	}
}
