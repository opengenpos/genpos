/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaDispPara Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARADSPS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*             ParaDispParaRead()  : reads the dispenser parameter
*             ParaDispParaWrite() : sets the dispenser parameter
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Feb-02-96:  03.01.00 : M.Ozawa  : initial                                   
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
;    Synopsis:  VOID ParaDispParaRead( PARADISPPARA *pData )
;       input:  pData->uchAddress
;      output:  pData->usPluNumber
;               pData->uchAdjective
;      Return:  Nothing
;
; Descruption:  This function reads the dispenser parameter
;===============================================================================
**/

VOID ParaDispParaRead( PARADISPPARA *pData )
{

    USHORT  i;

    i = (pData->usAddress - 1);       /* "-1" fits program address to RAM address */      

    pData->usPluNumber = Para_DispPara[i].usPluNumber;

    pData->uchAdjective = Para_DispPara[i].uchAdjective;

}

/**
;=============================================================================
;    Synopsis:  VOID ParaDispParaWrite( PARADISPPARA *pData )
;       input:  pData->uchAddress
;               pData->usPluNumber
;               pData->uchAdjective
;      output:  Nothing
;      Return:  Nothing
;
; Descruption:  This function sets the dispenser parameter.
;===============================================================================
**/

VOID ParaDispParaWrite( PARADISPPARA *pData )
{

    USHORT  i;

    i = (pData->usAddress - 1);       /* "-1" fits program address to RAM address */      

    Para_DispPara[i].usPluNumber =pData->usPluNumber;
    
    Para_DispPara[i].uchAdjective = pData->uchAdjective;

}

/**** End of File ****/
