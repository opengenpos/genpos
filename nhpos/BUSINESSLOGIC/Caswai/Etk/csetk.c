/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server ETK module, Program List
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSETK.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*    USER I/F FUNCTION
*
*    EtkInit();              * ETK function initiarize  *
*    EtkCreatFile();         * ETK file create *
*    EtkCheckAndCreatFile()  * ETK check and create file *
*    EtkCheckAndDeleteFile() * ETK check and delete file *
*    EtkTimeIn();            * timein function *
*    EtkTimeOut();           * timeout function *
*    EtkAssign();            * ETK assign function *
*    EtkDelete();            * ETK delete function *
*    EtkIndJobRead();        * read allowance job code *
*    EtkCurAllIdRead();      * read current all id *
*    EtkSavAllIdRead();      * read saved all id *
*    EtkCurIndRead();        * read current individual ETK record *
*    EtkSavIndRead();        * read saved individual ETK record *
*    EtkAllReset();          * reset all record *
*    EtkGetResetTime();      * Get reset date time *
*    EtkAllLock();           * lock all ETK record. do not update *
*    EtkIssuedCheck();       * Check Issued flag *
*    EtkIssuedSet();         * Set Issucd Flag *
*    EtkIssuedClear();       * Clear Issued Flag*
*    EtkStatRead();          * Status Read command *
*    EtkCurIndWrite();       * Write current individual ETK record *
*    EtkOpenFile();          * Open ETK file *
*    EtkCloseFile();         * Close ETK file *
*    EtkReadFile();          * Read ETK file *
*    EtkWriteFile();         * Write ETK file *
*    EtkAllDataCheck();      * Check Data is in buffer or not
*
*   INTERNAL FUNCTION        * THIS FUNCTION IS BELONG CSETKIN.C *
*
*       Etk_GetHeader();
*       Etk_PutHeader();
*       Etk_Index();
*       Etk_IndexDel();
*       Etk_ParaRead();
*       Etk_ParaWrite();
*       Etk_EmpBlkGet();
*       Etk_BlkEmp();
*       Etk_IndLockCheck();
*       Etk_LockCheck();         * Check All lock *
*       Etk_TimeInCheck();
*       Etk_IndTimeInCheck();
*       Etk_CloseFileReleaseSem();
*       Etk_WriteFile();
*       Etk_ReadFile();
*       Etk_CompIndex();
*
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*    Date       Ver.Rev :NAME       :Description
*    June-15-93:00.00.01:M.YAMAMOTO :Initial
*    Nov -09-95:03.01.00:T.Nakahata : R3.1 Initial
*           Increase No. of Employees ( 200 => 250 )
*           Expand Employee No. ( 4 => 9digits )
*           Add Employee Name (Max. 16 Char.)
*    Apr -09-96:03.01.03:T.Nakahata : reduce stack size(AllReset, AllDataChk)
*    May -08-96:03.01.05:T.Nakahata : fix a glith at EtkAllReset()
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
#include	<tchar.h>
#include	<math.h>
#include    <stdio.h>
#include    <string.h>

#include    <ecr.h>
#include	<regstrct.h>
#include    "pif.h"
#include    "rfl.h"
#include    "paraequ.h"
#include    "plu.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "appllog.h"
#include    "csetk.h"
#include    "csttl.h"
#include    "csetkin.h"
#include    "cscas.h"
#include    "uie.h"
#include    "csstbcas.h"
#include    "pifmain.h"
#include    "item.h"

TCHAR   CONST  auchETK_PARA_FILE[] = _T("PARAMETK");
TCHAR   CONST  auchETK_TOTAL_FILE[] = _T("TOTALETK");

PifSemHandle  husEtkSem = PIF_SEM_INVALID_HANDLE;
SHORT         hsEtkParaFile = -1;        /* paramater file */
SHORT         hsEtkTotalFile = -1;       /* total file */

static UCHAR  uchEtkAllLockMem = 0;

/*
*==========================================================================
**    Synopsis:   VOID    EtkInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Initialize for Etk Module.
*               Save semaphore handle.
*==========================================================================
*/
VOID    EtkInit(VOID)
{
    /* Create Semaphore */
    husEtkSem = PifCreateSem( 1 );  /* save semaphore handle */
}

