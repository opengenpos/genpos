/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.3              ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server CASHIER module, Header file for user                        
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCAS.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date          Ver.Rev :NAME         :Description
*   May-06-92   : 00.00.01:M.YAMAMOTO   :Initial
*   Jun-06-93   : 00.00.01:H.YAMAGUCHI  :Change No of cashier from 16 to 32
*   Nov-06-95   : 03.01.00 : T.Nakahata  : R3.1 Initial
*   Sep-02-98   : 03.03.00 : A.Mitsui    : V3.3 Initial
*
*   Jan-24-00   : 01.00.00 : K.Yanagida  : Saratoga 
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

#define CAS_NUMBER_OF_MAX_CASHIER		MAX_NO_CASH	/* number of cashier (max) */ /* V3.3 */
#define CAS_NUMBER_OF_CASHIER_NAME		20			/* number of cashier name */ /* V3.3 */
#define CAS_NUMBER_OF_CASHIER_STATUS	6			/* Width of cashier status (CHAR) */
#define CAS_COUNTER_OF_SEM				1			/* counter of semaphore */
#define CAS_PARA_EMPTY_TABLE_SIZE		(CAS_NUMBER_OF_MAX_CASHIER / 8) + ((7 + (CAS_NUMBER_OF_MAX_CASHIER%8)) / 8)
													/* cashier empty table size */
#define CAS_TERMINAL_OF_CLUSTER			16			/* number of terminal in a cluster */
#define CAS_FILE_HED_POSITION			0x0L		/* for file rewind */

#define CAS_CASHIERNO_SIZE				2			/* Cashier No size */
#define CAS_BACKUP_WORK_SIZE			450			/* Buffer size for back up */

/*------------------------------
    DEFINE FOR CASHIER STATUS
        fbCashierStatus[0]
------------------------------*/ 
        
#define CAS_NOT_CLOSE_OWN_CHECK     0x01        /* not close own check */
#define CAS_NOT_NON_GUEST_CHECK     0x02        /* not non-guest check transaction */
#define CAS_NOT_GUEST_CHECK_OPEN    0x04        /* not guest check open */
#define CAS_TRAINING_CASHIER        0x08        /* training cashier status */
#define CAS_NOT_SINGLE_ITEM         0x10        /* not single item sales */
#define CAS_NOT_TRANSFER_TO         0x20        /* not check transfer from */
#define CAS_NOT_TRANSFER_FROM       0x40        /* not check transfer to */
#define CAS_NOT_CLOSE_ANY_CHECK     0x80        /* not close any check */

/*------------------------------
    DEFINE FOR CASHIER STATUS
        fbCashierStatus[1]
------------------------------*/ 

#define CAS_NOT_SURROGATE           0x01        /* not surrogate sign-in */
#define CAS_FINALIZE_NON_GCF        0x02        /* release on finalize non-gcf */
#define CAS_FINALIZE_GCF            0x04        /* release on finalize gcf */
#define CAS_SECRET_CODE             0x08        /* requeset secret code */
#define CAS_NOT_CAS_INT             0x10        /* not cashier interrupt */
#define CAS_USE_TEAM                0x20        /* use team */
#define CAS_NOT_DRAWER_A            0x40        /* not drawer A, 2172 */
#define CAS_NOT_DRAWER_B            0x80        /* not drawer B, 2172 */

/*------------------------------
    DEFINE FOR CASHIER STATUS
        fbCashierStatus[2]
------------------------------*/ 

#define CAS_OPEN_CASH_DRAWER_SIGN_IN	0x01	/* Opens the cash drawer on sign-in ***PDINU*/
#define CAS_ORDER_DEC_DEFAULT			0x02	/* We use terminal defaults order dec*/
#define CAS_ORDER_DEC_FORCE				0x04	/* We require order declare before itemization*/
#define CAS_OPEN_CASH_DRAWER_SIGN_OUT	0x08	// Opens the cash drawer on sign-Out ***PDINU
#define CAS_MSR_MANUAL_SIGN_IN			0x10	// JMASON Flag to prevent manual signing when using MSR

