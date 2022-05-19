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
* Title       : Parameter ParaFlexMem Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAFLEX.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*             ParaFlexMemRead()  : reads the number of RECORD and PTD FLAG STATUS
*             ParaFlexMemWrite() : sets the number of RECORD and PTD FLAG STATUS
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
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>


/**
;=============================================================================
;    Synopsis:  VOID ParaFlexMemRead( PARAFLEXMEM *pData )
;       input:  pData->uchAddress
;      output:  pData->usRecordNumber
;               pData->uchPTDFlag
;      Return:  Nothing
;
; Descruption:  This function reads the number of RECORD and PTD FLAG STATUS.
;===============================================================================
**/
static  struct {
	ULONG   ulRecordNumber;
	UCHAR   uchPTDFlag;
} ParaPseudoFlexSettings [] = {
	{400, 0}
	};
VOID ParaFlexMemRead( PARAFLEXMEM *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if (i < FLEX_MLD_ADR) {
		pData->ulRecordNumber = ParaFlexMem[i].ulRecordNumber;
		pData->uchPTDFlag = ParaFlexMem[i].uchPTDFlag;
	} else {
		i -= FLEX_MLD_ADR;
		pData->ulRecordNumber = ParaPseudoFlexSettings[i].ulRecordNumber;
		pData->uchPTDFlag = ParaPseudoFlexSettings[i].uchPTDFlag;
	}
}

/**
;=============================================================================
;    Synopsis:  VOID ParaFlexMemWrite( PARAFLEXMEM *pData )
;       input:  pData->uchAddress
;               pData->usRecordNumber
;               pData->uchPTDFlag
;      output:  Nothing
;      Return:  Nothing
;
; Descruption:  This function sets the number of RECORD and PTD FLAG STATUS.
;===============================================================================
**/

VOID ParaFlexMemWrite( PARAFLEXMEM *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if (i < FLEX_MLD_ADR) {
		ParaFlexMem[i].ulRecordNumber = pData->ulRecordNumber;
		ParaFlexMem[i].uchPTDFlag = pData->uchPTDFlag;
	}
}


