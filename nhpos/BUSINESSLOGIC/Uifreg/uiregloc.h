/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : User Interface for Reg, Local Header File                         
* Category    : User Interface, NCR 2170 US Hospitality Application         
* Program Name: UIREGLOC.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :   Name    : Description
* May-13-92:M.Suzuki   : initial                                   
* Jul-01-93:K.You      : add ETK, post receipt feature.                                   
* Jul-20-93:K.You      : add EJ reverse feature.                                   
* Feb-23-95:hkato      : R3.0
* Nov-08-95:hkato      : R3.1
* Jul-31-98:hrkato     : V3.3 (New Key Sequence)
* Nov-25-99:hrkato     : Saratoga
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
#define NON_DECIMAL           0xFF              /* Non Decimal input data               */

/*
*---------------------------------------------------------------------------
*   Function Name
*---------------------------------------------------------------------------
*/
#define CID_CASHIER             2                   /* Cashier function name */
#define CID_CASHIERSIGNIN       3                   /* Cashier sign in function name */
#define CID_CASHIERSIGNOUT      4                   /* Cashier sign out function name */
#define CID_TRANSVOID           5                   /* Transaction Void function name */
#define CID_SINGLETRANS         6                   /* Single Transaction function name */
#define CID_CHANGECOMP          7                   /* Change Computation function name */
#define CID_RECEIPT             8                   /* Receipt/no Receipt function name */
#define CID_SALES               9                   /* Cashier Sales function name */
#define CID_ITEM                10                  /* Item function name */
#define CID_DISCOUNT            11                  /* Discount function name */
#define CID_EC                  12                  /* EC function name */
#define CID_TOTAL               13                  /* Total Key function name */
#define CID_CHARGETIPS          14                  /* Charge Tips function name */
#define CID_PRINTONDEMAND       15                  /* Print on Demand function name */
#define CID_CANCEL              16                  /* Cancel function name */
#define CID_PAYMENT             17                  /* Payment function name */
#define CID_TENDER              18                  /* Tender function name */
#define CID_FOREIGNIN           19                  /* Foreign In function name */
#define CID_FOREIGNOUT          20                  /* Foreign Out function name */
#define CID_FOREIGNOUTEC        21                  /* Foreign EC function name */
#define CID_PAYMODIFIER         22                  /* Payment Modifier function name */
#define CID_FEEDRELEASE         23                  /* Feed/Release function name */
#define CID_RELEASE             24                  /* Release function name */
#define CID_FEED                25                  /* Feed function name */
#define CID_NEWCHECK            26                  /* New Check function name */
#define CID_NEWCHECKENTRY       27                  /* New Check Entry function name */
#define CID_GCINFORMATION       28                  /* GC Information function name */
#define CID_REORDER             29                  /* Reorder function name */
#define CID_PB                  30                  /* PB Entry function name */
#define CID_LINENO              31                  /* Line No function name */
#define CID_ADDCHECK            32                  /* Add Check function name */
#define CID_ADDCHECKENTRY       33                  /* Add Check Entry function name */
#define CID_GCNUMBERENTRY       34                  /* GC No Entry function name */
#define CID_WAITER              35                  /* Waiter function name */
#define CID_WAITERSIGNIN        36                  /* Waiter Sign In function name */
#define CID_WAITERSIGNOUT       37                  /* Waiter Sign Out function name */
#define CID_DECLARED            38                  /* Declared Tips function name */
#define CID_CHKTRANSFER         39                  /* Check Transfer function name */
#define CID_TRANSFERIN          40                  /* Transfer In function name */
#define CID_CHECKNO             41                  /* Check No Entry function name */
#define CID_TRANSFEROUT         42                  /* Transfer Out function name */
#define CID_REGDEFPROC          43                  /* Default Process function name */

#define CID_PERSONFIRST         44                  /* No of person 1'st entry function name, GCA */
#define CID_PERSON              45                  /* No of person entry function name, GCA */

#define CID_ETKTIMEIN           46                  /* ETK Time In function name */
#define CID_ETKTIMEOUT          47                  /* ETK Time Out function name */
#define CID_POSTRECEIPT         48                  /* Post Receipt function name */
#define CID_REVERSE             49                  /* EJ Reverse function name */
#define CID_ALPHAFIRST          50                  /* Alpha Name function name, R3.0 */
#define CID_ALPHA               51                  /* Alpha Name function name, R3.0 */

