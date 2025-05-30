#if POSSIBLE_DEAD_CODE
/*========================================================================*\
*   Title              : Client/Server SERVER module, Header file (User Int)                        
*   Category           : Client/Server SERVER module, US Hospitality Model                                                 
*   Program Name       : CSSERM.H                                            
*  ------------------------------------------------------------------------  
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories    :
*
*   Date           :NAME           :Description
*   May-07-92      :H.NAKASHIMA    :Initial
*   Jun-04-93      :H.YAMAGUCHI    :Modified same as HP Int'l 
*   Aug-08-95      :T.KOYAMA       :Matched Terminal Header 
*
*
\*=======================================================================*/
/*
===========================================================================
    DEFINE
===========================================================================
*/
/*
------------------------------------------
    Special Status  (fsSerExeSpe)
------------------------------------------
*/
#define     SER_SPESTS_INQUIRY      0x0010      /* during Inquiry */
#define     SER_SPESTS_INQTIMEOUT   0x0020      /* Time Out Inquiry */
#define     SER_SPESTS_FINAL_AC42   0x0100      /* Final Back Up Copy */
#define     SER_SPESTS_BEGIN_BCAS   0x0200      /* beginning of Bcast */
#define     SER_SPESTS_BMOD         0x0400      /* BM -> Out Of Date */
#define     SER_SPESTS_MTOD         0x0800      /* MT -> Out Of Date */
#define     SER_SPESTS_BCAS         0x2000      /* Broadcasting */
#define     SER_SPESTS_AC85         0x4000      /* ?  -> Out Of Date */
#define     SER_SPESTS_AC42         0x8000      /* Back Up Copy */    

/*
------------------------------------------
    INQUIRY Status  (usSerCurStatus)
------------------------------------------
*/
#define     SER_INQ_M_UPDATE    0x0001      /* Master Up To Date */
#define     SER_INQ_BM_UPDATE   0x0002      /* BM Up To Date */

/*
===========================================================================
    TYPEDEF
===========================================================================
*/
/*--------------------------------------------
    Inquiry Data structrue (SERVER MODULE)
---------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
    USHORT      usStatus;                       /* Inquiry Status */
    CLIINQDATE  CurDate;                        /* Inquiry Date */
    USHORT      ausTranNo[CLI_ALLTRANSNO];      /* Transaction # (each) */
    USHORT      ausPreTranNo[CLI_ALLTRANSNO];   /* Prev. Transaction # */
    USHORT      fsBcasInf;                      /* Broadcast Information */
} SERINQSTATUS;

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
VOID    SstInitialize(USHORT usMode);
SHORT   SstSetFlag(USHORT fsFlag);
SHORT   SstResetFlag(USHORT fsFlag);
SHORT   SstReadFlag(USHORT *pfsFlag);
SHORT   SstSetFlagWaitDone(USHORT fsFlag, USHORT husCliSem);
USHORT  SstIncTransNo(UCHAR uchUniqueAddr, USHORT usTranNo);
VOID    SstReadInqStat(SERINQSTATUS *pInqData);
VOID    SstChangeInqDate(CLIINQDATE *pInqDate);
VOID    SstChangeInqStat(USHORT usStatus);
VOID    SstChangeInqCurTraNo(USHORT *ausTransNo);
VOID    SstChangeInqPreTraNo(USHORT *ausTransNo);
VOID    SstChangeInqBcasInf(USHORT fsBcasInf);
VOID    SstSetBcasPointer(VOID *pBcas, USHORT usFunCode);
SHORT   SstSetBcasRegNo(UCHAR uchUA, USHORT usRegNo, 
                        USHORT usFunCode, UCHAR uchStatus);
SHORT   SstChkBcasing(VOID);
VOID    SstResetCurTransNo(VOID);
VOID    SstResetIndTransNo(UCHAR uchUniqueAddr);
VOID    SstReadFileFH(USHORT  offusSeekPos, 
                      VOID    *pReadBuffer, 
                      USHORT  usReadSize, 
                      SHORT   hsFileHandle);
VOID    SstWriteFileFH(USHORT offusSeekPos, 
                       VOID   *pWriteBuffer, 
                       USHORT usWriteSize,
                       SHORT  hsFileHandle);


/*===== End Of Definition =====*/
#endif