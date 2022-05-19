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
* Title       : Parameter ParaSecurityNo Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASECU.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSecurityNoRead()  : reads SECURITY Number
*               ParaSecurityNoWrite() : sets SECURITY Number
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
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaSecurityNoRead( PARASECURITYNO *pData )
;       input:  Nothing
;      output:  pData->usSecurityNumber
;
;      Return:  Nothing
;
; Descruption:  This function reads SECURITY NumbeSECURITY Numberr.
;===============================================================================
**/

VOID ParaSecurityNoRead( PARASECURITYNO *pData )
{
    pData->usSecurityNumber = ParaSecurityNo;
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSecurityNoWrite( PARASECURITYNO *pData )
;       input:  pData->usSecurityNumber           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets SECURITY Number.
;===============================================================================
**/

VOID ParaSecurityNoWrite( CONST PARASECURITYNO *pData )
{
    ParaSecurityNo = pData->usSecurityNumber;
}
