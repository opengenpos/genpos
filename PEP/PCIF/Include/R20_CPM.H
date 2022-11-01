/*
*===========================================================================
* Title       : Charge Posting Manager, Header File
* Category    : Charge Posting Manager, Charge Posting Application
* Program Name: CPM.H
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
* Nov-01-93 00.00.01 :  Yoshiko. Jim  : Initial
* Aug-10-95 03.00.00 :  T.Koyama      : 2170 R3.0
*
*===========================================================================
*/
#if !defined(CPM_INCLUDED)
#define CPM_INCLUDED



/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/

// Pif network port number for Charge Post Manager
#define     CLI_PORT_CPM        50     /* Charge Posting Manager port no. */
#define     CLI_PORT_CPM_HOST   60     /* Charge Posting Manager IF to CPM Host port no. */


/**     return value    **/
#define     CPM_RET_SUCCESS         0           /* process succeed          */
#define     CPM_RET_NOT_MASTER     -20          /* I'm not master (O/S)     */
#define     CPM_RET_FAIL           -31          /* other communication err  */
#define     CPM_RET_BUSY           -32          /* CPM is busy              */
#define     CPM_RET_STOP           -33          /* CPM is stop process      */
#define     CPM_RET_OFFLINE        -34          /* CPM & PMS is offline     */
#define     CPM_RET_LENGTH         -35          /* receive length error     */
#define     CPM_RET_PORT_NONE      -36          /* PMS is not provide       */
#define     CPM_RET_TIMEOUT        -37          /* time out error           */

/**     terminal type   **/
#define     CPM_FROM_SELF           0           /* called from self term.   */
#define     CPM_FROM_NOT_SELF       1           /* called from other term.  */

/**     status of CPM  **/
#define     CPM_CHANGE_STOP         0
#define     CPM_CHANGE_START        1

/** tally required type **/
#define     CPM_TALLY_READ          1           /* read tally               */
#define     CPM_TALLY_RESET         2           /* read & reset tally       */

// request function code
#define     CPM_FC2_ROOMCHG         0x29        /* room charge func. code   */
#define     CPM_FC2_ACCTCHG         0x2c        /* account charge func code */
#define     CPM_FC2_PRECREDITA      0x2d        /* pre-credit autho. func code */
#define     CPM_FC2_CREDITA         0x2e        /* credit autho. func code  */

// response function codes
#define     CPM_FC2_ACCEPTED        0x30        /* accepted                 */
#define     CPM_FC2_REJECTED        0x31        /* rejected                 */
#define     CPM_FC2_OVERRIDE        0x32        /* override required        */
#define     CPM_FC2_NEEDGLINE       0x33        /* need guest line number   */
#define     CPM_FC2_STOP_CHARG      0x35        /* stop Charge Post/Credit Auth.*/
#define     CPM_FC2_STOP_POST       0x36        /* stop Posting                 */
#define     CPM_FC2_START           0x37        /* start Charge Post/Credit Auth*/
#define     CPM_FC2_PARTTEND        0x39        /* partial tender */

#define     CPM_NORMALTYP           '0'         /* normal message type      */
#define     CPM_OVERTYP             '1'         /* override request msg type*/

#define     CPM_CONT_MSG            '0'         /* continue message         */
#define     CPM_LAST_MSG            '1'         /* last message             */

