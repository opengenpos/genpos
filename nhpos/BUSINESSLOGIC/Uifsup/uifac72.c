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
* Title       : PLU Parameter File Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC72.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC72Function()       : A/C No.72 Function Entry Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-29-92:00.00.01    :K.You      : initial                                   
* Jul-20-93:01.00.12    :K.You      : fixed bug                                    
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */
/*
*=============================================================================
**  Synopsis: SHORT UifAC72Function( VOID ) 
*
*       Input:  nothing          
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               
**  Description: A/C No.72 Function Entry Mode  
*===============================================================================
*/





SHORT UifAC72Function( VOID ) 
{


    MaintDisp(AC_PLUPARA_READ_RPT,              /* A/C Number */
              0,                                /* Descriptor */
              0,                                /* Page Number */
              0,                                /* PTD Type */
              0,                                /* Report Type */
              0,                                /* Reset Type */
              0L,                               /* Amount Data */
              0);                               /* Lead Through Address */

    return(MaintPLUReport());                   /* Execute PLU Parameter Read Report */ 

}

/****** End of Source ******/

