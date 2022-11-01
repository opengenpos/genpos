#if POSSIBLE_DEAD_CODE
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
#ifndef PEPENTRY
    #if     defined(PEP)
        #define PEPENTRY FAR
    #else
        #define PEPENTRY FAR
    #endif
#endif

/*
******************************************************
*         RETURN VALUE FOR USER I/F FUNCTION         *
******************************************************
*/

#define ETK_SUCCESS                 0
#define ETK_FILE_NOT_FOUND        -01
#define ETK_FULL                  -02
#define ETK_NOT_IN_FILE           -03
#define ETK_NO_MAKE_FILE          -06
#define ETK_LOCK                  -10
#define ETK_DELETE_FILE           -11
#define ETK_NOT_IN_JOB            -12
#define ETK_FIELD_OVER            -13
#define ETK_NOT_TIME_OUT          -14
#define ETK_NOT_TIME_IN           -15
#define ETK_DATA_EXIST            -16
#define ETK_CONTINUE              -17
#define ETK_NOT_IN_FIELD          -18
#define ETK_NOTISSUED             -19
#define ETK_NOT_MASTER            -20

#define ETK_ISSUED                -30

/*
*************************************
*       DEFINE                      *
*************************************
*/

#define ETK_MAX_EMPLOYEE        250 /* Max Employee records */
/* #define ETK_MAX_EMPLOYEE        200 Max Employee records */
#define ETK_MAX_FIELD           15  /* Number of Field (MAX) */
#define ETK_JOBCODE_RECORD_SIZE  3  /* ALLOWANCE JOB CODE RECORD SIZE */
#define ETK_TIME_NOT_IN         0x2a2a  /* ** Id is time not in a etk file */

#define ETK_MAX_NAME_SIZE       16  /* Max. size of employee name */
#define ETK_MAX_WORK_BUFFER     256 /* Max. size of work buffer */


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
        USHORT  usFromMonth;            /*  */
        USHORT  usFromDay;              /*  */
        USHORT  usFromTime;             /*  */
        USHORT  usFromMinute;           /*  */
        USHORT  usToMonth;              /*  */
        USHORT  usToDay;                /*  */
        USHORT  usToTime;               /*  */
        USHORT  usToMinute;             /*  */
    } ETK_TIME;

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
VOID    PEPENTRY EtkInit(VOID);
SHORT   PEPENTRY EtkCreatFile(USHORT usNumberOfEmployee);
SHORT   PEPENTRY EtkCheckAndCreatFile(USHORT usNumberOfEmployee);
SHORT   PEPENTRY EtkCheckAndDeleteFile(USHORT usNumberOfEmployee);
SHORT   PEPENTRY EtkTimeIn(ULONG ulEmployeeNo,
                           UCHAR  uchJobCode,
                           ETK_FIELD  *pEtkField);
SHORT   PEPENTRY EtkTimeOut(ULONG ulEmployeeNo,
                           ETK_FIELD  *pEtkField);
SHORT   PEPENTRY EtkAssign(ULONG    ulEmployeeNo,
                           ETK_JOB  *pEtkJob,
                           UCHAR    *puchEmployeeName );
SHORT   PEPENTRY EtkDelete(ULONG ulEmployeeNo);
SHORT   PEPENTRY EtkIndJobRead(ULONG ulEmployeeNo,
                               ETK_JOB  *pEtkJob,
                               UCHAR    *puchEmployeeName);
SHORT   PEPENTRY EtkCurAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT   PEPENTRY EtkSavAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT   PEPENTRY EtkCurIndRead(ULONG ulEmployeeNo,
                     USHORT *pusFieldNo,
                     ETK_FIELD  *pEtkField,
                     ETK_TIME *pEtkTime);

SHORT   PEPENTRY EtkSavIndRead(ULONG ulEmployeeNo,
                     USHORT *pusFieldNo,
                     ETK_FIELD  *pEtkField,
                     ETK_TIME *pEtkTime);

SHORT   PEPENTRY EtkGetResetTime(ETK_TIME *pEtkTime);
SHORT   PEPENTRY EtkAllReset(VOID);
SHORT   PEPENTRY EtkAllLock(VOID);
VOID    PEPENTRY EtkAllUnLock(VOID);
SHORT   PEPENTRY EtkIssuedCheck(VOID);
SHORT   PEPENTRY EtkIssuedSet(VOID);
SHORT   PEPENTRY EtkIssuedClear(VOID);
SHORT   PEPENTRY EtkOpenFile(CONST CHAR FAR *pszMode,
                             UCHAR uchFileType,
                             SHORT *phsEtkFile);
VOID    PEPENTRY EtkCloseFile(SHORT hsEtkFile);
SHORT   PEPENTRY EtkReadFile(SHORT hsEtkFile,
                   ULONG ulOffset,
                   USHORT usLen,
                   UCHAR  *puchReadData);
VOID    PEPENTRY EtkWriteFile(SHORT hsEtkFile,
                    ULONG ulOffset,
                    USHORT usLen,
                    UCHAR  *puchWriteData);
USHORT  PEPENTRY EtkConvertError(SHORT sError);
SHORT   PEPENTRY EtkAllDataCheck(UCHAR uchMinor);
SHORT   PEPENTRY EtkStatRead(ULONG  ulEmployeeNo,
                             SHORT   *psTimeIOStat,
                             USHORT  *pusMaxFieldNo,
                             ETK_JOB *pEtkJob);
SHORT   PEPENTRY EtkCurIndWrite(ULONG ulEmployeeNo,
                                USHORT *pusFieldNo,
                                ETK_FIELD *pEtkField);
SHORT   PEPENTRY EtkCreateTotalIndex(VOID);
/*====== End of Source ======*/
#endif