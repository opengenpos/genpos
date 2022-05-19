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
* Title       : Parameter ParaReportName Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAREPO.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaReportNameRead()  : reads REPORT NAME
*               ParaReportNameWrite() : sets REPORT NAME
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaReportNameRead( PARAREPORTNAME *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_REPORTNAME_LEN+1]
;
;      Return:  Nothing
;
; Description:  This function reads REPORT NAME from the memory resident
;               database. We ensure the mnemonic copied is zero terminated and the
;               receiving buffer must be sized to PARA_REPORTNAME_LEN + 1 characters
;               in length.
;===============================================================================
**/
VOID ParaReportNameRead( PARAREPORTNAME *pData )
{
    USHORT    i = pData->uchAddress - 1;               /* "-1" fits program address to RAM address */           

    pData->aszMnemonics[0] = '\0';                     // initialize destination to an empty string
	if (i < MAX_REPO_NO) {
		_tcsncpy(pData->aszMnemonics, ParaReportName[i], PARA_REPORTNAME_LEN); /* ### Mod (2171 for Win32) V1.0 */
	} else {
		NHPOS_ASSERT(i < MAX_REPO_NO);
	}

    pData->aszMnemonics[PARA_REPORTNAME_LEN] = '\0';   // ensure zero terminated string with max count of characters.
}

/**
;=============================================================================
;    Synopsis:  VOID ParaReportNameWrite( PARAREPORTNAME *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_REPORTNAME_LEN+1]
;      output:  Nothing
;      Return:  Nothing
;
; Description:  This function sets REPORT NAME into the memory resident
;               database. We copy a maximum of PARA_REPORTNAME_LEN characters from the
;               argument into the memory resident database.
;
;               If the mnemonic is the full character count then it is not stored
;               zero terminated and reading from the memory resident database must
;               accommodate this possibility.
;===============================================================================
**/
VOID ParaReportNameWrite( PARAREPORTNAME *pData )
{
    USHORT    i = pData->uchAddress - 1;                /* "-1" fits program address to RAM address */           

	if (i < MAX_REPO_NO) {
		_tcsncpy(ParaReportName[i], pData->aszMnemonics, PARA_REPORTNAME_LEN);
	} else {
		NHPOS_ASSERT(i < MAX_REPO_NO);
	}
}
