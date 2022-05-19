/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
* Title       : DIF MODE HEADER FILE                         
* Category    : DIF MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: diflocal.h
* ----------------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------------
* Abstract: 
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    	: Description
* Feb-13-95: 03.01.00 : M.Ozawa     : initial                                   
*
*** NCR2172 ***
*
* Oct-04-99: 01.00.00 : M.Teraki    : Initial
*                                   : Add #pragma pack(...)
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

#define DIF_STX                         0x02    /* start text */
#define DIF_ETX                         0x03    /* end text */
#define DIF_ACK                         0x06    /* acknowledge */
#define DIF_WACK                        0x13    /* wait acknowledge */
#define DIF_NAK                         0x15    /* not acknowledge */

/**     RS-232 Port control   **/
#define DIF_PORT_NONE                   -1          /* not exist RS-232 port    */
#define DIF_RETRY_COUNT                 3           /* retry counter            */
#define DIF_WAIT_PORT                   3           /* retry counter for port   */
#define DIF_WAIT_PORT_TIME              5           /* retry timer 5 sec.       */
#define DIF_WAIT_REQ_TIME               0           /* not return unless received */

#define DIF_INI_SEQ_NUM                 0x30

#define DIF_SERVICE_PROVIDED            0x01        /* provide dispenser i/f service */
#define DIF_ENABLE_STATUS               0x02        /* dispenser i/s enable status */
#define DIF_POWER_DOWN                  0x04        /* after power down recovery */

/*--------------------------------------------------------------------------
*		DIF data
--------------------------------------------------------------------------*/


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


    typedef struct{
        UCHAR   uchSTX;                      /* 1: start of text */
        UCHAR   uchMsg;                      /* 2: message type */
        UCHAR   uchSeqNum;                   /* 3: sequence number */
        UCHAR   auchData[5];                 /* 4: data */
        UCHAR   uchETX[1];                   /* 5: end of text */
        TCHAR   auchLRC[2];                  /* 6: LRC data */
    }DIFREQDATA;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


/*--------------------------------------------------------------------------
*   external work area declarations
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*		protocol definitions
--------------------------------------------------------------------------*/

SHORT   DifGetComPort(VOID);
VOID    THREADENTRY DifMainReceiveTask(VOID);
SHORT   DifCheckLRC( UCHAR *DifRcvData, USHORT usDataSize );
SHORT   DifCheckSeqNum(DIFMSG *pDifMsg);
SHORT   DifSearchData(DIFMSG *pDifMsg);

/****** End of Definition ******/
