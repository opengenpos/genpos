/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, Electoric Journal Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVEJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*   SerRecvEJ():    Electoric Journal function handling
*
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev. :   Name     : Description
* Nov-21-95 : 00.00.01 : T.Nakahata : Initial (R3.1)
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <ej.h>
#include    <csstbej.h>
#include    "servmin.h"
/*
*===========================================================================
** Synopsis:    VOID    SerRecvEJ(VOID)
*
** Return:      none.
*
** Description: This function executes for Elctoric Journal function request.
*===========================================================================
*/
VOID    SerRecvEJ( VOID )
{
    CLIREQEJ    *pReqMsgH;
    CLIRESEJ    ResMsgH;
    CLIRESEJREPT *pResReptMsgH;
    USHORT      usLen;
	SHORT       sSerSendStatus = 0;

    pReqMsgH = ( CLIREQEJ * )SerRcvBuf.auchData;

	NHPOS_ASSERT(sizeof(pReqMsgH->auchEJRead) >= sizeof(EJ_READ));
	NHPOS_ASSERT(sizeof(pResReptMsgH->auchEJRead) >= CLI_MAX_EJREAD);
	NHPOS_ASSERT(sizeof(pResReptMsgH->auchEJRead[0]) == sizeof(UCHAR));

    memset( &ResMsgH, 0, sizeof( CLIRESEJ ));
    ResMsgH.usFunCode   = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo     = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.ulDataLen   = 0;
    ResMsgH.sResCode    = STUB_SUCCESS;

    switch ( pReqMsgH->usFunCode & CLI_RSTCONTCODE ) {

    case CLI_FCEJCLEAR:     /* EJ  clear    */
        ResMsgH.sReturn = EJClear();
        break;             

    case CLI_FCEJREADFILE:  /* EJ read file */
        ResMsgH.sReturn = EJReadFile(pReqMsgH->ulOffset, ResMsgH.auchEJData, CLI_MAX_EJDATA, &ResMsgH.ulDataLen);
        break;             

    case CLI_FCEJREAD:      /* EJ read */
        pResReptMsgH = ( CLIRESEJREPT * )&ResMsgH;
        ResMsgH.sReturn = EJRead(( EJ_READ * )pReqMsgH->auchEJRead, pResReptMsgH->auchEJData, pReqMsgH->uchType );
        pResReptMsgH->usDataLen = CLI_MAX_EJREPTDATA;
        //_tcsncpy( pResReptMsgH->auchEJRead, pReqMsgH->auchEJRead, CLI_MAX_EJREAD );
        memcpy( pResReptMsgH->auchEJRead, pReqMsgH->auchEJRead, CLI_MAX_EJREAD );
        pResReptMsgH->usDataLen += CLI_MAX_EJREAD;
        break;

    case CLI_FCEJLOCK:     /* EJ Lock, V3.3    */
        ResMsgH.sReturn = EJLock(pReqMsgH->uchType);
        break;             

    case CLI_FCEJUNLOCK:   /* EJ UnLock, V3.3    */
        ResMsgH.sReturn = EJUnLock(pReqMsgH->uchType);
        break;             

    default:
        return;
    }

    usLen = sizeof( CLIRESEJ ) - CLI_MAX_EJDATA;

    sSerSendStatus = SerSendResponse(( CLIRESCOM * )&ResMsgH, (USHORT)(usLen + ResMsgH.ulDataLen), (UCHAR *)NULL, (USHORT)0);   /* ### Mod (2171 for Win32) V1.0 */
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerRecvEJ(): sSerSendStatus = %d, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn = %d", sSerSendStatus, pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgH.sReturn);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/* ===== End of File ( SERVEJ.C ) ===== */
