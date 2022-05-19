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
* Title       : Parameter ParaPLUNoMenu Module
* Category    : User Interface For Supervisor, NCR 2170 System Application
* Program Name: PARAPLU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               ParaPLUNoMenuRead()       : reads PLU Number
*               ParaPLUNoMenuWrite()      : sets PLU Number
*               ParaPLUNoMenuWriteTouch() : write PLU number into Touchscreen circular buffer
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda      : initial
* Nov-30-92:  01.00.03 : K.You        : Chg from "pararam.h" to <pararam.h>
* Jun-23-93:  01.00.04 : O.Nakada     : Initial for GP Model
* Jul-08-93:  01.00.05 : T.Koike      : Modify Read/Write Function                 
*                                       (Change Structure Format)
* Apr-07-94 :           : hkato       : Change uniniram
* Apr-24-15 : 02..02.01 : R.Chambers  : source cleanup, use PLU_MAX_TOUCH_PRESET
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
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaPLUNoMenuRead( PARAPLUNOMENU *pData )
;       input:  pData->uchMenuPageNumber
;               pData->uchMinorFSCData
;      output:  pData->uchAdjective
;               pData->uchModStat
;               pData->aucPLUNumber
;      
;      Return:  Nothing
;
; Description:  This function reads PLU Number of appointed address is used with
;               older keyboard functionality and Action Code 4 in Supervisor Mode
;               in order to read PLU assigned to a given keyboard FSCs.
;===============================================================================
**/
VOID ParaPLUNoMenuRead( PARAPLUNOMENU *pData )
{
    USHORT i;

	if(pData->uchMenuPageNumber >= STD_TOUCH_MENU_PAGE)
	{
		pData->uchMenuPageNumber = 1;
	}
    /* "-1" fits program address to RAM address */
    i = (( USHORT)(pData->uchMenuPageNumber - 1) * FSC_PLU_MAX + ( USHORT)( pData->uchMinorFSCData) - 1);
    
    pData->PluNo = ParaPLUNoMenu[i];
}

/**
;=============================================================================
;  
;    Synopsis:  VOID ParaPLUNoMenuWrite( PARAPLUNOMENU *pData )
;       input:  pData->uchMenuPageNumber
;               pData->uchMinorFSCData
;               pData->uchAdjective
;               pData->uchModStat
;               pData->aucPLUNumber
;      output:  Nothing
;
;      Return:  Nothing
;
; Description:  This function sets PLU Number in appointed address is used with
;               older keyboard functionality and Action Code 4 in Supervisor Mode
;               in order to make PLU assignment to keyboard FSCs.
;               
;===============================================================================
**/
VOID ParaPLUNoMenuWrite( PARAPLUNOMENU *pData ) 
{
    USHORT i;

	if(pData->uchMenuPageNumber >= STD_TOUCH_MENU_PAGE)
	{
		pData->uchMenuPageNumber = 1;
	}
    /* "-1" fits program address to RAM address */
    i = (( USHORT)(pData->uchMenuPageNumber - 1) * FSC_PLU_MAX + ( USHORT)( pData->uchMinorFSCData) - 1);

    ParaPLUNoMenu[i] = pData->PluNo;
}
/**
;=============================================================================
;  
;    Synopsis:  VOID ParaPLUNoMenuWrite( PARAPLUNOMENU *pData )
;       input:  pData->uchMenuPageNumber
;               pData->uchMinorFSCData
;               pData->uchAdjective
;               pData->uchModStat
;               pData->aucPLUNumber
;      output:  Nothing
;
;      Return:  UCHAR  address in ParaPLUNoMenu[] modified. 
;
; Description:  This function sets PLU Number in appointed address.
;               This function is used by function BlFwIfSetPresetPLUNumber()
;               which in turn handles Touchscreen button presses where the
;               button information includes a PLU number.
;
;               See also function HandlePluButtonPress() in FrameworkWndButton.cpp.
;
;               This function is used to create a temporary PLU preset in
;               what amounts to a slot in a circular buffer so that a message
;               with this preset location can be inserted into the UI ring
;               buffer allowing the Touchscreen button PLU to be processed
;               as if the button specified a preset PLU instead of a PLU number.
;               This was done as part of Touchscreen changes when creating the
;               Facade to transform button presses into key presses on a keyboard.
;               
;===============================================================================
**/
UCHAR ParaPLUNoMenuWriteTouch( PARAPLUNOMENU *pData ) 
{
	static   UCHAR i = 1, sRet = 0;

	if(i > PLU_MAX_TOUCH_PRESET || i < 1)
	{
		i = 1;
	}
		
    /* "-1" fits program address to RAM address */
	ParaPLUNoMenu[i-1] = pData->PluNo;

	sRet = i;
	i++;
	return sRet;
}
/* ---------- End of PARAPLU.C ---------- */
