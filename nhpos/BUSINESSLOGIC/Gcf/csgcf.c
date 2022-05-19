/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server Guest Check module, Program List                        
*   Category           : Client/Server Guest Check module, NCR2170 US HOSPITALITY MODEL.
*   Program Name       : CSGCF.C
* --------------------------------------------------------------------------
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
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows
*
*           Gcf_ReqSemOpenFileGetHeadChkLck   * Request Semaphore Open GCfile And Get Header, Check All lock.
*           Gcf_CloseFileReleaseSem           * Close File and Release Semaphore 
*   #       GusCreateGCF            * Creat GCF file *
*           GusCheckAndCreateGCF    * Check and Create GCF file *
*           GusCheckAndDeleteGCF    * Check and delete GCF file *
*   #       GusManAssignNo          * Regist GCF No. *  
*   #       GusReadLock             * Read and Lock Guest Check *            
*   #       GusDelete               * Delete Guest Check Data *            
*   #       GusGetAutoNo            * Get auto guest check No *
*   #       GusBakAutoNo            * Backup Auto Guest Check No *
*           GusIndRead              * Read Guest Check Data *
*   #       GusAllIdRead            * Read all Guest No *      
*           GusAllLock              * Lock all Guest check no *            
*           GusAllUnLock            * Unlock all guest check no *            
*           GusAssignNo             * Assign Guest check no *            
*   #       GusResetReadFlag        * reset read flag *
*   #       GusCheckExist           * check Guest check data *
*   #       GusAllIdReadBW          * Read all guest check no by waiter *
*           GusInitialize           * initialize GCF module *
*           GusReadAllLockedGCN     * Read all locked GCN *
*           GusStoreFileFH          * Store Guest Check File(File Handle I/F Version) *
*   #       GusReadLockFH           * Read and Lock Guest Check(File Handle I/F Version) *            
*           GusIndReadFH            * Read Guest Check Data(File Handle I/F Version) *
*   #       GusReqBackUpFH          * Request Back up. (File Handle I/F Version)
*   #       GusResBackUpFH          * Response of Back Up.(File Handle I/F Version)
*   #       GusDeleteDeliveryQueue  * Delete GC from delivery queue
*   #       GusInformQueue          * Inform Queue
*   #       GusSearchQueue          * Search Queue
*   #       GusCheckAndReadFH       * Check and read
*   #       GusResetDeliveryQueue   * Reset delivery queue
*   # =  modified or added function at release 3.0
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   May-06-92:00.00.01:M.YAMAMOTO :Initial
*   Sep-26-92:00.00.05:H.YAMAGUCHI:Adds GusBakAutoNo
*   Oct-28-92:00.01.10:H.YAMAGUCHI:Adds GusCheckAndCreateGCF, GusCheckAndDeleteGCF
*   Nov-02-92:01.00.01:H.YAMAGUCHI:Adds GusReadAllLockedGCN
*   Jul-15-93:00.00.01:H.YAMAGUCHI:Modified GusReadLock
*   Nov-12-93:00.00.01:H.YAMAGUCHI:Modified GusCreateGCF "Prohibited Drive throu "
*   Mar-11-94:00.00.04:K.YOU      :add flex GC feature.(add GusStoreFileFH,
*            :        :           :GusReadLockFH, GusIndReadFH) 
*   Mar-31-94:00.00.04:K.YOU      :(add GusReqBackUpFH, GusResBackUpFH) 
*   Jan-12-95:        :hkato      :R3.0
*   Nov-29-95:03.01.00:T.Nakahata :R3.1 Initial (Mod. GusCheckAndDeleteGCF)
*   Aug-12-99:03.05.00:M.Teraki   :Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
*
** NCR2171 **
*   Aug-26-99:01.00.00:M.Teraki   :initial (for 2171)
*   Dec-27-99:01.00.00:hrkato     :Saratoga
** GenPOS
*   Aug-24-15:02.02.01:R.Chambers :source cleanup
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
#include    <malloc.h>
#include    <string.h>
#include	<stdlib.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>
#include    <regstrct.h>
#include	"pifmain.h"
#include    <transact.h>
#include    <storage.h>
#include    <csgcs.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csstbgcf.h>
#include    <csstbpar.h>
#include    "csgcsin.h"
#include    <appllog.h>
#include    <nb.h>
#include    <prtprty.h>
#include	<ConnEngineObjectIf.h>

static TCHAR CONST  auchGC_FILE[] = _T("TOTALGCF");        /* File name */

USHORT husGcfSem = 0;         /* Semaphore */
SHORT  hsGcfFile = -1;        /* File Handle */
SHORT  fsGcfDurAllLock = 0;   /* All Lock Flag, if 0 then not locked else is locked */

static VOID Gcf_IncrementGCNo (USHORT usSystemType, GCNUM *usGcn)
{
	(*usGcn)++;
    if (FLEX_STORE_RECALL == usSystemType) {
        if (*usGcn > GCF_MAX_DRIVE_NUMBER) {
            *usGcn = 1;
        }
    } else {
        if (*usGcn > GCF_MAX_CHK_NUMBER) {
            *usGcn = 1;
        }
    }
}

static  SHORT Gcf_FindEmptyBlockChain (GCF_FILEHED *pGcf_FileHed, GCF_INDEXFILE *pGcf_IndexFile, ULONG *pulHitPoint)
{
	SHORT sStatus, sDeleteTranDone = 0;

	sStatus = Gcf_BlockReserve(pGcf_FileHed, &(pGcf_IndexFile->offusReserveBlockNo));
	while (sStatus == GCF_FULL) {
		if (Gcf_DeletePaidTran(pGcf_FileHed, pGcf_IndexFile->usGCNO) == GCF_FULL) {
			NHPOS_ASSERT(!"Gcf_FindEmptyBlockChain(): GCF FULL ERROR, Full and could not free space up");
			return(GCF_FULL);
		}
		sDeleteTranDone = 1;
		sStatus = Gcf_BlockReserve(pGcf_FileHed, &(pGcf_IndexFile->offusReserveBlockNo));
	}
	if (sDeleteTranDone) {
		// Redo the search for the guest check number since we have had to
		// delete a paid transaction which has in turn moved the indexes around.
		GCF_INDEXFILE   Gcf_IndexFileTemp = *pGcf_IndexFile;

		sStatus = Gcf_Index(pGcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFileTemp, pulHitPoint);
	}
	return sStatus;
}

//Compare method for Read all guest checks.  Used in GusAllIdRead.
int compare (const void * a, const void * b)
{
  return ( *(USHORT*)a - *(USHORT*)b );
}

