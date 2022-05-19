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
*   Title              : Client/Server Report module, Header file for user                        
*   Category           : Client/Server Report module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : RPTCOM.H                                            
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date          Ver.Rev  :NAME         :Description
*   Jun-16-93   : 00.00.01 :M.YAMAMOTO   :Initial
*   Jul-07-93   : 01.00.12 : J.IKEDA     : Adds RptRound()
*   Jul-20-93   : 01.00.12 : K.You       : Adds RPT_NO_OF_TERM.
*   Oct-07-93   : 02.00.01 : M.SUZUKI    : Adds RptHourChkFileExist.
*   Dec-10-93   : 00.00.01 : M.SUZUKI    : Adds RPT_WAIT_TIMER,RPT_EXIST_CREDIT,
*                                               RPT_EXIST_HASH.
*   Oct-21-97   : 03.01.15 : hrkato      : ETK Bug Fix
** NCR2172 **
*
*   Oct-05-99   : 01.00.00 : M.Teraki    : Added #pragma(...)
*
*===========================================================================
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*
===========================================================================
    DEFINE
===========================================================================
*/

#define     RPT_NO_OF_TERM      16    /* No. of Terminal,  PIF_NET_MAX_IP */

/*  
===========================================================================
    PROTOTYPE
===========================================================================
*/
extern    USHORT  usRptLockHnd;
extern    USHORT  usRptPrintStatus;
extern    UCHAR   uchRptWgtFlag;
extern    UCHAR   uchRptMldAbortStatus;  /* abort status by LCD R3.0 */
    
#define USRPTCASLENTTL  (sizeof(TTLCASHIER) - (sizeof(TtlCas.uchMajorClass)\
                                                + sizeof(TtlCas.uchMinorClass)\
                                                + sizeof(TtlCas.uchResetStatus)\
                                                + sizeof(TtlCas.FromDate)\
                                                + sizeof(TtlCas.ToDate)\
                                                + sizeof(TtlCas.ulCashierNumber)))

#define USRPTWAILENTTL  (sizeof(TTLWAITER) - (sizeof(TtlWai.uchMajorClass) + \
                                               sizeof(TtlWai.uchMinorClass) + \
                                               sizeof(TtlWai.uchResetStatus) + \
                                               sizeof(TtlWai.FromDate) + \
                                               sizeof(TtlWai.ToDate) + \
                                               sizeof(TtlWai.ulWaiterNumber)))

#define USRPTFINLENTTL  (sizeof(TTLREGFIN) - (sizeof(TtlData.uchMajorClass) + \
                                               sizeof(TtlData.uchMinorClass) + \
                                               sizeof(TtlData.uchResetStatus) + \
                                               sizeof(TtlData.FromDate) + \
                                               sizeof(TtlData.ToDate) + \
                                               sizeof(TtlData.usTerminalNumber)))

#define RPT_WAIT_TIMER          50             /* Sleep Timer */
#define RPT_EXIST_CREDIT         1             /* Exist CREDIT */
#define RPT_EXIST_HASH           2             /* Exist HASH */



typedef struct {
                    UCHAR  uchMDEPT;
                    USHORT usDEPT;
                    TCHAR  auchMnemo[OP_DEPT_NAME_SIZE];
                    UCHAR  uchStatus;
                    LTOTAL DEPTTotal;
                } DEPTTBL;

typedef struct {
                    USHORT usCpnNo;
                    TCHAR  auchMnemo[STD_CPNNAME_LEN];
                    TOTAL CPNTotal;
                } CPNTBL;

typedef struct {
        ULONG   ulWageTotal[48];        /* V3.3 */
} RPTHOURLYETK;

typedef struct {
        UCHAR   uchJobCode;              /* Job Code to be time in */
        USHORT  usInMinute;              /* Minute to be time in */
        USHORT  usOutMinute;             /* Minute to be time out */
} RPTETKWORKFIELD;

