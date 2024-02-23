/*hhhh
*****************************************************************************
**    
**  Title:      Header File for Portable Platform Interface
**
**  File Name:  PIF.H
**
**  Categoly:   Portable Platform Interface
**
**  Abstruct:   This header file provides function prototype declarations
**              for Portable Platform Interface functions and defines data
**              types and symbolic constants used with the functions.
**              Applications, System Applications, Recovery Manager, Platform
**              Interface and Simulator modules should include this file.
**
**              When the Simulator includes this header, the Simulator should
**              defines "SIM". When the Platform Interface includes this
**              header, the Platform Interface should defines "PIFXINU".
**              When the other modules includes this header, they should
**              define nothing. 
**
**              PIF stands for Platform Independent Functions and was a
**              layer developed by NCR to provide an operating system API
**              abstraction layer to allow the use of multiple operating
**              systems with the same point of sale application.
**
**              NCR 7448 was the sucessor to the NCR 2170 for NHPOS Rel 1.4.Both used direct
**              peripheral communications through a serial port, RS-232 with DB-9 connector, with
**              devices such as printers. They did not use the OPOS components.
**
**              The NCR 7448 had a low resolution non-touchscreen LCD display and a keyboard user interface
**              similar to the final versions of the NCR 2170.
**
**              Support for OPOS for selected peripherals was added in 2003 with NHPOS Rel 2.0
**              and the porting of NHPOS from the NCR 7448 and Windows CE to Windows XP and the
**              newer NCR terminals such as the NCR 7402 touchscreen terminal.
**
**              NCR 7448 was manufacture discontinued around 2003 by NCR.
**
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date       Name           Description        
**  1.00      MAY-01-92  I.Shimizu      Initial release
**  00.00.01  AUG-17-92  I.Shimizu      Correspond to rev. G document
**            AUG-18-92  I.Shimizu      Add DEVICE_COIN_DISPENSER
**            AUG-20-92  I.Shimizu      Add PIF_ERROR_COM_BUSY
**            AUG-24-92  I.Shimizu      Change PIF_ERROR_COM_BUSY
**            SEP-04-92  I.Shimizu      Add PIF_ERROR_NET_XXXX
**                                      Add PifOpenMsr(), PifReadMsr() and
**                                      PifClose()
**                                      Modify PifGetWaiter()
**                                      Change the loaded S/W information
**            SEP-05-92  I.Shimizu      Add usRestartFlag in SYSCONFIG
**            SEP-11-92  O.Nakada       Modify fPip of PROTOCOL
**            SEP-11-92  O.Nakada       Add PIF_COM_PROTOCOL_XXX
**            SEP-16-92  O.Nakada       Add PIF_YAM_XXX
**            SEP-17-92  I.Shimizu      Modify PIF_SOFT_TABLE, PIF_SOFT_INFO
**                                      and SYSCONFIG
**            SEP-17-92  O.Nakada       Add PIF_COM_SEND_TIME,
**                                      PIF_YAM_SEND_TIMER
**            SEP-19-92  I.Shimizu      Modify PifXinuInit()
**            SEP-21-92  I.Shimizu      Add usInitFlag into SYSCONFIG
**                                      Delete uchInitSumCheck in SYSCONFIG
**                                      Add ausComBaud and auchComByteFormat
**                                      into SYSCONFIG
**            SEP-22-92  I.Shimizu      Add pointer to tally area in SYSCONFIG
**            SEP-24-92  I.Shimizu      Add pointer to RS232 log area
**                                      and POWER_UP_MENU
**            SEP-30-92  I.Shimizu      Add PIF_ERROR_MSR_POWER_FAILURE
**            OCT-06-92  I.Shimizu      Delete PIF_ERROR_COM_BUSY
**            OCT-09-92  I.Shimizu      Correct auchPFileDate in SYSCONF
**            OCT-10-92  I.Shimizu      Modify PIF_SOFT_INFO
**            OCT-28-92  T.Koyama       Change Log Size
**            OCT-29-92  I.Shimizu      Add pLogIHC into SYSCONF
**            FEB-04-93  O.Nakada       Add auchRmtPort into SYSCONF,
**                                      DEVICE_THERMAL_PRINTER and
**                                      PIF_COM_PROTOCOL_ESC
**            FEB-05-93  T.Kawamura     Add Dram Disk information into 
**                                      PIF_SOFT_INFO
**            FEB-17-93  O.Nakada       Add PIF_PIP_DIRECT_WRITE
**            FEB-19-93  Y.Nozawa       Added PIF_ERROR_COM_ACCESS_DENIED
**            MAY-12-93  T.Koyama       Change WAITER LOCK ID
**            JUL-05-93  T.Kojima       Change the size of "Key Track" and
**                                      "Pif Error Log"
**            JUL-07-93  T.Koyama       Add DEVICE_KITCHEN_CRT   
**                                          DEVICE_CHARGE_POSTING
**                                          DEVICE_EPT           
**            AUG-20-93  T.Kojima       Change SYSCONFIG, add modem initial
**                                      strings
**  D5        SEP-22-93  T.Kunishige    Add PIF_COM_PROTOCOL_XON
**  G0        FEB-20-95  O.Nakada       Correspond to hospitality release 3.0
**                                      functions
**  G4        MAR-11-96  O.Nakada       Add DEVICE_BEVERAGE
**  G5        APR-18-96  O.Nakada       Add LOAD_DEVICE_MODEM
**  G9        FEB-17-97  M.Suzuki       Add DEVICE_410_COIN_DISPENSER   
**                                          DEVICE_NEW_CHARGE_POSTING
**  G10       MAR-27-97  O.Nakada       Add DEVICE_SCANNER_SCALE
** 
** NCR2171 **
**  Ver1.0    AUG-20-99  T.Koyama       initial (for Win32)
**            OCT-16-02  R.Chambers     Added line for PifGetComRowHandle from Platform group
**
*****************************************************************************
hhhh*/


#ifndef _INC_PIF
#define _INC_PIF