/*
*==========================================================================
**    Synopsis:   SHORT   Gcf_ReqSemOpenFileGetHeadChkLck(GCF_FILEHED *pGcf_FileHed, 
*                                                         UCHAR       uchOption)
*
*       Input:    UCHAR        uchOption        * GCF_LOCKCHECK     - Do All Lock Check 
*                                               * GCF_NOT_LOCKCHECK - Do Not All Lock Check
*                   
*      Output:    GCF_FILEHED *pGcf_FileHed     * File Header 
*       InOut:    Nothing
*
**  Return   :    GCF_SUCCESS
*                 GCF_ALL_LOCKED
*                 GCF_FILE_NOT_FOUND
*                 
**  Description: Open a GCF File and request semaphore.
*                             
*==========================================================================
*/
static SHORT   Gcf_ReqSemOpenFileGetHeadChkLck(GCF_FILEHED  *pGcf_FileHed, UCHAR uchOption)
{
    /* Request semaphore */
    PifRequestSem(husGcfSem);

    /* Check During All lock */
    if (uchOption == GCF_LOCKCHECK) {
        if (fsGcfDurAllLock) { /* fsGcfDurAllLock <> 0 */
            PifReleaseSem(husGcfSem);
            return(GCF_ALL_LOCKED);
        }
    }

    /* File Open */
    if ((hsGcfFile = PifOpenFile(auchGC_FILE, auchOLD_FILE_READ_WRITE)) < 0) {
        PifReleaseSem(husGcfSem);
        PifLog(MODULE_GCF, FAULT_ERROR_FILE_OPEN);
        PifLog(MODULE_ERROR_NO(MODULE_GCF), (USHORT)abs(hsGcfFile));
        return(GCF_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Gcf_ReadFile(GCF_FILE_HEAD_POSITION, pGcf_FileHed, sizeof(GCF_FILEHED), GCF_SEEK_READ);
    return(GCF_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:   VOID Gcf_CloseFileReleaseSem(VOID)
*
*       Input:    Nothing
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*                 
*
**  Description:
*               Close file and Release semaphore.              
*==========================================================================
*/
static VOID    Gcf_CloseFileReleaseSem(VOID)
{
    PifCloseFile(hsGcfFile);
	hsGcfFile = -1;
    PifReleaseSem(husGcfSem);
}

/*
*==========================================================================
**    Synopsis:   VOID Gcf_ReadFile(ULONG   offulSeekPos, 
*                                   VOID    *pReadBuffer, 
*                                   ULONG  ulReadSize, 
*                                   UCHAR   uchMode)
*
*       Input:    ULONG     offulSeekPos   Seek value from file head
*                 USHORT    usReadSize     Read buffer size 
*                 UCHAR     uchMode        GCF_SEEK_READ Seek and Read
*                                          GCF_CONT_READ  Read not Seek
*                                          GCF_SEEK_ONRY  Seek Only
*                 
*      Output:    VOID      *pReadBuffer   Read buffer
*       InOut:    Nothing
*
**  Return   :    SHORT     status of the read request completion.
*                 GCF_SUCCESS   - either data read, GCF_SEEK_READ, or seek made, GCF_SEEK_ONRY.
*                 GCF_FATAL     - fatal error such as seek did not complete or insufficent number of bytes read
*                 GCF_NO_READ_SIZE - read request but zero number of bytes or NULL buffer pointer.
*
**  Description:
*               Read data from the Guest Check File at the specified location.  This is
*               a general purpose read function used for any read to the
*               Guest Check File so an address to the buffer to receive the data read
*               from the file along with the number of bytes is specified.
*==========================================================================
*/
SHORT   Gcf_ReadFile(ULONG      offulSeekPos,
                     VOID       *pReadBuffer, 
                     ULONG      ulReadSize,
                     UCHAR      uchMode)
{
    ULONG   ulActualPosition;
    int     iStatus = 0;
	ULONG	ulActualBytesRead; //11-10-3 RPH

    if (uchMode != GCF_CONT_READ) {
        iStatus = PifSeekFile(hsGcfFile, offulSeekPos, &ulActualPosition);
        if (iStatus < 0) {
            PifLog(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
			PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
            PifLog(MODULE_ERROR_NO(MODULE_GCF), (USHORT)abs(iStatus));
            PifAbort(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
			return(GCF_FATAL);
        }
    }

    if (uchMode == GCF_SEEK_ONLY) {
        return(GCF_SUCCESS);
    }
    if (0 == ulReadSize || pReadBuffer == NULL) {    /* PifReadFile() is occurred system-error by read-size = 0 */
        return(GCF_NO_READ_SIZE);
    }

	//11-10-3 RPH Changes For PifReadFile
	PifReadFile(hsGcfFile, pReadBuffer, ulReadSize, &ulActualBytesRead);
    if ( ulActualBytesRead != ulReadSize) {
        PifLog(MODULE_GCF, FAULT_ERROR_FILE_READ);
		PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
        PifAbort(MODULE_GCF, FAULT_ERROR_FILE_READ);
		return(GCF_FATAL);
    }
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   VOID Gcf_WriteFile(ULONG  offulSeekPos, 
*                                    VOID   *pWriteBuffer, 
*                                    USHORT usWriteSize)
*
*       Input:    ULONG     offulSeekPos   Seek value from file head for writing.
*                 VOID      *pWriteBuffer  Write buffer
*                 USHORT    usWriteSize    Write buffer size 
*                 
*      Output:    Nothing 
*       InOut:    Nothing
*
**  Return   :    SHORT     status of the write request completion.
*                 GCF_SUCCESS   - either data written or seek made, usWriteSize == 0.
*                 GCF_FATAL     - fatal error such as seek did not complete
*
**  Description:
*               Write data to Guest Check File at the specified location.  This is
*               a general purpose write function used for any write to the
*               Guest Check File so an address to the buffer to copy to the file
*               along with the number of bytes is specified.
*==========================================================================
*/
SHORT Gcf_WriteFile(ULONG offulSeekPos, VOID *pWriteBuffer, USHORT usWriteSize)
{
	int     iStatus = 0;
    ULONG   ulActualPosition;           /* for seek */

    iStatus = PifSeekFile(hsGcfFile, offulSeekPos, &ulActualPosition);
    if (iStatus < 0) {
        PifLog(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
        PifLog(MODULE_ERROR_NO(MODULE_GCF), (USHORT)abs(iStatus));
        PifAbort(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
		return(GCF_FATAL);
    }
    if (0 == usWriteSize || pWriteBuffer == NULL) {      /* PifWriteFile() is occurred system-error by read-size = 0 */
        return(GCF_SUCCESS);
    }
    PifWriteFile(hsGcfFile, pWriteBuffer, usWriteSize);
    return(GCF_SUCCESS);
}


/*
*==========================================================================
**    Synopsis:   SHORT GusCreateGCF (GCNUM usRecordNumber, UCHAR uchType)
*
*       Input:    GCNUM usRecordNumber
*                 UCHAR uchType  
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    GCF_SUCCESS
*                 GCF_NOT_CREATE
*                 GCF_ALL_LOCKED
*
**  Description:
*               Create Guest Check File.              
*==========================================================================
*/
SHORT GusCreateGCF (GCNUM usRecordNumber, UCHAR uchType)
{
	GCF_FILEHED     Gcf_FileHed = { 0 };
    ULONG           ulActualPosition;
    USHORT          usNoBlock;
	ULONG			ulRecordNumber = usRecordNumber;

    /* Request semaphore */
    PifRequestSem(husGcfSem);

    /* Delete OLD GCF */
    PifDeleteFile(auchGC_FILE);

    /* Check Input Data */
    if (0 == usRecordNumber) {
        PifReleaseSem(husGcfSem);
        return(GCF_SUCCESS);
    }

    if (FLEX_STORE_RECALL == uchType) {
        if ( usRecordNumber > GCF_MAX_DRIVE_NUMBER ) {      
            PifReleaseSem(husGcfSem);
			NHPOS_ASSERT(usRecordNumber <= GCF_MAX_DRIVE_NUMBER);
            return(GCF_DATA_ERROR);
        }
    } else {
        if ( usRecordNumber > GCF_MAX_GCF_NUMBER ) {
            PifReleaseSem(husGcfSem);
			NHPOS_ASSERT(usRecordNumber <= GCF_MAX_GCF_NUMBER);
			return(GCF_DATA_ERROR);
        }
    }

    /* Open NEW GCF */
    if ((hsGcfFile = PifOpenFile(auchGC_FILE, auchNEW_FILE_WRITE)) < 0x0000) {
		char  xBuff[128];
		sprintf (xBuff, "**ERROR: Guest Check File Open failed %d.", hsGcfFile);
		NHPOS_ASSERT_TEXT((hsGcfFile >= 0), xBuff);
        PifLog(MODULE_GCF, FAULT_ERROR_FILE_OPEN);
        PifLog(MODULE_ERROR_NO(MODULE_GCF), (USHORT)abs(hsGcfFile));
        PifAbort(MODULE_GCF, FAULT_ERROR_FILE_OPEN);
    }

    /* Make File Header */
    Gcf_FileHed.usSystemtype    = uchType;          // Remember the system type, FLEX_aaaaa
    Gcf_FileHed.usStartGCN      = 1;
    Gcf_FileHed.usMaxGCN        = usRecordNumber;
    Gcf_FileHed.usCurGCN        = 0;
    Gcf_FileHed.usCurNoPayGCN   = 0;                
    Gcf_FileHed.usCurDrive[0]   = 0;
    Gcf_FileHed.usCurDrive[1]   = 0;                
    Gcf_FileHed.usCurDrive[2]   = 0;                
    Gcf_FileHed.usCurDrive[3]   = 0;                
    Gcf_FileHed.usCurPayGC      = 0;                
    Gcf_FileHed.offusFreeGC     = 1;
    Gcf_FileHed.offulGCIndFile  = sizeof(GCF_FILEHED);
    Gcf_FileHed.offulGCDataFile = Gcf_FileHed.offulGCIndFile + (ulRecordNumber * sizeof(GCF_INDEXFILE));

    Gcf_GetNumberofBlockTypical(Gcf_FileHed.usSystemtype, &usNoBlock);

    if (FLEX_STORE_RECALL == Gcf_FileHed.usSystemtype) {
		// Store Recall system has Drive Thu queues so allocate the room for them as well.
        Gcf_FileHed.offulDrvNOFile[0] = Gcf_FileHed.offulGCDataFile + (GCF_DATA_BLOCK_SIZE * ulRecordNumber * usNoBlock );
        Gcf_FileHed.offulDrvNOFile[1] = Gcf_FileHed.offulDrvNOFile[0] + (ulRecordNumber * sizeof(GCNUM) );
        Gcf_FileHed.offulDrvNOFile[2] = Gcf_FileHed.offulDrvNOFile[1] + (ulRecordNumber * sizeof(GCNUM) );
        Gcf_FileHed.offulDrvNOFile[3] = Gcf_FileHed.offulDrvNOFile[2] + (ulRecordNumber * sizeof(GCNUM) );
        Gcf_FileHed.offulPayTranNO    = Gcf_FileHed.offulDrvNOFile[3] + (ulRecordNumber * sizeof(GCNUM) );
        Gcf_FileHed.ulGCFSize         = Gcf_FileHed.offulPayTranNO + (ulRecordNumber * sizeof(GCNUM) );
    } else {
		// No Drive Thru queues for other system types.
        Gcf_FileHed.offulDrvNOFile[0] = 0L;
        Gcf_FileHed.offulDrvNOFile[1] = 0L;               
        Gcf_FileHed.offulDrvNOFile[2] = 0L;              
        Gcf_FileHed.offulDrvNOFile[3] = 0L;              
        Gcf_FileHed.offulPayTranNO    = 0L;              
        Gcf_FileHed.ulGCFSize         = Gcf_FileHed.offulGCDataFile + (GCF_DATA_BLOCK_SIZE * ulRecordNumber * usNoBlock);
    }

    /* Check File size and Creat file */
    if (PifSeekFile(hsGcfFile, Gcf_FileHed.ulGCFSize, &ulActualPosition) < 0) {
        PifCloseFile(hsGcfFile);
        PifDeleteFile(auchGC_FILE);
        PifReleaseSem(husGcfSem);
        return(GCF_NOT_CREATE);
    }
    
    /* Creat Free Que */
	//Create empty ques for the maximum number of checks
    Gcf_CreatAllFreeQue(Gcf_FileHed.offulGCDataFile, (USHORT)(usRecordNumber * usNoBlock));

    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));
    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusCheckAndCreateGCF (USHORT usRecordNumber, UCHAR uchType)
*
*       Input:    USHORT usRecordNumber
*                 UCHAR uchType  
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    GCF_SUCCESS
*                 GCF_NOT_CREATE
*
**  Description:
*               Chech in not exist file, then Create Guest Check File.              
*==========================================================================
*/
SHORT GusCheckAndCreateGCF (GCNUM usRecordNumber, UCHAR uchType)
{
    /* Attempt to Open Guest Check File and create if unable to do so */
    if ((hsGcfFile = PifOpenFile(auchGC_FILE, auchOLD_FILE_READ_WRITE)) < 0) {
		SHORT   sStatus = GusCreateGCF(usRecordNumber, uchType);
        return(sStatus);
    }

    PifCloseFile(hsGcfFile);
	hsGcfFile = -1;
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusCheckAndDeleteGCF (USHORT usRecordNumber,
*                                             UCHAR uchType, BOOL *pfDeleted)
*
*       Input:    USHORT usRecordNumber
*                 UCHAR uchType
*                 BOOL  fDeleted    -   Deleted Flag, Add Rel 3.1
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    GCF_SUCCESS
*
**  Description:
*               Chech GCF size, if not equal , then delete GCF.              
*==========================================================================
*/
SHORT GusCheckAndDeleteGCF (GCNUM usRecordNumber, UCHAR uchType, BOOL *pfDeleted)
{
    SHORT           sStatus;
    GCF_FILEHED     Gcf_FileHed;

    *pfDeleted = TRUE;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);

    if (sStatus == GCF_SUCCESS)
	{
		Gcf_CloseFileReleaseSem();
		if ( Gcf_FileHed.usSystemtype == uchType )
		{
			if ( Gcf_FileHed.usMaxGCN == usRecordNumber )
			{
				*pfDeleted = FALSE;
				return(GCF_SUCCESS);
			}
		}
		PifDeleteFile(auchGC_FILE);
		PifLog(MODULE_GCF, LOG_EVENT_GCF_DELETE_FILE);
    }
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusManAssignNo (USHORT usGCNumber)
*
*       Input:    USHORT usGCNumber     Guest Check Number
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_EXIST
*                               GCF_ALL_LOCKED
*                               GCF_FILE_NOT_FOUND
*                               GCF_FILE_FULL
*
**  Description:
*               Manual Assign Guest Check No. (Regist GCNumber in GCF.)
*==========================================================================
*/
SHORT GusManAssignNo (GCNUM usGCNumber)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    ULONG           ulHitPoint;
    SHORT           sStatus;

    /* Check guest Check No */
    if (0 == usGCNumber) {   /* usGCNumber = 0 */
        return(GCF_NOT_IN);
    }

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);

    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }
    
	NHPOS_ASSERT(Gcf_FileHed.usMaxGCN > Gcf_FileHed.usCurNoPayGCN);
	NHPOS_ASSERT(Gcf_FileHed.usMaxGCN > Gcf_FileHed.usCurDrive[GCF_DELIVERY_1]);
	NHPOS_ASSERT(Gcf_FileHed.usMaxGCN > Gcf_FileHed.usCurDrive[GCF_DELIVERY_2]);

    /* Check GC Number is Full? */    
    if  ((Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurNoPayGCN) ||
         (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_1]) ||
         (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_2])) {
		//there are no more empty slots to enter any guest checks
        Gcf_CloseFileReleaseSem();
		NHPOS_ASSERT(Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurNoPayGCN);
		NHPOS_ASSERT(Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_1]);
		NHPOS_ASSERT(Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_2]);
		NHPOS_ASSERT(!"GCF FULL ERROR, no empty que!");
        return(GCF_FULL);
    }

	if  (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurGCN) {
		//Look into the GC File and find a transaction that has been payed out,
		//and make room for a new guest check
        if (Gcf_DeletePaidTran(&Gcf_FileHed, usGCNumber) == GCF_FULL) {
            Gcf_CloseFileReleaseSem();
			NHPOS_ASSERT(!"GCF FULL ERROR, Deleting a Paid Transaction Failed to free up space");
			return(GCF_FULL);
        }
    }

    /* Search Index Table And Assign by GC Number */
    
    Gcf_IndexFile.usGCNO       = usGCNumber;
    Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;  /* Set Read Flag */
    Gcf_IndexFile.offusBlockNo = 0;
    Gcf_IndexFile.offusReserveBlockNo = 0;
    
    sStatus = Gcf_Index(&Gcf_FileHed, GCF_REGIST, &Gcf_IndexFile, &ulHitPoint);

	if (sStatus != GCF_FULL) {
		if (sStatus == GCF_SUCCESS) {
			//Look through the index and see if the guest check has been 
			//created yet, if so

			if (Gcf_FileHed.usSystemtype != FLEX_STORE_RECALL) {
				//we check if we are in fact using the drive through system
				Gcf_CloseFileReleaseSem();
				return(GCF_EXIST);
			}
			//If the Guest Check number exists, but has not be paid out yet, 
			//return error
			if (!(Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) ||
				(Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) ||
				(Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK)) {
				Gcf_CloseFileReleaseSem();
				return(GCF_EXIST);
			}
			//else we free the que 
			Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
			Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusBlockNo);
			Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;  /* Set Read Flag */
			Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));

			if (Gcf_FileHed.offulPayTranNO != 0L) {
				ULONG   ulDriveIndexSerHitPoint;

				sStatus = Gcf_DriveIndexSer (Gcf_FileHed.usCurPayGC, usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulPayTranNO);
				if (sStatus == GCF_SUCCESS) {
					//look through the file for the previous GC number and cleare it in the table
					Gcf_DriveIndexDeleteStd (&Gcf_FileHed, ulDriveIndexSerHitPoint);
				}
			}
		}

		Gcf_ReadFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ);

		sStatus = Gcf_FindEmptyBlockChain (&Gcf_FileHed, &Gcf_IndexFile, &ulHitPoint);
		if (sStatus == GCF_FULL) {
			Gcf_WriteFile (GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));
			Gcf_CloseFileReleaseSem();
			NHPOS_ASSERT(!"GCF FULL ERROR, Full and could not free space up");
			return sStatus;
		}

		Gcf_FileHed.usCurNoPayGCN++;    /* Current Non Pay GC Number + 1 */
    
		Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));

		/* Write File Header */
    
		Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

		sStatus = GCF_SUCCESS;
	}

    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusReadLock  (USHORT usGCNumber,
*                                     UCHAR  *pusRcvBuffer,
*                                     USHORT usSize,
*                                     USHORT usType);
*
*       Input:    USHORT usGCNumber         guest check no.
*                 USHORT usSize             size of pusRcvBuffer 
*                 USHORT usType             0:By Reg  0<>:Sup
*
*      Output:    UCHAR  *pusRcvBuffer      save guest check data 
*       InOut:    Nothing
*
**  Return   :    Normal End  : number of deta length
*                 Abnormal End: GCF_NOT_IN
*                               GCF_LOCKED
*                               GCF_ALL_LOCKED
*                               GCF_FULL
*
**  Description:
*               Read and lock Guest check file.              
*==========================================================================
*/
SHORT GusReadLock  (GCNUM  usGCNumber,
                    UCHAR   *pusRcvBuffer,
                    USHORT  usSize,
                    USHORT  usType)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    ULONG           offulIndexPoint;
    USHORT          usAssignBlock, usCopyBlock;
    SHORT           sStatus, sLen, i, sMaster;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);

    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    sMaster = CliCheckAsMaster();               /* Check Master */
    /* Check DriveThrough Type */
    
    if (0 == usGCNumber) {  /* GC number == 0 DRIVE THROUGH TYPE */
        if (usType < GCF_PAYMENT_QUEUE1) {          
            Gcf_CloseFileReleaseSem();
            return(GCF_NOT_IN);
        }

		NHPOS_ASSERT(usType >= GCF_PAYMENT_QUEUE1);

        if (0 == Gcf_FileHed.usCurDrive[usType - GCF_PAYMENT_QUEUE1]) {
            Gcf_CloseFileReleaseSem();
            return(GCF_NOT_IN);
        }
        Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[usType - GCF_PAYMENT_QUEUE1], &usGCNumber, sizeof(GCNUM), GCF_SEEK_READ);
    }

    Gcf_IndexFile.usGCNO = usGCNumber;
    
    /* Search GCNO in Index */
    
    if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }
    /* Check During Read */
    
    if ((Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) != 0) {
        Gcf_CloseFileReleaseSem();
        return(GCF_LOCKED);
    }

    Gcf_IndexFile.fbContFlag |= GCF_READ_FLAG_MASK;
    
    Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
    
	//copy the specified blocks to the buffer, so that it can be read
    sLen = Gcf_DataBlockCopy(&Gcf_FileHed, Gcf_IndexFile.offusBlockNo, pusRcvBuffer, usSize, &usCopyBlock);

    if ( GCF_SUPER != usType ) {      /* if user is Sup, then return  */
		Gcf_GetNumberofBlockAssign(Gcf_FileHed.usSystemtype, &usAssignBlock);
		NHPOS_ASSERT(usAssignBlock >= usCopyBlock);

		{
			SHORT sDeleteTranDone = 0;
			GCF_INDEXFILE   Gcf_IndexFileTemp = Gcf_IndexFile;

			sStatus = Gcf_FreeQueTake(&Gcf_FileHed, (USHORT)(usAssignBlock - usCopyBlock), &Gcf_IndexFile.offusReserveBlockNo);
			if (sStatus == GCF_FULL) {
				Gcf_IndexFile.fbContFlag &= ~GCF_READ_FLAG_MASK;      /* Read Flag clear */
				Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
				while (sStatus == GCF_FULL) {
					if (Gcf_DeletePaidTran(&Gcf_FileHed, usGCNumber) == GCF_FULL) {
						/* avoid gcf full at add check, 05/07/01 */
						if ((FLEX_PRECHK_BUFFER == Gcf_FileHed.usSystemtype) || (FLEX_PRECHK_UNBUFFER == Gcf_FileHed.usSystemtype)) {
							break;
						}
						NHPOS_ASSERT(!"GCF FULL ERROR, could not free the que for a new GC");
						sLen = GCF_FULL;
						break;
					}
					sDeleteTranDone = 1;
					sStatus = Gcf_BlockReserve(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
				}

				if (sDeleteTranDone) {
					// Redo the search for the guest check number in case we have had to
					// delete a paid transaction which has in turn moved the indexes around.
					sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFileTemp, &offulIndexPoint);
				}
			}
		}

		Gcf_IndexFile.fbContFlag |= GCF_READ_FLAG_MASK;
		Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));

		if ((sLen >= 0) && (usType >= GCF_PAYMENT_QUEUE1)) {
			ULONG  ulDriveIndexSerHitPoint;

			NHPOS_ASSERT(usType >= GCF_PAYMENT_QUEUE1);

			if (Gcf_DriveIndexSer(Gcf_FileHed.usCurDrive[usType - GCF_PAYMENT_QUEUE1], 
								  usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulDrvNOFile[usType - GCF_PAYMENT_QUEUE1])
								  == GCF_SUCCESS) {
				Gcf_DriveIndexDeleteSpl (&Gcf_FileHed, (USHORT)(usType - GCF_PAYMENT_QUEUE1), ulDriveIndexSerHitPoint);
			}
		}
		if ((sLen >= 0) && (usType == GCF_REORDER)) { 
			ULONG  ulDriveIndexSerHitPoint;

			for (i = 0; i < 2; i++) {
				//look into the index and see if the guest check exists
				if (Gcf_DriveIndexSer(Gcf_FileHed.usCurDrive[i], usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulDrvNOFile[i]) == GCF_SUCCESS) {

					Gcf_DriveIndexDeleteSpl (&Gcf_FileHed, i, ulDriveIndexSerHitPoint);
					if (sMaster == STUB_SUCCESS) {
						NbWriteMessage(1, (UCHAR)(NB_REQPAYMENT1 << i));
					}
				}
			}
		}

		/* Write File Header */
    
		Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));
    
		if (sMaster == STUB_SUCCESS) {
			switch (usType) {
			case GCF_PAYMENT_QUEUE1:
				NbWriteMessage(1, NB_REQPAYMENT1);
				break;

			case GCF_PAYMENT_QUEUE2:
				NbWriteMessage(1, NB_REQPAYMENT2);
				break;
			}
		}
    }

    Gcf_CloseFileReleaseSem();
    return(sLen);
}
/*
*==========================================================================
**    Synopsis:   SHORT GusDelete(USHORT usGCNumber)
*                                     
*       Input:    USHORT usGCNumber     guest check no.
*
*      Output:    Nothing 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*                               GCF_FILE_NOT_FOUND
*
**  Description:
*               Delete a GCNumber and Data of GCF.
*==========================================================================
*/
SHORT GusDelete(GCNUM usGCNumber)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    SHORT           sStatus , i;
    ULONG           ulDriveIndexSerHitPoint;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);

    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    if (0 == usGCNumber) {  /* GC number == 0 */
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    Gcf_IndexFile.usGCNO = usGCNumber;

    if (Gcf_IndexDel(&Gcf_FileHed, &Gcf_IndexFile) == GCF_NOT_IN) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

	//Free the que and return the ques to be allowed for use with new GC
    Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
    Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusBlockNo);

	// If this guest check is also on a drive thru queue, then we need
	// to find it and remove it from the queue.
