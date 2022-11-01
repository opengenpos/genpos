/////////////////////////////////////////////////////////////////////////////
// SIO32.H :
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Jun-16-1997                        initial
//
/////////////////////////////////////////////////////////////////////////////

#ifndef	SIO32_H
#define	SIO32_H

#ifdef __cplusplus
extern "C"{
#endif


/////////////////////////////////////////////////////////////////////////////
//  Result Code
/////////////////////////////////////////////////////////////////////////////

#define	SIO32_OK					1
#define	SIO32_E_SYSERROR			(-1)
#define	SIO32_E_TIMEOUT				(-3)		// timeout
#define	SIO32_E_READ_ERROR			(-11)		// read (receive) error
#define	SIO32_E_WRITE_ERROR			(-12)		// write (send) error
#define	SIO32_E_OFFLINE				(-13)		// network is not ready

#define	SIO32_E_ALREADYOPENED		(-1001)
#define	SIO32_E_CREATEFILE			(-1002)

#define SIO32_ERROR_CTS_DSR			(-57)		// CTS Low and DSR Low
#define SIO32_ERROR_CTS				(-59)		// CTS Low
#define SIO32_ERROR_DSR				(-60)		// DSR Low
#define SIO32_ERROR_OVERRUN			(-62)		// Overrun Error
#define SIO32_ERROR_FRAMING			(-63)		// Framing Error
#define SIO32_ERROR_PARITY			(-64)		// Parity Error
#define SIO32_ERROR_NO_PORT			(-65)		// No Port Address
#define SIO32_ERROR_OVERFLOW		(-150)		// Buffer Overflow


/////////////////////////////////////////////////////////////////////////////
//  General
/////////////////////////////////////////////////////////////////////////////

#define	SIO32_CNTL_TXTIMER			1
#define	SIO32_CNTL_RXTIMER			2
#define	SIO32_CNTL_TXCLEAR			0x0001
#define	SIO32_CNTL_RXCLEAR			0x0002

#define	SIO32_STS_FRAMING			0x0800
#define	SIO32_STS_PARITY			0x0400
#define	SIO32_STS_OVERRUN			0x0200
#define	SIO32_STS_RLSD				0x0080
#define	SIO32_STS_DSR				0x0020
#define	SIO32_STS_CTS				0x0010


/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////

SHORT	SioInit32();
SHORT	SioOpen32(UCHAR uchPort, ULONG ulBaudRate, UCHAR uchByteSize,
				  UCHAR uchParity, UCHAR uchStopBits);
SHORT	SioClose32();
SHORT	SioRead32(VOID *pvReadBuf, SHORT sReadLen);
SHORT	SioWrite32(VOID *pvWriteBuf, SHORT sWriteLen);
SHORT	SioCntlGetStatus32();
BOOL	SioCntlSetTimer32(USHORT usFunc, ULONG ulTime);
BOOL	SioCntlClearBuffer32(USHORT usFlags);
BOOL	SioCntlDtr32(BOOL fState);
SHORT	SioGetErrorCode32();


/////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif                                      // ifndef(SIO32_H)
