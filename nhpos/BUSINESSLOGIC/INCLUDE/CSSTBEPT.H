/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user
*   Category           : Client/Server STUB EEPT interface
*   Program Name       : CSSTBEPT.H
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    Oct-28-93      :H.Yamaguchi        :Initial
*    Jan-21-94      :M.Ozawa            :Delete EEPT definition for GP
*    Nov-19-96      :Y.shozy            :Append EEPT definition for GP R2.0
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(CSSTBEPT_H_INCLUDED)
#define CSSTBEPT_H_INCLUDED

/*
===========================================================================
    DEFINE
===========================================================================
*/

#include "eept.h"
/*
===========================================================================
    TYPEDEF
===========================================================================
*/

/*------------------------------------------
    Charge Posting Request Message (Request)
------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  usEEptReqNo;
    USHORT  usSndLen;
    TCHAR   auchEEptData[CLI_MAX_EEPTDATA];
} CLIREQEEPT;

/*-------------------------------------------
    Charge Posting Request Message (Response)
-------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;                       /* Set by EEPT Recv Process */
    USHORT  usEEptReqNo;
    USHORT  usRetLen;                      /* Set by EEPT Recv Process */
    TCHAR   auchEEptData[CLI_MAX_EEPTDATA];  /* Set by EEPT Recv Process */
} CLIRESEEPT;

/*============================================================
    EEPT Send Buffer for EEPT Receive Process
============================================================*/
typedef struct {
        XGHEADER    EEptMesHed;    /* Header 8 bytes */
        CLIRESEEPT   EEptResDat;    /* Response Data  from Host computer */
}EEPTSENDMES;

/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*---------------------------------------
    USER I/F DFL FUNCTION (CLIENT)
----------------------------------------*/
/* For register Mode */

SHORT   CliRptPLURead(VOID *myTotal);

SHORT CliForwardStoreForward(LONG lUieAc102_SubMenu);

SHORT CliToggleStoreForward(SHORT onOff);
LONG CliGetNextBlockIterateAll(LONG lUieAc102_SubMenu, EEPT_BLOCK_HEADER * blockHeader);
LONG CliGetNextBlockIterate(LONG lUieAc102_SubMenu, EEPT_BLOCK_HEADER * blockHeader);
LONG CliGetSpecifiedBlockHeader(LONG lUieAc102_SubMenu, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader);
LONG CliUpdateSpecifiedBlock(LONG lUieAc102_SubMenu, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader);
/*
===========================================================================
    MEMORY
===========================================================================
*/
extern EEPTSENDMES    EEptSndBuf;                     /* Send Buffer         (R2.0)*/

#endif
/*===== END OF DEFINITION =====*/
