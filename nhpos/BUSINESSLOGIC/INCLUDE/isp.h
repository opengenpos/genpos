/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
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
* Dec-01-92:01.00.02:H.Yamaguchi: Remove IspDispDescriptor                                   
*          :        :           :                                    
*
** NCR2172 **
*
* Oct-05-99:01.00.00:M.Teraki   :initial (for 2172)
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

#define ISP_ERR_FRAME_SEQERR        (-4)     /* Detect a frame sequence error */
#define ISP_ERR_TIME_OUT            (-7)     /* time out detected             */
#define ISP_ERR_ILLEGAL             (-8)     /* Received invalid message      */
#define ISP_ERR_MULTI_SEND         (-10)     /* multi sending                 */
#define ISP_ERR_MULTI_RECV         (-11)     /* multi receiving               */
#define ISP_ERR_POWER_DOWN         (-14)     /* power down my terminal        */

#define ISP_ERR_NETWORK            (-19)     /* Detect network error          */

/*  The followings errors are defined for ISP user interface  */
/*  From -31 to -37 & -20 (External Error Code)               */

#define ISP_ERR_NOT_MASTER         (-20)     /* not master                    */

#define ISP_ERR_INVALID_MESSAGE    (-31)     /* Received invalid message, same numeric code as PARA_ERR_INVALID_MESSAGE  */
#define ISP_ERR_REQ_VERIFYPASS     (-32)     /* Request to exchange password  */
#define ISP_ERR_NOTMATCH_PASSWORD  (-33)     /* Does not match password       */
#define ISP_ERR_DUR_BACKUP         (-34)     /* Indicate during Backup copy   */
#define ISP_ERR_CONTINUE           (-35)     /* Continue this function        */
#define ISP_ERR_REQUEST_RESET      (-36)     /* Request to reset first        */
#define ISP_ERR_REQUEST_LOCKKBD    (-37)     /* Request to lock keyboard first */
#define ISP_ERR_REQUEST_LOCK_CS    (-38)     /* Request to lock cashier or server first */

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


/* --- IspResetLog.uchStatus,   FVT#5,   Jan/20/99 --- */
#define ISP_RESETLOG_LOGOFF     0x01 
#define ISP_FILENAME_SIZE		STD_FILENAME_LENGTH_MAX

/*
 * data struct used to generate an Electronic Journal entry to document.
 * see function IspRecvTtlSetLog() which translates uchMajorClass type to
 * a code placed into uchAction. IspRecvTtlSetLog() also sets additional
 * members of the struct depending on function code, sets uchReset value,
 * and other data of a remote application request for totals.
**/
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


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID   IspInit(VOID);                         /* Creates ISP Server Task  */

/*===== END OF DEFINITION =====*/
