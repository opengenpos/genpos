/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : EPT MODE HEADER FILE                         
* Category    : EPT MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: ept.h
* ----------------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name      : Description
* Nov-16-93: 00.00.01 : K.You       : initial                                   
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

/* for debug */
/* #define EPTDEBUG                        1 */

#define     CLI_PORT_EPT_HOST   70     /* EPT Manager IF to EPT Host port no. */

/* tally required type */
#define EPT_TALLY_READ                  1       /* read tally */
#define EPT_TALLY_RESET                 2       /* read & reset tally */

#define EPT_SUCCESS                     0x00    /* success */

// Special values used in message fields
#define EPT_STX                         0x02    /* start text */
#define EPT_ETX                         0x03    /* end text */
#define EPT_EOT                         0x04    /* end of text */
#define EPT_ENQ                         0x05    /* enquiry */
#define EPT_ACK                         0x06    /* acknowledge */
#define EPT_NAK                         0x15    /* not acknowledge */

#define EPT_COM_ERR                     -31     /* general comm. error for EPT */
#define EPT_NOT_EPTPORT                 -36     /* not exist EPT port */
#define EPT_ABORTED                     -37     /* aborted by user for EPT */


/**     return value    **/
#define     EEPT_RET_SUCCESS         0           /* process succeed          */
#define     EEPT_RET_NOT_MASTER     -20          /* I'm not master (O/S)     */
#define     EEPT_RET_FAIL           -31          /* other communication err  */
#define     EEPT_RET_BUSY           -32          /* EEPT is busy              */
#define     EEPT_RET_STOP           -33          /* EEPT is stop process      */
#define     EEPT_RET_OFFLINE        -34          /* EEPT & PMS is offline     */
#define     EEPT_RET_LENGTH         -35          /* receive length error     */
#define     EEPT_RET_PORT_NONE      -36          /* PMS is not provide       */
#define     EEPT_RET_TIMEOUT        -37          /* time out error           */

/**     status of EPT  **/
#define     EPT_CHANGE_STOP         0
#define     EPT_CHANGE_START        1

/** tally required type **/
#define     EPT_TALLY_READ          1           /* read tally               */
#define     EPT_TALLY_RESET         2           /* read & reset tally       */

/* --- Function Code & Status Code --- */
#define     EPT_FC2_EPT             0x1e        /* EPT (Enhance Type)       */
#define     EPT_FC2_ASK             0x3a        /* Ask                      */

#define     EPT_FC2_ACCEPTED        0x30        /* accepted                 */
#define     EPT_FC2_REJECTED        0x31        /* rejected                 */
#define     EPT_FC2_OVERRIDE        0x32        /* override required        */
#define     EPT_FC2_NEEDGLINE       0x33        /* need guest line number   */
#define     EPT_FC2_STOP_CHARG      0x35        /* stop Charge Post/Credit Auth.*/
#define     EPT_FC2_STOP_POST       0x36        /* stop Posting                 */
#define     EPT_FC2_START           0x37        /* start Charge Post/Credit Auth*/
#define     EPT_FC2_PARTTEND        0x39        /* partial tender           */
#define     EPT_FC2_UNSOLI          0x42        /* Unsolicited Message      */

#define     EPT_NORMALTYP           '0'         /* normal message type      */
#define     EPT_OVERTYP             '1'         /* override request msg type*/

#define     EPT_CONT_MSG            '0'         /* continue message         */
#define     EPT_LAST_MSG            '1'         /* last message             */

/* --- Message Instruction (Response Message) --- */
#define     EPT_RES_INST_NODISP     0x30        /* No Display               */
#define     EPT_RES_INST_DISPMSG    0x31        /* Display Response Message */
#define     EPT_RES_INST_DISPTEXT   0x32        /* Display Response Text    */
#define     EPT_RES_INST_PRINT      0x33        /* Print Response Text      */

