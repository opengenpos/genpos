/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1998       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Header                        
* Category    : ISP Server , NCR 2170 US Hospitality Model         
* Program Name: ISP.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
* Sep-03-98:03.03.00:A.Mitsui   : V3.3 Initial
*          :        :           :                                    
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
/*
*--------------------------------------------------------------------------
*    Define Descriptor control for " COMM " (not used)
*--------------------------------------------------------------------------
*/

/*   Argument " IspDispDescriptor " function  */

#define ISP_DESC_OFF                    1   /* Descriptor OFF        */
#define ISP_DESC_START_ON               2   /* Descriptor ON start   */
#define ISP_DESC_STOP_ON                3   /* Descriptor ON stop    */
#define ISP_DESC_START_BLINK            4   /* Descriptor BLINK start */
#define ISP_DESC_STOP_BLINK             5   /* Descriptor BLINK stop  */

/*   Argument " IspDispDescriptor " function */

#define ISP_MODULE_STUB            0x0001   /* Stub       Module ID */
#define ISP_MODULE_SERVER          0x0002   /* Server     Module ID */
#define ISP_MODULE_ISP             0x0004   /* ISP Server Module ID */
#define ISP_MODULE_ISPNOTPROVIDE   0xfbfb   /* ISP Server not provide */
                                            /* Reserved Bit 3/4/5/6/7 */

/*   Define Descriptor control   */

#define ISP_COMM_DESC                   0   /* COMM descriptor  */

/*   Control descriptor   */

#define ISP_DESC_CONT_ON          0x000ff   /* Check ON     modlule  */
#define ISP_DESC_CONT_BLINK       0x0ff00   /* Check BLINK  modlule  */

/*
*---------------------------------------------------------------------------
*   Define Return Code 
*---------------------------------------------------------------------------
*/

/* Define error code for internal processing    */
/*  From -1 to -29 (Internal Error Code)        */

#define ISP_SUCCESS                  0     /* Success                       */
#define ISP_WORK_AS_MASTER           0     /* Work as Master                */
#define ISP_WORK_AS_BMASTER          1     /* Work as Backup Master         */
#define ISP_WORK_AS_SATELLITE        2     /* Work as satellite             */

#define ISP_ERR_FRAME_SEQERR        -4     /* Detect a frame sequence error */
#define ISP_ERR_TIME_OUT            -7     /* time out detected             */
#define ISP_ERR_ILLEGAL             -8     /* Received invalid message      */
#define ISP_ERR_MULTI_SEND         -10     /* multi sending                 */
#define ISP_ERR_MULTI_RECV         -11     /* multi receiving               */
#define ISP_ERR_POWER_DOWN         -14     /* power down my terminal        */

#define ISP_ERR_NETWORK_NOT_OPEN   -18     /* Detect network error STUB_NET_NOT_OPENED */
#define ISP_ERR_NETWORK            -19     /* Detect network error          */

/*  The followings errors are defined for ISP user interface  */
/*  From -31 to -37 & -20 (External Error Code)               */

#define ISP_ERR_NOT_MASTER         -20     /* not master                    */

#define ISP_ERR_INVALID_MESSAGE    -31     /* Received invalid message      */
#define ISP_ERR_REQ_VERIFYPASS     -32     /* Request to exchange password  */
#define ISP_ERR_NOTMATCH_PASSWORD  -33     /* Does not match password       */
#define ISP_ERR_DUR_BACKUP         -34     /* Indicate during Backup copy   */
#define ISP_ERR_CONTINUE           -35     /* Continue this function        */
#define ISP_ERR_REQUEST_RESET      -36     /* Request to reset first        */
#define ISP_ERR_REQUEST_LOCKKBD    -37     /* Request to lock keyboard first */
#define ISP_ERR_REQUEST_LOCK_CS    -38     /* Request to lock cashier or server first */
#define ISP_ERR_HOST_NOT_FOUND     -39     // The IP address or Host Name was not found

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#define ISP_FILENAME_SIZE		STD_FILENAME_LENGTH_MAX

typedef struct {
    UCHAR   uchStatus;      /* Status       */
    UCHAR   uchAction;      /* action code  */
    UCHAR   uchDayPtd;      /* daily/ptd    */
    UCHAR   uchCurSave;     /* current/saved*/
    UCHAR   uchReset;       /* issued or not*/
    ULONG   ulNumber;       /* ind. number  */
    UCHAR   uchError;       /* error code   */
    UCHAR   uchYear;        /* year         */
    UCHAR   uchMonth;       /* month        */
    UCHAR   uchDay;         /* day          */
    UCHAR   uchHour;        /* hour         */
    UCHAR   uchMin;         /* minute       */
    WCHAR   aszMnemonic[ISP_FILENAME_SIZE];/* mneminics    */
} ISPRESETLOG;              /* ISP Reset Log, V3.3 */

extern  ISPRESETLOG         IspResetLog;

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID   IspInit(VOID);                         /* Creates ISP Server Task  */
VOID   IspDispDescriptor(USHORT usModleID,
                         USHORT usContDes );  /* Display COMM Descriptor (not used) */

/*===== END OF DEFINITION =====*/        