/*
*==========================================================================
**    Synopsis:    SHORT   Etk_LockCheck(VOID)
*
*       Input:    Nothing
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    ETK_UNLOCK
*                  ETK_LOCK
*
**  Description:
*                Check if Etk is Locked.
*==========================================================================
*/
static SHORT   Etk_LockCheck(VOID)
{
    if (!uchEtkAllLockMem) {    /* (uchEtkAllLockMem == 0) */
        return(ETK_UNLOCK);
    }
    return(ETK_LOCK);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkCreatFile(USHORT usNumberOfEmployee)
*
*       Input:    UCHAR uchNumberOfEmployee - Number of Employee
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*          Normal End:   ETK_SUCCESS
*        Abnormal End:   ETK_NO_MAKE_FILE
*
**  Description:
*               Create Etk File.
*==========================================================================
*/
SHORT   EtkCreatFile(USHORT usNumberOfEmployee)
{
    SHORT sStatus;

    PifRequestSem(husEtkSem);          /* Request Semaphore */

    PifDeleteFile(auchETK_PARA_FILE);       /* Delete Etk Paramater File */
    PifDeleteFile(auchETK_TOTAL_FILE);      /* Delete Etk Total File */

    /* Check NumberOfEtk */
    if (!usNumberOfEmployee) {   /* usNumberOfEmployee == 0 */
        PifReleaseSem(husEtkSem);   /* 0 then delete file */
        return(ETK_SUCCESS);
    }

    /* Open Etk Paramater File */
    if ((hsEtkParaFile = PifOpenFile(auchETK_PARA_FILE, auchNEW_FILE_WRITE)) < 0) {  /* pif_error_file_exist */
		PifLog(MODULE_ETK, FAULT_ERROR_FILE_OPEN);    /* then abort */
		PifLog(MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(hsEtkParaFile));    /* then abort */
		PifLog(MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);    /* then abort */
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_OPEN);
    }

    /* Open Etk Total File */
    if ((hsEtkTotalFile = PifOpenFile(auchETK_TOTAL_FILE, auchNEW_FILE_WRITE)) < 0) {  /* pif_error_file_exist */
		PifLog(MODULE_ETK, FAULT_ERROR_FILE_OPEN);    /* then abort */
		PifLog(MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(hsEtkTotalFile));    /* then abort */
		PifLog(MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);    /* then abort */
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_OPEN);
    }

    /* Make Etk Paramater File */
    if ((sStatus = Etk_MakeParaFile(usNumberOfEmployee)) == ETK_NO_MAKE_FILE) {
        PifReleaseSem(husEtkSem);
		PifLog(MODULE_ETK, FAULT_ERROR_FILE_OPEN);    /* then abort */
		PifLog(MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(sStatus));    /* then abort */
		PifLog(MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);    /* then abort */
        return(ETK_NO_MAKE_FILE);
    }

    /* Make Etk Total File */
    if ((sStatus = Etk_MakeTotalFile(usNumberOfEmployee)) == ETK_NO_MAKE_FILE) {
        PifReleaseSem(husEtkSem);
        return(ETK_NO_MAKE_FILE);
    }

    Etk_CloseAllFileReleaseSem();
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkCheckAndCreatFile(USHORT usNumberOfEmployee)
*
*       Input:    UCHAR usNumberOfEmployee - Number of Etk
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*          Normal End:   ETK_SUCCESS
*        Abnormal End:   ETK_NO_MAKE_FILE
*
**  Description:
*               Check if not exist , then Create Etk File.
*==========================================================================
*/
SHORT   EtkCheckAndCreatFile(USHORT usNumberOfEmployee)
{
    SHORT   sStatus;

    /* Open Etk File */
    if ((hsEtkParaFile = PifOpenFile(auchETK_PARA_FILE, auchOLD_FILE_READ_WRITE)) < 0) {  /* pif_error_file_not_found) */
        sStatus = EtkCreatFile(usNumberOfEmployee);
        return(sStatus);
    }

    PifCloseFile(hsEtkParaFile);  hsEtkParaFile = -1;           // set file handle to invalid value after PifCloseFile()

    /* Open Etk Total File */
    /* saratoga */
    if (usNumberOfEmployee) {
        PifRequestSem(husEtkSem);          /* Request Semaphore */
        if ((hsEtkTotalFile = PifOpenFile(auchETK_TOTAL_FILE, auchOLD_FILE_READ_WRITE)) < 0) {  /* pif_error_file_not_exist */
        /* Make Etk Total File */

            if ((hsEtkTotalFile = PifOpenFile(auchETK_TOTAL_FILE, auchNEW_FILE_WRITE)) >= 0) {  /* pif_error_file_exist */

                if ((sStatus = Etk_MakeTotalFile(usNumberOfEmployee)) == ETK_NO_MAKE_FILE) {
                    PifCloseFile(hsEtkTotalFile);
					hsEtkTotalFile = -1;           // set file handle to invalid value after PifCloseFile()
                    PifReleaseSem(husEtkSem);
                    return(ETK_NO_MAKE_FILE);
                }
            }
        }
        PifCloseFile(hsEtkTotalFile);  hsEtkTotalFile = -1;           // set file handle to invalid value after PifCloseFile()
        PifReleaseSem(husEtkSem);
    }
    return(ETK_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:    SHORT   EtkCheckAndDeleteFile(USHORT usNumberOfEmployee)
*
*       Input:    UCHAR usNumberOfEmployee - Number of Etk
*      Output:    nothing
*       InOut:    nothing
*
**  Return    :
*            Normal End: ETK_SUCCESS    
*                        ETK_DELETE_FILE
**  Description:
*               Check ETK file, if not equal then delete file.
*==========================================================================
*/
SHORT   EtkCheckAndDeleteFile(USHORT usNumberOfEmployee)
{
	ETK_PARAHEAD EtkFileHead = {0};     /* ETK file header */

    if ((hsEtkParaFile = PifOpenFile(auchETK_PARA_FILE, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
         PifDeleteFile(auchETK_TOTAL_FILE);   /* Delete Etk File  Saratoga */
        return(ETK_SUCCESS);
    }

    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_PARAHEAD));
    PifCloseFile(hsEtkParaFile);  hsEtkParaFile = -1;           // set file handle to invalid value after PifCloseFile()

    if ( EtkFileHead.usNumberOfMaxEtk != usNumberOfEmployee) {
         PifDeleteFile(auchETK_PARA_FILE);   /* Delete Etk File  Ignore error */
         PifDeleteFile(auchETK_TOTAL_FILE);   /* Delete Etk File  Ignore error */
         PifLog(MODULE_ETK, LOG_EVENT_ETK_DELETE_FILE);
/*        return(ETK_DELETE_FILE); */
   }
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkTimeIn(ULONG ulEmployeeNo,
*                                             UCHAR  uchJobCode,
*                                             ETK_FIELD  *pEtkField)
*
*
*       Input:    ULONG ulEmployeeNo    -   Employee NO to be time in
*                 UCHAR uchJobCode      -   Job Code to be time in 
*      Output:    
*            typedef struct {
*                 USHORT  usYear                  * Year to be time in *
*                 USHORT  usMonth                 * Month to be time in *
*                 USHORT  usDay                   * Day to be time in *
*                 UCHAR   uchJobCode              * Job Code to be time in *
*                 USHORT  usTimeinTime            * Time to be time in *
*                 USHORT  usTimeinMinute          * Minute to be time in *
*                 USHORT  usTimeOutTime           * None *
*                 USHORT  usTimeOutMinute         * None *
*           } ETK_FIELD;
*                 
**       InOut:
*
**  Return    :
*            Normal End: ETK_SUCCESS
*          Abnormal End: 
*                        ETK_NOT_TIME_OUT
*                        ETK_LOCK
*                        ETK_FILE_NOT_FOUND
*                        ETK_NOT_IN_FILE
*                        ETK_NOT_IN_JOB
*                        ETK_FIELD_OVER
*
**  Description:
*               Etk time-in function.
*==========================================================================
*/
SHORT   EtkTimeIn(ULONG  ulEmployeeNo,
                           UCHAR  uchJobCode,
                           ETK_FIELD  *pEtkField)
{
    USHORT  usParaBlockNo;                    /* paramater block # */
    ETK_TOTALHEAD EtkTotalHead = {0};         /* ETK TOTAL FILE HEADER */
    ETK_PARAINDEX EtkParaIndex = {0};         /* ETK PARAMATER INDEX FILE */
    ETK_TOTALINDEX    EtkTotalIndex = {0};    /* Etk Record to be read */
    DATE_TIME DateTime;                       /* for TOD read */
    SHORT   sStatus;                          /* work status */
    ULONG   ulIndexPos;                       /* Etk_Index() 's */
    ETK_FIELD_IN_FILE EtkInField = {0};       /* Etk Field structcure's pointer in file */

    PifRequestSem(husEtkSem);    /* Request Semaphore */

    /* Etk individual lock check */    
    if (Etk_LockCheck() == ETK_LOCK) {
        PifReleaseSem(husEtkSem);
        return(ETK_LOCK);
    }

    /* File Open */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            
    
    /* Check Etk assigned on Paramater File */
    if (((sStatus = Etk_IndexPara(ETK_SEARCH_ONLY, ulEmployeeNo, &EtkParaIndex, &ulIndexPos)) == ETK_NOT_IN_FILE)) {
        Etk_CloseAllFileReleaseSem();      /* close file and release semaphore */
        return(sStatus);        
    }

    /* Read Etk Job Field */
    if (Etk_CheckAllowJob(&EtkParaIndex.EtkJob, uchJobCode) != ETK_SUCCESS) {
        Etk_CloseAllFileReleaseSem();      /* close file and release semaphore */
        return(ETK_NOT_IN_JOB);        
    }

    /* Get File Header in Total File */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));
                 
    if (Etk_Index(&EtkTotalHead.EtkCur, ETK_REGIST, ulEmployeeNo, &EtkTotalIndex, &usParaBlockNo, &ulIndexPos) == ETK_FULL) {
           Etk_CloseAllFileReleaseSem();
		   PifLog (MODULE_ETK, LOG_ERROR_ETK_ETK_FULL);
		   PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
           return(ETK_FULL);   /* return if assign ETK full */
     }

    /* Check Field full */
    if (ETK_MAX_FIELD == (EtkTotalIndex.uchStatus & ETK_MASK_FIELD_POINTER)) {
        Etk_CloseAllFileReleaseSem();
        return ETK_FIELD_OVER;
    }

    /* read TOD and make output condition */

    PifGetDateTime(&DateTime);
    pEtkField->usYear  = DateTime.usYear;
    pEtkField->usMonth = DateTime.usMonth;
    pEtkField->usDay   = DateTime.usMDay;
    pEtkField->uchJobCode = uchJobCode;
    pEtkField->usTimeinTime = DateTime.usHour;
    pEtkField->usTimeinMinute = DateTime.usMin;
    pEtkField->usTimeOutTime   = ETK_TIME_NOT_IN;
    pEtkField->usTimeOutMinute = ETK_TIME_NOT_IN;

    /* Check Time Out */

	// The following is to work around a field issue seen on some Amtrak trains in which
	// the initialization sequence which beings with the Start Log In screen in which
	// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
	// the Inventory app for other train trip startup activities does not complete properly.
	// The result is that the LSA is unable to complete the Start Log In screen task and
	// is stuck with it partially complete. A control string is used to automate this task
	// so as part of the sanity check as to whether to allow this workaround, check that a
	// control string is running.
	//
	// This change checks if the Employee is doing Time-In when they have already done a Time-In
	// without an intervening Time-Out. If we are Amtrak just ignore this error.
	//      Richard Chambers, Aug-17-2018, for Amtrak only
    if ((EtkTotalIndex.uchStatus & ETK_TIME_IN) != 0 && ! ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
        /* Forgot Timt Out */
        Etk_CloseAllFileReleaseSem();
        return ETK_NOT_TIME_OUT;
/*      EtkIndex.uchStatus++; */
        /* Check Field full */
/*      if (ETK_MAX_FIELD == (EtkIndex.uchStatus & ETK_MASK_FIELD_POINTER)) {
            return ETK_FIELD_OVER;
        }  */
    }

    /* Set ETK to ETK file */
    EtkInField.usMonth = pEtkField->usMonth;
    EtkInField.usDay   = pEtkField->usDay;
    EtkInField.uchJobCode = pEtkField->uchJobCode;
    EtkInField.usTimeinTime = pEtkField->usTimeinTime;
    EtkInField.usTimeinMinute = pEtkField->usTimeinMinute;
    EtkInField.usTimeOutTime = pEtkField->usTimeOutTime;
    EtkInField.usTimeOutMinute = pEtkField->usTimeOutMinute;

    Etk_TimeFieldWrite(&EtkTotalHead.EtkCur, &EtkTotalIndex, usParaBlockNo, &EtkInField);

    /* Set Time in Flag */
    EtkTotalIndex.uchStatus |= ETK_TIME_IN;

    /* Up-Date File Header */
    Etk_WriteFile(hsEtkTotalFile,  ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));

    /* Up-Date Index Record */
    Etk_WriteFile(hsEtkTotalFile, ulIndexPos, &EtkTotalIndex, sizeof(ETK_TOTALINDEX));

    Etk_CloseAllFileReleaseSem();      /* close file and release semaphore */

	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
		// An Amtrak requirement is that when an operator, LSA in Amtrak terminology, does a Time-in
		// then the cashier record is marked to enable operator Sign-in until the operator does a Time-out.
		CASIF   CasIf = {0};
		SHORT   sError;

		CasIf.ulCashierNo = ulEmployeeNo;
		sError = SerCasIndRead(&CasIf);
		if (sError == ETK_SUCCESS && (CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_OPERATOR_DENIED_AM) == (CAS_OPERATOR_DISABLED_AM)) {
			CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] &= ~(CAS_OPERATOR_DISABLED_AM);
//			CasIf.fbCashierStatus[CAS_CASHIERSTATUS_2] = (CAS_OPEN_CASH_DRAWER_SIGN_IN);  // Amtrak requested this option to be turned off by default, then changed their minds
			sStatus = CasAssign(&CasIf);
		}
	}

    return(ETK_SUCCESS);    
}