SHORT   RptHourlyAllEdit( TTLHOURLY *pTtlData, UCHAR uchMinorClass, RPTHOURLYETK *pRptHourlyEtk);
SHORT   RptHourlyRngEdit( TTLHOURLY *pTtlData, UCHAR uchMinorClass, RPTHOURLYETK *pRptHourlyEtk);

SHORT  RptDEPTTableAll( UCHAR   uchMinorClass, DEPTTBL *pTable);
SHORT  RptDEPTTableAllLocal(UCHAR   uchMinorClass, DEPTTBL *pTable);


VOID   RptPLUIndOpen( PLUIF  *pPluIf,
                      TTLPLU *pTtlPlu,
                      UCHAR uchMinor,
                      UCHAR uchType);
SHORT  RptPLUIndAdj( PLUIF  *pPluIf,
                     TTLPLU *pTtlPlu,
                     UCHAR uchMinor,
                     UCHAR uchType);
SHORT  RptPLUCodeAdj( PLUIF_REP  *pPluIf_Rep,
                      TTLPLU     *pTtlPlu,
                      UCHAR      uchMinor,
                      UCHAR      *puchFlag,
                      UCHAR      *puchScaleIn,
                      UCHAR      uchType);
VOID   RptPLUCodeOpen( PLUIF_REP  *pPluIf_Rep,
                       TTLPLU     *pTtlPlu,
                       UCHAR      uchMinor,
                       UCHAR      *puchFlag,
                       UCHAR      *puchScaleIn,
                       UCHAR      uchType);
/* 2172 */
SHORT  RptPLUAdj(PLUIF      *pPluIf,
                 TTLPLU     *pTtlPlu,
                 UCHAR      uchMinor,
                 UCHAR      *puchFlag,
                 UCHAR      *puchScaleIn,
                 UCHAR      uchType);
/* 2172 */        
VOID   RptPLUOpen(PLUIF     *pPluIf,
                  TTLPLU     *pTtlPlu,
                  UCHAR      uchMinor,
                  UCHAR      *puchFlag,
                  UCHAR      *puchScaleIn,
                  UCHAR      uchType);
/*2172 */
SHORT  RptPLUACTAdj( PLUIF     *pPluIf,
                     TTLPLU     *pTtlPlu,
                     UCHAR      uchMinor,
                     UCHAR      *puchFlag,
                     UCHAR      *puchScaleIn,
                     UCHAR      uchType);
/* 2172 */
VOID   RptPLUACTOpen( PLUIF     *pPluIf,
                      TTLPLU     *pTtlPlu,
                      UCHAR      uchMinor,
                      UCHAR      *puchFlag,
                      UCHAR      *puchScaleIn,
                      UCHAR      uchType);

SHORT  RptPLUByDEPTAdj( PLUIF_DEP  *pPluIf_Dep, 
                        TTLPLU *pTtlPlu,
                        UCHAR uchMinor,
                        UCHAR *puchFlag,
                        UCHAR uchType,
                        USHORT usDEPTNo,
                        UCHAR *puchScaleIn);
VOID   RptPLUByDEPTOpen( PLUIF_DEP  *pPluIf_Dep, 
                         TTLPLU *pTtlPlu,
                         UCHAR uchMinor,
                         UCHAR *puchFlag,
                         UCHAR uchType,
                         USHORT usDEPTNo,
                         UCHAR *puchScaleIn);
UCHAR  RptPLUPrintChk1( TTLPLU  *pTtPlu, 
                        UCHAR uchContOther,
                        UCHAR uchPluProhibitVal);
UCHAR  RptPLUPrintChk2( TTLPLU  *pTtlPlu, UCHAR uchContOther, UCHAR uchPluProhibitVal);
UCHAR  RptHourGetBlock(VOID);


SHORT RptETKCalHourlyTotal(UCHAR uchMinorClass, N_DATE *pFromDate, N_DATE *pToDate, RPTHOURLYETK *pRptHourlyEtk);

/*
***********************************
*--- Report Common ---------------*
***********************************
*/
DCURRENCY RptRound( DCURRENCY lAmount, DSCOUNTER sDivisor );

VOID   TtlGetNdate (N_DATE *pnDate);


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*====== End of Header ======*/
