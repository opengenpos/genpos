/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Enhanced Kitchen Display System, Header File
* Category    : Enhanced Kitchen Display System, NCR 2170 HOSP US ENH. Application
* Program Name: ENHKDS.H
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
*                                       
* Date      Ver.        Name            Description
* Oct-22-99 00.00.01    M.Ozawa         Initial
* Nov-19-03 02.00.00    R.Herrington    added new defines, change in protocol
* Mar-31-15 02.02.01    R.Chambers      updated comments.
*
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
#define KDS_PERFORM         0
#define KDS_FAIL          -30
#define KDS_BUSY          -31

#define KDS_STX                 0x02
#define KDS_ETX                 0x03
#define KDS_ENQ                 0x05
#define KDS_ACK                 0x06

#define KDS_MIT_LEN         4
#define KDS_SMI_LEN         12
#define KDS_MSN_LEN         5
#define KDS_TERM_LEN        3
#define KDS_INF_IP_LEN      12
#define KDS_PORT_LEN        4
#define KDS_LINE_LEN        3
#define KDS_LMT_LEN         2
#define KDS_ST1_LEN         2
#define KDS_ST2_LEN         25
#define KDS_QTY_LEN         3
#define KDS_PLU_LEN         STD_PLU_NUMBER_LEN
#define KDS_ROUTE_LEN       4   //RPH 11-19-03
//#define KDS_ROUTE_LEN       3   /* by FVT */
/* #define KDS_ROUTE_LEN       2 */
#define KDS_OPEID_LEN       3

#define KDS_LANGID_LEN		4 //RPH 11-19-03
#define KDS_SUBLANGID_LEN	4 //RPH 11-19-03
#define KDS_CODEPG_LEN		4 //RPH 11-19-03

#define KDS_LINE_DATA_LEN   400
#define KDS_I_TOTAL_LEN     7
#define KDS_T_TOTAL_LEN     7
#define KDS_T_TAX_LEN       7

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/* Item Module I/F */

// The following struct is an intermediate data struct used by the Kitchen Display System
// functionality accessed through function ItemSendKds() to convey to the actual KDS interface
// functionality accessed through function KdsSendData() the item information to be displayed
// on one or more KDS CRTs.  They are called KDS CRTs in various places though these days, 2015,
// they are more like LCD displays instead.
typedef struct {
    USHORT  usLineNum;
    UCHAR   uchTransMode1;             // transaction mode identifier determined by ItemSendKdsSetTransMode1()
    UCHAR   uchTransMode2;             // transaction mode identifier determined by ItemSendKdsSetTransMode2()
    USHORT  usLineMainType;
    USHORT  usSubType1;                // identifies the type of item and data being sent to the KDS
    ULONG   ulSubType2;                // actual data for the type of item being sent to the KDS
    UCHAR   auchSubType2[KDS_ST2_LEN];
    SHORT   sQty;
    TCHAR   aszPluNo[KDS_PLU_LEN];
    ULONG   ulOperatorId;              // operator id of the current operator as determined by ItemSendKdsSetOpeId()
    TCHAR   aszLineData[KDS_LINE_DATA_LEN];
    LONG    lItemTotal;
    LONG    lTransTotal;
    LONG    lTransTax;
    UCHAR   auchCRTNo[KDS_ROUTE_LEN];  // which CRTs (1 - 4 in auchCRTNo[0], 5-8 in auchCRTNo[1]) are to be updated by this KDS message
	ULONG	ulForeGroundColor;
	ULONG	ulBackGroundColor;
	UCHAR	uchOrderDec;
} KDSSENDDATA;

