/*
*===========================================================================
* Title       : EPT MODE HEADER FILE
* Category    : EPT MODE Application Program - US Hospitality Application
* Program Name: EEPT.H
* --------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
* Date         Ver.        Name            Description
* Nov-21-96 : 02.00.00 : hrkato        : R2.0
* Dec-08-99 : 01.00.00 : hrkato        : Saratoga
*** GenPOS ***
* Jun-17-15 : 02.02.01 : R.Chambers    : added define EEPT_RET_SERVICE_ERROR, fixed up comments.
*/

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/

#if !defined(EEPT_H_INCLUDED)
#define EEPT_H_INCLUDED

/* tally required type,	Saratoga */
#define EPT_TALLY_READ                  1       /* read tally */
#define EPT_TALLY_RESET                 2       /* read & reset tally */

#define EPT_SUCCESS                     0x00    /* success */

// Special values used in message fields
#define EEPT_STX                        0x02    /* start text */
#define EEPT_ETX                        0x03    /* end text */
#define EEPT_EOT                        0x04    /* end of text */
#define EEPT_ENQ                        0x05    /* enquiry */
#define EEPT_ACK                        0x06    /* acknowledge */
#define EEPT_NAK                        0x15    /* not acknowledge */

#define EEPT_COM_ERR                    (-31)   /* general comm. error for EPT */
#define EEPT_NOT_EPTPORT                (-36)   /* not exist EPT port */
#define EEPT_ABORTED                    (-37)   /* aborted by user for EPT */


/**     return value
		These return values, defines beginning with EEPT_RET should
		have the same values and meanings as do the CPM_RET values
		which are defined in the include file CPM.H so if values
		are added here, they should be added to CPM.H,CSSTBEPT.H and vice versa.
**/
#define     EEPT_RET_AUTO_STORED    (1)         //external server error and auto storing is in effect, same as CPM_RET_AUTO_STORED 
#define     EEPT_RET_SUCCESS         0          /* process succeed          */
#define		EEPT_RET_COM_ERRORS		( -10)      /* Error opening com port, same as CPM_RET_COM_ERRORS	 */
#define     EEPT_RET_NOT_MASTER     ( -20)      /* I'm not master (O/S), same as CPM_RET_NOT_MASTER     */
#define     EEPT_RET_FAIL           ( -31)      /* other communication err, same as CPM_RET_FAIL  */
#define     EEPT_RET_BUSY           ( -32)      /* EEPT is busy, same as CPM_RET_BUSY              */
#define     EEPT_RET_STOP           ( -33)      /* EEPT is stop process, same as CPM_RET_STOP      */
#define     EEPT_RET_OFFLINE        ( -34)      /* EEPT & PMS is offline, same as CPM_RET_OFFLINE     */
#define     EEPT_RET_LENGTH         ( -35)      /* receive length error, same as CPM_RET_LENGTH     */
#define     EEPT_RET_PORT_NONE      ( -36)      /* PMS is not provide, same as CPM_RET_PORT_NONE        */
#define     EEPT_RET_TIMEOUT        ( -37)      /* time out error, same as CPM_RET_TIMEOUT   */
#define     EEPT_RET_BADPROV        ( -38)      /* equipment provisioning wrong, same as CPM_RET_BADPROV */
#define     EEPT_RET_CANCELLED      ( -39)      /* operation cancelled, same as CPM_RET_CANCELLED  */
#define     EEPT_RET_SERVERCOMMS    ( -40)      /* server communication issue or TCP/IP issue, same as CPM_RET_SERVERCOMMS  */
#define     EEPT_RET_INVALIDDATA    ( -41)      /* invalid data such as acct no, etc sent, same as CPM_RET_INVALIDDATA  */
#define		EEPT_RET_PINNEEDED		( -42)      // Pin is needed for a manual EBT, same as CPM_RET_PINNEEDED
#define     EEPT_RET_SERVICE_ERROR  ( -43)      /* service error due to some platform or infrastructure problem such as missing Datacap control, same as CPM_RET_SERVICE_ERROR  */


// See EptErrorCodes EptErrorCodeTable[] error code translation table in file uieerror.c
#define     EEPT_RET_DSI_FAILED     (-132)          //Initialization of DSI Client control failed
#define     EEPT_RET_INVALIDTYPE    (-146)          //Invalid Transaction Type
#define     EEPT_RET_INVALIDCODE    (-164)          //Invalid Transaction Code
#define     EEPT_RET_VOID_RETURN    (-216)          //return error VoidReturn is only allowed for positive amounts
#define     EEPT_RET_EXTRNL_SERVER  (-224)          //error, external server reports an error, EEPTINTERFACE_DATACAP_SERVER_ERROR