#ifdef __cplusplus
extern "C"{
#endif


/*******************************
 *  Define Symbolic Constants  *
 *******************************/
/* data type and modifier of function entry */
#ifdef  _WIN32_WCE
#ifndef PASCAL
#define PASCAL  _stdcall
//#define PASCAL  __stdcall
#endif
#ifndef LOADDS
#define LOADDS
#endif
#ifndef CDECL
#define CDECL
#endif
#else
#ifndef _INC_WINDOWS
#ifndef PASCAL
#define PASCAL  pascal
#endif
#endif
#ifdef WIN32
#define LOADDS
#else
#define LOADDS  _loadds
#endif
#ifdef WIN32
#ifndef CDECL
#define CDECL
#endif
#else
#define CDECL   _cdecl
#endif
#endif      /* _WIN32_WCE */

#ifdef  _WIN32_WCE
#ifndef PIFENTRY
#define PIFENTRY    _stdcall
//#define PIFENTRY    __stdcall
#endif
#else
#if     defined(SIM)
    #define PIFENTRY FAR LOADDS
#elif   defined(PIFXINU)
/*     #define PIFENTRY FAR LOADDS */
    #define PIFENTRY FAR
#else
    #define PIFENTRY  
#endif
#endif      /* _WIN32_WCE */

/* data type and modifier of thread entry */
#define THREADENTRY  CDECL 

/* function return values or return data */
#define PIF_OK  1

#define PIF_ERROR_SYSTEM                (-1)

#define PIF_ERROR_FILE_EXIST            (-5)
#define PIF_ERROR_FILE_EOF              (-6)
#define PIF_ERROR_FILE_DISK_FULL        (-7)
#define PIF_ERROR_FILE_NOT_FOUND        (-8)

#define PIF_ERROR_COM_POWER_FAILURE     (-2)
#define PIF_ERROR_COM_TIMEOUT           (-3)
#define PIF_ERROR_COM_NOT_PROVIDED      (-4)
#define PIF_ERROR_COM_BUSY              (-5)
#define PIF_ERROR_COM_EOF               (-6)
#define PIF_ERROR_COM_ABORTED           (-9)
#define PIF_ERROR_COM_ACCESS_DENIED     (-10)      // same as PIF_ERROR_NET_ACCESS_DENIED
#define PIF_ERROR_COM_ERRORS            (-10)      // same as PIF_ERROR_NET_ERRORS
#define PIF_ERROR_COM_OFFLINE           (-13)
#define PIF_ERROR_COM_LOOT_TEST         (-55)
#define PIF_ERROR_COM_NO_DSR_AND_CTS    (-57)
#define PIF_ERROR_COM_NO_CTS            (-59)
#define PIF_ERROR_COM_NO_DSR            (-60)
#define PIF_ERROR_COM_OVERRUN           (-62)
#define PIF_ERROR_COM_FRAMING           (-63)
#define PIF_ERROR_COM_PARITY            (-64)
#define PIF_ERROR_COM_MONITOR           (-66)
#define PIF_ERROR_COM_BUFFER_OVERFLOW   (-150)
#define PIF_ERROR_COM_NO_INTERRUPT      (-151)
#define PIF_ERROR_COM_TIMEOUT_M_L       (-162)
#define PIF_ERROR_COM_COMM_M_L          (-163)

#define PIF_NET_ONLINE                  1
#define PIF_NET_OFFLINE                 0
#define PIF_ERROR_NET_POWER_FAILURE     (-2)        /* power failure            */
#define PIF_ERROR_NET_TIMEOUT           (-3)        /* network timeout          */
#define PIF_ERROR_NET_NOT_PROVIDED      (-4)        /* not supported            */
#define PIF_ERROR_NET_ALREADY_EXIST     (-5)        /* already exists. local end point is busy. not same as -16 which is remote end point busy. */
#define PIF_ERROR_NET_EOF               (-6)        /* end of file ?            */
#define PIF_ERROR_NET_DISK_FULL         (-7)        /* disk full ?              */
#define PIF_ERROR_NET_NOT_FOUND         (-8)        /* network host not found, hostname lookup failed   */
#define PIF_ERROR_NET_ABORTED           (-9)        /* function aborted         */
#define PIF_ERROR_NET_ACCESS_DENIED     (-10)       // access denied ?  same as PIF_ERROR_COM_ACCESS_DENIED
#define PIF_ERROR_NET_ERRORS            (-10)       // network errors   same as PIF_ERROR_COM_ERRORS
#define PIF_ERROR_NET_READ              (-11)       /* reading failure ?        */
#define PIF_ERROR_NET_WRITE             (-12)       /* writing failure ?        */
#define PIF_ERROR_NET_OFFLINE           (-13)       /* offline                  */
#define PIF_ERROR_NET_UNREACHABLE       (-14)       /* remote end point is unreachable or can not be found    */
#define PIF_ERROR_NET_MESSAGE           (-15)       /* message corrupt          */
#define PIF_ERROR_NET_BUSY              (-16)       /* target or remote end point is busy. not same as -5 error which is local busy.  */
#define PIF_ERROR_NET_BUFFER_OVERFLOW	(-21)		// indicates provided buffer is too small for packet
#define PIF_ERROR_NET_TOO_LONG          (-22)       /* too long data            */
#define PIF_ERROR_NET_CLEAR             (-23)       /* target cleared receive queue */

// --- the following errors are internal use or critical errors ---
#define PIF_ERROR_NET_RE_SCHEDULE       (-30)       /* re-scheduled   (internal)*/
#define PIF_ERROR_NET_MANY_USERS        (-35)       /* too many users (critical)*/
#define PIF_ERROR_NET_BAD_HANDLE        (-36)       /* bad handle     (critical)*/
#define PIF_ERROR_NET_BAD_DATA          (-37)       /* bad data       (critical)*/
#define PIF_ERROR_NET_CRITICAL          (-38)       /* critical error (critical)*/
#define PIF_ERROR_NET_QUEUE             (-39)       /* queue error    (critical)*/
#define PIF_ERROR_NET_MEMORY            (-40)       /* memory error   (critical)*/
#define PIF_ERROR_NET_NOT_WORK          (-41)       /* not initialized(critical)*/
#define PIF_ERROR_NET_ILLEGAL			(-51)		// indicates generic error.
#define PIF_ERROR_NET_DISCOVERY         (-52)       // indicates target not discovered
#define PIF_ERROR_NET_REQUEUE_RQST      (-53)       // indicates message received but prohibitted so requeue request

#define PIF_KEYERROR                0    /* indicate keyboard error */
#define PIF_KEYLOCK                 1    /* indicate first position of keylock */
#define PIF_VIRTUALKEYEVENT         8    // indicates virtual key board event such as FreedomPay wedge information, KEYBOARD_VIRTUAL
#define PIF_ERROR_KEY_POWER_FAILURE 0xFF /* indicate power failure */
                                         /* These values must not be changed. */

// These positions are based on the NCR 7448 terminal keylock.
// Some other configurations such as RealPOS with the NCR 5932 Wedge keyboard
// may not allow for all of these positions.
#define PIF_NCRKLOCK_MODE_EX		0			// Ex keylock position
#define PIF_NCRKLOCK_MODE_LOCK		1			// Lock keylock position
#define PIF_NCRKLOCK_MODE_REG		3			// Register keylock position
#define PIF_NCRKLOCK_MODE_SUP		4			// Supervisor keylock position
#define PIF_NCRKLOCK_MODE_PROG		5			// Program keylock position

#define PIF_DRAWER_CLOSED           0   /* This value must not be changed. */
#define PIF_DRAWER_OPENING          3
#define PIF_DRAWER_OPENED           2
#define PIF_DRAWER_NOT_PROVIDED     1

#define PIF_ERROR_WAITER_NOT_PROVIDED   (-4)

#define PIF_MSR_NO_DATA                 0
#define PIF_ERROR_MSR_POWER_FAILURE   (-2)
#define PIF_ERROR_MSR_NOT_PROVIDED    (-4)

#define PIF_ERROR_VIO                 (-1)    /* video I/O error */


#define PIF_MAX_FILE 80         /* V1.0.04 */

/* used with serial communication functions */
#define PIF_PIP_GET_STATUS      1
#define PIF_PIP_LOOP_TEST       2
#define PIF_PIP_GET_CONFIG      3
#define PIF_PIP_RESET_SEQ       4
#define PIF_PIP_DIRECT_WRITE    5

#define PIF_COM_GET_STATUS      1
#define PIF_COM_LOOP_TEST       2
#define PIF_COM_SEND_TIME       3             // set send timer value, address of variable with value, same value as PIF_NET_SET_TIME
#define PIF_COM_GET_CONFIG      4
#define PIF_COM_SET_TIME        5             // set receive timer value, address of variable with value, same value as PIF_NET_SET_STIME
#define PIF_COM_GET_RECEIVED_DATA_SIZE  6
#define PIF_COM_RECIVE_PURGE    7
#define PIF_COM_SEND_PURGE      8
#define PIF_COM_RESET_COMPULSORY    9

#define PIF_YAM_READ            1
#define PIF_YAM_WRITE           2
#define PIF_YAM_RECEIVE_TIMER   3
#define PIF_YAM_SEND_TIMER      4
#define PIF_YAM_GET_STATUS      5
#define PIF_YAM_YREAD_START     6       /* YMODEM read start */
#define PIF_YAM_YREAD_END       7       /* YMODEM read end  */
#define PIF_YAM_YWRITE_START    8       /* YMODEM write start */
#define PIF_YAM_YWRITE_END      9       /* YMODEM write end  */

#define PIF_COM_PROTOCOL_NON    0       // Use the specified COMM port with no underlying protocol
#define PIF_COM_PROTOCOL_PIP    1
#define PIF_COM_PROTOCOL_YAM    2
#define PIF_COM_PROTOCOL_ESC    3
#define PIF_COM_PROTOCOL_XON    4
#define PIF_COM_PROTOCOL_LAN    5		// Use network protocol with UDP rather than COMM port or TCP protoco, PIF_NET_GENERALPORTMODE
#define PIF_COM_PROTOCOL_DLL    6		// Use DLL based protocol (encapsulating Active-X object as for DataCap),SCF_TYPENAME_EPT_EX_DLL
#define PIF_COM_PROTOCOL_DLL1   7		// Use Virtual Keyboard based protocol (FreedomPay and PIF_VIRTUALKEYEVENT), SCF_TYPENAME_EPT_EX_DLL1
#define PIF_COM_PROTOCOL_DLL2   8		// Use DLL based protocol, SCF_TYPENAME_EPT_EX_DLL2
#define PIF_COM_PROTOCOL_DLL3   9		// Use DLL based protocol, SCF_TYPENAME_EPT_EX_DLL2
#define PIF_COM_PROTOCOL_TCP   10		// Use network protocol with TCP rather than COMM port or UDP protocol, PIF_NET_DIRECTPORTMODE

// Used with uchComTextFormat of struct PROTOCOL
#define PIF_COM_FORMAT_TEXT         0x20

#define  PIF_SEM_ACTION_FLAGS_REQ_ZERO       0x00000001      // indicates log if action is a Request and will block
#define  PIF_SEM_ACTION_FLAGS_REL_NONZERO    0x00000002      // indicates log if action is a Release and already released


/*
	used with PifNetControl() to set network options such as
	send or receive time out values.
*/
#define PIF_NET_SET_MODE    1       /* function IDs of control function */
    // defines for bit map value used with PIF_NET_SET_MODE
	#define PIF_NET_NMODE        0x01      /* normal mode              */
	#define PIF_NET_DMODE        0x02      /* data only mode           */
	#define PIF_NET_TMODE        0x20      /* timeout options          */
	#define PIF_NET_RESTRICTED   0x40      // Virtual port restricted to messages only within this cluster, PIF_PORT_FLAG_CLUSTER

	// additional internal net_body.c flags used for network connection state.
	// included here because used in conjunction with the above PIF_NET_SET_MODE flags.
	#define     PIF_NET_EVENT_MODE      0x2000      /* power failure event mode */
	#define     PIF_NET_POWER_FAIL      0x4000      /* power down     (internal)*/
	#define     PIF_NET_USED            0x8000      /* user used cell (internal)*/

#define PIF_NET_CLEAR              2       /* clear received messages  */
#define PIF_NET_SET_TIME           3       // set the receive timer value, same value as PIF_COM_SEND_TIME
#define PIF_NET_GET_STATUS         4
#define PIF_NET_SET_STIME          5       // set send timer value, same value as PIF_COM_SET_TIME
#define PIF_NET_RESET_COMPULSORY   7
#define PIF_NET_GET_NAME          20       /* get local name           */
#define PIF_NET_SET_REMOTE        21       /* set remote address       */
#define PIF_NET_SET_EVENT         22       /* set power down event     */
#define PIF_NET_SET_STANDBY       23       /* set standby state        */
#define PIF_NET_SET_TIME_EX       24       /* set receive timer value  */
#define PIF_NET_DIAG              25       /* execute diag. function   */
#define PIF_NET_SET_LOG           26       /* set LOG function address */
#define PIF_NET_POWER_FAILURE     27       /* set power down           */
#define	PIF_NET_RESET_SEQNO	      28       // Reset the message sequence number to zero in (pUser->infSending).usSeqNo
#define PIF_NET_GET_USER_INFO     30       /* retrieve the pUser stats */

#define PIF_NET_GENERALPORTMODE 0x0001     // standard terminal mode with UDP protocol and XGHEADER header. see PifNetOpenG().
#define PIF_NET_DIRECTPORTMODE  0x0002     // direct UDP protocol that does not use standard terminal mode. See PifNetOpenD().
#define PIF_NET_TCPCONNECTMODE  0x0004     // direct TCP protocol for special services such as remote LAN kitchen printer.

/* --- timer values used in PifNetControl() with function code  --- */
#define     PIF_NO_WAIT   ((USHORT)( 0))        /* no wait option           */
#define     PIF_FOREVER    ((SHORT)(-1))        /* wait forever             */

/*------------------------------------------------
    Port Address definition

	These port numbers are virtual port numbers that are
	part of the terminal messages transported over UDP/IP
	between the GenPOS terminals in a cluster.

	There is a single standard UDP/IP port number which is monitored
	by the PIF network layer. The PIF network layer reads the
	UDP/IP transported message and then looks at the message header
	for one of the following virtual port numbers to determine
	which thread network message queue to put the message on.

	This allows the single UDP/IP port number to be multiplexed
	between several threads. See SpawnNetManager() as well as
	NetManager() which are involved in the initialization of the
	network layer with the various virtual port queues.
-------------------------------------------------*/
#define ANYPORT 0       /* indicate any port assignment */

/* use port no. from 1 to 200 for PIFNET.DLL control, except prot no. 7, 2172 */
#define PIF_PORT_STUB        10     /* Stub or Client port number. See CstNetOpen() and CstInitialize(). See also CLI_PORT_STUB  */
#define PIF_PORT_SERVER      20     /* Server port number. See SerStartUp(). See also CLI_PORT_SERVER  */
#define PIF_PORT_NB          30     /* Notice Board port no. See also CLI_PORT_NB */
#define PIF_PORT_ISPSERVER   40     /* ISP or Remote Host Server port no. See IspMain() and IspNetOpen(). See also CLI_PORT_ISPSERVER */
#define PIF_PORT_CPM         50     /* Charge Posting Manager port no. See also CLI_PORT_CPM */
#define PIF_PORT_CPM_HOST    60     /* Charge Post IF to charge post host. See also CLI_PORT_CPM_HOST */
#define PIF_PORT_EEPT_HOST   70     /* EEPT IF to enhanced electronic payment terminal host. See also CLI_PORT_EEPT_HOST */

#define PIF_PORT_FLAG_REMOVE   0x00ff    // used to remove flags to keep only the port number
#define PIF_PORT_FLAG_CLUSTER  0x0100    // Flag indicates that messages should be filtered to be from Master or Backup Master only

#define PIF_NET_MAX_IP      16     /* max. cluster size */

/* used with "control keyboard, PifControlKeyboard()" */
#define PIF_KEY_ENABLE      0
#define PIF_KEY_DISABLE     1

/* used with display functions */
#define DISP_OPER   1
#define DISP_CUST   2

#define DISP_ATTR_NORMAL    0x00
#define DISP_ATTR_BLINK     0x80
#define DISP_ATTR_COMMA     0x20
#define DISP_ATTR_DP        0x40

#define DISP_DESCR_OFF      0
#define DISP_DESCR_ON       1
#define DISP_DESCR_BLINK    2

/* used with video I/O functions */
#define PIF_VIO_TEXT4025    1
#define PIF_VIO_TEXT8025    3
#define PIF_VIO_OFF         0
#define PIF_VIO_ON          1
#define PIF_VIO_NORMAL      0x01/*0x07*/  /* BACK:black, CHAR:white */ //colorpalette-changes
#define PIF_VIO_BLINK       0x80          /* blinking attribute     */


#define PIF_SEM_FLAG_LOG_REQUEST    0x0001
#define PIF_SEM_FLAG_LOG_RELEASE    0x0002

#define PIF_SEM_FLAG_LOG_REQ_REL    (PIF_SEM_FLAG_LOG_REQUEST | PIF_SEM_FLAG_LOG_RELEASE)

/* 
	used with "get system configuration, PifSysConfig()" to determine
	the hardware configuration.

	Some of the display options used with SysConfig.uchOperType
	are obsolete since the NCR 7448. The older NCR 2170 had several
	different display hardware options such as a 2x20 (2 rows, 20 characters)
	display.

	With touchscreen, its just a standard window within an LCD display.

	These are also used with SysConfig.uchCustType to indicate the type
	of customer display as well.
*/
#define DISP_NONE           0x00 
#define DISP_2X20           0x80      // indicates 2x20 display. 2 lines, 20 columns. obsolete for Operator Display. also used for Customer Display
#define DISP_10N10D         0x90      // some kind of an old, legacy display?
#define DISP_LCD            0xA0      // indicates standard LCD display for Operator Display. Standard touchscreen interface.
#define DISP_4X20           0xB0      /* 2172 indicates 4x20 display, 4 lines, 20 columns. */

//  Types of keyboards that can be used
//  These must correlate with the device names used in the first line of a
//  section for describing a keyboard device in file DeviceConfig.txt in
//  DeviceConfig subsystem.
//
//  NCR 7448 was the sucessor to the NCR 2170 for NHPOS Rel 1.4. Both allowed
//  two different kinds of keyboards, one with standard motion keys and one with
//  micromotion keys. NCR 7448 was manufacture discontinued around 2003 by NCR.
#define KEYBOARD_MICRO_NAME		_T("NCR 7448 Micromotion")
#define KEYBOARD_CONV_NAME		_T("NCR 7448 Conventional")
#define KEYBOARD_WEDGE_68_NAME	_T("NCR 5932 68 Key")
#define KEYBOARD_WEDGE_78_NAME	_T("NCR 5932 78 Key")
#define KEYBOARD_NONE_NAME		_T("NONE")
#define KEYBOARD_TOUCH_NAME		_T("Touch Screen")
#define KEYBOARD_BIG_TICKET		_T("Big Ticket Keyboard")

#define KEYBOARD_NONE       0x00
#define KEYBOARD_TOUCH      0x02  // Using touch screen terminal with no keyboard
#define KEYBOARD_CONVENTION 0x10  // obsolete NCR 7448 terminal with Conventional keyboard
#define KEYBOARD_MICRO      0x12  // obsolete NCR 7448 terminal with Micromotion keyboard
#define KEYBOARD_PCSTD_102  0x20  // standard PC 102 key keyboard
#define KEYBOARD_WEDGE_68   0x22  // 64 key NCR 5932 Wedge keyboard
#define KEYBOARD_WEDGE_78   0x24  // 78 key NCR 5932 Wedge keyboard
#define KEYBOARD_TICKET		0x26  // Big Ticket keyboard
#define KEYBOARD_VIRTUAL	0x28  // FreedomPay or other virtual terminal that uses Wedge interface

// Used as values for SysConfig.uchDrawer1 and SysConfig.uchDrawer2
// to indicate whether there are zero, one, or two cash drawers.
// GenPOS allows up to two different cash drawers which allows individual
// Cashiers to have their own cash drawer for purposes of money management.
#define DRAWER_NONE         0x00      // no cash drawer for this drawer position
#define DRAWER_PROVIDED     0xFF      // this drawer position does have a cash drawer provisioned.

// In addition to these defines see also SYSCONFIG_IF_WEDGE_ON as
// well as the use of the special flag KEYBOARD_VIRTUAL in DeviceEngine
// which is used to indicate if the MSR is a keyboard wedge type
// of device in which the card swipe data is provided via a series
// of Windows keyboard event messages.
#define IMSR_NONE           0x00
#define IMSR_TRACK12        0x03
#define IMSR_TRACK2         0x02
#define IMSR_TRACK2J        0x0A
#define IMSR_CPT_TRACK2		0x04	//this is used for Vivotech contactless payment terminals
#define IMSR_SIG_CAPTURE	0x10	//this is used for Ingenico i6550 and similar Signature Capture terminals
#define IMSR_PINPAD_OPOS	0x20	//this is used for Ingenico i6550 and similar OPOS type of PIN pad terminal
#define VIVOTECH_MSR		_T("ViVOtech")	

// Defines for values used with SEL.INPUT.DEV.MSR.uchPaymentType to indicate device or payment type for input
#define DEFAULT_ZERO		0
#define CONTACTLESS_PAYMENT 0x01
#define SWIPE_PAYMENT		12
#define	MANUAL_ENTRY		20
#define BIOMETRICS_ENTRY	25    // entry was through biometrics interface such as finger scan
#define	NEW_FSC_ENTRY		30    // used with UIFREG_NEW_FSC status return from UifDiaCP3().  see also UIF_DIA_NEW_FSC

#if !defined (ITM_MSR_SEPA)
#define ITM_MSR_SEPA                    0x3d    /* separator of ISO-II MSR  */
#define ITM_MSR_NAME_SEPA               0x5e    /* separator of ISO-II MSR, '^' character  */
#define ITM_MSR_NAME_FILL               0x7e    /* separator of ISO-II MSR, '~' character  */
#define ITM_MSR_NAME_LASTFIRST          0x2f    // separator between last name and first name, e.g. Briggs/John
#endif

#if !defined (ITM_MSR_GIFT_SEPA)
#define ITM_MSR_GIFT_SEPA				0x97	/* Separator for Gift Cards */
#endif

/* 2172 */
// The following defines influence the compiling of the
// printer library. There are two types of printers
// that were supported by NHPOS now GenPOS back in the
// days of the NCR 2170 point of sale terminal.
//   - Receipt/Journal printer which had a printed journal
//   - Receipt printer only which had an electronic journal
//
// Instead of being defined here, these defines are instead
// specified on the compiler command line so that they are
// defined with a compiler option on the command line and not
// through a preprocessor directive in a source file.
//   Richard Chambers, Nov-10-2020
#define PRINTER_NONE        0x00
//#define PRINTER_RJ          0x01     Kato
//#define PRINTER_THERMAL     0x02     Kato

// Following defines were used for several different NCR 2170 and
// NCR 7448 terminal configurations. These were used back in the
// 1990s to indicate if terminal to terminal communication was over
// RS-232 (COMM board) or over Ethernet LAN (Network board).
// By NHPOS 1.4 on the NCR 7448 most configurations used Ethernet
// for terminal to terminal communication.
#define COM_BOARD_NONE      0x00    // indicates COMM board hardware is not provided. Used with old NCR 2170
#define COM_PROVIDED        0xF0    // indicates comm is provided with RS-232. Used with old NCR 2170.
#define COM_PROVIDED_NET    0xF1	// indicates comm is provided with LAN

#define NET_BOARD_NONE      0x00    // indicates network board hardware is not provided. Used with old NCR 2170
#define NET_BOARD_NCR_DLC   0xFE
#define NET_BOARD_PROVIDED  0xFF


// These bit masks are used with Sysconfig.usInterfaceDetect to indicate what kinds
// of equipment have been provisioned through the DeviceConfig or other utility
#define  SYSCONFIG_IF_CPM_COMM    0x0001    // CPM over communications port
#define  SYSCONFIG_IF_CPM_LAN     0x0002    // CPM over LAN
#define  SYSCONFIG_IF_EEPT_COMM   0x0010    // EEPT over communications port
#define  SYSCONFIG_IF_EEPT_LAN    0x0020    // EEPT over LAN
#define  SYSCONFIG_IF_EEPT_DLL    0x0040    // EEPT using DLL interface such as for DataCap or other
#define  SYSCONFIG_IF_EEPT_DLL1   0x0080    // EEPT using DLL interface such as for FreedomPay or other
#define  SYSCONFIG_IF_EEPT_DLL2   0x0100    // EEPT using DLL interface such as for DataCap or other
#define  SYSCONFIG_IF_EEPT_DLL3   0x0200    // EEPT using DLL interface such as for DataCap or other
#define  SYSCONFIG_IF_WEDGE_ON    0x0400    // One or more devices are using a keyboard wedge interface

// The functionality for waiters was in the original
// NCR 2170 point of sale terminal software however
// with the NCR 7448 terminal when the several versions
// of NHPOS were consolidated into a single product ofering,
// it appears that the waiter functionality was not one of
// the pieces of functionality included in the combined product.
// The NCR 2170 had only a small amoutn of storage space and RAM
// so there were several different versions of NHPOS available
// for the NCR 2170 depending on the needs of the customer.
//    Richard Chambers, Nov-10-2020

#define WAITER_NONE         0x00        // value for SysConfig.uchWaiterType
#define WAITER_PEN          0x01        // value for SysConfig.uchWaiterType
#define WAITER_LOCK         0x02        // value for SysConfig.uchWaiterType

// These values are used to represent the base number of the EEPTDLL error
// codes to the max. Also the base LDT offset. If more than 800 EEPTDLL_LDTOFFSET
// LDT values are added. This definition should be changed.
#define EEPTDLL_ERROR_TEXT_MAX_LEN   48

#define EEPTDLL_BASE_ERROR		-100      // CPM_RET_EEPT_START
#define EEPTDLL_MAX_ERROR		-232      // see also EEPTDLL_MAX_LDTOFFSET below and tables such as EptErrorCodeTable[] in uieerror.c and DsiClient.cpp
#define EEPTDLL_BASE_ALT_TENDER   -100    // indicates lead thru should be LDT_ALTERNATE_TENDER_RQD
#define EEPTDLL_MAX_ALT_TENDER    -144    // indicates lead thru should be LDT_ALTERNATE_TENDER_RQD
#define EEPTDLL_LDTOFFSET		 800
#define EEPTDLL_BASE_LDTOFFSET	 900											//EEPTDLL_LDTOFFSET + labs(EEPTDLL_BASE_ERROR)
#define EEPTDLL_MAX_LDTOFFSET	 ((EEPTDLL_MAX_ERROR * -1) + EEPTDLL_LDTOFFSET)	//EEPTDLL_LDTOFFSET + labs(EEPTDLL_MAX_ERROR)

#define EEPTDLL_MULTIUSE_3328   -232      // see LDT_EPT_ERROR_MULTIUSE. indicates this is multi-use code 3328 or TRANSACTION NOT COMPLETE which contains an additional helpful text message
#define EEPTDLL_USE_ERROR_TEXT  -233      // indicates that the error text from the response should be used in the error dialog

#define RETURNS_BASE_ERROR		-100
#define RETURNS_MAX_ERROR		-232      // see also EEPTDLL_USE_ERROR_TEXT. see also EEPTDLL_MAX_LDTOFFSET below and tables such as EptErrorCodeTable[] in uieerror.c and DsiClient.cpp
#define RETURNS_LDTOFFSET		 2800
#define RETURNS_BASE_LDTOFFSET	 2900											//RETURNS_LDTOFFSET + labs(RETURNS_BASE_ERROR)
#define RETURNS_MAX_LDTOFFSET	 ((RETURNS_MAX_ERROR * -1) + RETURNS_LDTOFFSET)	//RETURNS_LDTOFFSET + labs(RETURNS_MAX_ERROR)

#define RETURNS_MSG_TENDERMUSTMATCH      (RETURNS_BASE_ERROR)
#define RETURNS_MSG_ALREADYSETTLED       (RETURNS_BASE_ERROR -  1)
#define RETURNS_MSG_MUSTBEFINALIZED      (RETURNS_BASE_ERROR -  2)
#define RETURNS_MSG_MUSTBEAPPROVED       (RETURNS_BASE_ERROR -  3)
#define RETURNS_MSG_MUSTBEFORWARDED      (RETURNS_BASE_ERROR -  4)
#define RETURNS_MSG_MUSTBESAMECARD       (RETURNS_BASE_ERROR -  5)
#define RETURNS_MSG_TRANSNOTALLOWED      (RETURNS_BASE_ERROR -  6)
#define RETURNS_MSG_ORIGINALDECLINED     (RETURNS_BASE_ERROR -  7)
#define RETURNS_MSG_TENDERTOTALEXCEEDED  (RETURNS_BASE_ERROR -  8)
#define RETURNS_MSG_TIPREQUIRESCC        (RETURNS_BASE_ERROR -  9)
#define RETURNS_MSG_TIPNOTONCARDSWIPED   (RETURNS_BASE_ERROR - 10)

// special defines for specific Electronic Payment errors
#define EEPTDLL_ERR_INVALID_NUMBER   (EEPTDLL_LDTOFFSET + 149)
#define EEPTDLL_ERR_EXPIRE_DATE      (EEPTDLL_LDTOFFSET + 150)

#define SIZE_64KB           1

/* for System Parameters to identify the device
	used in function PifGetLanConfig() and function
	PifGetSerialConfig () in pif subsystem, pifconf.c.

	See also use of table stagDeviceTable[] in pifconf.c and
	function PifGetActiveDevice() which uses the table.

	See also struct SYSCONFIG struct member UCHAR auchComPort[PIF_LEN_PORT]
	with which the following are used to indicate the device type that
	is using a particular Serial COM port.

	PIF stands for Platform Independent Functions and was a
	layer developed by NCR to provide an operating system API
	abstraction layer to allow the use of multiple operating
	systems with the same point of sale application.

	NCR 7448 was the sucessor to the NCR 2170 for NHPOS Rel 1.4.Both used direct
	peripheral communications through a serial port, RS-232 with DB-9 connector, with
	devices such as printers. They did not use the OPOS components.

	The NCR 7448 had a low resolution non-touchscreen LCD display and a keyboard user interface
	similar to the final versions of the NCR 2170.
	
	Support for OPOS for selected peripherals was added in 2003 with NHPOS Rel 2.0
	and the porting of NHPOS from the NCR 7448 and Windows CE to Windows XP and the
	newer NCR terminals such as the NCR 7402 touchscreen terminal.

	NCR 7448 was manufacture discontinued around 2003 by NCR.
*/
#define DEVICE_NONE                     0
#define DEVICE_PC                       1       // NCR 2172 Legacy PCIF communications over COM port. SCF_TYPENAME_PC removed DevicePC subsystem, 3/30/2021 Richard Chambers
#define DEVICE_SLIP_PRINTER             2		// changed to OPOS interface in Rel 2.0. NCR 2172 functionality, through Serial COM port. Still supported.
#define DEVICE_KITCHEN_PRINTER          3		// direct to device through Serial COM port. NCR 2172 functionality still supported. Added LAN interface in Rel 2.3. See also SCF_TYPENAME_KITCHEN_PRINTER
#define DEVICE_SCANNER                  4		/* direct to device through Serial COM port NCR 67xx scale. See also SCF_TYPE_SCALE*/
#define DEVICE_SCALE                    5		/* direct to device through Serial COM port. See also SCF_TYPE_SCANNER */
#define DEVICE_PREPAY_CARD              6       // NCR 2172 Legacy that appears to have been removed with the port to the NCR 7448.
#define DEVICE_COIN_DISPENSER           7       // direct to device through Serial COM port from NCR 7448 for cafeterias. See also SCF_TYPENAME_COIN
#define DEVICE_KITCHEN_CRT              8
#define DEVICE_CHARGE_POSTING           9       // See also DEVICEIO_CHARGE_POSTING and SCF_TYPENAME_CHARGE_POSTING. legacy charge post. COM2170 charge post system? Obsolete. Removed 3/22/2021 Richard Chambers
#define DEVICE_EPT                      10      // NCR 2172 Legacy Electronic Payment for COM port to old style electronic payment terminal. SCF_TYPENAME_EPT removed DeviceEpt subsystem, 3/30/2021 Richard Chambers
#define DEVICE_THERMAL_PRINTER          11      // NCR 2170 and NCR 7448 legacy thermal printer through RS-232 serial port. replaced with OPOS control in Rel 2.0 in 2003.
#define DEVICE_BEVERAGE                 12      // NCR 2172 7448  legacy beverage dispenser. SCF_TYPENAME_BEVERAGE
#define DEVICE_NEW_CHARGE_POSTING       13      // legacy charge post. COM2170 charge post system? Obsolete. Removed 3/22/2021 Richard Chambers
#define DEVICE_410_COIN_DISPENSER       14		// NCR 2170 legacy NCR 410 change dispenser? Obsolete.
#define DEVICE_SCANNER_SCALE            15		/* direct to device through Serial COM port NCR 78xxx in counter scanner/scale for grocery stores. See also SCF_TYPE_SCANNER as well as type list beginning with UIE_SCANNER_UNKNOWN */
#define DEVICE_ODISPLAY                 16      /* 2172 Operator facing display. Originally 2x20 with indicator LEDs then LCD display */
#define DEVICE_CDISPLAY                 17      /* 2172 Customer facing 2x20 display */
#define DEVICE_EPT_EX                   18      /* NCR 7448 Saratoga. legacy enhanced electronic payment interface. SCF_TYPENAME_EPT_EX removed DeviceEptEx subsystem, 3/30/2021 Richard Chambers */
#define DEVICE_CHARGE_POSTINGLAN        19      // legacy charge post. COM2170 charge post system? Obsolete. SCF_TYPENAME_CHARGE_POSTINGLAN Removed 3/22/2021 Richard Chambers
#define DEVICE_EPT_LAN                  (DEVICE_CHARGE_POSTINGLAN + 1)
#define DEVICE_EPT_EX_LAN               (DEVICE_EPT_LAN + 1)
#define DEVICE_EPT_EX_DLL               (DEVICE_EPT_EX_LAN + 1)    // NHPOS 2.x electronic payment through Datacap control. SCF_TYPENAME_EPT_EX_DLL
#define DEVICE_EPT_EX_DLL1              (DEVICE_EPT_EX_DLL + 1)
#define DEVICE_EPT_EX_DLL2              (DEVICE_EPT_EX_DLL + 2)
#define DEVICE_EPT_EX_DLL3              (DEVICE_EPT_EX_DLL + 3)
#define DEVICE_RJ_PRINTER               99       // legacy receipt/journal printer. replaced by electronic journal.

#define COM_BYTE_ODD_PARITY     0x08
#define COM_BYTE_EVEN_PARITY    0x18
#define COM_BYTE_2_STOP_BITS    0x04
#define COM_BYTE_7_BITS_DATA    0x02
#define COM_BYTE_8_BITS_DATA    0x03

// for device config option for handshake with serial connections JHHJ 9-13-05
#define COM_BYTE_HANDSHAKE_NONE		0x01
#define COM_BYTE_HANDSHAKE_RTSCTS	0x02
#define COM_BYTE_HANDSHAKE_CTS		0x04
#define COM_BYTE_HANDSHAKE_RTS		0x08
#define COM_BYTE_HANDSHAKE_XONOFF	0x10
#define COM_BYTE_HANDSHAKE_DTRDSR	0x20

/* for Interface Parameters */
#define POWER_UP_RESET              0x01
#define POWER_UP_CLEAR_TOTAL        0x02
#define POWER_UP_CLEAR_WHOLE_MEMORY 0x04
#define POWER_UP_PROM               0x10
#define POWER_UP_RSYSTEM            0x20
#define POWER_UP_MENU               0x40
#define POWER_UP_CLEAR_FILE         0x80

#define LOAD_MODE_UNIQUE            0x0001      /* for T-POS/E */
#define LOAD_MODE_GROUP             0x0002      /* for T-POS   */
#define LOAD_MODE_SYSTEM            0x0004
#define LOAD_MODE_APPL              0x0008
#define LOAD_MODE_EXEC              0x0010

#define LOAD_MODE_DEPT              0x0800      /* for T-POS   */
#define LOAD_MODE_PLU               0x1000      /* for T-POS   */
#define LOAD_MODE_PARA3             0x2000      /* for T-POS   */
#define LOAD_MODE_PARA2             0x4000      /* for T-POS   */
#define LOAD_MODE_PARA1             0x8000      /* for T-POS   */

#define IF_PARA_LOADED_SOFTWARE         0x0001
#define IF_PARA_REQUEST_TURN_OFF_POWER  0x0002
#define IF_PARA_LOAD_REQUEST_FROM_APPL  0x0004

#define IF_PARA_LOADED_DEPT             0x0800
#define IF_PARA_LOADED_PLU              0x1000
#define IF_PARA_LOADED_PARA3            0x2000
#define IF_PARA_LOADED_PARA2            0x4000
#define IF_PARA_LOADED_PARA1            0x8000

/* for Store and Forward */

#define PIF_STORE_AND_FORWARD_DISABLED			0x00   // disabled
#define PIF_STORE_AND_FORWARD_ENABLED			0x01   // enabled, look to next bits to determine if it is on for particular interface
#define PIF_STORE_AND_FORWARD_ONE_ON			0x02   // interface one (Credit) enabled and on
#define PIF_STORE_AND_FORWARD_TWO_ON			0x04   // interface two (Employee Payroll Deduction) enabled and on
#define PIF_STORE_AND_FORWARD_ONE_SINGLE		0x08   // interface one (Credit) enabled for single transaction only
#define PIF_STORE_AND_FORWARD_ONE_FORWARDING	0x10   // indicates that Store and Forward interface one (Credit) is forwarding
#define PIF_STORE_AND_FORWARD_TWO_FORWARDING	0x20   // indicates that Store and Forward interface two (Employee Payroll Deduction) is forwarding
#define PIF_STORE_AND_FORWARD_STAT_MASK			0x0f   // used to mask out the status bits that are in the upper nibble.
#define PIF_STORE_AND_FORWARD_FORWARDING        (PIF_STORE_AND_FORWARD_ONE_FORWARDING | PIF_STORE_AND_FORWARD_TWO_FORWARDING)
#define DSI_STORE_AND_FORWARD_OFF               0      // indicates a particular DSI Client is NOT doing Store and Forward.
#define DSI_STORE_AND_FORWARD_ON                1      // indicates a particular DSI Client is doing Store and Forward.
#define DSI_STORE_AND_FORWARD_TOGGLE_SINGLE  0x10      // indicates a particular DSI Client is to do opposite of current settings a single time.
#define DSI_STORE_AND_FORWARD_TOGGLE_CLEAR   0x20      // indicates a particular DSI Client is to clear the Toggle Single if on.

// above three definitions correspond to SCF_DATANAME_STOREFORWARD
#define PIF_STORE_AND_FORWARD_NAME_MAX	9 // Maximum length of EEpt Store file filename. Corresponds to RMT_FILENAME_SIZE in 
										  // NHPOS\BUSINESSLOGIC\Isp\rmt.h and PEP\PCIF\Include\R20_RMT.H

/* for Unique Information */
#define LOAD_DEVICE_COM             1           /* load by RS-232C         */
#define LOAD_DEVICE_MODEM           2           /* load by Modem           */

/* for Loaded Software Information */
#define SOFT_INFO_TYPE_APPL         0x0001
#define SOFT_INFO_TYPE_SYSTEM       0x0002

/*For Release 2.x, we change PIF_LEN_UDATA to match the PIF_LEN_UDATA_EX, previously
it was set to 477, this needed to be increased to accomodate for TCHARs.  I chose 1024
because when a PifNetSend is called, it sends the value in an XGRAMEX structure, which uses
the PIF_LEN_UDATA_EX, which is 1024, when the master receives the data, it was using a regular
XGRAM structure, which only had a buffer size of PIF_LEN_UDATA(477) JHHJ

NOTE: this define is also located in pif.h, and pifnet.h, in the regular include file, AND the
      SYSTEMCONFIG include files.
	  
NOTE: See also IspRecvSysConfig() and the PCIF message format buffer
      CLISYSPARA which is used to report SYSCONFIG settings to a
      remote management station should you be thinking of changing any
	  of these defines. Changes may affect PCIF as well.
*/

/* length */
#define PIF_LEN_THREAD_NAME     9 
#define PIF_LEN_PORT            (6+1) /* reserve port 0 - expected max number of physical RS-232 serial ports plus 1 */
/* #define PIF_LEN_PORT            5 */

#define PIF_LEN_UDATA           1400   //JHHJ   this define should match CLI_MAX_REQDATA in csstbfcc.h
#define PIF_LEN_UDATA_EX        1400    /* 2172 */
#define PIF_LEN_IP              4
#define PIF_LEN_HOST_NAME       16      /* 2172 */
#define PIF_LEN_RMT_PORT        4
#define PIF_LEN_OPTION          9
#define PIF_LEN_PFILE           5
#define PIF_LEN_PFILE_DATE      4
#define PIF_LEN_EXCEPTION_LOG   50
#define PIF_LEN_FAULT_LOG       10  
#define PIF_LEN_KEY_TRACK       500 
#define PIF_LEN_FUNC_TRACK      60
#define PIF_LEN_ERROR_TRACK     91
#define PIF_LEN_SOFTWARE_ID     17
#define PIF_LEN_PACKAGE_ID      17
#define PIF_LEN_TRACK1          80   // was 76.  See http://www.gae.ucm.es/~padilla/extrawork/tracks.html
#define PIF_LEN_TRACK2          44   // was 37.  See http://www.gae.ucm.es/~padilla/extrawork/tracks.html
#define PIF_LEN_TRACKJ         108   // was 69.  See http://www.gae.ucm.es/~padilla/extrawork/tracks.html
#define PIF_LEN_TRACK2_PAD       6   // padding used with function RflEncryptByteString(), RflDecryptByteString().
#define PIF_LEN_VERSION         64 

#define PIF_LEN_ABORT_DISPLAY_LEN   40
#define PIF_LEN_M0DEM_STR		64

#define PIF_STANZA_CLICK        1
#define PIF_STANZA_ERROR        2
#define PIF_STANZA_CYCLIC       3

#define PIF_TASK_TIME_CRITICAL  7
#define PIF_TASK_HIGHEST        6
#define PIF_TASK_ABOVE_NORMAL   5
#define PIF_TASK_NORMAL         4
#define PIF_TASK_LOWEST         3
#define PIF_TASK_ABOVE_IDLE     2
#define PIF_TASK_IDLE           1

// typedef for the values used with the PifFile functions for
// the file manipulation functions.  All functions that are
// using Pif file functions should use this as the type
// for variables containing a Pif file handle.
typedef short PifFileHandle;
#define PIF_FILE_INVALID_HANDLE  0xffff

typedef unsigned short PifSemHandle;
#define PIF_SEM_INVALID_HANDLE  0xffff

typedef struct {
	ULONG    ulSymbology;
	ULONG    ulAlignment;
	ULONG    ulTextPosition;
} UifPrinterBarcodeStruct;

extern UifPrinterBarcodeStruct  UifPrinterBarcodeSettings;


#define PIF_MAX_LOGO_PATH		260
#define PIF_MAX_NUM_LOGOS		10
/*
	Following are used as part of audit process to provide
	PifLog () traces for possible problems with loops which
	are exited only if particular conditions apply.  These
	constants are used for initial number of iterations before
	a pertinent PifLog () is issued as well as for resetting the
	count down variable to issue a new PifLog () if the terminal
	is stuck in the loop.

	Typically we want to provide some kind of PifLog () within the
	first 5 iterations through the loop and then follow that with a
	PifLog () every 10 or 15 seconds.  In most cases, the operator or
	supervisor will not wait much longer than about a minute before
	doing something drastic such as rebooting the terminal.

	The goal of this is to provide a mechanism to let us know if a
	terminal lockup condition is being caused by a wait loop for which
	the exit criteria is not being met within a reasonable time.

	Most places with retry loops have a PifSleep () of about 100
	milliseconds between iterations so a reset for the count down of
	150 means about 15 seconds between PifLog ().


	Notes on Setting up and Using Automation Testing
	------------------------------------------------
	On setting up and using automation to run a series of terminal actions automatically.

	To use the automation functionality you must have the option turned on in the registry or
	you can create the environmental variable UIE_TEST_OPTION8 so that when GenPOS starts up
	and checks for that variable, it will set SysConfig.ausOption[UIE_TEST_OPTION8] properly
	to allow automation to be used.

	Then you must record the key sequence of actions that you want to run automatically.

	Setting up the Registery for automation.
	In the registry, 
      The registry key is HKEY_LOCAL_MACHINE\SOFTWARE\NCR\SARATOGA\Pif\Option
      Add a new Dword Key labeled "Option8", with the Decimal value of 2172.

	To run automation.
	  Start the recording by entering a delay in seconds followed by pressing the receipt feed button.
	     The delay time in seconds is 4 digits as in 0005 for 5 seconds or 0010 for ten seconds.
		 The delay is the time spent waiting before repeating the automation sequence.
		 You should hear a couple of beeps indicating you are in record mode.
		     If you don't hear the beeps, check that the registry settings exist.

	  Enter the key strokes or button presses that you want to record.
	     Automation will record on key strokes or button presses that haven FSC associated with them.
		 Automation will only work for BusinessLogic type FSC processing.
		 Automation does not drive the actual windowing interface so can not be used for some buttons.

	  End the recording by pressing 00 and the journal feed button.
	     You should hear a couple of beeps indication you are out of record mode and in run mode.

	  To stop the automation run, press the Clear key or button and it will stop momentarily.

	Some considerations.
	  Turn off receipt printing or you will generate lots of paper.
	  Set EJ to override so as to eliminate possibilitye of EJ Full.
	  Any error dialogs will stop automation until the dialog is acknowledged and closed.
	  The fastest usable rate is about 5 seconds.  10 seconds is most commonly used however.

 */
#define PIF_LOG_COUNT_DOWN_START     5
#define PIF_LOG_COUNT_DOWN_RESET     150

/* definitions of return value for PifRequestPowerSem() */

#define PIF_RETURN_FROM_SUSPEND   1
#define PIF_AC_LOST               2
#define PIF_SWITCH_PRESSED        3

/* used with "control power switch, PifControlPowerSwitch()", V1.0.04 */
#define PIF_POWER_SWITCH_ENABLE      0
#define PIF_POWER_SWITCH_DISABLE     1

/* definitions of registry keys for PIF control */

/* PIF root key */
#define PIFROOTKEY TEXT("Software\\NCR\\Saratoga\\Pif")

/* PIF key for file*/
#define FILEKEY TEXT("File")

/* PIF datas of file key */
#define PATH        TEXT("Path")        /* saratoga path */
#define TEMPPATH    TEXT("TempPath")    /* saratoga path for temporary DRAM file */
#define DATABASE    TEXT("DataBase")    /* file directory */
#define TOTALDATA   TEXT("TotalData")   /* total file directory */
#define PIFLOG      TEXT("Log")         /* piflog file directory */
#define KEYBOARD    TEXT("Keyboard")    /* Keyboard 80/176, Saratoga */

/* PIF key for network */
#define NETWORKKEY TEXT("Network")

/* PIF key for port */
#define PORT    TEXT("PORT")
#define MODEMSTRINGS TEXT("ModemStrings")

/* PIF Registry key for options */
// to set this up in the registry HKEY_LOCAL_MACHINE\SOFTWARE\NCR\SARATOGA\Pif\Option
// See PIFROOTKEY and OPTIONKEY.

#define OPTIONKEY TEXT("Option")

#define OPTION1   TEXT("Option1")
#define OPTION2   TEXT("Option2")
#define OPTION3   TEXT("Option3")
#define OPTION4   TEXT("Option4")
#define OPTION5   TEXT("Option5")
#define OPTION6   TEXT("Option6")
#define OPTION7   TEXT("Option7")
#define OPTION8   TEXT("Option8")     // Turn on automation with the receipt feed button, see notes above.  see also UIE_TEST_OPTION8
#define OPTION9	  TEXT("Option9")	  //used for testing contactless VivoTech.  see also UIE_TEXT_OPTION9

// SYSCONFIG memory resident area SysConfig.ausOption[] offsets for options follow

#define UIE_DECIMAL_OPTION1     1       /* number of decimal places, 0 -> 2 places, 1 -> 3 places, 2 -> no places */
#define UIE_LOG_OPTION6         5       /* logging flags, see also RflLoggingRulesTransactions()  */
#define UIE_POWER_OPTION7       6       /* power down option */

#define UIE_TEST_PASS           (2172)  /* password               */
#define UIE_TEST_OPTION8        7       /* system debug or test option for automation */

// Option9 is set in the registry and is used for testing vivotech ontactless payments only!!
// if this option is set then it will always use the Operator ID as test 
// add a DWORD Option9 and the value is 9
#define OPTION_9							9
#define UIE_TEXT_OPTION9					8		//TESTING FOR VIVOTECH DEVICES

#define LOG_COMMA       1
#define LOG_CR          1
#define LOG_LINENO      4
#define LOG_TASKNAME    8
#define LOG_MODULEID    4
#define LOG_CODE        4
#define LOG_DATETIME    8

#define LOG_HEADLINE (LOG_LINENO + LOG_COMMA + LOG_LINENO + LOG_COMMA + LOG_CR)
#define LOG_LOGLINE  (LOG_TASKNAME + LOG_COMMA + LOG_MODULEID +  LOG_COMMA + LOG_CODE + LOG_COMMA+ LOG_DATETIME + LOG_COMMA + LOG_DATETIME + LOG_COMMA + LOG_CR)
#define LOG_MAX_LINE  25000           // max number of lines written by PifLog() before returning to beginning of file.
#define LOGTRANS_LINENO      5
#define LOGTRANS_LOGLINE  (256)
#define LOGTRANS_HEADLINE (LOGTRANS_LINENO + LOG_COMMA + LOGTRANS_LINENO + LOG_COMMA + LOG_CR)
#define LOGTRANS_MAX_LINE  50000          // max number of lines written by PifTransactionLog() before returning to beginning of file.


// the following defines are used to create pseudo FSC codes that will be
// interpreted not as key presses but rather as commands for actions needed
// in the touch screen button to key press translation since a button press
// may result in several calls to the UI key buffer to push data such as
// a PLU number followed by the PLU key FSC code.
//
// WARNING:  the value for FSC_PRICE_GUARD_SCAN will need to change as new FSCs are added.
//
#define FSC_PRICE_GUARD_ASCII		0xFF
#define FSC_PRICE_GUARD_SCAN		0x36

#define FSC_QTY_GUARD_ASCII			0xFF
#define FSC_QTY_GUARD_SCAN			(FSC_PRICE_GUARD_SCAN + 1)

#define FSC_CNT_STRING_GUARD_ASCII	0xFF
#define FSC_CNT_STRING_GUARD_SCAN	(FSC_QTY_GUARD_SCAN + 1)

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

/************************
 *  Declare Data Types  *
 ************************/
/* PIFDEF data type is used by the Recovery Manager, the Portable Platform   */
/* Interface and the Simulator to access the table of pointer to a function. */
typedef VOID (PIFENTRY *PIFDEF)(VOID);

 
/************************
 *  Declare Structures  *
 ************************/
/* structures used with each functions */
typedef struct {           // see function PifGetDateTime(DATE_TIME *pDateTime), SYSTEMTIME data used
    USHORT  usSec;
    USHORT  usMin;         // starting here must match N_DATE struct for totals
    USHORT  usHour;        //    must match N_DATE struct
    USHORT  usMDay;        //    must match N_DATE struct
    USHORT  usMonth;       // ending here must match N_DATE struct
    USHORT  usYear;
    USHORT  usWDay;        // same as wDayOfWeek of SYSTEMTIME, 0 -> Sunday thru 6 -> Saturday
	TCHAR   wszTimeZoneName[32];    // truncated timezone from SYSTEMTIME added for Amtrak
} DATE_TIME;


typedef struct {
    UCHAR   auchFaddr[PIF_LEN_IP];   // IPv4 host address for UDP or TCP traffic
    USHORT  usFport;                 // Far port number for UDP or TCP traffic
    USHORT  usLport;                 // Local port number for UDP traffic
} XGHEADER, *PXGHEADER;

/* --- my name buffer structure --- */

typedef struct {                                /* XGNAME structure         */
    UCHAR   auchLaddr[PIF_LEN_IP];              /* ULONG format IP      */
    UCHAR   auchSaddr[PIF_LEN_IP];              /* ULONG format source IP last receive */
    USHORT  usIpPortNo;                         /* UDP/IP port number last receive */
    USHORT  usLport;                            /* NHPOS local port number    */
} XGNAME, *PXGNAME;

typedef struct {
    UCHAR   auchFaddr[PIF_LEN_IP];
    USHORT  usFport;
    USHORT  usLport;
    UCHAR   auchData[PIF_LEN_UDATA];
} XGRAM, *PXGRAM;

typedef struct {
    UCHAR   auchFaddr[PIF_LEN_IP];
    USHORT  usFport;
    USHORT  usLport;
    UCHAR   auchData[PIF_LEN_UDATA_EX];
} XGRAMEX, *PXGRAMEX;  /* for externel communication, 2172 */

// for examples of usage see kitchen printing functionality in cskpin.c
// see function KpsSetProtocol() for initialization example.
typedef struct {
    SHORT   fPip;
    USHORT  usPipAddr;
    ULONG   ulComBaud;
    UCHAR   uchComByteFormat;
    UCHAR   uchComTextFormat;
    UCHAR   auchComNonEndChar[4];
    UCHAR   auchComEndChar[3];
    UCHAR   uchComDLEChar;
	USHORT  usNetPortNo;        // IP port number for TCP connections using host address in uchNetIpAddr
	union {
		UCHAR   uchNetIpAddr [4];   // IPv4 host address for TCP connection or UDP terminal address.
		XGHEADER xgHeader;          // PifNet networking layer for terminal UDP or TCP traffic. See PifOpenNetComIoEx().
	};
} PROTOCOL;

//#define LEN_COM_PARA  12  /* this is the size from usComBaud to uchComDLEChar */

// Following are structs and defines for standard Serial Port devices such
// as Scanner or Serial Port MSR which previously had their own unique
// versions of what was the same struct and defines.
typedef struct _PIF_DEVICE_IF {
    SHORT   usHandle;                       /* handle of serial port */
    UCHAR   fchStatus;                      /* status                */
} PIF_DEVICE_IF;

// Following defines used with fchStatus in PIF_DEVICE_IF struct
#define PIF_DEVICE_IF_PROVIDED     0x01        /* device is provided and usable */


typedef struct {
    UCHAR   uchLength1;
    TCHAR   auchTrack1[PIF_LEN_TRACK1];
    UCHAR   uchLength2;
    TCHAR   auchTrack2[PIF_LEN_TRACK2];
    UCHAR   uchLengthJ;
    TCHAR   auchTrackJ[PIF_LEN_TRACKJ];
	UCHAR	uchPaymentType;
} MSR_BUFFER;

typedef struct {
    USHORT  usLength;
    TCHAR   auchEncryptedPin[64];
    USHORT  usLengthKsn;
    TCHAR   auchKeySerialNumber[64];
    UCHAR   uchLength1;                     /* ISO track-1 length      */
    TCHAR   auchTrack1[PIF_LEN_TRACK1];     /* ISO track-1 data        */
	UCHAR   uchLength2;
	TCHAR   auchTrack2[PIF_LEN_TRACK2];
	UCHAR	uchPaymentType;
	LONG    lAmount;                        // amount to display as part of PIN entry
	USHORT  usOptions;                      // options for PINPad device I/O UIF_PINPAD_OPTION_READ_PIN and others
	SHORT   sPinPadRet;                     // contains EEPT_RET_xxx status from XEptDllCollectStripeAndPin(), XEptDllCollectPin(), etc.
} PINPAD_BUFFER;                            // this struct must be same as struct UIE_PINPAD in size and members

typedef struct {
	USHORT   usOptions;                      // options for the I/O request
	LONG     m_SigCapMaxX;                   // max X for signature; SIGCAP_BUFFER struct
	LONG     m_SigCapMaxY;                   // max X for signature; SIGCAP_BUFFER struct
	TCHAR   *m_SigCapPointArray;             // pointer to buffer holding signature; SIGCAP_BUFFER struct
	USHORT   m_ArrayLength;                  // length of the array currently in m_SigCapPointArray; SIGCAP_BUFFER struct
	USHORT   m_ArrayMaxSize;                 // max buffer size containing signature; SIGCAP_BUFFER struct
} SIGCAP_BUFFER;


#define PIF_LEN_SCANNER     (1 + 101)       /* length of scanner       */

typedef struct {               /* scanner                 */
    UCHAR   uchStrLen;                      /* scanner length          */
    TCHAR   auchStr[PIF_LEN_SCANNER];       /* scanner data            *///RPH WIDE
} SCANNER_BUFFER;

//----------------------------------------------------------
// Following is used for virtual key type devices that
// have special interfaces.  An example is a device such as
// the FredomPay electronic payment terminal which has a
// USB connection but appears to be a keyboard.
// Associated defines to locate this code are:
//    BL_DEVICE_DATA_VIRTUALKEY

#define VIRTUALKEY_EVENT_FREEDOMPAY      1
#define VIRTUALKEY_EVENT_SCANNER         2
#define VIRTUALKEY_EVENT_MSR             3

typedef struct {
	USHORT  usKeyEventType;      // Type of event hence type of data in this datastructure, VIRTUALKEY_EVENT_
	union {
		struct {  // data associated with usKeyEventType == VIRTUALKEY_EVENT_FREEDOMPAY
			UCHAR   uchLength1;
			TCHAR   auchTrack1[PIF_LEN_TRACK1];
			UCHAR   uchLength2;
			TCHAR   auchTrack2[PIF_LEN_TRACK2];
			UCHAR   uchLengthJ;
			TCHAR   auchTrackJ[PIF_LEN_TRACKJ];
		} freedompay_data;
		struct {  // data associated with usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER
			UCHAR   uchStrLen;                      /* scanner length          */
			TCHAR   auchStr[PIF_LEN_SCANNER];       /* scanner data            *///RPH WIDE
		} scanner_data;
		struct {  // data associated with usKeyEventType == UIE_VIRTUALKEY_EVENT_MSR
			UCHAR   uchLength1;                     /* ISO track-1 length      */
			TCHAR   auchTrack1[PIF_LEN_TRACK1];     /* ISO track-1 data        */
			UCHAR   uchLength2;                     /* ISO track-2 length      */
			TCHAR   auchTrack2[PIF_LEN_TRACK2];     /* ISO track-2 data        */
			UCHAR   uchLengthJ;                     /* JIS track-II length     */
			TCHAR   auchTrackJ[PIF_LEN_TRACKJ];     /* JIS track-II data       */
		} msr_data;
	} u;
} VIRTUALKEY_BUFFER;

#define CHARDATA_FLAGS_TOUCHUTTON      0x01      // CharData.uchFlags to indicate keypress to use touch screen menu page for translation of CharData
#define CHARDATA_FLAGS_HARDKEY         0x02      // CharData.uchFlags to indicate keypress is a hard coded key such as P1, P2, etc.

typedef struct {
	UCHAR   uchFlags;        // if non-zero then indicates special processing.  See CHARDATA_FLAGS_ defines above.
    UCHAR   uchASCII;
    USHORT  uchScan;
} CHARDATA;

/*
*    STANZA parameters are used with the PifPlayStanza() function to
*    create a short sound effect.
*
*    See ItemSalesMakeStanza() for an example of creating a STANZA.
*
*    See as well function UieQueueStanzaToPlay() which allows a STANZA
*    to be put into the play queue to be played by the thread of
*    function UiePlayStanzaThread().
*/
#define STANZA_DURATION_END_STANZA       0    // indicates the end of the stanza sequence
#define STANZA_FREQUENCY_SILENCE        19    // indicates a duration of silence.
#define STANZA_FREQUENCY_LOOP       0x7fff    // indicates the stanza sequence is to loop back to the first, duration indicates number of times.
typedef struct {
    USHORT  usDuration;        // duration of the tone to be played in milliseconds
    USHORT  usFrequency;       // frequency of the tone to be played in hertz for the given duration
} STANZA;

//logo file information for printing multiple receipts.
//JHHJ
typedef struct {
	ULONG ulSpecialPercent;
	UCHAR uchSpecialLogo;
	UCHAR uchStandardLogo;
	TCHAR aszFileName[PIF_MAX_NUM_LOGOS][PIF_MAX_LOGO_PATH];
	UCHAR uchStatus;
} FLOGOFILE;

/* structures for the logging and tracking functions */
/* The logging and tracking area are defined in the system, and each usMark, */
/* usMaxCount and usCount are initialied by the system.                      */
/* for the Exception Log */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of aExceptionLog */
    USHORT  usCount;    /* current count */
    struct {            /* ring buffer (ring buffer) */   
        UCHAR   auchDateTime[7]; /* mmddyyyyhhmmss */
        USHORT  usModuleId;      /* module ID */
        USHORT  usExceptionCode; /* exception code (error code or event code) */
    } aExceptionLog[PIF_LEN_EXCEPTION_LOG];
} PIF_EXCEPTION_LOG;

