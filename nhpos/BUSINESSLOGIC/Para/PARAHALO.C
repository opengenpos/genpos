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
* Title       : Parameter ParaTransHALO Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAHALO.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaTransHALORead()  : reads TRANSACTION HALO
*               ParaTransHALOWrite() : sets TRANSACTION HALO DATA
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
;    Synopsis:  VOID ParaTransHALORead( PARATRANSHALO *pData )       
;       input:  pData->uchAddress
;      output:  pData->uchHALO
;
;      Return:  Nothing
;
; Descruption:  This function reads TRANSACTION HALO.
;===============================================================================
**/

VOID ParaTransHALORead( PARATRANSHALO *pData )
{
	if (pData->uchAddress > 0) {
		UCHAR    i;
		i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     
		pData->uchHALO = ParaTransHALO[i];
	} else {
		pData->uchHALO = 0;
	}
}

/**
;=============================================================================
;    Synopsis:  VOID ParaTransHALOWrite( PARATRANSHALO *pData )
;       input:  pData->uchAddress
;               pData->uchHALO           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets TRANSACTION HALO DATA.
;===============================================================================
**/

VOID ParaTransHALOWrite( PARATRANSHALO *pData )
{
	if (pData->uchAddress > 0) {
		UCHAR    i;
		i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     
		ParaTransHALO[i] = pData->uchHALO;
	}
}


