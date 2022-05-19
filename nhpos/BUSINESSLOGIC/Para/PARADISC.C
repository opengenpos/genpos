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
* Title       : Parameter ParaDisc Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARADISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaDiscRead()  : reads DISCOUNT/BONUS RATE
*               ParaDiscWrite() : sets DISCOUNT/BONUS RATE
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
;    Synopsis:  VOID ParaDiscRead( PARADISCTBL *pData )
;       input:  pData->uchAddress
;      output:  pData->uchDiscRate
;
;      Return:  Nothing
;
; Descruption:  This function reads DISCOUNT/BONUS RATE.
;===============================================================================
**/

VOID ParaDiscRead( PARADISCTBL *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);    /* "-1" fits program address to RAM address */  

    pData->uchDiscRate = ParaDisc[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaDiscWrite( PARADISCTBL *pData )
;       input:  pData->uchAddress
;               pData->uchDiscRate           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets DISCOUNT/BONUS RATE.
;===============================================================================
**/

VOID ParaDiscWrite( PARADISCTBL *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    ParaDisc[i] = pData->uchDiscRate;

}


