/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Guest Check File Source File                        
* Category    : Client/Server STUB, US Hospitality Model
* Program Name: CSSTBGCF.C                                            
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               CliGusManAssignNo()         record specified GCN
*#              CliGusStoreFile()           store GCF
*#              CliGusReadLock()            read & lock GCF       R3.0
*               CliGusDelete()              delete GCF
*               CliGusGetAutoNo()           get auto generation GCN
*               CliGusAllIdRead()           read all GCN
*               CliGusResetReadFlag()       reset read flag for GCF
*#              CliGusIndRead()             individual read GCF
*               CliGusInformQueue()         inform queue data,    R3.0
*               CliGusDeleteDeliveryQueue() delete delivery queue data,  R3.0
*
*#              SerGusReadLock()            read & lock GCF       R3.0
*               SerGusDelete()              delete GCF
*               SerGusIndRead()             individual read GCF
*               SerGusAllIdRead()           read all GCN
*               SerGusAllLock()             lock all GCF
*               SerGusAllUnLock()           unlock all GCF
*               SerGusAssignNo()            assign GCN
*               SerGusResetReadFlag()       reset read flag for GCF
*               SerGusCheckExist()          check exist specified GCN
*               SerGusAllIdReadBW()         read all GCN by waiter
*               SerGusReadAllLockedGCN()    read all GCN loked
*
*               CliGusStoreFileFH()         store GCF(file handle I/F version)
*#              CliGusReadLockFH()          read & lock GCF(file handle I/F version)   R3.0
*               CliGusIndReadFH()           individual read GCF(file handle I/F version)
*
*               CstInitGcfMsgH()     Initialize Request/Response Msg Hed
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-08-92:H.Nakashima: initial                                   
* Jul-15-93:H.Yamaguchi: Modified Cli/SerGusReadLock                                   
* Jul-27-93:H.Yamaguchi: Adds CstInitCasMsgH() for reduction                                  
* Mar-15-94:K.You      : Adds flex GC file feature(add CliGusStoreFileFH,
*          :           : CliGusReadLockFH, CliGusIndReadFH). 
* Apr-21-95:hkato      : R3.0
* Aug-08-12:M.Teraki   : R3.5 Remove STORE_RECALL_MODEL/GUEST_CHECK_MODEL
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
#include	<tchar.h>
#include    <stdio.h>
#include    <math.h>
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbpar.h>
#include    "csstubin.h"
#include    "mld.h"
#include	<uie.h>
#include	<uireg.h>
#include	<item.h>
#include    <../item/include/itmlocal.h>
#include	<ConnEngineObjectIf.h>

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;


/*
*===========================================================================
** Synopsis:    VOID   CstInitGcfMsgH(CLIREQGCF  *pReqMsgH, CLIRESGCF  *pResMsgH)
*
*     Inout:    ReqMsgH - Request  Message header
*               ResMsgH - Response Message header
*
** Return:      noting
*
** Description: This function supports to initialize messages.
*===========================================================================
*/
static VOID   CstInitGcfMsgH(CLIREQGCF  *pReqMsgH, CLIRESGCF  *pResMsgH)
{
    memset(pReqMsgH, 0, sizeof(CLIREQGCF));
    memset(pResMsgH, 0, sizeof(CLIRESGCF));
    memset(&CliMsg, 0, sizeof(CLICOMIF));
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusManAssignNo(USHORT usGCNumber);
*     InPut:    usGCNumber  - GC number to assign
*               
** Return:      GCF_SUCCESS:    assigned.
*               GCF_EXIST:      already exist this number
*               GCF_ALL_LOCKED: still locked all GCF	
*
** Description: This function supports to assign Guest Check Number.
*===========================================================================
*/              
SHORT	CliGusManAssignNo(GCNUM usGCNumber)
{   
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    SHORT           sError;
    SHORT           sCliRetCode;
    SHORT           sRetCode = GCF_SUCCESS;
    SHORT           sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;
	CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
           
    CliMsg.usFunCode    = CLI_FCGCFMASSIGN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusManAssignNo(usGCNumber);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = GusManAssignNo(usGCNumber);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusManAssignNo() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

                
/*              
*===========================================================================
** Synopsis:    SHORT   CliGusStoreFile(USHORT usType,
*                                       USHORT usGCNumber,
*                                       UCHAR *puchSendBuffer,
*                                       USHORT usSize);
*     InPut:    usType  - 0:conter type, 1:drive through
*               usGCNumber  - GCN
*               *puchSendBuffer - send buffer pointer to store
*               usSize  - size of buffer
*
** Return:      GCF_SUCCESS:    stored
*				GCF_FILE_FULL:  file full
*               GCF_NOT_LOCKED: has not been read
*               GCF_ALL_LOCKED: during reporting
*
** Description: This function supports to store Guest Check File.
*===========================================================================
*/
/*
SHORT	CliGusStoreFile(USHORT usType,
                        USHORT usGCNumber,
                        UCHAR *puchSendBuffer,
                        USHORT usSize)
{
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError, sBRetCode = 0;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFSTORE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchReqData = puchSendBuffer;
    CliMsg.usReqLen     = usSize;

    if (STUB_SELF == (sError = CstSendMaster())) {
        CliMsg.sRetCode = GusStoreFile( usType, 
                                        usGCNumber, 
                                        puchSendBuffer, 
                                        usSize);
    }
    if (GCF_SUCCESS == CliMsg.sRetCode) {
        if (STUB_SELF == CstSendBMaster()) {
            sBRetCode = GusStoreFile(   usType, 
                                        usGCNumber, 
                                        puchSendBuffer, 
                                        usSize);
            if (STUB_M_DOWN == sError) {
                CliMsg.sRetCode = sBRetCode;
            } else if (sBRetCode < 0) {
//                CstBackUpError();
            }
        }
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}
*/
/*
*===========================================================================
** Synopsis:    SHORT   CliGusReadLock(	USHORT usGCNumber,
*                                       UCHAR *puchRcvBuffer,
*                                       USHORT usSize,
*                                       USHORT usType)            R3.0
*     InPut:    usGCNumber  - GCN
*               usSize  - size of buffer
*    OutPut:    *puchRcvBuffer  - receive buffer pointer to read
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*               GCF_LOCKED: already read
*               GCF_ALL_LOCKED:	during reporting
*
** Description: This function supports to read and to lock Guest Check File.
*===========================================================================
*/
SHORT	CliGusReadLock(GCNUM usGCNumber,
						UCHAR *puchRcvBuffer,
						USHORT usSize,
                        USHORT usType)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError;            
    SHORT       sCliRetCode;
    SHORT       sErrorBM = STUB_SUCCESS, sErrorM = STUB_SUCCESS;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFREADLOCK | CLI_FCWBACKUP | CLI_FCCLIENTWB
                    /*  + CLI_FCWCONFIRM */ ;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusReadLock(usGCNumber, puchRcvBuffer, usSize, usType); 
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SUCCESS == sErrorM) {
        CliMsg.usFunCode = CLI_FCGCFLOCK | CLI_FCWBACKUP | CLI_FCCLIENTWB;
        CliMsg.usResLen = usSize = 0;
    }
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = GusReadLock(usGCNumber, puchRcvBuffer, usSize, usType);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
    }

    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusReadLock() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusDelete(ULONG usGCNumber);
*     InPut:    ulGCNumber  - GCN to delete
*
** Return:      GCF_SUCCESS:    deleted
*               GCF_NOT_IN: GCN not in file			
*
** Description: This function supports to delete Guest Check File.
*===========================================================================
*/
SHORT   CliGusDelete(GCNUM usGCNumber)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError;            
    SHORT       sCliRetCode;
    SHORT       sErrorBM = STUB_SUCCESS, sErrorM = STUB_SUCCESS;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
           
    CliMsg.usFunCode    = CLI_FCGCFDELETE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusDelete(usGCNumber);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = GusDelete(usGCNumber);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusDelete() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

// ----------------------------------------------------
// routines and data structs for Returns/Exchanges/Refunds limits on tenders

#define  GUSTENDER_FLAG_STORED      0x00000001
#define  GUSTENDER_FLAG_FORWARDED   0x00000002
#define  GUSTENDER_FLAG_APPROVED    0x00000004
#define  GUSTENDER_FLAG_PREAUTH     0x00000008
#define  GUSTENDER_FLAG_CAPTURED    0x00000010
#define  GUSTENDER_FLAG_EPT_TENDER  0x00000020
#define  GUSTENDER_FLAG_DECLINED    0x00000040

typedef struct {
		UCHAR   uchTenderYear;
		UCHAR   uchTenderMonth;
		UCHAR   uchTenderDay;
		UCHAR   uchTenderHour;
		UCHAR   uchTenderMinute;
		UCHAR   uchTenderSecond;
} DATE_TIME_ITEMTENDER;

typedef struct {
	ULONG      ulGusTenderFlag;
	DCURRENCY  lTenderAmount;                 // Stored standard Credit
	DCURRENCY  lChange;
	DCURRENCY  lGratuity;
	DATE_TIME_ITEMTENDER  dtStamp;
	TENDERKEYINFO         TenderKeyInfo;
} GusTenderTableEntry;

static struct GusTenderTableStruct {
	ITEMTENDER ItemTender;
	ULONG      ulGusTenderFlag;
	DATE_TIME_ITEMTENDER  dtLatestStamp;
	TENDERKEYINFO         TenderKeyInfo;

	GusTenderTableEntry   StoredStandardCredit;         // Stored standard Credit
	GusTenderTableEntry   StoredPreauthCredit;          // Stored Preauth Credit
	GusTenderTableEntry   StoredCapturedCredit;         // Stored Preauth Capture Credit

	GusTenderTableEntry   ApprovedStandardCredit;       // Stored standard Credit
	GusTenderTableEntry   ApprovedPreauthCredit;        // Stored Preauth Credit
	GusTenderTableEntry   ApprovedCapturedCredit;       // Stored Preauth Capture Credit

	GusTenderTableEntry   DeclinedStandardCredit;       // Stored standard Credit
	GusTenderTableEntry   DeclinedPreauthCredit;        // Stored Preauth Credit
	GusTenderTableEntry   DeclinedCapturedCredit;       // Stored Preauth Capture Credit

} GusLoadTenderTable[50];

static  DATE_TIME  GusTenderBatchCloseDateTime;

SHORT  CliGusLoadTenderTableInit (DATE_TIME *pBatchCloseDateTime)
{
	static DATE_TIME BatchCloseDateTimeSave = {70, 0};  // start with bogus seconds data to trigger first log

	memset (GusLoadTenderTable, 0, sizeof(GusLoadTenderTable));

	if (pBatchCloseDateTime) {
		GusTenderBatchCloseDateTime = *pBatchCloseDateTime;
		GusTenderBatchCloseDateTime.usYear %= 100;      // convert from YYYY form to a YY form to be compatible with tender YY form
	} else
		memset (&GusTenderBatchCloseDateTime, 0, sizeof(GusTenderBatchCloseDateTime));

	if (memcmp (&BatchCloseDateTimeSave, &GusTenderBatchCloseDateTime, sizeof(BatchCloseDateTimeSave)) != 0) {
		char xBuff[128];
		sprintf (xBuff, "CliGusLoadTenderTableInit(): %4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d", GusTenderBatchCloseDateTime.usYear, GusTenderBatchCloseDateTime.usMonth, GusTenderBatchCloseDateTime.usMDay,
				GusTenderBatchCloseDateTime.usHour, GusTenderBatchCloseDateTime.usMin, GusTenderBatchCloseDateTime.usSec);
		NHPOS_NONASSERT_TEXT (xBuff);
		BatchCloseDateTimeSave = GusTenderBatchCloseDateTime;
	}
	return 0;
}

