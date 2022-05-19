/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Waiter Number Assignment Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC50.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC50Function()   : A/C No.50 Function Entry Mode
*               UifAC50EnterData1() : A/C No.50 Enter Data1 Mode
*               UifAC50EnterALPHA() : A/C No.50 Enter Waiter Name Mode
*               UifAC50EnterData2() : A/C No.50 Enter Data2 Mode
*               UifAC50EnterData3() : A/C No.50 Enter Data3 Mode
*               UifAC50EnterData4() : A/C No.50 Enter Data4 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-03-92:00.00.01    :K.You      : initial                                   
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
* Aug-27-98:03.03.00    :hrkato     : V3.3
* Aug-11-99:03.05.00    :K.Iwata    : R3.5 (remove WAITER_MODEL)
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
#include <pif.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

SHORT   UifAC50Function(KEYMSG *pKeyMsg)
{
    return(0);
    pKeyMsg = pKeyMsg;
}
SHORT   UifAC50EnterData1(KEYMSG *pKeyMsg)
{
    return(0);
    pKeyMsg = pKeyMsg;
}
SHORT   UifAC50EnterALPHA(KEYMSG *pKeyMsg)
{
    return(0);
    pKeyMsg = pKeyMsg;
}
SHORT   UifAC50EnterData2(KEYMSG *pKeyMsg)
{
    return(0);
    pKeyMsg = pKeyMsg;
}
SHORT   UifAC50EnterData3(KEYMSG *pKeyMsg)
{
    return(0);
    pKeyMsg = pKeyMsg;
}
SHORT   UifAC50EnterData4(KEYMSG *pKeyMsg)
{
    return(0);
    pKeyMsg = pKeyMsg;
}


/* --- End of Source --- */
