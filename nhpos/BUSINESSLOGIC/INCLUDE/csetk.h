/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server ETK module, Header file for user
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSETK.H
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date         Ver.Rev : NAME        : Description
*   Jun-16-93 : 00.00.01 : M.YAMAMOTO  : Initial
*   Mar-14-96 : 03.01.00 : T.Nakahata  : R3.1 Initial
*       Increase No. of Employees (200 => 250)
*       Increase Employee No. ( 4 => 9digits)
*       Add Employee Name (Max. 16 Char.)
*   May-09-96 : 03.01.00 : T.Nakahata  : Reduce work buffer size.
*
** NCR2171 **
*
*   Oct-05-99 : 01.00.00 : M.Teraki    :initial (for 2171)
*   Apr-08-15 : 02.02.01 : R.Chambers  :guard against multiple inclusion.
*===========================================================================
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(CSETK_H_INCLUDE)
#define CSETK_H_INCLUDE

/*
===========================================================================
    DEFINE
===========================================================================
*/

/* Employee Time Keeping subsystem error codes
   These error codes are converted by function EtkConvertError()
   into LDT_ type codes for use by function UieErrorMsg() to
   display errors to the operator.

   These codes should be synchronized with similar values used by
   the lower level STUB_ error codes as well as the Totals
   error codes defined in file csttl.h and the Guest Check
   error codes defined in file csgcs.h and the Cashier error codes in 
 */
#define ETK_SUCCESS                 0
#define ETK_FILE_NOT_FOUND        (-01)
#define ETK_FULL                  (-02)
#define ETK_NOT_IN_FILE           (-03)    // same as CAS_NOT_IN_FILE
#define ETK_NO_MAKE_FILE          (-06)
#define ETK_LOCK                  (-10)    // same as CAS_ALREADY_LOCK
#define ETK_DELETE_FILE           (-11)
#define ETK_NOT_IN_JOB            (-12)
#define ETK_FIELD_OVER            (-13)
#define ETK_NOT_TIME_OUT          (-14)
#define ETK_NOT_TIME_IN           (-15)
#define ETK_DATA_EXIST            (-16)
#define ETK_CONTINUE              (-17)
#define ETK_NOT_IN_FIELD          (-18)
#define ETK_NOTISSUED             (-19)
#define ETK_NOT_MASTER            (-20)
#define ETK_ISSUED                (-30)
#define ETK_NO_READ_SIZE          (-31)    /* for File Read, seek to or read of header failed. Indicates invalid ETK file. */
#define ETK_UNLOCK                (-32)    /* for File Read */
#define ETK_UNKNOWN_EMPLOYEE      (-40)    // indicates that the employee name lookup is for an unknown employee

// Following are used to transform Network Layer (STUB_) errors into ETK_ errors
// Notice that error code is same as STUB_ plus STUB_RETCODE.
#define	ETK_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define	ETK_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define ETK_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define ETK_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define ETK_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define ETK_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)

/*
*************************************
*       DEFINE                      *
*************************************
*/

#define ETK_MAX_EMPLOYEE        250      /* Max Employee records, should be same as FLEX_ETK_MAX */
#define ETK_MAX_FIELD            15      /* Number of Field (MAX) */
#define ETK_JOBCODE_RECORD_SIZE   3      /* ALLOWANCE JOB CODE RECORD SIZE */
#define ETK_TIME_NOT_IN         0x2a2a   /* ** Id is time not in a etk file */

#define ETK_MAX_NAME_SIZE        20      /* Max. size of employee name JHHJ Database Changes 3-29-04 - must be same as STD_CASHIERNAME_LEN*/
#define ETK_MAX_WORK_BUFFER     256      /* Max. size of work buffer */


/* FOR EtkOpenFile() */
#define ETK_PARAM_FILE          0   /* PARAMATER FILE */
#define ETK_TOTAL_FILE          1   /* TOTAL FILE */