static  void CliGusAssignTenderStampToDateTime (ITEMTENDER *pItemTender, DATE_TIME_ITEMTENDER *pDateTime)
{
	memset (pDateTime, 0, sizeof(DATE_TIME_ITEMTENDER));

	pDateTime->uchTenderYear = pItemTender->uchTenderYear;
	pDateTime->uchTenderMonth = pItemTender->uchTenderMonth;
	pDateTime->uchTenderDay = pItemTender->uchTenderDay;
	pDateTime->uchTenderHour = pItemTender->uchTenderHour;
	pDateTime->uchTenderMinute = pItemTender->uchTenderMinute;
	pDateTime->uchTenderSecond = pItemTender->uchTenderSecond;
}

// We will return negative value if left is less than right
// We will return zero if left equals right
// We will return positive non-zero value if left is greater than right
static SHORT CliGusCompTenderStamp (DATE_TIME_ITEMTENDER *pDateTimeLeft, DATE_TIME_ITEMTENDER *pDateTimeRight)
{
	SHORT  sLeft, sRight, sComp;

	sLeft = pDateTimeLeft->uchTenderYear;
	sRight = pDateTimeRight->uchTenderYear;
	if ((sComp = sLeft - sRight) == 0) {
		sLeft = pDateTimeLeft->uchTenderMonth;
		sRight = pDateTimeRight->uchTenderMonth;
		if ((sComp = sLeft - sRight) == 0) {
			sLeft = pDateTimeLeft->uchTenderDay;
			sRight = pDateTimeRight->uchTenderDay;
			if ((sComp = sLeft - sRight) == 0) {
				sLeft = pDateTimeLeft->uchTenderHour;
				sRight = pDateTimeRight->uchTenderHour;
				if ((sComp = sLeft - sRight) == 0) {
					sLeft = pDateTimeLeft->uchTenderMinute;
					sRight = pDateTimeRight->uchTenderMinute;
					if ((sComp = sLeft - sRight) == 0) {
						sLeft = pDateTimeLeft->uchTenderSecond;
						sRight = pDateTimeRight->uchTenderSecond;
						sComp = sLeft - sRight;
					}
				}
			}
		}
	}

	return sComp;
}

static SHORT CliGusAssignTenderStampToLatest (ITEMTENDER *pItemTender, DATE_TIME_ITEMTENDER *pDateTime)
{
	SHORT  sRetStatus = 0;

	if (pItemTender->uchTenderYear > pDateTime->uchTenderYear) {
		CliGusAssignTenderStampToDateTime (pItemTender, pDateTime);
		sRetStatus = 1;
	} else if (pItemTender->uchTenderYear == pDateTime->uchTenderYear) {
		if (pItemTender->uchTenderMonth > pDateTime->uchTenderMonth) {
			CliGusAssignTenderStampToDateTime (pItemTender, pDateTime);
			sRetStatus = 1;
		} else if (pItemTender->uchTenderMonth == pDateTime->uchTenderMonth) {
			if (pItemTender->uchTenderDay > pDateTime->uchTenderDay) {
				CliGusAssignTenderStampToDateTime (pItemTender, pDateTime);
				sRetStatus = 1;
			} else if (pItemTender->uchTenderDay == pDateTime->uchTenderDay) {
				if (pItemTender->uchTenderHour > pDateTime->uchTenderHour) {
					CliGusAssignTenderStampToDateTime (pItemTender, pDateTime);
					sRetStatus = 1;
				} else if (pItemTender->uchTenderHour == pDateTime->uchTenderHour) {
					if (pItemTender->uchTenderMinute > pDateTime->uchTenderMinute) {
						CliGusAssignTenderStampToDateTime (pItemTender, pDateTime);
						sRetStatus = 1;
					} else if (pItemTender->uchTenderMinute == pDateTime->uchTenderMinute) {
						if (pItemTender->uchTenderSecond > pDateTime->uchTenderSecond) {
							CliGusAssignTenderStampToDateTime (pItemTender, pDateTime);
							sRetStatus = 1;
						}
					}
				}
			}
		}
	}

	return sRetStatus;
}

static VOID  CliGusLoadTenderTableEntryAssign (ITEMTENDER *pItemTender, TENDERKEYINFO *pTendKeyInfo, struct GusTenderTableStruct *pTableEntry)
{
	GusTenderTableEntry  *pTableEntrySection = 0;

	switch (pTendKeyInfo->usTranCode) {
		case TENDERKEY_TRANCODE_PREAUTH:
			pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_PREAUTH;
			if (pItemTender->fbModifier & STORED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->StoredPreauthCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_STORED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_STORED;
			}
			if (pItemTender->fbModifier & APPROVED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->ApprovedPreauthCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_APPROVED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_APPROVED;
			}
			if (pItemTender->fbModifier & DECLINED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->DeclinedPreauthCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_DECLINED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_DECLINED;
			}
			break;

		case TENDERKEY_TRANCODE_PREAUTHCAPTURE:
			pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_CAPTURED;
			if (pItemTender->fbModifier & STORED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->StoredCapturedCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_STORED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_STORED;
			}
			if (pItemTender->fbModifier & APPROVED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->ApprovedCapturedCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_APPROVED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_APPROVED;
			}
			if (pItemTender->fbModifier & DECLINED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->DeclinedCapturedCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_DECLINED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_DECLINED;
			}
			break;

		default:
			if (pItemTender->fbModifier & STORED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->StoredStandardCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_STORED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_STORED;
			}
			if (pItemTender->fbModifier & APPROVED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->ApprovedStandardCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_APPROVED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_APPROVED;
			}
			if (pItemTender->fbModifier & DECLINED_EPT_MODIF) {
				pTableEntrySection = &(pTableEntry->DeclinedStandardCredit);
				pTableEntry->ulGusTenderFlag |= GUSTENDER_FLAG_DECLINED;
				pTableEntrySection->ulGusTenderFlag |= GUSTENDER_FLAG_DECLINED;
			}
			break;
	}

	if (pTableEntrySection) {
		if (CliGusAssignTenderStampToLatest (pItemTender, &(pTableEntrySection->dtStamp))) {
			pTableEntrySection->TenderKeyInfo = *pTendKeyInfo;
			pTableEntrySection->lTenderAmount += pItemTender->lTenderAmount;
			pTableEntrySection->lChange += pItemTender->lChange;
			pTableEntrySection->lGratuity += pItemTender->lGratuity;
			CliGusAssignTenderStampToDateTime (pItemTender, &(pTableEntrySection->dtStamp));
		}
	}
}

SHORT CliGusLoadTenderTableAdd (ITEMTENDER *pItemTender)
{
	BOOL    bConsolidate = TRUE;
	USHORT  usGlobalRules = 0, usFlags = 0;
	SHORT   sRulesCheck = 0;
	UCHAR   uchMinorClassTemp = pItemTender->uchMinorClass;
	struct GusTenderTableStruct *pTable = GusLoadTenderTable;
	PARATENDERKEYINFO TendKeyInfo;

	if (uchMinorClassTemp == CLASS_TEND_TIPS_RETURN) {
		// if this is a tips return then just ignore it.
		return 0;
	}

	if (pItemTender->uchMinorClass >= CLASS_FOREIGN1 && pItemTender->uchMinorClass <= CLASS_FOREIGN8) {
		// force foreign currency tenders to be same as tender #1 to be cash.
		// foreign currency would be cash and the tender amount is local cash equivalent of foreign currency.
		// for instance in US for Amtrak, Canadian currency is converted to US currency during tender.
		uchMinorClassTemp = CLASS_TENDER1;
	}

	ItemTenderGetSetTipReturn (0);    // clear the tips indicator just in case it is set

	sRulesCheck = ParaTendReturnsRuleCheckUse (uchMinorClassTemp, &usFlags, &usGlobalRules);
	bConsolidate = (usFlags & PARATENDRETURNFLAGS_CONSOLIDATETENDER);

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = uchMinorClassTemp;
	CliParaRead(&TendKeyInfo);

	if (uchMinorClassTemp == CLASS_TENDER1) {
		// force tender #1 to be cash.
		TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_CHARGE;
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_SALE;
	}

	for (pTable = GusLoadTenderTable; pTable < GusLoadTenderTable + (sizeof(GusLoadTenderTable)/sizeof(GusLoadTenderTable[0])); pTable++) {
		if (pTable->ItemTender.uchMajorClass == 0 && pTable->ItemTender.uchMinorClass == 0) {
			// find an empty place in the individual tender table to put this tender.
			// if we are doing consolidation on this tender, that is the next step.
			pTable->ItemTender = *pItemTender;
			pTable->ItemTender.uchMinorClass = uchMinorClassTemp;   // in case of foreign currency we need to over ride the minor class
			CliGusAssignTenderStampToDateTime (pItemTender, &(pTable->dtLatestStamp));
			pTable->TenderKeyInfo = TendKeyInfo.TenderKeyInfo;
			CliGusLoadTenderTableEntryAssign (pItemTender, &(TendKeyInfo.TenderKeyInfo), pTable);
			break;
		}

		if (pTable->TenderKeyInfo.usTranType == TendKeyInfo.TenderKeyInfo.usTranType ||
			pTable->ItemTender.uchMajorClass == pItemTender->uchMajorClass && pTable->ItemTender.uchMinorClass == uchMinorClassTemp) {
			BOOL  bAddToExisting = bConsolidate;
			if (pTable->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT && TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) {
				bAddToExisting = (_tcscmp (pTable->ItemTender.aszCPMsgText[NUM_CPRSPCO_CARDLABEL], pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]) == 0);
			}
			if (bAddToExisting) {
				if (TendKeyInfo.TenderKeyInfo.usTranType != TENDERKEY_TRANTYPE_CREDIT) {
					pTable->ItemTender.lTenderAmount += pItemTender->lTenderAmount;
					pTable->ItemTender.lChange += pItemTender->lChange;
					pTable->ItemTender.lGratuity += pItemTender->lGratuity;
				} else {
					// this will update the current transaction tender amount total for this tender
					// if we are in a transaction.
					// however for returns which also uses this function, we need to call the helper
					// function CliGusLoadTenderTableInitializeForReturns() to clean up the transaction
					// tender amount before starting the return process.
					pTable->ItemTender.lTenderAmount += pItemTender->lTenderAmount;
					pTable->ItemTender.lChange += pItemTender->lChange;
					pTable->ItemTender.lGratuity += pItemTender->lGratuity;
					CliGusLoadTenderTableEntryAssign (pItemTender, &(TendKeyInfo.TenderKeyInfo), pTable);
				}
				break;
			}
		}
	}
	return 0;
}