#define CID_DIACASHIERENTRY     52                  /* Cashier No Entry function name */
#define CID_DIAWAITERENTRY      53                  /* Waiter No Entry function name */
#define CID_DIAGUESTNOENTRY     54                  /* Guest No Entry function name */
#define CID_DIATABLENOENTRY     55                  /* Table No Entry function name */
#define CID_DIALINENOENTRY      56                  /* Line No Entry function name */
#define CID_DIANOPERSONENTRY    57                  /* No of Person Entry function name */
#define CID_DIAETKENTRY         58                  /* ETK Job Code Entry function name */
#define CID_DIACPENTRY1         59                  /* Charge Posting Entry function name */
#define CID_DIACPENTRY2         60                  /* Charge Posting Entry function name */
#define CID_DIACPENTRY3         61                  /* Charge Posting Entry function name */
#define CID_DIACPENTRY4         62                  /* Charge Posting Entry function name */
#define CID_DIAOEPENTRY         63                  /* Order Number Entry function name, R3.0 */
#define CID_DIAALPHAENTRY       64                  /* Alpha Name Entry function name, R3.0 */
#define CID_DIASEATNOENTRY      65                  /* Seat# Entry function name,      R3.1 */
#define CID_MSR                 66                  /* MSR function name,              R3.1 */
#define CID_SIGNINMSR           67                  /* MSR Sign-in name,               R3.1 */
#define CID_CASHIERSIGNOUTMSR   68                  /* MSR Cashier Sign-out namr,      R3.1 */
#define CID_CASHIERWAITERMSR    69                  /* MSR Waiter-in name,             R3.1 */
#define CID_CASOUTWAIINMSR      70                  /* MSR Waiter Sign-in name,        R3.1 */
#define CID_WAITERSIGNOUTMSR    71                  /* MSR Waiter Sign-out name,       R3.1 */
#define CID_DIATRANSFER         72                  /* Transfer No Entry name,         R3.1 */
#define CID_SEATENTRY           73                  /* Seat# Key name,                 R3.1 */
#define CID_BARTENDER           74                  /* Bartender function name,        R3.1 */
#define CID_BARTENDERSIGNIN     75                  /* Bartender Sign-In name,         R3.1 */
#define CID_BARTENDERSIGNOUT    76                  /* Bartender Sign-Out name,        R3.1 */
#define CID_TRANSFER            77                  /* Transfer (Type 2) name,         R3.1 */
#define CID_TRANSFITEM          78                  /* Transfer (Itemize) name,        R3.1 */
#define CID_CURSORVOID          79                  /* Cursor VOID ,                   R3.1 */
#define CID_DISPENSERFIRST      80                  /* Beverage Dispenser 1'st,        R3.1 */
#define CID_DISPENSER           81                  /* Beverage Dispenser ,            R3.1 */
#define CID_DIASCROLLENTRY      82                  /* Scroll Key Entry ,              R3.1 */

#define CID_CASINT              83                  /* Cashier Interrupt,              R3.3 */
#define CID_CASINTSIGNIN        84                  /* Cashier Interrupt,              R3.3 */
#define CID_CASINTSIGNINOUTMSR  85                  /* Cashier Interrupt,              R3.3 */

#define CID_NEWKEYSEQ           86                  /* Reduce NC, RO, AC Key,          V3.3 */
#define CID_NEWKEYSEQENTRY      87                  /* Reduce NC, RO, AC Entry,        V3.3 */
#define CID_NEWKEYSEQGC         88                  /* Split/Multi Check,              V3.3 */

#define CID_POSTRECEIPTSIGNOUT  89                  /* post receipt in sign-out,       V3.3 */
#define CID_CASLOCKSIGNINOUT    90                  /* server lock sign-in/out,        V3.3 */
#define CID_DIAINSERTLOCKENTRY  91                  /* Insert Waiter Lock Entry function name, V3.3 */

#define CID_DIADEPTNO           92                  /* dept no. entry, 2172 */
#define CID_DIAOEPKEYENTRY      93                  /* OEP Key. entry, 2172 */
#define CID_DIAPRICEENTRY       94                  /* price entry, 2172 */

