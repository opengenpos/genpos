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

#ifndef _NCRPWRHDR_
#define _NCRPWRHDR_

/////////////////////////////////////////////////////////////////////////////
// Include header files
/////////////////////////////////////////////////////////////////////////////

#ifndef CTL_CODE
# error Please include WINIOCTL.H or DEVIOCTL.H beforehand
#endif

/////////////////////////////////////////////////////////////////////////////
// Global Definitions
/////////////////////////////////////////////////////////////////////////////

// I/O Control function code

#define     IOCTL_NCRPWR_WAIT_EVENTS		CTL_CODE(FILE_DEVICE_UNKNOWN,   \
													 0x800,                 \
													 METHOD_BUFFERED,       \
													 FILE_ANY_ACCESS)

#define		IOCTL_NCRPWR_GET_AC_STATE		CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x801,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define     IOCTL_NCRPWR_SET_MODE			CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x802,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define     IOCTL_NCRPWR_GET_MODE			CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x804,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define     IOCTL_NCRPWR_ENTER_SUSPEND		CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x806,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define     IOCTL_NCRPWR_POWER_OFF			CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x808,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define     IOCTL_NCRPWR_POWER_OFF_EX		CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x80A,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define     IOCTL_NCRPWR_REBOOT				CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x80C,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define		IOCTL_NCRPWR_COPY_REGISTRY		CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x80E,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define		IOCTL_NCRPWR_RESTORE_REGISTRY	CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x810,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

#define		IOCTL_NCRPWR_ADVISE_SAFE_MODE	CTL_CODE(FILE_DEVICE_UNKNOWN,   \
		                                             0x812,                 \
				                                     METHOD_BUFFERED,       \
						                             FILE_ANY_ACCESS)

// DeviceIoControl(IOCTL_NCRPWR_WAIT_EVENTS) response data

typedef struct _NCRPWR_EVENTS				// power event status
{
	DWORD		dwEvents;						// power events
} NCRPWR_EVENTS, *PNCRPWR_EVENTS;

#define	NCRPWR_AC_LOST					1		// a/c lost
#define	NCRPWR_SWITCH_PRESSED			2		// power switch pressed
#define	NCRPWR_RETURN_FROM_SUSPEND		3		// return from suspend mode
#define	NCRPWR_AC_RECOVERED				4		// a/c recovered

// DeviceIoControl(IOCTL_NCRPWR_GET_AC_STATE) response data

typedef struct _NCRPWR_AC_STATE				// power state
{
	DWORD		dwState;						// power state
} NCRPWR_AC_STATE, *PNCRPWR_AC_STATE;

#define	NCRPWR_AC_STATE_ON				1		// a/c active
#define	NCRPWR_AC_STATE_OFF				2		// a/c inactive

// DeviceIoControl(IOCTL_NCRPWR_SET_MODE / IOCTL_NCRPWR_SET_MODE)

typedef struct _NCRPWR_MODE					// power mode definition
{
	BOOL		fEnablePowerOff;				// power off when power switch
	BOOL		fEnableSuspendOnAcLost;			// suspend when A/C lost
	BOOL		fRestoreRegistryWhenOn;			// restore registry file
	BOOL		fSaveRegistryWhenOff;			// copy registry file
	TCHAR		szRegistryFile[MAX_PATH];		// registry file name
} NCRPWR_MODE, *PNCRPWR_MODE;

// DeviceIoControl(IOCTL_NCRPWR_REBOOT) command parameter

typedef struct _NCRPWR_REBOOT				// reboot option
{
	DWORD		dwRebootFlag;					// reboot option flag
} NCRPWR_REBOOT, *PNCRPWR_REBOOT;

#define	NCRPWR_COLD_REBOOT				 1		// cold boot
#define	NCRPWR_WARM_REBOOT				 2		// warm boot
#define	NCRPWR_INSTANT_RESET		0x8000		// instant reset option

/////////////////////////////////////////////////////////////////////////////
#endif // _NCRPWRHDR_
/////////////////////////// End Of Program //////////////////////////////////