SHORT CliGusLoadTenderTableInitializeForReturns (VOID)
{
	struct GusTenderTableStruct *pTableEntry = GusLoadTenderTable;

	for (pTableEntry = GusLoadTenderTable; pTableEntry < GusLoadTenderTable + (sizeof(GusLoadTenderTable)/sizeof(GusLoadTenderTable[0])); pTableEntry++) {
		if (pTableEntry->ItemTender.uchMajorClass == 0 && pTableEntry->ItemTender.uchMinorClass == 0)
			break;

		// for each entry in the table determine what the initial tender amounts should be for Returns
		// the purpose of this is to review the data loaded into the table and determine what should be the initial
		// tender amounts to be used for each of the entries in the table when processing returns.
		// we only do this for Credit tender types which have the complicated rules.
		if (pTableEntry->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) {
			GusTenderTableEntry  *pTableEntrySection = 0;

			pTableEntry->ItemTender.lTenderAmount = pTableEntry->ItemTender.lChange = pTableEntry->ItemTender.lGratuity = 0;

//			if (pTableEntry->ulGusTenderFlag & GUSTENDER_FLAG_DECLINED)
//				continue;

			if (pTableEntry->ulGusTenderFlag & GUSTENDER_FLAG_CAPTURED) {
				pTableEntrySection = 0;
				if (pTableEntry->StoredCapturedCredit.ulGusTenderFlag & GUSTENDER_FLAG_STORED)
					pTableEntrySection = &(pTableEntry->StoredCapturedCredit);
				if (pTableEntry->ApprovedCapturedCredit.ulGusTenderFlag & GUSTENDER_FLAG_APPROVED)
					pTableEntrySection = &(pTableEntry->ApprovedCapturedCredit);
				if (pTableEntry->DeclinedCapturedCredit.ulGusTenderFlag & GUSTENDER_FLAG_DECLINED)
					pTableEntrySection = &(pTableEntry->DeclinedCapturedCredit);

				if (pTableEntrySection) {
					pTableEntry->ItemTender.lTenderAmount = pTableEntrySection->lTenderAmount;
					pTableEntry->ItemTender.lChange = pTableEntrySection->lChange;
					pTableEntry->ItemTender.lGratuity = pTableEntrySection->lGratuity;
				}
			} else if (pTableEntry->ulGusTenderFlag & GUSTENDER_FLAG_PREAUTH) {
				pTableEntrySection = 0;
				if (pTableEntry->StoredPreauthCredit.ulGusTenderFlag & GUSTENDER_FLAG_STORED)
					pTableEntrySection = &(pTableEntry->StoredPreauthCredit);
				if (pTableEntry->ApprovedPreauthCredit.ulGusTenderFlag & GUSTENDER_FLAG_APPROVED)
					pTableEntrySection = &(pTableEntry->ApprovedPreauthCredit);
				if (pTableEntry->DeclinedPreauthCredit.ulGusTenderFlag & GUSTENDER_FLAG_DECLINED)
					pTableEntrySection = &(pTableEntry->DeclinedPreauthCredit);

				if (pTableEntrySection) {
					pTableEntry->ItemTender.lTenderAmount = pTableEntrySection->lTenderAmount;
					pTableEntry->ItemTender.lChange = pTableEntrySection->lChange;
					pTableEntry->ItemTender.lGratuity = pTableEntrySection->lGratuity;
				}
			}

			pTableEntrySection = 0;
			if (pTableEntry->StoredStandardCredit.ulGusTenderFlag & GUSTENDER_FLAG_STORED)
				pTableEntrySection = &(pTableEntry->StoredStandardCredit);
			if (pTableEntry->ApprovedStandardCredit.ulGusTenderFlag & GUSTENDER_FLAG_APPROVED)
				pTableEntrySection = &(pTableEntry->ApprovedStandardCredit);
			if (pTableEntry->DeclinedStandardCredit.ulGusTenderFlag & GUSTENDER_FLAG_DECLINED)
				pTableEntrySection = &(pTableEntry->DeclinedStandardCredit);

			if (pTableEntrySection) {
				pTableEntry->ItemTender.lTenderAmount += pTableEntrySection->lTenderAmount;
				pTableEntry->ItemTender.lChange += pTableEntrySection->lChange;
				pTableEntry->ItemTender.lGratuity += pTableEntrySection->lGratuity;
			}
		}
	}

	return 0;
}


// This function accepts the tender information of a tender on a Return transaction
// and determines whether the tender should be allowed or not.
// The rules are from directives in the file PARAMTDR in the Database folder.
// This task is accomplished in a number of steps.
// Review the tenders that have been performed against this transaction to
// determine if this new tender matches any of them.  If we find a matching
// tender already in the transaction then we next need to look to see if the
// rules for allowing a return are met or not.
//
// we will return the following values
//  -1 means that the rules check failed
//   0 means that the rules check succeeded and the tender may be used
int CliGusLoadTenderTableLookupAndCheckRules (ITEMTENDER *pItemTender)
{
	struct GusTenderTableStruct *pTable = GusLoadTenderTable;
	struct GusTenderTableStruct *pTableEnd = GusLoadTenderTable + (sizeof(GusLoadTenderTable)/sizeof(GusLoadTenderTable[0]));
	USHORT  usGlobalRules = 0, usFlags = 0;
	SHORT   sRulesCheck = 0;
	UCHAR   uchMinorClassTemp = pItemTender->uchMinorClass;
	ULONG   ulErrorFlags = 0;
	PARATENDERKEYINFO TendKeyInfo;

	if (pItemTender->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		// if this is a tips return then just ignore it.
		return 0;
	}

	if (pItemTender->uchMinorClass >= CLASS_FOREIGN1 && pItemTender->uchMinorClass <= CLASS_FOREIGN8) {
		// force foreign currency tenders to be same as tender #1 to be cash.
		uchMinorClassTemp = 1;
	}

	sRulesCheck = ParaTendReturnsRuleCheckUse (uchMinorClassTemp, &usFlags, &usGlobalRules);

	if (sRulesCheck < 0 || pItemTender->lTenderAmount >= 0) {
		// there are no rules for this tender including any global rules
		return 0;
	}

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = uchMinorClassTemp;
	CliParaRead(&TendKeyInfo);

	if (uchMinorClassTemp == 1) {
		// force tender #1 to be cash.
		// this includes any foreign currency tenders.
		TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_CHARGE;
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_SALE;
	}

	pTable = GusLoadTenderTable;
	pTableEnd = GusLoadTenderTable + (sizeof(GusLoadTenderTable)/sizeof(GusLoadTenderTable[0]));

	ulErrorFlags = 0;
	for (; pTable < pTableEnd; pTable++) {
		ULONG   ulErrorFlagsHistory = 0;
		BOOL    bPossibleMatchingTender = FALSE;

		if (pTable->ItemTender.uchMajorClass == 0 && pTable->ItemTender.uchMinorClass == 0)
			break;

		bPossibleMatchingTender = FALSE;
		if ((usGlobalRules & PARATENDRETURNGLOBAL_LIKETOLIKEONLY) != 0) {
//			if (pTable->ItemTender.uchMajorClass != pItemTender->uchMajorClass || pTable->ItemTender.uchMinorClass != pItemTender->uchMinorClass) {
//				ulErrorFlags |= PARATENDRETURNGLOBAL_LIKETOLIKEONLY;
//				continue;
//			}
			if (pTable->TenderKeyInfo.usTranType != TendKeyInfo.TenderKeyInfo.usTranType) {
				ulErrorFlags |= PARATENDRETURNGLOBAL_LIKETOLIKEONLY;
				continue;
			}
			ulErrorFlags &= ~PARATENDRETURNGLOBAL_LIKETOLIKEONLY;
		}
		bPossibleMatchingTender = TRUE;
		if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT || pTable->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) {
			bPossibleMatchingTender = FALSE;

			// check the global rules first
			// if this is an electronic payment tender then we need to check the various global rules for
			// electronic payment tenders.
			if ((usGlobalRules & PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY) != 0) {
				if (ItemComonCheckReturnCardLabel (pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]) == 0) {
					ulErrorFlags |= PARATENDRETURNGLOBAL_TIPNOTONACCOUNT;
					continue;
				}

				// return on a credit tender must use the same card label which is
				// the card type and the last 4 digits of the account number
				if (_tcscmp (pTable->ItemTender.aszCPMsgText[NUM_CPRSPCO_CARDLABEL], pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]) != 0) {
					ulErrorFlags |= PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY;
					continue;
				}

				ulErrorFlags &= ~(PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY | PARATENDRETURNGLOBAL_TIPNOTONACCOUNT);
			}

			if ((pTable->ulGusTenderFlag & (GUSTENDER_FLAG_STORED | GUSTENDER_FLAG_APPROVED | GUSTENDER_FLAG_PREAUTH | GUSTENDER_FLAG_CAPTURED)) == GUSTENDER_FLAG_STORED) {
				if ((usGlobalRules & PARATENDRETURNGLOBAL_ALLOWSTORED) == 0) {
					// return on a stored tender is not allowed
					ulErrorFlags |= PARATENDRETURNGLOBAL_ALLOWSTORED;
					continue;
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_ALLOWSTORED;
			}

			if ((pTable->ulGusTenderFlag & GUSTENDER_FLAG_PREAUTH) != 0) {
				if ((pTable->ulGusTenderFlag & GUSTENDER_FLAG_APPROVED) == 0) {
					if ((usGlobalRules & PARATENDRETURNGLOBAL_APPROVEDONLY) != 0) {
						// return on a unapproved tender is not allowed
						ulErrorFlags |= PARATENDRETURNGLOBAL_APPROVEDONLY;
						continue;
					}
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_APPROVEDONLY;
			}

			if (pTable->ulGusTenderFlag & (GUSTENDER_FLAG_PREAUTH | GUSTENDER_FLAG_CAPTURED)) {
				if ((pTable->ulGusTenderFlag & GUSTENDER_FLAG_CAPTURED) == 0) {
					if ((usGlobalRules & PARATENDRETURNGLOBAL_ALLOWPREAUTH) == 0) {
						// return on a preauth that has not been captured is not allowed
						ulErrorFlags |= PARATENDRETURNGLOBAL_ALLOWPREAUTH;
						continue;
					}
					ulErrorFlags &= ~PARATENDRETURNGLOBAL_ALLOWPREAUTH;
				}
			}

			if (pTable->ulGusTenderFlag & GUSTENDER_FLAG_DECLINED) {
				ulErrorFlagsHistory |= PARATENDRETURNGLOBAL_ALLOWDECLINED;
				if ((usGlobalRules & PARATENDRETURNGLOBAL_ALLOWDECLINED) == 0) {
					// return on a preauth that has been declined is not allowed
					ulErrorFlags |= PARATENDRETURNGLOBAL_ALLOWDECLINED;
					continue;
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_ALLOWDECLINED;
			}

			if ((usGlobalRules & PARATENDRETURNGLOBAL_BATCHONLY) != 0) {
				// check to see if in the batch and if not, do not allow
				// we perform a check against the date of the last batch and this credit tender.
				// if the credit tender was done after the last settlement then we allow it, otherwise not.
				if (pTable->dtLatestStamp.uchTenderYear < GusTenderBatchCloseDateTime.usYear) {
					ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
					continue;
				}
				if (pTable->dtLatestStamp.uchTenderYear == GusTenderBatchCloseDateTime.usYear) {
					if (pTable->dtLatestStamp.uchTenderMonth < GusTenderBatchCloseDateTime.usMonth) {
						ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
						continue;
					}
					if (pTable->dtLatestStamp.uchTenderMonth == GusTenderBatchCloseDateTime.usMonth) {
						if (pTable->dtLatestStamp.uchTenderDay < GusTenderBatchCloseDateTime.usMDay) {
							ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
							continue;
						}
						if (pTable->dtLatestStamp.uchTenderDay == GusTenderBatchCloseDateTime.usMDay) {
							if (pTable->dtLatestStamp.uchTenderHour < GusTenderBatchCloseDateTime.usHour) {
								ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
								continue;
							}
							if (pTable->dtLatestStamp.uchTenderHour == GusTenderBatchCloseDateTime.usHour) {
								if (pTable->dtLatestStamp.uchTenderMinute < GusTenderBatchCloseDateTime.usMin) {
									ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
									continue;
								}
								if (pTable->dtLatestStamp.uchTenderMinute == GusTenderBatchCloseDateTime.usMin) {
									if (pTable->dtLatestStamp.uchTenderSecond < GusTenderBatchCloseDateTime.usSec) {
										ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
										continue;
									}
								}
							}
						}
					}
				}
//				ulErrorFlags &= ~PARATENDRETURNGLOBAL_BATCHONLY;
			}
			bPossibleMatchingTender = TRUE;
		}

		if (bPossibleMatchingTender) {
			// we have found a matching tender and we now need to check that this particular tender
			// is allowed.  This is over and above the other rules which are primarily for electronic
			// payment tenders such as credit card or debit card.
			// there are a few things we need to determine.
			if (pTable->ItemTender.lTenderAmount - pTable->ItemTender.lChange + pItemTender->lTenderAmount >= 0) {
				int  iRetStatus = 0;

				// we need to indicate if there were any declined credit card tenders in this batch
				// so that we can decide if we are going to allow the return or not.
				// Amtrak wants a Return with a Declined credit tender to be allowed.
				// so we provide a way for indicating if a credit tender was declined so that a
				// decision can be made whether to allow it anyway or not.
				// we are using (ItemTender->fbModifier | DECLINED_EPT_MODIF) to indicate a declined tender.
				// we are using (ItemTender->fbModifier | STORED_EPT_MODIF) to indicate a stored but not approved tender.
				iRetStatus |= (ulErrorFlagsHistory & (PARATENDRETURNGLOBAL_ALLOWDECLINED));
				iRetStatus |= (((pTable->ulGusTenderFlag & (GUSTENDER_FLAG_STORED | GUSTENDER_FLAG_APPROVED | GUSTENDER_FLAG_DECLINED)) == GUSTENDER_FLAG_STORED) ? PARATENDRETURNGLOBAL_ALLOWSTORED : 0);
				return iRetStatus;
			}
			ulErrorFlags |= PARATENDRETURNGLOBAL_ERROR_TENDERAMT;
		}
	}

	if (ulErrorFlags != 0) {
		char   xBuff[128];
		sprintf (xBuff, "-- Rule match failed 0x%x", ulErrorFlags);
		NHPOS_ASSERT_TEXT((ulErrorFlags == 0), xBuff);

		// now we analyze the results of our filters
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ERROR_TENDERAMT)
			return (RETURNS_MSG_TENDERTOTALEXCEEDED);    // MSG: Amount exceeds allowed amount.
		if (ulErrorFlags & PARATENDRETURNGLOBAL_LIKETOLIKEONLY)
			return (RETURNS_MSG_TENDERMUSTMATCH);        // MSG: Same Tender Type is required for Returns
		if (ulErrorFlags & PARATENDRETURNGLOBAL_BATCHONLY)
			return (RETURNS_MSG_ALREADYSETTLED);         // MSG: Credit must be in current batch before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ALLOWPREAUTH)
			return (RETURNS_MSG_MUSTBEFINALIZED);        // MSG: Preauth must be Captured before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_APPROVEDONLY)
			return (RETURNS_MSG_MUSTBEAPPROVED);         // MSG: Credit must be Approved before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ALLOWSTORED)
			return (RETURNS_MSG_MUSTBEFORWARDED);        // MSG: Credit must be Forward before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_TIPNOTONACCOUNT)
			return (RETURNS_MSG_TIPNOTONCARDSWIPED);     // MSG: Credit must be to same card before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY)
			return (RETURNS_MSG_MUSTBESAMECARD);         // MSG: Credit must be to same card before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ALLOWDECLINED)
			return (RETURNS_MSG_ORIGINALDECLINED);       // MSG: Declined card not allowed.
	}
	return -1;
}