/*                 
*==========================================================================
**    Synopsis:    SHORT   EtkTimeOut(ULONG ulEmployeeNo,
*                                              ETK_FIELD  *pEtkField)
*       Input:    ULONG     ulEmployeeNo
*      Output:    ETK_FIELD pEtkField
*            typedef struct {
*                 USHORT  usMonth                 * Month to be time in *
*                 USHORT  usDay                   * Day to be time in *
*                 UCHAR   uchJobCode              * Job Code to be time in *
*                 USHORT  usTimeinTime            * Time to be time in *
*                 USHORT  usTimeinMinute          * Minute to be time in *
*                 USHORT  usTimeOutTime           * Time to be time out *
*                 USHORT  usTimeOutMinute         * Time to be time out *
*           } ETK_FIELD;
*       InOut:    Nothing
*
**  Return    :    
*            Normal End: ETK_SUCCESS
*          Abnormal End: 
*                        ETK_NOT_TIME_IN
*                        ETK_LOCK
*                        ETK_FILE_NOT_FOUND
*                        ETK_NOT_IN_FILE
*                        ETK_FIELD_OVER
*
**  Description:
*                Etk Sign-out function
*==========================================================================
*/
SHORT   EtkTimeOut(ULONG ulEmployeeNo, ETK_FIELD  *pEtkField)
{
    ETK_TOTALHEAD   EtkTotalHead = {0};   /* ETK Total File header */
    ETK_TOTALINDEX  EtkTotalIndex = {0};  /* Etk Record to be read */
    ETK_FIELD_IN_FILE EtkInFile = {0};    /* Etk field record to be read */
    DATE_TIME DateTime;          /* for TOD read */
    SHORT   sStatus;             /* work status */
    USHORT  usParaBlockNo;       /* paramater block # */
    ULONG   ulIndexPos;          /* Etk_Index() 's */

    PifRequestSem(husEtkSem);    /* Request Semaphore */

    /* Etk individual lock check */    
    if (Etk_LockCheck() == ETK_LOCK) {
        PifReleaseSem(husEtkSem);
        return(ETK_LOCK);
    }

    /* Total File Open.  If error then this function releases the semaphore */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get File Header in Total File */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));
                 
    if (((sStatus = Etk_Index(&EtkTotalHead.EtkCur, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkTotalIndex, &usParaBlockNo, &ulIndexPos)) == ETK_NOT_IN_FILE)) {
        Etk_CloseIndFileReleaseSem(hsEtkTotalFile);
        return(sStatus);
    }  /* sStatus is next used at checking ETK_NOT_TIME_OUT */

    /* Check Time in */
    if (!(EtkTotalIndex.uchStatus & ETK_TIME_IN)) {
        Etk_CloseIndFileReleaseSem(hsEtkTotalFile);
        return ETK_NOT_TIME_IN;
    }
        
    /* Check Field full */
    if (ETK_MAX_FIELD == (EtkTotalIndex.uchStatus & ETK_MASK_FIELD_POINTER)) {
        Etk_CloseIndFileReleaseSem(hsEtkTotalFile);
        return ETK_FIELD_OVER;
    }

    /* read TOD and make output condition */
    Etk_TimeFieldRead(&EtkTotalHead.EtkCur, &EtkTotalIndex, usParaBlockNo, &EtkInFile);

    PifGetDateTime(&DateTime);
    EtkInFile.usTimeOutTime   = DateTime.usHour;
    EtkInFile.usTimeOutMinute = DateTime.usMin;

    /* Make Out Put Data */
    pEtkField->usYear          = DateTime.usYear;
    pEtkField->usMonth         = EtkInFile.usMonth;
    pEtkField->usDay           = EtkInFile.usDay;
    pEtkField->uchJobCode      = EtkInFile.uchJobCode;     /* space */
    pEtkField->usTimeinTime    = EtkInFile.usTimeinTime;
    pEtkField->usTimeinMinute  = EtkInFile.usTimeinMinute;
    pEtkField->usTimeOutTime   = EtkInFile.usTimeOutTime;
    pEtkField->usTimeOutMinute = EtkInFile.usTimeOutMinute;

    /* Set ETK to ETK file */
    Etk_TimeFieldWrite(&EtkTotalHead.EtkCur, &EtkTotalIndex, usParaBlockNo, &EtkInFile);
    EtkTotalIndex.uchStatus++;                   /* Number of Field is +1 */

    /* Set Time out flag */
    EtkTotalIndex.uchStatus &= ~ETK_TIME_IN;

    /* Up-Date File Header */
    Etk_WriteFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));

    /* Up-Date Index Record */
    Etk_WriteFile(hsEtkTotalFile, ulIndexPos, &EtkTotalIndex, sizeof(ETK_TOTALINDEX));

    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);

	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
		// An Amtrak requirement is that when an operator, LSA in Amtrak terminology, does a Time-out
		// then the cashier record is marked to disable operator Sign-in until the operator does a Time-in.
		CASIF   CasIf = {0};
		SHORT   sError;

		CasIf.ulCashierNo = ulEmployeeNo;
		sError = SerCasIndRead(&CasIf);
		if (sError == ETK_SUCCESS) {
			CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] |= (CAS_OPERATOR_DISABLED_AM);
//			CasIf.fbCashierStatus[CAS_CASHIERSTATUS_2] = (CAS_OPEN_CASH_DRAWER_SIGN_IN);  // Amtrak requested this option to be turned off by default, then changed their minds
			sStatus = CasAssign(&CasIf);
		}
	}

    return(ETK_SUCCESS); 
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkAssign(ULONG     ulEmployeeNo,
*                                             ETK_JOB   *pEtkJob,
*                                             UCHAR     *puchEmployeeName)
*
*       Input:      ulEmployeeNo    -   Employee Number to be assign
*                   struct {
*                       UCHAR   uchJobCode1;     Allowance Job code 1
*                       UCHAR   uchJobCode2;     Allowance Job code 2
*                       UCHAR   uchJobCode3;     Allowance Job code 3
*                   } ETK_JOB
*                   puchEmployeeName -   Employee Name   
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*                           ETK_FULL
*
**  Description:
*                Add, Regist Inputed Etk.
*==========================================================================
*/
SHORT   EtkAssignImplement (ULONG ulEmployeeNo, ETK_JOB *pEtkJob, TCHAR *puchEmployeeName)
{
    ETK_PARAINDEX   EtkParaIndex = {0};
    ETK_TOTALHEAD   EtkTotalHead = {0};
    ETK_TOTALINDEX  EtkTotalIndex = {0};
    USHORT          usParaBlockNo;
    ULONG           ulIndexPos;          /* Etk_Index() 's */

    /* Make Para Index Record */
    EtkParaIndex.ulEmployeeNo = ulEmployeeNo;
    EtkParaIndex.EtkJob = *pEtkJob;
    
    /* ===== Add Employee Name (R3.1) BEGIN ===== */
    _tcsncpy( EtkParaIndex.auchEmployeeName, puchEmployeeName, STD_CASHIERNAME_LEN );
    /* ===== Add Employee Name (R3.1) END ===== */

    /* Assign Etk */
    if (Etk_IndexPara(ETK_REGIST, ulEmployeeNo, &EtkParaIndex, &ulIndexPos) == ETK_FULL) {
           return(ETK_FULL);   /* return if assign ETK full */
    }

    /* Get File Header in Total File */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));

    if (Etk_Index(&EtkTotalHead.EtkCur, ETK_REGIST, ulEmployeeNo, &EtkTotalIndex, &usParaBlockNo, &ulIndexPos) == ETK_FULL) {
        return(ETK_FULL);   /* return if assign ETK full */
    }

    /* Up-Date File Header */
    Etk_WriteFile(hsEtkTotalFile,  ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));

    return(ETK_SUCCESS);
}