/**     status of EEPT  **/
#define     EEPT_CHANGE_STOP         0
#define     EEPT_CHANGE_START        1

/** tally required type **/
#define     EEPT_TALLY_READ          1           /* read tally               */
#define     EEPT_TALLY_RESET         2           /* read & reset tally       */

/* --- Function Code & Status Code --- */
#define     EEPT_FC2_EPT             0x1e        /* EPT (Enhance Type)       */
#define     EEPT_FC2_ASK             0x3a        /* Ask                      */
#define     EEPT_FC2_ACTION_EPT      0x3b        /* Ask for EPT action such as EMV Parameter Download. */

#define     EEPT_FC2_ACCEPTED        0x30        /* accepted                 */
#define     EEPT_FC2_REJECTED        0x31        /* rejected                 */
#define     EEPT_FC2_OVERRIDE        0x32        /* override required        */
#define     EEPT_FC2_NEEDGLINE       0x33        /* need guest line number   */
#define     EEPT_FC2_STOP_CHARG      0x35        /* stop Charge Post/Credit Auth.*/
#define     EEPT_FC2_STOP_POST       0x36        /* stop Posting                 */
#define     EEPT_FC2_START           0x37        /* start Charge Post/Credit Auth*/
#define     EEPT_FC2_PARTTEND        0x39        /* partial tender           */
#define     EEPT_FC2_DECLINED        0x3A        /* Request was Declined by card vendor */
#define     EEPT_FC2_UNSOLI          0x42        /* Unsolicited Message      */
#define     EEPT_FC2_TIMEOUT         0x43        // receive time out
#define     EEPT_FC2_ERROR           0x44        // receive an error from transport mechanism such as DSI Client
#define     EEPT_FC2_INVALID         0x45        // receive an invalid status from transport mechanism such as DSI Client
#define     EEPT_FC2_STORED          0x46        /* the transaction was stored  */

#define     EEPT_NORMALTYP           '0'         /* normal message type      */
#define     EEPT_OVERTYP             '1'         /* override request msg type*/

#define     EEPT_CONT_MSG            '0'         /* continue message         */
#define     EEPT_LAST_MSG            '1'         /* last message             */

/* --- Message Instruction (Response Message) --- */
#define     EEPT_RES_INST_NODISP     0x30        /* No Display               */
#define     EEPT_RES_INST_DISPMSG    0x31        /* Display Response Message */
#define     EEPT_RES_INST_DISPTEXT   0x32        /* Display Response Text    */
#define     EEPT_RES_INST_PRINT      0x33        /* Print Response Text      */
#define     EEPT_RES_INST_REQPIN     0x34        /* Request Pin Prior to retransmission of EEPT info */
#define		EEPT_INT_TO_ASC_HEX		 0x30		 //in InterpretXML, used to convert int value to string

// used for the block header to determine what the transaction has done
//  EEPT_BLOCK_HEADER  blockHeader.sProcessed
#define	NOT_PROCESSED			0
#define	PROCESSED				1
#define	PROCESSED_ERROR			3
#define PROCESSED_DECLINED		4
#define PROCESSED_HARD_ERROR	5    // hard error such as connectivity that can be retried

#define EEPT_SKIP_TRANS		  15
#define EEPT_DECLINED		  16
#define EEPT_ERROR			  17
#define EEPT_NOW_PROCESSED	  18      // retried a failed Forward attempt and it work so now processed.
#define EEPT_ERROR_DONOTHING  19
#define EEPT_HARD_ERROR       20    // hard error such as EEPTINTERFACE_SOCK_ERROR see pdata->lRawError for details

#define EEPT_APPROVED		0
#define RPT_SUMMARY			1
#define RPT_FAILED			3
/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/

