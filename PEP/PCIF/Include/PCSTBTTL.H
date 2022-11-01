#if POSSIBLE_DEAD_CODE
/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server PC STUB module, Header file for user                        
*   Category           : Client/Server PC STUB Total interface
*   Program Name       : PCSTBTTL.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    May-21-92      :H.NAKASHIMA        :Initial
*    Aug-23-95      :T.Koyama           :Add SerTtlTotalReadPlu()
*    Sep-07-98      :O.Nakada           :Delete SerTtlWaiTotalCheck()
*
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
/* ###Add Saratoga */

/*--------------------------------------------------------------
    USHORT  usFunc; Define function either Pickup or Loan.
--------------------------------------------------------------*/
#define ISP_FUNC_LOAN                1  /* Execute loan   function */
#define ISP_FUNC_PICKUP              2  /* Execute piclup function */

/*--------------------------------------------------------------
    UCHAR   uchOpeMode; Define operation mode for Pickup/Loan
--------------------------------------------------------------*/
#define ISP_PICKUP_NOMAL_OP         1  /* Execute Normal operation       */
#define ISP_PICKUP_VOID_OP          2  /* Execute E/C or Void operation  */

#define ISP_CLASS_TUM_TEND1          1  /* Tender #1(Cash)              */
#define ISP_CLASS_TUM_TEND2          2  /* Tender #2(Food)              */
#define ISP_CLASS_TUM_TEND3          3  /* Tender #3(Charge)            */
#define ISP_CLASS_TUM_TEND4          4  /* Tender #4(Misc1)             */
#define ISP_CLASS_TUM_TEND5          5  /* Tender #5(Misc2)             */
#define ISP_CLASS_TUM_TEND6          6  /* Tender #6(Misc3)             */
#define ISP_CLASS_TUM_TEND7          7  /* Tender #7(Misc4)             */
#define ISP_CLASS_TUM_TEND8          8  /* Tender #8(Misc5)             */
#define ISP_CLASS_TUM_TEND9          9  /* Tender #9(Misc6)             */
#define ISP_CLASS_TUM_TEND10        10  /* Tender #10(Misc7)            */
#define ISP_CLASS_TUM_TEND11        11  /* Tender #11(Misc8)            */
#define ISP_CLASS_TUM_FCURRENCY1    21  /* Foreign Currency #1          */
#define ISP_CLASS_TUM_FCURRENCY2    22  /* Foreign Currency #2          */
#define ISP_CLASS_TUM_FCURRENCY3    23  /* Foreign Currency #3          */
#define ISP_CLASS_TUM_FCURRENCY4    24  /* Foreign Currency #4          */
#define ISP_CLASS_TUM_FCURRENCY5    25  /* Foreign Currency #5          */
#define ISP_CLASS_TUM_FCURRENCY6    26  /* Foreign Currency #6          */
#define ISP_CLASS_TUM_FCURRENCY7    27  /* Foreign Currency #7          */
#define ISP_CLASS_TUM_FCURRENCY8    28  /* Foreign Currency #8          */

/*
===========================================================================
    TYPEDEF
===========================================================================
*/
/*
*===========================================================================
   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
    USHORT  usFunc;                     /* Function        */
    UCHAR   uchMedia;                   /* Media Class     */
    UCHAR   uchOpeMode;                 /* Operation Mode  */
    ULONG   ulTotal;                    /* Total           */
    USHORT  usCount;                    /* Counter         */
} ISPPICKUP;
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
    PROTOTYPE

      NOTE:  New functions added to this library
         may also need to have an entry point
		 added to the file NCR2172Stub.def in
		 order to make the entry point visible.

         You will know to do this if you see
		 link errors with your new function specifed
		 as not found.

===========================================================================
*/

/*---------------------------------------
    USER I/F TOTAL FUNCTION (SERVER)
----------------------------------------*/

SHORT    STUBENTRY   SerTtlTotalRead(VOID FAR *pTotal);
SHORT    STUBENTRY   SerTtlTotalReadPlu(VOID FAR *pTotal);
SHORT    STUBENTRY   SerTtlTotalReset(VOID FAR *pTotal, USHORT usMDCBit);
/* SHORT    STUBENTRY   SerTtlTotalReset(VOID *pTotal, UCHAR uchMDCBit); */
SHORT    STUBENTRY   SerTtlIssuedSet(VOID FAR *pTotal, UCHAR fIssued);
SHORT    STUBENTRY   SerTtlIssuedCheck(VOID FAR *pTotal, UCHAR fIssued);
SHORT    STUBENTRY   SerTtlIssuedReset(VOID FAR *pTotal, UCHAR fIssued);
SHORT    STUBENTRY   SerTtlTotalCheck(VOID FAR *pTotal);
/* SHORT    STUBENTRY   SerTtlWaiTotalCheck(VOID); */
SHORT    STUBENTRY   SerTtlCasTotalCheck(VOID);

/* ###ADD Saratoga */
SHORT    STUBENTRY SerTtlUpdateFile(ISPPICKUP FAR *pData, USHORT usDataLen);
SHORT    STUBENTRY SerTtlTotalReadPluEx(VOID FAR *pTotal);
SHORT   STUBENTRY SerTtlTumUpdateDelayedBalance(VOID);
VOID STUBENTRY SerTtlCloseSavFile();

/*===== END OF DEFINITION =====*/
#endif