SHORT   EtkAssign(ULONG    ulEmployeeNo,
                           ETK_JOB  *pEtkJob,
                           TCHAR    *puchEmployeeName)
{
	SHORT  sRetStatus;

    PifRequestSem(husEtkSem);

    /* Open File */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            

	sRetStatus = EtkAssignImplement (ulEmployeeNo, pEtkJob, puchEmployeeName);
    if (sRetStatus != ETK_SUCCESS) {
		PifLog (MODULE_ETK, LOG_ERROR_ETK_ETK_FULL);
		PifLog (MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(sRetStatus));
		PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
    }

    Etk_CloseAllFileReleaseSem();  /* file close and release semaphore */
    return(sRetStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkDelete(ULONG ulEmployeeNo)
*
*       Input:    ULONG ulEmployeeNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*                           ETK_NOT_IN_FILE
*                           ETK_DATA_EXIST
*
**  Description:
*                Delete Inputed Etk No from index table total and para file.
*==========================================================================
*/
SHORT   EtkDelete(ULONG ulEmployeeNo)
{
    SHORT   sStatus;                        /* status save area */
    ETK_TOTALHEAD    EtkTotalHead = {0};    /* Total File header */
    ETK_PARAINDEX    EtkParaIndex = {0};    /* Etk Paramater Record to be read */
    ETK_TOTALINDEX   EtkTotalIndex = {0};   /* Etk Total index Record to be read */
    USHORT       usParaBlockNo;
    ULONG        ulIndexPos;

    PifRequestSem(husEtkSem);   /* request semaphore */

    /* Open file */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get File Header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));

    /* Check ETK assign and delete */
    if (((sStatus = Etk_IndexPara(ETK_SEARCH_ONLY, ulEmployeeNo, &EtkParaIndex, &ulIndexPos)) != ETK_NOT_IN_FILE)) {
        /* total check */
        if (Etk_Index(&EtkTotalHead.EtkCur, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkTotalIndex, &usParaBlockNo, &ulIndexPos) == ETK_RESIDENT) {
            if (EtkTotalIndex.uchStatus) {
                /* EtkTotalIndex.uchStatus & ETK_MASK_FIELD_POINTER / EtkIndex == 8x */
                sStatus = ETK_DATA_EXIST;
            } else {
                if ((sStatus = Etk_IndexDel(&EtkTotalHead, ulEmployeeNo)) ==
                    ETK_SUCCESS) {
                    sStatus = Etk_IndexDelPara(ulEmployeeNo);
                }
            }
        } else {
            sStatus = Etk_IndexDelPara(ulEmployeeNo);
        }
    }
    Etk_CloseAllFileReleaseSem();      /* file close and release semaphore */
    return(sStatus);        
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkIndJobRead(ULONG ulEmployeeNo,
*                                                 ETK_JOB  *pEtkJob,
*                                                 UCHAR    *puchEmployeeName)
*
*       Input:    ULONG     ulEmployeeNo
*      Output:    ETK_JOB   *pEtkJob
*                 UCAHR     *puchEmployeeName
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description:
*                Read all allowance Job Code.
*
*                WARNING:  up to the max number of characters in an
*                          employee name. it is up to the caller to make the
*                          buffer large enough and to make sure that
*                          puchEmployeeName will be zero terminated if the full
*                          number of characters are used in the employee name mnemonic.
*==========================================================================
*/
SHORT   EtkIndJobRead(ULONG    ulEmployeeNo,
                               ETK_JOB  *pEtkJob,
                               TCHAR    *puchEmployeeName )
{
    SHORT   sStatus;                 /* Save status */
	ETK_PARAINDEX   EtkIndex = {0};        /* Index record save area */
    ULONG   ulIndexPos;              /* Etk_Index() 's */

    PifRequestSem(husEtkSem);   /* request samaphore */

    /* open file */
    if (Etk_OpenIndFile(ETK_PARAM_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* check ETK assgin */
    if ((sStatus = Etk_IndexPara(ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &ulIndexPos)) != ETK_RESIDENT) {
        Etk_CloseIndFileReleaseSem(hsEtkParaFile);
        return(sStatus);        
    }

    /* Make Output Condition */
    *pEtkJob = EtkIndex.EtkJob;

	// up to the max number of characters in an employee name. it is up to the caller to make the
	// buffer large enough and to make sure that puchEmployeeName will be zero terminated if the full
	// number of characters are used in the employee name mnemonic.
    _tcsncpy( puchEmployeeName, EtkIndex.auchEmployeeName, STD_CASHIERNAME_LEN );

    Etk_CloseIndFileReleaseSem(hsEtkParaFile);                                /* close file and release semaphore */
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT  EtkCurAllIdRead(USHORT usRcvBufferSize,
*                                                 ULONG  *aulRcvBuffer)
*
*       Input:    USHORT usRcvBufferSize
*      Output:    
*       InOut:    USHORT *aulRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description:
*                Read all Etk No. in this file.
*==========================================================================
*/
SHORT  EtkCurAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer)
{
    USHORT  cusi;                                                
	ETK_PARAHEAD   EtkFileHead = {0};      /* file header */
    ETK_PARAINDEX  EtkIndexEntry = {0};    /* index table entry */ 

    PifRequestSem(husEtkSem);       /* request semaphore */

    /* open Paramater file */
    if (Etk_OpenIndFile(ETK_PARAM_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* get file header */
    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_PARAHEAD));

    /* Count inputed ETK no in index table */
    for (cusi = 0;
         ((cusi * sizeof(EtkIndexEntry.ulEmployeeNo)) < usRcvBufferSize) &&
          (cusi < EtkFileHead.usNumberOfEtk);
         cusi++) {
        Etk_ReadFile(hsEtkParaFile, sizeof(ETK_PARAHEAD) + (cusi * sizeof(ETK_PARAINDEX)), &EtkIndexEntry, sizeof(ETK_PARAINDEX));
        *aulRcvBuffer++ = EtkIndexEntry.ulEmployeeNo;
    }

    Etk_CloseIndFileReleaseSem(hsEtkParaFile);      /* close file and release semaphore */
    return(cusi);
}


/*
*==========================================================================
**    Synopsis:   SHORT  EtkCurAllIdRead(USHORT usRcvBufferSize,
*                                                 ULONG  *aulRcvBuffer)
*
*       Input:    USHORT usRcvBufferSize
*      Output:    
*       InOut:    USHORT *aulRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description:
*                Read all Etk No. in this file.
*==========================================================================
*/
SHORT  EtkCurAllIdReadIncr(USHORT usRcvBufferSize, ULONG *aulRcvBuffer,USHORT usEmpNo)
{
    USHORT  cusi, usReadRecords = 0;                                                
	ETK_PARAHEAD   EtkFileHead = {0};       /* file header */
    ETK_PARAINDEX  EtkIndexEntry = {0};     /* index table entry */ 

    PifRequestSem(husEtkSem);       /* request semaphore */

    /* open Paramater file */
    if (Etk_OpenIndFile(ETK_PARAM_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* get file header */
    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_PARAHEAD));

    /* Count inputed ETK no in index table */
    for (cusi = usEmpNo;
         ((usReadRecords * sizeof(EtkIndexEntry.ulEmployeeNo)) < usRcvBufferSize) &&
          (cusi < EtkFileHead.usNumberOfEtk);
         cusi++) {
        Etk_ReadFile(hsEtkParaFile, sizeof(ETK_PARAHEAD) + (cusi * sizeof(ETK_PARAINDEX)), &EtkIndexEntry, sizeof(ETK_PARAINDEX));
        *aulRcvBuffer++ = EtkIndexEntry.ulEmployeeNo;
		usReadRecords++;
    }

    Etk_CloseIndFileReleaseSem(hsEtkParaFile);      /* close file and release semaphore */
    return(usReadRecords);
}
                                                                                                                                               
/*
*==========================================================================
**    Synopsis:   SHORT  EtkSavAllIdRead(USHORT usRcvBufferSize,
*                                                 ULONG  *aulRcvBuffer)
*
*       Input:    USHORT usRcvBufferSize
*      Output:    
*       InOut:    ULONG *aulRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description:
*                Read all Etk No. in this file.
*==========================================================================
*/
SHORT  EtkSavAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer)
{
    USHORT  cusi;                                                
    ETK_TOTALHEAD  EtkFileHead = {0};      /* file header */
    ETK_TOTALINDEX EtkIndexEntry = {0};    /* index table entry */ 

    PifRequestSem(husEtkSem);       /* request semaphore */
                            
    /* open file */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* get file header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Count inputed ETK no in index table */
    for (cusi = 0;
         ((cusi * sizeof(EtkIndexEntry.ulEmployeeNo)) < usRcvBufferSize) &&
          (cusi < EtkFileHead.EtkSav.usNumberOfEtk);
         cusi++) {
        Etk_ReadFile(hsEtkTotalFile, EtkFileHead.EtkSav.ulOffsetOfIndex + (cusi * sizeof(ETK_TOTALINDEX)), &EtkIndexEntry, sizeof(ETK_TOTALINDEX));
        *aulRcvBuffer++ = EtkIndexEntry.ulEmployeeNo;
    }

    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);      /* close file and release semaphore */
    return(cusi);
}
/*
*==========================================================================
**    Synopsis:    SHORT   EtkCurIndRead(ULONG ulEmployeeNo,
*                                        USHORT *pusFieldNo,
*                                        ETK_FIELD  *pEtkField,
*                                        ETK_TIME   *pEtkTime)
*
*       Input:    ULONG     ulEmployeeNo
*                 USHORT    *pusFieldNo    
*      Output:    USHORT    *pusFieldNo
*                 ETK_FIELD *pEtkField
*                 ETK_TIME  *pEtkTime
*       InOut:
*
**  Return    :
*            Normal End:    ETK_SUCCESS          All field is read.
*                           ETK_CONTINUE         This is still
*                                                far from complete.
*          Abnormal End:    ETK_FILE_NOT_FOUND   file is not found.
*                           ETK_NOT_IN_FILE      This employee no
*                                                is not in file.
*                           ETK_NOT_IN_FIELD     Data is not on and after
*                                                this field.
*
**  Description:
*                Read Individual ETK record Field in current buffer
*                by ulEmployeeNo.
*                but Read only 1 Field at one time. then if user read
*                all field, user call this function many time until
*                getting return value that is ETK_SUCCESS or ETK_NOT_IN_FIELD.
*                ETK_CONTINUE: this field is getting now is data in it.
*                              data is still in it.
*                ETK_NOT_IN_FIELD: Data is not on and after this field.
*                pusFieldNo is set 0 at First Time by user.
*                and User does not touch pusFieldNo after 2nd Time.
*                because pusFiledNo is setted by this function.
*                and this function use it only.
*                pusFiledNo is setted filed No in Record to be read.
*                
*==========================================================================
*/
SHORT   EtkCurIndRead(ULONG ulEmployeeNo,
                      USHORT *pusFieldNo,
                      ETK_FIELD  *pEtkField,
                      ETK_TIME   *pEtkTime)
{
    UCHAR    uchFieldNo;                        /* Field Number work */
    USHORT   usParaBlockNo;                     /* paramater block no save area */
    SHORT    sErrorStatus;                      /* error status save area */
    ETK_TOTALINDEX     EtkIndex = {0};          /* Etk Record to be read */
    ETK_TOTALINDEX     EtkIndexWork = {0};      /* work of above */
    ETK_FIELD_IN_FILE  EtkFieldInFile = {0};    /* Field record in file */
    ETK_TOTALHEAD      EtkFileHead = {0};       /* file header save area */
    ULONG    ulIndexPos;                        /* Etk_Index() 's */
    DATE_TIME DateTime;

    PifRequestSem(husEtkSem);            /* request samaphore */

    /* open file */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get File Header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Search inputed ETK no in Index table */
    if (Etk_Index(&EtkFileHead.EtkCur, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &usParaBlockNo, &ulIndexPos) == ETK_RESIDENT) {
        /* Read Paramater */
        uchFieldNo = (UCHAR)(EtkIndex.uchStatus & ETK_MASK_FIELD_POINTER);
        if (EtkIndex.uchStatus & ETK_TIME_IN) {
            uchFieldNo++;
        }
        if (uchFieldNo == 0) {
            sErrorStatus = ETK_NOT_IN_FIELD;
        } else if ((*pusFieldNo >= ETK_MAX_FIELD) ||
            (uchFieldNo <= *(UCHAR *)pusFieldNo)) {
            sErrorStatus = ETK_NOT_IN_FIELD;
        } else {
            EtkIndexWork.uchStatus = *(UCHAR *)pusFieldNo;
            Etk_TimeFieldRead(&EtkFileHead.EtkCur, &EtkIndexWork,
                              usParaBlockNo, &EtkFieldInFile);
            PifGetDateTime(&DateTime);                          /* Jan/16/2001  TAR#160538 */
            pEtkField->usYear  =         DateTime.usYear;       /* Jan/16/2001  TAR#160538 */
            pEtkField->usMonth =         EtkFieldInFile.usMonth;
            pEtkField->usDay   =         EtkFieldInFile.usDay;
            pEtkField->uchJobCode =      EtkFieldInFile.uchJobCode;
            pEtkField->usTimeinTime =    EtkFieldInFile.usTimeinTime;
            pEtkField->usTimeinMinute =  EtkFieldInFile.usTimeinMinute;
            pEtkField->usTimeOutTime =   EtkFieldInFile.usTimeOutTime;
            pEtkField->usTimeOutMinute = EtkFieldInFile.usTimeOutMinute;
            (*pusFieldNo)++;
            if (*pusFieldNo == (USHORT)uchFieldNo) {
                sErrorStatus = ETK_SUCCESS;
            } else {
                sErrorStatus = ETK_CONTINUE;
            }
        }
    } else {
        sErrorStatus = ETK_NOT_IN_FILE;
    }
    *pEtkTime = EtkFileHead.EtkTime;     /* copy structure */
    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);   /* close file and release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkCurAllLoggedInRead(ULONG ulEmployeeNo,
*                                        USHORT *pusFieldNo,
*                                        ETK_FIELD  *pEtkField,
*                                        ETK_TIME   *pEtkTime)
*
*       Input:    ULONG     ulEmployeeNo
*                 USHORT    *pusFieldNo    
*      Output:    USHORT    *pusFieldNo
*                 ETK_FIELD *pEtkField
*                 ETK_TIME  *pEtkTime
*       InOut:
*
**  Return    :
*            Normal End:    ETK_SUCCESS          All field is read.
*                           ETK_CONTINUE         This is still
*                                                far from complete.
*          Abnormal End:    ETK_FILE_NOT_FOUND   file is not found.
*                           ETK_NOT_IN_FILE      This employee no
*                                                is not in file.
*                           ETK_NOT_IN_FIELD     Data is not on and after
*                                                this field.
*
**  Description:
*                
*==========================================================================
*/
SHORT   EtkCurAllLoggedInRead(USHORT usRcvArraySize, ETK_DETAILS *Etk_Details)
{
	SHORT    sErrorStatus;               /* error status save area */

    PifRequestSem(husEtkSem);            /* request samaphore */

	if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
		return(ETK_FILE_NOT_FOUND);
	}

	{
		ETK_TOTALINDEX     EtkIndexEntry = {0};          /* Etk Record to be read */
		ETK_TOTALINDEX     EtkIndexWork = {0};
		ETK_FIELD_IN_FILE  EtkFieldInFile = {0};         /* Field record in file */
		ETK_TOTALHEAD      EtkFileHead = {0};            /* file header save area */
		DATE_TIME          DateTime;
		USHORT             cusi, usReadRecords = 0;                                                

		/* Get File Header */
		Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

		/* Count inputed ETK no in index table */
		for (cusi = 0; (usReadRecords < usRcvArraySize) && (cusi < EtkFileHead.EtkCur.usNumberOfEtk); cusi++) {
			Etk_ReadFile(hsEtkTotalFile, EtkFileHead.EtkCur.ulOffsetOfIndex + (cusi * sizeof(ETK_TOTALINDEX)), &EtkIndexEntry, sizeof(ETK_TOTALINDEX));
			if (EtkIndexEntry.uchStatus & ETK_TIME_IN) {
				Etk_Details[usReadRecords].ulEmployeeNo = EtkIndexEntry.ulEmployeeNo;

				Etk_TimeFieldRead(&EtkFileHead.EtkCur, &EtkIndexWork, EtkIndexEntry.usBlockNo, &EtkFieldInFile);
				PifGetDateTime(&DateTime);                          /* Jan/16/2001  TAR#160538 */

				Etk_Details[usReadRecords].EtkDetails.usYear  =         DateTime.usYear;       /* Jan/16/2001  TAR#160538 */
				Etk_Details[usReadRecords].EtkDetails.usMonth =         EtkFieldInFile.usMonth;
				Etk_Details[usReadRecords].EtkDetails.usDay   =         EtkFieldInFile.usDay;
				Etk_Details[usReadRecords].EtkDetails.uchJobCode =      EtkFieldInFile.uchJobCode;
				Etk_Details[usReadRecords].EtkDetails.usTimeinTime =    EtkFieldInFile.usTimeinTime;
				Etk_Details[usReadRecords].EtkDetails.usTimeinMinute =  EtkFieldInFile.usTimeinMinute;
				Etk_Details[usReadRecords].EtkDetails.usTimeOutTime =   EtkFieldInFile.usTimeOutTime;
				Etk_Details[usReadRecords].EtkDetails.usTimeOutMinute = EtkFieldInFile.usTimeOutMinute;
				usReadRecords++;
			}
		}

		sErrorStatus = usReadRecords;
	}

	{
		USHORT         cusi, usReadRecords = 0;                                                
		ETK_PARAHEAD   EtkFileHead = {0};        /* file header */
		ETK_PARAINDEX  EtkIndexEntry = {0};      /* index table entry */ 

		/* get file header */
		Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_PARAHEAD));

		/* Count inputed ETK no in index table */
		for (cusi = 0; (usReadRecords < sErrorStatus) && (cusi < EtkFileHead.usNumberOfEtk); cusi++) {
			Etk_ReadFile(hsEtkParaFile, sizeof(ETK_PARAHEAD) + (cusi * sizeof(ETK_PARAINDEX)), &EtkIndexEntry, sizeof(ETK_PARAINDEX));
			if (EtkIndexEntry.ulEmployeeNo == Etk_Details[usReadRecords].ulEmployeeNo) {
				Etk_Details[usReadRecords].EtkJob = EtkIndexEntry.EtkJob;
				memcpy (Etk_Details[usReadRecords].auchEmployeeName, EtkIndexEntry.auchEmployeeName, sizeof(EtkIndexEntry.auchEmployeeName));
				usReadRecords++;
			}
			else if (EtkIndexEntry.ulEmployeeNo > Etk_Details[usReadRecords].ulEmployeeNo) {
				while (EtkIndexEntry.ulEmployeeNo > Etk_Details[usReadRecords].ulEmployeeNo && usReadRecords < sErrorStatus) {
					memset (&(Etk_Details[usReadRecords].EtkDetails), 0, sizeof(Etk_Details[usReadRecords].EtkDetails));
					memset (Etk_Details[usReadRecords].auchEmployeeName, 0, sizeof(Etk_Details[usReadRecords].auchEmployeeName));
					usReadRecords++;
				}
			}
		}
	}

	Etk_CloseAllFileReleaseSem();   /* close file and release semaphore */

    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkSavIndRead(ULONG ulEmployeeNo,
*                                        USHORT *pusFieldNo,
*                                        ETK_FIELD  *pEtkField,
*                                        ETK_TIME   *pEtkTime)
*
*       Input:    ULONG     ulEmployeeNo
*                 USHORT    *pusFieldNo    
*      Output:    USHORT    *pusFieldNo
*                 ETK_FIELD *pEtkField
*                 ETK_TIME  *pEtkTime
*       InOut:
*
**  Return    :
*            Normal End:    ETK_SUCCESS          All field is read.
*                           ETK_CONTINUE         This is still
*                                                far from complete.
*          Abnormal End:    ETK_FILE_NOT_FOUND   file is not found.
*                           ETK_NOT_IN_FILE      This employee no
*                                                is not in file.
*                           ETK_NOT_IN_FIELD     Data is not on and after
*                                                this field.
*
**  Description:
*                Read Individual ETK record Field in saved buffer by ulEmployeeNo.
*                but Read only 1 Field at one time. then if user read
*                all field, user call this function many time until
*                getting return value that is ETK_SUCCESS or ETK_NOT_IN_FIELD.
*                ETK_CONTINUE: this field is getting now is data in it.
*                              data is still in it.
*                ETK_NOT_IN_FIELD: Data is not on and after this field.
*                pusFieldNo is set 0 at First Time by user.
*                and User does not touch pusFieldNo after 2nd Time.
*                because pusFiledNo is setted by this function.
*                and this function use it only.
*                pusFiledNo is setted filed No in Record to be read.
*                
*==========================================================================
*/
SHORT   EtkSavIndRead(ULONG    ulEmployeeNo,
                       USHORT *pusFieldNo,
                       ETK_FIELD  *pEtkField,
                       ETK_TIME   *pEtkTime)
{
    UCHAR    uchFieldNo;                        /* Field Number work */
    USHORT   usParaBlockNo;                     /* paramater block no save area */
    SHORT    sErrorStatus;                      /* error status save area */
    ETK_TOTALINDEX     EtkIndex = {0};          /* Etk Record to be read */
    ETK_TOTALINDEX     EtkIndexWork = {0};      /* work of above */
    ETK_FIELD_IN_FILE  EtkFieldInFile = {0};    /* Field record in file */
    ETK_TOTALHEAD      EtkFileHead = {0};       /* file header save area */
    ULONG    ulIndexPos;                        /* Etk_Index() 's */
    DATE_TIME DateTime;

    PifRequestSem(husEtkSem);            /* request samaphore */

    /* open file */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get file header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Search inputed ETK no in Index table */
    if (Etk_Index(&EtkFileHead.EtkSav, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &usParaBlockNo, &ulIndexPos) == ETK_RESIDENT) {
        /* Read Paramater */
        uchFieldNo = (UCHAR)(EtkIndex.uchStatus & ETK_MASK_FIELD_POINTER);
        if (EtkIndex.uchStatus & ETK_TIME_IN) {
            uchFieldNo++;
        }
        if (uchFieldNo == 0) {
            sErrorStatus = ETK_NOT_IN_FIELD;
        } else if ((*pusFieldNo >= ETK_MAX_FIELD) ||
            (uchFieldNo <= *(UCHAR *)pusFieldNo)) {
            sErrorStatus = ETK_NOT_IN_FIELD;
        } else {
            EtkIndexWork.uchStatus = *(UCHAR *)pusFieldNo;
            Etk_TimeFieldRead(&EtkFileHead.EtkSav, &EtkIndexWork, usParaBlockNo, &EtkFieldInFile);

            PifGetDateTime(&DateTime);                          /* Jan/16/2001  TAR#160538 */
            pEtkField->usYear  =         DateTime.usYear;       /* Jan/16/2001  TAR#160538 */
            pEtkField->usMonth =         EtkFieldInFile.usMonth;
            pEtkField->usDay   =         EtkFieldInFile.usDay;
            pEtkField->uchJobCode =      EtkFieldInFile.uchJobCode;
            pEtkField->usTimeinTime =    EtkFieldInFile.usTimeinTime;
            pEtkField->usTimeinMinute =  EtkFieldInFile.usTimeinMinute;
            pEtkField->usTimeOutTime =   EtkFieldInFile.usTimeOutTime;
            pEtkField->usTimeOutMinute = EtkFieldInFile.usTimeOutMinute;
            (*pusFieldNo)++;
            if (*pusFieldNo == (USHORT)uchFieldNo) {
                sErrorStatus = ETK_SUCCESS;
            } else {
                sErrorStatus = ETK_CONTINUE;
            }
        }
    } else {
        sErrorStatus = ETK_NOT_IN_FILE;
    }
    *pEtkTime = EtkFileHead.EtkTime;     /* copy structure */
    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);    /* close file and release semaphore */
    return(sErrorStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   EtkAllReset(VOID)
*
*       Input:    
*      Output:
*       InOut:
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*                           ETK_NOT_ISSUED
*                           ETK_NOT_TIME_OUT
*
**  Description:
*                Reset All employee records.
*                1, "Reset" That is data in the current buffer to be emptied
*                   into the saved buffer.
*                   but it takes a lot of time to do the above.
*                   then this function swap the current buffer's offset value
*                   in the ETK file header with the saved buffer's one.
*                2, Erase the current buffer's data.
*                   but it takes a lot of time to do the above.
*                   then Erased the buffer's data is only field pointer on
*                   status field at index table and paramater block empty
*                   table in the ETK file. "erase" fill the memory with null.
*                
*==========================================================================
*/
SHORT   EtkAllReset(VOID)
{
    ETK_TOTALHEAD   EtkFileHead = {0};         /* File Header */
    ETK_HEAD        EtkHeadWork = {0};         /* for Work */
    DATE_TIME       DateTime;                  /* for TOD read */
    ETK_TOTALINDEX  aWorkRcvBuffer[50] = {0};  /* reduce work buffer 4/9/96 */
/*    ETK_TOTALINDEX aWorkRcvBuffer[FLEX_ETK_MAX]; */
    ULONG           ulWriteOffset;
    ULONG           ulReadOffset;
    USHORT          usBytesToRead;
    USHORT          usBytesWritten;
    USHORT          usMaxWriteSize;

    PifRequestSem(husEtkSem);            /* request samaphore */

    /* open file */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get file header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Check Issued flag */
    if (EtkFileHead.uchIssuedFlag == ETK_FLAG_SET) {
        Etk_CloseIndFileReleaseSem(hsEtkTotalFile);
        return(ETK_NOTISSUED);
    }
    /* Check time in */
    if (Etk_CheckStatus(&EtkFileHead.EtkCur) == ETK_NOT_TIME_OUT) {
        Etk_CloseIndFileReleaseSem(hsEtkTotalFile);
        return(ETK_NOT_TIME_OUT);
    }

    /* Swap the saved buffer's offset in the ETK file
        with the current buffer's one */
    EtkHeadWork = EtkFileHead.EtkSav;
    EtkFileHead.EtkSav = EtkFileHead.EtkCur;
    EtkFileHead.EtkCur = EtkHeadWork;

    /* Move Index File */
/******************* Reduce Memory Size (4/9/96) *****************
    Etk_ReadFile(hsEtkTotalFile, EtkFileHead.EtkSav.ulOffsetOfIndex, 
                 aWorkRcvBuffer,
                 sizeof(ETK_TOTALINDEX) * (EtkFileHead.EtkSav.usNumberOfEtk));
    Etk_WriteFile(hsEtkTotalFile, EtkFileHead.EtkCur.ulOffsetOfIndex, 
                  aWorkRcvBuffer,
                  sizeof(ETK_TOTALINDEX) * (EtkFileHead.EtkSav.usNumberOfEtk));
******************* Reduce Memory Size (4/9/96) ******************/

    usBytesToRead  = sizeof(aWorkRcvBuffer);
    usBytesWritten = 0;
    usMaxWriteSize = sizeof(ETK_TOTALINDEX) * EtkFileHead.EtkSav.usNumberOfEtk;
    ulReadOffset   = EtkFileHead.EtkSav.ulOffsetOfIndex;
    ulWriteOffset  = EtkFileHead.EtkCur.ulOffsetOfIndex;

    while ( usBytesWritten < usMaxWriteSize ) {
        if ( usMaxWriteSize < ( usBytesWritten + usBytesToRead )) {
            usBytesToRead = usMaxWriteSize - usBytesWritten;
        }
        Etk_ReadFile(hsEtkTotalFile, ulReadOffset, aWorkRcvBuffer, usBytesToRead);
        Etk_WriteFile(hsEtkTotalFile, ulWriteOffset, aWorkRcvBuffer, usBytesToRead);

        ulReadOffset  += ( ULONG )(usBytesToRead);
        ulWriteOffset += ( ULONG )(usBytesToRead);
        usBytesWritten += usBytesToRead;
    }

    /* Move Empty Table Size */

    Etk_ReadFile(hsEtkTotalFile, EtkFileHead.EtkSav.ulOffsetOfParaEmp, aWorkRcvBuffer, ETK_PARA_EMPTY_TABLE_SIZE);
    Etk_WriteFile(hsEtkTotalFile, EtkFileHead.EtkCur.ulOffsetOfParaEmp, aWorkRcvBuffer, ETK_PARA_EMPTY_TABLE_SIZE);

    EtkFileHead.EtkCur.usNumberOfEtk = EtkFileHead.EtkSav.usNumberOfEtk;

    /* Erace Current Buffer's data */
    PifGetDateTime(&DateTime);
    EtkFileHead.uchIssuedFlag = ETK_FLAG_SET;
    EtkFileHead.EtkTime.usFromMonth  = EtkFileHead.EtkTime.usToMonth;
    EtkFileHead.EtkTime.usFromDay    = EtkFileHead.EtkTime.usToDay;
    EtkFileHead.EtkTime.usFromTime   = EtkFileHead.EtkTime.usToTime;
    EtkFileHead.EtkTime.usFromMinute = EtkFileHead.EtkTime.usToMinute;
    EtkFileHead.EtkTime.usToMonth  =  DateTime.usMonth;
    EtkFileHead.EtkTime.usToDay    =  DateTime.usMDay;
    EtkFileHead.EtkTime.usToTime   =  DateTime.usHour;
    EtkFileHead.EtkTime.usToMinute =  DateTime.usMin;

    Etk_ClearStatus(&EtkFileHead.EtkCur);

    /* write file header to total file */
    Etk_WriteFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);
    return(ETK_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:    SHORT   EtkAllLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*
**  Description:
*                Lock All Etk. 
*==========================================================================
*/
SHORT   EtkAllLock(VOID)
{
    SHORT   sErrorStatus;

    PifRequestSem(husEtkSem);               /* reqest semaphore */

    if (Etk_OpenIndFile(ETK_PARAM_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    if (Etk_LockCheck() == ETK_LOCK) {        /* check all lock memory. */
        sErrorStatus = ETK_LOCK;
    } else {
        uchEtkAllLockMem = 0xff;
        sErrorStatus = ETK_SUCCESS;
    }
    Etk_CloseIndFileReleaseSem(hsEtkParaFile);      /* close file and release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    VOID   EtkAllUnLock(VOID)
*
*       Input:    Nothing
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*                Unlock All Etk
*==========================================================================
*/
VOID   EtkAllUnLock(VOID)
{
    PifRequestSem(husEtkSem);       /* reqest semaphore */
    uchEtkAllLockMem = 0x00;        /* clear all ETK lock mamory */
    PifReleaseSem(husEtkSem);       /* release semaphore */
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkIssuedCheck(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_ISSUED
*                           ETK_NOTISSUED
*          Abnormal End:    ETK_FILE_NOT_FOUND
*                           
*
**  Description:
*                Check issued flag. 
*==========================================================================
*/
SHORT   EtkIssuedCheck(VOID)
{
    SHORT   sErrorStatus;
    ETK_TOTALHEAD EtkFileHead = {0};     /* ETK file header */

    PifRequestSem(husEtkSem);               /* reqest semaphore */

    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get File Header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Check Issued Flag */
    if (EtkFileHead.uchIssuedFlag == ETK_FLAG_SET) {
        sErrorStatus = ETK_NOTISSUED;
    } else {
        sErrorStatus = ETK_ISSUED;
    }

    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);      /* close file and release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkIssuedSet(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS        * setted issued flag *
*          Abnormal End:    ETK_NOTISSUED      * already setted issued flag *
*                           ETK_FILE_NOT_FOUND * File is not found *
*
**  Description:
*               Set Issued flag. 
*==========================================================================
*/
SHORT   EtkIssuedSet(VOID)
{
    ETK_TOTALHEAD EtkFileHead = {0};     /* ETK file header */

    PifRequestSem(husEtkSem);               /* reqest semaphore */

    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get File Header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Set Issued Flag */
    EtkFileHead.uchIssuedFlag = ETK_FLAG_CLEAR;

    /* write file header to total file */
    Etk_WriteFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));
    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);      /* close file and release semaphore */
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkOpenFile(CONST CHAR FAR *pszMode,
*                                               UCHAR uchFileType,
*                                               SHORT *phsEtkFile)
*
*       Input:    pszMode
*                 uchFileType           paramater file : 0 ETK_PARAM_FILE 
*                                       total file     : 1 ETK_TOTAL_FILE
*      Output:    SHORT *phsEtkFile
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*                
**  Description:
*                Open ETK file.
*==========================================================================
*/
SHORT   EtkOpenFile(CONST CHAR FAR *pszMode,
                             UCHAR uchFileType,
                             SHORT *phsEtkFile)
{
    SHORT   sError = ETK_SUCCESS;

    PifRequestSem(husEtkSem);       /* reqest semaphore */

    if (uchFileType == ETK_PARAM_FILE) {
        if ((*phsEtkFile = PifOpenFile(auchETK_PARA_FILE, pszMode)) < 0) {  /* pif_error_file_exist */
            sError = ETK_FILE_NOT_FOUND;
        }
    } else if (uchFileType == ETK_TOTAL_FILE) {
        if ((*phsEtkFile = PifOpenFile(auchETK_TOTAL_FILE, pszMode)) < 0) {  /* pif_error_file_exist */
            sError = ETK_FILE_NOT_FOUND;
        }
    }
    PifReleaseSem(husEtkSem);
    return(sError);
}

/*
*==========================================================================
**    Synopsis:    VOID    EtkCloseFile(SHORT hsEtkFile)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   NONE
*                
**  Description:
*                Close ETK file.
*==========================================================================
*/
VOID   EtkCloseFile(SHORT hsEtkFile)
{
    PifRequestSem(husEtkSem);       /* reqest semaphore */
    PifCloseFile(hsEtkFile);
    PifReleaseSem(husEtkSem);
}
/*
*==========================================================================
**    Synopsis:    SHORT    EtkReadFile(SHORT hsEtkFile
*                                       ULONG ulOffset
*                                       ULONG ulLen
*                                       UCHAR *puchReadData,
										ULONG *pulActualBytesRead)
*
*       Input:  SHORT hsEtkFile         -   Handle referring to open file.
*               ULONG ulOffset          -   Number of bytes from file header.
*               USHORT usLen            -   Number of bytes to be read.
*               UCHAR  *puchReadData    -   Storage location for data.
				ULONG *pulActualBytesRead - Number of bytes Actual Read
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   ETK_SUCCESS
*                 ETK_FILE_NOT_FOUND
*                
**  Description:
*                Read ETK file.
	RPH 11-7-3 Changes Made to Reflect PifReadFile
	EtkReadFile no longer returns the size read, instead it will be "returned"
	by pulActualBytesRead
*==========================================================================
*/
SHORT    EtkReadFile(SHORT hsEtkFile,
                     ULONG ulOffset,
                     ULONG ulLen,
                     UCHAR  *puchReadData,
					 ULONG	*pulActualBytesRead)
{
    ULONG   ulActualPosition;
	SHORT	sReturn = ETK_NO_MAKE_FILE;

    PifRequestSem(husEtkSem);       /* reqest semaphore */

    if (PifSeekFile(hsEtkFile, ulOffset, &ulActualPosition) < 0) {
        PifReleaseSem(husEtkSem);
    } else{
		//RPH 11-7-3 Changes for PifReadFile
		sReturn = PifReadFile(hsEtkFile, puchReadData, ulLen, pulActualBytesRead);
		
		PifReleaseSem(husEtkSem);
	}
    return sReturn;
}
/*
*==========================================================================
**    Synopsis:    VOID     EtkWriteFile(SHORT hsEtkFile
*                                        ULONG ulOffset
*                                        USHORT usLen
*                                        UCHAR  *puchWriteData)
*
*       Input:      SHORT hsEtkFile     -   Handle referring to open file.
*                   ULONG ulOffset      -   Number of bytes from file header.
*                   USHORT usLen        -   Number of bytes to be write.
*                   UCHAR  *puchWriteData - Pointer to data to be written.
*
**      Output:   Nothing
*       InOut:    Nothing
*
**  Return    :   ETK_SUCCESS
*                 ETK_FILE_NOT_FOUND
*                
**  Description:
*                Write ETK file.
*==========================================================================
*/
VOID    EtkWriteFile(SHORT hsEtkFile,
                              ULONG ulOffset,
                              ULONG usLen,
                              UCHAR  *puchWriteData)
{
    PifRequestSem(husEtkSem);       /* reqest semaphore */

    Etk_WriteFile(hsEtkFile, ulOffset, puchWriteData, usLen);

    PifReleaseSem(husEtkSem);
}
/*
*==========================================================================
**    Synopsis:   SHORT     EtkAllDataCheck(UCHAR uchMinor)
*
*       Input:    UCHAR     uchMinor     CLASS_TTLCURDAY
*                                        CLASS_TTLSAVDAY
*
**      Output:   Nothing
*       InOut:    Nothing
*
**  Return    :   ETK_SUCCESS           * Data is not in buffer in order to input uchMinor.
*                 ETK_FILE_NOT_FOUND    * ETK file is not exist 
*                 ETK_DATA_EXIST        * Data is in buffer in order to input uchMinor.
*                
**  Description:
*                Check Data exist in buffer in order to input uchMinor.
*==========================================================================
*/
SHORT    EtkAllDataCheck(UCHAR uchMinor)
{
    ETK_TOTALHEAD   EtkFileHead = {0};
    ETK_TOTALINDEX  aEtkIndex[50] = {0};  /* reduce memory size 4/9/96 */
/*    ETK_TOTALINDEX  aEtkIndex[FLEX_ETK_MAX]; */
    ULONG           ulIndex;
    USHORT          usNumberOfEmployee, cusI;
    SHORT           sStatus = ETK_SUCCESS;
    USHORT          usBytesToRead;
    USHORT          usNoOfEtkChecked;
    USHORT          usBytesChecked;
    USHORT          usMaxReadSize;

    PifRequestSem(husEtkSem);       /* reqest semaphore */

    /* File Open */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get Header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    if (uchMinor == CLASS_TTLCURDAY) {
        ulIndex = EtkFileHead.EtkCur.ulOffsetOfIndex;
        usNumberOfEmployee = EtkFileHead.EtkCur.usNumberOfEtk;
    } else {
        ulIndex = EtkFileHead.EtkSav.ulOffsetOfIndex;
        usNumberOfEmployee = EtkFileHead.EtkSav.usNumberOfEtk;
    }
/*** Reduce Memory Size (4/9/96) *****************************
    Etk_ReadFile(hsEtkTotalFile, ulIndex, aEtkIndex, (usNumberOfEmployee * sizeof(ETK_TOTALINDEX)));

    for (cusI = 0; cusI < usNumberOfEmployee; cusI++) {
        if ((aEtkIndex[cusI].uchStatus & ETK_MASK_FIELD_POINTER) != 0) {
            sStatus = ETK_DATA_EXIST;
            break;
        } else if ((aEtkIndex[cusI].uchStatus & ETK_TIME_IN) != 0) {
            sStatus = ETK_DATA_EXIST;
            break;
        } else {
            sStatus = ETK_SUCCESS;
            continue;
        }
    }
**** Reduce Memory Size (4/9/96) ****************************/

    usBytesToRead = sizeof(aEtkIndex);
    usNoOfEtkChecked = 0;
    usBytesChecked = 0;
    usMaxReadSize = sizeof(ETK_TOTALINDEX) * usNumberOfEmployee;

    while (( usNoOfEtkChecked < usNumberOfEmployee ) && ( sStatus != ETK_DATA_EXIST )) {

        if ( usMaxReadSize < (usBytesChecked + usBytesToRead )) {
            usBytesToRead = usMaxReadSize - usBytesChecked;
        }

        Etk_ReadFile(hsEtkTotalFile, ulIndex, aEtkIndex, usBytesToRead);

        /* --- determine etk data exists in etk index buffer --- */
        cusI = 0;
        while (( cusI < ( usBytesToRead / sizeof(ETK_TOTALINDEX))) &&
               ( sStatus != ETK_DATA_EXIST )) {
            if ((aEtkIndex[cusI].uchStatus & ETK_MASK_FIELD_POINTER) ||
                (aEtkIndex[cusI].uchStatus & ETK_TIME_IN)) {
                sStatus = ETK_DATA_EXIST;
            }
            cusI++;
        }

        ulIndex += ( ULONG )usBytesToRead;
        usNoOfEtkChecked += ( usBytesToRead / sizeof(ETK_TOTALINDEX));
        usBytesChecked += usBytesToRead;
    }

    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);          /* close file and release semaphore */
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   EtkCurIndWrite(ULONG    ulEmployeeNo,
*                                                  USHORT *pusFieldNo,
*                                                  ETK_FIELD  *pEtkField)
*
*       Input:    ULONG     ulEmployeeNo
*                 USHORT    *pusFieldNo    
*                 ETK_FIELD *pEtkField
*
*      Output:    
*       InOut:
*
**  Return    :
*            Normal End:    ETK_SUCCESS          Writed.
*          Abnormal End:    ETK_FILE_NOT_FOUND   file is not found.
*                           ETK_NOT_IN_FILE      This employee no
*                                                is not in file.
*                           ETK_NOT_IN_FIELD     Data is not on and after
*                                                this field.
*
**  Description:
*                pusFiledNo is setted field No in Record to be read.
*==========================================================================
*/
SHORT   EtkCurIndWrite(ULONG       ulEmployeeNo,
                                USHORT     *pusFieldNo,
                                ETK_FIELD  *pEtkField)
{                                  
    USHORT   usParaBlockNo;                    /* paramater block no save area */
    SHORT    sErrorStatus;                     /* error status save area */
    ETK_TOTALINDEX     EtkIndex = {0};         /* Etk Record to be read */
    ETK_TOTALINDEX     EtkIndexWork = {0};     /* Etk Record to be read */
    ETK_FIELD_IN_FILE  EtkFieldInFile = {0};   /* Field record in file */
    ETK_TOTALHEAD      EtkFileHead = {0};      /* file header save area */
    ULONG    ulIndexPos;                       /* Etk_Index() 's */

    PifRequestSem(husEtkSem);            /* request samaphore */

    /* open file */
    if (Etk_OpenIndFile(ETK_TOTAL_FILE) == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenIndFile() will release the semaphore for us
        return(ETK_FILE_NOT_FOUND);
    }                                            
    /* Check Input Value */
    if (*pusFieldNo > (ETK_MAX_FIELD - 1)) {
        Etk_CloseIndFileReleaseSem(hsEtkTotalFile);          /* close file and release semaphore */
        return(ETK_NOT_IN_FIELD);
    }

    /* Get file header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* Search inputed ETK no in Index table */
    if (Etk_Index(&EtkFileHead.EtkCur, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &usParaBlockNo, &ulIndexPos) == ETK_RESIDENT) {
        /* Write Paramater */
        EtkFieldInFile.usMonth =         pEtkField->usMonth;
        EtkFieldInFile.usDay   =         pEtkField->usDay;
        EtkFieldInFile.uchJobCode =      pEtkField->uchJobCode;
        EtkFieldInFile.usTimeinTime =    pEtkField->usTimeinTime;
        EtkFieldInFile.usTimeinMinute =  pEtkField->usTimeinMinute;
        EtkFieldInFile.usTimeOutTime =   pEtkField->usTimeOutTime;
        EtkFieldInFile.usTimeOutMinute = pEtkField->usTimeOutMinute;
        EtkIndexWork.uchStatus = (UCHAR)*pusFieldNo;
        Etk_TimeFieldWrite(&EtkFileHead.EtkCur, &EtkIndexWork, usParaBlockNo, &EtkFieldInFile);
        sErrorStatus = ETK_SUCCESS;
        if (((USHORT)(EtkIndex.uchStatus & ETK_MASK_FIELD_POINTER)) <= *pusFieldNo) {
            if (EtkFieldInFile.usTimeOutTime != ETK_TIME_NOT_IN) {
                EtkIndex.uchStatus = (UCHAR)(*pusFieldNo + 1);
            } else if ((EtkFieldInFile.usTimeinTime != ETK_TIME_NOT_IN) &&
                (EtkFieldInFile.usTimeOutTime == ETK_TIME_NOT_IN)) {
                EtkIndex.uchStatus = (UCHAR)*pusFieldNo;
                EtkIndex.uchStatus |= ETK_TIME_IN;
            } else {
                EtkIndex.uchStatus = (UCHAR)*pusFieldNo;
                EtkIndex.uchStatus |= ETK_TIME_IN;
            }

            /* Up-Date Index Record */
            Etk_WriteFile(hsEtkTotalFile, ulIndexPos, &EtkIndex, sizeof(ETK_TOTALINDEX));
        } 
    } else {
        sErrorStatus = ETK_NOT_IN_FILE;
    }

    Etk_CloseIndFileReleaseSem(hsEtkTotalFile);          /* close file and release semaphore */
    return(sErrorStatus);
}


SHORT  EtkStatCheckEmployeesLoggedIn (ULONG ulEmployeIdCheck, ULONG  *ulEmployeeNoList, int nMax)
{
	ETK_PARA_TOTAL  etkList[50];
	SHORT  sR, i;
	SHORT  sCount = 0;

    PifRequestSem(husEtkSem);   /* request employee time keeping data samaphore */

    /* open file */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            
	sR = Etk_GetParaTotalBlocks (etkList, 50);
	Etk_CloseAllFileReleaseSem();    /* close file and release employee time keeping semaphore */

	for (i = 0; i < sR; i++) {
		if (sCount < nMax && (etkList[i].uchStatus & ETK_TIME_IN)) {
			*ulEmployeeNoList = etkList[i].paraData.ulEmployeeNo;
			ulEmployeeNoList++; sCount++;
		}
	}

	return sCount;
}

SHORT  EtkStatCheckCreateFirstEmployee (ULONG  ulEmployeeNo)
{
	ULONG           etkList[50];
	SHORT           sRetStatus = 0;
	SHORT           sLookUpStatus;
	SHORT           sLoggedInCount = 0;
    ETK_PARAHEAD    EtkParaHead;
	TCHAR           aszEmployeeName[21] = {_T('X'), 0};

	sLoggedInCount = EtkStatCheckEmployeesLoggedIn (ulEmployeeNo, etkList, 50);

    PifRequestSem(husEtkSem);   /* request employee time keeping data samaphore */

    /* open file */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* Get File Header */
    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));

	// The following is to work around a field issue seen on some Amtrak trains in which
	// the initialization sequence which beings with the Start Log In screen in which
	// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
	// the Inventory app for other train trip startup activities does not complete properly.
	// The result is that the LSA is unable to complete the Start Log In screen task and
	// is stuck with it partially complete. A control string is used to automate this task
	// so as part of the sanity check as to whether to allow this workaround, check that a
	// control string is running.
	//
	// This change checks if the LSA has already done a Log-in and if so, just report success.
	//      Richard Chambers, Aug-17-2018, for Amtrak only
	if (EtkParaHead.usNumberOfEtk > 0 && sLoggedInCount > 1) {
		// more than one employee logged in so we should already have our LSA assigned.
		sRetStatus = 2;
	} else if (EtkParaHead.usNumberOfEtk > 0 && sLoggedInCount == 1) {
		// if there is only one employee logged in and it is this employee then we will
		// handle this as if the employee is doing a Log-in for the first time.
		// if this is an employee other than the one employee logged in then we should
		// already have our LSA assigned.
		if (etkList[0] == ulEmployeeNo) {
			sRetStatus = 0;
		} else {
			sRetStatus = 2;
		}
	}
	else {
		ETK_JOB  EtkJob = {0};
		// There are no employees in the Employee file
		// Per Amtrak we will create a new Employee who will have the
		// Supervisor Job Code.

		EtkJob.uchJobCode1 = MSR_ID_JOBCODE_SUP_MAX;              // First employee is a Supervisor in Amtrak land
		sLookUpStatus = UifReadAmtrakEmployeeFile (ulEmployeeNo, aszEmployeeName);
		sRetStatus = 0;
		if (sLookUpStatus < 0) {
			_tcscpy (aszEmployeeName, _T("Unknown"));
			sRetStatus = 1;
		}
		EtkAssignImplement (ulEmployeeNo, &EtkJob, aszEmployeeName);
	}
	
    Etk_CloseAllFileReleaseSem();    /* close file and release employee time keeping semaphore */

	if (sRetStatus == 0 || sRetStatus == 1) {
		CASIF   CasIf = {0};
		SHORT   sError;

		CasIf.ulCashierNo = ulEmployeeNo;
		sError = SerCasIndRead(&CasIf);
		if (sError == CAS_NOT_IN_FILE || sError == CAS_PERFORM) {
			// Setup a default Operator Record.  For Amtrak we will prohibit some Operator actions which the Supervisor can change.
			CasIf.ulCashierNo = ulEmployeeNo;
			CasIf.ulCashierSecret = ulEmployeeNo;
			CasIf.usSupervisorID = 899;         // Assign a Supervisor Id to this LSA who should have Supervisor authority
			CasIf.fbCashierStatus[0] &= ~CAS_OPERATOR_DENIED_AM;             // Ensure the operator can do a Sign-in
			CasIf.fbCashierStatus[2] |= CAS_PIN_REQIRED_FOR_TIMEIN;          // Amtrak requested this option to be turned on by default
//			CasIf.fbCashierStatus[2] |= CAS_PIN_REQIRED_FOR_SIGNIN;          // Amtrak requested this option to be turned off by default
//			CasIf.fbCashierStatus[3] |= CAS_PIN_REQIRED_FOR_SUPR;            // Amtrak requested this option to be turned off by default
//			CasIf.fbCashierStatus[CAS_CASHIERSTATUS_2] = (CAS_OPEN_CASH_DRAWER_SIGN_IN);  // Amtrak requested this option to be turned off by default, then changed their minds
			_tcscpy (CasIf.auchCashierName, aszEmployeeName);

			SerCasAssign(&CasIf);
		}
		else {
			NHPOS_ASSERT_TEXT(0, "==ERROR: EtkStatCheckCreateFirstEmployee() SerCasIndRead error.");
			sRetStatus = ETK_NOT_IN_JOB;
		}
	}
	return sRetStatus;
}

/*
*==========================================================================
**    Synopsis:    SHORT   EtkStatRead(ULONG   ulEmployeeNo,
*                                               SHORT  *psTimeIOStat,
*                                               USHORT *pusMaxFieldNo,
*                                               ETK_JOB  *pEtkJob)
*
*       Input:    ULONG     ulEmployeeNo        * Employee No *
*      Output:    SHORT     *psTimeIOStat       * Set Time in/out Status *
*                 USHORT    *pusMaxFieldNo      * Max Field No to be setted *
*                 ETK_JOB   *pEtkJob            * Allowance Job Code *
*
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*                           ETK_NOT_IN_FILE
*
**  Description:
*                Read all allowance Job Code.
*==========================================================================
*/
SHORT   EtkStatRead(ULONG  ulEmployeeNo,
                             SHORT  *psTimeIOStat,
                             USHORT *pusMaxFieldNo,
                             ETK_JOB  *pEtkJob)
{
    USHORT    usParaBlockNo;               /* paramater block entry save area */
    SHORT   sStatus;                       /* Save status */
    ETK_TOTALHEAD    EtkFileHead = {0};    /* file header save area */
    ETK_TOTALINDEX   EtkIndex = {0};       /* Index record save area */
    ETK_PARAINDEX    EtkParaIndex = {0};   /* Paramater index */
    ULONG   ulIndexPos;                    /* Etk_Index() 's */

    PifRequestSem(husEtkSem);   /* request samaphore */

    /* open file */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* check ETK assgin */
    if ((sStatus = Etk_IndexPara(ETK_SEARCH_ONLY, ulEmployeeNo, &EtkParaIndex, &ulIndexPos)) == ETK_NOT_IN_FILE) {
        Etk_CloseAllFileReleaseSem();                                /* close file and release semaphore */
        return(sStatus);
    }

    /* get file header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_TOTALHEAD));

    /* check ETK assgin */
    if ((sStatus = Etk_Index(&EtkFileHead.EtkCur, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &usParaBlockNo, &ulIndexPos)) == ETK_NOT_IN_FILE) {
        *psTimeIOStat = ETK_NOT_TIME_IN;
        *pEtkJob = EtkParaIndex.EtkJob;
        *pusMaxFieldNo = 0;
        Etk_CloseAllFileReleaseSem();                                /* close file and release semaphore */
        return(ETK_SUCCESS);        
    }

    /* Set Time in/Out Flag  */
    if (EtkIndex.uchStatus & ETK_TIME_IN) {
        *psTimeIOStat = ETK_NOT_TIME_OUT;
    } else {
        *psTimeIOStat = ETK_NOT_TIME_IN;
    }        

    /* Set Max Field No */
    *pusMaxFieldNo = EtkIndex.uchStatus & ETK_MASK_FIELD_POINTER;
    if (EtkIndex.uchStatus & ETK_TIME_IN) {
        (*pusMaxFieldNo)++;
    }

    /* Make Output Condition */
    *pEtkJob = EtkParaIndex.EtkJob;

    Etk_CloseAllFileReleaseSem();                                /* close file and release semaphore */
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT  EtkCreateTotalIndex(VOID)
*
*       Input:  Nothing 
*      Output:  Nothing  
*       InOut:  Nothing 
*
**  Return    :
*           SUCCESS
*           NOT_IN_FILE
*
**  Description:
*                Create Total File Index.
*==========================================================================
*/
SHORT  EtkCreateTotalIndex(VOID)
{
    ETK_PARAHEAD    EtkParaFileHead = {0};       /* file header */
    ETK_TOTALHEAD   EtkTotalFileHead = {0};     /* file header */
    ETK_PARAINDEX   EtkIndexEntry = {0};        /* index table entry */ 
    ETK_TOTALINDEX  EtkIndex = {0};
    USHORT  cusi;                                                
    USHORT          usParaBlockNo;
    ULONG           ulIndexPos;               /* Etk_Index() 's */

    PifRequestSem(husEtkSem);       /* request semaphore */

    /* open file */
    if (Etk_OpenAllFile() == ETK_FILE_NOT_FOUND) {
		// PifReleaseSem(husEtkSem);      not needed as function Etk_OpenAllFile() will release the semaphore for us if error
        return(ETK_FILE_NOT_FOUND);
    }                                            

    /* get para file header */
    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaFileHead, sizeof(ETK_PARAHEAD));

    /* get total file header */
    Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalFileHead, sizeof(ETK_TOTALHEAD));

    /* Count inputed ETK no in index table */
    for (cusi = 0 ;cusi < EtkParaFileHead.usNumberOfEtk; cusi++) {
        Etk_ReadFile(hsEtkParaFile, sizeof(ETK_PARAHEAD) + (cusi * sizeof(ETK_PARAINDEX)), &EtkIndexEntry, sizeof(ETK_PARAINDEX));
        Etk_Index(&EtkTotalFileHead.EtkCur, ETK_REGIST, EtkIndexEntry.ulEmployeeNo, &EtkIndex, &usParaBlockNo, &ulIndexPos);
    }

    /* Up-Date File Header */
    Etk_WriteFile(hsEtkTotalFile,  ETK_FILE_HED_POSITION, &EtkTotalFileHead, sizeof(ETK_TOTALHEAD));

    Etk_CloseAllFileReleaseSem();                                /* close file and release semaphore */
    return(ETK_SUCCESS);    
}
/*====== End of Source ======*/
