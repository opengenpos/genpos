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
* Title       : Parameter ParaTtlKeyCtl Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAKCTL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaTtlKeyCtlRead()  : reads bit of TOTAL KEY
*               ParaTtlKeyCtlWrite() : sets bit of TOTAL KEY
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
;    Synopsis:  VOID ParaTtlKeyCtlRead( PARATTLKEYCTL *pData )
;       input:  pData->uchAddress
;               pData->uchField
;      output:  pData->uchTtlKeyMDCData
;
;      Return:  Nothing
;
; Descruption:  This function reads total status bits of TOTAL KEY.
;
;               This data is the same as in ItemTotal.auchTotalStatus array elements
;               1 through 6. auchTotalStatus[0] is the total key type and auchTotalStatus[1],
;               auchTotalStatus[2], etc. is a copy of the six elements from ParaTtlKeyCtl.
;
;               Use defined constants TLCT_NO3_ADR, TLCT_NO4_ADR, etc. in paraequ.h for address.
;===============================================================================
**/

VOID ParaTtlKeyCtlRead( PARATTLKEYCTL *pData )
{
    UCHAR    i;
    UCHAR    j;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     
    
    j = pData->uchField;

    /* calculate for fitting program address to ram address */
    
    j = ( UCHAR)((j - 1) / 2);

    pData->uchTtlKeyMDCData = ParaTtlKeyCtl[i][j];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaTtlKeyCtlWrite( PARATTLKEYCTL *pData )
;       input:  pData->uchAddress
;               pData->uchField
;               pData->uchTtlKeyMDCData
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets bit of TOTAL KEY.
;===============================================================================
**/

VOID ParaTtlKeyCtlWrite( PARATTLKEYCTL *pData )
{
    UCHAR    i;                   
    UCHAR    j;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */               

    j = pData->uchField;

    /* calculate for fitting program address to ram address */
    
    j = ( UCHAR)((j - 1) / 2);

    ParaTtlKeyCtl[i][j] = pData->uchTtlKeyMDCData;

}

