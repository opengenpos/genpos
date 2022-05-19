/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
* Title       : EPT MODE HEADER FILE                         
* Category    : EPT MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: prevept.h
* ----------------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------------
* Abstract: 
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name          : Description
* Nov-16-93: 00.00.01 : K.You       : initial                                   
*
** NCR2172 **
*
* Oct-05-99: 01.00.00 : M.Teraki    : initial (for 2172)
*
*===================================================================================
*===================================================================================
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
*   PVCS ENTRY
*    $Revision$
*    $Date$
*    $Author$
*    $Log$
\*-----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
*   Define Declarations
--------------------------------------------------------------------------*/
#if !defined(EPT_INCLUDED)
#define EPT_INCLUDED

#include "regstrct.h"

/* for debug */
/* #define EPTDEBUG                        1 */

/* tally required type */
#define EPT_TALLY_READ                  1       /* read tally */
#define EPT_TALLY_RESET                 2       /* read & reset tally */

#define EPT_SUCCESS                     0x00    /* success */

#define EPT_STX                         0x02    /* start text */
#define EPT_ETX                         0x03    /* end text */
#define EPT_EOT                         0x04    /* end of text */
#define EPT_ENQ                         0x05    /* enquiry */
#define EPT_ACK                         0x06    /* acknowledge */
#define EPT_NAK                         0x15    /* not acknowledge */
#define EPT_COM_ERR                     -31     /* general comm. error for EPT */
#define EPT_NOT_EPTPORT                 -36     /* not exist EPT port */
#define EPT_ABORTED                     -37     /* aborted by user for EPT */
 
/*--------------------------------------------------------------------------
*               EPT data
--------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

    typedef struct{
        UCHAR   auchSTX[1];						/* 1: start of text */
        UCHAR   auchTransID[8];					/* 2: transaction ID */
        UCHAR   auchActionCode[3];				/* 10: action code */
        UCHAR   auchAcctNumber[NUM_NUMBER_EPT];	/* 13: account number - zero terminated */
        UCHAR   auchExpiraDate[NUM_EXPIRA];		/* 33: account expiration date */
        UCHAR   auchChargeTtl[8];				/* 37: charge total */
        UCHAR   auchAuthoCode[6];				/* 45: authorization code */
        UCHAR   auchTransType[3];				/* 51: transaction type */
        UCHAR   auchProductCode[10];			/* 54: product code */
        UCHAR   auchETX[1];						/* 64: end of text */
        UCHAR   auchLRC[1];						/* 65: LRC data */
    }EPTREQDATA;

    typedef struct{
        UCHAR   auchSTX[1];						/* 1: start of text */
        UCHAR   auchTransID[8];					/* 2: transaction ID */
        UCHAR   auchAcctNumber[NUM_NUMBER_EPT];	/* 10: account number  - zero terminated */
        UCHAR   auchExpiraDate[NUM_EXPIRA];		/* 30: account expiration date */
        UCHAR   auchReplyData[18];				/* 34: reply data from host */
        UCHAR   auchETX[1];						/* 52: end of text */
        UCHAR   auchLRC[1];						/* 53: LRC data */
    }EPTRSPDATA;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*--------------------------------------------------------------------------
*   external work area declarations
--------------------------------------------------------------------------*/

/* extern  EPTTALLY UNINT  EPTTally; */                  /* tally counter */
        
/*--------------------------------------------------------------------------
*               protocol definitions
--------------------------------------------------------------------------*/

SHORT EPTConfirmMessage( EPTREQDATA *ReqData,
                         EPTRSPDATA *RcvData );
SHORT EPTGetPort( VOID );
SHORT EPTOpenSetPort( VOID );
SHORT EPTConfirmCtlData( UCHAR uchCtlCode );
SHORT EPTConfirmReqData( EPTREQDATA *ReqData,
                         USHORT usDataSize );
SHORT EPTRcvRspData( EPTRSPDATA *RcvData,
                     USHORT usDataSize );
VOID EPTClosePort( SHORT hsEPTPort );
VOID EPTReadResetTally( UCHAR uchType, EPTTALLY *pTally );

#endif          /* EPT_INCLUDED */
/****** End of Definition ******/