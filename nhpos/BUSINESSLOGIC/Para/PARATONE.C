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
* Title       : Parameter ParaToneCtl Module                         
* Category    : User I/F For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: PARATONE.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaToneCtlRead()  : reads TONE CONTROL DATA
*               ParaToneCtlWrite() : sets TONE CONTROL DATA
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-23-92:  00.00.01 : K.you    : initial                                   
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
*==============================================================================
**   Synopsis:  VOID ParaToneCtlRead( PARATONECTL *pData )
*               
*     Input:  Nothing
*    Output:  Nothing
*     InOut:  *pData          : pointer to structure for PARATONECTL
* 
** Return:    Nothing
*
** Descruption:  This function reads Tone Control Data.
*================================================================================
*/

VOID ParaToneCtlRead( PARATONECTL *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */             

    pData->uchToneCtl = ParaToneCtl[i];
}

/**
*==============================================================================
**   Synopsis:  VOID ParaToneCtlRead( PARATONECTL *pData )
*               
*     Input:  *pData          : pointer to structure for PARATONECTL  
*    Output:  Nothing
*     InOut:  Nothing         
* 
** Return:    Nothing
*
** Descruption:  This function Writes Tone Control Data.
*================================================================================
*/

VOID ParaToneCtlWrite( PARATONECTL *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */             

	if( i < MAX_TONE_SIZE ) {   // out of bounds guard, ParaToneCtlWrite( PARATONECTL *pData )
		ParaToneCtl[i] = pData->uchToneCtl;
	}
}