// Perform an initial check on the transaction data.  If there is any problem found then
// do not allow the transaction return to go forward.
SHORT CliGusLoadTenderTableInitialCheck (VOID)
{
	struct GusTenderTableStruct *pTable = GusLoadTenderTable;
	struct GusTenderTableStruct *pTableEnd = GusLoadTenderTable + (sizeof(GusLoadTenderTable)/sizeof(GusLoadTenderTable[0]));
	USHORT  usGlobalRules = 0, usFlags = 0, usCountPossibleTenders = 0;
	SHORT   sRulesCheck = 0;
	ULONG   ulErrorFlags = 0;
	UCHAR   uchMinorClass = 1;       // We will just use Cash type of tender as it does not really matter for our purposes.
	PARATENDERKEYINFO TendKeyInfo;

	sRulesCheck = ParaTendReturnsRuleCheckUse (uchMinorClass, &usFlags, &usGlobalRules);

	if (sRulesCheck < 0) {
		// there are no rules for this tender including any global rules
		return 0;
	}

	TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_CHARGE;
	TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_SALE;

	pTable = GusLoadTenderTable;
	pTableEnd = GusLoadTenderTable + (sizeof(GusLoadTenderTable)/sizeof(GusLoadTenderTable[0]));

	usCountPossibleTenders = 0;

	ulErrorFlags = 0;
	for (; pTable < pTableEnd; pTable++) {

		if (pTable->ItemTender.uchMajorClass == 0 && pTable->ItemTender.uchMinorClass == 0)
			break;

		if (pTable->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) {
			GusTenderTableEntry  *pTableEntrySection = 0;

			if ((pTable->ulGusTenderFlag & (GUSTENDER_FLAG_STORED | GUSTENDER_FLAG_APPROVED | GUSTENDER_FLAG_PREAUTH | GUSTENDER_FLAG_CAPTURED)) == GUSTENDER_FLAG_STORED) {
				if ((usGlobalRules & PARATENDRETURNGLOBAL_ALLOWSTORED) == 0) {
					// return on a tender that has been stored but no other processing is not allowed
					ulErrorFlags |= PARATENDRETURNGLOBAL_ALLOWSTORED;
					break;
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_ALLOWSTORED;
			}

			if ((pTable->ulGusTenderFlag & GUSTENDER_FLAG_PREAUTH) != 0) {
				if ((pTable->ulGusTenderFlag & GUSTENDER_FLAG_APPROVED) == 0) {
					if ((usGlobalRules & PARATENDRETURNGLOBAL_APPROVEDONLY) != 0) {
						// return on a unapproved tender is not allowed
						ulErrorFlags |= PARATENDRETURNGLOBAL_APPROVEDONLY;
						break;
					}
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_APPROVEDONLY;
			}

			if (pTable->ulGusTenderFlag & (GUSTENDER_FLAG_PREAUTH | GUSTENDER_FLAG_CAPTURED)) {
				if ((pTable->ulGusTenderFlag & GUSTENDER_FLAG_CAPTURED) == 0) {
					if ((usGlobalRules & PARATENDRETURNGLOBAL_ALLOWPREAUTH) == 0) {
						// return on a preauth that has not been captured is not allowed
						ulErrorFlags |= PARATENDRETURNGLOBAL_ALLOWPREAUTH;
						break;
					}
					ulErrorFlags &= ~PARATENDRETURNGLOBAL_ALLOWPREAUTH;
				}
			}

			if (pTable->ulGusTenderFlag & GUSTENDER_FLAG_DECLINED) {
				if ((usGlobalRules & PARATENDRETURNGLOBAL_ALLOWDECLINED) == 0) {
					// return on a preauth that has been declined is not allowed
					ulErrorFlags |= PARATENDRETURNGLOBAL_ALLOWDECLINED;
					break;
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_ALLOWDECLINED;
			}

			if ((usGlobalRules & PARATENDRETURNGLOBAL_BATCHONLY) != 0) {
				// check to see if in the batch and if not, do not allow
				// we perform a check against the date of the last batch and this credit tender.
				// if the credit tender was done after the last settlement then we allow it, otherwise not.
				if (pTable->dtLatestStamp.uchTenderYear < GusTenderBatchCloseDateTime.usYear) {
					ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
					break;
				}
				if (pTable->dtLatestStamp.uchTenderYear == GusTenderBatchCloseDateTime.usYear) {
					if (pTable->dtLatestStamp.uchTenderMonth < GusTenderBatchCloseDateTime.usMonth) {
						ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
						break;
					}
					if (pTable->dtLatestStamp.uchTenderMonth == GusTenderBatchCloseDateTime.usMonth) {
						if (pTable->dtLatestStamp.uchTenderDay < GusTenderBatchCloseDateTime.usMDay) {
							ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
							break;
						}
						if (pTable->dtLatestStamp.uchTenderDay == GusTenderBatchCloseDateTime.usMDay) {
							if (pTable->dtLatestStamp.uchTenderHour < GusTenderBatchCloseDateTime.usHour) {
								ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
								break;
							}
							if (pTable->dtLatestStamp.uchTenderHour == GusTenderBatchCloseDateTime.usHour) {
								if (pTable->dtLatestStamp.uchTenderMinute < GusTenderBatchCloseDateTime.usMin) {
									ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
									break;
								}
								if (pTable->dtLatestStamp.uchTenderMinute == GusTenderBatchCloseDateTime.usMin) {
									if (pTable->dtLatestStamp.uchTenderSecond < GusTenderBatchCloseDateTime.usSec) {
										ulErrorFlags |= PARATENDRETURNGLOBAL_BATCHONLY;
										break;
									}
								}
							}
						}
					}
				}
				ulErrorFlags &= ~PARATENDRETURNGLOBAL_BATCHONLY;
			}
			if (pTable->ItemTender.lTenderAmount - pTable->ItemTender.lChange > 0)
				usCountPossibleTenders++;
		} else {
			// check to see if there is any more tender amount available for a return with
			// this tender or not.  if the value is less than 1 then no more tender amount
			// available for a return.  Otherwise it is a possible tender that can be used.
			if (pTable->ItemTender.lTenderAmount - pTable->ItemTender.lChange > 0)
				usCountPossibleTenders++;
		}
	}

	if (usCountPossibleTenders < 1 || (ulErrorFlags & (PARATENDRETURNGLOBAL_ALLOWPREAUTH | PARATENDRETURNGLOBAL_ALLOWDECLINED | PARATENDRETURNGLOBAL_BATCHONLY))) {
		char   xBuff[128];
		sprintf (xBuff, "-- Rule match failed 0x%x  usCountPossibleTenders %d", ulErrorFlags, usCountPossibleTenders);
		NHPOS_ASSERT_TEXT((ulErrorFlags == 0), xBuff);

		sRulesCheck = (RETURNS_MSG_TRANSNOTALLOWED);
		// now we analyze the results of our filters
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ERROR_TENDERAMT)
			return (RETURNS_MSG_TENDERTOTALEXCEEDED);   // MSG: Amount exceeds allowed amount.
		if (ulErrorFlags & PARATENDRETURNGLOBAL_LIKETOLIKEONLY)
			return (RETURNS_MSG_TENDERMUSTMATCH);       // MSG: Same Tender Type is required for Returns
		if (ulErrorFlags & PARATENDRETURNGLOBAL_BATCHONLY)
			return (RETURNS_MSG_ALREADYSETTLED);        // MSG: Credit must be in current batch before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ALLOWPREAUTH)
			return (RETURNS_MSG_MUSTBEFINALIZED);       // MSG: Preauth must be Captured before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_APPROVEDONLY)
			return (RETURNS_MSG_MUSTBEAPPROVED);        // MSG: Credit must be Approved before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ALLOWSTORED)
			return (RETURNS_MSG_MUSTBEFORWARDED);       // MSG: Credit must be Forward before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY)
			return (RETURNS_MSG_MUSTBESAMECARD);        // MSG: Credit must be to same card before Return allowed
		if (ulErrorFlags & PARATENDRETURNGLOBAL_ALLOWDECLINED)
			return (RETURNS_MSG_ORIGINALDECLINED);      // MSG: Declined card not allowed.
	}
	return sRulesCheck;
}

// convert from a Returns check error code to a Returns check 
SHORT CliGusLoadTenderTableConvertError (SHORT sGusRetValue)
{
	SHORT  sRetCode = LDT_PROHBT_ADR;
	if (sGusRetValue <= RETURNS_BASE_ERROR) {
		sRetCode = abs(sGusRetValue) + RETURNS_BASE_ERROR + RETURNS_BASE_LDTOFFSET;
	}

	return sRetCode;
}

// ----------------------------------------------------
static struct {
	int        iIndex;
	ITEMTENDER ItemTender[50];
}  ItemTenderConsolidate;

static SHORT CliGusConsolidateTender (ITEMTENDER *pItemTender)
{
	int    iLoop = 0;
	SHORT  sRetStatus = -1;

	for (iLoop = 0; iLoop < ItemTenderConsolidate.iIndex && ItemTenderConsolidate.ItemTender[iLoop].usCheckTenderId; iLoop++) {
		if (pItemTender->uchMinorClass != CLASS_TEND_TIPS_RETURN && ItemTenderConsolidate.ItemTender[iLoop].usCheckTenderId == pItemTender->usCheckTenderId) {
			USHORT  fbModifier = (ItemTenderConsolidate.ItemTender[iLoop].fbModifier | pItemTender->fbModifier);
			ItemTenderConsolidate.ItemTender[iLoop] = *pItemTender;
			ItemTenderConsolidate.ItemTender[iLoop].fbModifier = fbModifier;
			sRetStatus = 0;
			break;
		}
	}

	if (sRetStatus < 0 && iLoop >= ItemTenderConsolidate.iIndex && iLoop < 50) {
		ItemTenderConsolidate.ItemTender[iLoop] = *pItemTender;
		ItemTenderConsolidate.iIndex++;
		sRetStatus = 0;
	}

	return sRetStatus;
}

SHORT  CliGusLoadFromConnectionEngine (TCHAR  *uchUniqueIdentifier)
{
	LONG    lErrorCode = 0;
	int     iLoop;
	SHORT   sRetStatus = 0;

    ItemTendCPClearFile();
	/*--- LCD Initialize,  R3.0 ---*/
	ItemCommonMldInit();

	ConnEngineSendRequestTransactionStatus (1, 0);
	if ((sRetStatus = ConnEngineSendRequestTransaction (_T("return-retrieve"), uchUniqueIdentifier)) == 0) {
		for (iLoop = 240; iLoop > 0; iLoop--) {
			PifSleep (50);
			if (ConnEngineSendRequestTransactionStatus (-1, &lErrorCode) == 2) {
				break;
			}
		}
		if (iLoop < 1) {
			lErrorCode = sRetStatus = LDT_CPM_OFFLINE_ADR;
		}
	} else {
		iLoop = 0;
		lErrorCode = sRetStatus = LDT_CPM_OFFLINE_ADR;
	}

	ConnEngineSendRequestTransactionStatus (0, 0);

	if (iLoop > 0 && lErrorCode == 0) {
		ULONG          ulActualBytesRead;
		SHORT          sTempFileHandle;
		GCNUM          usNewGCNumber;
		static UCHAR   auchCP_TEMP_OLD_FILE_READ_WRITE[]  = "towr";

		CliGusLoadTenderTableInit(0);

		memset (&ItemTenderConsolidate, 0, sizeof(ItemTenderConsolidate));

		sTempFileHandle = PifOpenFile (_T("OBJTEMP"), auchCP_TEMP_OLD_FILE_READ_WRITE);
		if (sTempFileHandle < 0) {
			char  xBuff[128];
			sprintf (xBuff, "CliGusLoadFromConnectionEngine(): Error opening OBJTEMP %d.", sTempFileHandle);
			NHPOS_ASSERT_TEXT((sTempFileHandle >= 0), xBuff);
			return LDT_CLRABRT_ADR;
		}

	   /*----- Get Order# in case of Transaction Open R3.1 -----*/
		sRetStatus = ItemCommonGetSRAutoNo(&usNewGCNumber);
		if (sRetStatus != 0) {
			return(sRetStatus);
		}

		{
			TRANGCFQUAL      *pWorkGCF = TrnGetGCFQualPtr();
			TRANINFORMATION  *pWorkTran = TrnGetTranInformationPointer();

			pWorkTran->TranCurQual.fsCurStatus |= CURQUAL_TRETURN;        // indicate TTL_TUP_TRETURN for totals functionality, CLASS_PRINT_TRETURN

			pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;    /* ItemReturnsLoadGuestCheck(): indicate this is a transaction return */
		}

		sRetStatus = ItemCommonTransOpen(usNewGCNumber);
		if (sRetStatus != 0) {
			return(sRetStatus);
		}

		ItemSalesCalcInitCom();                             /* ppi initialize r3.1 2172 */

		ItemReturnRegularDiscountInit();

		if (sTempFileHandle >= 0) {
			int              bDoLoop = 1;
			TRANGCFQUAL      WorkQual = {0};
			TRANITEMIZERS    WorkItemizers = {0};
			USHORT           usTranConsStoVli = 0;
			SHORT            sRetStatus;
			SHORT            sBatchClose = 0;
			DATE_TIME        DateTimeBatchClose;

			PifSeekFile (sTempFileHandle, SERTMPFILE_DATASTART, &ulActualBytesRead);
			PifReadFile(sTempFileHandle, &WorkQual, sizeof(WorkQual), &ulActualBytesRead);
			PifReadFile(sTempFileHandle, &WorkItemizers, sizeof(WorkItemizers), &ulActualBytesRead);
			PifReadFile(sTempFileHandle, &usTranConsStoVli, sizeof(usTranConsStoVli), &ulActualBytesRead);

			sBatchClose = ParaTendReturnsBatchCloseReadDateTime (&DateTimeBatchClose);
			CliGusLoadTenderTableInit(&DateTimeBatchClose);

			{
				TRANGCFQUAL   *pWorkGCF = TrnGetGCFQualPtr();
				TRANMODEQUAL  *pWorkMode = TrnGetModeQualPtr();
				int           i;

				// init the reason code and return type data in ModeQual as this area
				// may not be initialized during starting a new transaction.
				pWorkMode->usReasonCode = 0;
				pWorkMode->usReturnType = 0;

				// set the unique identifier of the return to this
				for (i = 0; i < sizeof(WorkQual.uchUniqueIdentifierReturn)/sizeof(WorkQual.uchUniqueIdentifierReturn[0]); i++) {
					pWorkGCF->uchUniqueIdentifierReturn[i] = WorkQual.uchUniqueIdentifier[i];
				}

				// The following are global flags in the memory resident data base that modify the behavior
				// of a number of FSCs such as discounts.  Any global flag process in function ObjectMakerTRANSACTION()
				// need to be added here in order to make the global flags available to the transaction being returned.
				pWorkGCF->fsGCFStatus = WorkQual.fsGCFStatus;
				pWorkGCF->fsGCFStatus2 = WorkQual.fsGCFStatus2;
				pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;    /* ItemReturnsLoadGuestCheck(): indicate this is a transaction return */

				// update the tender count for this check so that new tenders will have a unique id
				// Inventory and MWS need to be able to link tender within transaction to use in Forward
				ItemTenderLocalCheckTenderid (2, WorkQual.usCheckTenderId);
			}

			while (bDoLoop) {
				union {
					ITEMSALES      ItemSales;
					ITEMTENDER     ItemTender;
					ITEMTOTAL      ItemTotal;
					ITEMTRANSOPEN  ItemTransOpen;
					ITEMDISC       ItemDisc;
					ITEMCOUPON     ItemCoupon;
					ITEMMISC       ItemMisc;
					ITEMAFFECTION  ItemAffect;
				} MyUnion = {0};

				PifReadFile(sTempFileHandle, &MyUnion, sizeof(UCHAR) + sizeof(UCHAR), &ulActualBytesRead);
				if (ulActualBytesRead < 2)
					break;
				switch (MyUnion.ItemSales.uchMajorClass) {
					case CLASS_ITEMSALES:
						{
							PifReadFile(sTempFileHandle, &MyUnion.ItemSales.uchMinorClass + 1, sizeof(ITEMSALES) - sizeof(UCHAR)*2, &ulActualBytesRead);
							ItemSalesLabelKeyedOnlyProc (MyUnion.ItemSales.auchPLUNo);
							sRetStatus = TrnStoSales (&MyUnion.ItemSales);
						}
						break;
					case CLASS_ITEMTENDER:
						PifReadFile(sTempFileHandle, &MyUnion.ItemTender.uchMinorClass + 1, sizeof(ITEMTENDER) - sizeof(UCHAR)*2, &ulActualBytesRead);
						{
							int iLoop;

							for (iLoop = 0; iLoop < NUM_CPRSPCO_EPT; iLoop++) {
								MyUnion.ItemTender.aszCPMsgText[iLoop][STD_CPRSPTEXT_LEN] = 0;  // ensure zero terminator specified
							}
						}
						sRetStatus = CliGusConsolidateTender (&MyUnion.ItemTender);
						break;
					case CLASS_ITEMTOTAL:
						PifReadFile(sTempFileHandle, &MyUnion.ItemTotal.uchMinorClass + 1, sizeof(ITEMTOTAL) - sizeof(UCHAR)*2, &ulActualBytesRead);
						sRetStatus = TrnStoTotal ( &MyUnion.ItemTotal );
						break;
					case CLASS_ITEMTRANSOPEN:
						PifReadFile(sTempFileHandle, &MyUnion.ItemTransOpen.uchMinorClass + 1, sizeof(ITEMTOTAL) - sizeof(UCHAR)*2, &ulActualBytesRead);
						sRetStatus = TrnStoOpen ( &MyUnion.ItemTransOpen );
						break;
					case CLASS_ITEMDISC:
						PifReadFile(sTempFileHandle, &MyUnion.ItemDisc.uchMinorClass + 1, sizeof(ITEMDISC) - sizeof(UCHAR)*2, &ulActualBytesRead);
						ItemSalesLabelKeyedOnlyProc (MyUnion.ItemDisc.auchPLUNo);
						ItemReturnRegularDiscountAdd (&MyUnion.ItemDisc);
						sRetStatus = TrnStoDisc ( &MyUnion.ItemDisc );
						break;
					case CLASS_ITEMCOUPON:
						PifReadFile(sTempFileHandle, &MyUnion.ItemCoupon.uchMinorClass + 1, sizeof(ITEMCOUPON) - sizeof(UCHAR)*2, &ulActualBytesRead);
						sRetStatus = TrnStoCoupon ( &MyUnion.ItemCoupon );
						break;
					case CLASS_ITEMMISC:
						PifReadFile(sTempFileHandle, &MyUnion.ItemMisc.uchMinorClass + 1, sizeof(ITEMMISC) - sizeof(UCHAR)*2, &ulActualBytesRead);
						sRetStatus = TrnStoMisc ( &MyUnion.ItemMisc );
						break;
					case CLASS_ITEMAFFECTION:
						PifReadFile(sTempFileHandle, &MyUnion.ItemAffect.uchMinorClass + 1, sizeof(ITEMAFFECTION) - sizeof(UCHAR)*2, &ulActualBytesRead);
						sRetStatus = TrnStoAffect ( &MyUnion.ItemAffect );
						break;
					default:
						{
							char  xBuffer[128];
							sprintf (xBuffer, "CliGusLoadFromConnectionEngine(): %S - %d  %d", uchUniqueIdentifier, MyUnion.ItemSales.uchMajorClass, MyUnion.ItemSales.uchMinorClass);
							NHPOS_ASSERT_TEXT(0, "UNKNOWN Major Class.");
						}
						bDoLoop = 0;
						break;
				}
			}
		}
		PifCloseFile (sTempFileHandle);

		{
			int  iLoop = 0;
			for (iLoop = 0; iLoop < ItemTenderConsolidate.iIndex; iLoop++) {
				CliGusLoadTenderTableAdd (&ItemTenderConsolidate.ItemTender[iLoop]);
				sRetStatus = TrnStoTender( &ItemTenderConsolidate.ItemTender[iLoop]);
			}
		}


		{
			SHORT sGusRetValue = 0;

			CliGusLoadTenderTableInitializeForReturns();

			if ((sGusRetValue = CliGusLoadTenderTableInitialCheck ()) < 0) {
				sRetStatus = CliGusLoadTenderTableConvertError (sGusRetValue);
				NHPOS_ASSERT_TEXT(0, "CliGusLoadFromConnectionEngine(): Initial Check failed.");
				UieErrorMsg( sRetStatus, UIE_WITHOUT);
				return UIF_CANCEL;
			}
		}

		{
			MLDTRANSDATA     WorkMLD;

			MldPutTransToScroll(MldMainSetMldData(&WorkMLD));
			MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);
		}
	} else if (lErrorCode != 0) {
		char  xBuffer[128];
		sprintf (xBuffer, "CliGusLoadFromConnectionEngine(): %S lErrorCode = %d", uchUniqueIdentifier, lErrorCode);
		NHPOS_ASSERT_TEXT((lErrorCode == 0), xBuffer);
		if (lErrorCode > 0 && lErrorCode <= MAX_LEAD_NO) {
			sRetStatus = (SHORT)lErrorCode;
		} else {
			switch (lErrorCode) {
				case 3000:   // lookup failed, unique identifier not found
					sRetStatus = LDT_NTINFL_ADR;
					break;
				case 3001:   // lookup succeeded however the transaction has already been target of a return
					sRetStatus = LDT_ILLEGLNO_ADR;
					break;
				case 3002:   // lookup succeeded however transaction return no longer allowed, probably settled and End of Day
					sRetStatus = LDT_SALESRESTRICTED;
					break;
				default:
					sRetStatus = LDT_GCFSUNMT_ADR;
					break;
			}
		}
		UieErrorMsg( sRetStatus, UIE_WITHOUT);
		sRetStatus = UIF_CANCEL;
	}

	return sRetStatus;
}

SHORT  CliGusLoadPaidTransaction (GCNUM  usGCNumber, GCNUM usNewGCNumber)
{
	SHORT          sRetStatus = LDT_PROHBT_ADR;

	CliGusLoadTenderTableInit(0);

	if (usGCNumber != 0) {
		ItemTendCPClearFile();
		/*--- LCD Initialize,  R3.0 ---*/
		ItemCommonMldInit();

        /*----- Read Transaction -----*/
		if ((sRetStatus = ItemTransGetReturnsGcf (usGCNumber, usNewGCNumber)) == TRN_SUCCESS) {
			TRANINFORMATION  *pWorkTran = TrnGetTranInformationPointer();
			ITEMTRANSOPEN    ItemTransOpen = {0};

			ItemTransOpen.usGuestNo = pWorkTran->TranGCFQual.usGuestNo;                  /* save GCF No */
			ItemTransOpen.uchCdv = pWorkTran->TranGCFQual.uchCdv;                        /* save CDV */

			/*--- Display GCF/Order#, R3.1 ---*/
			MldPutGcfToScroll(MLD_SCROLL_1, pWorkTran->hsTranConsStorage);
			MldScrollWrite(&ItemTransOpen, MLD_NEW_ITEMIZE);    /* display reorder if no storage */
			MldScrollFileWrite(&ItemTransOpen);
			MldDispSubTotal(MLD_TOTAL_1, pWorkTran->TranItemizers.lMI);
        } else {
			/*----- Error(Communication etc.) -----*/
            sRetStatus = GusConvertError(sRetStatus);
        }
    }

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusGetAutoNo(USHORT *pusGCNumber);
*    OutPut:    *pusGCNumber    - GCN to store
*
** Return:      GCF_SUCCESS:    deleted
*               GCF_FILE_FULL:  file full
*               GCF_NOT_LOCKED: has not been read
*               GCF_ALL_LOCKED: during reporting
*
** Description: 
*   This function supports to get auto generation Guest Check Number.
*===========================================================================
*/
SHORT	CliGusGetAutoNo(GCNUM *pusGCNumber)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError, sBRetCode = 0;            
    SHORT       sCliRetCode, sErrorBM = 0, sErrorM = 0;
	SHORT		sMoved=0;
    GCNUM      usGCNassign;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    CliMsg.usFunCode    = CLI_FCGCFAGENGCN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusGetAutoNo(&(ResMsgH.usGCN));
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
	usGCNassign = ResMsgH.usGCN;
    if (STUB_SUCCESS == sErrorM) {
		// inform the Backup Master of the auto assigned guest check number.
        ReqMsgH.usGCN = usGCNassign;
        CliMsg.usFunCode = CLI_FCGCFABAKGCN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    }
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        if (STUB_SUCCESS != sErrorM) {
            CliMsg.sRetCode = GusGetAutoNo(&(ResMsgH.usGCN));
			sErrorBM = STUB_SUCCESS;
        } else {
            CliMsg.sRetCode = GusBakAutoNo(ResMsgH.usGCN);
			sErrorBM = STUB_SUCCESS;
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		NHPOS_ASSERT(ResMsgH.usGCN == usGCNassign)
		CliMsg.sRetCode = sCliRetCode;
		ResMsgH.usGCN = usGCNassign;
	}

    *pusGCNumber = ResMsgH.usGCN;

    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusGetAutoNo() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusAllIdRead(USHORT usType,
*                                       USHORT *pusRcvBuffer,
*                                       USHORT usSize);
*     InPut:    usType  - 0:counter type, 1:drive through
*               usSize  - size of buffer
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read all Guest Check Number.
*===========================================================================
*/
SHORT	CliGusAllIdRead(USHORT usType,
						USHORT *pusRcvBuffer,
						USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFREADAGCNO;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = (UCHAR *)pusRcvBuffer;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusAllIdRead(usType, pusRcvBuffer, usSize);

    } else if (CliMsg.sRetCode > 0) {
        if ((USHORT)CliMsg.sRetCode > usSize) {
            CliMsg.sRetCode = usSize;
        }
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusResetReadFlag(USHORT usGCNumber, USHORT usType);
*     InPut:    usGCNumber  - GCN to reset read flag
*               usType      - Queue Number
*
** Return:      GCF_SUCCESS:    reset read flag
*               GCF_NOT_IN: GCN not in file			
*               GCF_ALL_LOCKED: during reporting
*
** Description: This function supports to reset read flag on GCF.
*===========================================================================
*/
SHORT   CliGusResetReadFlag(GCNUM usGCNumber, USHORT usType)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError, sBRetCode = 0;            
    SHORT       sCliRetCode, sErrorBM = 0, sErrorM = 0;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFRESETFG | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusResetReadFlag(usGCNumber, usType);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = GusResetReadFlag(usGCNumber, usType);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusResetReadFlag() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusIndRead(  USHORT usGCNumber,
*                                       UCHAR *puchRcvBuffer,
*                                       USHORT usSize);
*     InPut:    usGCNumber  - GCN
*               usSize  - size of buffer
*    OutPut:    *puchRcvBuffer  - receive buffer pointer to read
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read a Guest Check File.
*===========================================================================
*/
SHORT   CliGusIndRead(GCNUM usGCNumber,
                        UCHAR *puchRcvBuffer,
                        USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
           
    CliMsg.usFunCode    = CLI_FCGCFINDREADRAM;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusIndRead(usGCNumber, puchRcvBuffer, usSize);
    } 
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusAllIdReadBW(USHORT usType,
*                                       ULONG ulWaiterNo,
*                                       USHORT *pusRcvBuffer,
*                                       USHORT usSize);
*     InPut:    usType  - 0:counter type, 1:drive through
*               usSize  - size of buffer
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read all Guest Check Number by Waiter, V3.3.
*===========================================================================
*/
SHORT	CliGusAllIdReadBW(USHORT usType,
                        ULONG  ulWaiterNo,
						USHORT *pusRcvBuffer,
						USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCType    = usType;
    ReqMsgH.ulWaiterNo  = ulWaiterNo;
           
    CliMsg.usFunCode    = CLI_FCGCFALLIDBW;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = (UCHAR *)pusRcvBuffer;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusAllIdReadBW(usType, ulWaiterNo, pusRcvBuffer, usSize);

    } else if (CliMsg.sRetCode > 0) {
        if ((USHORT)CliMsg.sRetCode > usSize) {
            CliMsg.sRetCode = usSize;
        }
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

SHORT	CliGusAllIdReadFiltered(USHORT usType,
								ULONG ulWaiterNo,
								USHORT usTableNo,
								GCF_STATUS_STATE *pusRcvBuffer,
								USHORT usCount)
{
	CLIREQGCFFILTERED    ReqMsgH = {0};
	CLIRESGCF            ResMsgH = {0};
    SHORT                sError;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));

    ReqMsgH.usGCType    = usType;
    ReqMsgH.ulWaiterNo  = ulWaiterNo;
	ReqMsgH.usTableNo  = usTableNo;
           
    CliMsg.usFunCode    = CLI_FCGCREADFILTERED;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCFFILTERED);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = (UCHAR *)pusRcvBuffer;
    CliMsg.usResLen     = usCount * sizeof(GCF_STATUS_STATE);

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusAllIdReadFiltered(usType, ulWaiterNo, usTableNo, pusRcvBuffer, usCount);

    } else if (CliMsg.sRetCode > 0) {
        if ((USHORT)CliMsg.sRetCode > usCount) {
            CliMsg.sRetCode = usCount;
        }
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

SHORT	CliGusSearchForBarCode(USHORT usType,
                        UCHAR *uchUniqueIdentifier,
						GCF_STATUS_STATE *pusRcvBuffer,
						USHORT usCount)
{
	CLIREQGCFFILTERED   ReqMsgH = {0};
	CLIRESGCF           ResMsgH = {0};
    SHORT               sError;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));

    ReqMsgH.usGCType    = usType;
    memcpy (ReqMsgH.uchUniqueIdentifier, uchUniqueIdentifier, 24);
           
    CliMsg.usFunCode    = CLI_FCGCREADUNIQUEID;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCFFILTERED);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = (UCHAR *)pusRcvBuffer;
    CliMsg.usResLen     = usCount * sizeof(GCF_STATUS_STATE);

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusSearchForBarCode(usType, uchUniqueIdentifier, pusRcvBuffer, usCount);

    } else if (CliMsg.sRetCode > 0) {
        if ((USHORT)CliMsg.sRetCode > usCount) {
            CliMsg.sRetCode = usCount;
        }
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusReadLock(	USHORT usGCNumber,
*                                       UCHAR *puchRcvBuffer,
*                                       USHORT usSize,
*                                       USHORT usType)         R3.0
*     InPut:    usGCNumber  - GCN
*               usSize  - size of buffer
*    OutPut:    *puchRcvBuffer	- receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN: GCN not in file			
*               GCF_LOCKED: already read
*               GCF_ALL_LOCKED: during reporting
*
** Description: This function supports to read and to lock Guest Check File.
*===========================================================================
*/
SHORT   SerGusReadLock(GCNUM usGCNumber,
                        UCHAR *puchRcvBuffer,
                        USHORT usSize,
                        USHORT usType)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFREADLOCK | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = GusReadLock(usGCNumber, puchRcvBuffer, usSize, usType);
    } else {
        sRetCode = GCF_NOT_MASTER;
    }
    if (0 <= sRetCode) {
        CliMsg.usFunCode = CLI_FCGCFLOCK | CLI_FCWBACKUP | CLI_FCSERVERWB;
        CliMsg.usResLen = 0;
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusDelete(USHORT usGCNumber);
*     InPut:    usGCNumber  - GCN to delete
*
** Return:      GCF_SUCCESS:    deleted
*               GCF_NOT_IN: GCN not in file			
*
** Description: This function supports to delete Guest Check File.
*===========================================================================
*/
SHORT   SerGusDelete(GCNUM usGCNumber)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
           
    CliMsg.usFunCode    = CLI_FCGCFDELETE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = GusDelete(usGCNumber);
    } else {
        sRetCode = GCF_NOT_MASTER;
    }
    if (GCF_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusIndRead(  USHORT usGCNumber,
*                                       UCHAR *puchRcvBuffer,
*                                       USHORT usSize);
*     InPut:    usGCNumber  - GCN
*               usSize  - size of buffer
*    OutPut:    *puchRcvBuffer  - receive buffer pointer to read
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read a Guest Check File.
*===========================================================================
*/
SHORT   SerGusIndRead(GCNUM usGCNumber,
                        UCHAR *puchRcvBuffer,
                        USHORT usSize)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return GusIndRead(usGCNumber, puchRcvBuffer, usSize);
    } else //US CUSTOMS SCER For Release 2.X JHHJ 7-25-05
	//If we are not the master, then send a request to the master for the information required
	{
		return CliGusIndRead(usGCNumber, puchRcvBuffer, usSize);
	}

    return GCF_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusAllIdRead(USHORT usType,
*                                       USHORT *pusRcvBuffer,
*                                       USHORT usSize);
*     InPut:    usType  - 0:counter type, 1:drive through
*               usSize  - size of buffer
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read all Guest Check Number.
*===========================================================================
*/
SHORT   SerGusAllIdRead(USHORT usType,
                        USHORT *pusRcvBuffer,
                        USHORT usSize)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return GusAllIdRead(usType, pusRcvBuffer, usSize);
    } else //US CUSTOMS SCER For Release 2.X JHHJ 7-25-05
	//If we are not the master, then send a request to the master for the information required
	{
		return  CliGusAllIdRead(usType, pusRcvBuffer, usSize);
	}
    return GCF_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusAllLock(VOID);
