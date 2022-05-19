/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Notice Board Common Header                        
* Category    : Notice Board , NCR 2170 US Hospitality Model         
* Program Name: NBCOM.H                                                      
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-27-92:00.00.01:H.Yamaguchi: initial                                   
* Apr-17-93:00.00.09:H.Yamaguchi: Adds retry Co and change timer control.                                   
* Sep-07-93:00.00.09:H.Yamaguchi: Same as HPINT model                                    
*          :        :           :                                    
** NCR2172 **
*
* Oct-05-99:01.00.00:M.Teraki   : Added #pragma(...)
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

#define NB_STACK              (((500+150)*2)/2) /* Stack Size, Saratoga   */
#define NB_WITH_BACKUP_MASTER 1              /* With Backup Master System */

/* Define function code for Zig Zag */
#define NB_FCSTART_MASK       0x0370
#define NB_FCSTARTASMASTER    0x0160         /* Starts as master           */
#define NB_FCSTARTONLINE      0x0260         /* Starts online information  */

/* Define Descriptor control for 2X20 */

#define NB_MASTER_DESC        1              /* Master descriptor               */
#define NB_BMASTER_DESC       2              /* B-Master descriptor             */
#define NB_DELAY_BALANCE_DESC	  17	//Delayed Balance JHHJ
#define NB_RSTFORDESC         0x0f003        /* Reset for descriptor control    */
#define NB_DESCRIPTOR_OFF     0x8001         /* Disply off  M and BM descriptor */

/* Define state control */

#define NB_RSTLOWSTATE        0x0ff00        /* Reset lower 8 bit state control    */
#define NB_NORMALSTATE        0x0            /* Normal state  **                   */
#define NB_WAITSTATE          0x0100         /* Wait   state  ** wait for starting */
#define NB_WAITSTATE_DELAY    0x0101         /* Wait   state  ** delay to send     */
#define NB_SENDSTATE          0x0200         /* Send   state  **                   */

/* Define Sleep timer for Waite state */

#define NB_SLEEPTIME          2000           /* 2000 mili seconds  */

#define NB_MDCTIMER             9            /* MDC address 9 and 10       */
#define NB_TMDEFAULT         5000L           /* Default 5 sec. timer value */
#define NB_TMBALANCE         2000L           /* Use for M and BM 2 sec.    */
#define NB_RCVMAXFIX        60000L           /* Recive 60 sec. timer value */

/* Use timer for Power up condition until executing NbStart Function */
#define NB_TMPOWERUP       (NB_TMDEFAULT - NB_TMBALANCE)/1000L 
#define NB_TMDEFAULT_SEC   (NB_TMDEFAULT)/1000L     /* Default 5 sec. timer */

/* Define Others */

#define NB_MASTERUADD           1            /* Master Unique address       */
#define NB_BMASTERUADD          2            /* B-Master Unique address     */
#define NB_SENDMAXTIMES        12            /* Send Max Times              */
#define NB_MAXLIMIT             5            /* Response time out Max Limit */
#define NB_RETRYMAXTIMES       10            /* Retry Max Times             */
#define NB_DEFAULT_TIMES        5            /* Wait for 15 seconds at BM   */
#define NB_MASK_MDC_TIME     0x0f            /* Mask for MDC Time (0 - 15)  */
#define NB_MASKFUNC          0xfff0          /* Mask for function code      */
#define NB_REDISPLAY				50


/*
*---------------------------------------------------------------------------
*   Define Return Code 
*---------------------------------------------------------------------------
*/

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


typedef struct {
        USHORT  usNbFunCode;
        USHORT  usSeqNO;
        USHORT  fsSystemInf;
        USHORT  fsBackupInf;
		ULONG   ulParaHashCurrent;
        TCHAR   auchMessage[NB_MAX_MESSAGE];
}NBCONTMES;

typedef struct {
        XGHEADER    NbMesHed;
        NBCONTMES   NbConMes;
}NBSENDMES;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/
extern NBMESSAGE   NbSysFlag;          /* Retains sytem control information */

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern NBSENDMES   NbSndBuf;           /* Sends    buffer */
extern NBSENDMES   NbRcvBuf;           /* Receives buffer */

extern USHORT  usNbRcvFMT;              /* Received a message from Master   */
extern USHORT  usNbRcvFBM;              /* Received a message from B-Master */
extern USHORT  usNbSndCnt;              /* Controls a request same message counter */
extern USHORT  usNbPreDesc;             /* Saves previous descriptor data   */