//    if ((Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK) != 0) {
        for ( i = 0; i < GCF_MAX_QUEUE; i++) {
            if (Gcf_DriveIndexSer(Gcf_FileHed.usCurDrive[i], usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulDrvNOFile[i]) == GCF_SUCCESS) {
							//look into the index and see if the guest check exists
				Gcf_DriveIndexDeleteSpl (&Gcf_FileHed, i, ulDriveIndexSerHitPoint);
            }
        }
//    }

	// If the guest check being deleted is not paid, then decrement
	// the count of the number of unpaid checks outstanding.
    if ((Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) == 0) {
		NHPOS_ASSERT(Gcf_FileHed.usCurNoPayGCN > 0);
		if (Gcf_FileHed.usCurNoPayGCN > 0) {
			Gcf_FileHed.usCurNoPayGCN--;
		}
    } else {
        if (Gcf_FileHed.offulPayTranNO != 0L) {
            if (Gcf_DriveIndexSer(Gcf_FileHed.usCurPayGC, usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulPayTranNO) == GCF_SUCCESS) {
				//look into the index and see if the guest check exists
				Gcf_DriveIndexDeleteStd (&Gcf_FileHed, ulDriveIndexSerHitPoint);
            }
        }
    }

    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));
    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:   SHORT GusGetAutoNo(USHORT *pusGCNumber);
