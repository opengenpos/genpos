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
* Program Name: dif.h
* ----------------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------------
* Abstract: 
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name      : Description
* Feb-13-95: 03.01.00 : M.Ozawa     : initial                                   
*
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

/* for debug */
/* #define DIFDEBUG                        1 */

#define DIF_SUCCESS                     0x00    /* success */

#define DIF_COM_ERR                     -1      /* general comm. error */
#define DIF_NOT_DIFPORT                 -2      /* not exist port */
#define DIF_SEARCH_ERR                  -3      /* plu search error */
 
#define DIF_POUR_REQ_MSG                0x50    /* pour request message */
#define DIF_NEXT_ORDER_MSG              0x4E    /* next order message */
#define DIF_END_OF_POUR_MSG             0x45    /* end of pour session message */

/*--------------------------------------------------------------------------
*		DIF data
--------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

    typedef struct{
        UCHAR   uchMsg;                      /* 2: message type */
        UCHAR   uchSeqNum;                   /* 3: sequence number */
        UCHAR   auchData[5];                 /* 4: data */
    }DIFMSG;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*--------------------------------------------------------------------------
*		prototype definitions
--------------------------------------------------------------------------*/

VOID  DifInit(VOID);
VOID  DifEnableStatus(VOID);
VOID  DifEnableStatus2(VOID);
SHORT DifDisableStatus(VOID);
VOID  DifRewriteRingBuffer(VOID);
VOID  DifClearSavedBuffer(VOID);

/****** End of Definition ******/
