
// The waiter functionality no longer exists in GenPOS however there are
// still some vestiges of when it was a part of NHPOS. This removal
// was done with NHPOS source code by NCR before the software was donated
// to Georgia Southern University in 2002. Or it may be that development
// was started and never completed by the Oiso, Japan team at NCR.
//
// I am now ifdefing out the contents of this include file, cswai.h, and the associated
// file csstbwai.h however there are a couple of defines that are still used in several
// parts of the GenPOS source which need to be removed at a later time.
//
// See also use of the define FLEX_WT_ADR which is the address in Flexible Memory
// for the Waiter File size. However this size is no longer used and should be
// a value of zero (0).
//
//    Richard Chambers, Georgia Southern University, Aug-10-2017

#define WAI_PERFORM              0    // same as CAS_PERFORM, still used some places.

#define WAI_FILE_NOT_FOUND      -1    // same as CAS_FILE_NOT_FOUND, still used some places

#if defined(POSSIBLE_DEAD_CODE)

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
*   Title              : Client/Server WAITER module, Header file for user                        
*   Category           : Client/Server WAITER module, NCR2170 HOSPITAL FOR US MODEL
*   Program Name       : CSWAI.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            : /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*    Date        Ver.Rev    :NAME        :Description
*    May-06-92   00.00.01   :M.YAMAMOTO  :Initial
*    Dec-06-95   03.01.00   :M.OZAWA     :Add WaiIndTermLock/WaiIndTermUnLock/
*                                         WaiAllTermLock/WaiIndTermUnLock
*
** NCR2172 **
*
*    Oct-05-99   01.00.00   :M.Teraki   :initial (for 2172)
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

#define WAI_NUMBER_OF_MAX_WAITER   100          /* number of waiter (max) */
#define WAI_NUMBER_OF_WAITER_NAME   10          /* Width of waiter name (CHAR) */
#define WAI_COUNTER_OF_SEM           1          /* counter of semaphore */
#define WAI_PARA_EMPTY_TABLE_SIZE   (((WAI_NUMBER_OF_MAX_WAITER - 1) / 8) + 1)  /* Width of Empty Table */
#define WAI_TERMINAL_OF_CLUSTER     16    


/* 
===========================================================================
    OTHER DEFINE
===========================================================================
*/

#define WAI_FILE_HED_POSITION       0x0L        /* for file rewind */
#define WAI_EOF                     0xff        /* last data for back up (file transfer) */
#define WAI_CONTINUE                0x01        /* exist Next data for back up (File Transfer) */
#define WAI_WAITERNO_SIZE              2        /* WAITER No SIZE */
/*------------------------------
    DEFINE FOR WAITER STATUS
------------------------------*/ 
        
#define WAI_TRAINING_WAITER         0x01        /* training waiter status */

/*------------------------------
    DEFINE FOR RETUR VALUE
------------------------------*/

/* External error code for user interface */

#define WAI_PERFORM              0          

#define WAI_FILE_NOT_FOUND      -1
#define WAI_WAITER_FULL         -2
#define WAI_NOT_IN_FILE         -3
#define WAI_LOCK                -5    
#define WAI_NO_MAKE_FILE        -6 
#define WAI_NO_COMPRESS_FILE    -7 

#define WAI_ALREADY_LOCK        -10
#define WAI_OTHER_LOCK          -11
#define WAI_DURING_SIGNIN       -12
#define WAI_DIFF_NO             -13
#define WAI_SECRET_UNMATCH      -14
#define WAI_NO_SIGNIN           -15
#define WAI_ALREADY_OPENED      -16       
#define WAI_DELETE_FILE         -17       

#define WAI_NOT_MASTER          -20

/* Internal error code for user interface */

#define WAI_NOT_SIGNIN          -31
#define WAI_UNLOCK              -32
#define WAI_RESIDENT            -33
#define WAI_FILE_ERROR          -34
#define WAI_REGISTED            -35
#define WAI_ALLLOCK             -36
#define WAI_COMERROR            -37

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
    UCHAR   fbWaiterStatus;
    TCHAR   auchWaiterName[WAI_NUMBER_OF_WAITER_NAME];
    UCHAR   uchWaiterSecret;
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
}WAI_PARAENTRY;

/*----------------------
    USER I/F
----------------------*/
typedef struct{
    USHORT  usWaiterNo;
    UCHAR   fbWaiterStatus;
    TCHAR   auchWaiterName[WAI_NUMBER_OF_WAITER_NAME];
    UCHAR   uchWaiterSecret;
    USHORT  usUniqueAddress;
    USHORT  usGstCheckStartNo;
    USHORT  usGstCheckEndNo;
}WAIIF;

typedef struct{
    ULONG   ulWaiFileSize;
    UCHAR   uchNumberOfMaxWaiter;
    UCHAR   uchNumberOfResidentWaiter;
    USHORT  offusOffsetOfIndexTbl;
    USHORT  offusOffsetOfParaEmpTbl;
    USHORT  offusOffsetOfParaTbl;
}WAI_FILEHED;

typedef struct{
    USHORT  usWaiterNo;
    UCHAR   uchParaBlock;
}WAI_INDEXENTRY;


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
extern UCHAR FARCONST auchWAI_WAITER[];

/*----------------------
    USER I/F
----------------------*/

VOID    WaiInit(VOID);
SHORT   WaiCreatFile(UCHAR uchNumberOfWaiter);
SHORT   WaiCheckAndCreatFile(UCHAR uchNumberOfWaiter);
SHORT   WaiCheckAndDeleteFile(UCHAR uchNumberOfWaiter);
SHORT   WaiSignIn(WAIIF *Waiif);
SHORT   WaiSignOut(WAIIF *Waiif);
SHORT   WaiClose(USHORT usWaiterNo);
SHORT   WaiAssign(WAIIF *Waiif);
SHORT   WaiDelete(USHORT usWaiterNo);
SHORT   WaiSecretClr(USHORT usWaiterNo);
SHORT   WaiAllIdRead(USHORT usRcvBufferSize,USHORT *ausRcvBuffer);
SHORT   WaiIndRead(WAIIF *Waiif);
SHORT   WaiIndLock(USHORT usWaiterNo);
SHORT   WaiIndUnLock(USHORT usWaiterNo);
SHORT   WaiAllLock(VOID);
VOID    WaiAllUnLock(VOID);
SHORT   WaiIndTermLock(USHORT usTerminalNo);    /* R3.1 */
SHORT   WaiIndTermUnLock(USHORT usTerminalNo);  /* R3.1 */
SHORT   WaiAllTermLock(VOID);                   /* R3.1 */
VOID    WaiAllTermUnLock(VOID);                 /* R3.1 */
SHORT   WaiChkSignIn(USHORT *ausRcvBuffer);
SHORT   WaiSendFile(VOID);
SHORT   WaiRcvFile(UCHAR  *puchRcvData,
                   USHORT usRcvLen,
                   UCHAR  *puchSndData,
                   USHORT *pusSndLen);

USHORT  WaiConvertError(SHORT sError);
SHORT   WaiPreSignOut(WAIIF *Waiif);

/*====== End of Define =======*/
#endif