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
* Title       : Parameter ParaTare Module                         
* Category    : User I/F For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: PARATARE.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaTareRead()  : reads TARE
*               ParaTareWrite() : sets TARE
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev   :   Name    : Description
* Nov-24-92: 01.00.00  : J.Ikeda   : initial                                   
* Nov-30-92: 01.00.03  : K.You     : Chg from "pararam.h" to <pararam.h>                                   
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
**   Synopsis:  VOID ParaTareRead( PARATARE *pData )
*               
*     Input:  Nothing
*    Output:  Nothing
*     InOut:  *pData          : pointer to structure for PARATARE
* 
** Return:    Nothing
*
** Descruption:  This function reads TARE.
*================================================================================
*/

VOID ParaTareRead( PARATARE *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */             

    pData->usWeight = ParaTare[i];
}

/**
*==============================================================================
**   Synopsis:  VOID ParaTareWrite( PARATARE *pData )
*               
*     Input:  *pData          : pointer to structure for PARATARE  
*    Output:  Nothing
*     InOut:  Nothing         
* 
** Return:    Nothing
*
** Descruption:  This function sets TARE.
*================================================================================
*/

VOID ParaTareWrite( PARATARE *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */             

	if( i < MAX_TARE_SIZE ) {   // out of bounds guard, ParaTareWrite( PARATARE *pData )
		ParaTare[i] = pData->usWeight;
	}
}