/*
*************************************
    STRUCTURE DEFINE                *
*************************************
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

    typedef struct  {
        USHORT  usYear;                  /* Year to be time in */
        USHORT  usMonth;                 /* Month to be time in */
        USHORT  usDay;                   /* Day to be time in */
        UCHAR   uchJobCode;              /* Job Code to be time in */
        USHORT  usTimeinTime;            /* Time to be time in */
        USHORT  usTimeinMinute;          /* Minute to be time in */
        USHORT  usTimeOutTime;           /* Time to be time out */
        USHORT  usTimeOutMinute;         /* Minute to be time out */
    }ETK_FIELD;

    typedef struct {
        UCHAR   uchJobCode1;            /* Allowance Job code 1 */
        UCHAR   uchJobCode2;            /* Allowance Job code 2 */
        UCHAR   uchJobCode3;            /* Allowance Job code 3 */
    }ETK_JOB;

    typedef struct {
        USHORT  usFromMonth;            /* Month to be time in */
        USHORT  usFromDay;              /* Day to be time in */
        USHORT  usFromTime;             /*  */
        USHORT  usFromMinute;           /*  */
        USHORT  usToMonth;              /* Month to be time out */
        USHORT  usToDay;                /* Day to be time out */
        USHORT  usToTime;               /*  */
        USHORT  usToMinute;             /*  */
    } ETK_TIME;

	typedef struct {
		ULONG        ulEmployeeNo;
		TCHAR        auchEmployeeName[STD_CASHIERNAME_LEN];    /* Employee Name, R3.1 */
		ETK_JOB      EtkJob;              /* Job Code Structure */
		ETK_FIELD    EtkDetails;
	}ETK_DETAILS;

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
    USER I/F
----------------------*/
VOID    EtkInit(VOID);
SHORT   EtkCreatFile(USHORT usNumberOfEmployee);
SHORT   EtkCheckAndCreatFile(USHORT usNumberOfEmployee);
SHORT   EtkCheckAndDeleteFile(USHORT usNumberOfEmployee);
SHORT   EtkTimeIn(ULONG ulEmployeeNo, UCHAR  uchJobCode, ETK_FIELD  *pEtkField);
SHORT   EtkTimeOut(ULONG ulEmployeeNo, ETK_FIELD  *pEtkField);
SHORT   EtkAssign(ULONG    ulEmployeeNo, ETK_JOB  *pEtkJob, WCHAR* puchEmployeeName );
SHORT   EtkStatCheckEmployeesLoggedIn (ULONG ulEmployeIdCheck, ULONG  *ulEmployeeNoList, int nMax);
SHORT   EtkStatCheckCreateFirstEmployee (ULONG  ulEmployeeNo);
SHORT   EtkDelete(ULONG ulEmployeeNo);
SHORT   EtkIndJobRead(ULONG ulEmployeeNo, ETK_JOB  *pEtkJob, WCHAR* puchEmployeeName);
SHORT   EtkCurAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT   EtkCurAllIdReadIncr(USHORT usRcvBufferSize, ULONG *aulRcvBuffer, USHORT usEmpNo);
SHORT   EtkSavAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT   EtkCurIndRead(ULONG ulEmployeeNo, USHORT *pusFieldNo, ETK_FIELD  *pEtkField, ETK_TIME *pEtkTime);
SHORT   EtkCurAllLoggedInRead(USHORT usRcvArraySize, ETK_DETAILS *Etk_Details);

SHORT   EtkSavIndRead(ULONG ulEmployeeNo, USHORT *pusFieldNo, ETK_FIELD  *pEtkField, ETK_TIME *pEtkTime);

SHORT   EtkGetResetTime(ETK_TIME *pEtkTime);
SHORT   EtkAllReset(VOID);
SHORT   EtkAllLock(VOID);
VOID    EtkAllUnLock(VOID);
SHORT   EtkIssuedCheck(VOID);
SHORT   EtkIssuedSet(VOID);
SHORT   EtkIssuedClear(VOID);
SHORT   EtkOpenFile(CONST CHAR *pszMode, UCHAR uchFileType, SHORT *phsEtkFile);
VOID    EtkCloseFile(SHORT hsEtkFile);
SHORT   EtkReadFile(SHORT hsEtkFile, ULONG ulOffset, ULONG ulLen, UCHAR *puchReadData, ULONG *pulActualBytesRead);
VOID    EtkWriteFile(SHORT hsEtkFile, ULONG ulOffset, ULONG ulLen, UCHAR  *puchWriteData);
USHORT  EtkConvertErrorWithLineNumber(SHORT sError, ULONG ulCalledFromLineNo);
#define EtkConvertError(sError) EtkConvertErrorWithLineNumber(sError,__LINE__)
SHORT   EtkAllDataCheck(UCHAR uchMinor);
SHORT   EtkStatRead(ULONG  ulEmployeeNo, SHORT   *psTimeIOStat, USHORT  *pusMaxFieldNo, ETK_JOB *pEtkJob);
SHORT   EtkCurIndWrite(ULONG ulEmployeeNo, USHORT *pusFieldNo, ETK_FIELD *pEtkField);
SHORT   EtkCreateTotalIndex(VOID);

#endif
/*====== End of Source ======*/
