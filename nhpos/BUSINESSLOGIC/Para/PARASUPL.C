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
* Title       : Parameter ParaSupLevel Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASUPL.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSupLevelRead()  : reads SUPERVISOR Number/LEVEL
*               ParaSupLevelWrite() : sets SUPERVISOR Number/LEVEL
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
;    Synopsis:  SHORT ParaSupLevelRead( PARASUPLEVEL *pData )
;       input:  pData->uchAddress
;               pData->usSuperNumber
;      output:  pData->usSuperNumber
;               pData->uchSecret
;               pData->uchAddress
;      Return:  SUCCESS         : successful
;               LDT_NTINFL_ADR  : error
;
; Descruption:  This function reads SUPERVISOR Number/LEVEL.
;===============================================================================
**/

SHORT ParaSupLevelRead( PARASUPLEVEL *pData )
{
    UCHAR    i;
    UCHAR    j;

    /* search supervisor number */
    if (pData->usSuperNumber != 0) {

        /* search address */
        for (j = SPNO_LEV1_ADR; j < SPNO_ADR_MAX; j++) {
            
            /* compare supervisor number */
            if ( pData->usSuperNumber == ParaSupLevel[j].usSuperNumber ) {
                pData->uchAddress = ( UCHAR)( j + 1 );    /* "+1" fits ram adress to program address */
                break;
			}
        }

		if (j >= SPNO_ADR_MAX) {
			return(LDT_NTINFL_ADR);         
		}
    }

	// pData->uchAddress may change in the loop above.
    i = ( UCHAR)(pData->uchAddress - 1);

    pData->usSuperNumber = ParaSupLevel[i].usSuperNumber;
    pData->uchSecret = ParaSupLevel[i].uchSecret;

    return(SUCCESS);        /* read data */
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSupLevelWrite( PARASUPLEVEL *pData )
;       input:  pData->uchAddress
;               pData->usSuperNumber
;               pData->uchSecret
;      output:  Nothing
;      Return:  Nothing
;
; Descruption:  This function sets SUPERVISOR Number/LEVEL.
;===============================================================================
**/

VOID ParaSupLevelWrite( PARASUPLEVEL *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if( i < MAX_SUPLEVEL_SIZE ) {   // out of bounds guard, ParaSupLevelWrite( PARASUPLEVEL *pData )
		ParaSupLevel[i].usSuperNumber = pData->usSuperNumber;
		ParaSupLevel[i].uchSecret = pData->uchSecret;
	}
}