#define CID_MONEYDEC            95                  /* Money declaration,       Saratoga    */
#define CID_MONEYIN             96                  /* Money declaration-in                 */
#define CID_MONEYTEND           97                  /* Money declaration, tender entry      */
#define CID_MONEYOUT            98                  /* Money declaration-out,   Saratoga    */
#define CID_DIACPENTRY5         99                  /* Guest Line # for EPT,    Saratoga    */

#define CID_FSTOTALIN          100                  /* Foodstamp Total In function name */
#define CID_FOODSTAMP          101                  /* Foodstamp function name          */
#define CID_FSTENDER           102                  /* Foodstamp Tender function name   */
#define CID_WICTRAN            103                  /* WIC transaction function name    */

#define CID_NUM_TYPE		   104					// Number Type entry function name (UifItem) US Customs SCER /4/8/03

#define CID_DIACPENTRY6        105                  /* Charge Posting Entry function name */
#define CID_DIATAREENTRY	   106					/* Tare entry function name*/

/*
*---------------------------------------------------------------------------
*   User Interface for Reg Status Bit (flUifRegStatus)
*---------------------------------------------------------------------------
*/
#define UIFREG_MODIFIER         0x00000001          /* Modifier Bit */
#define UIFREG_PARTIAL          0x00000002          /* Partial Tender Bit */
#define UIFREG_FINALIZE         0x00000004          /* Finalize Bit */
#define UIFREG_WAITERSIGNOUT    0x00000008          /* Waiter Sign Out Bit */
#define UIFREG_CASHIERSIGNOUT   0x00000010          /* Cashier Sign Out Bit */
#define UIFREG_BUFFERFULL       0x00000020          /* Buffer Full Bit */
#define UIFREG_SEATTRANS        0x00000040          /* Seat Trans Bit,          R3.1 */
#define UIFREG_BASETRANS        0x00000080          /* Base Trans Bit,          R3.1 */
#define UIFREG_CASHIERSIGNIN    0x00000100          /* Cashier Sign-in Bit,     R3.1 */
#define UIFREG_WAITERSIGNIN     0x00000200          /* Waiter Sign-in Bit,      R3.1 */
#define UIFREG_BARSIGNIN        0x00000400          /* Bar Tenderr Sign-in Bit, R3.1 */
#define UIFREG_ETKSIGNIN        0x00000800          /* ETK Sign-in Bit,         R3.1 */
#define UIFREG_TYPE2_TRANS      0x00001000          /* Type 2 Trans Bit,        R3.1 */
#define UIFREG_CASINTSIGNIN     0x00002000          /* Cashier Interrupt Sign-in Bit,   R3.3 */
#define UIFREG_CASINTRECALL     0x00004000          /* Cashier Interrupt Recall Bit,    R3.3 */
#define UIFREG_CASINTNEWIN      0x00008000          /* Cashier Interrupt New Sign-in Bit,    R3.3 */
#define UIFFUNC_FSTOTAL         0x00010000          /* Inform Foodstamp Total   */
#define UIFFUNC_FSTENDER        0x00020000          /* Inform Foodstamp Tender  */
#define UIFFUNC_FS2NORMAL       0x00040000          /* Inform FS Tender to Normal Tender*/
#define UIFREG_CURSORRETURN     0x00080000          /* Notify Payment to do an accept */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Item Module Interface Data 
*===========================================================================
*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef union {                                /* Reg Data */
    UIFREGOPEOPEN   regopeopen;                 /* Operator open */
    UIFREGTRANSOPEN regtransopen;               /* Transaction open */
    UIFREGSALES     regsales;                   /* Sales */
    UIFREGDISC      regdisc;                    /* Discount */
    UIFREGCOUPON    regcoupon;                  /* Coupon */
    UIFREGTOTAL     regtotal;                   /* Total */
    UIFREGTENDER    regtender;                  /* Tender */
    UIFREGMISC      regmisc;                    /* Misc Transaction */
    UIFREGMODIFIER  regmodifier;                /* Modifier */
    UIFREGOTHER     regother;                   /* Other Transaction */
    UIFREGOPECLOSE  regopeclose;                /* Operator Close */
    UIFREGMODEIN    regmodein;                  /* Mode In */
} UIFITEMDATA;

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
extern UIFITEMDATA UifRegData;                        /* Item Module Interface Data */
extern UCHAR       uchUifRegVoid;                     /* Item Void Save Area */
extern TCHAR       aszUifRegNumber[NUM_NUMBER];       /* Number Save Area */
extern UIFDIADATA  UifRegDiaWork;                     /* Dialog Work Area */
extern ULONG       flUifRegStatus;                    /* UI for Reg Status Area, Saratoga */
extern SHORT       husUifRegHandle;                   /* UIC Handle Save Area */
extern UCHAR       auchUifDifFsc[2];                  /* Beverage Dispenser String Buffer, R3.1 */
extern UCHAR       fchUifSalesMod;                    /* Sales Modifier Save Area, 2172 */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT   UifCashier(KEYMSG *pData);                  /* uifcas.c */
SHORT   UifCashierSignIn(KEYMSG *pData);            /* uifcasin.c */
SHORT   UifCashierSignOut(KEYMSG *pData);           /* uifcasot.c */
SHORT   UifTransVoid(KEYMSG *pData);                /* uiftvoid.c */
SHORT   UifSingleTrans(KEYMSG *pData);              /* uifsingl.c */
SHORT   UifChangeComp(KEYMSG *pData);               /* uifccomp.c */
SHORT   UifReceipt(KEYMSG *pData);                  /* uifrecpt.c */
SHORT   UifOrderDisplay(KEYMSG *pData);             /* uifodisp.c */
SHORT   UifSales(KEYMSG *pData);                    /* uifsales.c */
SHORT   UifItem(KEYMSG *pData);                     /* uifitem.c */
SHORT   UifDiscount(KEYMSG *pData);                 /* uifdisc.c */
SHORT   UifEC(KEYMSG *pData);                       /* uifec.c */
SHORT   UifTotal(KEYMSG *pData);                    /* uiftotal.c */
SHORT   UifChargeTips(KEYMSG *pData);               /* uifchtip.c */
SHORT   UifPrintOnDemand(KEYMSG *pData);            /* uifprdmd.c */
SHORT   UifCancel(KEYMSG *pData);                   /* uifcan.c */
SHORT   UifPayment(KEYMSG *pData);                  /* uifpay.c */
SHORT   UifTender(KEYMSG *pData);                   /* uiftend.c */
SHORT   UifForeignIn(KEYMSG *pData);                /* uifforin.c */
SHORT   UifForeignOut(KEYMSG *pData);               /* uifforot.c */
SHORT   UifForeignOutEC(KEYMSG *pData);             /* uifforot.c */
SHORT   UifPayModifier(KEYMSG *pData);              /* uifpaymd.c */
SHORT   UifFeedRelease(KEYMSG *pData);              /* uiffedrl.c */
SHORT   UifRelease(KEYMSG *pData);                  /* uifrelse.c */
SHORT   UifFeed(KEYMSG *pData);                     /* uiffeed.c */
SHORT   UifNewCheck(KEYMSG *pData);                 /* uifnewck.c */
SHORT   UifNewCheckEntry(KEYMSG *pData);            /* uifncken.c */
SHORT   UifGCInformation(KEYMSG *pData);            /* uiggcinf.c */
SHORT   UifReorder(KEYMSG *pData);                  /* uifreord.c */
SHORT   UifPB(KEYMSG *pData);                       /* uifpb.c */
SHORT   UifLineNo(KEYMSG *pData);                   /* uifline.c */
SHORT   UifAddCheck(KEYMSG *pData);                 /* uifaddck.c */
SHORT   UifAddCheckEntry(KEYMSG *pData);            /* uifacken.c */
SHORT   UifGCNumberEntry(KEYMSG *pData);            /* uifgcnum.c */
SHORT   UifWaiter(KEYMSG *pData);                   /* uifwai.c */
SHORT   UifWaiterSignIn(KEYMSG *pData);             /* uifwaiin.c */
SHORT   UifWaiterSignOut(KEYMSG *pData);            /* uifwaiot.c */
SHORT   UifDeclared(KEYMSG *pData);                 /* uifdeclr.c */
SHORT   UifChkTransfer(KEYMSG *pData);              /* uifcktrs.c */
SHORT   UifTransferIn(KEYMSG *pData);               /* uiftrsin.c */
SHORT   UifCheckNo(KEYMSG *pData);                  /* uifchkno.c */
SHORT   UifTransferOut(KEYMSG *pData);              /* uiftrsot.c */
SHORT   UifRegDefProc(KEYMSG *pData);               /* uifrgdef.c */