*                                     
*
*       Input:    Nothing
*
*      Output:    USHORT    pusGCNumber         Guest Check No. 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_FILE_FULL
*                               GCF_ALL_LOCKED
*                               GCF_FULL
*
**  Description:
*               Get Auto Guest Check No.
*==========================================================================
*/
SHORT GusGetAutoNo(GCNUM *pusGCNumber)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    USHORT          usFirstStartGCN, usFlag = 1;
    ULONG           ulPoint;
    SHORT           sStatus, sRetStatus = GCF_SUCCESS;

	*pusGCNumber = 0;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);

    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    /* Check GC Number is Full? */
	NHPOS_ASSERT(Gcf_FileHed.usMaxGCN > Gcf_FileHed.usCurNoPayGCN);
	NHPOS_ASSERT(Gcf_FileHed.usMaxGCN > Gcf_FileHed.usCurDrive[GCF_DELIVERY_1]);
	NHPOS_ASSERT(Gcf_FileHed.usMaxGCN > Gcf_FileHed.usCurDrive[GCF_DELIVERY_2]);
    
    if  ((Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurNoPayGCN) ||
         (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_1]) ||
         (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_2])) {
			//we have all the guestchecks full
        Gcf_CloseFileReleaseSem();
		NHPOS_ASSERT(!"GCF FULL ERROR, usMaxGCN is full, cannot assign new one");
        PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_06);
        return(GCF_FULL);
    }

    if  (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurGCN) {
		// if we have reached the full number of Guest checks, 
		// try and delete one so that we can continue
        if (Gcf_DeletePaidTran(&Gcf_FileHed, Gcf_FileHed.usStartGCN) == GCF_FULL) {
            Gcf_CloseFileReleaseSem();
            PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_01);
            return(GCF_FULL);
        }
    }

    Gcf_IndexFile.usGCNO = usFirstStartGCN = Gcf_FileHed.usStartGCN;
	sRetStatus = GCF_FULL;

    do {
        Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;
        Gcf_IndexFile.offusBlockNo = 0;
        Gcf_IndexFile.offusReserveBlockNo = 0;

        sStatus = Gcf_Index(&Gcf_FileHed, GCF_REGIST, &Gcf_IndexFile, &ulPoint);

		if (sStatus == GCF_FULL) {
            PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_02);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_FileHed.usStartGCN);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_IndexFile.usGCNO);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_FileHed.usMaxGCN);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_FileHed.usCurGCN);
            PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
			sRetStatus = GCF_FULL;
			break;
		}

        if (sStatus == GCF_SUCCESS) {
            if (FLEX_STORE_RECALL == Gcf_FileHed.usSystemtype) {
				ULONG   ulDriveIndexSerHitPoint;

                if ((Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) &&
                    !(Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) &&
                    !(Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK)) {

					// If the Guest Check does exist, and has been paid already
					// we will return the blocks of space and then treat this as
					// if Gcf_Index() has inserted it as an empty index.
                    Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
                    Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusBlockNo);
					//return the used que to a free que
                    if (Gcf_FileHed.offulPayTranNO != 0L) {
                        if (Gcf_DriveIndexSer(Gcf_FileHed.usCurPayGC, Gcf_IndexFile.usGCNO, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulPayTranNO) == GCF_SUCCESS) {
							Gcf_DriveIndexDeleteStd (&Gcf_FileHed, ulDriveIndexSerHitPoint);
                        }
                    }
					Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;
					Gcf_IndexFile.offusBlockNo = 0;
					Gcf_IndexFile.offusReserveBlockNo = 0;
					Gcf_WriteFile(ulPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
					sStatus = GCF_NOT_IN;
                } 
            }
		}

        if (sStatus == GCF_NOT_IN) { 
			//if the guest check number has not yet been used, then we will
			//have to create it
			sStatus = Gcf_FindEmptyBlockChain (&Gcf_FileHed, &Gcf_IndexFile, &ulPoint);
			if (sStatus == GCF_FULL) {
				// We need to delete the index which has been created otherwise we will
				// have a locked but empty guest check sitting in the guest check file.
				if (Gcf_FileHed.usCurGCN > 0) {
					Gcf_FileHed.usCurGCN--;
					Gcf_DeleteTableinFile(Gcf_FileHed.offulGCIndFile, Gcf_FileHed.usCurGCN, ulPoint, sizeof(GCF_INDEXFILE));
					Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));
				}
				Gcf_CloseFileReleaseSem();
				PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_03);
				return sStatus;
			}
    
			Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;  // ensure that the read flag is set.
            Gcf_WriteFile(ulPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));

            Gcf_FileHed.usCurNoPayGCN++;    /* Increment count of Non Paid Guest Checks */

            *pusGCNumber = Gcf_FileHed.usStartGCN = Gcf_IndexFile.usGCNO;
            Gcf_IncrementGCNo (Gcf_FileHed.usSystemtype, &(Gcf_FileHed.usStartGCN));

			sRetStatus = GCF_SUCCESS;
            break;
        }
        Gcf_IncrementGCNo (Gcf_FileHed.usSystemtype, &(Gcf_IndexFile.usGCNO));
    }  while (usFirstStartGCN != Gcf_IndexFile.usGCNO);

	Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));

    Gcf_CloseFileReleaseSem();

	if ((usFirstStartGCN == Gcf_IndexFile.usGCNO) && (sStatus == GCF_FULL)) {
        PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_04);
        PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_FileHed.usStartGCN);
        PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_FileHed.usMaxGCN);
        PifLog(MODULE_DATA_VALUE(MODULE_GCF), Gcf_FileHed.usCurGCN);
        PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
	}

    return(sRetStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusBakAutoNo(USHORT usGCNumber);
*                                     
*
*       Input:    Nothing
*
*      Output:    USHORT    usGCNumber         Guest Check No. 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_FILE_FULL
*                               GCF_ALL_LOCKED
*                               GCF_FULL
*
**  Description:
*               Get Auto Guest Check No.
*==========================================================================
*/
SHORT GusBakAutoNo(GCNUM usGCNumber)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    ULONG           ulHitPoint;
    SHORT           sStatus;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    /* Check GC Number is Full? */
    if  ((Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurNoPayGCN) ||
         (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_1]) ||
         (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurDrive[GCF_DELIVERY_2])) {
        Gcf_CloseFileReleaseSem();
		PifLog(MODULE_GCF, LOG_ERROR_GCF_BAK_FULL_ERR_01);
        return(GCF_FULL);
    }

    if  (Gcf_FileHed.usMaxGCN <= Gcf_FileHed.usCurGCN) {
		//if we have reached the full number of Guest checks, 
		//try and delete one so that we can continue
        if (Gcf_DeletePaidTran(&Gcf_FileHed, usGCNumber) == GCF_FULL) {
            Gcf_CloseFileReleaseSem();
			PifLog(MODULE_GCF, LOG_ERROR_GCF_BAK_FULL_ERR_02);
            return(GCF_FULL);
        }
    }

    /* Search Index Table And Assign by GC Number */
    Gcf_IndexFile.usGCNO       = usGCNumber;
    Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;  /* Set Read Flag */
    Gcf_IndexFile.offusBlockNo = 0;
    Gcf_IndexFile.offusReserveBlockNo = 0;
    
    sStatus = Gcf_Index(&Gcf_FileHed, GCF_REGIST, &Gcf_IndexFile, &ulHitPoint);
    if (sStatus == GCF_SUCCESS) {
		//see if the guest check is in the file, and has been created
        if ((FLEX_STORE_RECALL != Gcf_FileHed.usSystemtype) ||
             !(Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) ||
             (Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) ||
             (Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK)) 
			 //the GC is open to be written
		{
            Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
            Gcf_CloseFileReleaseSem();
            return(GCF_EXIST);
        }
        Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
        Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusBlockNo);
        Gcf_IndexFile.fbContFlag   = GCF_READ_FLAG_MASK;  /* Set Read Flag */
        Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
        if (Gcf_FileHed.offulPayTranNO != 0L) {
			ULONG   ulDriveIndexSerHitPoint;

            if (Gcf_DriveIndexSer(Gcf_FileHed.usCurPayGC, usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulPayTranNO) == GCF_SUCCESS) {
				Gcf_DriveIndexDeleteStd (&Gcf_FileHed, ulDriveIndexSerHitPoint);
            }
        }
    }
	
    Gcf_ReadFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ);

	sStatus = Gcf_FindEmptyBlockChain (&Gcf_FileHed, &Gcf_IndexFile, &ulHitPoint);
	if (sStatus == GCF_FULL) {
		Gcf_WriteFile (GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));
		Gcf_CloseFileReleaseSem();
		PifLog(MODULE_GCF, LOG_ERROR_GCF_BAK_FULL_ERR_03);
		return sStatus;
	}

    Gcf_FileHed.usCurNoPayGCN++;  /* Current Non Pay GC Number + 1  */
    
    Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));

	Gcf_IncrementGCNo (Gcf_FileHed.usSystemtype, &(Gcf_IndexFile.usGCNO));

    Gcf_FileHed.usStartGCN = Gcf_IndexFile.usGCNO;
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));

    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:   SHORT GusIndRead(USHORT usGCNumber,
*                                  UCHAR  *puchRcvBuffer,
*                                  USHORT usSize);
*   
*
*       Input:    USHORT usGCNumber,         Guest Check No.
*                 USHORT usSize              Width of puchRcvBuffer.
*      Output:    UCHAR  *puchRcvBuffer      Storage location for data. 
*       InOut:    Nothing
*
**  Return   :    Normal End  : length of guest check data
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Read guest check data indvidual.
*==========================================================================
*/
SHORT GusIndRead(GCNUM     usGCNumber,
                 UCHAR     *puchRcvBuffer,
                 USHORT    usSize)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus == GCF_SUCCESS) {
		ULONG           offulIndexPoint;
		GCF_INDEXFILE   Gcf_IndexFile = {0};
		USHORT          usCopyBlock;

		Gcf_IndexFile.usGCNO = usGCNumber;
		sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint);
		if (sStatus == GCF_SUCCESS) {
			sStatus = Gcf_DataBlockCopy(&Gcf_FileHed, 
								  Gcf_IndexFile.offusBlockNo, 
								  puchRcvBuffer, 
								  usSize, 
								  &usCopyBlock);
		}
		Gcf_CloseFileReleaseSem();
    }
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusAllIdRead(USHORT usType,
*                                    USHORT *pausRcvBuffer,
*                                    USHORT usSize);
*
*       Input:    USHORT usType             0: counter type, 1: drive through type
*                 USHORT usSize             Width of ausRvcBuffer.
*      Output:    USHORT *pausRcvBuffer     Storage location for Guest Check Numbers. 
*       InOut:    Nothing
*
**  Return   :    Normal End  : length of guest check data in bytes
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Read All Id(Guest check No or Drive through Number).
*
*               NOTE:
*               In the days of 2170 and NeighborhoodPOS, the guest check numbers
*               were held in a BYTE array. However with changes to touchscreen with
*               Rel 2.0.0 we increased the number of guest checks requiring a change
*               in the variable from a BYTE (UCHAR) to a USHORT. However the people
*               that did the change kept the logic of returning the size as the number
*               of bytes. The number of dependencies is so large that changing this to
*               be the number of guest check numbers rather than the number of bytes
*               is too much at this point.  Aug-24-2015  R. Chambers
*==========================================================================
*/
SHORT GusAllIdRead(USHORT usType,
                   USHORT *pausRcvBuffer,
                   USHORT usSize)
{
	GCF_FILEHED     Gcf_FileHed = { 0 };
    SHORT           sStatus;       // function call status variable.
	USHORT          usI;           // temporary index variable
	USHORT          usK = 0;       // count of number of open guest checks found

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	NHPOS_ASSERT(Gcf_FileHed.usCurGCN <= Gcf_FileHed.usMaxGCN);
	NHPOS_ASSERT(Gcf_FileHed.usCurNoPayGCN <= Gcf_FileHed.usMaxGCN);

	if (0 == Gcf_FileHed.usCurNoPayGCN) {
		// no open guest checks
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }   

	//Get list of open counter Type guest checks
	usK = 0;        // initialize count of open guest checks to zero. this is index into return buffer.
    for (usI = 0 ;
        (usI < Gcf_FileHed.usCurGCN) && ((usK * 2) < usSize); usI++ ) {
		GCF_INDEXFILE   Gcf_IndexFile = { 0 };

        if ( 0 == usI ) {  
            Gcf_ReadFile(Gcf_FileHed.offulGCIndFile, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ);
        } else {
            Gcf_ReadFile(0L, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_CONT_READ);
        }

        if (0 == (Gcf_IndexFile.fbContFlag & (GCF_DRIVE_THROUGH_MASK | GCF_PAYMENT_TRAN_MASK))) {
			// has not been paid and is not a Drive Thru guest check so add it to the list.
			pausRcvBuffer[usK] = Gcf_IndexFile.usGCNO;
            usK++;
        }
    }

	if ( usType == GCF_DRIVE_THROUGH_STORE )
	{
		SHORT      i;
		SHORT      iQueue[] = { GCF_PAYMENT_1, GCF_PAYMENT_2 };     // list of drive thru queues for payment guest checks.

		// If requested, add to list the open guest checks from the Drive Thru queues as well.
		for (i = 0; (i < 2) && ((usK * 2) < usSize); i++) {
			GCNUM    auchIndexDrive[GCF_MAX_DRIVE_NUMBER] = { 0 };
			USHORT   usCurDriveCount = Gcf_FileHed.usCurDrive[iQueue[i]];

            if (0 == usCurDriveCount || Gcf_FileHed.offulDrvNOFile[iQueue[i]] < 1) {          /* = 0 */
				// Drive Thru queue is empty or queue does not exist so skip.
                continue;
            }

			NHPOS_ASSERT(usCurDriveCount <= GCF_MAX_DRIVE_NUMBER);

			if (usCurDriveCount > GCF_MAX_DRIVE_NUMBER) usCurDriveCount = GCF_MAX_DRIVE_NUMBER;

            Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[iQueue[i]], auchIndexDrive, usCurDriveCount * sizeof(GCNUM), GCF_SEEK_READ);
  
            for (usI = 0; (usI < usCurDriveCount) && ((usK * 2) < usSize); usI++ ) {
				ULONG           offulIndexPoint;
				GCF_INDEXFILE   Gcf_IndexFile = { 0 };

				Gcf_IndexFile.usGCNO = auchIndexDrive[usI];
                if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
                    Gcf_CloseFileReleaseSem();
                    PifLog(MODULE_GCF, LOG_ERROR_GCF_DRIVE_UNMATCH);
                    return(GCF_FATAL);
                }

				NHPOS_ASSERT(Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK);

                if ((Gcf_IndexFile.fbContFlag & (GCF_DRIVE_THROUGH_MASK | GCF_PAYMENT_TRAN_MASK)) == GCF_DRIVE_THROUGH_MASK) {
					// has not been paid but is a Drive Thru guest check so add it to the list.
                    pausRcvBuffer[usK] = Gcf_IndexFile.usGCNO;
                    usK++;
                }
            }
        }
	}

    Gcf_CloseFileReleaseSem();

    if (0 == usK) {
		// no open guest checks found.
		sStatus = GCF_NOT_IN;
	}
	else {
		// sort the list of guest check numbers and return number of bytes.
		qsort (pausRcvBuffer, usK, sizeof(USHORT), compare);
		sStatus = (usK * sizeof(USHORT));  /* return the number of BYTES and not number of items, <sigh> */
	}

	return sStatus;
}
/*
*==========================================================================
**    Synopsis:   SHORT GusAllLock  (VOID)
*
*       Input:    Nothing
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_LOCKED
*                               GCF_DUR_LOCKED
*
**  Description:
*               Lock GCF. (Set Gcf All Lock Flag)
*==========================================================================
*/
SHORT GusAllLock  (VOID)
{
    GCF_FILEHED     Gcf_FileHed;
    USHORT          cui;
    SHORT           sStatus;
    
    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        if ( GCF_ALL_LOCKED == sStatus ) {
            return(GCF_DUR_LOCKED);
        }
        return(sStatus);
    }

    /* Check all guest checks to see if any are locked */
    sStatus = GCF_SUCCESS;
    for (cui = 0; cui < Gcf_FileHed.usCurGCN; cui++) {
		GCF_INDEXFILE   Gcf_IndexFile;

        if ( 0 == cui ) {  
            Gcf_ReadFile(Gcf_FileHed.offulGCIndFile, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ);
        } else {
            Gcf_ReadFile(0L, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_CONT_READ);
        }
        if ((Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) != 0) {
            sStatus = GCF_LOCKED;
            break;
        }
    }

    if ( GCF_SUCCESS == sStatus ) {
        fsGcfDurAllLock = GCF_SET_ALL_LOCK;    /* if no guest checks are locked then all lock */
    }
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:   SHORT GusAllUnLock  (VOID)
*
*       Input:    Nothing
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*
**  Description:
*               UnLock GCF (Clear Lock Flag)
*==========================================================================
*/
SHORT GusAllUnLock  (VOID)
{
    PifRequestSem(husGcfSem);
    fsGcfDurAllLock = 0x0000;           /* clear all lock memory */
    PifReleaseSem(husGcfSem);
    return(GCF_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:   SHORT GusAssignNo(USHORT usGCAssignNO)
*
*       Input:    usGCAssignNO          Start Guest Check No.
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  :  GCF_SUCCESS
*                 Abnormal End : GCF_FILE_NOT_FOUND
*                                GCF_DATA_ERROR
*
**  Description:
*               Assign guest check No.
*==========================================================================
*/
SHORT GusAssignNo(GCNUM usGCAssignNO)
{
    SHORT           sStatus;
    GCF_FILEHED     Gcf_FileHed;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    if ((usGCAssignNO < GCF_MAX_CHK_NUMBER) && (usGCAssignNO)) {
        if (FLEX_STORE_RECALL == Gcf_FileHed.usSystemtype) {
            if (usGCAssignNO > GCF_MAX_DRIVE_NUMBER) {
                Gcf_CloseFileReleaseSem();
                return(GCF_DATA_ERROR);
            } 
        }
    } else {
        Gcf_CloseFileReleaseSem();
        return(GCF_DATA_ERROR);
    }

    Gcf_FileHed.usStartGCN = usGCAssignNO;
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));
    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:   SHORT   GusResetReadFlag(USHORT usGCNumber, USHORT usType)
*
*       Input:    USHORT usGCNumber
*                 USHORT usType
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*                               GCF_ALL_LOCKED
*
**  Description:
*               Reset Read flag.
*==========================================================================
*/
SHORT   GusResetReadFlag(GCNUM usGCNumber, USHORT usType)
{
    GCF_FILEHED         Gcf_FileHed;
    GCF_INDEXFILE       Gcf_IndexFile;
    SHORT               sStatus, i, sMaster;
    ULONG               ulHitPoint;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    if (0 == usGCNumber) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    sMaster = CliCheckAsMaster();               /* Check Master */

    /* Search Index File */
    Gcf_IndexFile.usGCNO = usGCNumber;
    sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &ulHitPoint);

    if (sStatus == GCF_SUCCESS) {
		/* Found the guest check so we need to Reset Read Flag and remove from current position in queues */
		ULONG  ulDriveIndexSerHitPoint;

		Gcf_FreeQueReturn(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
		Gcf_IndexFile.fbContFlag &= ~GCF_READ_FLAG_MASK;

		if (usType == GCF_NO_APPEND) {
			Gcf_IndexFile.fbContFlag &= ~GCF_DRIVE_THROUGH_MASK;
		} else {
			Gcf_IndexFile.fbContFlag |= GCF_DRIVE_THROUGH_MASK;
		}

		Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));

		for ( i = 0; i < GCF_MAX_QUEUE; i++) {
			if (Gcf_DriveIndexSer(Gcf_FileHed.usCurDrive[i], usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulDrvNOFile[i]) == GCF_SUCCESS) {
				Gcf_DriveIndexDeleteSpl (&Gcf_FileHed, i, ulDriveIndexSerHitPoint);
			}
		}
		if (usType == GCF_APPEND_QUEUE1) {
			Gcf_QueDrive(&Gcf_FileHed, usGCNumber, GCF_PAYMENT_1);
			Gcf_QueDrive(&Gcf_FileHed, usGCNumber, GCF_DELIVERY_1);
		} else if (usType == GCF_APPEND_QUEUE2) {
			Gcf_QueDrive(&Gcf_FileHed, usGCNumber, GCF_PAYMENT_2);
			Gcf_QueDrive(&Gcf_FileHed, usGCNumber, GCF_DELIVERY_2);
		}

		Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(Gcf_FileHed));

		if (sMaster == STUB_SUCCESS) {
			switch (usType) {
			case GCF_APPEND_QUEUE1:
				NbWriteMessage(1, NB_REQPAYMENT1 | NB_REQDELIVERY1);
				break;

			case GCF_APPEND_QUEUE2:
				NbWriteMessage(1, NB_REQPAYMENT2 | NB_REQDELIVERY2);
				break;
			}
		}
	}

    Gcf_CloseFileReleaseSem();
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:   SHORT   GusCheckExist(VOID)
*
*       Input:    Nothing
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Normal End  : GCF_FILE_NOT_FOUND - error opening file
*                 Abnormal End: GCF_EXIST - at least one unpaid Guest Check
*                 Abnormal End: GCF_ALL_LOCKED - Guest Check File is locked
*
**  Description:
*               Check if guest check No Exist.
*==========================================================================
*/
SHORT   GusCheckExist(VOID)
{
    SHORT           sStatus;
    GCF_FILEHED     Gcf_FileHed;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    if (0 == Gcf_FileHed.usCurNoPayGCN) {
        sStatus = GCF_SUCCESS;
    } else {
        sStatus = GCF_EXIST;
    }
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:   SHORT   GusAllIdReadBW(USHORT usType,
*                                       USHORT usWaiterNo.
*                                       USHORT *pusRcvBuffer.
*                                       USHORT usSize);
*
*       Input:    USHORT usType         0: Counter Type, 1:drive thorou type
*                 ULONG  ulWaiterNo     Waiter no
*                 USHORT usSize         size of puchRcvBuffer.
*                 
*      Output:    USHORT pusRcvBuffer   Storage location for Guest Check NO. 
*
*       InOut:    Nothing
*
**  Return   :    Normal End  : Length of Guest Check No.
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*              Read guest No by Waiter No.
*==========================================================================
*/
SHORT   GusAllIdReadBW(USHORT usType, ULONG ulWaiterNo, USHORT *pusRcvBuffer, USHORT usSize)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus;
    USHORT          usLength = 0;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }
    
	// If there are any not paid guest checks in the transaction file
	// then we will traverse all of the guest checks looking for checks
	// that were created by the specified operator.
    if (0 != Gcf_FileHed.usCurNoPayGCN) {
		ULONG   offulIndexPosition = Gcf_FileHed.offulGCIndFile;
 		USHORT  usI;
   
		// get current drive through type 
		for (usI = 0; (usI < Gcf_FileHed.usCurGCN) && ((usLength * 2) < usSize); usI++) {
			GCF_INDEXFILE   Gcf_IndexFile = {0};

			Gcf_ReadFile(offulIndexPosition, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ); 
			offulIndexPosition += sizeof(GCF_INDEXFILE);

			if (Gcf_IndexFile.offusBlockNo)
			{
				TRANGCFQUAL     TranGCFQual = {0};

				Gcf_ReadFile(Gcf_FileHed.offulGCDataFile + (Gcf_IndexFile.offusBlockNo - 1) * GCF_DATA_BLOCK_SIZE + sizeof(GCF_DATAHEAD), &TranGCFQual, sizeof(TRANGCFQUAL), GCF_SEEK_READ);
				if (TranGCFQual.ulCashierID == ulWaiterNo) {    // V3.3 
					*pusRcvBuffer = Gcf_IndexFile.usGCNO;
					pusRcvBuffer++;
					usLength++;
				}
			}
		}
    }

    if (0 == usLength) { /* length == 0 */
        usLength = GCF_NOT_IN;
    } else {
        usLength *= 2;
    }
    Gcf_CloseFileReleaseSem();
    return(usLength);
}

static VOID GusAssignToStatusState (GCF_STATUS_STATE *pusRcvBuffer, TRANGCFQUAL *pTranGCFQual, GCF_INDEXFILE *pGcf_IndexFile)
{
	memset (pusRcvBuffer, 0, sizeof(*pusRcvBuffer));   // zero out this array entry to prep for data assignments.

	pusRcvBuffer->usGCNO = pGcf_IndexFile->usGCNO;
	pusRcvBuffer->fbContFlag = pGcf_IndexFile->fbContFlag;
	if (pTranGCFQual) {
		pusRcvBuffer->ulCashierID = pTranGCFQual->ulCashierID;
		pusRcvBuffer->usTableNo = pTranGCFQual->usTableNo;
		pusRcvBuffer->lCurBalance = pTranGCFQual->lCurBalance;
	}
	pusRcvBuffer->uchGcfDay = pGcf_IndexFile->GcfCreateTime.uchGcfDay;
	pusRcvBuffer->uchGcfHour = pGcf_IndexFile->GcfCreateTime.uchGcfHour;
	pusRcvBuffer->uchGcfMinute = pGcf_IndexFile->GcfCreateTime.uchGcfMinute;
	pusRcvBuffer->uchGcfMonth = pGcf_IndexFile->GcfCreateTime.uchGcfMonth;
	pusRcvBuffer->uchGcfSecond = pGcf_IndexFile->GcfCreateTime.uchGcfSecond;
	pusRcvBuffer->uchGcfYear = pGcf_IndexFile->GcfCreateTime.uchGcfYear;
}