/* for the Fault Log */
typedef struct {
    USHORT  usMark;                   /* 0xFFFF fixed */
    USHORT  usMaxCount;               /* max number of apRetAddr */
    UCHAR   auchThreadName[PIF_LEN_THREAD_NAME-1];  /* thread name */
    USHORT  usFaultModule;            /* module ID */
    UCHAR   uchFaultCode;             /* fault code */
    UCHAR   auchDateTime[7];          /* mmddyyyyhhmmss */
    USHORT  usXinuStatus;             /* XINU status coverted to "+" */
    VOID    *pCurrentStack;           /* current stack pointer */
    PIFDEF  apRetAddr[PIF_LEN_FAULT_LOG]; /* return address to the callers */
} PIF_FAULT_LOG;
    
/* for the Key Track */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of auchKeyTrack */
    USHORT  usCount;    /* current count */
    UCHAR   auchKeyTrack[PIF_LEN_KEY_TRACK]; /* tracking area (ring buffer) */
} PIF_KEY_TRACK;

/* for the Function Track of the Recovery Manager */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of aFuncTrack */
    USHORT  usCount;    /* current count */
    struct {            /* tracking area (ring buffer) */
        USHORT  usModuleId; /* module ID */
        PIFDEF  pRetAddr1;  /* return address of the caller */
        PIFDEF  pRetAddr2;  /* return address of the caller of the caller */
    } aFuncTrack[PIF_LEN_FUNC_TRACK];
} PIF_FUNC_TRACK;    