*
** Return:      GCF_SUCCESS:    all locked
*               GCF_LOCKED:
*               GCF_DUR_LOCKED:	
*
** Description: This function supports to lock all Guest Check File.
*===========================================================================
*/
SHORT   SerGusAllLock(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return GusAllLock();
    } 
    return GCF_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusAllUnLock(VOID);
*
** Return:      GCF_SUCCESS:    all unlocked
*
** Description:  This function supports to unlock all Guest Check File.
*===========================================================================
*/
SHORT   SerGusAllUnLock(VOID)
{
    return GusAllUnLock();
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusAssignNo(USHORT usGCAssignNo);
*     InPut:    usGCAssignNo    - GCN to assign
*
** Return:      GCF_SUCCESS:    assigned
*
** Description: This function supports to assign GCN.
*===========================================================================
*/
SHORT   SerGusAssignNo(GCNUM usGCAssignNo)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCAssignNo;
           
    CliMsg.usFunCode    = CLI_FCGCFASSIGN | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = GusAssignNo(usGCAssignNo);
    } else {
        sRetCode = GCF_NOT_MASTER;
    }
    if (GCF_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusResetReadFlag(USHORT usGCNumber);
*     InPut:    usGCNumber  - GCN to reset read flag
*
** Return:      GCF_SUCCESS:    reset read flag
*               GCF_NOT_IN: GCN not in file			
*               GCF_ALL_LOCKED: during reporting
*
** Description: This function supports to reset read flag on GCF.
*===========================================================================
*/
SHORT	SerGusResetReadFlag(GCNUM usGCNumber)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = GCF_NO_APPEND;
           
    CliMsg.usFunCode    = CLI_FCGCFRESETFG | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = GusResetReadFlag(usGCNumber, GCF_NO_APPEND);
    } else {
        sRetCode = GCF_NOT_MASTER;
    }
    if (GCF_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusCheckExist(VOID);
*
** Return:      GCF_SUCCESS:    not exist GCF
*               GCF_EXIST:      exist GCF
*               GCF_ALL_LOCKED: during reporting
*
** Description: This function supports to check that GCF exist or not.
*===========================================================================
*/
SHORT	SerGusCheckExist(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return GusCheckExist();
    } 
    return GCF_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusAllIdReadBW(USHORT usType,
*                                           USHORT usWaiterNo;
*                                           USHORT *pusRcvBuffer,
*                                           USHORT usSize);
*     InPut:    usType  - 0:counter type, 1:drive through
*               usSize  - size of buffer
*               usWaiterNo  - waiter number want to be read GCF
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description:  
*  This function supports to read Guest Check File by waiter number.
*===========================================================================
*/
SHORT	SerGusAllIdReadBW(USHORT   usType,
                            ULONG  ulWaiterNo,
                            USHORT  *pusRcvBuffer,
                            USHORT usSize)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return GusAllIdReadBW(usType, ulWaiterNo, pusRcvBuffer, usSize);
    } else //US CUSTOMS SCER For Release 2.X JHHJ 7-25-05
	//If we are not the master, then send a request to the master for the information required
	{
		return CliGusAllIdReadBW(usType, ulWaiterNo, pusRcvBuffer, usSize);
	}
    return GCF_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusReadAllLockedGCN(USHORT *pusRcvBuffer,
*                                             USHORT usSize);
*
*     InPut:    usSize         - receive buffer size
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read All Guest Check Number locked.
*===========================================================================
*/
SHORT	SerGusReadAllLockedGCN(USHORT *pusRcvBuffer,
                               USHORT usSize)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return GusReadAllLockedGCN(pusRcvBuffer, usSize);
    } 
    return GCF_NOT_MASTER;
}

