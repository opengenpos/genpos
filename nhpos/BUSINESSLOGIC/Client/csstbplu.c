/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Enhanced EPT Source File
* Category    : Client/Server STUB, NCR 2170 GP 2.0 Application
* Program Name: CSSTBEEPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*           CliEEptSendMessage()         * Send request to Host
*           CliEEptReceiveMessage()      * Receive from Host
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :   Name    : Description
* Nov-19-96:Y.shozy    : initial R2.0
* Dec-07-99:hrkato     : Saratoga
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
#include	<tchar.h>
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csstbstb.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <eept.h>
#include    <csstbept.h>
#include    "csstubin.h"
#include	<csttl.h>
#include	<csstbttl.h>
/*
*===========================================================================
** Synopsis:    SHORT   CliRptPLURead(TTLPLUEX *myTotal)
*
*     Input:    *pSendData    - Request Data
*               usSendLen     - Request data length
*    OutPut:    Nothing
*
** Return:      EEPT_RET_SUCCESS:    OK
*   Abnormal    EEPT_RET_PORT_NONE:
*
** Description:  This function supports to request data to PLUs.
*===========================================================================
*/
SHORT   CliRptPLURead(TTLPLUEX *myTotal)
{
	myTotal->uchMajorClass = CLASS_TTLPLU;

	SerTtlTotalReadPluEx(myTotal);

    return(SUCCESS);
	//SHORT    STUBENTRY SerTtlTotalReadPluEx(VOID FAR *pTotal);
}

/*===== END OF SOURCE =====*/