/* for the Error Track of the XINU status of the Platform Interface */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of aFuncTrack */
    USHORT  usCount;    /* current count */
    struct {            /* tracking area (ring buffer) */
        USHORT  usModuleId;   /* module ID */
        USHORT  usXinuStatus; /* XINU status converted to "+" */
    } aErrorTrack[PIF_LEN_ERROR_TRACK];
} PIF_ERROR_TRACK;

/* for Resource Log of the Recovery Manager */
typedef struct {
    VOID  *paThreadTable;
    VOID  *paXinuTable;
    VOID  *paSemTable;
    VOID  *paFileTable;
    VOID  *paPortTable;
    VOID  *paNetTable;
    VOID  *paXinuProcess;
    VOID  *paXinuQueue;
    VOID  *paXinuSem;
    VOID  *paXinuPort;
    VOID  *paXinuHeap;
    SHORT *psXinuPid;
} PIF_RESOURCE;

/* Loaded Software Segment Information Table */
typedef struct {
    USHORT  usLoadType;                 /* 0: Unrecoverable  1: Recoverable */
    VOID    (*StartAddress)(VOID);      /* Start Address */
    USHORT  usCodeSegment;              /* The First Code Segment Adddress */
    ULONG   ulCodeSize;                 /* Total Size of Code Segments */
    USHORT  ulDataSrcSegment;           /* Initial Data to Load*/
    USHORT  usDataSize;                 /* Initialized Data Segment Size */
    USHORT  usDataBssSize;              /* Uninitialized Data Segment Size */
    USHORT  usDataTargetSegment;        /* Runtime Data Segment Address */
    USHORT  usStackSize;                /* Main Thread Stack Size */
    USHORT  usStackSegment;             /* Runtime Stack Segment Address */
    USHORT  usFarDataSrcSegment;        /* -+                   */
    USHORT  usFarDataSize;              /*  +- RAM machine only */
    USHORT  usFarDataBssSize;           /*  |                   */
    USHORT  usFarDataTargetSegment;     /* -+                   */
} PIF_SOFT_TABLE;

