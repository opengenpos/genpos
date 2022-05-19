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
* Title       : Parameter ParaTtlKeyTyp Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAKTYP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaTtlKeyTypRead()  : reads characteristic type of total key
*               ParaTtlKeyTypWrite() : sets characteristic type of total key
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
;    Synopsis:  VOID ParaTtlKeyTypRead( PARATTLKEYTYP *pData )
;       input:  pData->uchAddress
;      output:  pData->uchTypeIdx
;
;      Return:  Nothing
;
; Descruption:  This function reads characteristic type of total key.
// Total Key Type information is stored in Para.ParaTtlKeyTyp  read CLASS_PARATTLKEYTYP
// Currently for 20 total keys, there are only 17 addresses because
// Totals 1,2,9 have a "specified type" already
// SubTotal(1)    Check Total(2)       Food Stamp Total (9)
// To get the Total key address from the relative total key we use the following table
// ParaTtlKeyTyp Address::	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16
// Relative Total Key::		3	4	5	6	7	8	10	11	12	13	14	15	16	17	18	19	20
//
// use defined constants TLTY_NO3_ADR, TLTY_NO4_ADR, etc. in paraequ.h for address
;===============================================================================
**/

VOID ParaTtlKeyTypRead( PARATTLKEYTYP *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */         

    pData->uchTypeIdx = ParaTtlKeyTyp[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaTtlKeyTypWrite( PARATTLKEYTYP *pData )
;       input:  pData->uchAddress
;               pData->uchTypeIdx
;      output:  Nothing
;      Return:  Nothing
;
; Descruption:  This function sets characteristic type of total key.
;===============================================================================
**/

VOID ParaTtlKeyTypWrite( PARATTLKEYTYP *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */         

    ParaTtlKeyTyp[i] = pData->uchTypeIdx;

}

