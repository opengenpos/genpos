/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Employee time keeping interface
*   Program Name       : PCSTBETK.H                                            
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
*    Date           :NAME               :Description
*    Jun-16-93      :H.Yamaguchi        :Initial
*    Oct-05-93      :H.YAMAGUCHI        :Adds EtkStatRead/EtkCurIndWrite 
*                                          by FVT Comm't
*    Jan-29-96      :M.SUZUKI           : R3.1 Initial
*                               Increase No. of Employees (200 => 250)
*                               Increase Employee No. ( 4 => 9digits)
*                               Add Employee Name (Max. 16 Char.)
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

/*------------------------------------------
	Employee Time Keeping Message (Response)
------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
	USHORT	usFunCode;
	USHORT	usSeqNo;
	SHORT	sResCode;
    SHORT   sReturn;
	USHORT  usFieldNo;
	SHORT   sTMStatus;   
	USHORT  usDataLen;   
	UCHAR   auchETKData[sizeof(ETK_FIELD)];
    UCHAR   auchEmployeeName[ETK_MAX_NAME_SIZE];    /* Add Rel 3.1 */
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
SHORT  STUBENTRY SerEtkAssign(ULONG   ulEmployeeNo,
                              ETK_JOB  FAR *pEtkJob,
                              UCHAR *pauchEmployeeName);
SHORT  STUBENTRY SerEtkDelete(ULONG ulEmployeeNo);
SHORT  STUBENTRY SerEtkAllReset(VOID);
SHORT  STUBENTRY SerEtkIssuedSet(VOID);
SHORT  STUBENTRY SerEtkCurIndWrite(ULONG     ulEmployeeNo,
                                   USHORT    *pusFieldNo,
                                   ETK_FIELD *pEtkField);

/*--------------------------------------------
    Not support backup 
--------------------------------------------*/ 
SHORT  STUBENTRY SerEtkIndJobRead(ULONG ulEmployeeNo,
                                  ETK_JOB FAR *pEtkJob,
                                  UCHAR *puchEmployeeName);
SHORT  STUBENTRY SerEtkCurAllIdRead(USHORT usRcvBufferSize, 
                                    ULONG FAR *aulRcvBuffer);
SHORT  STUBENTRY SerEtkSavAllIdRead(USHORT usRcvBufferSize, 
                                    ULONG FAR *aulRcvBuffer);
SHORT  STUBENTRY SerEtkCurIndRead(ULONG      ulEmployeeNo,
                                  USHORT     FAR *pusFieldNo,
                                  ETK_FIELD  FAR *pEtkField,
                                  ETK_TIME   FAR *pEtkTime);
SHORT  STUBENTRY SerEtkSavIndRead(USHORT     usEmployeeNo,
                                  USHORT     FAR *pusFieldNo,
                                  ETK_FIELD  FAR *pEtkField,
                                  ETK_TIME   FAR *pEtkTime);
SHORT  STUBENTRY SerEtkAllLock(VOID);
VOID   STUBENTRY SerEtkAllUnLock(VOID);
SHORT  STUBENTRY SerEtkIssuedCheck(VOID);
SHORT  STUBENTRY SerEtkStatRead(ULONG  ulEmployeeNo,
                                SHORT   *psTimeIOStat,
                                USHORT  *pusMaxFieldNo,
                                ETK_JOB *pEtkJob);
SHORT  STUBENTRY SerEtkCreateTotalIndex(VOID);

/*===== END OF DEFINITION =====*/

