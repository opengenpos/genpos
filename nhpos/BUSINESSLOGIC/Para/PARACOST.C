/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaCost Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARACOST.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaLaborCostRead()  : reads Labor Cost Value
*               ParaLaborCostWrite() : sets Labor Cost Value
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Dec-20-95:  03.01.00 : M.Ozawa  : initial                                   
** GenPOS
* Jul-17-17:  02.02.02 : R.Chambers  : test max address, remove unneeded include file.
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

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaLaborCostRead( PARALABORCOST *pData )
;       input:  pData->uchAddress
;      output:  pData->usLaborCost
;
;      Return:  Nothing
;
; Descruption:  This function reads Labor Cost Value.
;===============================================================================
**/
VOID ParaLaborCostRead( PARALABORCOST *pData )
{
    USHORT    i = (pData->uchAddress - 1);   /* "-1" fits program address to RAM address */  

	pData->usLaborCost = 0;
	if (i < MAX_LABORCOST_SIZE)              // ParaLaborCostRead() read a labor cost value
		pData->usLaborCost = ParaLaborCost[i];
}

/**
;=============================================================================
;    Synopsis:  VOID ParaLaborCostWrite( PARALABORCOST *pData )
;       input:  pData->uchAddress
;               pData->usLaborCost           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets Labor Cost Value.
;===============================================================================
**/
VOID ParaLaborCostWrite( PARALABORCOST *pData )
{
    USHORT    i = (pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if (i < MAX_LABORCOST_SIZE)              // ParaLaborCostWrite() write a labor cost value
		ParaLaborCost[i] = pData->usLaborCost;
}