extern USHORT  husNbSemHand;            /* Controls to read/write flags     */
extern USHORT  usNbStack[NB_STACK];     /* Stack size                       */
extern USHORT  husNbBkupSem;            /* Controls to Backup Copy 02/06/2001 */

/*--- Check for net work condition for system ------------------------------*/
extern SHORT   sNbSavRes;               /* Send counter of confirm msg */
/*--------------------------------------------------------------------------*/ 

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

/*--------------------------------------------------------------
    NBMAIN.C  (N.B. main routine ) 
--------------------------------------------------------------*/
VOID   THREADENTRY NbMain(VOID);          /* Starts NB Task       */
VOID   NbInitialize(VOID);                /* Initialize           */
VOID   NbExecuteNormal(VOID);             /* Execute Normal state */
VOID   NbExecuteSend(USHORT fsSystemInf); /* Execute Send state   */
VOID   NbErrResHand(SHORT sRet);          /* Execute Error Response Handling */
VOID   NbRcvMesHand(VOID);                /* Receive message handling */
VOID   NbExecuteWait(USHORT fsSystemInf); /* Execute Wait state   */
VOID   THREADENTRY NbMainCE(VOID);        /* Starts NB Task,    V3.3 */

/*--------------------------------------------------------------
    NBDESC.C  (N.B. display descruptor routine ) 
--------------------------------------------------------------*/
VOID   NbDescriptor(USHORT fsSystemInf);          /* Display descriptor   */
VOID   NbDispWithoutBoard(USHORT fsSystemInf);    /* Display desc., when without a board */

/*--------------------------------------------------------------
    NBGETMDC.C  (N.B. calculate receive time routine ) 
--------------------------------------------------------------*/
LONG   NbGetRcvTime(VOID);                /* Get receive timer value */
USHORT NbGetMDCTime(VOID);                /* Get MDC timer value */

/*--------------------------------------------------------------
    NBNET.C  (N.B. net work control routine ) 
--------------------------------------------------------------*/
SHORT  NbNetOpen(VOID);                   /* Open net work        */
SHORT  NbNetReceive(VOID);                /* Receive data         */
SHORT  NbNetSend(VOID);                   /* send data            */
SHORT  NbNetReOpen(VOID);                 /* Re-open net work     */
SHORT  NbNetChkBoard(USHORT usUAdd);      /* Check communication board */
VOID   NbNetDelaytoSend(VOID);            /* Delay time until reach to next send time  */           
SHORT  NbNetClear(VOID);                  /* Clear network (02/08/2001) */

/*--------------------------------------------------------------
    NBSUB.C  (N.B. sub routine ) 
--------------------------------------------------------------*/
#if 1
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   --  NbCheckOffline and NbPreCheckOffline debug enabled  --    ====++++====")
SHORT  NbCheckOffline_Debug(SHORT sRet, char *aszFilePath, int iLineNo);       /* Check and set/reset online flag */
SHORT  NbPreCheckOffline_Debug(char *aszFilePath, int iLineNo);                /* Precheck offline      */
#define NbCheckOffline(sRet) NbCheckOffline_Debug(sRet,__FILE__, __LINE__)
#define NbPreCheckOffline() NbPreCheckOffline_Debug(__FILE__, __LINE__)
#else
SHORT  NbCheckOffline(SHORT  sRet);     /* Check and set/reset online flag */
SHORT  NbPreCheckOffline();             /* Precheck offline      */
#endif

VOID   NbMakeSndMessage(VOID);            /* Make up a message to send */
USHORT NbCheckUAddr(VOID);                /* Check terminal address    */
VOID   NbExecRcvAsMMes(VOID);             /* Execute a received as master's message    */
VOID   NbExecRcvMonMes(VOID);             /* Execute a received online monitor message */
VOID   NbSetFCtoSndBuf(USHORT usFunCode); /* Set function code and reset atsrt flag    */
VOID   NbEndThread(VOID);                 /* Go to EndThread or Abort */
VOID   NbExecRcvSetOnline(VOID);          /* Set online flag */
SHORT  NbMakeConfirmMessage(SHORT sTermNo, USHORT fsSystemInf); /* Precheck offline counter */
SHORT   NbCheckCpmEpt(VOID);              /* Check CPM/EPT System,  V3.3 */
 
/*========== END OF DEFINITION ============*/