/*------------------------------
    DEFINE FOR CASHIER STATUS
        fbCashierOption
------------------------------*/ 

#define CAS_WAI_SIGN_IN             0x01        /* waiter/surrogate sign-in */
#define CAS_MISC_SIGN_IN            0x02        /* misc. sign-in */

/*------------------------------
    DEFINE FOR RETURN VALUE
------------------------------*/

/* External error cede for User interface */

#define CAS_PERFORM              0
#define CAS_FILE_NOT_FOUND      -1
#define CAS_CASHIER_FULL        -2
#define CAS_NOT_IN_FILE         -3
#define CAS_LOCK                -5
#define CAS_NO_MAKE_FILE        -6
#define CAS_NO_COMPRESS_FILE    -7

#define CAS_ALREADY_LOCK        -10
#define CAS_OTHER_LOCK          -11
#define CAS_DURING_SIGNIN       -12
#define CAS_DIFF_NO             -13
#define CAS_SECRET_UNMATCH      -14
#define CAS_NO_SIGNIN           -15
#define CAS_ALREADY_OPENED      -16
#define CAS_DELETE_FILE         -17
#define CAS_REQUEST_SECRET_CODE -18
#define CAS_NOT_ALLOWED         -19

#define CAS_NOT_MASTER          -20

/* Internal error cede  */

#define CAS_NOT_SIGNIN          -31
#define CAS_UNLOCK              -32
#define CAS_RESIDENT            -33
#define CAS_REGISTED            -34
#define CAS_ALLLOCK             -35
#define CAS_EOF                 -36
#define CAS_COMERROR            -37

#define CAS_CONTINUE            -40
#define CAS_NO_READ_SIZE        -41        

/*
===========================================================================
    TYPEDEF
===========================================================================
*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct{
    UCHAR   fbCashierStatus[CAS_NUMBER_OF_CASHIER_STATUS];	//doesn't seem right
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;
    UCHAR   uchTerminal;
    UCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME];
    UCHAR   uchCashierSecret;
}CAS_PARAENTRY_HGV10;

typedef struct{
    UCHAR   fbCashierStatus[2];
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;
    UCHAR   uchTerminal;
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME];
    UCHAR   uchCashierSecret;
}CAS_PARAENTRY_HGV203;

typedef struct{
    UCHAR   fbCashierStatus[3];
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;
    UCHAR   uchTerminal;
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME ];
    UCHAR   uchCashierSecret;
}CAS_PARAENTRY_HGV210;

typedef struct{
    UCHAR   fbCashierStatus[3];
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;
    UCHAR   uchTerminal;
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME ];
    ULONG   ulCashierSecret;
}CAS_PARAENTRY_HGV212;		//any changes should be synchronized with CASIF

typedef struct{
    UCHAR   fbCashierStatus[CAS_NUMBER_OF_CASHIER_STATUS];
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;
    UCHAR   uchTerminal;
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME ];
    ULONG   ulCashierSecret;
	USHORT	usSupervisorID;
	USHORT	usCtrlStrKickoff;
	USHORT	usStartupWindow;
	ULONG	ulGroupAssociations;
}CAS_PARAENTRY_HGV220;		//any changes should be synchronized with CASIF

typedef struct{
    UCHAR   fbCashierStatus[CAS_NUMBER_OF_CASHIER_STATUS];
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;
    UCHAR   uchTerminal;
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME ];
    ULONG   ulCashierSecret;
	USHORT	usSupervisorID;
	USHORT	usCtrlStrKickoff;
	USHORT	usStartupWindow;
	ULONG	ulGroupAssociations;
}CAS_PARAENTRY;		//any changes should be synchronized with CASIF

/*----------------------
    USER I/F
----------------------*/
typedef struct{
    USHORT  usCashierNo;
    UCHAR   fbCashierStatus[CAS_NUMBER_OF_CASHIER_STATUS];  /* V3.3 */
    USHORT  usGstCheckStartNo;                              /* V3.3 */
    USHORT  usGstCheckEndNo;                                /* V3.3 */
    UCHAR   uchChgTipRate;                                  /* V3.3 */
    UCHAR   uchTeamNo;                                      /* V3.3 */
    UCHAR   uchTerminal;                                    /* V3.3 */
    UCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME];
    UCHAR   uchCashierSecret;
    USHORT  usUniqueAddress;
    UCHAR   fbCashierOption;                                /* V3.3 */
}CASIF_HGV10;

