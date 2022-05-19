/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Guest Check File interface                                                
*   Program Name       : CSSTBGCF.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    May-21-92      :H.NAKASHIMA        :Initial
*    Apr-21-95      :hkato              :R3.0
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
/*
===========================================================================
    PROTOTYPE
===========================================================================
*/
#include "csgcs.h"

typedef struct {
	GCNUM    usGCNO;                 /* Guest Check No */
	USHORT   fbContFlag;             /* Control Flag */
	USHORT   usNoBlocksInUse;
	USHORT   usNoBlocksReserve;
	USHORT   usNoFragsInUse;
	USHORT   usNoFragsReserve;
	USHORT   usWidestInUse;
	USHORT   usWidestReserve;
} CLIGUS_TALLY_INDEX;

typedef struct {
	ULONG    ulGCFSize;          /* Number of Bytes in File */
	USHORT   usSystemtype;       /* System Type 0 ~ 4. That is setted by Flexible memory assignment */
	GCNUM    usStartGCN;         /* Start Guest Check No in Auto Genaration system use. */
	GCNUM    usMaxGCN;           /* Maximum number of GC, That is setted by Flexible memory assignment */
	GCNUM    usCurGCN;           /* Current number of GC to be assignment */
	GCNUM    usCurNoPayGCN;      /* Current number of Non Pay GC to be assignment  Add R3.0 */
	GCNUM    usCurPayGC;         /* Current number of Payment Transaction Chg R3.0 */
	USHORT   usGlobalUnPaidCount;
	USHORT   usGlobalBlocksCountInUse;
	USHORT   usGlobalBlocksCountFree;
} CLIGUS_TALLY_HEAD;

/*---------------------------------------
    USER I/F G.C.F FUNCTION (CLIENT)
----------------------------------------*/

SHORT   CliGusManAssignNo(GCNUM usGCNumber);
SHORT   CliGusStoreFile(USHORT usType, GCNUM usGCNumber, UCHAR *puchSndBuffer, USHORT usSize);  
SHORT    CliGusReadLock(GCNUM usGCNumber, UCHAR *puchRcvBuffer, USHORT usSize, USHORT usType);
SHORT    CliGusDelete(GCNUM usGCNumber);
int      CliGusLoadTenderTableLookupAndCheckRules (VOID *pItemTender);
SHORT    CliGusLoadTenderTableConvertError (SHORT sGusRetValue);
SHORT    CliGusLoadFromConnectionEngine (TCHAR  *uchUniqueIdentifier);
SHORT    CliGusLoadPaidTransaction (GCNUM  usGCNumber, GCNUM usNewGCNumber);
SHORT    CliGusGetAutoNo(GCNUM *pusGCNumber);
SHORT    CliGusAllIdRead(USHORT usType, USHORT *pusRcvBuffer, USHORT usSize);
SHORT    CliGusResetReadFlag(GCNUM usGCNumber, USHORT usType);
SHORT    CliGusIndRead(GCNUM usGCNumber, UCHAR *puchRcvBuffer, USHORT usSize);
SHORT   CliGusStoreFileFH(USHORT usType,
                          GCNUM usGCNumber,
                          SHORT  hsFileHandle,
                          ULONG  ulStartPoint,
                          ULONG ulSize);
SHORT   CliGusReadLockFH(GCNUM usGCNumber, 
                         SHORT  hsFileHandle, 
                         USHORT usStartPoint, 
                         USHORT usSize,
                         USHORT usType,
						 ULONG	*pulActualBytesRead);                    /* R3.0 */
SHORT	CliGusReturnsLockClear(GCNUM usGCNumber);
SHORT   CliGusIndReadFH(GCNUM usGCNumber, SHORT  hsFileHandle, USHORT usStartPoint, USHORT usSize, ULONG *pulActualBytesRead);
SHORT   CliGusDeleteDeliveryQueue(GCNUM  uchOrderNo, USHORT usType);
SHORT   CliGusInformQueue(USHORT   usQueue, USHORT  usType, GCNUM   uchOrderNo, UCHAR   uchForBack);
SHORT   CliGusSearchQueue(USHORT usQueue, USHORT  uchMiddleNo, USHORT uchRightNo);
SHORT   CliGusRetrieveFirstQueue(USHORT usQueue, USHORT  usType, GCNUM   *uchOrderNo);
SHORT   CliGusCheckAndReadFH(GCNUM     usGCNumber,          /* R3.0 */
                             USHORT     usSize,
                             SHORT      hsFileHandle,
                             USHORT     usStartPoint,
                             USHORT     usFileSize,
							 ULONG		*pulActualBytesRead);
SHORT   CliGusDeliveryIndRead(USHORT usQueue, UCHAR *puchRcvBuffer, USHORT usSize, USHORT *puchOrder);

SHORT   GusWalkGcfDataFile (CLIGUS_TALLY_HEAD *pTallyHead, CLIGUS_TALLY_INDEX *pTallyArray, USHORT *nArrayItems);

SHORT    CliGusAllIdReadBW(USHORT usType, ULONG ulWaiter, USHORT *pusRcvBuffer, USHORT usSize);
SHORT    CliGusAllIdReadFiltered (USHORT usType, ULONG ulWaiterNo, USHORT usTableNo, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount);
SHORT    CliGusSearchForBarCode (USHORT usType, UCHAR *uchUniqueIdentifier, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount);
//PDINU
SHORT GusOptimizeGcfDataFile (CLIGUS_TALLY_HEAD *pTallyHead);

/*---------------------------------------
    USER I/F G.C.F FUNCTION (SERVER)
----------------------------------------*/

SHORT    SerGusReadLock(GCNUM usGCNumber,
                        UCHAR *puchRcvBuffer, 
                        USHORT usSize,
                        USHORT usType);                     /* R3.0 */
SHORT    SerGusDelete(GCNUM usGCNumber);
SHORT    SerGusIndRead(GCNUM usGCNumber,
                        UCHAR *puchRcvBuffer, 
                        USHORT usSize);
SHORT    SerGusAllIdRead(USHORT usType,
                        USHORT *pusRcvBuffer,
                        USHORT usSize);
SHORT    SerGusAllLock(VOID);
SHORT    SerGusAllUnLock(VOID);
SHORT    SerGusAssignNo(GCNUM usGCAssignNo);
SHORT    SerGusResetReadFlag(GCNUM usGCNumber);
SHORT    SerGusCheckExist(VOID);
SHORT    SerGusAllIdReadBW(USHORT usType, ULONG  ulWaiterNo, USHORT *pusRcvBuffer, USHORT usSize);
SHORT	SerGusReadAllLockedGCN(USHORT *pusRcvBuffer, USHORT usSize);
SHORT   SerGusResetDeliveryQueue(VOID);                          /* R3.0 */

/*===== END OF SOURCE =====*/
