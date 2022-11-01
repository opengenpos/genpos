/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server CASHIER module, Header file for internal                        
*   Category           : C/S CASHIER module, NCR2170 US GENERAL PURPOSE APPLICATION
*   Program Name       : CSCASIN.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*   Date     :Ver.Rev :NAME       :Description
*   Feb-15-93:00.00.01:H.YAMAGUCHI:Initial
*   Sep-03-98:03.03.00:A.Mitsui   :V3.3 Initial
*
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

/* Define Mode  for Cas_Index()  */
#define    CAS_SEARCH_ONLY            1         /* FOR Cas_Index() */
#define    CAS_REGIST                 2         /* FOR Cas_Index() */

/* Define Mode  for Cas_ChangeFlag()  */
#define    CAS_GET_STATUS             1         /* FOR Cas_ChangeFlag() */
#define    CAS_SET_STATUS             2         /* FOR Cas_ChangeFlag() */
#define    CAS_RESET_STATUS           3         /* FOR Cas_ChangeFlag() */

/* Define  fbFlag */
#define    CAS_OPEN_SIGNIN            0x0001    /* Open for  Register */
#define    CAS_PRECLOSE_SIGNIN        0x0002    /* PreClose  Register */

#define    CAS_OPEN_PICKUP_LOAN       0x0010    /* Open  for Pickup Loan */

/* Define for backup function  */
#define    CAS_CONT                   1
#define    CAS_END                    2

/*
===========================================================================
    TYPEDEF
===========================================================================
*/

typedef struct {
    USHORT  usSeqNO;                /* Sequnece number */
    ULONG   offulFilePosition;      /* Write and Read Position (Number of byte from file head) */
    UCHAR   uchStatus;              /* Status EOF, CONTINUE */
}CAS_BACKUP;

typedef struct {
    USHORT  usDataLen;              /* Data length  */
    UCHAR   auchData[1];            /* Cashier file data  */
}CAS_BAKDATA;

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
    cscasin.h
----------------------*/
VOID    Cas_GetHeader	(CAS_FILEHED *CasFileHed);
VOID    Cas_PutHeader	(CAS_FILEHED *CasFileHed);
SHORT   Cas_Index		(CAS_FILEHED *CasFileHed, USHORT usIndexMode, ULONG ulCashierNo, USHORT *usParaBlockNo);
SHORT   Cas_IndexDel	(CAS_FILEHED *CasFileHed, ULONG ulCashierNo);
VOID    Cas_ParaRead	(CAS_FILEHED *CasFileHed, USHORT usParablockNo, CAS_PARAENTRY  *pArg);
VOID    Cas_ParaWrite	(CAS_FILEHED *CasFileHed, USHORT usParaBlockNo, CAS_PARAENTRY *pArg);
SHORT   Cas_EmpBlkGet	(CAS_FILEHED *CasFileHed, USHORT *usParaBlcokNo);
SHORT   Cas_CheckBlockData (CAS_FILEHED *pCasFileHed);
VOID    Cas_BlkEmp		(CAS_FILEHED *CasFileHed, USHORT usParaBlockNo);
SHORT   Cas_IndLockCheck(ULONG ulCashierNo);
SHORT   Cas_LockCheck	(VOID);
SHORT   Cas_IndSignInCheck(ULONG ulCashierNo);
VOID    Cas_CloseFileReleaseSem(VOID);
VOID    Cas_WriteFile	(ULONG offulFileSeek, VOID *pTableHeadAddress, ULONG cusSizeofWrite);
SHORT   Cas_ReadFile	(ULONG offulFileSeek, VOID *pTableHeadAddress, ULONG cusSizeofRead);
SHORT	Cas_CompIndex	(ULONG *pKey, CAS_INDEXENTRY *pusHitPoint);
VOID    Cas_ChangeFlag	(USHORT usParaBlockNo, USHORT usMode, UCHAR fbStatus);

/*-------------------------------
    nocas.c
-------------------------------*/
SHORT   Cas_CheckSystem(VOID);
SHORT   NoCasSignIn(CASIF *pCasif);
SHORT   NoCasSignOut(CASIF *pCasif);
SHORT   NoCasClose(USHORT usTerminalNo);
SHORT   NoCasAllIdRead(USHORT usRcvBufferSize, USHORT *ausRcvBuffer);
SHORT   NoCasIndRead(CASIF *pCasif);
SHORT   NoCasAllLock(VOID);
SHORT   NoCasChkSignIn(USHORT *ausRcvBuffer);
SHORT   NoCasPreSignOut(CASIF *pCasif);
SHORT   NoCasIndLock(USHORT usTerminalNo);
SHORT   NoCasChkCondition(USHORT usTermAddr);

/*
===========================================================================
    MEMORY
===========================================================================
*/
extern  USHORT  husCashierSem;    
extern  SHORT   hsCashierFileHandle;
extern  SHORT   hsCashierNewFileHandle;             /* V3.3 */
extern  ULONG	aulCasTerminalTbl[CAS_TERMINAL_OF_CLUSTER];
extern  ULONG	ulCasIndLockMem;
extern  UCHAR   uchCasAllLockMem;
extern  UCHAR   auchCasFlag[CAS_NUMBER_OF_MAX_CASHIER];           
extern  UCHAR   auchCasOffset[CAS_TERMINAL_OF_CLUSTER];           
/*------------------------------------------
    External symbol access for file 
-------------------------------------------*/
extern UCHAR FARCONST  auchNEW_FILE_WRITE[];        /* to csop.c */
extern UCHAR FARCONST  auchOLD_FILE_WRITE[];        /* to csop.c */
extern UCHAR FARCONST  auchOLD_FILE_READ_WRITE[];   /* to csop.c */
extern UCHAR FARCONST  auchOLD_FILE_READ[];         /* to csop.c */

/*
===========================================================================
    MACRO
===========================================================================
*/

#define     CAS_IAM_NO_CASHIER  (Cas_CheckSystem() == CAS_FILE_NOT_FOUND)

/*========= End of Define =========*/