SHORT   UifNoPersonFirst( KEYMSG *pData );          /* GCA */
SHORT   UifNoPerson( KEYMSG *pData );               /* GCA */
SHORT   UifAlphaFirst( KEYMSG *pData );             /* R3.0 */
SHORT   UifTent( KEYMSG *pData );					//PDINU
SHORT   UifAlpha( KEYMSG *pData );                  /* R3.0 */
SHORT   UifCheckScannerData(KEYMSG *pData);                 /* V3.3 */
UCHAR   UifCheckScannerCDV(UCHAR *aszPluNo, SHORT sLen);    /* V3.3 */

SHORT   UifETKTimeIn( KEYMSG *pData );              /* uifetkin.c */
SHORT   UifETKTimeOut( KEYMSG *pData );             /* uifetkot.c */
SHORT   UifPostReceipt(KEYMSG *pData);              /* uifpost.c */
SHORT   UifReverse(KEYMSG *pData);                  /* uifrever.c */

SHORT   UifDiaCashierEntry(KEYMSG *pData);          /* uidcaset.c */
SHORT   UifDiaWaiterEntry(KEYMSG *pData);           /* uidwaiet.c */
SHORT   UifDiaGuestNoEntry(KEYMSG *pData);          /* uidgcnet.c */
SHORT   UifDiaTableNoEntry(KEYMSG *pData);          /* uidtblet.c */
SHORT   UifDiaLineNoEntry(KEYMSG *pData);           /* uidlinet.c */
SHORT   UifDiaNoPersonEntry(KEYMSG *pData);         /* uidperet.c */
SHORT   UifDiaETKEntry(KEYMSG *pData);              /* uidetket.c */
SHORT   UifDiaCPEntry1(KEYMSG *pData);              /* uidcpet.c */
SHORT   UifDiaCPEntry2(KEYMSG *pData);              /* uidcpet.c */
SHORT   UifDiaCPEntry3(KEYMSG *pData);              /* uidcpet.c */
SHORT   UifDiaCPEntry4(KEYMSG *pData);              /* uidcpet.c */
SHORT   UifDiaCPEntry5(KEYMSG *pData);              /* uidcpet.c,   Saratoga */
SHORT   UifDiaCPEntry6(KEYMSG *pData);				// uidcpet.c
SHORT   UifDiaOEPEntry(KEYMSG *pData);              /* uidoep.c   R3.0 */