typedef struct{
    USHORT  usCashierNo;
    UCHAR   fbCashierStatus[2];  /* V3.3 */
    USHORT  usGstCheckStartNo;                              /* V3.3 */
    USHORT  usGstCheckEndNo;                                /* V3.3 */
    UCHAR   uchChgTipRate;                                  /* V3.3 */
    UCHAR   uchTeamNo;                                      /* V3.3 */
    UCHAR   uchTerminal;                                    /* V3.3 */
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME + 1];
    UCHAR   uchCashierSecret;
    USHORT  usUniqueAddress;
    UCHAR   fbCashierOption;                                /* V3.3 */
}CASIF_HGV203;

typedef struct{
    USHORT  usCashierNo;
    UCHAR   fbCashierStatus[3];  /* V3.3 */
    USHORT  usGstCheckStartNo;                              /* V3.3 */
    USHORT  usGstCheckEndNo;                                /* V3.3 */
    UCHAR   uchChgTipRate;                                  /* V3.3 */
    UCHAR   uchTeamNo;                                      /* V3.3 */
    UCHAR   uchTerminal;                                    /* V3.3 */
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME + 1];
    UCHAR   uchCashierSecret;
    USHORT  usUniqueAddress;
    UCHAR   fbCashierOption;                                /* V3.3 */
}CASIF_HGV210;

typedef struct{
    ULONG	ulCashierNo;
    UCHAR   fbCashierStatus[3];  /* V3.3 */
    USHORT  usGstCheckStartNo;                              /* V3.3 */
    USHORT  usGstCheckEndNo;                                /* V3.3 */
    UCHAR   uchChgTipRate;                                  /* V3.3 */
    UCHAR   uchTeamNo;                                      /* V3.3 */
    UCHAR   uchTerminal;                                    /* V3.3 */
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME + 1];
    ULONG   ulCashierSecret;
    USHORT  usUniqueAddress;
    ULONG   fbCashierOption;                                /* V3.3 */
}CASIF_HGV212;		//any changes should be synchronized with CAS_PARAENTRY

typedef struct{
    ULONG	ulCashierNo;
    UCHAR   fbCashierStatus[CAS_NUMBER_OF_CASHIER_STATUS];  /* V3.3 */
    USHORT  usGstCheckStartNo;                              /* V3.3 */
    USHORT  usGstCheckEndNo;                                /* V3.3 */
    UCHAR   uchChgTipRate;                                  /* V3.3 */
    UCHAR   uchTeamNo;                                      /* V3.3 */
    UCHAR   uchTerminal;                                    /* V3.3 */
    WCHAR   auchCashierName[CAS_NUMBER_OF_CASHIER_NAME + 1];
    ULONG   ulCashierSecret;
    USHORT  usUniqueAddress;
    ULONG   fbCashierOption;                                /* V3.3 */
	USHORT	usSupervisorID;
	USHORT	usCtrlStrKickoff;
	USHORT	usStartupWindow;
	ULONG	ulGroupAssociations;
}CASIF;		//any changes should be synchronized with CAS_PARAENTRY

