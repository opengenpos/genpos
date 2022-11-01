/////////////////////////////////////////////////////////////////////////////
// PIF32.H :
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Aug-32-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#ifndef	NET32_H
#define	NET32_H

#ifdef __cplusplus
extern "C"{
#endif

#pragma	pack(push, 8)


/////////////////////////////////////////////////////////////////////////////
//  Result Code
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  General
/////////////////////////////////////////////////////////////////////////////

#define PIF_NET_MAX_IP 16
#define DEVICENAME "NCRPOS-"

/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////

SHORT  NetOpen32(VOID *pData);
/* SHORT  NetOpen32(CONST XGHEADER FAR *pHeader); */
SHORT  NetSend32(USHORT usNet, CONST VOID FAR *pBuffer,
                           USHORT usBytes);
SHORT  NetReceive32(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes);
SHORT  NetControl32(USHORT usNet, USHORT usFunc, ...);
VOID   NetClose32(USHORT usNet);
USHORT NetInit32(VOID);
VOID NetUninit32(VOID);
VOID PifGetDestHostAddress(UCHAR *puchDestAddr, UCHAR *puchSrcAddr);
BOOL PifIsValidNet(USHORT usFile);
VOID Net32AbortEx(USHORT usFaultModule, USHORT usFaultCode, BOOL fAppl);

USHORT   NetInitialize32(USHORT  usPortNo);
VOID NetFinalize32(VOID);

/////////////////////////////////////////////////////////////////////////////


#pragma	pack(pop)

#ifdef __cplusplus
}
#endif

#endif                                      // ifndef(NET32_H)
