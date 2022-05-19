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
* Title       : Parameter ParaDeptNoMenu Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARADept.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaDeptNoMenuRead()  : reads Dept Number
*               ParaDeptNoMenuWrite() : sets Dept Number
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
*          :           :                                    
** GenPOS **
* Feb-13-18: 02.02.02 : R.Chambers   : added guards to protect array bounds.
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
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>


/**
;=============================================================================
;    Synopsis:  VOID ParaDeptNoMenuRead( PARADEPTNOMENU *pData )
;       input:  pData->uchMenuPageNumber
;               pData->uchMinorFSCData
;      output:  pData->usDeptNumber
;      
;      Return:  Nothing
;
; Descruption:  This function reads Dept Number from the specified address offset calculated
;               with a menu selection or menu page.
;
;               The purpose of this function is to allow the menu selection or menu page
;               functionality of NHPOS keyboard type systems. The menu selection functionality
;               is a kind of virtual keyboard that allows multiple different keyboard
;               key function layouts to be overlaid on the single physical keyboard.
;
;               The menu selection functionality is not used with touchscreen type
;               interfaces. See the comments at CVTTBL FARCONST CvtRegTouch[] in
;               file cvtregtouch.c.
;===============================================================================
**/

VOID ParaDeptNoMenuRead( PARADEPTNOMENU *pData )
{
    USHORT i;

    /* "-1" fits program address to RAM address */
    i = (( USHORT)(pData->uchMenuPageNumber - 1) * FSC_DEPT_MAX + ( USHORT)( pData->uchMinorFSCData) - 1);  

	if (i < MAX_DEPTKEY_SIZE) {
		pData->usDeptNumber = ParaDeptNoMenu[i];
	} else {
		NHPOS_ASSERT (i < MAX_DEPTKEY_SIZE);
		pData->usDeptNumber = 0;
	}
}

/**
;=============================================================================
;  
;    Synopsis:  VOID ParaDeptNoMenuWrite( PARADEPTNOMENU *pData )
;       input:  pData->uchMenuPageNumber
;               pData->uchMinorFSCData
;               pData->usDeptNumber
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets Dept Number in the specified address offset calculated
;               with a menu selection or menu page.
;
;               The purpose of this function is to allow the menu selection or menu page
;               functionality of NHPOS keyboard type systems. The menu selection functionality
;               is a kind of virtual keyboard that allows multiple different keyboard
;               key function layouts to be overlaid on the single physical keyboard.
;
;               The menu selection functionality is not used with touchscreen type
;               interfaces. See the comments at CVTTBL FARCONST CvtRegTouch[] in
;               file cvtregtouch.c.
;===============================================================================
**/

VOID ParaDeptNoMenuWrite( PARADEPTNOMENU *pData )   
{
    USHORT i;

    /* "-1" fits program address to RAM address */
    i = (( USHORT)(pData->uchMenuPageNumber - 1) * FSC_DEPT_MAX + ( USHORT)( pData->uchMinorFSCData) - 1);  

	if (i < MAX_DEPTKEY_SIZE) {
		ParaDeptNoMenu[i] = pData->usDeptNumber;
	} else {
		NHPOS_ASSERT (i < MAX_DEPTKEY_SIZE);
	}
}