SHORT   UifDiaDeptNoEntry(KEYMSG *pData);           /* uiddept.c   2172 */
SHORT   UifDiaOEPKeyEntry(KEYMSG *pData);           /* uidoeket.c  2172 */
SHORT UifDiaPriceEntry(KEYMSG *pData);              /* uidprcet.c  2172 */
SHORT UifDiaDeptEntry(KEYMSG *pData);              /* uidprcet.c  2172 */
SHORT UifDiaTareEntry(KEYMSG *pData);              /* uidprcet.c  2172 */

SHORT   UifRegInputCheck(KEYMSG *pData,UCHAR uchLen);           /* uifrgcom.c */
SHORT   UifRegInputCheckWOAmount(KEYMSG *pData,UCHAR uchLen);   /* uifrgcom.c */
SHORT   UifRegVoidNoCheck(VOID);                                /* uifrgcom.c */
VOID    UifRegWorkClear(VOID);                                  /* uifrgcom.c */
LONG    UifRegShiftQty(LONG lData,UCHAR uchDigit);              /* uifitem.c */
SHORT   UifRegInputZeroCheck(KEYMSG *pData, UCHAR uchLen);      /* uifrgcom.c, Saratoga */

SHORT   UifRegStringWaitEvent(USHORT usEventNumber);                 /* R3.0 */
VOID    UifRegPauseDisp(UCHAR uchLead);                         /* R3.0 */
//VOID    UifRegPauseDisp(USHORT uchLead);                         /* R3.0 */ //CALLSTRING
SHORT   UifRegSearchKey(UCHAR *pData, UCHAR uchKey, USHORT usLen);/* R3.0 */
//SHORT   UifRegSearchKey(USHORT *pData, UCHAR uchKey, USHORT usLen);/* R3.0 */
SHORT   UifDiaAlphaNameEntry(KEYMSG *pData);                    /* R3.0 */
SHORT   UifDiaSeatNoEntry(KEYMSG *pData);           /* uidseaet.c, R3.1 */
SHORT   UifDiaScrollEntry(KEYMSG *pData);           /* uidsclet.c, R3.1 */