typedef struct{
    ULONG   ulCasFileSize;
    USHORT  usNumberOfMaxCashier;       /* V3.3 */
    USHORT  usNumberOfResidentCashier;  /* V3.3 */
    USHORT  offusOffsetOfIndexTbl;
    USHORT  offusOffsetOfParaEmpTbl;
    USHORT  offusOffsetOfParaTbl;
}CAS_FILEHED;

typedef struct{
    USHORT  usCashierNo;
    USHORT  usParaBlock;                /* V3.3 */
}CAS_INDEXENTRY_HGV210;

typedef struct{
    ULONG	ulCashierNo;
    USHORT  usParaBlock;                /* V3.3 */
}CAS_INDEXENTRY_HGV220;

typedef struct{
    ULONG	ulCashierNo;
    USHORT  usParaBlock;                /* V3.3 */
}CAS_INDEXENTRY;

/* ------ Cashier old file structure (R3.1) for migration ----- */
typedef struct{
    ULONG   ulCasFileSize;
    UCHAR   uchNumberOfMaxCashier;
    UCHAR   uchNumberOfResidentCashier;
    USHORT  offusOffsetOfIndexTbl;
    USHORT  offusOffsetOfParaEmpTbl;
    USHORT  offusOffsetOfParaTbl;
}CAS_FILEHEDOLD;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*----------------------
    File Name
----------------------*/
extern WCHAR FARCONST  auchCAS_CASHIER[];
extern WCHAR FARCONST  auchCAS_CASHIEROLD[];

/*----------------------
    USER I/F
----------------------*/

VOID    CasInit			(VOID);
SHORT   CasCreatFile	(USHORT usNumberOfCashier);
SHORT   CasCheckAndCreatFile(USHORT usNumberOfCashier);
SHORT   CasCheckAndDeleteFile(USHORT usNumberOfCashier);
SHORT   CasSignIn		(CASIF *Casif);
SHORT   CasSignOut		(CASIF *Casif);
SHORT   CasClose		(ULONG ulCashierNo);
SHORT   CasAssign		(CASIF *Casif);
SHORT   CasDelete		(ULONG ulCashierNo);
SHORT   CasSecretClr	(ULONG ulCashierNo);
SHORT   CasAllIdRead	(USHORT usRcvBufferSize, ULONG *ausRcvBuffer);
SHORT   CasIndRead		(CASIF *Casif);
SHORT   CasCheckFileBlockData (VOID);
SHORT   CasIndLock		(ULONG ulCashierNo);
SHORT   CasIndUnLock	(ULONG ulCashierNo);
SHORT   CasAllLock		(VOID);
VOID    CasAllUnLock	(VOID);
SHORT   CasIndTermLock	(USHORT usTerminalNo);    /* R3.1 */
SHORT   CasIndTermUnLock(USHORT usTerminalNo);  /* R3.1 */
SHORT   CasAllTermLock	(VOID);                   /* R3.1 */
VOID    CasAllTermUnLock(VOID);                 /* R3.1 */
SHORT   CasChkSignIn	(ULONG *aulRcvBuffer);
SHORT   CasSendFile		(VOID);
SHORT   CasRcvFile		(UCHAR  *puchRcvData, USHORT usRcvLen, UCHAR  *puchSndData, USHORT *pusSndLen);

USHORT  CasConvertError	(SHORT sError);
SHORT   CasPreSignOut	(CASIF *Casif);
SHORT   CasMigrate31File(VOID);									/* V3.3, PEP Only */
SHORT   CasDeleteFile();										/* V3.3, PEP Only */
SHORT   CasMigrateHGV10File(VOID);								/* R10 to R20 Migration JHHJ*/
SHORT   CasMigrateHGV203File(VOID);								// R203 to R21 Migration ***PDINU
SHORT	CasIndReadHGV10	(CASIF_HGV10 *pCasif, USHORT usParaBlockNo, CAS_FILEHED *CasFileHed);

/*======= End of Define ========*/
