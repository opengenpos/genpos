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
* Title       : Parameter ParaBoundary Age Module 
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAAGE.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaAgeRead()  : reads Boundary Age Parameter
*               ParaAgeWrite() : sets Boundary Age Parameter
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* FEB-28-94:  00.00.01 : M.INOUE  : initial 
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
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>


/**
;=============================================================================
;    Synopsis:  VOID ParaBoundAgeRead( PARABOUNDAGE *pData )
;       input:  pData->uchAddress
;      output:  pData->uchAgePara
;
;      Return:  Nothing
;
; Descruption:  This function reads Boundary Age Parameter.
;===============================================================================
**/

VOID ParaBoundAgeRead( PARABOUNDAGE *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);      /* "-1" fits program address to RAM address */    

    pData->uchAgePara = Para.auchParaBoundAge[i];
}

/**
;=============================================================================
;    Synopsis:  VOID ParaBoundAgeWrite( PARABOUNDAGE *pData )
;       input:  pData->uchAddress
;               pData->uchAgePara           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets  Boundary Age Parameter.
;===============================================================================
**/

VOID ParaBoundAgeWrite( PARABOUNDAGE *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */  

    Para.auchParaBoundAge[i] = pData->uchAgePara;

}

/***** end of paraage.c ******/