/*--------------------------------------------------------------------------
*       EPT data
--------------------------------------------------------------------------*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

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
    UCHAR   auchTransNo[4];                 /* Consecutive#          */
    UCHAR   auchCPFlag;                     /* Charge Post Flag     */
    UCHAR   auchSLDNumber[4];               /* SLD#                 */
    UCHAR   auchMSRData[NUM_MSRDATA];       /* MSR Data             */
    UCHAR   auchExpDate[4];                 /* Expiration Date      */
    UCHAR   auchGuestLineNo[2];             /* Guest Line#          */
    UCHAR   auchCashierNo[3];               /* Checker#             */
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

    UCHAR   auchIpAddress[4][3];            /* IpAddress of Server  */
    UCHAR   auchIpPort[5];                  /* IpPort# of Server    */
    UCHAR   auchMerchantID[24];             /* Merchant ID          */
	UCHAR	auchTranType[2];				// Transaction Type Credit, Debit, etc
	UCHAR	auchTranCode[2];				// Transaction Code Sale, Return, Void, etc
    UCHAR   auchComPort[3];                 /* Com Port of Canadian Approved Pin Pad  */
    UCHAR	auchRefNo[NUM_REFERENCE];		/* Reference number returned in transaction response */
	UCHAR   auchDriversLicence[19];         /* DriversLicence #     */
    UCHAR   auchStateCode[2];               /* State Code of DL#    */
    UCHAR   auchDateOfBirth[6];             /* Date of Birth        */
    UCHAR   auchCheckSeqNo[20];             /* Check #              */
    UCHAR   auchCanadianSeqNo[20];          /* Sequence # from Canadian Debit    */
    UCHAR   auchAuthCode[NUM_AUTHCODE];     /* Authorization Code   */
    UCHAR   auchAcqRefData[40];             /* Acquirer Reference returned by processor on PreAuth Transaction requests      */
    UCHAR   auchCustomerCode[17];           /* For Purchase Card Level II transactions             */
	UCHAR	auchDecimal;					// Decimal places
	UCHAR	auchLanguageId[4];				// Language
	UCHAR	auchSubLanguageId[4];			// Sub Language ID
	UCHAR	auchCodePage[4];				// Code Page
	UCHAR	auchNotUsed[50];				/* Future Use Area		*/
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
} EEPTREQDATA;

typedef struct {
    UCHAR   uchSTX;                         /* STX Character        */
    UCHAR   auchSrcTerminal[2];             /* Source Terminal      */
    UCHAR   auchDesTerminal[2];             /* Destination Terminal */
    UCHAR   auchDataSeqNo[3];               /* Data Sequence#       */
    UCHAR   auchStatus;                     /* Status Code          */
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
    UCHAR   auchPartTender[10];             /* Partial Tender Amount*/
    WCHAR   auchMsgText[400];               /* Message Text         */

	UCHAR	auchAuthCode[NUM_AUTHCODE];		// Authorization Code From Processor for the Approved transaction
	UCHAR	auchInvoiceNo[16];				// InvoiceNo supplied by request
	UCHAR	auchRefNo[NUM_REFERENCE];		// Reference Number returned by the processor
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
    UCHAR   auchNotUsed[50];                /* Future Use Area      */\
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
} EEPTRSPDATA;

#define EEPT_MSG_TEXT   400                 /* Length of Message Text */

/* --- Saratoga */
typedef struct  {
    USHORT  usEPTSendOK;                 /* 1: send success in EPT */
    USHORT  usEPTSendErr;                /* 3: send error in EPT */
    USHORT  usEPTRespOK;                 /* 5: receive response success in EPT */
    USHORT  usEPTRespErr;                /* 7: receive response error in EPT */
}EPTTALLY;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*--------------------------------------------------------------------------
*   external work area declarations
--------------------------------------------------------------------------*/

extern  EPTTALLY UNINT  EPTTally;                   /* tally counter */
        
/*--------------------------------------------------------------------------
*       protocol definitions
--------------------------------------------------------------------------*/

SHORT EPTConfirmMessage( EEPTREQDATA *ReqData,
                         EEPTRSPDATA *RcvData );
SHORT EPTGetPort( VOID );
SHORT EPTOpenSetPort( VOID );
SHORT EPTConfirmCtlData( UCHAR uchCtlCode );
SHORT EPTConfirmReqData( EEPTREQDATA *ReqData,
                         USHORT usDataSize );
SHORT EPTRcvRspData( EEPTRSPDATA *RcvData,
                     USHORT usDataSize );
VOID EPTClosePort( SHORT hsEPTPort );
VOID EPTReadResetTally( UCHAR uchType, EPTTALLY *pTally );


/****** End of Definition ******/