SHORT   UifFillInEmployeeRecord (UIFDIAEMPLOYEE  *pUifDiaEmployee);

SHORT   UifMSR(KEYMSG *pData);                      /* uifmsr.c,   R3.1 */
SHORT   UifSignInMSR(KEYMSG *pData);                /* uifmsr.c,   R3.1 */
SHORT	UifTimeMSR(KEYMSG *pData);				    /* uifmsr.c,   R3.1 */
SHORT   UifCashierSignOutMSR(KEYMSG *pData);        /* uifmsr.c,   R3.1 */
SHORT   UifCashierWaiterMSR(KEYMSG *pData);         /* uifmsr.c,   R3.1 */
SHORT   UifCasSignOutWaiSignInMSR(KEYMSG *pData);   /* uifmsr.c,   R3.1 */
SHORT   UifWaiterSignOutMSR(KEYMSG *pData);         /* uifmsr.c,   R3.1 */
SHORT   UifDiaTransferNoEntry(KEYMSG *pData);       /* uidtsfer.c, R3.1 */
SHORT   UifSeat(KEYMSG *pData);                     /* uifseat.c,  R3.1 */
SHORT   UifSeatEntry(KEYMSG *pData);                /* uifseat.c,  R3.1 */
SHORT   UifBartender(KEYMSG *pData);                /* uifwai.c    R3.1 */
SHORT   UifBartenderSignIn(KEYMSG *pData);          /* uifwaiin.c, R3.1 */
SHORT   UifBartenderSignOut(KEYMSG *pData);         /* uifwaiot.c, R3.1 */
SHORT   UifTransfer(KEYMSG *pData);                 /* uiftrnsf.c, R3.1 */
SHORT   UifTransfItem(KEYMSG *pData);               /* uiftrnsf.c, R3.1 */
SHORT   UifCursorVoid(KEYMSG *pData);               /* uifcvoid.c, R3.1 */
SHORT   UifDispenserFirst(KEYMSG *pData);           /* uifdspnf.c, R3.1 */
SHORT   UifDispenser(KEYMSG *pData);                /* uifdspns.c, R3.1 */

SHORT   UifCasInt(KEYMSG *pData);                   /* uifcas.c,   R3.3 */
SHORT   UifCasIntSignIn(KEYMSG *pData);             /* uifcasin.c, R3.3 */
SHORT   UifCasIntSignInOutMSR(KEYMSG *pData);       /* uifmsr.c,   R3.3 */

SHORT   UifNewKeySeq(KEYMSG *pData);                /* uifnewks.c  V3.3 */
SHORT   UifNewKeySeqEntry(KEYMSG *pData);           /* uifnra.c    V3.3 */

SHORT   UifNewKeySeqGC(KEYMSG *pData);              /* uifnewgc.c  V3.3 */

SHORT UifPostReceiptSignOut(KEYMSG *pData);         /* uifpstot.c  V3.3 */
SHORT UifCasLockSignInOut(KEYMSG *pData);           /* uifslock.c  V3.3 */
SHORT   UifDiaInsertLockEntry(KEYMSG *pData);       /* uidwinet.c  V3.3 */

SHORT   UifMoneyDec(KEYMSG *pData);                 /* uifmonot.c   */
SHORT   UifMoneyIn(KEYMSG *pData);                  /* uifmonin.c   */
SHORT   UifMoneyTend(KEYMSG *pData);                /* uifmontl.c   */
SHORT   UifMoneyOut(KEYMSG *pData);                 /* uifmonfl.c   */

SHORT   UifFSTotalIn(KEYMSG *pData);                /* uiffsin.c    */
SHORT   UifFoodStamp(KEYMSG *pData);                /* uiffs.c      */
SHORT   UifFSTender(KEYMSG *pData);                 /* uiffstnd.c   */
SHORT   UifWICTran(KEYMSG *pData);                  /* uifwictr.c   */


/****** End of Definition ******/