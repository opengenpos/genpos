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
* Title       : Parameter ParaStoRegNo Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAREGN.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaStoRegNoRead()  : reads STORE/REGISTER Number
*               ParaStoRegNoWrite() : sets STORE/REGISTER Number
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
;=============================================================================
**/

#include	<tchar.h>
#include	<stdio.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>


/**
;=============================================================================
;    Synopsis:  VOID ParaStoRegNoRead( PARASTOREGNO *pData )
;       input:  Nothing
;      output:  pData->usStoreNo
;               pData->usRegisterNo
;
;      Return:  Nothing
;
; Descruption:  This function reads STORE/REGISTER Number.
;===============================================================================
**/

VOID ParaStoRegNoRead( PARASTOREGNO *pData )
{
    pData->usStoreNo = ParaStoRegNo[0];         /* 0 = Store No. RAM address */
    pData->usRegisterNo = ParaStoRegNo[1];      /* 1 = Register No. RAM address */
}

/**
;=============================================================================
;    Synopsis:  VOID ParaStoRegNoWrite( PARASTOREGNO *pData )
;       input:  pData->usStoreNo
;               pData->usRegisterNo
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets STORE/REGISTER Number.
;===============================================================================
**/

VOID ParaStoRegNoWrite( CONST PARASTOREGNO *pData )
{
	char       xBuff[128];
	USHORT     usTermNo = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);

	// guard the terminal position to ensure it is in correct range, default is we be Master
	if (!(usTermNo >= 1 && usTermNo <= PIF_NET_MAX_IP))
		usTermNo = 1;

	usTermNo--;    // Change from one based to zero based to access the C array

    ParaStoRegNo[0] = pData->usStoreNo;         /* 0 = StoreNo RAM address */
    ParaStoRegNo[1] = pData->usRegisterNo;      /* 1 = RegisterNo RAM address */

	if (usTermNo < MAX_TERMINALINFO_SIZE) {
		ParaTerminalInformationPara[usTermNo].usStoreNumber = pData->usStoreNo;
		ParaTerminalInformationPara[usTermNo].usRegisterNumber = pData->usRegisterNo;
	} else {
		NHPOS_ASSERT_TEXT((usTermNo < MAX_TERMINALINFO_SIZE), "==ERROR: ParaStoRegNoWrite() address out of bounds.");
	}

	sprintf (xBuff, "==NOTE: ParaStoRegNoWrite() usTermNo %d  usStoreNo %d  usRegisterNo %d", usTermNo, pData->usStoreNo, pData->usRegisterNo);
	NHPOS_NONASSERT_TEXT(xBuff);
}
