/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1995      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Employee time keeping interface
*   Program Name       : CSSTBETK.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date     :NAME        :Description
*    Jun-16-93:H.Yamaguchi :Initial
*    Oct-05-93:H.YAMAGUCHI :Adds EtkStatRead/EtkCurIndWrite by FVT Comm't
*    Nov-10-95:T.Nakahata  : R3.1 Initial
*                               Increase No. of Employees (200 => 250)
*                               Increase Employee No. ( 4 => 9digits)
*                               Add Employee Name (Max. 16 Char.)
*
** NCR2171 **
*
*    Oct-05-99:M.Teraki    :initial (for 2171)
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

/*
===========================================================================
    DEFINE
===========================================================================
*/
/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/*------------------------------------------
    Employee Time Keeping Message (Response)
------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usFieldNo;
    SHORT   sTMStatus;   
    USHORT  usDataLen;   
    WCHAR   auchETKData[sizeof(ETK_FIELD)];
    WCHAR   auchEmployeeName[STD_CASHIERNAME_LEN];    /* Add Rel 3.1 */
} CLIRESETKTIME;


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

/*-------------------------------------------
    USER I/F ETK FUNCTION (CLIENT) W Backup
-------------------------------------------*/
SHORT  CliEtkTimeIn(ULONG ulEmployeeNo,
                    UCHAR uchJobCode,
                    ETK_FIELD *pEtkField);
SHORT  CliEtkTimeOut(ULONG ulEmployeeNo,
                     ETK_FIELD *pEtkField);
SHORT  SerEtkAssign(ULONG ulEmployeeNo,
                    ETK_JOB *pEtkJob,
                    WCHAR* pauchEmployeeName);
SHORT  SerEtkDelete(ULONG ulEmployeeNo);
SHORT  SerEtkAllReset(VOID);
SHORT  SerEtkIssuedSet(VOID);
SHORT  SerEtkCurIndWrite(ULONG ulEmployeeNo,
                         USHORT *pusFieldNo,
                         ETK_FIELD *pEtkField);

/*---------------------------------------------
    USER I/F ETK FUNCTION (CLIENT) W/O Backup
---------------------------------------------*/
SHORT  CliEtkStatRead(ULONG   ulEmployeeNo,
                      SHORT   *psTimeIOStat,
                      USHORT  *pusMaxFieldNo,
                      ETK_JOB *pEtkJob);
SHORT  CliEtkIndJobRead(ULONG   ulEmployeeNo,
                        ETK_JOB *pEtkJob,
                        WCHAR* pauchEmployeeName);

/*--------------------------------------------
    Not support backup 
--------------------------------------------*/ 
SHORT  SerEtkIndJobRead(ULONG ulEmployeeNo,
                        ETK_JOB *pEtkJob,
                        WCHAR* puchEmployeeName);
SHORT  SerEtkCurAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT  SerEtkSavAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT  SerEtkCurIndRead(ULONG ulEmployeeNo,
                        USHORT *pusFieldNo,
                        ETK_FIELD *pEtkField,
                        ETK_TIME *pEtkTime);
SHORT  CliEtkCurIndRead(ULONG     ulEmployeeNo,
                                    USHORT     FAR *pusFieldNo,
                                    ETK_FIELD  FAR *pEtkField,
                                    ETK_TIME   FAR *pEtkTime);
SHORT  SerEtkSavIndRead(ULONG ulEmployeeNo,
                        USHORT *pusFieldNo,
                        ETK_FIELD *pEtkField,
                        ETK_TIME *pEtkTime);
SHORT  SerEtkAllLock(VOID);
VOID   SerEtkAllUnLock(VOID);
SHORT  SerEtkIssuedCheck(VOID);
SHORT  SerEtkAllDataCheck(UCHAR uchMinor);
SHORT  SerEtkStatRead(ULONG ulEmployeeNo,
                      SHORT *psTimeIOStat,
                      USHORT *pusMaxFieldNo,
                      ETK_JOB *pEtkJob);

SHORT CliEtkCurAllIdRead(USHORT usRcvBufferSize, ULONG  *aulRcvBuffer);
   
/*===== END OF DEFINITION =====*/