SHORT   GusAllIdReadFiltered (USHORT usType, ULONG ulWaiterNo, USHORT usTableNo, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus;
    USHORT          usLength = 0;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }
    
	// If there are any not paid guest checks in the transaction file
	// then we will traverse all of the guest checks looking for checks
	// that were created by the specified operator.
    if (0 != Gcf_FileHed.usCurNoPayGCN) {
		ULONG     offulIndexPosition = Gcf_FileHed.offulGCIndFile;
		USHORT    usI;
    
		// get current drive through type 
		for (usI = 0; (usI < Gcf_FileHed.usCurGCN) && (usLength < usCount); usI++) {
			GCF_INDEXFILE   Gcf_IndexFile = {0};

			Gcf_ReadFile(offulIndexPosition, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ); 
			offulIndexPosition += sizeof(GCF_INDEXFILE);

			if (Gcf_IndexFile.offusBlockNo)
			{
				TRANGCFQUAL     TranGCFQual = {0};

				Gcf_ReadFile(Gcf_FileHed.offulGCDataFile + (Gcf_IndexFile.offusBlockNo - 1) * GCF_DATA_BLOCK_SIZE + sizeof(GCF_DATAHEAD), &TranGCFQual, sizeof(TRANGCFQUAL), GCF_SEEK_READ);
				if (ulWaiterNo && TranGCFQual.ulCashierID != ulWaiterNo) {    // V3.3
					continue;
				} else if (usTableNo && TranGCFQual.usTableNo != usTableNo) {
					continue;
				} else if (Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) {
					continue;
				}
				GusAssignToStatusState (pusRcvBuffer, &TranGCFQual, &Gcf_IndexFile);
				pusRcvBuffer++;
				usLength++;
			}
		}
    }

    Gcf_CloseFileReleaseSem();
    return(usLength);
}

SHORT   GusSearchForBarCode (USHORT usType, UCHAR *uchUniqueIdentifier, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus;
    USHORT          usLength = 0;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	// If there are any not paid guest checks in the transaction file
	// then we will traverse all of the guest checks looking for checks
	// that were created by the specified operator.
    if (0 != Gcf_FileHed.usCurNoPayGCN) {
		ULONG   offulIndexPosition = Gcf_FileHed.offulGCIndFile;
		USHORT  usI;
    
		// get current drive through type 
		for (usI = 0; (usI < Gcf_FileHed.usCurGCN) && (usLength < usCount); usI++) {
			GCF_INDEXFILE   Gcf_IndexFile = {0};

			Gcf_ReadFile(offulIndexPosition, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ); 
			offulIndexPosition += sizeof(GCF_INDEXFILE);

			if (Gcf_IndexFile.offusBlockNo)
			{
				TRANGCFQUAL     TranGCFQual = {0};

				Gcf_ReadFile(Gcf_FileHed.offulGCDataFile + (Gcf_IndexFile.offusBlockNo - 1) * GCF_DATA_BLOCK_SIZE + sizeof(GCF_DATAHEAD), &TranGCFQual, sizeof(TRANGCFQUAL), GCF_SEEK_READ);
				if (Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) {
					continue;
				}
				if (UifCompareUniqueIdWithoutTenderId (TranGCFQual.uchUniqueIdentifier, uchUniqueIdentifier) == 0) {
					GusAssignToStatusState (pusRcvBuffer, &TranGCFQual, &Gcf_IndexFile);
					pusRcvBuffer++;
					usLength++;
				}
			}
		}
    }

    Gcf_CloseFileReleaseSem();
    return(usLength);
}

/*
*==========================================================================
**    Synopsis:   VOID   GusInitialize(USHORT usMode)
*
*       Input:    UCHAR     uchMode 
*                 
*      Output:    Nothing 
*
*       InOut:    Nothing
*
*                 
**  Return   :    Nothing
*
**  Description:
*               Initiarize GCF.              
*==========================================================================
*/
VOID   GusInitialize(USHORT usMode)
{
    husGcfSem = PifCreateSem( 1 );
	PifSetTimeoutSem(husGcfSem, 5000);

    if (usMode & POWER_UP_CLEAR_TOTAL) {
		PARAFLEXMEM     ParaFlexMem = {0};

        ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
        ParaFlexMem.uchAddress    = FLEX_GC_ADR;
        CliParaRead(&ParaFlexMem);
        CliCreateFile(FLEX_GC_ADR, ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag);
    }
}
/*
*==========================================================================
**    Synopsis:   SHORT   GusReadAllLockedGCN(USHORT *pusRcvBuffer.
*                                             USHORT usSize);
*
*       Input:    USHORT usSize         size of puchRcvBuffer.
*                 
*      Output:    USHORT pusRcvBuffer   Storage location for Guest Check NO. 
*
*       InOut:    Nothing
*
**  Return   :    Normal End  : Length of Guest Check No.
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*              Read locked guest No.
*==========================================================================
*/
SHORT   GusReadAllLockedGCN(USHORT *pusRcvBuffer, USHORT usSize)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus;
    USHORT          usLength , usI;
    ULONG           offulIndexPosition;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    if (0 == Gcf_FileHed.usCurGCN) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    offulIndexPosition = Gcf_FileHed.offulGCIndFile;
    
    for (usI = 0, usLength = 0; (usI < Gcf_FileHed.usCurGCN) && ((usLength * 2) < usSize); usI++) {
		GCF_INDEXFILE   Gcf_IndexFile = {0};

        Gcf_ReadFile(offulIndexPosition, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ); 
        offulIndexPosition += sizeof(GCF_INDEXFILE);

        if (Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) {
            *pusRcvBuffer = Gcf_IndexFile.usGCNO;
            pusRcvBuffer++;
            usLength++;
        }
    }

    if (0 == usLength) { /* length == 0 */
        usLength = GCF_NOT_IN;
    } else {
        usLength *= 2;
    }
    Gcf_CloseFileReleaseSem();
    return(usLength);
}

SHORT   GusReadAllGuestChecksWithStatus (GCF_STATUS_STATE *pRcvBuffer, USHORT usRcvMaxCount, GCF_STATUS_HEADER *pGcf_FileHed)
{
	GCNUM           LastGcNum = 0;
	GCF_FILEHED     Gcf_FileHed = {0};
    SHORT           sStatus;
    USHORT          usI;
    ULONG           offulIndexPosition;

	if (! CLI_IAM_MASTER) {
		pGcf_FileHed->usStartGCN = LastGcNum;
		pGcf_FileHed->usMaxGcn = Gcf_FileHed.usMaxGCN;
		return 0;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
		pGcf_FileHed->usStartGCN = LastGcNum;
		pGcf_FileHed->usMaxGcn = Gcf_FileHed.usMaxGCN;
        return(sStatus);
    }

    if (0 == Gcf_FileHed.usCurGCN) {
		pGcf_FileHed->usStartGCN = LastGcNum;
		pGcf_FileHed->usMaxGcn = Gcf_FileHed.usMaxGCN;
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    offulIndexPosition = Gcf_FileHed.offulGCIndFile;
    
    for (usI = 0; (usI < Gcf_FileHed.usCurGCN) && (usI < usRcvMaxCount); usI++) {
		GCF_INDEXFILE   Gcf_IndexFile = {0};

        Gcf_ReadFile(offulIndexPosition, &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ); 
		GusAssignToStatusState (pRcvBuffer, 0, &Gcf_IndexFile);
		LastGcNum = pRcvBuffer->usGCNO;
        offulIndexPosition += sizeof(GCF_INDEXFILE);
        pRcvBuffer++;
    }

	// if caller wants the Guest Check File header then give it to them.
	if (pGcf_FileHed) {
		pGcf_FileHed->usStartGCN = LastGcNum;
		pGcf_FileHed->usMaxGcn = Gcf_FileHed.usMaxGCN;
	}

	Gcf_CloseFileReleaseSem();
    return (usI);
}



/*
*==========================================================================
**    Synopsis:   SHORT GusStoreFileFH (USHORT usType
*                                       USHORT usGCNumber,
*                                       SHORT  hsFileHandle,
*                                       USHORT usStartPoint,
*                                       USHORT usSize);
*
*       Input:  USHORT usType    * drive through = 1, other = 0 *
*               USHORT usGCNumber
*               SHORT  hsFileHandle 
*               USHORT usStartPoint
*               USHORT usSize
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_FULL
*                               GCF_NOT_IN_FILE
*                               GCF_NOT_LOCKED
*                               GCF_ALL_LOCKED
*
**  Description:
*               Store GCNumber in GCF(File Handle I/F Version).              
*==========================================================================
*/
SHORT GusStoreFileFH(USHORT  usType,
                     GCNUM  usGCNumber,
                     SHORT   hsFileHandle,
                     ULONG   ulStartPoint,
                     ULONG   ulSize)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus, sMaster;
    USHORT          fbStatus;

	if (hsFileHandle < 0) {
		PifLog (MODULE_GCF, LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_1);
		return GCF_FATAL;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    sMaster = CliCheckAsMaster();               /* Check Master */

    /* Check guest Check No */
    if (0 == usGCNumber) {   /* usGCNumber = 0 */
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    fbStatus = 0;                       /* Copy type */ 
    if ( ( ((usType & GCF_DRIVE_THROUGH_FIELD) >= GCF_DRIVE_THROUGH_STORE) &&
           ((usType & GCF_DRIVE_THROUGH_FIELD) <= GCF_DRIVE_TH_DELIVERY2)      ) ||
		 ( ((usType & GCF_DRIVE_THROUGH_FIELD) >= GCF_DRIVE_PUSH_THROUGH_STORE) &&
		   ((usType & GCF_DRIVE_THROUGH_FIELD) <= GCF_DRIVE_PUSH_DELIVERY2)    ) ) {
        fbStatus = GCF_DRIVE_THROUGH_MASK;       /*  Set drive through type */
    }

    if (GCF_PAID == Gcf_CheckPayment(ulStartPoint, hsFileHandle)) {
        fbStatus |= GCF_PAYMENT_TRAN_MASK;       /*  Set paid transaction type */
    } else {
        fbStatus &= ~GCF_PAYMENT_TRAN_MASK;      /*  Reset paid transaction type */
    }

#if 0
	// If this is a drive thru system and if this is a paid guest check then lets
	// check to see if we have filled up the current paid transaction queue or not.
	if (Gcf_FileHed.offulPayTranNO != 0L) {
		if ((fbStatus & GCF_PAYMENT_TRAN_MASK) && (Gcf_FileHed.usCurPayGC + 1 > Gcf_FileHed.usMaxGCN)) {
			NHPOS_ASSERT(!"Gcf_FileHed.usCurPayGC + 1 > Gcf_FileHed.usMaxGCN,  Try Gcf_DeletePaidTran();");
			if (Gcf_DeletePaidTran(&Gcf_FileHed, usGCNumber) == GCF_FULL) {
				Gcf_CloseFileReleaseSem();
				NHPOS_ASSERT(!"GCF FULL ERROR, Current number of paid GC");
				PifLog (MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_05);
				return(GCF_FULL);
			}
		}
	}
#endif

    sStatus = Gcf_StoreDataFH(&Gcf_FileHed, fbStatus, usGCNumber, hsFileHandle, ulStartPoint, ulSize, usType);
    if ((sStatus == GCF_SUCCESS) && (fbStatus & GCF_PAYMENT_TRAN_MASK)) { 
		  /* 
			update payment transaction queue if this is a drive thru system.

			Check to see if the Guest Check Number is already in the payed check queue.
			If so, nothing else to do.
			Otherwise, add this Guest Check Number to the end of the payed check queue
			for reuse.
		 */
        if (Gcf_FileHed.offulPayTranNO != 0L) {
			ULONG  ulDriveIndexSerHitPoint;

            if (Gcf_DriveIndexSer(Gcf_FileHed.usCurPayGC, usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulPayTranNO) == GCF_NOT_IN) {
                Gcf_WriteFile(Gcf_FileHed.offulPayTranNO + (Gcf_FileHed.usCurPayGC * sizeof(GCNUM)), &usGCNumber, sizeof(usGCNumber));
				if (Gcf_FileHed.usCurPayGC < Gcf_FileHed.usMaxGCN)
					Gcf_FileHed.usCurPayGC++;    /* Current Non Pay GC Number + 1 */
			}
		}
    }

    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

    if (sMaster == STUB_SUCCESS) {
        switch (usType & GCF_DRIVE_THROUGH_FIELD) {
        case GCF_DRIVE_PUSH_THROUGH_STORE:
			// drop through as transaction prepended in function Gcf_StoreDataFH ()
        case GCF_DRIVE_THROUGH_STORE:
            NbWriteMessage(1, NB_REQPAYMENT1);
            break;

        case GCF_DRIVE_PUSH_DELIVERY1:
			// drop through as transaction prepended in function Gcf_StoreDataFH ()
        case GCF_DRIVE_DELIVERY1:
            NbWriteMessage(1, NB_REQPAYMENT1 | NB_REQDELIVERY1);
            break;

        case GCF_DRIVE_PUSH_THROUGH2:
			// drop through as transaction prepended in function Gcf_StoreDataFH ()
        case GCF_DRIVE_THROUGH2:
            NbWriteMessage(1, NB_REQPAYMENT2);
            break;

        case GCF_DRIVE_PUSH_DELIVERY2:
			// drop through as transaction prepended in function Gcf_StoreDataFH ()
        case GCF_DRIVE_DELIVERY2:
            NbWriteMessage(1, NB_REQPAYMENT2 | NB_REQDELIVERY2);
            break;

        case GCF_DRIVE_TH_TENDER1:
            NbWriteMessage(1, NB_REQORDER1);
            break;

        case GCF_DRIVE_TH_TENDER2:
            NbWriteMessage(1, NB_REQORDER2);
            break;

        case GCF_DRIVE_DE_TENDER1:
            NbWriteMessage(1, NB_REQDELIVERY1 | NB_REQORDER1);
            break;

        case GCF_DRIVE_DE_TENDER2:
            NbWriteMessage(1, NB_REQDELIVERY2 | NB_REQORDER2);
            break;

        }
    }
        
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusReadLockFH(USHORT usGCNumber,
*                                     SHORT  hsFileHandle,
*                                     USHORT usStartPoint,
*                                     USHORT usSize,
*                                     USHORT usType);
*
*       Input:    USHORT usGCNumber         guest check no.
*                 USHORT usStartPoint       start pointer to be stored 
*                 USHORT usSize             size of the file 
*                 USHORT usType             0:By Reg  0<>:Sup
*
*      Output:    SHORT  hsFileHandle       file handle 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*                               GCF_LOCKED
*                               GCF_ALL_LOCKED
*                               GCF_FULL
*
**  Description:
*               Read and lock Guest check file(File Handle I/F Version).              
*==========================================================================
*/
SHORT GusReadLockFH(GCNUM  usGCNumber,
                    SHORT   hsFileHandle,
                    USHORT  usStartPoint,
                    USHORT  usSize,
                    USHORT  usType,
					ULONG	*pulBytesRead)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    ULONG           offulIndexPoint, ulDriveIndexSerHitPoint;
    USHORT          usAssignBlock, usCopyBlock;
    SHORT           sStatus, i, sMaster, sRet;

	*pulBytesRead = 0;

	if (hsFileHandle < 0) {
		PifLog (MODULE_GCF, LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_2);
		return GCF_FATAL;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }
    sMaster = CliCheckAsMaster();               /* Check Master */

    /* Check DriveThrough Type */
    
    if (0 == usGCNumber) {  /* GC number == 0 DRIVE THROUGH TYPE */
        if (usType < GCF_PAYMENT_QUEUE1) {          
            Gcf_CloseFileReleaseSem();
            return(GCF_NOT_IN);
        }

		NHPOS_ASSERT(usType >= GCF_PAYMENT_QUEUE1);

        if (0 == Gcf_FileHed.usCurDrive[usType - GCF_PAYMENT_QUEUE1]) {
            Gcf_CloseFileReleaseSem();
            return(GCF_NOT_IN);
        }
        Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[usType - GCF_PAYMENT_QUEUE1], &usGCNumber, sizeof(GCNUM), GCF_SEEK_READ);
    }
    Gcf_IndexFile.usGCNO = usGCNumber;
    
    /* Search GCNO in Index */
    
    if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }
    /* Check During Read */
    if ((Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) != 0) {
        Gcf_CloseFileReleaseSem();
        return(GCF_LOCKED);
    }

	if (GCF_RETURNS == usType) {
		/* Check if not a Paid Transaction, if so not allowed */
		if ((Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) == 0) {
			Gcf_CloseFileReleaseSem();
			return(GCF_NOT_ALLOWED);
		}
		/* Check if already been returned , if so not allowed */
		if ((Gcf_IndexFile.fbContFlag & GCF_RETURNS_TRAN_MASK) != 0) {
			Gcf_CloseFileReleaseSem();
			return(GCF_NOT_ALLOWED);
		}
		Gcf_IndexFile.fbContFlag |= GCF_RETURNS_TRAN_MASK;
	} else {
		/* Check if a Paid Transaction, if so not allowed */
		if ((Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) != 0) {
			Gcf_CloseFileReleaseSem();
			return(GCF_NOT_ALLOWED);
		}
		Gcf_IndexFile.fbContFlag |= GCF_READ_FLAG_MASK;
	}
    
    Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
    
    sRet = Gcf_DataBlockCopyFH(&Gcf_FileHed, Gcf_IndexFile.offusBlockNo, hsFileHandle, usStartPoint, usSize, &usCopyBlock, pulBytesRead);

    if ( GCF_SUPER != usType && GCF_RETURNS != usType) {
		/*
			if user is not Supervisor, then add in additional blocks needed to bring
			the number of blocks to max number for a transaction then remove the
			transaction from the necessary queues and let other terminals know that
			the queues have changed.
		*/
		Gcf_GetNumberofBlockAssign(Gcf_FileHed.usSystemtype, &usAssignBlock);

		NHPOS_ASSERT(usAssignBlock >= usCopyBlock);

		if (usAssignBlock > usCopyBlock) {
			SHORT sDeleteTranDone = 0;
			GCF_INDEXFILE   Gcf_IndexFileTemp = Gcf_IndexFile;

			sStatus = Gcf_FreeQueTake(&Gcf_FileHed, (USHORT)(usAssignBlock - usCopyBlock), &Gcf_IndexFile.offusReserveBlockNo);
			if (sStatus == GCF_FULL) {
				Gcf_IndexFile.fbContFlag &= ~GCF_READ_FLAG_MASK;      /* Read Flag clear */
				Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
				while (sStatus == GCF_FULL) {
					if (Gcf_DeletePaidTran(&Gcf_FileHed, usGCNumber) == GCF_FULL) {
						/* avoid gcf full at add check, 05/07/01 */
						if ((FLEX_PRECHK_BUFFER == Gcf_FileHed.usSystemtype) || (FLEX_PRECHK_UNBUFFER == Gcf_FileHed.usSystemtype)) {
							break;
						}
						Gcf_IndexFile.fbContFlag &= ~GCF_READ_FLAG_MASK;      /* Read Flag clear */
						NHPOS_ASSERT(!"GCF FULL ERROR, could not free the que for a new GC");
						sRet = GCF_FULL;
						break;
					}
					sDeleteTranDone = 1;
					sStatus = Gcf_BlockReserve(&Gcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
				}

				if (sDeleteTranDone) {
					// Redo the search for the guest check number in case we have had to
					// delete a paid transaction which has in turn moved the indexes around.
					sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFileTemp, &offulIndexPoint);
				}
			}
		}

		Gcf_IndexFile.fbContFlag |= GCF_READ_FLAG_MASK;
		Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
    
	//RPH SR#201
		if ((sRet >= 0) && (usType >= GCF_PAYMENT_QUEUE1)) { 

			NHPOS_ASSERT(usType >= GCF_PAYMENT_QUEUE1);

			if (Gcf_DriveIndexSer(Gcf_FileHed.usCurDrive[usType - GCF_PAYMENT_QUEUE1], 
								  usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulDrvNOFile[usType - GCF_PAYMENT_QUEUE1])
								  == GCF_SUCCESS) {
					Gcf_DriveIndexDeleteSpl (&Gcf_FileHed, (USHORT)(usType - GCF_PAYMENT_QUEUE1), ulDriveIndexSerHitPoint);
			}
		}
		//RPH SR#201
		if ((sRet >= 0) && (usType == GCF_REORDER)) {
			for ( i=0; i < 2; i++) {
				if (Gcf_DriveIndexSer(Gcf_FileHed.usCurDrive[i], usGCNumber, &ulDriveIndexSerHitPoint, Gcf_FileHed.offulDrvNOFile[i]) == GCF_SUCCESS) {
					Gcf_DriveIndexDeleteSpl (&Gcf_FileHed, i, ulDriveIndexSerHitPoint);
					if (sMaster == STUB_SUCCESS) {
						NbWriteMessage(1, (UCHAR)(NB_REQPAYMENT1 << i));
					}
				}
			}
		}

		/* Write File Header */
    
		Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));
    
		if (sMaster == STUB_SUCCESS) {
			switch (usType) {
			case GCF_PAYMENT_QUEUE1:
				NbWriteMessage(1, NB_REQPAYMENT1);
				break;

			case GCF_PAYMENT_QUEUE2:
				NbWriteMessage(1, NB_REQPAYMENT2);
				break;
			}
		}
    }
        
    Gcf_CloseFileReleaseSem();
    return(sRet);
}