#ifdef __cplusplus
extern "C"{
#endif

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#include "regstrct.h"

#define EEPTREQDATA_FLAGS_RETURN        0x01
#define EEPTREQDATA_FLAGS_VOIDRETURN    0x02
#define EEPTREQDATA_FLAGS_VOIDSALE      0x03

#define EEPTREQDATA_SPECIALFLAGS_ALLOWSTORE         0x01  // Allow storing if error in connectivity
#define EEPTREQDATA_CUSTOMERFLAGS_ENABLE_AMTRAK     0x02  // SYSCONFIG_CUSTOMER_ENABLE_AMTRAK set enabling Amtrak specific
#define EEPTREQDATA_SPECIALFLAGS_AUTODECLINE        0x04  // SYSCONFIG_CUSTOMER_ENABLE_AMTRAK set enabling Amtrak specific
#define EEPTREQDATA_SPECIALFLAGS_FORCESTORE         0x08  // Force storing whether connectivity exists or not
#define EEPTREQDATA_SPECIALFLAGS_MERCURYPAYTEST     0x10  // SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST, Trigger additional logic for Mercury Payments, now Vantiv, testing
#define EEPTREQDATA_SPECIALFLAGS_NO_PARTIAL_AUTH    0x20  // SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH, Turn off EPT Partial Authorization

typedef struct {
    UCHAR   uchSTX;                         /* STX Character        */
    UCHAR   auchSrcTerminal[2];             /* Source Terminal      */
    UCHAR   auchDesTerminal[2];             /* Destination Terminal */
    UCHAR   auchTenderID[2];                /* Tender ID            */
    UCHAR   auchDataSeqNo[3];               /* Data Sequence#       */
    UCHAR   auchFuncCode;                   /* Function Code        */
    UCHAR   auchTerminalNo;                 /* Terminal#            */
    UCHAR   auchSystemNo[4];                /* System#              */
    UCHAR   auchStoreNo[4];                 /* Store#               */
    UCHAR   auchHotelID[2];                 /* Hotel ID             */
    UCHAR   auchGuest[6];                   /* Guest Check No       */
    UCHAR   auchChargeTtl[10];              /* Charge Total         */
    UCHAR   auchTransNo[4];                 /* Consective#          */
    UCHAR   auchCPFlag;                     /* Charge Post Flag     */
    UCHAR   auchSLDNumber[4];               /* SLD#                 */
    UCHAR   auchMSRData[NUM_MSRDATA];                /* MSR Data             */
    UCHAR   auchExpDate[4];                 /* Expiration Date      */
    UCHAR   auchGuestLineNo[2];             /* Guest Line#          */
    UCHAR   auchCashierNo[10];               /* Checker#     STD_CASHIERNO_LEN        */
    UCHAR   auchChargeTip[10];              /* Charge Tip           */
    UCHAR   auchTtlTax[10];                 /* Total Tax Amount     */
    UCHAR   auchAcctNo[22];                 /* Account#    is right justified and filled with leading zeros  */
    UCHAR   auchActionCode[3];              /* Action Code for EPT  */
    UCHAR   auchDiscItemTtl1[10];           /* Discount Itemizer#1  */
    UCHAR   auchDiscItemTtl2[10];           /* Discount Itemizer#2  */
	UCHAR	auchDiscItemTtl3[STD_DISC_ITEMIZERS_MAX-2][10];
												/* ^ -2 necessary because the first 2 discount itemizers were
												already implemented into the structure JHHJ 3-29-04*/
    UCHAR   auchBonus1[10];                 /* Bonus Itemizer#1     */
    UCHAR   auchBonus2[10];                 /* Bonus Itemizer#2     */
    UCHAR   auchBonus3[10];                 /* Bonus Itemizer#3     */
    UCHAR   auchBonus4[10];                 /* Bonus Itemizer#4     */
    UCHAR   auchBonus5[10];                 /* Bonus Itemizer#5     */
    UCHAR   auchBonus6[10];                 /* Bonus Itemizer#6     */
    UCHAR   auchBonus7[10];                 /* Bonus Itemizer#7     */
    UCHAR   auchBonus8[10];                 /* Bonus Itemizer#8     */
    UCHAR	auchBonus9[12][10];				/* Space Holder for New Bonus Totals 8-<12 JHHJ  & RJC to 20 not 100 */
    UCHAR   auchFreq[10];                   /* Freq Shopper Itemizer*/
    UCHAR   auchFoodStamp[10];              /* Food Stamp Itemizer  */
    UCHAR   auchDebitCashBack[10];          /* Used for amount to specify for Cash Back for Debit and other EPT  */

    UCHAR   auchIpAddress[4][3];              /* IpAddress of Server  */
    UCHAR   auchIpPort[5];                  /* IpPort# of Server    */
    UCHAR   auchMerchantID[24];             /* Merchant ID          */
	UCHAR	auchTranType[2];				// Transaction Type Credit, Debit, etc
	UCHAR	auchTranCode[2];				// Transaction Code Sale, Return, Void, etc
    UCHAR   auchComPort[3];                 /* Com Port of Canadian Approved Pin Pad  */
    UCHAR	auchRefNo[NUM_REFERENCE];					/* Reference number returned in transaction response */
	UCHAR   auchDriversLicence[19];         /* DriversLicence #     */
    UCHAR   auchStateCode[2];               /* State Code of DL#    */
    UCHAR   auchDateOfBirth[6];             /* Date of Birth        */
    UCHAR   auchCheckSeqNo[20];             /* Check #              */
    UCHAR   auchCanadianSeqNo[20];          /* Sequence # from Canadian Debit    */
    UCHAR   auchAuthCode[NUM_AUTHCODE];               /* Authorization Code   */
    UCHAR   auchAcqRefData[40];             /* Acquirer Reference returned by processor on PreAuth Transaction requests      */
    UCHAR   auchCustomerCode[17];           /* For Purchase Card Level II transactions             */
	UCHAR	auchDecimal;					// Decimal places
	UCHAR	auchLanguageId[4];				// Language
	UCHAR	auchSubLanguageId[4];			// Sub Language ID
	UCHAR	auchCodePage[4];				// Code Page
	UCHAR	auchRoutingNumber[9];			// Bank Routing Number
	UCHAR	auchInvoiceNo[NUM_INVOICE_NUM];	// InvoiceNo supplied by request
	UCHAR	auchSpecialFlags[1];			/* Special flags that are used to indicate particular states */
	UCHAR	auchNotUsed[24];				/* Future Use Area		*/
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
	UCHAR	uchType;
	UCHAR   uchSpecialProcessingFlags;      // indicates if special process is needed or can be done.
	TCHAR	auchVersionNo[40];
	USHORT  usCheckTenderId;                // Unique tender id for split checks identifying specific tender
	USHORT  usReturnType;                   // the return type indicating Return, Exchange, Refund.
	UCHAR	auchPaymentType;				//used for determining a swipe or contactless transactions
	UCHAR   uchUniqueIdentifier[24];		// Unique identifer for returns as digits, binary coded decimal
	UCHAR	uchUniqueIdentifierReturn[24];  // Unique identifer for transaction being returned as digits, binary coded decimal
	TCHAR	aszCardHolder[NUM_CPRSPTEXT];   // card holder name
	TCHAR	aszCardLabel[NUM_CPRSPTEXT];    // 28:number, see also TRN_DSI_CARDTYPE and EEPTRSPDATA.aszCardLabel
	UCHAR   aszStoredRefNum[24];            // pseudo ref number for Stored credit tender transactions for Amtrak
    LONG	lTenderAmount;                  /* 12:tender amount */
    LONG	lGratuity;				        /* raw, unmodified tip amount, possibly negative for returned tips */
	ITEMDISC		      TranChargeTips;
	ITEMTENDERCHARGETIPS  ReturnsTenderChargeTips;
	ITEMTENDERSRNO        srno;             // RecordNo - from DSI Client XML.  record identifier in current batch.
} EEPTREQDATA;

typedef struct {
    LONG        lTransBonus[STD_NO_BONUS_TTL];    /* bonus total of transaction for GOV PAY and others */
	ITEMTENDER  eeptTender;
	ITEMDISC		      TranChargeTips;
	ITEMTENDERCHARGETIPS  ReturnsTenderChargeTips;
	UCHAR   uchSpecialProcessingFlags;      // indicates if special process is needed or can be done.
	UCHAR   uchUniqueIdentifier[24];		// Unique identifer for returns as digits, binary coded decimal
	UCHAR	uchUniqueIdentifierReturn[24];  // Unique identifer for transaction being returned as digits, binary coded decimal
	UCHAR   aszStoredRefNum[24];            // pseudo ref number for Stored credit tender transactions for Amtrak
} EEPTREQSTRUCT;

#define  EEPTREQUNIONTYPE_REQDATA     1
#define  EEPTREQUNIONTYPE_REQSTRUCT   2
typedef struct {
	USHORT usType;
	union {
		EEPTREQDATA    eeptReqData;
		EEPTREQSTRUCT  eeptReqStruct;
	} u;
} EEPTREQUNION;

typedef struct {
    UCHAR   uchSTX;                         /* STX Character        */
    UCHAR   auchSrcTerminal[2];             /* Source Terminal      */
    UCHAR   auchDesTerminal[2];             /* Destination Terminal */
    UCHAR   auchDataSeqNo[3];               /* Data Sequence#       */
    UCHAR   auchEptStatus;                  /* Status Code          */
    UCHAR   auchTerminalNo;                 /* Terminal#            */
    UCHAR   auchLastMsgID;                  /* Last Message ID      */
    UCHAR   auchMsgInstruction;             /* Message Instruction  */
    UCHAR   auchRspMsgNo[2];                /* Response Message#    */
    UCHAR   auchFolioNumber[6];             /* Folio#               */
    UCHAR   auchPostTransNo[6];             /* Posted Transaction# or NetePay batch record number */
    UCHAR   auchMsgRecpt;                   /* Message on Receipt   */
    UCHAR   auchMsgSlip;                    /* Message on Slip      */
    UCHAR   auchPrintLineNo[2];             /* Print Line#          */
    UCHAR   auchPrintSelect;                /* Print Selection      */
    UCHAR   auchPartTender[10];             /* Partial Tender Amount*/
    WCHAR   auchMsgText[NUM_MAX_CRSPCO_BUF]; /* Message Text with EPT response text  */

	UCHAR	auchAuthCode[NUM_AUTHCODE];			// Authorization Code From Processor for the Approved transaction
	UCHAR	auchInvoiceNo[NUM_INVOICE_NUM];		// InvoiceNo supplied by request
	UCHAR	auchRefNo[NUM_REFERENCE];			// Reference Number returned by the processor
	UCHAR	auchBalance[10];				// Balance Amount for EBT or PrePaid Transactions
	UCHAR	auchAcqRefData[40];				// Acquirer Reference retruned by processor on PreAuth
	UCHAR	auchPrePaidExp[8];				// PrePaid card expiration date or Trace Number
	UCHAR	auchBankRespCode[40];			// Bank Response code Canadian debit
	UCHAR	auchISORespCode[40];			// ISO Response code Canadian debit
	UCHAR	auchTranDate[12];				// Trans Date Canadian debit
	UCHAR	auchTranTime[10];				// Trans Time Canadian debit
	UCHAR	auchErrorNumber[6];				// Error Code Returned other than Approve, Decline, etc
	WCHAR	auchErrorText[40];				// TextResponse
	UCHAR	auchResOrigin[10];				// Origin of the Response
	UCHAR	auchLanguageId[4];				// Language
	UCHAR	auchSubLanguageId[4];			// Sub Language ID
	UCHAR	auchCodePage[4];				// Code Page
	UCHAR	auchRoutingNumber[9];			// Bank Routing Number
	UCHAR	auchNotUsed[41];				/* Future Use Area		*/
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
	UCHAR	auchAuthorize[10];				// Authorized Amount sstoltz*
	UCHAR	auchCashBack[10];				// Cash Back from the response
	UCHAR	auchGratuity[10];				// Gratuity from the response
	USHORT  usCheckTenderId;                // Unique tender id for split checks identifying specific tender
	USHORT  usReturnType;                   // the return type indicating Return, Exchange, Refund.
	UCHAR	auchStoreNo[4];                 // Store Number (Train Number) at the time the block was created.
	ITEMTENDERSRNO  srno;                   // RecordNo - from DSI Client XML.  record identifier in current batch.
	TCHAR	aszCardLabel[NUM_CPRSPTEXT];    // 28:number, see also TRN_DSI_CARDTYPE and EEPTRSPDATA.aszCardLabel
	TCHAR	auchCardType[12];				// Card Type Code From Processor for the Approved transaction
} EEPTRSPDATA;

typedef struct {
    UCHAR   uchSTX;                         /* STX Character        */
    UCHAR   auchSrcTerminal[2];             /* Source Terminal      */
    UCHAR   auchDesTerminal[2];             /* Destination Terminal */
    UCHAR   auchTenderID[2];                /* Tender ID            */
    UCHAR   auchDataSeqNo[3];               /* Data Sequence#       */
    UCHAR   auchFuncCode;                   /* Function Code        */
    UCHAR   auchNotUsed1[6];                /* Not Used             */
    UCHAR   auchTerminalNo;                 /* Terminal#            */
    UCHAR   auchSystemNo[4];                /* System#              */
    UCHAR   auchStoreNo[4];                 /* Store#               */
    UCHAR   auchHotelID[2];                 /* Hotel ID             */
    UCHAR   auchGuest[6];                   /* Guest Check No       */
    UCHAR   auchChargeTtl[10];              /* Charge Total         */
    UCHAR   auchTransNo[4];                 /* Consective#          */
    UCHAR   auchCPFlag;                     /* Charge Post Flag     */
    UCHAR   auchSLDNumber[4];               /* SLD#                 */
    UCHAR   auchNotUsed2[7];                /* Not Used              */
    UCHAR   auchMSRData[40];                /* MSR Data             */
    UCHAR   auchExpDate[4];                 /* Expiration Date      */
    UCHAR   auchNotUsed3[8];                /* Not Used             */
    UCHAR   auchGuestLineNo[2];             /* Guest Line#          */
    UCHAR   auchCashierNo[3];               /* Checker#             */
    UCHAR   auchNotUsed4[3];                /* Not Used             */
    UCHAR   auchChargeTip[10];              /* Charge Tip           */
    UCHAR   auchTtlTax[10];                 /* Total Tax Amount     */
    UCHAR   auchAcctNo[22];                 /* Account#             */
    UCHAR   auchActionCode[3];              /* Action Code for EPT  */
    UCHAR   auchNotUsed5[5];                /* Not Used             */
    UCHAR   auchDiscItemTtl1[10];           /* Discount Itemizer#1  */
    UCHAR   auchDiscItemTtl2[10];           /* Discount Itemizer#2  */
    UCHAR   auchNotUsed6[20];               /* Not Used             */
    UCHAR   auchNotUsed7[10];               /* Not Used             */
    UCHAR   auchNotUsed8[10];               /* Not Used             */
    UCHAR   auchBonus1[10];                 /* Bonus Itemizer#1     */
    UCHAR   auchBonus2[10];                 /* Bonus Itemizer#2     */
    UCHAR   auchBonus3[10];                 /* Bonus Itemizer#3     */
    UCHAR   auchBonus4[10];                 /* Bonus Itemizer#4     */
    UCHAR   auchBonus5[10];                 /* Bonus Itemizer#5     */
    UCHAR   auchBonus6[10];                 /* Bonus Itemizer#6     */
    UCHAR   auchBonus7[10];                 /* Bonus Itemizer#7     */
    UCHAR   auchBonus8[10];                 /* Bonus Itemizer#8     */
    UCHAR   auchFreq[10];                   /* Freq Shopper Itemizer*/
    UCHAR   auchFoodStamp[10];              /* Food Stamp Itemizer  */
    UCHAR   auchNotUsed9[10];               /* Not Used             */
    UCHAR   auchNotUsed10[20];              /* Not Used             */
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
} EEPTREQDATA_v14;

typedef struct {
    UCHAR   uchSTX;                         /* STX Character        */
    UCHAR   auchSrcTerminal[2];             /* Source Terminal      */
    UCHAR   auchDesTerminal[2];             /* Destination Terminal */
    UCHAR   auchNotUsed1[2];                /* Not Used             */
    UCHAR   auchDataSeqNo[3];               /* Data Sequence#       */
    UCHAR   auchStatus;                     /* Status Code          */
    UCHAR   auchNotUsed2[6];                /* Not Used             */
    UCHAR   auchTerminalNo;                 /* Terminal#            */
    UCHAR   auchLastMsgID;                  /* Last Message ID      */
    UCHAR   auchMsgInstruction;             /* Message Instruction  */
    UCHAR   auchRspMsgNo[2];                /* Response Message#    */
    UCHAR   auchFolioNumber[6];             /* Folio#               */
    UCHAR   auchPostTransNo[6];             /* Posted Transaction#  */
    UCHAR   auchMsgRecpt;                   /* Message on Receipt   */
    UCHAR   auchMsgSlip;                    /* Message on Slip      */
    UCHAR   auchPrintLineNo[2];             /* Print Line#          */
    UCHAR   auchPrintSelect;                /* Print Selection      */
    UCHAR   auchNotUsed3[2];                /* Not Used             */
    UCHAR   auchNotUsed4[20];               /* Not Used             */
    UCHAR   auchNotUsed5[20];               /* Not Used             */
    UCHAR   auchPartTender[10];             /* Partial Tender Amount*/
    UCHAR   auchMsgText[400];               /* Message Text         */
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
} EEPTRSPDATA_v14;

typedef struct {
	WCHAR	auchPinBlock[20];				// Pin Block
	WCHAR	auchDervdKey[20];				// Derived Key
	WCHAR	auchTrackNumber[37];			// ABA Track 2 Account Data
    UCHAR   auchStatus;                     // Status Code
	UCHAR	auchErrorNumber[6];				// Error Code Returned
	WCHAR	auchErrorText[40];				// TextResponse
} MERCURYEPTRSPDATA;

#define EEPT_MSG_TEXT   400                 /* Length of Message Text */

/* --- Saratoga */
typedef struct  {
    USHORT  usEPTSendOK;                 /* 1: send success in EPT */
    USHORT  usEPTSendErr;                /* 3: send error in EPT */
    USHORT  usEPTRespOK;                 /* 5: receive response success in EPT */
    USHORT  usEPTRespErr;                /* 7: receive response error in EPT */
}EPTTALLY;

//Store and Forward
//*****NEED TO MAKE SURE THAT BOTH EEPT_BLOCK_HEADER ARE THE SAME*******
#define EEPT_BLOCK_FLAGS_PROCESSED     0x00000001      // used with stored Preauth to indicate stored Preauth Capture needed
#define EEPT_BLOCK_FLAGS_PREAUTH_ERR   0x00000002      // used with stored Preauth Capture to indicate corresponding Preauth was Declined or had an error
typedef struct{
		ULONG			ulDataLength;
		ULONG			ulCurrentBlockOffset;       // updated when block is read to provide current position.
		ULONG			ulBlockFlags;               // flags for the Preauth process and other state driven procedures
		ULONG			ulCashierID;
		USHORT			usGuestNo;                  // guest check No of the transaction
		USHORT			usConsNo;                   // consecutive No of the transaction
		USHORT			usSeatNo;                   // seat No of the transaction
        LONG			lChargeTtl;                 /* 12:tender amount */
        LONG			lChargeTip;                 /* modified tip amount, always positive */
        LONG			lTenderAmount;              /* raw, tender amount */
        LONG			lGratuity;                  /* raw, unmodified tip amount, possibly negative for returned tips */
		LONG			lAttempts;
		LONG			lTotal;
		LONG            lRawErrorCode;              // error code from the last DSI Client response message
		SHORT			sErrorCode;
		SHORT			sProcessed;
		SHORT			TenderNo;
		USHORT          usEeptreqdata_Flag;
		TCHAR			TextResp[40];
		UCHAR			auchStoreNo[4];             // Store Number (Train Number) at the time the block was created.
		UCHAR			AcctNo[26];
		DATE_TIME		DateTime;
		ITEMDISC		      TranChargeTips;
		ITEMTENDERCHARGETIPS  ReturnsTenderChargeTips;
		UCHAR			uchPaymentType;					//used for contactless or swipe types of transactions
		UCHAR			uchSpecialProcessingFlags;      // indicates if special process is needed or can be done.
		UCHAR           auchAuthorize[10];
        UCHAR			aszPostTransNo[NUM_POSTTRAN];   /* 68:posted transaction number */
		UCHAR			auchAcqRefData[NUM_ACQREFDATA];	// XEPT Acquirer Reference Number - used for pre-authorization
		UCHAR			uchUniqueIdentifier[24];        // Unique identifer for returns as digits, binary coded decimal
		UCHAR			uchUniqueIdentifierReturn[24];  // Unique identifer for transaction being returned as digits, binary coded decimal
		USHORT			usCheckTenderId;                // Unique tender id for split checks identifying specific tender
		USHORT          usReturnType;
		USHORT			usTranType;                     // Transaction Type (Credit, Debit, EBT, etc)
		USHORT			usTranCode;                     // Transaction Code (Sale, Return, Void, etc)
		TCHAR			aszCardLabel[NUM_CPRSPTEXT];    // 28:number, see also TRN_DSI_CARDTYPE and EEPTRSPDATA.aszCardLabel
		ITEMTENDERAUTHCODE  authcode;                   // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno;		                // XEPT Reference Number generated for processor of transaction for EEPT
		ITEMTENDERINVNO     invno;                      // XEPT Invoice number from processor of transaction for EEPT
		ITEMTENDERSRNO      srno;                       // XEPT RecordNo - from DSI Client XML.  record identifier in current batch.
		TCHAR           AcctNumberEncrypted[384];       // encrypted account number and expiration
}EEPT_BLOCK_HEADER;

// The following defines are used with Store and Forward in order to allow for the updating of Preauth Capture transactions
// with the data from the corresponding Preauth transaction.
#define EEPT_BLOCK_REPL_AUTHDATA     "#AUT--#890123456"                     // used with Store and Forward as a place holder for authorization code in stored transactions
#define EEPT_BLOCK_REPL_REFNODATA    "#REF--#890123456789012"               // used with Store and Forward as a place holder for reference number in stored transactions
#define EEPT_BLOCK_REPL_ACQREFDATA   "#ACQ--#89012345678901234567890"
#define EEPT_BLOCK_REPL_INVNODATA    "#INV--#8901234"                       // used with Store and Forward as a place holder for invoice number in stored transactions
#define EEPT_BLOCK_REPL_SRNODATA     "#SRN--#8901"                          // used with Store and Forward as a place holder for record number in stored transactions

/*============================================================
    EEPT Store and Forward File Header for S&F Report
============================================================*/
// Following defines are used with member lUieAc102_SubMenu of EEPTF_HEADER struct
// to indicate particular conditions. Some are used as commands during processing of
// the EEPTSTO1 file in function ItemTendStoreForward() and others are used as persistent
// state to indicate some action taken such as File Recovery in EEptFileRecovery().
#define EEPT_SUBMENU_NONSTATUSMASK    0x0ffff         // used to isolate options and Stored File number
#define EEPT_SUBMENU_OPTIONSMASK      0x0f000         // mask for those bits used to indication options
#define EEPT_SUBMENU_OPIGNOREERRS     0x01000         // option to ignore errors during Forwarding
#define EEPT_SUBMENU_OPNODISPLAY      0x02000         // option to not display progress during Forwarding

#define EEPT_SUBMENU_RECOVEREDFILE    0x10000         // status indicator that file went through File Recovery at some point.

typedef struct{
		UCHAR	uchMajorClass;
		UCHAR	uchMinorClass;
		ULONG	ulBeginPoint;	// Points to first data block. During forward process we will advance the pointer to indicate
		// the next block to be forwarded.
		ULONG	ulEndPoint;		// Points to end of last data block (beginning of next free block). During forward process we 
		ULONG	ulFirstTansStart;
		ULONG	ulFirstTransEnd;
		// will know all blocks have been forwarded when ulBeginPoint is equal to ulEndPoint
		ULONG	ulTransStored;	// Number of transactions stored in this file.
		ULONG	ulApproved;		// Number of approved trans stored in the file
		ULONG	ulFailedTrans;	// Number of failed Transactions
		ULONG	ulErrorTrans;	// Number of Trans that had an Error
		ULONG	ulHardErrorTrans;	// Number of Transactions with a hard error, EEPT_HARD_ERROR, PROCESSED_HARD_ERROR
		ULONG   ulStoredConsecNo;  // the consecutive number incremented each time for a store
		LONG	lTotalPending;	// Sum of dollar value of stored transactions.
		LONG	lTotalPendingVoid;	 // Sum of dollar value of stored transactions.
		LONG	lUieAc102_SubMenu;	 // indicates which host we are processing.
		USHORT  usRegisterNumber;    // the number of this particular terminal or register
		DATE_TIME	fromTime;	// First transaction stored in file.
		DATE_TIME	toTime;		// Last transaction stored in file.
}EEPTF_HEADER;

typedef struct{
		UCHAR	uchPurchase[10];  // ULONG can hold values into the millions. This array will hold number in millions, plus decimal and cents
		UCHAR	uchTax[10];	      // Enough room to hold % of above
		UCHAR	uchChargeTip[10]; // ULONG can hold values into the millions. This array will hold number in millions, plus decimal and cents
} EEPT_AMOUNT;

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
/**     for Tally       **/
extern  EPTTALLY UNINT  EPTTally;                   /* tally counter        */

/*
===========================================================================
    MEMORY
===========================================================================
*/

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

USHORT  EEptConvertError(SHORT sError);
VOID 	EEPTReadResetTally(UCHAR uchType, EPTTALLY *pTally);
USHORT	StoreForwardOFF();

SHORT CliStoreForwardRead(LONG lUieAc102_SubMenu, EEPTF_HEADER *StoreForwardData);

#ifdef __cplusplus
}
#endif

#endif
/* ---------- End of EEPT.H ---------- */