/*              
*===========================================================================
** Synopsis:    SHORT   CliGusStoreFileFH(USHORT usType,
*                                         USHORT usGCNumber,
*                                         SHORT  hsFileHandle,
*                                         USHORT usStartPoint,
*                                         USHORT usSize);
*     InPut:    usType  - 0:conter type, 1:drive through
*               usGCNumber  - GCN
*               hsFileHandle    - file handle of file to be stored
*               usStartPoint    - start pointer of stored data 
*               usSize  - size of file
*
** Return:      GCF_SUCCESS:    stored
*				GCF_FILE_FULL:  file full
*               GCF_NOT_LOCKED: has not been read
*               GCF_ALL_LOCKED: during reporting
*
** Description: This function supports to store Guest Check File
*               (file handle I/F version).
*===========================================================================
*/
SHORT	CliGusStoreFileFH(USHORT usType,
                          GCNUM usGCNumber,
                          SHORT  hsFileHandle,
                          ULONG  ulStartPoint,
                          ULONG  ulSize)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError;            
    SHORT       sCliRetCode;
	SHORT		sErrorBM = STUB_SUCCESS, sErrorM = STUB_SUCCESS;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFSTORE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchReqData = ( UCHAR *)&hsFileHandle;
    CliMsg.usReqLen     = ulSize;

    if (STUB_SELF == (sErrorM = CstSendMasterFH(!CLI_FCBAKGCF))) {
        CliMsg.sRetCode = GusStoreFileFH(usType, usGCNumber, hsFileHandle, ulStartPoint, ulSize);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMasterFH(!CLI_FCBAKGCF))) {
        CliMsg.sRetCode = GusStoreFileFH( usType, usGCNumber, hsFileHandle, ulStartPoint, ulSize);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		// We depend on the CstSendBMasterFH () function to handle the
		// error condition indication to the operator and the caller
		// to display an error dialog if the Master Terminal is down.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || sErrorBM) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusStoreFileFH() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusReadLockFH(USHORT usGCNumber,
*                                        SHORT  hsFileHandle,
*                                        USHORT usStartPoint, 
*                                        USHORT usSize,
*                                        USHORT usType)              R3.0
*     InPut:    usGCNumber  - GCN
*               usStartPoint - start read pointer of file
*               usSize  - size of file
*    OutPut:    *hsFileHandle  - file handle of file to read
*
** Return:      Data Length
*               GCF_NOT_IN:	GCN not in file			
*               GCF_LOCKED: already read
*               GCF_ALL_LOCKED:	during reporting
*
** Description: This function supports to read and to lock Guest Check File.
*               (file handle I/F version).
*===========================================================================
*/
SHORT	CliGusReadLockFH(GCNUM usGCNumber,
                         SHORT  hsFileHandle,
                         USHORT usStartPoint, 
                         USHORT usSize,
                         USHORT usType,
						 ULONG	*pulActualBytesRead)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError;            
    SHORT       sCliRetCode, sErrorBM = 0, sErrorM = 0;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFREADLOCK | CLI_FCWBACKUP | CLI_FCCLIENTWB
                    /*  + CLI_FCWCONFIRM */ ;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = ( UCHAR *)&hsFileHandle;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == (sErrorM = CstSendMasterFH(CLI_FCBAKGCF))) {
        CliMsg.sRetCode = GusReadLockFH(usGCNumber, hsFileHandle, usStartPoint, usSize, usType, pulActualBytesRead); 
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SUCCESS == sErrorM && CliMsg.sRetCode == GCF_SUCCESS) {
		// If the Master Terminal is not down then we just want to do a lock on the
		// guest check since we have already retrieved the guest check from the Master.
        CliMsg.usFunCode = CLI_FCGCFLOCK | CLI_FCWBACKUP | CLI_FCCLIENTWB;
        CliMsg.usResLen = usSize = 0;
    }
    if (STUB_SELF == (sErrorBM = CstSendBMasterFH(CLI_FCBAKGCF))) {
        CliMsg.sRetCode = GusReadLockFH(usGCNumber, hsFileHandle, usStartPoint, usSize, usType, pulActualBytesRead);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
			CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || sErrorBM) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusReadLockFH() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