/* KDS I/F */
/*   This struct is used with the Select Electronics kitchen display system
*    using the Oasys iPAD with bump bar and the ROD Gold software server interface.
*
*    WARNING: Provisioning of the ROD Gold software using the ROD Gold Maintenance utility
*             must be correct so far as IP addresses and options.
*
*             Important: in ROD Global settings the Enhanced Record Format check box
*                        must be checked in order to use this new KDS record format.
*                        The System Type must be NCR NHPOS (GenPOS was previously NHPOS).
*
*                        In addition, KDS provisioning data in P51 Assignment IP Address for KDS
*                        needs to provide the IP address and port number of the ROD Gold server
*                        and the port used by GenPOS for the ROD Gold to send messages to GenPOS.
*/
typedef struct {
    UCHAR   uchSTX;
    UCHAR   auchMsgIndType[KDS_MIT_LEN];
    UCHAR   uchMsgType;
    UCHAR   auchSubMsgInfo[KDS_SMI_LEN];
    UCHAR   auchMsgSeqNo[KDS_MSN_LEN];
    UCHAR   auchTerminal[KDS_TERM_LEN];
    UCHAR   auchIPAddr[KDS_INF_IP_LEN];
    UCHAR   auchPort[KDS_PORT_LEN];			/* Port Address to talk to NHPOS from KDS */
    UCHAR   aszLineNum[KDS_LINE_LEN];
    UCHAR   szTransMode1;
    UCHAR   szTransMode2;
    UCHAR   aszLineMainType[KDS_LMT_LEN];
    UCHAR   aszSubType1[KDS_ST1_LEN];
    UCHAR   aszSubType2[KDS_ST2_LEN];
    UCHAR   aszQty[KDS_QTY_LEN];
    UCHAR   aszPluNo[KDS_PLU_LEN];
    UCHAR   aszRouting[KDS_ROUTE_LEN];
    UCHAR   aszOperatorId[KDS_OPEID_LEN];
	UCHAR	auchLanguageId[KDS_LANGID_LEN];//RPH 11-19-03
	UCHAR	auchSubLanguageId[KDS_SUBLANGID_LEN];//RPH 11-19-03
	UCHAR	auchCodePage[KDS_CODEPG_LEN];//RPH 11-19-03
    TCHAR   aszLineData[KDS_LINE_DATA_LEN]; //Actual Displayed Data
    UCHAR   aszItemTotal[KDS_I_TOTAL_LEN];
    UCHAR   aszTransTotal[KDS_T_TOTAL_LEN];
    UCHAR   aszTransTax[KDS_T_TAX_LEN];
	ULONG	ulForegroundColor;//Item foreground
	ULONG	ulBackgroundColor;//Item background
	UCHAR	uchPluStatus[4]; //Status Bytes for PLU
	UCHAR	uchOrderDec;//Order Declaration Mode
	UCHAR	auchDummy[68];//future use, end at byte 104 so that version no. starts byte 105, Select is looking for a 1025 byte message.
	UCHAR   auschMajorVersionNo[2];  // 2 digit major version number, see KdsSendProcess()
	UCHAR   auschMinorVersionNo[2];  // 2 digit minor version number, see KdsSendProcess()
	UCHAR   auschIssueVersionNo[2];  // 2 digit issue version number, see KdsSendProcess()
	UCHAR   auschBuildVersionNo[3];  // 3 digit build number, see KdsSendProcess()
    UCHAR   uchETX;
    UCHAR   uchCRC;
} KDSINF;


/* KDS Response I/F */

typedef struct {
    UCHAR   uchSTX;
    UCHAR   auchMsgIndType[KDS_MIT_LEN];
    UCHAR   uchMsgType;
    UCHAR   auchSubMsgInfo[KDS_SMI_LEN];
    UCHAR   auchMsgSeqNo[KDS_MSN_LEN];
    UCHAR   auchTerminal[KDS_TERM_LEN];
    UCHAR   auchIPAddr[KDS_INF_IP_LEN];
    UCHAR   aszLineNum[KDS_LINE_LEN];
    UCHAR   uchETX;
    UCHAR   uchCRC;
} KDSRESINF;



#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
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
SHORT KdsSendData(KDSSENDDATA *pData);
VOID  KdsInit(VOID);
VOID   KdsOpen(VOID);
VOID   KdsClose(VOID);

/* ---------- End of DFL.H ---------- */