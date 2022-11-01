#if POSSIBLE_DEAD_CODE
/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
* Title       : EPT MODE HEADER FILE                         
* Category    : EPT MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: ept.h
* ----------------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------------
* Abstract: 
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name      : Description
* Nov-16-93: 00.00.01 : K.You       : initial                                   
*===================================================================================
*===================================================================================
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
*   PVCS ENTRY
*    $Revision$
*    $Date$
*    $Author$
*    $Log$
\*-----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
*   Define Declarations
--------------------------------------------------------------------------*/

/* for debug */
/* #define EPTDEBUG                        1 */

/* tally required type */
#define EPT_TALLY_READ                  1       /* read tally */
#define EPT_TALLY_RESET                 2       /* read & reset tally */

#define EPT_SUCCESS                     0x00    /* success */

#define EPT_STX                         0x02    /* start text */
#define EPT_ETX                         0x03    /* end text */
#define EPT_EOT                         0x04    /* end of text */
#define EPT_ENQ                         0x05    /* enquiry */
#define EPT_ACK                         0x06    /* acknowledge */
#define EPT_NAK                         0x15    /* not acknowledge */
#define EPT_COM_ERR                     -31     /* general comm. error for EPT */
#define EPT_NOT_EPTPORT                 -36     /* not exist EPT port */
#define EPT_ABORTED                     -37     /* aborted by user for EPT */
 
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
    UCHAR   auchNotUsed1[6];                /* Not Used             */
    UCHAR   auchTerminalNo;                 /* Terminal#            */
    UCHAR   auchSystemNo[4];                /* System#              */
    UCHAR   auchStoreNo[4];                 /* Store#               */
    UCHAR   auchHotelID[2];                 /* Hotel ID             */
    UCHAR   auchGuest[6];                   /* Guest Check No       */
    UCHAR   auchChargeTtl[10];              /* Charge Total         */
    UCHAR   auchTransNo[4];                 /* Consecutive#          */
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
    UCHAR   auchNotUsed9[20];               /* Not Used             */
    UCHAR   auchNotUsed10[20];              /* Not Used             */
    UCHAR   uchETX;                         /* ETX Character        */
    UCHAR   uchLRC;                         /* LRC Check Digit      */
} EPTREQDATA;

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
} EPTRSPDATA;

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

SHORT EPTConfirmMessage( EPTREQDATA *ReqData,
                         EPTRSPDATA *RcvData );
SHORT EPTGetPort( VOID );
SHORT EPTOpenSetPort( VOID );
SHORT EPTConfirmCtlData( UCHAR uchCtlCode );
SHORT EPTConfirmReqData( EPTREQDATA *ReqData,
                         USHORT usDataSize );
SHORT EPTRcvRspData( EPTRSPDATA *RcvData,
                     USHORT usDataSize );
VOID EPTClosePort( SHORT hsEPTPort );
VOID EPTReadResetTally( UCHAR uchType, EPTTALLY *pTally );


/****** End of Definition ******/



#endif