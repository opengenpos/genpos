/////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//---------------------------------------------------------------------------
//   Ver.  |   Date   |         Notes                                 | By
//---------+----------+-----------------------------------------------+------
// 1.00.00 |          | Initial                                       |
/////////////////////////////////////////////////////////////////////////////

#ifndef _NCRSCOMMHDR_
#define _NCRSCOMMHDR_

/////////////////////////////////////////////////////////////////////////////
// Include header files
/////////////////////////////////////////////////////////////////////////////

#ifndef CTL_CODE
# error Please include WINIOCTL.H or DEVIOCTL.H beforehand
#endif

/////////////////////////////////////////////////////////////////////////////
// Calling conventions for C++ programs
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"{
#endif

/////////////////////////////////////////////////////////////////////////////
//  Serial Communication Driver interface
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Definitions
/////////////////////////////////////////////////////////////////////////////

// I/O Control function code

#define IOCTL_SERIAL_ATTACH_DEVICE		CTL_CODE(FILE_DEVICE_SERIAL_PORT,	\
												 24,						\
												 METHOD_BUFFERED,			\
												 FILE_ANY_ACCESS)

#define IOCTL_SERIAL_SET_POWER			CTL_CODE(FILE_DEVICE_SERIAL_PORT,	\
												 25,						\
												 METHOD_BUFFERED,			\
												 FILE_ANY_ACCESS)			\

#define IOCTL_SERIAL_QUERY_POWER		CTL_CODE(FILE_DEVICE_SERIAL_PORT,	\
												 26,						\
												 METHOD_BUFFERED,			\
												 FILE_ANY_ACCESS)			\

// DeviceIoControl(IOCTL_SERIAL_ATTACH_DEVICE)
//
// @InBuffer:  LPDWORD lpdwPort;	- pointer to a buffer to hold COM port number
// @OutBuffer: (not used)


// DeviceIoControl(IOCTL_SERIAL_SET_POWER)
//
// @InBuffer:  PSERIAL_SET_POWER	pPower;	// ptr. to a buffer to hold power setting
// @OutBuffer: (not used)

typedef struct _SERIAL_SET_POWER		// serial power control
{
    DWORD   dwMode;							// serial power mode
} SERIAL_SET_POWER, *PSERIAL_SET_POWER;

#define	SERIAL_POWER_DISABLE	1		// always disabled
#define	SERIAL_POWER_ENABLE		2		// always enabled
#define	SERIAL_POWER_AUTO		3		// judgement is performed in driver

// DeviceIoControl(IOCTL_SERIAL_QUERY_POWER)
//
// @InBuffer:  (not used)
// @OutBuffer: PSERIAL_QUERY_POWER	pPower;	// ptr. to a buffer to hold power setting

typedef struct _SERIAL_QUERY_POWER		// serial power control
{
    DWORD   dwMode;							// serial power mode setting
	BOOL	fSupplying;						// +12V is supplying
} SERIAL_QUERY_POWER, *PSERIAL_QUERY_POWER;

/////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
/////////////////////////////////////////////////////////////////////////////
#endif // _NCRSCOMMHDR_
/////////////////////////// End Of Program //////////////////////////////////