/* Loaded Software Information */
typedef struct {
    PIF_SOFT_TABLE  *apSoftTable[4];
    USHORT  usCodeSegment;
    UCHAR   uchNoPage;
    UCHAR   auchSoftwareId[PIF_LEN_SOFTWARE_ID];
    UCHAR   auchPackageId[PIF_LEN_PACKAGE_ID];
    USHORT  usSoftwareType;
    UCHAR   uchParaPage;
    VOID    *pParaStartAddress;
    UCHAR   auchDateTime[6];
    UCHAR   auchReserved_A[14];
    USHORT  ausSoftwareCrc[8];
    UCHAR   auchSoftwareSumCheck[20];
    UCHAR   auchReserved_B[11];
    UCHAR   uchSumCheck;    /* Sum-Check of the Loaded Software Information */
} PIF_SOFT_INFO;

/* structure of application header defined by astart.asm of application */
typedef struct {
    USHORT  usRamDiskId;        /* fixed 0x02 */
    VOID    *pRamDiskAddr;      /* 512 bytes boundary */
    UCHAR   uchRamDiskPage;     /* start page# (0-) */
    VOID    *pCommonWork;       /* 16 bytes boundary (NULL: not provided) */
    USHORT  usCommonWorkSize;   /* bytes size (0: 64KB) */
    UCHAR   auchReserved[19];
} PIF_APPL_HEADER;

