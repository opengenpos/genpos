/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : uifsupex.h                                        :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-14 : 00.00.00     :                         :
:   Update Histories   : 92-11-26 : 01.00.00     : Adds ulUifGCumber       :
:                      : 99-11-30 : 01.00.00     : Saratoga                :
:  ----------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00A by Microsoft Corp.                :
:   Memory Model       : Midium Model                                      :
:   Condition          :                                                   :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\

            E X T E R N     D E F I N T I O Ns

\*------------------------------------------------------------------------*/


/* Define UifACEnter() Function */
extern  UIMENU CONST aACEnter[];

/* Define Key Sequence */
extern  UISEQ CONST aszSeqACFSC_AC[];
extern  UISEQ CONST aszSeqACFSC_EC[];
extern  UISEQ CONST aszSeqACFSC_NUM_TYPE[];
extern  UISEQ CONST aszSeqACFSC_CANCEL[];

extern  UISEQ CONST aszSeqACFSC_P1[];
extern  UISEQ CONST aszSeqAC23EnterDataType[];
extern  UISEQ CONST aszSeqAC63EnterData1[];
extern  UISEQ CONST aszSeqAC63EnterData2[];
extern  UISEQ CONST aszSeqAC63Error[];
extern  UISEQ CONST aszSeqAC3EnterKey[];
extern  UISEQ CONST aszSeqAC26EnterDEPT1[];
extern  UISEQ CONST aszSeqAC29EnterDEPT1[];
extern  UISEQ CONST aszSeqAC29EnterPLU1[];
extern  UISEQ CONST aszSeqAC30EnterData[];
//GroupReport - CSMALL
extern	UISEQ CONST aszSeqAC31EnterDataType[];


/* Define RAM Erea */

extern  USHORT  usUifACControl;                 /* Ui Supervisor Mode Control Save Area */
extern  UCHAR   uchUifPage;                     /* Menu Shift Page Save Area */
extern  UCHAR   uchUifACDataType;               /* Data Type Save Area */
extern  UCHAR   uchUifACRstType;                /* Reset Type Save Area */
extern  UCHAR   uchUifACRptType;                /* Report Type Save Area */
extern  UCHAR   uchUifACDescCtl;                /* Descriptor Control Save Area */
extern  USHORT  usUifACRptGcfNo;                /* GC No. Save Area */
extern  USHORT  usUifACRptTermNo;               /* Terminal No. Save Area */
extern  ULONG   ulUifACRptCashierNo;            /* Cashier/Waiter No. Save Area */
extern  SHORT   husUifACHandle;                 /* UIC Handle Save Area */
extern  USHORT  usUifAC160Ctrl;
extern  USHORT  usUifAC33Ctrl;

/* OTHER DEFINE */

#define UISUP_START_CPM     1                           /* A/C 78 TYPE # (START CPM) */
#define UISUP_STOP_CPM      2                           /* A/C 78 TYPE # (STOP CPM) */
#define STS_OEP_TBLNUMBER   1                   /* When OEP Table Number Inputed */
#define STS_OEP_TBLADDRESS  2                   /* When OEP Table Address Inputed */
#define STS_OEP_ACCESS      1                   /* Already OEP Table Accessed */

#define STS_CSTR_ACCESSED   0x01                   /* Already CSTR Table Accessed */
#define STS_CSTR_VOID       0x02                   /* Already CSTR Table VOID */

#define A71_CODE_MIN        1                       /* PPI Min Code Page ESMITH */
#define A71_CODE_MAX        300						/* PPI Max Code Page ESMITH */

#define A33_CSTR_MAX		400					/* Control String Max Value	ESMITH */
#define A33_CSTR_MIN		1					/* Control String MIN Value	ESMITH */
/* Loan/Pick-up,    Saratoga --- */
typedef struct {
    USHORT usModifier;     /* Modifier Key Status */
    LONG   lForQty;        /* For, Qty  */
} UIFAC1011KEYSTATUS;      /* AC10/11 Modifier Key Status Save Area */

UIFAC1011KEYSTATUS UifAC1011KeyStatus;

#define UISUP_DRAWER_A      0x0001                  /* Indicate A drawer (R2.0) */
#define UISUP_DRAWER_B      0x0002                  /* Indicate B drawer (R2.0) */

/* --- End of Header File --- */