SHORT GusReturnsLockClear(GCNUM  usGCNumber)
{
    GCF_FILEHED     Gcf_FileHed;
	GCF_INDEXFILE   Gcf_IndexFile = {0};
    ULONG           offulIndexPoint;
    SHORT           sStatus;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    Gcf_IndexFile.usGCNO = usGCNumber;
    
    /* Search GCNO in Index */
    if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }
    /* Check During Read */
    if ((Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) != 0) {
        Gcf_CloseFileReleaseSem();
        return(GCF_LOCKED);
    }

    /* Check if returns is not set */
    if ((Gcf_IndexFile.fbContFlag & GCF_RETURNS_TRAN_MASK) == 0) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_ALLOWED);
    }

	Gcf_IndexFile.fbContFlag &= ~GCF_RETURNS_TRAN_MASK;
    
    Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
    Gcf_CloseFileReleaseSem();
	return GCF_SUCCESS;
}

/*
*==========================================================================
**    Synopsis:   SHORT GusIndReadFH(USHORT usGCNumber,
*                                    SHORT  hsFileHandle,
*                                    USHORT usStartPoint,
*                                    USHORT usSize);
*   
*
*       Input:    USHORT usGCNumber,         Guest Check No.
*                 USHORT usStartPoint        Start pointer to be stored 
*                 USHORT usSize              Width of the file.
*      Output:    SHORT  hsFileHandle        file handle. 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Read guest check data indvidual(File Handle I/F Version).
*==========================================================================
*/
SHORT GusIndReadFH(GCNUM     usGCNumber,
                   SHORT      hsFileHandle,
                   USHORT     usStartPoint,
                   USHORT     usSize,
				   ULONG	  *pulBytesRead)
{
    GCF_FILEHED     Gcf_FileHed;
	GCF_INDEXFILE   Gcf_IndexFile = {0};
    SHORT           sStatus;
    ULONG           offulIndexPoint;
    USHORT          usCopyBlock;

	*pulBytesRead = 0;

	if (hsFileHandle < 0) {
		PifLog (MODULE_GCF, LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_3);
		return GCF_FATAL;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	// initialize the index struct to known values.
    Gcf_IndexFile.usGCNO = usGCNumber;

    sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint);
	if (sStatus == GCF_SUCCESS && (Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) == 0) {
		sStatus = Gcf_DataBlockCopyFH(&Gcf_FileHed, Gcf_IndexFile.offusBlockNo, hsFileHandle, usStartPoint, usSize, &usCopyBlock, pulBytesRead);
	}
	else {
		// Create zeroed out data in the temp file so that it will be processed
		// as if it were an empty guest check
		union {
			TRANGCFQUAL    TranGcfQual;
			TRANITEMIZERS  TranItemizers;
			USHORT         usTranConsStoVli;
			PRTIDXHDR      IdxFileInfo;
		}        u;
		ULONG    offulWritePoint = (ULONG)usStartPoint;

		memset (&u, 0, sizeof(u));

		u.TranGcfQual.DateTimeStamp.usHour = Gcf_IndexFile.GcfCreateTime.uchGcfHour;
		u.TranGcfQual.DateTimeStamp.usMin = Gcf_IndexFile.GcfCreateTime.uchGcfMinute;
		u.TranGcfQual.DateTimeStamp.usSec = Gcf_IndexFile.GcfCreateTime.uchGcfSecond;
		u.TranGcfQual.DateTimeStamp.usMonth = Gcf_IndexFile.GcfCreateTime.uchGcfMonth;
		u.TranGcfQual.DateTimeStamp.usMDay = Gcf_IndexFile.GcfCreateTime.uchGcfDay;
		u.TranGcfQual.DateTimeStamp.usYear = Gcf_IndexFile.GcfCreateTime.uchGcfYear + 2000;

		Gcf_WriteFileFH (offulWritePoint, &u.TranGcfQual, sizeof(TRANGCFQUAL), hsFileHandle);
		offulWritePoint += sizeof(TRANGCFQUAL);
		Gcf_WriteFileFH (offulWritePoint, &u.TranItemizers, sizeof(TRANITEMIZERS), hsFileHandle);
		offulWritePoint += sizeof(TRANITEMIZERS);
		Gcf_WriteFileFH (offulWritePoint, &u.usTranConsStoVli, sizeof(USHORT), hsFileHandle);
		offulWritePoint += sizeof(USHORT);
		Gcf_WriteFileFH (offulWritePoint, &u.IdxFileInfo, sizeof(PRTIDXHDR), hsFileHandle);
		offulWritePoint += sizeof(PRTIDXHDR);

		*pulBytesRead = offulWritePoint;
		if ((Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) != 0)
			sStatus = GCF_LOCKED;
	}

    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT   GusReqBackUpFH( SHORT hsFileHandle )
*                         
*       Input:    SHORT hsFileHandle    File Handle to Request Data.
*                 
*      Output:    Nothing 
*
*       InOut:    Nothing
*
*                 
**  Return   :    GCF_SUCCESS
*                 GCF_NOT_LOCKED
*                 GCF_FULL
**  Description:
*               Request Back up. (File Handle I/F Version)             
*==========================================================================
*/
SHORT   GusReqBackUpFH( SHORT hsFileHandle )
{
    GCF_FILEHED     Gcf_FileHed, Gcf_FileHed_AsMT;
    GCF_BACKUP      Gcf_ReqBak;
    GCF_BACKUP      Gcf_Backup;
    GCF_BAKDATA     Gcf_BKData;
    USHORT          usRcvLen, usInit;
    SHORT           sStatusLen,  sStatus, i;
    USHORT          usNoBlock;
	GCNUM           auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};   

	if (hsFileHandle < 0) {
		PifLog (MODULE_GCF, LOG_ERROR_GCF_BACKUPFH_FH_HANDLE_1);
		return GCF_FATAL;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        if ( sStatus == GCF_FILE_NOT_FOUND ) {
            sStatus = GCF_SUCCESS;    /* if file not found, then return */
        }
        return(sStatus);
    }

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE: Begin GusReqBackUpFH() to transfer Guest Checks.");
    fsGcfDurAllLock = 0x0000;           /* clear all lock memory */

    if (Gcf_FileHed.usCurGCN) {

        /* Creat Free Que */
        Gcf_GetNumberofBlockTypical(Gcf_FileHed.usSystemtype, &usNoBlock);
        Gcf_CreatAllFreeQue(Gcf_FileHed.offulGCDataFile, (USHORT)(Gcf_FileHed.usMaxGCN * usNoBlock));

        Gcf_FileHed.usCurGCN = 0;
        Gcf_FileHed.usCurNoPayGCN = 0;              
        Gcf_FileHed.usCurDrive[0] = 0;
        Gcf_FileHed.usCurDrive[1] = 0;              
        Gcf_FileHed.usCurDrive[2] = 0;              
        Gcf_FileHed.usCurDrive[3] = 0;              
        Gcf_FileHed.usCurPayGC = 0;              
    }

    /* Request File Header from as master */
    memset(&Gcf_ReqBak, 0, sizeof(GCF_BACKUP));

    for (;;) {

        Gcf_ReadFileFH(0, &usRcvLen, sizeof(USHORT), hsFileHandle);

        if  ((sStatusLen = SstReqBackUpFH(CLI_FCBAKGCF, (UCHAR *)&Gcf_ReqBak, sizeof(GCF_BACKUP), hsFileHandle, &usRcvLen)) < 0 ) {
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: End GusReqBackUpFH().");
            Gcf_CloseFileReleaseSem();
            return(sStatusLen);
        }

        Gcf_ReadFileFH(SERTMPFILE_DATASTART, &Gcf_Backup, sizeof(GCF_BACKUP), hsFileHandle);

        Gcf_ReadFileFH(SERTMPFILE_DATASTART + sizeof(GCF_BACKUP), &Gcf_BKData, sizeof(GCF_BAKDATA), hsFileHandle);

        if ( Gcf_Backup.usSeqNO != Gcf_ReqBak.usSeqNO )  {
            sStatus = GCF_BACKUP_ERROR;
            break;
        }

        if ( usRcvLen == sizeof(GCF_BACKUP) ) {
            usInit = 0;
            Gcf_WriteFileFH(SERTMPFILE_DATASTART + sizeof(GCF_BACKUP), &usInit, sizeof(USHORT), hsFileHandle); 
            Gcf_BKData.usDataLen = 0;
        }  else if ( usRcvLen - sizeof(GCF_BACKUP) - 2 != Gcf_BKData.usDataLen) {
            sStatus = GCF_BACKUP_ERROR;
            break;
        }

        if (0 == Gcf_ReqBak.usSeqNO) {
            Gcf_ReadFileFH(SERTMPFILE_DATASTART + sizeof(GCF_BACKUP) + sizeof(Gcf_BKData.usDataLen), &Gcf_FileHed_AsMT, sizeof(GCF_FILEHED), hsFileHandle);

            if (Gcf_FileHed.usMaxGCN != Gcf_FileHed_AsMT.usMaxGCN) {
                sStatus = GCF_BACKUP_ERROR;
                break;
            }
            if (Gcf_FileHed.usSystemtype != Gcf_FileHed_AsMT.usSystemtype) {
                sStatus = GCF_BACKUP_ERROR;
                break;
            }

            Gcf_FileHed.usStartGCN = Gcf_FileHed_AsMT.usStartGCN;
            Gcf_FileHed.usCurDrive[0] = Gcf_FileHed_AsMT.usCurDrive[0];
            Gcf_FileHed.usCurDrive[1] = Gcf_FileHed_AsMT.usCurDrive[1];
            Gcf_FileHed.usCurDrive[2] = Gcf_FileHed_AsMT.usCurDrive[2];
            Gcf_FileHed.usCurDrive[3] = Gcf_FileHed_AsMT.usCurDrive[3];
            Gcf_FileHed.usCurPayGC = Gcf_FileHed_AsMT.usCurPayGC;
            Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

            if (Gcf_BKData.usDataLen == (sizeof(GCF_FILEHED) + Gcf_FileHed.usMaxGCN * (GCF_MAX_QUEUE+1))) {
				USHORT  usNoBytes = (USHORT)(Gcf_FileHed.usMaxGCN * sizeof(GCNUM));
				ULONG   ulBaseOffset = SERTMPFILE_DATASTART + sizeof(GCF_BACKUP) + sizeof(Gcf_BKData.usDataLen) + sizeof(GCF_FILEHED);

                for (i = 0; i < GCF_MAX_QUEUE; i++) {
                    Gcf_ReadFileFH(ulBaseOffset + (Gcf_FileHed.usMaxGCN * i), auchWorkBuffer, usNoBytes, hsFileHandle);

                    Gcf_WriteFile(Gcf_FileHed.offulDrvNOFile[i], auchWorkBuffer, usNoBytes);
                }
                Gcf_ReadFileFH(ulBaseOffset + (Gcf_FileHed.usMaxGCN * GCF_MAX_QUEUE), auchWorkBuffer, usNoBytes, hsFileHandle);

                Gcf_WriteFile(Gcf_FileHed.offulPayTranNO, auchWorkBuffer, usNoBytes);
            }

            if (0 == Gcf_FileHed_AsMT.usCurGCN) {
				NHPOS_NONASSERT_NOTE("==STATE", "==STATE: End GusReqBackUpFH().");
                Gcf_CloseFileReleaseSem();
                return(GCF_SUCCESS);
            }
        } else {
            Gcf_Backup.usType |= GCF_BACKUP_STORE_FILE; /* Set backup copy flag */
            sStatus = Gcf_StoreDataFH(&Gcf_FileHed, Gcf_Backup.usType, Gcf_Backup.usGCNumber, hsFileHandle,
                                      SERTMPFILE_DATASTART + sizeof(GCF_BACKUP) + sizeof(Gcf_BKData.usDataLen), Gcf_BKData.usDataLen, GCF_COUNTER_TYPE); 
            if ( 0 > sStatus ) {
                break;
            }
        }

        Gcf_ReqBak.usSeqNO++ ;
        if ( Gcf_ReqBak.usSeqNO > Gcf_FileHed_AsMT.usCurGCN ) {
            break;
        }
    }     

    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

	NHPOS_NONASSERT_NOTE("==STATE", "==STATE: End GusReqBackUpFH().");

    Gcf_CloseFileReleaseSem();
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:   SHORT   GusResBackUpFH(UCHAR  *puchRcvData,
*                                        USHORT usRcvLen,
*                                        SHORT  hsFileHandle,
*                                        USHORT usOffset,
*                                        USHORT *pusSndLen)
*
*       Input:    UCHAR  *puchRcvData       Pointer to Request Data.
*                 USHORT usOffset           Offset to be written
*                 USHORT usRcvLen           Width of puchRcvData
*                 
*      Output:    SHORT  hsFileHandle       File handle to be written 
*
*       InOut:    USHORT *pusSndLen         Width of puchSndData
*                 
**  Return   :    Normal End  :     GCF_SUCCESS
*                 Abnormal End:     GCF_FATAL
*                                   GCF_FILE_NOT_FOUND
**  Description:
*               RESPONSE OF BACK UP.(File Handle I/F Version)              
*==========================================================================
*/
SHORT   GusResBackUpFH(UCHAR  *puchRcvData,
                       USHORT usRcvLen,
                       SHORT  hsFileHandle,
                       USHORT usOffset,
                       USHORT *pusSndLen)
{
    GCF_BACKUP          *pReq_Backup;
    GCF_BACKUP          *pGcf_Backup = 0;   /* decl'd for sizeof() only */
    GCF_BAKDATA         *pGcf_BKData = 0;   /* decl'd for sizeof() only */
    GCF_FILEHED         Gcf_FileHed;
    GCF_INDEXFILE       Gcf_IndexFile;
    USHORT              usCopyBlock;
    SHORT               sStatus;
	SHORT				i;
	GCNUM               auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};   
    ULONG				ulBytesRead;//RPH SR# 201

	if (hsFileHandle < 0) {
		PifLog (MODULE_GCF, LOG_ERROR_GCF_BACKUPFH_FH_HANDLE_2);
		return GCF_FATAL;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    pReq_Backup = (GCF_BACKUP *)puchRcvData;

    if (usRcvLen != sizeof(GCF_BACKUP)) {
        Gcf_CloseFileReleaseSem();
        PifLog(MODULE_GCF, LOG_ERROR_GCF_BACKUP_SIZE_ERR01);
        return(GCF_FATAL);
    }

    if (0 == pReq_Backup->usSeqNO) {   /* offset = 0 */

        if (*pusSndLen < sizeof(Gcf_FileHed)) {
            Gcf_CloseFileReleaseSem();
			PifLog(MODULE_GCF, LOG_ERROR_GCF_BACKUP_SIZE_ERR02);
            return(GCF_FATAL);
        }

        Gcf_WriteFileFH(usOffset + sizeof(GCF_BACKUP) + sizeof(pGcf_BKData->usDataLen), &Gcf_FileHed, sizeof(GCF_FILEHED), hsFileHandle); 

        sStatus = 0;
        if (FLEX_STORE_RECALL == Gcf_FileHed.usSystemtype) {
            for (i=0; i < GCF_MAX_QUEUE; i++) {
                Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[i], auchWorkBuffer, Gcf_FileHed.usMaxGCN * sizeof(GCNUM), GCF_SEEK_READ);

                Gcf_WriteFileFH(usOffset + sizeof(GCF_BACKUP) + sizeof(pGcf_BKData->usDataLen)+ sizeof(GCF_FILEHED)+ (Gcf_FileHed.usMaxGCN * i),
                                auchWorkBuffer, GCF_MAX_DRIVE_NUMBER * sizeof(GCNUM), hsFileHandle);
            }
            Gcf_ReadFile(Gcf_FileHed.offulPayTranNO, auchWorkBuffer, Gcf_FileHed.usMaxGCN * sizeof(GCNUM), GCF_SEEK_READ);

            Gcf_WriteFileFH(usOffset + sizeof(GCF_BACKUP) + sizeof(pGcf_BKData->usDataLen)+ sizeof(GCF_FILEHED)+ (Gcf_FileHed.usMaxGCN * GCF_MAX_QUEUE),
                            auchWorkBuffer, GCF_MAX_DRIVE_NUMBER * sizeof(GCNUM), hsFileHandle);
            sStatus = Gcf_FileHed.usMaxGCN * (GCF_MAX_QUEUE +1);
        }

        sStatus += sizeof(GCF_FILEHED);

    } else {

        Gcf_ReadFile(Gcf_FileHed.offulGCIndFile + ((pReq_Backup->usSeqNO - 1) * sizeof(GCF_INDEXFILE)), &Gcf_IndexFile, sizeof(GCF_INDEXFILE), GCF_SEEK_READ);

        *pusSndLen -= sizeof(GCF_BACKUP) + 2;

        Gcf_WriteFileFH(usOffset + sizeof(pGcf_Backup->usSeqNO), (USHORT *)&Gcf_IndexFile.fbContFlag, sizeof(USHORT), hsFileHandle); 
        Gcf_WriteFileFH(usOffset + sizeof(pGcf_Backup->usSeqNO) + sizeof(pGcf_Backup->usType), &Gcf_IndexFile.usGCNO, sizeof(USHORT), hsFileHandle);

        sStatus = Gcf_DataBlockCopyFH(&Gcf_FileHed, Gcf_IndexFile.offusBlockNo, hsFileHandle, (USHORT)(usOffset + sizeof(GCF_BACKUP) + sizeof(pGcf_BKData->usDataLen)),
                                      *pusSndLen, &usCopyBlock, &ulBytesRead);
    }

    *pusSndLen = sizeof(GCF_BACKUP);

    Gcf_WriteFileFH(usOffset, &pReq_Backup->usSeqNO, sizeof(USHORT), hsFileHandle);

    if ( 0 != sStatus ) {
        *pusSndLen += (USHORT)sStatus + 2 ;
        Gcf_WriteFileFH(usOffset + sizeof(GCF_BACKUP), ( USHORT *)&sStatus, sizeof(USHORT), hsFileHandle);
    }

    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}