/* structure of system header defined by start_up.asm of system */
typedef struct {
    UCHAR   uchSumcheck;
    UCHAR   auchReserved_A[15];
    UCHAR   szSystemId[18];
    UCHAR   szRSystemId[18];
    UCHAR   auchReserved_B[6];
} PIF_SYSTEM_HEADER;

/* Unique Information */
typedef struct {
    PIF_RESOURCE *pResource;
    UCHAR   uchLoadDevice;
    UCHAR   uchGroupAddr;
    USHORT  ausHddErrorCount[2];
    VOID    *pTallyHDD;
    VOID    *pCommonWork;
    USHORT  usCommonWorkSize;
    VOID    *pStackDump;
    UCHAR   uchStrap;
    PIF_SYSTEM_HEADER  *pSystemHeader;
    USHORT  usNetworkId;
    USHORT  usFacilityId;
    USHORT  usLaneNumber;
    UCHAR   uchSecurityEnable;      /*  Don't change these structures */
    UCHAR   auchSecurityKey[8];     /*                                */
    UCHAR   uchSecuritySumCheck;    /*                                */
    UCHAR   auchLoadCom[1];
    USHORT  ausLoadBaud[1];
    UCHAR   auchReserved_A[16];
} PIF_UNIQUE_INFO;

typedef struct {
	ULONG  ulDevice;            // contains device ID such as DEVICE_EPT_EX_DLL
	ULONG  ulTimeOut;           // network send/receive time out in milliseconds
	USHORT usPortNo;			// port number of the charge post gateway
	UCHAR  ucIpAddress[4];      // IP address of the charge post gateway
	USHORT usStoreForward;      // SCF_DATANAME_STOREFORWARD - Store And Forward status
	TCHAR  tchStoreForwardFile[PIF_STORE_AND_FORWARD_NAME_MAX]; // SCF_DATANAME_STOREFORWARDFILE - Store and Forward file name; 
} PIF_CPMEPT_INFO;

typedef struct {
	USHORT   usStoreNumber;        // The store number for this terminal
	USHORT   usRegisterNumber;     // the number of this particular terminal or register
	TCHAR    wszHostName[32];       // the name of the host
	UCHAR    auchHostIpAddress[4];  // the IP address of the host
	UCHAR    auchSlotStatus;        // 0 not in use, 1 in use standard, 2 in use named
} PIF_CLUSTER_HOST;

#define PIF_CLUSTER_HOST_NIU      0   // Not in use
#define PIF_CLUSTER_HOST_STANDRD  1   // In use, Standard
#define PIF_CLUSTER_HOST_NAMED    2   // In use, Named terminal

#define PIF_CLUSTER_DISCONNECTED_SAT  0x100   // used with Sysconfig.usTerminalPosition to indicate Disconnected Satellite
#define PIF_CLUSTER_JOINED_SAT        0x200   // used with Sysconfig.usTerminalPosition to indicate Disconnected Satellite was joined
#define PIF_CLUSTER_MASK_TERMINAL_NO(x)  ((x) & 0x001f)  // used to mask out terminal number in usTerminalPosition 

// Following defines are used to index into the array
// SysConfig.EPT_ENH_Info[ept_enh_count] which contains the
// IP address and port number information as setup by DeviceConfig.
#define  PIF_EPT_ENH_EX_LAN       0       // SCF_TYPENAME_EPT_EX_LAN
#define  PIF_EPT_ENH_EX_DLL       1       // SCF_TYPENAME_EPT_EX_DLL
#define  PIF_EPT_ENH_EX_DLL1      2       // SCF_TYPENAME_EPT_EX_DLL1
#define  PIF_EPT_ENH_EX_DLL2      3       // SCF_TYPENAME_EPT_EX_DLL2
#define  PIF_EPT_ENH_EX_DLL3      4       // SCF_TYPENAME_EPT_EX_DLL3
#define  PIF_EPT_ENH_EX_DLLLAST   (PIF_EPT_ENH_EX_DLL3 + 1)


// flags and masks to be used with ulCustomerSettingsFlags field of SYSCONFIG struct
// see function ProcessXmlFileStartUpCustomerSettings(), pifmain.c in BusinessLogic project
#define  SYSCONFIG_CUSTOMER_ENABLE_AMTRAK           0x00000001   // EEPTREQDATA_CUSTOMERFLAGS_ENABLE_AMTRAK, Amtrak PARAMINI amtraksettings
#define  SYSCONFIG_CUSTOMER_ENABLE_USCBP            0x00000002   // US Customs and Border Protection PARAMINI uscbpsettings
#define  SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE     0x00000004   // various Rel 2.1 and earlier behavior PARAMINI uscbpsettings
#define  SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST   0x00000008   // EEPTREQDATA_SPECIALFLAGS_MERCURYPAYTEST, Trigger additional logic for Mercury Payments, now Vantiv, testing
#define  SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH  0x00000010   // EEPTREQDATA_SPECIALFLAGS_NO_PARTIAL_AUTH, Turn off EPT Partial Authorization

// The ausOption array member of the SYSCONFIG struct describes various options that govern
// GenPOS behavior.  Some of these may no longer be used due to software evolution as they were
// originally used with the old and discontinued NCR 2170 or earlier terminal.
//  index      Description
//    0       
//    1        number of decimal places.  if value is 1 then 3 decimal places, if 2 then no decimal places (deprecated, now using MDC Address 367)
//    3        number of terminals in the cluster. (deprecated, now using MDC Address 365)
//    4        notice board
//    7        if value is 2172 then debug and automated testing is allowed.  see also UIE_TEST_OPTION8 and OPTION8 Registry key.
//    8        vivotech test indicator.  see also UIE_TEXT_OPTION9 and OPTION9 Registry key.

/* structure of system configuration */
typedef struct {
    /* Hardware Configuration */
    UCHAR   uchOperType;     // from NCR 2170 days. the operator display type. See DISP_LCD.
    UCHAR   uchCustType;     // the customer display type. Same defines as used for uchOperType.
    UCHAR   uchKeyType;
    UCHAR   uchDrawer1;      // whether there is a drawer one or not. See DRAWER_NONE and DRAWER_PROVIDED
    UCHAR   uchDrawer2;      // whether there is a drawer two or not. See DRAWER_NONE and DRAWER_PROVIDED
    UCHAR   uchIntegMSRType;
    UCHAR   uchPrinter; /* 2172 */
    UCHAR   uchCom;           // COMM board configuration. See defines beginning with COM_BOARD_NONE and comments there.
    UCHAR   uchNet;           // Network board configuration. See defines begging with NET_BOARD_PROVIDED
    UCHAR   uchWaiterType;    // unused since NCR 7448. See WAITER_NONE, WAITER_PEN, WAITER_LOCK.
    UCHAR   uchHardDisk1;      // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchHardDisk2;      // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchCardType;       // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchOper2Type;      // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchKey2Type;       // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchCust2Type;      // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchIntegMSR2Type;  // obsolete from NCR 2170 days. No longer used?
    UCHAR   uchSRAM1Size;      // obsolete since NCR 7448 and no longer used.
    UCHAR   uchSRAM2Size;      // obsolete since NCR 7448 and no longer used.
    UCHAR   uchDRAMSize;       // obsolete since NCR 7448 and no longer used.
    UCHAR   uchROM1Size;       // obsolete since NCR 7448 and no longer used.
    UCHAR   uchROM2Size;       // obsolete since NCR 7448 and no longer used.
    UCHAR   uchSRAM3Size;      // obsolete since NCR 7448 and no longer used.
    UCHAR   uchSRAM4Size;      // obsolete since NCR 7448 and no longer used.
    UCHAR   uchSRAM5Size;      // obsolete since NCR 7448 and no longer used.
    ULONG   ulFreeBytesAvailableToCaller;    // disk space. See PifSetDiskFreeSpace()
    ULONG   ulTotalNumberOfBytes;            // disk space. See PifSetDiskFreeSpace()
    ULONG   ulTotalNumberOfFreeBytes;        // disk space. See PifSetDiskFreeSpace()
    UCHAR   auchReserved_A[7];      /* for 32 bytes boundary */

    /* System Parameters
	 * NOTE: See also IspRecvSysConfig() and the PCIF message format buffer
	 *       CLISYSPARA which is used to report SYSCONFIG settings to a
	 *       remote management station.
	 *
	 * WARNING: The size PIF_LEN_PORT of these arrays was set by the number
	 *          of physical Serial COM ports on the NCR 7448 terminal, six.
	 *          The COM port number is used as an index into these arrays in
	 *          most cases.
	 *          A COM port number greater than COM6 is not expected!
	*/
    UCHAR   auchLaddr[PIF_LEN_IP];
    UCHAR   auchHaddr[PIF_LEN_IP];    /* 2172, host ip address */
	USHORT  usfPip[PIF_LEN_PORT];               // PIF_COM_PROTOCOL_aaa type. Normally PIF_COM_PROTOCOL_NON
    UCHAR   auchComPort[PIF_LEN_PORT];          // device type id such as DEVICE_KITCHEN_PRINTER
    USHORT  ausComBaud[PIF_LEN_PORT];           // baud rate for COM port usage
    UCHAR   auchComByteFormat[PIF_LEN_PORT];
    XGHEADER xgComHeader[PIF_LEN_PORT];            // kitchen printer LAN data.
    USHORT  ausOption[PIF_LEN_OPTION];
    USHORT  usSoftwareNumber;               
    USHORT  ausPFileNumber[PIF_LEN_PFILE];      /* parameter file number */
    UCHAR   auchRmtPort[2][PIF_LEN_RMT_PORT];   /* protocol converter 2174 */
    UCHAR   auchReserved_C[15];      /* for 16 bytes boundary */
    /* UCHAR   auchReserved_C[3];      / for 16 bytes boundary */
    UCHAR   uchSumCheck;            /* Sum-Check of the System Parameters */

    /* Interface Parameters */
    UCHAR   uchPowerUpMode;
    USHORT  usLoadMode;
    PIFDEF  CONST  *papPifFuncTable;      /* pointer to the table of */
                                          /* pointer to a function   */
    PIF_EXCEPTION_LOG  *pExceptionLog;    /* pointer to the Exception Log area */
    PIF_FAULT_LOG      *pFaultLog;        /* pointer to the Fault Log area */
    PIF_KEY_TRACK      *pKeyTrack;        /* pointer to the Key Track area */
    PIF_FUNC_TRACK     *pFuncTrack;       /* pointer to the Function Track area */
    PIF_ERROR_TRACK    *pErrorTrack;      /* pointer to the Error Track area */
    PIF_UNIQUE_INFO    *pUniqueInfo;      /* pointer to the Information */
    PIF_SOFT_INFO      *pSoftInfo;        /* pointer to the Loeaded S/W Info */
    VOID  *pTallyRJ;                      /* pointer to the R/J's tallies */
    VOID  *pTallySlip;                    /* pointer to the slip's tallies */
    VOID  *pTallyDLC;                     /* pointer to the DLC/IHC tallies */
    VOID  *pLogRS232;                     /* pointer to the RS232 log area */
    VOID  *pLogIHC;                       /* pointer to the IHC log area */
    USHORT  usInterfacePara;              /* Interface Parpameter */
    UCHAR   auchPFileDate[PIF_LEN_PFILE]  /* date for parameter file */
                         [PIF_LEN_PFILE_DATE];
	USHORT  usInterfaceDetect;            // bit map showing what interfaces such as Charge Post and EPT are setup
	USHORT  usNotUsed02;  //    USHORT  usRegisterNumber;
    VOID    *pModemStrings;    /* pointer of modem strings */
    UCHAR   auchReserved_D[2];  /* for 16 bytes boundary */
    UCHAR   uchRAMDiskPage;
    VOID    *pRAMDiskAddr;
    USHORT  usInitFlag;
    USHORT  usRestartFlag;
	USHORT  usTerminalPosition;       // position of terminal within cluster (1, 2, .. 16; 0x0100 and above for Disconnected Satellite (PIF_CLUSTER_DISCONNECTED_SAT)

	UCHAR   ucCPMaddr[PIF_LEN_IP];    // charge post manager host IP address 
	USHORT  usCPMport;                // charge post manager host port number
	unsigned int	unCodePage;
	PIF_CPMEPT_INFO EPT_ENH_Info [16];
	UCHAR	uchKBMode; //US CUSTOMS SCER For Rel 2.x JHHJ 7-25-05
	UCHAR   auchComHandShakePro[PIF_LEN_PORT];	//Handshake options for kitchen printers
	PIF_CLUSTER_HOST   PifNetHostInformation[16];  // host information for cluster
	USHORT  usTerminalPositionFromName;         // real terminal position as specified in Computer Name
	TCHAR   tcsLogoExtension[8];
	USHORT  usLoadLogoDelay;
	ULONG   ulCustomerSettingsFlags;
	WCHAR   tcsTerminalIdKeyword[48];     // identification string keyword, Amtrak traintrip.ini keyword with value for terminal id
	WCHAR   tcsReceiptPrinterInitString[48];  // initialization string used by some printers.  set in PARAMINI with initstring tag.  see ProcessXmlFileStartUpReceiptPrinterRules()
	WCHAR   tcsReportsHistoricalFolder[256];  // folder to place historical reports. use with Microsoft OneDrive or Google Drive.
	WCHAR   tcsReportsHistoricalType[8];      // type of file to create when generating historical reports.
    ULONG   ulReportsHistoricalMnemonics;     // flags for mnemonics options
} SYSCONFIG;

