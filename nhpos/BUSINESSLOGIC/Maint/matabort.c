/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintABORT Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATABORT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls "ABORT" in super/program mode.
*
*           The provided function names are as follows: 
*                   
*                   MaintABORT()   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-11-92: 00.00.01 : J.Ikeda   : initial                                   
* Dec-28-99: 01.00.00 : hrkato    : Saratoga
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
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include "maintram.h"


/*
*===========================================================================
** Synopsis:    VOID MaintABORT( VOID )
*               
*     Input:    uchSupPrgMode
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function controls ABORT in super/program mode.
*===========================================================================
*/

VOID MaintABORT(VOID)
{

    /* check operational times */

    if (uchMaintOpeCo & MAINT_ALREADYOPERATE) {                 /* print header */
        MaintMakeAbortKey();                                    
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    }

    /* clear self-work */

    memset(&MaintWork, 0, sizeof(MaintWork));
    uchMaintOpeCo = MAINT_1STOPERATION;
    usMaintLockHnd = 0;
    usMaintACNumber = 0;                                /* Saratoga */
}

/* --- End of Source File --- */