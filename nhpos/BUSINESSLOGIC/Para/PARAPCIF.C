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
* Title       : ParaPCI/F Module                         
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: PARAPCIF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write password.
*
*           The provided function names are as follows: 
* 
*              ParaPCIFRead()   : read PC password
*              ParaPCIFWrite()  : set PC password 
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : J.Ikeda   : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
* 2171 for Win32
* Aug-26-99:  01.00.00 : K.Iwata  : V1.0 Initial
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
=============================================================================
**/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h> 
#include <para.h>
#include <pararam.h>
#include <rfl.h>

/*
*===========================================================================
** Synopsis:    VOID ParaPCIfRead( PARAPCIF *pData )
*               
*     Input:    *pData            : pointer to structure for PARAPCIF
*                  (pData->uchAddress)
*    Output:    pData->aszMnemonics[PARA_PCIF_LEN+1] 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reads password in PC I/F.
*===========================================================================
*/

VOID ParaPCIFRead( PARAPCIF *pData )
{

    UCHAR  i;

    i = ( UCHAR)(pData->uchAddress - 1);    /* "-1" fits program address to RAM address */
    memset(pData->aszMnemonics, '\0', PARA_PCIF_LEN+1);

    _tcsncpy((TCHAR *)&pData->aszMnemonics[0], &ParaPCIF[0], PARA_PASSWORD_LEN);

}

/*
*===========================================================================
** Synopsis:    VOID ParaPCIFWrite( PARAPCIF *pData )
*               
*     Input:    *pData            : pointer to structure for PARAPCIF
*                  (pData->uchAddress)
*                  (pData->aszMnemonics[PARA_PCIF_LEN+1])
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets password in PC I/F.
*===========================================================================
*/

VOID ParaPCIFWrite( PARAPCIF *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

    _tcsncpy(&ParaPCIF[0], ( TCHAR *)&pData->aszMnemonics[0], PARA_PASSWORD_LEN);

}