/**************************** Add R3.0 ********************/
/*
*==========================================================================
**    Synopsis:   SHORT GusDeleteDeliveryQueue(UCHAR  uchOrderNo,
*                                              USHORT usType);
*   
*
*       Input:    UCHAR  uchOrderNo,         Guest Check No.
*                 USHORT usType              Queue Number.
*
*      Output:    Nothing
* 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Delete GC from delivery queue
*==========================================================================
*/
SHORT GusDeleteDeliveryQueue(GCNUM  uchOrderNo,
                             USHORT usType)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    SHORT           sStatus;
    ULONG           offulIndexPoint;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }
    /* Check guest Check No */
    if (0 == uchOrderNo) {   /* usGCNumber = 0 */
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    Gcf_RemoveQueDrive(&Gcf_FileHed, uchOrderNo, (USHORT)(usType - GCF_DRIVE_TH_DELIVERY1 + GCF_DELIVERY_1));

    /* Search GCNO in Index.  If found, then update its status */
    Gcf_IndexFile.usGCNO = uchOrderNo;
    
    sStatus =  Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint);
	if (sStatus == GCF_SUCCESS) {
        Gcf_IndexFile.fbContFlag &= ~GCF_DRIVE_THROUGH_MASK;
        Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));
    }

    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusInformQueue(USHORT  usQueue,
*                                      USHORT  usType,
*                                      UCHAR   uchOrderNo,
*                                      UCHAR   uchForBack);
*   
*
*       Input:    USHORT usQueue             Queue Number.
*                 USHORT usType              Type 
*                 UCHAR  uchOrderNo          GCF Number.
*                 UCHAR  uchForBack          Forward or Backward.
*
*      Output:    Nothing
* 
*       InOut:    Nothing
*
**  Return   :    Normal End  : Order Number or guest check number
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Read guest check queue returning a guest check number found
*               in the specified queue that is either to the right of the
*               specified guest check number (next item in the drive thru
*               queue), uchForBack = GCF_FORWARD, or to the left of the
*               specified guest check number (previous item in the drive
*               thru queue), uchForBack = GCF_BACKWARD.
*==========================================================================
*/
SHORT GusInformQueue(USHORT  usQueue,
                     USHORT  usType,
                     GCNUM   uchOrderNo,
                     UCHAR   uchForBack)
{
    GCF_FILEHED     Gcf_FileHed;
	GCNUM           auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};
    SHORT           sStatus, i;
    ULONG           offulIndexPoint;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	NHPOS_ASSERT(usQueue >= GCF_PAYMENT_QUEUE1);

    if (GCF_PAYMENT_QUEUE1 > usQueue || 0 == Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[usQueue- GCF_PAYMENT_QUEUE1], auchWorkBuffer, Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1] * sizeof(GCNUM), GCF_SEEK_READ);

    sStatus = GCF_NOT_IN;    
    i = 0;                                  /* clear counter */
    if (0 == uchOrderNo) {
        uchForBack = GCF_FORWARD;           /* set forward */
    } else {
		// Look for the specified transaction number in the drive thru queue.
		// If not found then we will close the Guest Check File and return GCF_NOT_IN.
		// If it is in the queue then we will look to see what is next to the found
		// item going either to the right to the next one in the queue, GCF_FORWARD,
		// or to the left to the previous one in the queue, GCF_BACKWARD.
        for (; i < (SHORT)Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]; i++) {
            if (uchOrderNo == auchWorkBuffer[i]) {
                if (uchForBack == GCF_BACKWARD) {
                    i--;
                } else {
                    i++;
                }
                break;
            }
        }
        if ( i < 0 || i >= (SHORT)Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]) {
            Gcf_CloseFileReleaseSem();
            return(GCF_NOT_IN);
        }
    }

    if (usType == GCF_STORE_PAYMENT) {
        sStatus = auchWorkBuffer[i];
    }
	else {
		for (;;) {
			GCF_INDEXFILE   Gcf_IndexFile = {0};

			Gcf_IndexFile.usGCNO = auchWorkBuffer[i];
			if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
				break;
			}
			if ((usType == GCF_STORE) && !(Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK)) {
				sStatus = auchWorkBuffer[i];
				break;
			} else if ((usType == GCF_PAYMENT) && (Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK)) {
				sStatus = auchWorkBuffer[i];
				break;
			}
			if (uchForBack == GCF_FORWARD) {
				i++;
				if (i >= (SHORT)Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]) {
					break;
				}
			} else {
				i--;
				if (i < 0) {
					break;
				}
			}
		}
	}
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusRetrieveFirstQueue(USHORT  usQueue,
*                                      USHORT  usType,
*                                      GCNUM   *usOrderNo);
*   
*
*       Input:    USHORT usQueue             Queue Number.
*                 USHORT usType              Type 
*                 GCNUM  uchOrderNo[2]       First two GCF Numbers
*
*      Output:    Nothing
* 
*       InOut:    Nothing
*
**  Return   :    Normal End  : Number items returned.
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Read guest check queue returning a up to two guest check numbers
*               with the numbers being the first items in the queue..
*==========================================================================
*/
SHORT GusRetrieveFirstQueue(USHORT  usQueue,
                     USHORT  usType,
                     GCNUM   *uchOrderNo)
{
    GCF_FILEHED     Gcf_FileHed;
	GCNUM           auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};
    SHORT           sStatus = GCF_NOT_IN;

	uchOrderNo[0] = 0;
	uchOrderNo[1] = 0;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	NHPOS_ASSERT(usQueue >= GCF_PAYMENT_QUEUE1);

    if (GCF_PAYMENT_QUEUE1 > usQueue || 0 == Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[usQueue - GCF_PAYMENT_QUEUE1], auchWorkBuffer, Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1] * sizeof(GCNUM), GCF_SEEK_READ);

	if (usType == GCF_STORE_PAYMENT) {
		uchOrderNo[0] = auchWorkBuffer[0];
		sStatus = 1;
		if (Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1] > 1) {
			uchOrderNo[1] = auchWorkBuffer[1];
			sStatus = 2;
		}
	}
	else {
	    SHORT   i = 0;

		sStatus = 0;
		for (i = 0; i < Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]; i++) {
			ULONG           offulIndexPoint;
			GCF_INDEXFILE   Gcf_IndexFile = {0};

			Gcf_IndexFile.usGCNO = auchWorkBuffer[i];
			if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
				break;
			}
			if ((usType == GCF_STORE) && !(Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK)) {
				uchOrderNo[sStatus] = auchWorkBuffer[i];
				sStatus++;
				if (sStatus >= 2)
					break;
			} else if ((usType == GCF_PAYMENT) && (Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK)) {
				uchOrderNo[sStatus] = auchWorkBuffer[i];
				sStatus++;
				if (sStatus >= 2)
					break;
			}
		}
        sStatus = (sStatus < 1) ? GCF_NOT_IN : sStatus;
	}
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusSearchQueue(USHORT  usQueue,
*                                      UCHAR   uchMiddleNo,
*                                      UCHAR   uchRightNo);
*   
*
*       Input:    USHORT usQueue             Queue Number.
*                 UCHAR  uchMiddleNo,        Middle Screen Guest Check No.
*                 UCHAR  uchRightNo,         Right Screen Guest Check No.
*
*      Output:    Nothing
* 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*               Search GC from  queue
*==========================================================================
*/
SHORT GusSearchQueue(USHORT usQueue, GCNUM  usMiddleNo, GCNUM usRightNo)
{
    GCF_FILEHED     Gcf_FileHed;
    SHORT           sStatus = GCF_SUCCESS;
	GCNUM   auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};
    ULONG   ulCount;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	NHPOS_ASSERT(usQueue >= GCF_PAYMENT_QUEUE1);

    if (GCF_PAYMENT_QUEUE1 > usQueue) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

	NHPOS_ASSERT(Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1] * sizeof(GCNUM) <= sizeof(auchWorkBuffer));

    Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[usQueue - GCF_PAYMENT_QUEUE1], auchWorkBuffer, Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1] * sizeof(GCNUM), GCF_SEEK_READ);
    
    for ( ulCount = 0; ulCount < Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]; ulCount++) {
        if ( auchWorkBuffer[ulCount] == usMiddleNo ) {
            break;
        }
    }
    if (ulCount == Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    if (ulCount + 1 == Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1]) {
        if ( usRightNo != 0) {
            sStatus = GCF_NOT_IN;
        }
        Gcf_CloseFileReleaseSem();
        return(sStatus);
    }
    if ( auchWorkBuffer[ulCount+1] != usRightNo ) {
        sStatus = GCF_NOT_IN;
    }
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusCheckAndReadFH(GCNUM  usGCNumber,
*                                         USHORT usSize,
*                                         SHORT  hsFileHandle,
*                                         USHORT usStartPoint, 
*                                         USHORT usFileSize);
*   
*
*       Input:    GCNUM  usGCNumber,         Guest Check No.
*                 USHORT usSize              Previous file size.
*                 SHORT  hsFileHandle        file handle. 
*                 USHORT usStartPoint        Start pointer to be stored 
*                 USHORT usFileSize          Width of the file.
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_NOT_IN
*
**  Description:
*    Check size and read guest check data indvidual(File Handle I/F Version).
*==========================================================================
*/
SHORT GusCheckAndReadFH(GCNUM     usGCNumber,
                        USHORT     usSize,
                        SHORT      hsFileHandle,
                        USHORT     usStartPoint,
                        USHORT     usFileSize,
						ULONG	   *pulBytesRead)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    SHORT           sStatus;
    ULONG           offulIndexPoint;
    USHORT          usCopyBlock;
	ULONG           offulBlockPoint;
    GCF_DATAHEAD    GcfDataHead;

	*pulBytesRead = 0;

	if (hsFileHandle < 0) {
		PifLog (MODULE_GCF, LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_4);
		return GCF_FATAL;
	}

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    Gcf_IndexFile.usGCNO = usGCNumber;
    sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint);
	if (sStatus == GCF_SUCCESS) {

		if (0 == Gcf_IndexFile.offusBlockNo) {
			Gcf_CloseFileReleaseSem();
			return(0);
		}

		offulBlockPoint = Gcf_FileHed.offulGCDataFile + ((Gcf_IndexFile.offusBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
		Gcf_ReadFile(offulBlockPoint, &GcfDataHead, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

		if (GcfDataHead.usDataLen == usSize){
			Gcf_CloseFileReleaseSem();
			return(GCF_EXIST);                 
		}

		sStatus = Gcf_DataBlockCopyFH(&Gcf_FileHed, Gcf_IndexFile.offusBlockNo, hsFileHandle, usStartPoint,
									usFileSize, &usCopyBlock, pulBytesRead);
    }
    Gcf_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusResetDeliveryQueue(VOID);
*   
*
*       Input:    Noting
*
*      Output:    Nothing
* 
*       InOut:    Nothing
*
**  Return   :    Normal End  : GCF_SUCCESS
*                 Abnormal End: GCF_ALL_LOCKED
*
**  Description:
*               Reset delivery queue
*==========================================================================
*/
SHORT GusResetDeliveryQueue(VOID)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    SHORT           sStatus, i;
	GCNUM           auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};
    ULONG           offulIndexPoint;

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    for (i = GCF_DELIVERY_1; i <= GCF_DELIVERY_2; i++) {
		SHORT j;

		NHPOS_ASSERT(Gcf_FileHed.usCurDrive[i] * sizeof(GCNUM) <= sizeof(auchWorkBuffer));
        Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[i], auchWorkBuffer, Gcf_FileHed.usCurDrive[i] * sizeof(GCNUM), GCF_SEEK_READ);
    
        for (j = 0; j < (SHORT)Gcf_FileHed.usCurDrive[i];) {
            Gcf_IndexFile.usGCNO = auchWorkBuffer[j];
    
            /* Search GCNO in Index */
            sStatus = Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint);

            if (sStatus == GCF_SUCCESS) {
                if (!(Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK)) {
                    /* Check Paid Transaction */
                    j++;
                    continue;
                }

                Gcf_IndexFile.fbContFlag &= ~GCF_DRIVE_THROUGH_MASK; /* Reset DRIVE flag */
                Gcf_WriteFile(offulIndexPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));       /* Write Index File */

				if ((Gcf_FileHed.usCurDrive[i] - j - 1) != 0 ) {
					Gcf_memmove(&auchWorkBuffer[j], &auchWorkBuffer[j+1], Gcf_FileHed.usCurDrive[i] - j - 1);     /* move queue */
				}

				if (Gcf_FileHed.usCurDrive[i] > 0) {
					Gcf_FileHed.usCurDrive[i]--;
				}
            }
        }
        Gcf_WriteFile(Gcf_FileHed.offulDrvNOFile[i], auchWorkBuffer,
                      (USHORT)(Gcf_FileHed.usCurDrive[i]  * sizeof(GCNUM)));   /* write queue */
    }

    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

    Gcf_CloseFileReleaseSem();
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT GusDeliveryIndRead(USHORT usQueue,
*                                          UCHAR  *puchRcvBuffer,
*                                          USHORT usSize,
*                                          UCHAR  *puchOrder);
*   
*
*       Input:    USHORT usQueue,            Queue No.
*                 USHORT usSize              Width of the file.
*      Output:    UCHAR  *puchRcvBuffer      Store Data Buffer. 
*                 UCHAR  *puchOrder          Order No. Area.
*       InOut:    Nothing
*
**  Return   :    Normal End  : length of guest check data
*                 Abnormal End: GCF_NOT_IN
*                 Abnormal End: GCF_NOT_PAID
*                 Abnormal End: GCF_CHANG_QUEUE
*
**  Description:
*               Read guest check data indvidual for Delivery Queue.
*==========================================================================
*/
SHORT GusDeliveryIndRead(USHORT     usQueue,
                         UCHAR      *puchRcvBuffer,
                         USHORT     usSize,
                         USHORT      *puchOrder)
{
    GCF_FILEHED     Gcf_FileHed;
    GCF_INDEXFILE   Gcf_IndexFile;
    SHORT            sStatus, sFlag = 0;
    ULONG           offulIndexPoint;
    USHORT          i, usLen, usCopyBlock;
	GCNUM			usGCNumber;
	GCNUM           auchWorkBuffer[GCF_MAX_DRIVE_NUMBER] = {0};

	NHPOS_ASSERT(usQueue >= GCF_PAYMENT_QUEUE1);

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_NOT_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

    if (GCF_PAYMENT_QUEUE1 > usQueue || 0 == Gcf_FileHed.usCurDrive[usQueue-GCF_PAYMENT_QUEUE1]) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

	NHPOS_ASSERT(Gcf_FileHed.usCurDrive[usQueue - GCF_PAYMENT_QUEUE1] * sizeof(GCNUM) <= sizeof(auchWorkBuffer));

    Gcf_ReadFile(Gcf_FileHed.offulDrvNOFile[usQueue-GCF_PAYMENT_QUEUE1], auchWorkBuffer, Gcf_FileHed.usCurDrive[usQueue-GCF_PAYMENT_QUEUE1] * sizeof(GCNUM), GCF_SEEK_READ);
    
    Gcf_IndexFile.usGCNO = usGCNumber = (USHORT)auchWorkBuffer[0];
    if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_NOT_IN) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN);
    }

    /* Check Paid Transaction */
    if ((Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) != 0) {
        usLen = Gcf_DataBlockCopy(&Gcf_FileHed, Gcf_IndexFile.offusBlockNo, puchRcvBuffer, usSize, &usCopyBlock);
        Gcf_CloseFileReleaseSem();
        return(usLen);
    }

    for (i = 1; i != Gcf_FileHed.usCurDrive[usQueue-GCF_PAYMENT_QUEUE1]; i++) {
        Gcf_IndexFile.usGCNO = (USHORT)auchWorkBuffer[i];
        if (Gcf_Index(&Gcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) != GCF_NOT_IN) {
            if (Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) {
                sFlag = 1;
                break;
            }
        }
    }

    if (sFlag == 0) {
        Gcf_CloseFileReleaseSem();
        return(GCF_NOT_IN_PAID);
    }

    Gcf_RemoveQueDrive(&Gcf_FileHed, usGCNumber, (USHORT)(usQueue - GCF_PAYMENT_QUEUE1));
    Gcf_QueDrive(&Gcf_FileHed, usGCNumber, (USHORT)(usQueue - GCF_PAYMENT_QUEUE1));
    *puchOrder = auchWorkBuffer[1];
    if (Gcf_FileHed.usCurDrive[usQueue-GCF_PAYMENT_QUEUE1] == 2) {
        *(puchOrder + 1) = auchWorkBuffer[0];
    } else {
        *(puchOrder + 1) = auchWorkBuffer[2];
    }
    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

    Gcf_CloseFileReleaseSem();
    return(GCF_CHANGE_QUEUE);
}