//  Types of error responses from Charge Post Host
//  These should be encoded as a 2 character ANSI character array and
//  the resulting value put into the auchCRTRespMsg member of the response.
#define CPM_PROHBT_ADR           0         /* global error     */
#define CPM_PROHBT_ADR0          0         /* global error     */
#define CPM_PROHBT_ADR1          1         /* global error     */
#define CPM_PROHBT_ADR2          2         /* global error     */
#define CPM_CP_NOALLOW_ADR       3         /* charge not allow */
#define CPM_GC_OUT_ADR           4         /* guest checked out */
#define CPM_WRONG_RM_ADR         5         /* wrong room number */
#define CPM_WRONG_GID_ADR        6         /* wrong guest ID */
#define CPM_VOID_NOALLOW_ADR     9         /* void not allowed */
#define CPM_WRONG_ACNO_ADR      10         /* wrong account number */
#define CPM_CP_ADVISE_ADR       11         /* advise front desk */
#define CPM_WRONG_ACTYPE_ADR    12         /* wrong account type */
#define CPM_WRONG_SLD_ADR       13         /* wrong SLD */
#define CPM_FLFUL_ADR           14         /* charge file full */
#define CPM_NTINFL_ADR          17         /* guest number not found */
#define CPM_WRONG_VOUCH_ADR     18         /* wrong voucher number */

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


//
//		We want to maintain backwards compatibility with the previous 3rd
//		party vendors of NeighborhoodPOS such as Gateway with the COMM2170
//		charge post application.  Therefore the structs should remain the same
//		as in Rel 1.4

typedef struct{
    UCHAR   auchSystemFunc[1];                  /* 1: system function       */
    UCHAR   auchSrcTerminal[2];                 /* 2: source terminal       */
    UCHAR   auchDesTerminal[2];                 /* 4: destination terminal  */
    UCHAR   auchDataSeqNo[5];                   /* 6: data sequence no.     */
    UCHAR   auchFuncCode2[1];                   /* 11: function code        */
    UCHAR   auchNotUsed2[6];                    /* 12: not used             */
    UCHAR   auchTerminalNo[1];                  /* 18: terminal number      */
    UCHAR   auchSystemNo[4];                    /* 19: system number        */
    UCHAR   auchStoreNo[4];                     /* 23: store number         */
    UCHAR   auchHotelID[2];                     /* 27: hotel ID             */
    UCHAR   auchGCNumber[6];                    /* 29: guest chk #/voucher #*/
    UCHAR   auchChargeTtl[10];                  /* 35: charge total         */
    UCHAR   auchTransNo[4];                     /* 45: consecutive number    */
    UCHAR   auchCPFlag[1];                      /* 49: charge post flag     */
    UCHAR   auchSLDNumber[4];                   /* 50: SLD number           */
    UCHAR   auchRoomNumber[5];                  /* 54: room number          */
    UCHAR   auchGuestID[2];                     /* 59: guest ID             */
    UCHAR   auchNotUsed3[26];                   /* 61: not used             */
    UCHAR   auchGuestLineNo[2];                 /* 87: guest line number    */
    UCHAR   auchCashierNo[3];                   /* 89: cashier number       */
    UCHAR   auchServerNo[3];                    /* 92: server number        */
    UCHAR   auchChargeTip[10];                  /* 95: charge tips amount   */
    UCHAR   auchTtlTax[10];                     /* 105: total tax amount    */
    UCHAR   auchMSRData[40];                    /* 115: MSR data            */
    UCHAR   auchDiscItemTtl1[10];               /* 155: discount itemizer 1 total */
    UCHAR   auchDiscItemTtl2[10];               /* 165: discount itemizer 2 total */
    UCHAR   auchEndofMsg[1];                    /* 175: end of message      */
}CPMRCREQDATA;

typedef struct{
    UCHAR   auchSystemFunc[1];                  /* 1: system function       */
    UCHAR   auchSrcTerminal[2];                 /* 2: source terminal       */
    UCHAR   auchDesTerminal[2];                 /* 4: destination terminal  */
    UCHAR   auchDataSeqNo[5];                   /* 6: data sequence no.     */
    UCHAR   auchFuncCode2[1];                   /* 11: function code        */
    UCHAR   auchAcceptAmt[6];                  /* 12: accepted amount      */
    UCHAR   auchTerminalNo[1];                  /* 18: terminal number      */
    UCHAR   auchLastMsgInd[1];                  /* 19: last msg indicator   */
    UCHAR   auchCRTRespMsg[2];                  /* 20: CRT response msg #   */
    UCHAR   auchFolioNumber[6];                 /* 22: folio number         */
    UCHAR   auchPostTransNo[6];                 /* 28: posted transaction # */
    UCHAR   auchMsgText[39];                    /* 34: message text         */
    UCHAR   auchEndofMsg[1];                    /* 73: end of message       */
}CPMRCRSPDATA;

