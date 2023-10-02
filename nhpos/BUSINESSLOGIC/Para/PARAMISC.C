/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1994            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaMiscPara Module 
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAMISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaMiscRead()  : reads Misc. Parameter
*               ParaMiscWrite() : sets Misc. Parameter
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Mar-10-93:  00.00.01 : M.Ozawa  : initial 
* Apr-7-94 :           : hkato    : Change uniniram
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
#include "ecr.h"
#include "plu.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"

/**
;=============================================================================
;	 Synopsis:  VOID ParaMiscParaRead( PARAMISCPARA *pData )
;       input:  pData->uchAddress
;	   output:  pData->ulMiscPara
;
;      Return:  Nothing
;
; Description:  This function reads Misc. Parameter.
				The address is a one based offset being in the range
				of 1 .. MAX_MISC_SIZE and we subtract one to translate
				the address to the array element offset which ranges
				from 0 .. MAX_MISC_SIZE-1 (zero based offset).

				We are extending the Misc. Parameters into another,
				non-contiguous memory area which will be used to store
				limits for Store and Forward.
;===============================================================================
**/

VOID ParaMiscParaRead( PARAMISCPARA *pData )
{
	UCHAR	 i = (UCHAR)(pData->uchAddress - 1);

	NHPOS_ASSERT(i < MAX_MISC_SIZE * 2);
	if (i < MAX_MISC_SIZE)
		pData->ulMiscPara = ParaMiscPara[i];
	else {
		i -= MAX_MISC_SIZE;
		if (i < MAX_MISC_SIZE)
			pData->ulMiscPara = Para.ParaStoreForward[i];
	}
}

VOID ParaStoreForwardParaRead( PARAMISCPARA *pData )
{
	UCHAR	 i = (UCHAR)(pData->uchAddress - 1);

	NHPOS_ASSERT(i < MAX_MISC_SIZE);
	pData->ulMiscPara = Para.ParaStoreForward[i];
}

/**
;=============================================================================
;	 Synopsis:  VOID ParaMiscParaWrite( PARAMISCPARA *pData )
;       input:  pData->uchAddress
;               pData->ulMiscPara           
;	   output:	Nothing   
;
;      Return:	Nothing
;
; Descruption:  This function sets Misc. Parameter.

				We are extending the Misc. Parameters into another,
				non-contiguous memory area which will be used to store
				limits for Store and Forward.
;===============================================================================
**/

VOID ParaMiscParaWrite( PARAMISCPARA *pData )
{
	UCHAR 	 i= (UCHAR)(pData->uchAddress - 1);

	NHPOS_ASSERT(i < MAX_MISC_SIZE * 2);
	if (i < MAX_MISC_SIZE)
		ParaMiscPara[i] = pData->ulMiscPara;
	else {
		i -= MAX_MISC_SIZE;
		if (i < MAX_MISC_SIZE)
			Para.ParaStoreForward[i] = pData->ulMiscPara;
	}
}

VOID ParaStoreForwardParaWrite( PARAMISCPARA *pData )
{
	UCHAR 	 i= (UCHAR)(pData->uchAddress - 1);

	NHPOS_ASSERT(i < MAX_MISC_SIZE);
	Para.ParaStoreForward[i] = pData->ulMiscPara;
}


//============   Per Terminal data file interfaces

VOID ParaTerminalInfoParaRead (PARATERMINALINFO *pData)
{
	UCHAR 	 i= (UCHAR)(pData->uchAddress - 1);

	if (i >= MAX_TERMINALINFO_SIZE) {
		i = 0;
	}
	pData->TerminalInfo = ParaTerminalInformationPara[i];
}


VOID ParaTerminalInfoParaWrite (PARATERMINALINFO *pData)
{
	UCHAR 	 i= (UCHAR)(pData->uchAddress - 1);

	NHPOS_ASSERT_TEXT(i < MAX_TERMINALINFO_SIZE,"ParaTerminalInfoParaWrite(): write ignored.");
	if (i < MAX_TERMINALINFO_SIZE) {
		ParaTerminalInformationPara[i] = pData->TerminalInfo;
	}
}

VOID  ParaReasonCodeInfoParaRead (PARAREASONCODEINFO  *pData)
{
	UCHAR 	 i= (UCHAR)(pData->uchAddress - 1);

	if (i < MAX_REASON_CODE_ITEMS) {
		pData->usMnemonicStartAddr = Para.ParaReasonCode[i][0];
		pData->usMnemonicCount = Para.ParaReasonCode[i][1];
	}
}

VOID  ParaReasonCodeInfoParaWrite (PARAREASONCODEINFO  *pData)
{
	UCHAR 	 i= (UCHAR)(pData->uchAddress - 1);

	if (i < MAX_REASON_CODE_ITEMS) {
		Para.ParaReasonCode[i][0] = pData->usMnemonicStartAddr;
		Para.ParaReasonCode[i][1] = pData->usMnemonicCount;
	}
}
/* --- end of Source File --- */