// following struct used by PIF_NET_GET_USER_INFO control function
typedef struct {
	USHORT  usLastSenduchNo;          // pMph->uchNo value of last sent message
	USHORT  usLastRecvuchNo;
	SHORT   sLastSendPifError;
	SHORT   sLastRecvPifError;
	int     iLastSendErrorWsa;
	SHORT   sLastSendErrorPif;
	int     iLastRecvErrorWsa;
	SHORT   sLastRecvErrorPif;
} PIFUSRTBLINFO, *PPIFUSRTBLINFO;

// changes to following structs must be synchronized with changes to SYSTLY and USRTLY
//
// **  WARNING  **  Additions to these structs may require a change to a static buffer in
//                  the function PifNetSystemInfo() that is used for the AC888 report.
//                  See comments for variable PifNetworkSystemInfoBuffer in PifNetSystemInfo().
typedef struct _PIF_NETSYSTALLY {               /* system tally from PIFNET.H Same as SYSTLY  */
    ULONG   ulTotalSent;                        /* no. of sent totally      */
    ULONG   ulSentError;                        /* no. of sent errors       */
    USHORT  usLastSentError;                    /* last sent error code     */
    ULONG   ulTotalRecv;                        /* no. of recv totally      */
    ULONG   ulRecvError;                        /* no. of recv errors       */
    USHORT  usLastRecvError;                    /* last sent error code     */
    ULONG   ulOutOfResources;                   /* no. of out of resources  */
    ULONG   ulIllegalMessage;                   /* no. of illegal messages  */
    ULONG   ulRecvUndeliver;                    /* no. of recv undeliverable*/
    ULONG   ulRecvLostTimeOut;                  /* no. of recv lost due to time expired */
    ULONG   ulRecvLostQueueFull;                /* no. of recv lost due to queue full */
    ULONG   ulRecvLostResources;                /* no. of recv lost due to insufficient resources */
    ULONG   ulRecvLostNetClosed;                /* no. of recv lost due to network close */
    ULONG   ulAckSentStatusBusy;                /* no. of recv lost due to time out with an Ack sent status busy */
    ULONG   ulSendUnreachableError;             /* no. of send errors due to address unreachable */
} PIF_NETSYSTALLY;

// **  WARNING  **  Additions to these structs may require a change to a static buffer in
//                  the function PifNetSystemInfo() that is used for the AC888 report.
//                  See comments for variable PifNetworkSystemInfoBuffer in PifNetSystemInfo().
typedef struct _PIF_NETUSERTALLY {              /* user tally from PIFNET.H Same as USRTLY */
	USHORT  usUserPort;                         // port number of GenPOS port for these statistics
    USHORT  usTotalSent;                        /* no. of sent totally      */
    USHORT  usSentError;                        /* no. of sent errors       */
    USHORT  usTotalRecv;                        /* no. of recv totally      */
    USHORT  usRecvError;                        /* no. of recv errors       */
    USHORT  usRecvLost;                         /* no. of recv lost         */
    USHORT  usNoOfPending;                      /* no. of pendings          */
	USHORT  usPendingHighWater;                 // max number of pending for this user
    USHORT  usRecvLostTimeOut;                  /* no. of recv lost due to time expired */
    USHORT  usRecvLostQueueFull;                /* no. of recv lost due to queue full */
    USHORT  usRecvRequestTimeOut;               /* no. of recv requests that timed out without a message coming in */
    USHORT  usSendRequestTimeOut;               /* no. of send requests that timed out without ack being returned */
    USHORT  usSendTargetBusy;                   /* no. of send requests that returned ST_TARGET_BUSY ack being returned */
    USHORT  usRecvTargetClear;                  /* no. of recv requests that were cleared from the receive queue */
    USHORT  usSendTargetClear;                  /* no. of send requests that returned ST_TARGET_CLEAR ack being returned */
    USHORT  usRecvPutOnQueue;                   /* no. of received messages put on the user's queue */
    USHORT  usRecvGivenNow;                     /* no. of received messages given to a pending receive and not put on queue */
    USHORT  usRecvBadSeqNoPort;                 /* no. of received messages that have a bad sequence number or wrong port */
    USHORT  usRecvTakenOffQueue;                /* no. of received messages taken off the user's queue (actual number given to user) */
    USHORT  usRecvOutOfResources;               /* no. of received messages received that could not be queue due to lack of resources */
    USHORT  usAckSentStatusBusy;                /* no. of Ack messages sent with a status of ST_TARGET_BUSY */
    USHORT  usRecvLostNetClosed;                /* no. of messages removed from queue due to Net Closed */
    USHORT  usRecvRequestWaitStart;             /* no. of message requests with no input waiting so put on wait queue */
    USHORT  usRecvMessageAlreadyExist;          /* no. of receive message requests with buffer already allocated and assigned */
    USHORT  usSendMessageAlreadyExist;          /* no. of send message requests with buffer already allocated and assigned */
    USHORT  usExeNetSendCount;                  /* no. of calls to function ExeNetSend() */
    USHORT  usExeNetReceiveCount;               /* no. of calls to function ExeNetReceive() */
    USHORT  usTimerCheckSkipped;                /* no. of times TimerProtocol() found pending receive and message on the receive queue */
    USHORT  usTimerSendWithRecvPending;         /* no. of times TimerProtocol() found pending send and message on the receive queue, Ack missed? */
} PIF_NETUSERTALLY;
// **  WARNING  **  Additions to the above structs may require a change to a static buffer in
//                  the function PifNetSystemInfo() that is used for the AC888 report.
//                  See comments for variable PifNetworkSystemInfoBuffer in PifNetSystemInfo().
//---------------------------------------------------------------------------------------------------------

/* function data type of Platform Interface */
typedef USHORT (PIFENTRY *PIFDISABLE)(VOID);
typedef VOID   (PIFENTRY *PIFRESTORE)(USHORT usFlags);

extern USHORT    usBMOption;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */


/*********************************
 *  Declare Function Prototypes  *
 *********************************/
/* Entry Point of Application */
VOID   PifMain(TCHAR *pVersion);
VOID   PifCheckDelayBalance(VOID);

/* Function of Platform Interface Initialization */
VOID   PifXinuInit(BOOL fOsType);

/* Function of Queue Initialization */
VOID   PifInitQueue(VOID);

/* Functions of Potable Platform Interface */
/* --- Task --- */
VOID   PIFENTRY PifLogExtended (USHORT usModuleId, USHORT usExceptionCode, char *szFilePath, ULONG ulLineNo);
#if 1
#if !defined(PifLog)
#define PifLog(usModuleId,usExceptionCode)  PifLogExtended (usModuleId, usExceptionCode, __FILE__ , __LINE__)
#endif
#else
VOID   PIFENTRY PifLog(USHORT usModuleId, USHORT usExceptionCode);
#endif
VOID   PIFENTRY PifTransactionLog(UCHAR *lpCondition, UCHAR *lpTransactionLog, UCHAR *lpFilename, ULONG ulLineNo);
VOID   PIFENTRY PifDebugDump (UCHAR *aszTag, UCHAR *aszFileName, int nLineNo, VOID *pStruct, int nSizeOf);
VOID   PIFENTRY PifLogAbort(UCHAR *lpCondition, UCHAR *lpFilename, UCHAR *lpFunctionname, ULONG ulLineNo);
ULONG  PIFENTRY PifLogNoAbort(UCHAR *lpCondition, UCHAR *lpFilename, UCHAR *lpFunctionname, ULONG ulLineNo);

VOID   PIFENTRY PifAbortExtended (USHORT usModuleId, USHORT usExceptionCode, char *szFilePath, ULONG ulLineNo);
#if 1
#if !defined(PifAbort)
#define PifAbort(usModuleId,usExceptionCode)  PifAbortExtended (usModuleId, usExceptionCode, __FILE__ , __LINE__)
#endif
#else
VOID   PIFENTRY PifAbort(USHORT usFaultModule, USHORT usFaultCode);
#endif
SHORT  PIFENTRY PifShutdown(VOID);
SHORT  PIFENTRY PifOpenSnapShotFile(CONST TCHAR *pszFileName, CHAR *pcSourcePath, int iLineNo);
SHORT  PIFENTRY PifWriteSnapShotFile(SHORT sIndex, CHAR *pszWriteLine);
SHORT  PIFENTRY PifCloseSnapShotFile(SHORT sIndex);

// Set/retrieve NHPOS main window handle within PIF subsystem
VOID PIFENTRY PifSetWindowHandle (void *pHwnd);
VOID PIFENTRY *PifGetWindowHandle (void **ppHwnd);
VOID PIFENTRY PifSnapShotTables (VOID);

ULONG PIFENTRY PifBeginThread(VOID (THREADENTRY *pStart)(VOID),
                               VOID  *pStack, USHORT usStackSize,
                               USHORT usPriority,
                               CONST CHAR  *pszThreadName, ...);
VOID   PIFENTRY PifEndThread(VOID);

/* the following functions not defined in the Recovery Manager */
USHORT CONST * PIFENTRY PifGetProcessId(VOID); /* for the Recovery Manager */
ULONG PIFENTRY PifGetThreadId(VOID);     /* for the users */

/* --- Semaphore --- */
USHORT PIFENTRY PifCreateSem(USHORT usCount);
VOID   PIFENTRY PifDeleteSem(USHORT usSem);
SHORT  PIFENTRY PifRequestSem(USHORT usSem);
SHORT  PIFENTRY PifRequestSemNew(USHORT usSem, char *aszFilePath, int iLineNo);
#define PifRequestSemNew_debug 1
#if PifRequestSemNew_debug
#define PifRequestSem(usSem)  PifRequestSemNew(usSem,__FILE__,__LINE__)
#endif
SHORT  PIFENTRY PifReleaseSem(USHORT usSem);
SHORT  PIFENTRY PifReleaseSemNew(USHORT usSem, char *aszFilePath, int iLineNo);
#define PifReleaseSemNew_debug 1
#if PifReleaseSemNew_debug
#define PifReleaseSem(usSem)  PifReleaseSemNew(usSem,__FILE__,__LINE__)
#endif
SHORT  PIFENTRY PifRetrieveValueSem(USHORT usSem);
USHORT PIFENTRY PifCreateSemNew(USHORT usCount, USHORT usInit, CHAR *pcFilePath, int iLineNo);

/* --- Queue --- */
USHORT PIFENTRY PifCreateQueue(USHORT usCount);
VOID   PIFENTRY PifDeleteQueue(USHORT usQueue);
VOID   PIFENTRY PifWriteQueue(USHORT usQueue, CONST VOID *pMessage);
VOID * PIFENTRY PifReadQueue(USHORT usQueue);

/* --- Time --- */
VOID   PIFENTRY PifSleep(USHORT usMsec);
VOID   PIFENTRY PifGetDateTime(DATE_TIME *pDateTime);
VOID   PIFENTRY PifSetDateTime(CONST DATE_TIME *pDateTime);

/* --- Get elapsed time since system was started (DollarTree SCER 2000-11-17) ---*/
VOID   PIFENTRY PifGetTickCount( ULONG * pulElapsedTimeInMsec );