typedef struct{
    UCHAR   auchSystemFunc[1];                  /* 1: system function       */
    UCHAR   auchSrcTerminal[2];                 /* 2: source terminal       */
    UCHAR   auchDesTerminal[2];                 /* 4: destination terminal  */
    UCHAR   auchDataSeqNo[5];                   /* 6: data sequence no.     */
    UCHAR   auchFuncCode2[1];                   /* 11: function code        */
    UCHAR   auchNotUsed2[6];                    /* 12: not used             */
    UCHAR   auchTerminalNo[1];                  /* 18: terminal number      */
    UCHAR   auchSystemNo[4];                    /* 19: system number        */
    UCHAR   auchStoreNo[4];                     /* 23: store number         */
    UCHAR   auchHotelID[2];                     /* 27: hotel ID             */
    UCHAR   auchGCNumber[6];                    /* 29: guest chk #/voucher #*/
    UCHAR   auchChargeTtl[10];                  /* 35: charge total         */
    UCHAR   auchTransNo[4];                     /* 45: consecutive number    */
    UCHAR   auchCPFlag[1];                      /* 49: charge post flag     */
    UCHAR   auchSLDNumber[4];                   /* 50: SLD number           */
    UCHAR   auchAcctType[2];                    /* 54: account type         */
    UCHAR   auchAcctNumber[24];                 /* 56: account number       */
    UCHAR   auchExpiraDate[4];                  /* 80: acct expiration date */
    UCHAR   auchNotUsed3[3];                    /* 84: not used             */
    UCHAR   auchGuestLineNo[2];                 /* 87: guest line number    */
    UCHAR   auchCashierNo[3];                   /* 89: cashier number       */
    UCHAR   auchServerNo[3];                    /* 92: server number        */
    UCHAR   auchChargeTip[10];                  /* 95: charge tips amount   */
    UCHAR   auchTtlTax[10];                     /* 105: total tax amount    */
    UCHAR   auchMSRData[40];                    /* 115: MSR data            */
    UCHAR   auchDiscItemTtl1[10];               /* 155: discount itemizer 1 total */
    UCHAR   auchDiscItemTtl2[10];               /* 165: discount itemizer 2 total */
    UCHAR   auchEndofMsg[1];                    /* 175: end of message      */
}CPMACREQDATA;

/**     TALLY DATA      **/
typedef struct  {
    USHORT  usPMSSendOK;                        /* send success in PMS              */
    USHORT  usPMSSendErr;                       /* send error in PMS                */
    USHORT  usPMSRecResp;                       /* receive response PMS             */
    USHORT  usPMSRecUnsoli;                     /* receive unsoli. PMS              */
    USHORT  usPMSThrowResp;                     /* throw response PMS               */
    USHORT  usTimeout;                          /* timeout in PifReadCom()          */
    USHORT  usIHCSendErr;                       /* send error in IHC                */
}CPMTALLY;
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
//extern  CPMTALLY UNINT  CpmTally;                   /* tally counter        */

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
VOID    CpmInit(VOID);

SHORT   CpmSendMessage(UCHAR *pSendData, USHORT usSendLen, UCHAR uchTerminal);
SHORT   CpmReceiveMessage(UCHAR *pRecData, USHORT usRecSize);
SHORT   CpmChangeStatus(UCHAR uchState);
VOID    CpmReadResetTally(UCHAR uchType, CPMTALLY *pTally);
USHORT  CpmConvertError(SHORT sError);

#endif /* CPM_INCLUDED */
/* ---------- End of CPM.H ---------- */
