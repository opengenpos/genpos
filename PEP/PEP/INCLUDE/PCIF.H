/*
*===========================================================================
* Title       : Header File of PC Interface
* Category    : PC Interface, NCR 2170 DLL for STUBWIN
* Program Name: PCIF.H
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Oct-26-92:00.00.01:O.Nakada   : Initial
*          :        :           :
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
*===========================================================================
*   Define Declarations 
*===========================================================================
*/

#define PCIF_FUNC_OPEN_LAN        1
#define PCIF_FUNC_OPEN_LAN_PORT   2

#define PCIF_FUNC_CLOSE_LAN      11

/*
*===========================================================================
*   Serial Port Status
*===========================================================================
*/
#define PCIF_SUCCESS                0
#define PCIF_ERROR                  (-1)
#define PCIF_ALREADY                (-2)
#define PCIF_OPEN                   (-3)
#define PCIF_QUEUE                  (-4)
#define PCIF_HARDWARE               (-10)
#define PCIF_BAUDRATE               (-12)
#define PCIF_CONTROL                (-50)
#define PCIF_NET_SOCK_ERR           (-51)  // PIF_ERROR_NET_ERRORS
#define PCIF_NET_MEMORY_ERR         (-52)  // PIF_ERROR_NET_MEMORY
#define PCIF_NET_BIND_ERR           (-53)  // PIF_ERROR_NET_BIND_ERR
#define PCIF_NET_NO_NETORK          (-54)  // PIF_ERROR_NET_NOT_PROVIDED
#define PCIF_NET_SETUP_PARMS        (-55)  // PIF_ERROR_NET_BAD_DATA


/*
*===========================================================================
*   Result Code of Modem Strings
*===========================================================================
*/
#define PCIF_RESULT_OK              0
#define PCIF_RESULT_CONNECT         1
#define PCIF_RESULT_RING            2
#define PCIF_RESULT_NO_CARRIER      3
#define PCIF_RESULT_ERROR           4
#define PCIF_RESULT_NO_DIAL_TONE    6
#define PCIF_RESULT_BUSY            7
#define PCIF_RESULT_ANSWER          8
#define PCIF_RESULT_HAND_SET        9


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Serial Port Format
*===========================================================================
*/
#pragma pack(push, 1)
typedef struct {
    UCHAR   uchPort;                        /* Port # */
    USHORT  usBaudRate;                     /* BaudRate */
    UCHAR   uchByteSize;                    /* Byte Size of Data */
    UCHAR   uchParity;                      /* Parity Bits */
    UCHAR   uchStopBits;                    /* Stop Bits */
} COMCONTROL;
#pragma pack(pop)


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT STUBENTRY PcifOpen(COMCONTROL FAR *pData);
SHORT STUBENTRY PcifClose(VOID);
SHORT STUBENTRY PcifConnect(UCHAR FAR *pData);
SHORT STUBENTRY PcifDisconnect(VOID);

SHORT STUBENTRY PcifOpenEx(USHORT usFunc, VOID FAR *pData);
SHORT STUBENTRY PcifCloseEx(USHORT usFunc, VOID FAR *pData);