SHORT	CliGusReturnsLockClear(GCNUM usGCNumber)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF   ReqMsgH;
    CLIRESGCF   ResMsgH;
    SHORT       sError;            
    SHORT       sCliRetCode, sErrorBM = 0, sErrorM = 0;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
           
    CliMsg.usFunCode    = CLI_FCGCRETURNSCLEAR | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == (sErrorM = CstSendMasterFH(CLI_FCBAKGCF))) {
        CliMsg.sRetCode = GusReturnsLockClear(usGCNumber); 
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SUCCESS == sErrorM && CliMsg.sRetCode == GCF_SUCCESS) {
        CliMsg.usFunCode = CLI_FCGCRETURNSCLEAR | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    }
    if (STUB_SELF == (sErrorBM = CstSendBMasterFH(CLI_FCBAKGCF))) {
        CliMsg.sRetCode = GusReturnsLockClear(usGCNumber);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
			CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || sErrorBM) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusReturnsLockClear() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusIndReadFH(USHORT usGCNumber,
*                                       SHORT  hsFileHandle,
*                                       USHORT usStartPoint, 
*                                       USHORT usSize);
*     InPut:    usGCNumber  - GCN
*               usSize  - size of file
*               usStartPoint - start read pointer of file
*    OutPut:    *puchRcvBuffer  - file handle of file to read
*
** Return:      Data Length
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to read a Guest Check File.
*               (file handle I/F version).
*===========================================================================
*/
SHORT   CliGusIndReadFH(GCNUM usGCNumber,
                        SHORT  hsFileHandle,
                        USHORT usStartPoint, 
                        USHORT usSize,
						ULONG  *pulActualBytesRead)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
           
    CliMsg.usFunCode    = CLI_FCGCFINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = ( UCHAR *)&hsFileHandle;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == SstReadFAsMasterFH(!CLI_FCBAKGCF)) {
        CliMsg.sRetCode = GusIndReadFH(usGCNumber, hsFileHandle, usStartPoint, usSize, pulActualBytesRead);
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/**************************** Add R3.0 ******************************/
/*
*===========================================================================
** Synopsis:    SHORT   CliGusInformQueue(USHORT usQueue,
*                                         USHORT usType,
*                                         UCHAR  uchOrderNo, 
*                                         UCHAR  uchForBack)
*     InPut:    usQueue    - Queue Number
*               usType     - Search Type
*               uchOrderNo - GC Number
*               uchForBack - Forward or Backward
*
*    OutPut:    Nothing
*
** Return:      Gc Number
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to Search GC from Drive through queue.
*===========================================================================
*/
SHORT   CliGusInformQueue(USHORT usQueue,
                          USHORT usType,
                          USHORT uchOrderNo, 
                          UCHAR  uchForBack)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = uchOrderNo;
    ReqMsgH.usGCType    = usType;
    ReqMsgH.usQueue     = usQueue;
    ReqMsgH.usForBack   = (USHORT)uchForBack;
           
    CliMsg.usFunCode    = CLI_FCGCFINFOQUE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusInformQueue(usQueue, usType, uchOrderNo, uchForBack);
    } 
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusDeleteDeliveryQueue(UCHAR  uchOrderNo,
*                                                USHORT usType)
*     InPut:    uchOrderNo - GC Number
*               usType     - Search Type
*
*    OutPut:    Nothing
*
** Return:      GCF_SUCCESS: deleted
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to delete GC from delivery queue.
*===========================================================================
*/
SHORT   CliGusDeleteDeliveryQueue(GCNUM  uchOrderNo,
                                  USHORT usType)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;            
    SHORT           sCliRetCode, sErrorBM = 0, sErrorM = 0;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = uchOrderNo;
    ReqMsgH.usGCType    = usType;
           
    CliMsg.usFunCode    = CLI_FCGCFDELDLIVQUE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusDeleteDeliveryQueue(uchOrderNo, usType);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = GusDeleteDeliveryQueue(uchOrderNo, usType);
		sErrorBM = STUB_SUCCESS;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusDeleteDeliveryQueue() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusSearchQueue(USHORT usQueue,
*                                         UCHAR  uchMiddleNo, 
*                                         UCHAR  uchRightNo)
*     InPut:    usQueue    - Queue Number
*               uchMiddleNo - Middle screen GC Number
*               uchRightNo - Right screen GC Number
*
*    OutPut:    Nothing
*
** Return:      GCF_SUCCESS
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to Search GC from queue.
*===========================================================================
*/
SHORT   CliGusSearchQueue(USHORT usQueue,
                          USHORT  usMiddleNo, 
                          USHORT  usRightNo)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usMiddleNo;
    ReqMsgH.usGCType    = usRightNo;
    ReqMsgH.usQueue     = usQueue;
           
    CliMsg.usFunCode    = CLI_FCGCFSEARCHQUE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusSearchQueue(usQueue, usMiddleNo, usRightNo);
    }
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusRetrieveFirstQueue(USHORT usQueue,
*                                         USHORT  usType,
*                                         GCNUM   *uchOrderNo)
*     InPut:    usQueue    - Queue Number
*               uchMiddleNo - Middle screen GC Number
*               uchRightNo - Right screen GC Number
*
*    OutPut:    Nothing
*
** Return:      GCF_SUCCESS
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to Search GC from queue.
*===========================================================================
*/
SHORT   CliGusRetrieveFirstQueue(USHORT usQueue,
                     USHORT  usType,
                     GCNUM   *uchOrderNo)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = 0;
    ReqMsgH.usGCType    = usType;
    ReqMsgH.usQueue     = usQueue;

	ResMsgH.usGCorderNo[0] = 0;
	ResMsgH.usGCorderNo[1] = 0;
           
    CliMsg.usFunCode    = CLI_FCGCRTRVFIRST2;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstReadFAsMaster()) {
        CliMsg.sRetCode = GusRetrieveFirstQueue(usQueue, usType, uchOrderNo);
    }
	else
	{
		uchOrderNo[0] = ResMsgH.usGCorderNo[0];
		uchOrderNo[1] = ResMsgH.usGCorderNo[1];
	}
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusCheckAndReadFH(USHORT usGCNumber,
*                                            USHORT usSize, 
*                                            SHORT  hsFileHandle,
*                                            USHORT usStartPoint, 
*                                            USHORT usFileSize);
*     InPut:    usGCNumber  - GCN
*               usSize - previous file size
*               usStartPoint - start read pointer of file
*               usFileSize  - size of file
*    OutPut:    *puchRcvBuffer  - file handle of file to read
*
** Return:      GCF_SUCCESS
*               GCF_NOT_IN:	GCN not in file			
*
** Description: This function supports to check size and to read a Guest Check File.
*               (file handle I/F version).
*===========================================================================
*/
SHORT   CliGusCheckAndReadFH(GCNUM usGCNumber,
                             USHORT usSize, 
                             SHORT  hsFileHandle,
                             USHORT usStartPoint, 
                             USHORT usFileSize,
							 ULONG	*pulActualBytesRead)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sError;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = usSize;
           
    CliMsg.usFunCode    = CLI_FCGCFCHECKREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = ( UCHAR *)&hsFileHandle;
    CliMsg.usResLen     = usFileSize;

    if (STUB_SELF == SstReadFAsMasterFH(!CLI_FCBAKGCF)) {
        CliMsg.sRetCode = GusCheckAndReadFH(usGCNumber, usSize, hsFileHandle, usStartPoint, usFileSize, pulActualBytesRead);
    } 
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerGusResetDeliveryQueue(VOID);
*     InPut:    Noting
*
** Return:      GCF_SUCCESS:    deleted
*
** Description: This function supports to reset delivery queue.
*===========================================================================
*/
SHORT   SerGusResetDeliveryQueue(VOID)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    CliMsg.usFunCode    = CLI_FCGCFRESETDVQUE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = GusResetDeliveryQueue();
    } else {
        sRetCode = GCF_NOT_MASTER;
    }
    if (GCF_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliGusDeliveryIndRead(USHORT usQueue,
*                      						  UCHAR *puchRcvBuffer,
*                                             USHORT usSize);
*                                             UCHAR  *puchOrder);
*     InPut:    usGCNumber  - GCN
*               usSize  - size of file
*    OutPut:    *puchRcvBuffer  - Recieve Buffer
*               *puchOrder - Order No. Store pointer
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
*               GCF_NOT_PAID: GCN not in Paid transaction			
*               GCF_CHANGE_QUEUE: change delivery queue			
*
** Description: This function supports to read a Guest Check File for Delivery Queue.
*               (file handle I/F version).
*===========================================================================
*/
SHORT   CliGusDeliveryIndRead(USHORT usQueue,
        					  UCHAR *puchRcvBuffer,
                              USHORT usSize,
                              USHORT  *puchOrder)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
    CLIRESGCF       ResMsgHSave;
    SHORT           sError;            
    SHORT           sCliRetCode;
	SHORT           sErrorBM = STUB_SUCCESS, sErrorM = STUB_SUCCESS;
	SHORT			sMoved = 0;

    PifRequestSem(husCliExeNet);
    CstInitGcfMsgH(&ReqMsgH, &ResMsgH);

    ReqMsgH.usGCType    = usQueue;
           
    CliMsg.usFunCode    = CLI_FCGCFDVINDREAD | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usSize;

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = GusDeliveryIndRead(usQueue, puchRcvBuffer, usSize, &(ResMsgH.usGCN));
		sErrorM = STUB_SUCCESS;
        sMoved = 1;
    } 

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	// Save the Master Terminal response if needed later.
	sCliRetCode = CliMsg.sRetCode;
	ResMsgHSave = ResMsgH;
    if (STUB_SUCCESS == sErrorM) {
        CliMsg.usFunCode = CLI_FCGCFDVINDREADBAK | CLI_FCWBACKUP | CLI_FCCLIENTWB;
        CliMsg.usResLen = usSize = 0;
    }
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        CliMsg.sRetCode = GusDeliveryIndRead(usQueue, puchRcvBuffer, usSize, &(ResMsgH.usGCN));
		sErrorBM = STUB_SUCCESS;
        sMoved = 1;
        if (STUB_SUCCESS == sErrorM) {
            CliMsg.sRetCode = sCliRetCode;
			ResMsgH = ResMsgHSave;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
		ResMsgH = ResMsgHSave;
	}

    *puchOrder = ResMsgH.usGCN;
    sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorM != sErrorMsave || sErrorBM != sErrorBMsave) {
			char xBuff[128];

			sprintf (xBuff, "==NOTE: CliGusDeliveryIndRead() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;    
}
/**************************** End Add R3.0 **************************/

/*===== END OF SOURCE =====*/