/* --- File --- */
/* See the function dfckmd() in piffile.c which is used to decode the file mode and also specifies the destination folder.
 * See the function RmtCheckOpenMode() in isprmt.c which is used to decode the file mode and also specifies the destination folder.
 *    i -> Icon folder for bitmaps used with window controls buttons, labels, etc.
 *    p -> Print folder for files to print to receipt printer
 *    s -> Saved Totals folder in the standard Database folder
 *    c -> Configuration folder (used primarily for Amtrak)
 *    h -> Historical folder for destination of reports (used primarily for cloud interface and transfer)
*/
SHORT  PIFENTRY PifGetFileAttribs(CONST TCHAR *pwszFileName, CONST CHAR *pszMode, VOID *pFileAttribs);
CONST TCHAR *  PIFENTRY PifGetFilePath(CONST TCHAR *pwszFileName, CONST CHAR *pszMode, TCHAR *pwszFilePath);
SHORT  PIFENTRY PifOpenFileNew(CONST TCHAR  *pszFileName, CONST CHAR  *pszMode, CHAR *pPath, int iLineNo);
SHORT  PIFENTRY PifReadFileNew(USHORT usFile, VOID *pBuffer, ULONG ulBytes, ULONG *pulBytesRead, CHAR *pcFilePath, int iLineNo);
VOID   PIFENTRY PifWriteFileNew(USHORT usFile, CONST VOID *pBuffer, ULONG usBytes, CHAR *pcFilePath, int iLineNo);
USHORT  PIFENTRY PifFileToFile(SHORT sDestFileHandle, SHORT sSrcFileHandle, ULONG ulDestOffset, ULONG ulSrcOffset, ULONG ulSrcFileLen); //US CUSTOMS
SHORT  PIFENTRY PifSeekFileNew(USHORT usFile, ULONG ulPosition, ULONG *pulActualPosition, CHAR *pcFilePath, int iLineNo);
SHORT PIFENTRY PifSetEndOfFile(USHORT usFile);
SHORT  PIFENTRY PifSeekWriteFileNew(USHORT usFile, ULONG ulPosition, CONST VOID *pBuffer, ULONG ulBytes, CHAR *pcFilePath, int iLineNo);
VOID   PIFENTRY PifCloseFile(USHORT usFile);
VOID   PIFENTRY PifDeleteFile(CONST TCHAR *pszFileName);
VOID  PIFENTRY PifDeleteFileNew(CONST TCHAR *pszFileName, CONST CHAR *pszMode);

/* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 --- */
VOID   PIFENTRY PifDeleteFileEx(CONST TCHAR *pszFileName, BOOL fTemporayDisk);
ULONG  PIFENTRY PifMoveFile(CONST TCHAR *szSrcFileName, BOOL fSrcTempDisk, CONST TCHAR *szDstFileName, BOOL fDstTempDisk);
ULONG  PIFENTRY PifGetFileSize(USHORT usFile, ULONG * pulFileSizeHigh);
USHORT PIFENTRY PifWriteFileEx(USHORT usFile, CONST VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifGetFileSizeEx(CONST TCHAR *szFileName, BOOL fTemporayDisk, ULONG *pulFileSizeHigh, ULONG *pulFileSizeLow);
SHORT  PIFENTRY PifGetDiskFreeSpace(BOOL fTemporayDisk,
                ULONG *pulFreeSizeHigh,  ULONG *pulFreeSizeLow,
                ULONG *pulTotalDiskHigh, ULONG *pulTotalDiskLow,
                ULONG *pulFreeDiskHigh,  ULONG *pulFreeDiskLow );

SHORT  PIFENTRY PifDebugFileCleanUp(VOID);

SHORT  PIFENTRY PifOpenTimeTracker(VOID);
USHORT PIFENTRY PifStartTimeTracker(USHORT usTimeIndex);
USHORT PIFENTRY PifEndTimeTracker(USHORT usTimeIndex);
ULONG  PIFENTRY PifTimeTrackerGetDiff(USHORT usTimeIndex);
USHORT PIFENTRY PifCloseTimeTracker(USHORT usTimeIndex);
/* --- END:   Add for Dollar Tree SCER 12/05/2000 --- */

/* --- Serial I/O --- */
SHORT  PIFENTRY PifOpenCom(USHORT usPortId, CONST PROTOCOL *pProtocol);
SHORT  PIFENTRY PifReadCom(USHORT usPort, VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifWriteCom(USHORT usPort, CONST VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifControlCom(USHORT usPort, USHORT usFunc, ...);
VOID   PIFENTRY PifCloseCom(USHORT usPort);
VOID*  PIFENTRY PifGetComRowHandle(USHORT usPort);

/* --- Network --- */
SHORT  PIFENTRY PifNetDiscoverNet (VOID);
SHORT  PIFENTRY PifNetOpen(CONST XGHEADER *pHeader);
SHORT  PIFENTRY PifNetOpenEx(CONST XGHEADER *pHeader, USHORT fsMode);
SHORT  PIFENTRY PifNetSend(USHORT usNet, CONST VOID *pBuffer, ULONG ulBytes);
SHORT  PIFENTRY PifNetReceive(USHORT usNet, VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifNetControl(USHORT usNet, USHORT usFunc, ...);
VOID   PIFENTRY PifNetClose(USHORT usNet);
SHORT  PIFENTRY PifNetSystemInfo (PIF_NETSYSTALLY *pNetTally, PIF_NETUSERTALLY *pNetUsers, USHORT *pusUsers);
SHORT  PIFENTRY PifNetAddJoin (PIF_CLUSTER_HOST  *pHostInfo, UCHAR *pauchTerminalNo);
SHORT  PIFENTRY PifNetRemoveJoin (TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo);
SHORT  PIFENTRY PifNetUpdateMasterBackup (TCHAR *pwszMasName, UCHAR *pauchMasIpAddress, TCHAR *pwszBMasName, UCHAR *pauchBMasIpAddress);

// Communications port and network port interface functions that allow use of either a COM port or UDP port or TCP port
// for communication with a device. Used with kitchen printer to allow LAN remote kitchen printer.
SHORT  PIFENTRY PifOpenNetComIoEx(USHORT usPortId, CONST PROTOCOL *pProtocol);
SHORT  PIFENTRY PifReadNetComIoEx(USHORT usPort, VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifWriteNetComIoEx(USHORT usPort, CONST VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifControlNetComIoEx(USHORT usPort, USHORT usFunc, ...);     // uses PifNetControl() argument standard, not PifControlCom() standard.
VOID   PIFENTRY PifCloseNetComIoEx(USHORT usPort);

/* --- I/O --- */
VOID   PIFENTRY PifGetChar(CHARDATA *pInputChar);
VOID   PIFENTRY PifControlKeyboard(USHORT usFunc);
UCHAR  PIFENTRY PifGetKeyLock(VOID);
SHORT  PIFENTRY PifGetWaiter(VOID);

VOID   PIFENTRY PifDisplayString(USHORT usDispId, USHORT usRow,
                                 USHORT usColumn, CONST TCHAR *puchString,
                                 USHORT usChars, UCHAR uchAttr);
VOID   PIFENTRY PifDisplayAttr(USHORT usDispId, USHORT usRow, USHORT usColumn, CONST TCHAR *puchString, USHORT usChars);
VOID   PIFENTRY PifLightDescr(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr);

USHORT PIFENTRY PifOpenDrawer(USHORT usDrawerId);
USHORT PIFENTRY PifDrawerStatus(USHORT usDrawerId);

VOID   PIFENTRY PifBeep(USHORT usMsec);
VOID   PIFENTRY PifPlayStanza(CONST STANZA *pStanza);
VOID   PIFENTRY PifToneVolume(USHORT usVolume);

SYSCONFIG CONST * PIFENTRY PifSysConfig(VOID);

USHORT PIFENTRY PifDisable(VOID); /* for the Recovery Manager */
VOID   PIFENTRY PifRestore(USHORT usFlags); /* for the Recovery Manager */

VOID   PIFENTRY PifTrackKey(UCHAR uchData);

SHORT  PIFENTRY PifReadMsr(MSR_BUFFER *pMsrBuffer);

SHORT  PIFENTRY PifLoadFarData(VOID *pAddress, ULONG usSize, TCHAR *pVersion);
VOID   PIFENTRY PifSaveFarData(VOID);

SHORT  PIFENTRY PifControl(USHORT usFunc, VOID *pArgList);

VOID   PIFENTRY PifRestart(USHORT usLoadMode);

USHORT PIFENTRY PifTurnOffPower(VOID);
USHORT PIFENTRY PifTurnOffPowerEx(USHORT usLock);

SHORT  PIFENTRY PifOpenMsr(VOID);
VOID   PIFENTRY PifCloseMsr(VOID);

/* loggign function */
VOID   CDECL PifSubLogging(USHORT usFuncId);

/* --- Viode I/O --- */
VOID    PIFENTRY PifVioStrAttr(USHORT usRow, USHORT usColumn, CONST UCHAR *puchString, USHORT usChars, USHORT usPage);
VOID    PIFENTRY PifVioStr(USHORT usRow, USHORT usColumn, CONST UCHAR *puchString, USHORT usChars, UCHAR  uchAttr, USHORT usPage);
VOID    PIFENTRY PifVioCurOnOff(USHORT usControl);
VOID    PIFENTRY PifVioCurType(UCHAR uchTop, UCHAR uchBottom);
VOID    PIFENTRY PifVioGetCurPos(USHORT *pusRow, USHORT *pusColumn, USHORT usPage);
VOID    PIFENTRY PifVioSetCurPos(USHORT usRow, USHORT usColumn, USHORT usPage);
VOID    PIFENTRY PifVioScrollUp(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow, USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr);
VOID    PIFENTRY PifVioScrollDown(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow, USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr);
VOID    PIFENTRY PifVioMode(USHORT usMode);
SHORT   PIFENTRY PifVioBackLight(USHORT usControl);
VOID    PIFENTRY PifVioActivePage(USHORT usPage);
VOID    PIFENTRY PifVioLoadCG(UCHAR *puchFonts, UCHAR uchBytes);
                             
VOID   PIFENTRY PifPlayStanza2(USHORT usFile, USHORT usLoop);

ULONG PIFENTRY PifGetThreadHandle(ULONG ulThreadId);
VOID  PIFENTRY  PifClearTaskTable(ULONG ulThreadId);

VOID PIFENTRY PifCloseFileAll(VOID);
SHORT PIFENTRY PifSetModemStrings(CONST TCHAR *pszModemString);

PifSemHandle PIFENTRY PifCreatePowerSem(VOID);
USHORT PIFENTRY PifRequestPowerSem(USHORT usSem);
VOID PIFENTRY PifReleasePowerSem(USHORT usSem);
VOID PIFENTRY PifDeletePowerSem(USHORT usSem);

SHORT PIFENTRY PifSetKeyboardType (USHORT usKeyBoardType);

SHORT  PIFENTRY PifOverrideHostName(TCHAR *pszHostName);
SHORT  PIFENTRY PifGetLocalHostName(TCHAR *pszHostName);
UCHAR * PIFENTRY PifGetHostAddrByName (CONST TCHAR *pszHostName, UCHAR *puchHostAddr);
SHORT  PIFENTRY PifGetHostsAddress(CONST TCHAR  *pszHostName, CHAR  *pszIPAddress);
SHORT  PIFENTRY PifSetHostsAddress(CONST TCHAR  *pszHostName, CONST CHAR  *pszIPAddress);
VOID   PIFENTRY PifRemoveHostsAddress(CONST TCHAR *pszHostName);
SHORT  PIFENTRY PifSetServicesPortNo(USHORT usPortNo);

SHORT PIFENTRY PifGetActiveDevice(USHORT usDeviceId, USHORT usPortNo, TCHAR  *pszDeviceName, TCHAR  *pszDllFileName);
SHORT PIFENTRY PifGetSystemVersion(TCHAR  *pszVersionName);
SHORT PIFENTRY PifGetVersion(TCHAR  *pszVersionName);

VOID PIFENTRY PifCreateAbortSem(VOID);
VOID PIFENTRY PifRequestAbortSem(TCHAR *pszDisplay);
VOID PIFENTRY PifReleaseAbortSem(VOID);
VOID PIFENTRY PifDeleteAbortSem(VOID);
VOID PIFENTRY PifHighLowSem(USHORT usSem, ULONG ulSemFlags, USHORT usMaxWaitingThreads, USHORT usMaxUnusedCount);
VOID PIFENTRY PifSetTimeoutSem(USHORT usSem, USHORT usTimeoutCount);

VOID PIFENTRY PifShutDownApp(VOID);						//JHHJ PifShutDownAPP Shuts down App

USHORT  PIFENTRY PifControlPowerSwitch(USHORT usFunc);  /* V1.0.04 */


SHORT PifFileMigration(VOID *pData, ULONG ulData, TCHAR *pVersion);
SHORT PifFileMigrationGP41(VOID *pData, TCHAR *pVersion);

/*****************
 *  End of PIF.H *
 *****************/
#ifdef __cplusplus
}
#endif

// Following are also defined in file pif.h for those files which do not include ecr.h

#if !defined(PifReadFile)
#define PifReadFile(usFile,pBuffer,ulBytes,pulBytesRead) PifReadFileNew((usFile),(pBuffer),(ulBytes),(pulBytesRead), __FILE__, __LINE__)
#endif

#if !defined(PifWriteFile)
#define PifWriteFile(usFile,pBuffer,ulBytes) PifWriteFileNew((usFile),(pBuffer),(ulBytes),__FILE__,__LINE__)
#endif

#if !defined(PifSeekWriteFile)
#define PifSeekWriteFile(usFile, ulPosition, pBuffer, ulBytes) PifSeekWriteFileNew((usFile),(ulPosition),(pBuffer),(ulBytes), __FILE__, __LINE__)
#endif

#if !defined(PifSeekFile)
#define PifSeekFile(usFile,ulPosition,pulActualPosition)  PifSeekFileNew((usFile),(ulPosition),(pulActualPosition), __FILE__, __LINE__)
#endif

#if !defined(PifCreateSem)
#define PifCreateSem(usCount)  PifCreateSemNew((usCount), (usCount), __FILE__, __LINE__)
#endif

#if !defined(PifOpenFile)
#define PifOpenFile(pszFileName,pszMode)  PifOpenFileNew((pszFileName),(pszMode), __FILE__, __LINE__)
#endif

#endif                                      /* _INC_PIF */
