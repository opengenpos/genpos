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
* Title       : Print module Include file used in print module                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtShr.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 00.00.01 :  R.itoh    :                                    
* Apr-21-94 : 00.00.01 :  M.Suzuki  :add PrtShrWriteFirstRetry(),PrtShrFirstRetryPrc()
*           :          :            :                                    
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
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
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/
/* -- max data define -- */
//#define PRT_SHR_MAX_DATA     (SHR_FRAME_SIZE - sizeof(SHRHEADERINF)) 
                                    /* max print data length to be buffered */
#define PRT_SHR_MAX_CHAR      42    /* max print data per 1 line */
#define PRT_SHR_MAX_TERM      16    /* max number of terminals to be connected,  PIF_NET_MAX_IP */      

#define PRT_SHR_SLEEP_TIME  3000    /* sleep time after CliShrPrint() */
#define PRT_SHR_RET_CO         3    /* retry counter was 5 but reduced improving user experience */ 

/* -- shared printer status -- */
#define PRT_SHARED_SYSTEM   0x01    /* indicate shared printer system */

#define PRT_SHR_NOT_PRINT      1    /* not print to receipt (fbPrtShrPrint) */

/* -- shared printer lock status -- */
#define PRT_SHR_LOCK         0x01   /* terminal locked */
#define PRT_SUPPRG_MODE      0x02   /* during super, prog mode */

/* -- shared printer alternative printer status -- */
#define PRT_WITHSHR             0   /* go to shared printer */ 
#define PRT_WITHALTR         0x01   /* go to alternative printer */

/* -- shared printer alternation status -- */
#define PRT_NEED_ALTR        0x01   /* need alternation */   
#define PRT_BUFFERING_ALTR   0x02   /* need alternation (for buffring print) */   
#define PRT_WRITE_ABORT      0x04   /* abort writing print data */   
#define PRT_TOTAL_STUB       0x08   /* indicate flag restored (for total stub) */   

/* -- shared printer abort key entry status -- */
#define PRT_SUCCESS             0   /* operation successful */
#define PRT_ABORTED            -1   /* abort key entered */
#define PRT_CONT                1   /* retry continue */
#define PRT_CONT_COND           2   /* retry conditionally with retry count exhaustion  */

/*
============================================================================
+                     T Y P E    D E F I N I T I O N S
============================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    SHRHEADERINF    HdInf;          /* SHRHEADERINF is defined in 'SHR.H' */
    UCHAR           auchPrtData[SHR_MAX_DATA_SIZE];
}PRTSHRINF;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/
/* -- for shared printer data -- */
extern PRTSHRINF PrtShrInf;                            /* shared print information */
extern UCHAR   uchPrtReceiptCo;                        /* number of receipt/ ticket to be printed */
extern USHORT  usPrtShrOffset;                         /* print data area offset */
extern UCHAR   fbPrtLockChk;                           /* lock check */
extern UCHAR   fbPrtShrStatus;                         /* shared print status */                        
extern UCHAR   fbPrtAltrStatus;                        /* printer alternation status */
extern UCHAR   fbPrtAbortStatus;                       /* abort key entry status */
extern UCHAR   fbPrtAltrSav;                           /* save alternation status */
extern UCHAR   fbPrtAbortSav;                          /* save abort key entry status */
extern UCHAR   fbPrtShrPrint;                          /* not print status */                        

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/

VOID    PrtShrInit(TRANINFORMATION  *pTran);
VOID    PrtShrPrint(TCHAR *pszBuff, USHORT  usLen);
SHORT   PrtShrSupSend(VOID);
SHORT   PrtShrRegSend(SHORT sSuccess);
SHORT   PrtShrWriteRetry(USHORT  usError, SHORT  sSuccess, SHORT  sRet);
SHORT   PrtShrWriteRetry_Reg(USHORT  usError, SHORT  sSuccess, SHORT  sRet);
SHORT   PrtShrRetryPrc(USHORT  usError, SHORT  sSuccess);
SHORT   PrtShrAltrRetry(USHORT  usError, SHORT  sSuccess, SHORT  sRet);
VOID    PrtShrAltr(TRANINFORMATION  *pTran, SHORT hsStorage);
UCHAR   PrtChkFrame(VOID);
UCHAR   PrtAltrMsg(VOID);
SHORT   PrtShrLockSub(UCHAR uchPrtStatus);
SHORT   PrtShrWriteFirstRetry(SHORT  sSuccess, SHORT  sRet);
SHORT   PrtShrFirstRetryPrc(SHORT  sSuccess);

/****** End Of Definition ******/