SHORT GusWalkGcfDataFile (CLIGUS_TALLY_HEAD *pTallyHead, CLIGUS_TALLY_INDEX *pTallyArray, USHORT *nArrayItems)
{
	SHORT            sStatus;
	GCF_FILEHED      Gcf_FileHed;
	USHORT           usCount = 0;

	memset (pTallyHead, 0, sizeof(CLIGUS_TALLY_HEAD));

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	if (pTallyArray && nArrayItems && *nArrayItems > 0)
	{
		GCF_TALLY_INDEX  *pGcfTallyArray = 0;
		USHORT           usTemp1;

		pGcfTallyArray = (GCF_TALLY_INDEX *)_alloca (sizeof(GCF_TALLY_INDEX) * (*nArrayItems));
		usCount = Gcf_WalkGcfDataFile (&Gcf_FileHed, pGcfTallyArray, *nArrayItems);

		for (usTemp1 = 0; usTemp1 < usCount; usTemp1++)
		{
			pTallyArray[usTemp1].usGCNO = pGcfTallyArray[usTemp1].gcf_Index.usGCNO;
			pTallyArray[usTemp1].fbContFlag = pGcfTallyArray[usTemp1].gcf_Index.fbContFlag;
			pTallyArray[usTemp1].usNoBlocksInUse = pGcfTallyArray[usTemp1].usNoBlocksInUse;
			pTallyArray[usTemp1].usNoBlocksReserve = pGcfTallyArray[usTemp1].usNoBlocksReserve;
			pTallyArray[usTemp1].usNoFragsInUse = pGcfTallyArray[usTemp1].usNoFragsInUse;
			pTallyArray[usTemp1].usNoFragsReserve = pGcfTallyArray[usTemp1].usNoFragsReserve;
			pTallyArray[usTemp1].usWidestInUse = pGcfTallyArray[usTemp1].usWidestInUse;
			pTallyArray[usTemp1].usWidestReserve = pGcfTallyArray[usTemp1].usWidestReserve;
		}
	}

    Gcf_CloseFileReleaseSem();

	pTallyHead->ulGCFSize = Gcf_FileHed.ulGCFSize;
	pTallyHead->usSystemtype = Gcf_FileHed.usSystemtype;
	pTallyHead->usStartGCN = Gcf_FileHed.usStartGCN;
	pTallyHead->usMaxGCN = Gcf_FileHed.usMaxGCN;
	pTallyHead->usCurGCN = Gcf_FileHed.usCurGCN;
	pTallyHead->usCurNoPayGCN = Gcf_FileHed.usCurNoPayGCN;
	pTallyHead->usCurPayGC = Gcf_FileHed.usCurPayGC;

	*nArrayItems = usCount;

	return GCF_SUCCESS;
}

SHORT GusOptimizeGcfDataFile (CLIGUS_TALLY_HEAD *pTallyHead)
{
	USHORT           usCount = 0;
	USHORT           nArrayItems = 1000;
	SHORT            sStatus;
	GCF_TALLY_INDEX  *pGcfTallyArray = 0;
	GCF_FILEHED      Gcf_FileHed;

	memset (pTallyHead, 0, sizeof(CLIGUS_TALLY_HEAD));

    sStatus = Gcf_ReqSemOpenFileGetHeadChkLck(&Gcf_FileHed, GCF_LOCKCHECK);
    if (sStatus != GCF_SUCCESS) {
        return(sStatus);
    }

	pGcfTallyArray = (GCF_TALLY_INDEX *)_alloca (sizeof(GCF_TALLY_INDEX) * nArrayItems);
	usCount = Gcf_OptimizeGcfDataFile (&Gcf_FileHed, pGcfTallyArray, nArrayItems, 1);

    /* Write File Header */
    Gcf_WriteFile(GCF_FILE_HEAD_POSITION, &Gcf_FileHed, sizeof(GCF_FILEHED));

    Gcf_CloseFileReleaseSem();

	pTallyHead->ulGCFSize = Gcf_FileHed.ulGCFSize;
	pTallyHead->usSystemtype = Gcf_FileHed.usSystemtype;
	pTallyHead->usStartGCN = Gcf_FileHed.usStartGCN;
	pTallyHead->usMaxGCN = Gcf_FileHed.usMaxGCN;
	pTallyHead->usCurGCN = Gcf_FileHed.usCurGCN;
	pTallyHead->usCurNoPayGCN = Gcf_FileHed.usCurNoPayGCN;
	pTallyHead->usCurPayGC = Gcf_FileHed.usCurPayGC;

	return GCF_SUCCESS;
}
/**************************** End Add R3.0 ********************/
/*========= End of File =========*/