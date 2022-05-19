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
* Title       : MaintReDisp Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 Application Program        
* Program Name: MATRDISP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*              VOID MaintReDisp( VOID );    
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-15-92: 00.00.01 :  K.You    : initial                                   
* Dec-17-92: 01.00.00 :  K.You    : Move 0L to flDispRegDescrControl in default case                        
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
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <display.h>

#include "maintram.h"

/*
*===========================================================================
** Synopsis:    VOID MaintReDisp( MAINTREDISP *pData )
*               
*     Input:    *pData          : pointer to structure for MAINTREDISP
*    Output:    Nothing 
*
** Return:      Nothing
*               
*
** Description: Control Descriptor and Redisplay Old Data. 
*===========================================================================
*/

VOID MaintReDisp(MAINTREDISP *pData)
{


    /* Set Descriptor Data */

    switch (pData->uchMinorClass) {
    case CLASS_MAINTREDISP_VOID:
        flDispRegDescrControl |= VOID_CNTRL;
        break;

    case CLASS_MAINTREDISP_TRAN:
        flDispRegDescrControl |= TRAIN_CNTRL;
        break;

    case CLASS_MAINTREDISP_ALPHA:
        flDispRegDescrControl |= CRED_CNTRL;
        break;

    case CLASS_MAINTREDISP_CLRWOALPH:
        flDispRegDescrControl &= ~VOID_CNTRL & ~TRAIN_CNTRL;
        break;

    case CLASS_MAINTREDISP_NORECEIPT:
        flDispRegDescrControl |= RECEIPT_CNTRL;
        flDispRegKeepControl |= RECEIPT_CNTRL; 
        break;

    case CLASS_MAINTREDISP_RECEIPT:
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
        flDispRegKeepControl &= ~RECEIPT_CNTRL; 
        break;

    case CLASS_MAINTREDISP_PAGEDISP:
        break;

    default:                                    /* CLASS_MAINTREDISP_CLR */
		if(!(flDispRegDescrControl & PM_CNTRL))	//Delayed Balance JHHJ
		{
			flDispRegDescrControl = 0L;
		}else
		{
			flDispRegDescrControl = 0;
			flDispRegDescrControl |= PM_CNTRL;
		}
		if( !(flDispRegKeepControl & PM_CNTRL))
		{
			flDispRegKeepControl = 0L;
		}else
		{
			flDispRegDescrControl = 0;
			flDispRegDescrControl |= PM_CNTRL;
		}
        break;
    }

    /* Control Descriptor and Redisplay */

    DispWrite(pData);
}
