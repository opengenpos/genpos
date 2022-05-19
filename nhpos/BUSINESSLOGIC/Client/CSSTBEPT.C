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
#include    <stdio.h>
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
#include    "EEPTL.H"

#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"


SHORT CliStoreForwardRead(LONG lUieAc102_SubMenu, EEPTF_HEADER *pStoreForwardData)
{
	short sTypeDsi = (SHORT) lUieAc102_SubMenu;
	short sRetStat = -1;

	pStoreForwardData->lUieAc102_SubMenu |= (lUieAc102_SubMenu & EEPT_SUBMENU_NONSTATUSMASK);
	return sRetStat;
}

SHORT CliForwardStoreForward(LONG lUieAc102_SubMenu){
	//return XEptStartForwardThread();
	//release the Semaphore so it will be requested in the ittend1 function.
	//PifReleaseSem(semStoreForward);
	return SUCCESS;
}

SHORT CliToggleStoreForward(SHORT onOff){
	return -1;
}

LONG CliGetNextBlockIterateAll(LONG lUieAc102_SubMenu, EEPT_BLOCK_HEADER * blockHeader){
	SHORT  sTypeDsi = (SHORT)lUieAc102_SubMenu;
	return -1;
}

LONG CliGetNextBlockIterate(LONG lUieAc102_SubMenu, EEPT_BLOCK_HEADER * blockHeader){
	SHORT  sTypeDsi = (SHORT)lUieAc102_SubMenu;
	return -1;
}

LONG CliGetSpecifiedBlockHeader(LONG lUieAc102_SubMenu, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader){
	SHORT  sTypeDsi = (SHORT)lUieAc102_SubMenu;
	return -1;
}

LONG CliUpdateSpecifiedBlock(LONG lUieAc102_SubMenu, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader){
	SHORT  sTypeDsi = (SHORT)lUieAc102_SubMenu;
	return -1;
}
/*===== END OF SOURCE =====*/