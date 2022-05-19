/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Function Library
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: Totalfl.c
* --------------------------------------------------------------------------
* Abstract:
*           VOID TtlInitial()       - Total Initialize
*           VOID TtlBaseCreate()    - Create Base Total File
*           SHORT  TtlDeptCreate()  - Create Dept Total File
*           SHORT  TtlPLUCreate()   - Create PLU Total File
*           SHORT  TtlCpnCreate()   - Create Coupon Total File,      R3.0
*           SHORT  TtlCasCreate()   - Create Cashier Total File
*           SHORT  TtlTtlUpdCreate()  - Create Total Update File
*           SHORT  TtlDeptDeleteFile()  - Delete Dept Total File
*           SHORT  TtlPLUDeleteFile()   - Delete PLU Total File
*           SHORT  TtlCpnDeleteFile()   - Delete Coupon Total File,  R3.0
*           SHORT  TtlCasDeleteFile()   - Delete Cashier Total File
*           SHORT  TtlIndFinCreate()    - Create Individual Financial R3.1
*           SHORT  TtlIndFinDeleteFile()  Delete Individual Financial R3.1
*           SHORT  TtlSerTimeCreate()   - Create Service Time Total   R3.1
*           SHORT  TtlSerTimeDeleteFile() Delete Service Time Total   R3.1
*           SHORT  TtlTotalRead()   - Read Total File
*           SHORT  TtlTotalIncrRead()   - Read Total File incrementally
*           SHORT  TtlTotalReset()  - Reset Total File
*           SHORT  TtlTotalDelete() - Delete Total Data
*           SHORT  TtlIssuedSet()   - Set Report Issued Flag
*           SHORT  TtlIssuedReset() - Reset Report Issued Flag
*           SHORT  TtlIssuedCheck() - Check Report Issued Flag
*           SHORT  TtlTotalCheck()  - Check 0 Total
*           SHORT  TtlUpdateFile()  - Update Total File
*           SHORT  TtlLock()        - File Access Lock
*           VOID   TtlUnlock()      - Unlock File Access
*           SHORT  TtlReadWrtChk()  - Check read&write point in Total update File
*           SHORT  TtlUpdateFileFH() - Update Total File(File Handle I/F Version)
*           SHORT  TtlReadFileFH()  - Read Data from File
*           SHORT  TtlTtlUpdDeleteFile() - Delete Total Update File
*           SHORT  TtlWaiTotalCheck() - Check Waiter Total
*           SHORT  TtlCasTotalCheck() - Check Cashier Total
* --------------------------------------------------------------------------
* Update Histories
*    Data   : Ver.Rev. : Name       : Description
* May-06-92 : 00.00.01 : K.Terai    :
* Mar-14-94 : 00.00.04 : K.You      : add flex GC file feature.(mod. TtlTtlUpdCreate)
*           :          :            : (add TtlUpdateFileFH, TtlReadFileFH, TtlTtlUpdDeleteFile)
*           :          :            : (mod TtlInitial)
* May-17-94 : 02.05.00 : Yoshiko.Jim: add TtlWaiTotalCheck(), TtlCasTotalCheck()
* Mar- 3-95 : 03.00.00 : hkato      : R3.0
* Mar-18-96 : 03.01.00 : T.Nakahata : R3.1 Initial
*   Add Service Time Total (Daily/PTD) and Individual Terminal Financial (Daily)
*   Increase Regular Discount ( 2 => 6 )
*   Add Net Total at Register Financial Total
*   Add Some Discount and Void Elements at Server Total
* Mar-21-96 : 03.01.01 : T.Nakahata : Cashier File Size USHORT to ULONG
* Aug-11-99 : 03.05.00 : K.Iwata    : R3.5 Remove WAITER_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-14-99 : 01.00.00 : hrkato     : Saratoga
* Jun-12-00 : 01.00.00 : m.teraki   : move code for thread creation to UifCreateMain()
* Jun-12-00 : 01.00.00 : m.teraki   : add TtlShutDown()
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : removed __DEL_2172 code for PLU totals. using TTLCS_TMPBUF_WORKING.
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
#include <tchar.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <csstbpar.h>
/* #include <search.h> */
#include <csttl.h>
#include <cswai.h>
#include <cscas.h>
#include <ttl.h>
#include <regstrct.h>
#include <transact.h>
#include <appllog.h>
#include <maint.h>
#include <ej.h>
#include <rfl.h>

/*                                         
*===========================================================================
*   Extern Declarations 
*===========================================================================
*/
/* --- Total File Handle --- */
SHORT   hsTtlBaseHandle = -1;       /* Base Total File Handle */
SHORT   hsTtlDeptHandle = -1;       /* Department Total File Handle */
SHORT   hsTtlPLUHandle = -1;        /* PLU Total File Handle */
SHORT   hsTtlCpnHandle = -1;        /* Coupon Total File Handle, R3.0 */
SHORT   hsTtlCasHandle = -1;        /* Cashier Total File Handle */
SHORT   hsTtlUpHandle = -1;         /* Total Update File Handle */
SHORT	hsTtlUpHandleDB = -1;	/* Total Delayed Balance Update File Handle EOD JHHJ*/

/* ===== New Total Files (Release 3.1) BEGIN ===== */
SHORT   hsTtlIndFinHandle = -1;     /* Individual Financial Handle, R3.1 */
SHORT   hsTtlSerTimeHandle = -1;    /* Service Time Handle, R3.1 */
SHORT   hsTtlSavFileHandle = -1;

/* ===== New Total Files (Release 3.1) END ===== */

/* --- Semaphore Handle --- */
PifSemHandle  usTtlSemRW = PIF_SEM_INVALID_HANDLE;                 /* Semaphore Handle for Total File Read & Write */
PifSemHandle  usTtlSemUpdate = PIF_SEM_INVALID_HANDLE;             /* Semaphore Handle for Update(TUM) */
PifSemHandle  usTtlSemWrite = PIF_SEM_INVALID_HANDLE;              /* Semaphore Handle for Update File Read&Write */
USHORT  usTtlQueueJoinUpdate = -1;       /* Queue Handle for Update Totals after a Join from Disconnected Satellite, TtlTUMJoinDisconnectedSatellite */
USHORT  usTtlQueueUpdateTotalsMsg = -1;  /* Queue Handle for Update Totals, TtlTUMUpdateTotalsMsg */

PifSemHandle  usTtlSemDBBackup = PIF_SEM_INVALID_HANDLE;           /* Semaphore Handle for Total database File backup */

/* --- Update Lock Handle --- */
CHAR    chTtlLock = TTL_UNLOCKED;            /* Update Lock Handle */
CHAR    chTtlUpdateLock = TTL_TUM_UNLOCK;    /* Update Total File Write Lock */

/* --- File Name --- */
TCHAR FARCONST szTtlBaseFileName[] = _T("TOTALBAS");   /* Base Total File Name */
TCHAR FARCONST szTtlDeptFileName[] = _T("TOTALDEP");   /* Dept Total File Name */
TCHAR FARCONST szTtlPLUFileName[]  = _T("TOTALPLU");   /* PLU Total File Name */
TCHAR FARCONST szTtlCpnFileName[]  = _T("TOTALCPN");   /* Coupon Total File Name, R3.0 */
TCHAR FARCONST szTtlCasFileName[]  = _T("TOTALCAS");   /* Cashier Total File Name */
TCHAR FARCONST szTtlUpFileName[]   = _T("TOTALUPD");     /* Update Total File Name */
TCHAR FARCONST szTtlUpDBFileName[] = _T("TOTALUPDDB");  /* Delayed Balance Update File EOD JHHJ */
/* CHAR FARCONST szTtlTUMName[]      = _T("TOTALS");  */   /* Total Update Module */
/* ===== Add New Totals (Release 3.1) BEGIN ===== */
TCHAR FARCONST szTtlIndFileName[]  = _T("TOTALIND"); /* Individual Financial, R3.1 */
TCHAR FARCONST szTtlSerTimeName[]  = _T("TOTALTIM"); /* Serive Time, R3.1 */
/* ===== Add New Totals (Release 3.1) END ===== */

TCHAR FARCONST szTtlStoreForwardName[] = _T("TOTALSTF");   /* Store and Forward file */
TCHAR FARCONST szTtlPreauthStoreName[] = _T("TOTALPRE");   /* Preauth Tender storage awaiting Preauth Capture */

/* temporary work for TtlUpdateFileFH */

/* static VOID (THREADENTRY *pFunc)(VOID) = TtlTUMMain; */   /* Add R3.0 */

SHORT  TtlReinitialTotalFiles (VOID)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};   /* Temporary Work Buffer(1024 bytes),   Saratoga */
    SHORT        sRetvalue;
	ULONG        ulMaxRecordNumber = RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR);

	/* --- Open Base Total File --- */
        if ((sRetvalue = TtlBaseInit(uchTmpBuff)) != 0) {
            hsTtlBaseHandle = -1;   /* Set (-) to Base Total File Handle */
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
            /* Execute PifAbort,  return Not Used */
        }

	/* --- Open Update Total File --- */
        if ((sRetvalue = TtlUpdateInit(uchTmpBuff, ulMaxRecordNumber)) != 0) { /* 2172 */
            hsTtlUpHandle = -1;     /* Set (-) to Total Update File Handle */
            TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue);
            /* Execute PifAbort,  return Not Used */
        }

	/* --- Open Delayed Balance Update Total File --- EOD JHHJ */
		if ((sRetvalue = TtlUpdateInit(uchTmpBuff, ulMaxRecordNumber)) != 0) { /* 2172 */
			hsTtlUpHandle = -1;     /* Set (-) to Total Update File Handle */
			TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue);
            /* Execute PifAbort,  return Not Used */
		}

	/* --- Open Dept Total File --- */
		if (hsTtlDeptHandle >= 0) {
			if ((sRetvalue = TtlDeptInit(uchTmpBuff)) != 0) {
				hsTtlDeptHandle = -1;     /* Indicate error, set (-) to Department Total File Handle */
				TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
				/* Execute PifAbort,  return Not Used */
			}
		}

	/* --- Open PLU Total File --- */
        if ((sRetvalue = TtlPLUInit(uchTmpBuff)) != 0) {
            hsTtlPLUHandle = -1;     /* Indicate error, set (-) to PLU Total File Handle */
            TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            /* Execute PifAbort,  return Not Used */
        }

	/* --- Open Coupon Total File, R3.0 --- */
		if (hsTtlCpnHandle >= 0) {
			if ((sRetvalue = TtlCpnInit(uchTmpBuff)) != 0) {
				hsTtlCpnHandle = -1;     /* Indicate error, set (-) to Coupon Total File Handle */
				TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
				/* Execute PifAbort,  return Not Used */
			}
		}

	/* --- Open Cashier Total File --- */
		if (hsTtlCasHandle >= 0) {
			if ((sRetvalue = TtlCasInit(uchTmpBuff)) != 0) {
				hsTtlCasHandle = -1;     /* Indicate error, set (-) to Cashier Total File Handle */
				TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
				/* Execute PifAbort,  return Not Used */
			}
		}

    /* --- Open Individual Financial File --- */
        if ((sRetvalue = TtlIndFinInit(uchTmpBuff)) != 0) {
            hsTtlIndFinHandle = -1;     /* Indicate error, set (-) to Individual Financial Total File Handle */
            TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            /* Execute PifAbort,  return Not Used */
        }

    /* --- Open Service Time Total --- */
		// The Service Time measurements are only collected with a Store/Recall system.
		// Other types of systems such as Post Guest Check do not collect Service Time Measurements.
		// See function CliCheckCreateFile() for the FLEX_GC_ADR type of file where it can be
		// seen that if the type is not FLEX_STORE_RECALL then delete the Service Time totals file.
		// If the Service Times are not being measured then the file handle will be less than zero.
		if (hsTtlSerTimeHandle >= 0) {
			if ((sRetvalue = TtlSerTimeInit(uchTmpBuff)) != 0) {
				hsTtlSerTimeHandle = -1;     /* Indicate error, set (-) to Service Time Total File Handle */
				TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
				/* Execute PifAbort,  return Not Used */
			}
		}

	return 0;
}

/*
*============================================================================
**Synopsis:     VOID  TtlInitial(USHORT usPowerUpMode)
*
*    Input:     UCHAR uchPowerUpMode    - Power Up Mode data
*                   Bit 1       0: Does not clear Total
*                               1: Clear Total
*
*                   Bit 15      0: Does not load Parameter from Loader Board
*                               1: Load Parameter from Loader Board
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing 
*
** Description  This function initializes the Total function.
*
*============================================================================
*/
VOID  TtlInitial(USHORT usPowerUpMode)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};   /* Temporary Work Buffer(1024 bytes),   Saratoga */
    SHORT       hsHandle;           /* Handle Area */
    SHORT       sRetvalue, sSts, sReInitFile = 0;
	ULONG        ulMaxRecordNumber = RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR);

    usTtlSemRW = PifCreateSem(1);              /* Create Semaphore for Total File Read & Write */
//	PifSetTimeoutSem(usTtlSemRW, 8000);
//    PifHighLowSem(usTtlSemRW, (PIF_SEM_ACTION_FLAGS_REQ_ZERO | PIF_SEM_ACTION_FLAGS_REL_NONZERO), 6, 1);
    usTtlSemUpdate = PifCreateSem(0);          /* Create Semaphore for Update (TUM) */
//    PifHighLowSem(usTtlSemUpdate, (PIF_SEM_ACTION_FLAGS_REQ_ZERO | PIF_SEM_ACTION_FLAGS_REL_NONZERO), 6, 1);
    usTtlQueueJoinUpdate = PifCreateQueue(10);      /* Queue Handle for Update Totals after a Join from Disconnected Satellite, TtlTUMJoinDisconnectedSatellite */
    usTtlQueueUpdateTotalsMsg = PifCreateQueue(20); /* Queue Handle for Update Totals, TtlTUMUpdateTotalsMsg */
    usTtlSemWrite = PifCreateSem(1);           /* Create Semaphore for Update File Read & Write */
//	PifSetTimeoutSem(usTtlSemWrite, 8000);
//    PifHighLowSem(usTtlSemWrite, (PIF_SEM_ACTION_FLAGS_REQ_ZERO | PIF_SEM_ACTION_FLAGS_REL_NONZERO), 6, 1);
    usTtlSemDBBackup = PifCreateSem(1);        /* Create Semaphore for Total database File backup */

    chTtlLock = TTL_UNLOCKED;       /* Set Unlock Status */
    hsTtlBaseHandle = -1;           /* Set (-) to Base Total File Handle */
    hsTtlDeptHandle = -1;           /* Set (-) to Department Total File Handle */
    hsTtlPLUHandle = -1;            /* Set (-) to PLU Total File Handle */
    hsTtlCpnHandle = -1;            /* Set (-) to Coupon Total File Handle, R3.0 */
    hsTtlCasHandle = -1;            /* Set (-) to Cashier Total File Handle */
    hsTtlUpHandle = -1;             /* Set (-) to Total Update File Handle */
	hsTtlUpHandleDB = -1;			/* Set (-) to Total Delayed Balance Update File Handle EOD JHHJ*/
    hsTtlIndFinHandle  = -1;        /* Individual Financial, R3.1 */
    hsTtlSerTimeHandle = -1;        /* Service Time, R3.1 */

	{
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: TtlInitial() usPowerUpMode = 0x%x", usPowerUpMode);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
	}

	/* --- Open Base Total File --- */
    hsHandle = PifOpenFile(&szTtlBaseFileName[0], auchOLD_FILE_READ_WRITE_FLASH);   /* Open the Base Total File */
    if (hsHandle < 0) {
        TtlAbort(MODULE_TTL_TFL_FINANCIAL, TTL_FILE_OPEN_ERR);
        /* Execute PifAbort,  return Not Used */
    }

    hsTtlBaseHandle = hsHandle;     /* Set Base Total File Handle */
    if (usPowerUpMode & POWER_UP_CLEAR_TOTAL) {
        if ((sRetvalue = TtlBaseInit(uchTmpBuff)) != 0) {
            hsTtlBaseHandle = -1;   /* Set (-) to Base Total File Handle */
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
            /* Execute PifAbort,  return Not Used */
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlBaseHandle=%d", hsTtlBaseHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	/* --- Open Update Total File --- */
    hsHandle = PifOpenFile(&szTtlUpFileName[0], auchOLD_FILE_READ_WRITE_FLASH);   /* Open the Update Total File */
	sReInitFile = 0;
	if (hsHandle < 0) {
		hsHandle = PifOpenFile(&szTtlUpFileName[0], auchNEW_FILE_READ_WRITE_FLASH);   /* Open the Update Total File */
		sReInitFile = 1;
	}
    if (hsHandle >= 0) {
        hsTtlUpHandle = hsHandle;       /* Set Update Total File Handle */
        if (sReInitFile || (usPowerUpMode & (POWER_UP_CLEAR_TOTAL | IF_PARA_LOADED_PARA1)) != 0) {
            if ((sRetvalue = TtlUpdateInit(uchTmpBuff, ulMaxRecordNumber)) != 0) { /* 2172 */
                hsTtlUpHandle = -1;     /* Set (-) toTotal Update File Handle */
                TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlUpHandle=%d", hsTtlUpHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	/* --- Open Delayed Balance Update Total File --- EOD JHHJ */
    hsHandle = PifOpenFile(&szTtlUpDBFileName[0], auchOLD_FILE_READ_WRITE_FLASH);     /* Open the Update Total File */
	sReInitFile = 0;
	if (hsHandle < 0) {
		hsHandle = PifOpenFile(&szTtlUpDBFileName[0], auchNEW_FILE_READ_WRITE_FLASH);   /* Open the Update Total File */
		sReInitFile = 1;
	}
    if (hsHandle >= 0) {
        hsTtlUpHandleDB = hsHandle;       /* Set Update Total File Handle */
        if (sReInitFile || (usPowerUpMode & (POWER_UP_CLEAR_TOTAL | IF_PARA_LOADED_PARA1)) != 0) {
            if ((sRetvalue = TtlUpdateInit(uchTmpBuff, ulMaxRecordNumber)) != 0) { /* 2172 */
                hsTtlUpHandle = -1;     /* Set (-) toTotal Update File Handle */
                TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	/* --- Open Dept Total File --- */
    hsHandle = PifOpenFile(&szTtlDeptFileName[0], auchOLD_FILE_READ_WRITE_FLASH);     /* Open the Dept Total File */
    if (hsHandle >= 0) {
        hsTtlDeptHandle = hsHandle; /* Set Dept Total File Handle */
        if ((usPowerUpMode & POWER_UP_CLEAR_TOTAL) != 0) {
            if ((sRetvalue = TtlDeptInit(uchTmpBuff)) != 0) {
                hsTtlDeptHandle = -1;   /* Set (-) toDepartment Total File Handle */
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlDeptHandle=%d", hsTtlDeptHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	/* --- Open PLU Total File --- */
    /* hsHandle = PifOpenFile(&szTtlPLUFileName[0], auchOLD_FILE_READ_WRITE); *//* ### DEL 2172 Rel1.0 (01/14/00) */
    sSts = TtlPLUOpenDB(); /* ### ADD 2172 Rel1.0 (01/14/00) Open the PLU Total database */
    if (sSts == TTL_SUCCESS) {
        /*hsTtlPLUHandle = hsHandle;*//* Set PLU Total File Handle */
        hsTtlPLUHandle = 1; /* ### DUMMY 2172 Rel1.0 (01/14/00) */
        if (usPowerUpMode & POWER_UP_CLEAR_TOTAL) {
            if ((sRetvalue = TtlPLUInit(uchTmpBuff)) != 0) {
                hsTtlPLUHandle = -1;   /* Set (-) toPLU Total File Handle */
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	/* --- Open Coupon Total File, R3.0 --- */
    hsHandle = PifOpenFile(&szTtlCpnFileName[0], auchOLD_FILE_READ_WRITE_FLASH);   /* Open the Coupon Total File */
    if (hsHandle >= 0) {
        hsTtlCpnHandle = hsHandle;  /* Set Coupon Total File Handle */
        if (usPowerUpMode & POWER_UP_CLEAR_TOTAL) {
            if ((sRetvalue = TtlCpnInit(uchTmpBuff)) != 0) {
                hsTtlCpnHandle = -1;   /* Set (-) to Coupon Total File Handle */
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlCpnHandle=%d", hsTtlCpnHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	/* --- Open Cashier Total File --- */
    hsHandle = PifOpenFile(&szTtlCasFileName[0], auchOLD_FILE_READ_WRITE_FLASH);   /* Open the Cashier Total File */
    if (hsHandle >= 0) {
        hsTtlCasHandle = hsHandle;  /* Set Cashier Total File Handle */
        if (usPowerUpMode & POWER_UP_CLEAR_TOTAL) {
            if ((sRetvalue = TtlCasInit(uchTmpBuff)) != 0) {
                hsTtlCasHandle = -1;     /* Set (-) toCashier Total File Handle */
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlCasHandle=%d", hsTtlCasHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    /* --- Open Individual Financial File --- */
    hsHandle = PifOpenFile(&szTtlIndFileName[0], auchOLD_FILE_READ_WRITE_FLASH);
    if (hsHandle >= 0) {
        hsTtlIndFinHandle = hsHandle;   /* save individual financial handle */
        if (usPowerUpMode & POWER_UP_CLEAR_TOTAL) {
            if ((sRetvalue = TtlIndFinInit(uchTmpBuff)) != 0) {
                hsTtlIndFinHandle = -1;
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlIndFinHandle=%d", hsTtlIndFinHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    /* --- Open Service Time Total --- */
	// The Service Time measurements are only collected with a Store/Recall system.
	// Other types of systems such as Post Guest Check do not collect Service Time Measurements.
	// See function CliCheckCreateFile() for the FLEX_GC_ADR type of file where it can be
	// seen that if the type is not FLEX_STORE_RECALL then delete the Service Time totals file.
	// If the Service Times are not being measured then the TOTALTIM file will not exist.
    hsHandle = PifOpenFile(&szTtlSerTimeName[0], auchOLD_FILE_READ_WRITE_FLASH);
    if (hsHandle >= 0) {
        hsTtlSerTimeHandle = hsHandle;  /* save service time handle */
        if (usPowerUpMode & POWER_UP_CLEAR_TOTAL) {
            if ((sRetvalue = TtlSerTimeInit(uchTmpBuff)) != 0) {
                hsTtlSerTimeHandle = -1;
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
				/* Execute PifAbort,  return Not Used */
            }
        }
    }

	{
		char  xBuff[128];
		sprintf(xBuff, "        TtlInitial(): hsTtlSerTimeHandle=%d", hsTtlSerTimeHandle);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	hsHandle = PifOpenFile(szTtlStoreForwardName, auchOLD_FILE_READ_WRITE_FLASH);
	if (hsHandle < 0) {
		TTLMESSAGEUNIONHEADER  TotalHeader = {0};
		ULONG                  ulActualPosition;

		TotalHeader.ulNextItemRead = sizeof(TTLMESSAGEUNIONHEADER);
		TotalHeader.ulNextItemWrite = sizeof(TTLMESSAGEUNIONHEADER);
		TotalHeader.ulFileSize = sizeof(TTLMESSAGEUNION) * 200 + sizeof(TTLMESSAGEUNIONHEADER);
		hsHandle = PifOpenFile (szTtlStoreForwardName, auchNEW_FILE_READ_WRITE_FLASH);
		PifSeekFile (hsHandle, 0, &ulActualPosition);
		PifWriteFile (hsHandle, &TotalHeader, sizeof(TTLMESSAGEUNIONHEADER));
	}
	PifCloseFile (hsHandle);

/* --- Start TUM --- */
    /* Commented out and move to UifCreateMain() */
    /* PifBeginThread(pFunc,                      */  /* Start Address */
    /*                &achTtlTUMStack[TTL_STACK], */  /* Stack Address */
    /*                TTL_STACK,                  */  /* Stack Size */
    /*                PRIO_TOTALS,                */  /* Priority */
    /*                szTtlTUMName,               */  /* Name */
    /*                NULL);                      */ 
}
/*
*============================================================================
**Synopsis:     VOID TtlBaseCreate(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing 
*
** Description  This function creates the BASE Total File.(W/ PTD)
*
*============================================================================
*/
VOID TtlBaseCreate(VOID)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};   /* Temporary Work Buffer(1024 bytes),   Saratoga */
    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   hsHandle;           /* Handle Area */
    ULONG   ulFileSize;         /* Base Total File Size */
    SHORT   sRetvalue;

    hsHandle = PifOpenFile(&szTtlBaseFileName[0], auchNEW_FILE_READ_WRITE_FLASH);  /* Create the Base Total File */
    if (hsHandle == PIF_ERROR_FILE_EXIST) {
        return;                     /* Return success */
    }
    if (hsHandle < 0) {
        TtlAbort(MODULE_TTL_TFL_FINANCIAL, TTL_FILE_OPEN_ERR);
		/* Execute PifAbort,  return Not Used */
    }

/* --- Base Total File Creation --- */
	NHPOS_ASSERT(((TTL_FIN_SIZE * 4) + (TTL_HOUR_SIZE * 4)) <= 0xFFFFFFFF);//MAXWORD
    ulFileSize = ((TTL_FIN_SIZE * 4) + (TTL_HOUR_SIZE * 4));   /* Set Base Total File Size */
    if (PifSeekFile(hsHandle, ulFileSize, &ulActualPosition) < 0) {  /* Seek Last Pointer of Base Total File */
        PifCloseFile(hsHandle);                 /* Close Base Total File */
        PifDeleteFile(&szTtlBaseFileName[0]);   /* Delete Base Total File */
        TtlAbort(MODULE_TTL_TFL_FINANCIAL, TTL_FILE_SEEK_ERR);
		/* Execute PifAbort,  return Not Used */
    }
        
    hsTtlBaseHandle = hsHandle;     /* Set Base Total File Handle */
    if ((sRetvalue = TtlBaseInit(uchTmpBuff)) != 0) {   /* Initialize Base Total File */
        PifCloseFile(hsTtlBaseHandle);                              /* Close Base Total File */
        PifDeleteFile(&szTtlBaseFileName[0]);                       /* Delete Base Total File */
        hsTtlBaseHandle = -1;                                       /* Set (-) to Base Total File Handle */
        TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
		/* Execute PifAbort,  return Not Used */
    }

    PifCloseFile(hsTtlBaseHandle);  /* Close Base Total File */
    hsTtlBaseHandle = -1;           /* Set (-) to Base Total File Handle */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlDeptCreate(USHORT usDeptNum, BOOL fPTD, UCHAR uchMethod)
*
*    Input:     USHORT  usDeptNum        - No of creation Dept
*    Input:     BOOL    fPTD            - Exist or not exits PTD File flag
*               UCHAR   uchMethod       - Creation Method
*                           TTL_COMP_CREATE : Compulsory Create File
*                           TTL_CHECK_CREATE: Check Create File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed Dept File Creation 
*               TTL_CHECK_RAM   (-6)    - Can not Dept File Creation
*                                         (Check The RAM Size)  
*
** Description  This function creates Dept total file, 2172.
*
*============================================================================
*/
SHORT  TtlDeptCreate(USHORT usDeptNum, BOOL fPTD, UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};       /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSDEPTHEAD *pDepthd = (VOID *)uchTmpBuff;   /* Set Dept File Header Read Buffer Address */

    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   hsHandle;           /* Handle Area */
    ULONG   ulDailycursize;     /* Daily Current Dept Total Table Size */
    ULONG   ulPTDcursize;       /* PTD Current Dept Total Table Size */
    ULONG   ulFileSize;         /* Dept Total File Size */
    SHORT   sRetvalue;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

/* --- Close & Delete Dept Total File --- */
    if (uchMethod == TTL_COMP_CREATE) {
        if (hsTtlDeptHandle >= 0) {   /* Check Dept Total File Handle Exist? */
            PifCloseFile(hsTtlDeptHandle);
                                    /* Close Dept Total File */
            PifDeleteFile(&szTtlDeptFileName[0]);   
                                    /* Delete Dept Total File */
            hsTtlDeptHandle = -1;    /* Set (-) Dept Total File Handle */
        }
    } else {
        if (hsTtlDeptHandle >= 0) {  /* Check Dept Total File Handle Exist? */
            PifReleaseSem(usTtlSemRW);  
                                    /* Release Semaphore for Total File Read & Write */
            return (TTL_SUCCESS);   /* Return Success */
        }
    }

    if (usDeptNum == 0) {            /* Check Dept Number */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);       /* Return Success */
    }

/* --- Create Dept Total File --- */    
    hsHandle = PifOpenFile(&szTtlDeptFileName[0], auchNEW_FILE_READ_WRITE_FLASH);  /* Create the Dept Total File */
    if (hsHandle < 0) {
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_DEPT, TTL_FILE_OPEN_ERR);
                                    /* Execute PifAbort */
                                    /* return Not Used */
    }
    ulDailycursize = (TTL_DEPTMISC_SIZE + (sizeof(TTLCSDEPTINDX) * (ULONG)usDeptNum) +
                      (TTL_DEPTBLOCKRECORD_SIZE * (ULONG)usDeptNum) + TTL_DEPTEMP_SIZE +
                      (TTL_DEPTTOTALRECORD_SIZE * (ULONG)usDeptNum));
                                    /* Caluculate Daily Dept Total Size */
    if (fPTD) {
        ulPTDcursize = ulDailycursize;    /* Caluculate PTD Dept Total Size */
    } else {
        ulPTDcursize = 0L;
    }
    ulFileSize = TTL_DEPTHEAD_SIZE + (ulDailycursize * 2) + (ulPTDcursize * 2);
    if (PifSeekFile(hsHandle, ulFileSize, &ulActualPosition) < 0) {    /* Seek Last Pointer of Dept Total File */
        PifCloseFile(hsHandle);                                        /* Close Dept Total File */
        PifDeleteFile(&szTtlDeptFileName[0]);                          /* Delete Dept Total File */
        PifReleaseSem(usTtlSemRW);                                     /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_DEPT, LOG_ERROR_TOTALUM_DEPT_CHECKRAM);
        return (TTL_CHECK_RAM);     /* Return Check RAM */
    }

    hsTtlDeptHandle = hsHandle;      /* Save Dept Total File Handle */

/* --- Update Dept File Header --- */
    memset(pDepthd, 0, TTL_DEPTHEAD_SIZE);          /* Clear Work */
    pDepthd->ulDeptFileSize = ulFileSize;           /* Set Dept Total File Size */
    pDepthd->usMaxDept = usDeptNum;                 /* Set Number of Max Dept */

	/* --- Set Dept Daily Current Offset --- */ 
    pDepthd->ulCurDayMiscOff = TTL_DEPTHEAD_SIZE;                                                    /* Set Daily Current Dept Misc Total Offset */
    pDepthd->ulCurDayIndexOff = pDepthd->ulCurDayMiscOff + TTL_DEPTMISC_SIZE;                        /* Set Daily Current Dept Index Offset */
    pDepthd->ulCurDayBlockOff = pDepthd->ulCurDayIndexOff + (sizeof(TTLCSDEPTINDX) * usDeptNum);     /* Set Daily Current Dept Block Offset */
    pDepthd->ulCurDayEmptOff = pDepthd->ulCurDayBlockOff + (TTL_DEPTBLOCKRECORD_SIZE * usDeptNum);   /* Set Daily Current Dept Empty Offset */
    pDepthd->ulCurDayTotalOff = pDepthd->ulCurDayEmptOff + TTL_DEPTEMP_SIZE;                         /* Set Daily Current Dept Total Offset */

	/* --- Set Dept Daily Previous Offset --- */ 
    pDepthd->ulPreDayMiscOff = pDepthd->ulCurDayTotalOff + (TTL_DEPTTOTALRECORD_SIZE * usDeptNum);   /* Set Daily Previous Dept Misc Total Offset */
    pDepthd->ulPreDayIndexOff = pDepthd->ulPreDayMiscOff + TTL_DEPTMISC_SIZE;                        /* Set Daily Previous Dept Index Offset */
    pDepthd->ulPreDayBlockOff = pDepthd->ulPreDayIndexOff + (sizeof(TTLCSDEPTINDX) * usDeptNum);     /* Set Daily Previous Dept Block Offset */
    pDepthd->ulPreDayEmptOff = pDepthd->ulPreDayBlockOff + (TTL_DEPTBLOCKRECORD_SIZE * usDeptNum);   /* Set Daily Previous Dept Empty Offset */
    pDepthd->ulPreDayTotalOff = pDepthd->ulPreDayEmptOff + TTL_DEPTEMP_SIZE;                         /* Set Daily Previous Dept Total Offset */

    if (fPTD){
		/* --- Set Dept PTD Current Offset --- */ 
        pDepthd->ulCurPTDMiscOff = pDepthd->ulPreDayTotalOff + (TTL_DEPTTOTALRECORD_SIZE * usDeptNum);   /* Set PTD Current Dept Misc Total Offset */
        pDepthd->ulCurPTDIndexOff = pDepthd->ulCurPTDMiscOff + TTL_DEPTMISC_SIZE;                        /* Set PTD Current Dept Index Offset */
        pDepthd->ulCurPTDBlockOff = pDepthd->ulCurPTDIndexOff + (sizeof(TTLCSDEPTINDX) * usDeptNum);     /* Set PTD Current Dept Block Offset */
        pDepthd->ulCurPTDEmptOff = pDepthd->ulCurPTDBlockOff + (TTL_DEPTBLOCKRECORD_SIZE * usDeptNum);   /* Set PTD Current Dept Empty Offset */
        pDepthd->ulCurPTDTotalOff = pDepthd->ulCurPTDEmptOff + TTL_DEPTEMP_SIZE;                         /* Set PTD Current Dept Total Offset */

		/* --- Set Dept PTD Previous Offset --- */ 
        pDepthd->ulPrePTDMiscOff = pDepthd->ulCurPTDTotalOff + (TTL_DEPTTOTALRECORD_SIZE * usDeptNum);   /* Set PTD Previous Dept Misc Total Offset */
        pDepthd->ulPrePTDIndexOff = pDepthd->ulPrePTDMiscOff + TTL_DEPTMISC_SIZE;                        /* Set PTD Previous Dept Index Offset */
        pDepthd->ulPrePTDBlockOff = pDepthd->ulPrePTDIndexOff + (sizeof(TTLCSDEPTINDX) * usDeptNum);     /* Set PTD Previous Dept Block Offset */
        pDepthd->ulPrePTDEmptOff = pDepthd->ulPrePTDBlockOff + (TTL_DEPTBLOCKRECORD_SIZE * usDeptNum);   /* Set PTD Previous Dept Empty Offset */
        pDepthd->ulPrePTDTotalOff = pDepthd->ulPrePTDEmptOff + TTL_DEPTEMP_SIZE;                         /* Set PTD Previous Dept Total Offset */
    }

    if ((sRetvalue = TtlDeptHeadWrite(pDepthd)) != 0) {             /* Write Dept File Header */
        PifCloseFile(hsTtlDeptHandle);                              /* Close Dept Total File */
        PifDeleteFile(&szTtlDeptFileName[0]);                       /* Delete Dept Total File */
        hsTtlDeptHandle = -1;                                       /* Set (-) Dept Total File Handle */
        PifReleaseSem(usTtlSemRW);                                  /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_DEPT, LOG_ERROR_TOTALUM_FILEHEAD_ERROR);
        TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);                   /* return Not Used */
    }

	/* --- Dept Total File Initialize --- */
    if ((sRetvalue = TtlDeptInit(uchTmpBuff)) != 0) {             /* Initialize Dept Total File */
        PifCloseFile(hsTtlDeptHandle);                            /* Close Dept Total File */
        PifDeleteFile(&szTtlDeptFileName[0]);                     /* Delete Dept Total File */
        hsTtlDeptHandle = -1;                                     /* Set (-) Dept Total File Handle */
        PifReleaseSem(usTtlSemRW);                                /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);                 /* return Not Used */
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlDEPTDeleteFile(USHORT usDeptNum, BOOL fPTD, UCHAR uchMethod)
*
*    Input:     USHORT  usDeptNum        - No of creation Dept
*               BOOL    fPTD            - Exist or not exits PTD File flag
*               UCHAR   uchMethod       - Delete Method
*                           TTL_COMP_DEL : Compulsory Delete File
*                           TTL_CHECK_DEL: Check Delte File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed Dept File Creation 
*
** Description  This function deletes Dept total file, 2172.
*
*============================================================================
*/
SHORT  TtlDeptDeleteFile(USHORT usDeptNum, BOOL fPTD, UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};       /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSDEPTHEAD *pDepthd = (VOID *)uchTmpBuff;   /* Set Dept File Header Read Buffer Address */

    ULONG   ulDailycursize;     /* Daily Current Dept Total Table Size */
    ULONG   ulPTDcursize;       /* PTD Current Dept Total Table Size */
    ULONG   ulFileSize;         /* Dept Total File Size */
    SHORT   sRetvalue;
    UCHAR   uchDelFlag;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (hsTtlDeptHandle < 0) {       /* Check Dept Total File Handle */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);
                                    /* Return */
    }

	/* --- Read Dept File Header --- */
    uchDelFlag = 0;

    if (uchMethod == TTL_COMP_DEL) {
        uchDelFlag = 1;
    } else {
        if ((sRetvalue = TtlDeptHeadRead(pDepthd)) != 0) {    /* Read Dept File Header */
            PifReleaseSem(usTtlSemRW);                        /* Release Semaphore for Total File Read & Write */
            TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
        }
        ulDailycursize = (TTL_DEPTMISC_SIZE + (sizeof(TTLCSDEPTINDX) * (ULONG)usDeptNum) +
                      (TTL_DEPTBLOCKRECORD_SIZE * (ULONG)usDeptNum) + TTL_DEPTEMP_SIZE +
                      (TTL_DEPTTOTALRECORD_SIZE * (ULONG)usDeptNum));
                                    /* Caluculate Daily Dept Total Size */
        if (fPTD) {
            ulPTDcursize = ulDailycursize;/* Caluculate PTD Dept Total Size */
        } else {
            ulPTDcursize = 0L;
        }
        ulFileSize = TTL_DEPTHEAD_SIZE + (ulDailycursize * 2) + (ulPTDcursize * 2);
        if (pDepthd->ulDeptFileSize != ulFileSize) {
            uchDelFlag = 1;
        } else {
            if ((fPTD && (pDepthd->ulCurPTDMiscOff == 0)) ||
                ((fPTD == 0) && pDepthd->ulCurPTDMiscOff)) {
                uchDelFlag = 1;
            }
        }
    }
    if (uchDelFlag) {
        PifCloseFile(hsTtlDeptHandle);             /* Close Dept Total File */
        PifDeleteFile(&szTtlDeptFileName[0]);      /* Delete Dept Total File */
        hsTtlDeptHandle = -1;                      /* Set (-) Dept Total File Handle */
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT  TtlCpnCreate(USHORT usCpnNum, BOOL fPTD, uchMethod)
*
*    Input:     USHORT  usCpnNum       - No of creation coupon
*                BOOL   fPTD              - Exist or not exits PTD File flag 
*               UCHAR   uchMethod      - Creation Method
*                           TTL_COMP_CREATE : Compulsory Create File
*                           TTL_CHECK_CREATE: Check Create File
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCESS     ( 0)    - Successed Dept File Creation 
*              TTL_CHECK_RAM   (-6)    - Can not Coupon File Creation
*                                         (Check The RAM Size)  
*
** Description  This function creates Coupon total file.       R3.0
*
*============================================================================
*/
SHORT  TtlCpnCreate(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};         /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSCPNHEAD    *pCpnhd = (VOID *)uchTmpBuff;    /* Set Coupon File Header Read Buffer Address */

    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   hsHandle;           /* Handle Area */
    USHORT  usDailycursize;     /* Daily Current Coupon Total Table Size */
    USHORT  usPTDcursize;       /* PTD Current Coupon Total Table Size */
    USHORT  usFileSize;         /* Coupon Total File Size */
    SHORT   sRetvalue;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

	/* --- Close & Delete Coupon Total File --- */
    if (uchMethod == TTL_COMP_CREATE) {
        if (hsTtlCpnHandle >= 0) {                /* Check Coupon Total File Handle Exist? */
            PifCloseFile(hsTtlCpnHandle);         /* Close Coupon Total File */
            PifDeleteFile(&szTtlCpnFileName[0]);  /* Delete Coupon Total File */
            hsTtlCpnHandle = -1;                  /* Set (-) Coupon Total File Handle */
        }
    } else {
        if (hsTtlCpnHandle >= 0) {                /* Check Coupon Total File Handle Exist? */
            PifReleaseSem(usTtlSemRW);            /* Release Semaphore for Total File Read & Write */
            return (TTL_SUCCESS);                 /* Return Success */
        }
    }


    if (usCpnNum == 0) {            /* Check Coupon Number */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);       /* Return Success */
    }

	/* --- Create Coupon Total File --- */    
    hsHandle = PifOpenFile(&szTtlCpnFileName[0], auchNEW_FILE_READ_WRITE_FLASH);    /* Create the Coupon Total File */
    if (hsHandle < 0) {
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_CPN, TTL_FILE_OPEN_ERR);
    }
    usDailycursize = TTL_CPNMISC_SIZE + (sizeof(TOTAL) * usCpnNum);     /* Caluculate Daily Coupon Total Size */
    if (fPTD) {
        usPTDcursize = TTL_CPNMISC_SIZE + (sizeof(TOTAL) * usCpnNum);   /* Caluculate PTD Coupon Total Size */
    } else {
        usPTDcursize = 0;
    }
    usFileSize = TTL_CPNHEAD_SIZE + (usDailycursize * 2) + (usPTDcursize * 2);    /* Calculate All Coupon Total Size */
    if (PifSeekFile(hsHandle, (ULONG)usFileSize, &ulActualPosition) < 0) {        /* Seek Last Pointer of Coupon Total File */
        PifCloseFile(hsHandle);                                                   /* Close Coupon Total File */
        PifDeleteFile(&szTtlCpnFileName[0]);                                      /* Delete Coupon Total File */
        PifReleaseSem(usTtlSemRW);                                                /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_CPN, LOG_ERROR_TOTALUM_CPN_CHECKRAM);
        return (TTL_CHECK_RAM);     /* Return Check RAM */
    }

    hsTtlCpnHandle = hsHandle;          /* Set Coupon Total File Handle */

	/* --- Update Coupon Total Header --- */
    memset(pCpnhd, 0, TTL_CPNHEAD_SIZE);        /* Clear Work */

	/* ---  Set Daily Coupon Total Offset --- */
    pCpnhd->usCpnFileSize = usFileSize;                                      /* Set Coupon Total File Size */
    pCpnhd->usMaxCpn = usCpnNum;                                             /* Set Max Coupon Number */
    pCpnhd->usCurDayTtlOff = TTL_CPNHEAD_SIZE;                               /* Set Daily Current Coupon Total Offset */
    pCpnhd->usPreDayTtlOff = pCpnhd->usCurDayTtlOff + usDailycursize;        /* Set Daily previous Coupon Total Offset */

	/* ---  Set PTD Coupon Total Offset --- */
    if (fPTD) {
        pCpnhd->usCurPTDTtlOff = pCpnhd->usPreDayTtlOff + usDailycursize;    /* Set PTD Current Coupon Total Offset */
        pCpnhd->usPrePTDTtlOff = pCpnhd->usCurPTDTtlOff+  usPTDcursize;      /* Set PTD prevoius Coupon Total Offset */
    }
    if ((sRetvalue = TtlCpnHeadWrite(pCpnhd)) != 0) {      /* Write Coupon File Header */
        PifCloseFile(hsTtlCpnHandle);                      /* Close Coupon Total File */
        PifDeleteFile(&szTtlCpnFileName[0]);               /* Delete Coupon Total File */
        hsTtlCpnHandle = -1;                               /* Set (-) to Coupon Total File Handle */
        PifReleaseSem(usTtlSemRW);                         /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_CPN, LOG_ERROR_TOTALUM_FILEHEAD_ERROR);
        TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
    }

	/* --- Coupon Total File Initialize --- */
    if ((sRetvalue = TtlCpnInit(uchTmpBuff)) != 0) {                /* Initialize Coupon Total */
        PifCloseFile(hsTtlCpnHandle);                               /* Close Coupon Total File */
        PifDeleteFile(&szTtlCpnFileName[0]);                        /* Delete Coupon Total File */
        hsTtlCpnHandle = -1;                                        /* Set (-) to Coupon Total File Handle */
        PifReleaseSem(usTtlSemRW);                                  /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT  TtlCpnDeleteFile(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod)
*
*    Input:     USHORT  usCpnNum         - No of creation Coupon
*               BOOL       fPTD             - Exist or not exits PTD File flag 
*               UCHAR   uchMethod        - Delete Method
*                           TTL_COMP_DEL : Compulsory Delete File
*                           TTL_CHECK_DEL: Check Delte File
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCESS         ( 0)    - Successed Coupon File Delete
*
** Description  This function deletes Coupon total file.       R3.0
*
*============================================================================
*/
SHORT  TtlCpnDeleteFile(USHORT usCpnNum, BOOL fPTD, UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};         /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSCPNHEAD    *pCpnhd = (VOID *)uchTmpBuff;    /* Pointer of Coupon File Header */
    USHORT  usDailycursize;     /* Daily Current Coupon Total Table Size */
    USHORT  usPTDcursize;       /* PTD Current Coupon Total Table Size */
    USHORT  usFileSize;         /* Coupon Total File Size */
    SHORT   sRetvalue;
    UCHAR   uchDelFlag;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

    if (hsTtlCpnHandle < 0) {       /* Check Coupon Total File Handle */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);
    }

    uchDelFlag = 0;

    if (uchMethod == TTL_COMP_DEL) {
        uchDelFlag = 1;
    } else {
		/* --- Read Coupon File Header --- */
        if ((sRetvalue = TtlCpnHeadRead(pCpnhd)) != 0) {       /* Read Coupon File Header */
            PifReleaseSem(usTtlSemRW);                         /* Release Semaphore for Total File Read & Write */
            TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
        }
        usDailycursize = TTL_CPNMISC_SIZE + (sizeof(TOTAL) * usCpnNum);     /* Caluculate Daily Coupon Total Size */
        if (fPTD) {
            usPTDcursize = TTL_CPNMISC_SIZE + (sizeof(TOTAL) * usCpnNum);   /* Caluculate PTD Coupon Total Size */
        } else {
            usPTDcursize = 0;
        }
        usFileSize = TTL_CPNHEAD_SIZE + (usDailycursize * 2) + (usPTDcursize * 2);

        if (pCpnhd->usCpnFileSize != usFileSize) {
            uchDelFlag = 1;
        } else {
            if ((fPTD && (pCpnhd->usCurPTDTtlOff == 0)) ||
                ((fPTD == 0) && pCpnhd->usCurPTDTtlOff)) {
                uchDelFlag = 1;
            }
        }
    }
    if (uchDelFlag) {
        PifCloseFile(hsTtlCpnHandle);           /* Close Coupon Total File */
        PifDeleteFile(&szTtlCpnFileName[0]);    /* Delete Coupon Total File */
        hsTtlCpnHandle = -1;                    /* Set (-) Coupon Total File Handle */
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlCasCreate(USHORT usCashierNum, UCHAR uchMethod)
*
*    Input:     USHORT  usCashierNum    - No of creation Casher
*               UCHAR   uchMethod       - Creation Method
*                           TTL_COMP_CREATE : Compulsory Create File
*                           TTL_CHECK_CREATE: Check Create File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed Cashier File Creation 
*               TTL_CHECK_RAM   (-6)    - Can not Cashier File Creation
*                                         (Check The RAM Size)  
*
** Description  This function creates Cashier Total File. V3.3
*
*============================================================================
*/
SHORT  TtlCasCreate(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod)
{
	union {
		TTLCS_TMPBUF_WORKING   uchTmpBuff;     /* Temporary Work Buffer,   Saratoga */
		TTLCSCASHEAD           Cashd;          /* Set Cashier File Header Read Buffer Address */
	} u;

    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   hsHandle;           /* Handle Area */
    ULONG   ulDailycursize;     /* Daily Current Cashier Total Table Size */
    ULONG   ulPTDcursize;       /* PTD Current Waiter Total Table Size */
    ULONG   ulFileSize;         /* Cashier Total File Size */
    SHORT   sRetvalue;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

/* --- Close & Delete Cashier Total File --- */
    if (uchMethod == TTL_COMP_CREATE) {
        if (hsTtlCasHandle >= 0) {                 /* Check Cashier Total File Handle Exist? */
            PifCloseFile(hsTtlCasHandle);          /* Close Cashier Total File */
            PifDeleteFile(&szTtlCasFileName[0]);   /* Delete Cashier Total File */
            hsTtlCasHandle = -1;                   /* Set (-) Cashier Total File Handle */
        }
    } else {
        if (hsTtlCasHandle >= 0) {                 /* Check Cashier Total File Handle Exist? */
            PifReleaseSem(usTtlSemRW);             /* Release Semaphore for Total File Read & Write */
            return (TTL_SUCCESS);                  /* Return Success */
        }
    }

    if (usCashierNum == 0) {        /* Check Number of Cashier */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);       /* Return Success */
    }

	/* --- Create Cashier Total File --- */    
    hsHandle = PifOpenFile(&szTtlCasFileName[0], auchNEW_FILE_READ_WRITE_FLASH);    /* Create the Cashier Total File */
    if (hsHandle < 0) {
        PifReleaseSem(usTtlSemRW);                             /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_CASHIER, TTL_FILE_OPEN_ERR);
    }
    ulDailycursize = (ULONG)((sizeof(TTLCSCASINDX) * (ULONG)usCashierNum) + TTL_CASEMP_SIZE + (TTL_CASTTL_SIZE * (ULONG)usCashierNum));
                                                       /* Caluculate Daily Cashier Total Size */
    if (fPTD) {
        ulPTDcursize = ulDailycursize;                 /* Caluculate PTD Cashier Total Size, V3.3 */
    } else {
        ulPTDcursize = 0L;
    }
    ulFileSize = ( ULONG )( TTL_CASHEAD_SIZE + (ulDailycursize * 2) + (ulPTDcursize * 2));   /* Calculate Cashier Total Size */
    if (PifSeekFile(hsHandle, ulFileSize, &ulActualPosition) < 0) {                          /* Seek Last Pointer of Cashier Total File */
        PifCloseFile(hsHandle);                                                              /* Close Cashier Total File */
        PifDeleteFile(&szTtlCasFileName[0]);                                                 /* Delete Cashier Total File */
        PifReleaseSem(usTtlSemRW);                                                           /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_CASHIER, LOG_ERROR_TOTALUM_CAS_CHECKRAM);
        return (TTL_CHECK_RAM);     /* Return Check RAM */
    }

    hsTtlCasHandle = hsHandle;      /* Save Cashier Total File Handle */

	/* --- Update Cashier Total Header --- **/
    memset(&u.Cashd, 0, TTL_CASHEAD_SIZE);       /* Clear Work */
    u.Cashd.ulCasFileSize = ulFileSize;        /* Set Cashier Total File Size */
    u.Cashd.usMaxCas = usCashierNum;           /* Set Number of Max Cashier */

	/* --- Set Daily Current Cashier Total Offset --- */
    u.Cashd.ulCurDayIndexOff = TTL_CASHEAD_SIZE;                                                        /* Set Daily Current Cashier Total Offset */
    u.Cashd.ulCurDayEmptOff = u.Cashd.ulCurDayIndexOff + (sizeof(TTLCSCASINDX) * (ULONG)usCashierNum);  /* Set Daily Current Cashier Empty Table Offset */
    u.Cashd.ulCurDayTotalOff = u.Cashd.ulCurDayEmptOff + TTL_CASEMP_SIZE;                               /* Set Daily Current Cashier Total Offset */

	/* --- Set Daily Previous Cashier Total Offset --- */
    u.Cashd.ulPreDayIndexOff = u.Cashd.ulCurDayTotalOff + (TTL_CASTTL_SIZE * (ULONG)usCashierNum);      /* Set Daily Current Cashier Total Offset */
    u.Cashd.ulPreDayEmptOff = u.Cashd.ulPreDayIndexOff + (sizeof(TTLCSCASINDX) * (ULONG)usCashierNum);  /* Set Daily Current Cashier Empty Table Offset */
    u.Cashd.ulPreDayTotalOff = u.Cashd.ulPreDayEmptOff + TTL_CASEMP_SIZE;                               /* Set Daily Current Cashier Total Offset */

    if (fPTD) {
		/* --- Set PTD Current Cashier Total Offset, V3.3 --- */
        u.Cashd.ulCurPTDIndexOff = u.Cashd.ulPreDayTotalOff + (TTL_CASTTL_SIZE * (ULONG)usCashierNum);      /* Set Daily Current Cashier Total Offset */
        u.Cashd.ulCurPTDEmptOff = u.Cashd.ulCurPTDIndexOff + (sizeof(TTLCSCASINDX) * (ULONG)usCashierNum);  /* Set Daily Current Cashier Empty Table Offset */
        u.Cashd.ulCurPTDTotalOff = u.Cashd.ulCurPTDEmptOff + TTL_CASEMP_SIZE;                               /* Set Daily Current Cashier Total Offset */

        /* --- Set PTD Previous Cashier Total Offset --- */
        u.Cashd.ulPrePTDIndexOff = u.Cashd.ulCurPTDTotalOff + (TTL_CASTTL_SIZE * (ULONG)usCashierNum);      /* Set Daily Current Cashier Total Offset */
        u.Cashd.ulPrePTDEmptOff = u.Cashd.ulPrePTDIndexOff + (sizeof(TTLCSCASINDX) * (ULONG)usCashierNum);  /* Set Daily Current Cashier Empty Table Offset */
        u.Cashd.ulPrePTDTotalOff = u.Cashd.ulPrePTDEmptOff + TTL_CASEMP_SIZE;                               /* Set Daily Current Cashier Total Offset */
    }

    if ((sRetvalue = TtlCasHeadWrite(&u.Cashd)) != 0) {    /* Write Cashier File Header */
        PifCloseFile(hsTtlCasHandle);                    /* Close Cashier Total File */
        PifDeleteFile(&szTtlCasFileName[0]);             /* Delete Cashier Total File */
        hsTtlCasHandle = -1;                             /* Set (-) Cashier Total File Handle */
        PifReleaseSem(usTtlSemRW);                       /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_CASHIER, LOG_ERROR_TOTALUM_FILEHEAD_ERROR);
        TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
    }

	/* --- Cashier Total File Initialize --- */
    if ((sRetvalue = TtlCasInit(u.uchTmpBuff)) != 0) {             /* Initialize Cashier Total File */
        PifCloseFile(hsTtlCasHandle);                            /* Close Cashier Total File */
        PifDeleteFile(&szTtlCasFileName[0]);                     /* Delete Cashier Total File */
        hsTtlCasHandle = -1;                                     /* Set (-) Cashier Total File Handle */
        PifReleaseSem(usTtlSemRW);                               /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlCasDeleteFile(USHORT usCashierNum, UCHAR uchMethod)
*
*    Input:     USHORT  usCashierNum    - No of creation Casher
*               UCHAR   uchMethod       - Delete Method
*                           TTL_COMP_DEL : Compulsory Delete File
*                           TTL_CHECK_DEL: Check Delte File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed Cashier File Creation 
*
** Description  This function creates Cashier Total File.
*
*============================================================================
*/
SHORT  TtlCasDeleteFile(USHORT usCashierNum, BOOL fPTD, UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};      /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSCASHEAD *pCashd = (VOID  *)uchTmpBuff;   /* Set Cashier File Header Read Buffer Address */
    ULONG   ulDailycursize;     /* Daily Current Cashier Total Table Size */
    ULONG  ulPTDcursize;       /* PTD Current Waiter Total Table Size */
    ULONG   ulFileSize;         /* Cashier Total File Size */
    SHORT   sRetvalue;
    UCHAR   uchDelFlag;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);
    }

	/* --- Read Cashier File Header --- */
    uchDelFlag = 0;

    if (uchMethod == TTL_COMP_DEL) {
        uchDelFlag = 1;
    } else {
        if ((sRetvalue = TtlCasHeadRead(pCashd)) != 0) {   /* Read Cashier File Header */
            PifReleaseSem(usTtlSemRW);                     /* Release Semaphore for Total File Read & Write */
            TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
        }
    
        ulDailycursize = (ULONG)((sizeof(TTLCSCASINDX) * (ULONG)usCashierNum) + TTL_CASEMP_SIZE + (TTL_CASTTL_SIZE * (ULONG)usCashierNum)); /* Caluculate Daily Cashier Total Size */
        if (fPTD) {
            ulPTDcursize = ulDailycursize;       /* Caluculate PTD Waiter Total Size */
        } else {
            ulPTDcursize = 0L;
        }
        ulFileSize = ( ULONG )( TTL_CASHEAD_SIZE + (ulDailycursize * 2) + (ulPTDcursize * 2));     /* Calculate Cashier Total Size */
        if (pCashd->ulCasFileSize != ulFileSize) {
            uchDelFlag = 1;
        } else {
            if ((fPTD && (pCashd->ulCurPTDIndexOff == 0L)) || ((fPTD == 0) && pCashd->ulCurPTDIndexOff)) {
                uchDelFlag = 1;
            }
        }
    }

    if (uchDelFlag) {
        PifCloseFile(hsTtlCasHandle);             /* Close Cashier Total File */
        PifDeleteFile(&szTtlCasFileName[0]);      /* Delete Cashier Total File */
        hsTtlCasHandle = -1;                      /* Set (-) Cashier Total File Handle */
    }

    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}
/*********************** Add New Functions (Release 3.1) BEGIN **************/
/*
*============================================================================
**Synopsis:     VOID TtlIndFinCreate(VOID)
*
*    Input:     Nothing
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function creates individual financial file.
*
*============================================================================
*/
VOID TtlIndFinCreate(VOID)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};        /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSINDHEAD *pIndFinhd = (VOID *)uchTmpBuff;   /* Set Individual Financial Header Read Buffer Address */
    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   hsHandle;           /* Handle Area */
    USHORT  usDailycursize;     /* Daily Current Total Table Size */
    USHORT  usFileSize;         /* Total File Size */
    SHORT   sRetvalue;
    USHORT  usTermNum;          /* No. of Terminals in a Cluster */
    /* SYSCONFIG CONST FAR * pSysConfig; */

    hsHandle = PifOpenFile(&szTtlIndFileName[0], auchNEW_FILE_READ_WRITE_FLASH);
                                    /* Create the Individual Financial Total File */
    if (hsHandle == PIF_ERROR_FILE_EXIST) {
        hsTtlIndFinHandle = PifOpenFile( &szTtlIndFileName[ 0 ], auchOLD_FILE_READ_WRITE_FLASH );
        return;                             /* Return success */
    }
    if (hsHandle < 0) {
        TtlAbort(MODULE_TTL_TFL_INDFIN, TTL_FILE_OPEN_ERR);
    }

    /* --- get no. of settelites --- */
    usTermNum = TTL_MAX_INDFIN; /* always creates max. terminal file, saratoga */

/**
    pSysConfig = PifSysConfig();
    usTermNum  = pSysConfig->ausOption[3] + 1;  / + 1... master term /

    if ( TTL_MAX_INDFIN < usTermNum ) {
        usTermNum = TTL_MAX_INDFIN;
    }
**/
    usDailycursize = ((sizeof(TTLCSINDINDX) * usTermNum) + TTL_INDEMP_SIZE + (TTL_INDTTL_SIZE * usTermNum));
    usFileSize = TTL_INDHEAD_SIZE + (usDailycursize * 2);

    if (PifSeekFile(hsHandle, (ULONG)usFileSize, &ulActualPosition) < 0) {   /* Seek Last Pointer of Individual Financial File */
        PifCloseFile(hsHandle);                                              /* Close Individual Financail File */
        PifDeleteFile(&szTtlIndFileName[0]);                                 /* Delete Individual Financial File */
        TtlAbort(MODULE_TTL_TFL_FINANCIAL, TTL_FILE_SEEK_ERR);
    }

    hsTtlIndFinHandle = hsHandle;      /* Save File Handle */

    /* --- Update Individual Financial Header --- */
    memset(pIndFinhd, 0, TTL_INDHEAD_SIZE);       /* Clear Work */
    pIndFinhd->usIndFinFileSize = usFileSize;     /* Set Total File Size */
    pIndFinhd->usMaxTerminal    = usTermNum;      /* Set Number of Max Terminal */

    /* --- Set Daily Current Individual Financial Total Offset --- */
    pIndFinhd->usCurDayIndexOff = TTL_INDHEAD_SIZE;
    pIndFinhd->usCurDayEmptOff  = pIndFinhd->usCurDayIndexOff + (sizeof(TTLCSINDINDX) * usTermNum);
    pIndFinhd->usCurDayTotalOff = pIndFinhd->usCurDayEmptOff + TTL_INDEMP_SIZE;

    /* --- Set Daily Previous Cashier Total Offset --- */
    pIndFinhd->usPreDayIndexOff = pIndFinhd->usCurDayTotalOff + (TTL_INDTTL_SIZE * usTermNum);
    pIndFinhd->usPreDayEmptOff  = pIndFinhd->usPreDayIndexOff + (sizeof(TTLCSINDINDX) * usTermNum);
    pIndFinhd->usPreDayTotalOff = pIndFinhd->usPreDayEmptOff + TTL_INDEMP_SIZE;

    if ((sRetvalue = TtlIndFinHeadWrite(pIndFinhd)) != 0) {  
        PifCloseFile(hsTtlIndFinHandle);
        PifDeleteFile(&szTtlIndFileName[0]);    
        hsTtlIndFinHandle = -1;
        TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
    }

    /* --- Individual Financial Total File Initialize --- */
    if ((sRetvalue = TtlIndFinInit(uchTmpBuff)) != 0) {
        PifCloseFile(hsTtlIndFinHandle);    /* Close Cashier Total File */
        PifDeleteFile(&szTtlIndFileName[0]);
        hsTtlIndFinHandle = -1;             /* Set (-) File Handle */
        TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
    }
/*    PifCloseFile(hsTtlIndFinHandle);
    hsTtlIndFinHandle = -1;*/
}

/*
*============================================================================
**Synopsis:     SHORT  TtlIndFinDeleteFile(USHORT usTermNum, UCHAR uchMethod)
*
*    Input:     USHORT  usTermNum       - No. of Terminal
*               UCHAR   uchMethod       - Delete Method
*                           TTL_COMP_DEL : Compulsory Delete File
*                           TTL_CHECK_DEL: Check Delte File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed File Deletion
*
** Description  This function deletes individual financial file.
*
*============================================================================
*/
SHORT  TtlIndFinDeleteFile(USHORT usTermNum, UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};        /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSINDHEAD *pIndFinhd = (VOID *)uchTmpBuff;   /* Set Individual Financial Header Read Buffer Address */
    USHORT  usDailycursize;     /* Daily Current Individual Financial Table Size */
    USHORT  usFileSize;         /* Individual Financial File Size */
    SHORT   sRetvalue;
    UCHAR   uchDelFlag;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (hsTtlIndFinHandle < 0) {    /* Check Individual Financial File Handle */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);       /* Return */
    }

    /* --- Read Individual Financial File Header --- */
    uchDelFlag = 0;

    if (uchMethod == TTL_COMP_DEL) {
        uchDelFlag = 1;
    } else {
        if ((sRetvalue = TtlIndFinHeadRead(pIndFinhd)) != 0) {
            PifReleaseSem(usTtlSemRW);                    /* Release Semaphore for Total File Read & Write */
            TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);   /* Execute PifAbort */
        }
    
        usDailycursize = ((sizeof(TTLCSINDINDX) * usTermNum) + TTL_INDEMP_SIZE + (TTL_INDTTL_SIZE * usTermNum));
        usFileSize     = TTL_INDHEAD_SIZE + (usDailycursize * 2);

        if (pIndFinhd->usIndFinFileSize != usFileSize) {
            uchDelFlag = 1;
        }
    }
    if (uchDelFlag) {
        PifCloseFile(hsTtlIndFinHandle);        /* Close Individual Financial File */
        PifDeleteFile(&szTtlIndFileName[0]);    /* Delete Individual Financial File */
        hsTtlIndFinHandle = -1;                 /* Set (-) Total File Handle */
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}

/*
*==========================================================================
**    Synopsis:   VOID TtlIndFinCheckDelete(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:  Check Individual Financial File size,
*                   if not equal the delete the File.
*==========================================================================
*/
VOID TtlIndFinCheckDelete(VOID)
{
    TTLCSINDHEAD    IndFinhd;           /* Individual Financial Header */
    /* SYSCONFIG CONST FAR * pSysConfig;   / System configlation */
    USHORT          usTermNum;          /* Current no. of terminals */

    /* --- retrieve file header, if invidividual financial file exists --- */
    if ( hsTtlIndFinHandle < 0 ) {
        hsTtlIndFinHandle = PifOpenFile( &szTtlIndFileName[ 0 ], auchOLD_FILE_READ_WRITE_FLASH );

        if ( hsTtlIndFinHandle == PIF_ERROR_FILE_NOT_FOUND) {
            return;
        }
        if ( hsTtlIndFinHandle < 0 ) {
            TtlAbort( MODULE_TTL_TFL_INDFIN, TTL_FILE_OPEN_ERR );
        }
    }

    TtlIndFinHeadRead( &IndFinhd );
    PifCloseFile( hsTtlIndFinHandle );
    hsTtlIndFinHandle = -1;

    /* --- get current no. of terminals in cluster --- */
    usTermNum = TTL_MAX_INDFIN; /* always creates max. terminal file, saratoga */
    
/**
    pSysConfig = PifSysConfig();
    usTermNum  = pSysConfig->ausOption[ 3 ] + 1;    / + 1... master term /
**/

    if ( IndFinhd.usMaxTerminal != usTermNum ) {
        PifDeleteFile( &szTtlIndFileName[ 0 ] );
    }
}


/*
*============================================================================
**Synopsis:     SHORT  TtlSerTimeCreate(UCHAR uchMethod);
*
*    Input:     UCHAR   uchMethod       - Creation Method
*                           TTL_COMP_CREATE : Compulsory Create File
*                           TTL_CHECK_CREATE: Check Create File
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed File Creation 
*               TTL_CHECK_RAM   (-6)    - Can not File Creation
*                                         (Check The RAM Size)  
*
** Description  This function creates Service Time Total File.(W/ PTD)
*
*============================================================================
*/
SHORT  TtlSerTimeCreate(UCHAR uchMethod)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};   /* Temporary Work Buffer(1024 bytes),   Saratoga */
    ULONG   ulActualPosition;   /* Actual Seek Pointer */
    SHORT   hsHandle;           /* Handle Area */
    USHORT  usFileSize;         /* Service Time File Size */
    SHORT   sRetvalue;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

    /* --- Close & Delete Service Time File --- */
    if (uchMethod == TTL_COMP_CREATE) {
        if (hsTtlSerTimeHandle >= 0) {  /* Check Service Time Handle Exist? */
            PifCloseFile(hsTtlSerTimeHandle);
            PifDeleteFile(&szTtlSerTimeName[0]); 
            hsTtlSerTimeHandle = -1;
        }
    } else {
        if (hsTtlSerTimeHandle >= 0) {  /* Check Service Time Handle Exist? */
            PifReleaseSem(usTtlSemRW);
            return (TTL_SUCCESS);   /* Return Success */
        }
    }

    /* --- Create Individual Financial File --- */    
    hsHandle = PifOpenFile(&szTtlSerTimeName[0], auchNEW_FILE_READ_WRITE_FLASH);
    if (hsHandle < 0) {
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        TtlAbort(MODULE_TTL_TFL_SERTIME, TTL_FILE_OPEN_ERR);
    }

    usFileSize = (TTL_SERTTL_SIZE * 4);

    if (PifSeekFile(hsHandle, (ULONG)usFileSize, &ulActualPosition) < 0) { 
                                    /* Seek Last Pointer of Service Time File */
        PifCloseFile(hsHandle);
        PifDeleteFile(&szTtlSerTimeName[0]);    
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_SERTIME, LOG_ERROR_TOTALUM_SER_CHECKRAM);
        return (TTL_CHECK_RAM);     /* Return Check RAM */
    }

    hsTtlSerTimeHandle = hsHandle;  /* Save Service Time Total File Handle */

    /* --- Service Time File Initialize --- */
    if ((sRetvalue = TtlSerTimeInit(uchTmpBuff)) != 0) {
        PifCloseFile(hsTtlSerTimeHandle);    /* Close Service Time Total File */
        PifDeleteFile(&szTtlSerTimeName[0]);
        hsTtlSerTimeHandle = -1;            /* Set (-) Service Time Total File Handle */
        PifReleaseSem(usTtlSemRW);          /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_SERTIME, LOG_ERROR_TOTALUM_FILEHEAD_ERROR);
        TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT  TtlSerTimeDeleteFile(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS     ( 0)    - Successed File Deletion
*
** Description  This function deletes service time file.
*
*               The Service Time measurements are only collected with a Store/Recall system.
*               Other types of systems such as Post Guest Check do not collect Service Time Measurements.
*               See function CliCheckCreateFile() for the FLEX_GC_ADR type of file where it can be
*               seen that if the type is not FLEX_STORE_RECALL then delete the Service Time totals file.
*               If the Service Times are not being measured then the file handle will be less than zero.
*============================================================================
*/
SHORT  TtlSerTimeDeleteFile(VOID)
{
    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (hsTtlSerTimeHandle < 0) {   /* Check Service Time File Handle */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
        return (TTL_SUCCESS);       /* Return */
    }

    PifCloseFile(hsTtlSerTimeHandle);       /* Close Service Time File */
    PifDeleteFile(&szTtlSerTimeName[0]);    /* Delete Service Time File */
    hsTtlSerTimeHandle = -1;                /* Set (-) Total File Handle */
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(TTL_SUCCESS);            /* Return Success */
}
/*********************** Add New Functions (Release 3.1) END **************/

/*
*============================================================================
**Synopsis:     SHORT   TtlTtlUpdCreate(USHORT usNoofItem)
*
*    Input:     USHORT  usNoofItem  - number of item 
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing 
*
** Description  This function creates Total Update File.
*
*============================================================================
*/
SHORT TtlTtlUpdCreate(USHORT usNoofItem)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};   /* Temporary Work Buffer(1024 bytes),   Saratoga */
    ULONG   ulActualPosition;              // Actual Seek Pointer
    SHORT   hsHandle;                      // Handle Area
    SHORT   sRetvalue;
    ULONG   ulFileSize;                    // Update Total File Size
	SHORT	i;
	struct {
		TCHAR	*aszFileName;             // pointer to file name
		SHORT   *phsHandle;               // pointer to global file handle
	} stTotalFiles [4] = {0};

	// Setup the data structures to be used in the loop following.
	// We are manipulating several files in the same way.
	stTotalFiles[0].aszFileName = szTtlUpFileName;
	stTotalFiles[0].phsHandle = &hsTtlUpHandle;
	stTotalFiles[1].aszFileName = szTtlUpDBFileName;
	stTotalFiles[1].phsHandle = &hsTtlUpHandleDB;

    /* check update status */
    PifRequestSem(usTtlSemWrite);   // SHORT TtlTtlUpdCreate(USHORT usNoofItem)

	for(i = 0; stTotalFiles[i].aszFileName; i++)
	{
		/* Request Semaphore for Update Total File Read & Write */
		hsHandle = PifOpenFile(stTotalFiles[i].aszFileName, auchNEW_FILE_READ_WRITE_FLASH);  /* Create the Base Total File */
		if (hsHandle == PIF_ERROR_FILE_EXIST)
		{
			if ((TtlUphdata.usReadPointer == TtlUphdata.usWritePointer) &&
				((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) == 0))
			{ 
				;				/* Check Update Total File Empty */
			} else {
				PifReleaseSem(usTtlSemWrite);   
				return(LDT_PROHBT_ADR);     
			} 

			PifCloseFile(*(stTotalFiles[i].phsHandle));

			PifDeleteFile(stTotalFiles[i].aszFileName);     
			hsHandle = PifOpenFile(stTotalFiles[i].aszFileName, auchNEW_FILE_READ_WRITE_FLASH);
		}

		if (hsHandle < 0)
		{
			PifReleaseSem(usTtlSemWrite);   
			TtlAbort(MODULE_TTL_TFL_UPDATE, TTL_FILE_OPEN_ERR);
		}

		/* calculate file size */
		if ((ulFileSize = TrnCalStoSize(usNoofItem, FLEX_ITEMSTORAGE_ADR)) < TTL_UPDATE_FILESIZE)
		{
			ulFileSize = TTL_UPDATE_FILESIZE;
		}

		if(i == 1) //we are creating the delay balance update file. it needs to be larger than a regular one.
		{
			ulFileSize *= STD_TTLUPDDB_SIZE_MOD;
		}

		/* Set Update Total File Size(? bytes) */
		sRetvalue = PifSeekFile(hsHandle, ulFileSize, &ulActualPosition);

		if ((sRetvalue < 0) || (ulFileSize != ulActualPosition))
		{ 
			PifCloseFile(hsHandle);     
			PifDeleteFile(stTotalFiles[i].aszFileName); 
			PifReleaseSem(usTtlSemWrite);   
			return(TTL_FAIL);
		}

		*(stTotalFiles[i].phsHandle) = hsHandle;       /* Set Update Total File Handle */

		/* Update Total File Initialize */
		if ((sRetvalue = TtlUpdateInit(uchTmpBuff, usNoofItem)) != 0)
		{
			PifCloseFile(*(stTotalFiles[i].phsHandle));

			PifDeleteFile(stTotalFiles[i].aszFileName);
			*(stTotalFiles[i].phsHandle) = -1; 
			        
			PifReleaseSem(usTtlSemWrite);   
			TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue);
		}
	}

		/* PifCloseFile(hsTtlUpHandle); */   
		/* hsTtlUpHandle = -1; */
    PifReleaseSem(usTtlSemWrite);   // SHORT TtlTtlUpdCreate(USHORT usNoofItem)
    return(TTL_SUCCESS);
}

/*
*============================================================================
**Synopsis:     SHORT  TtlTotalRead(VOID *pTotal)
*
*    Input:     VOID    *pTOTAL     - Major & Minor Class, Number    
*
*   Output:     VOID    *pTOTAL     - Read Total Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed the Reading File 
*               TTL_NOTINFILE       - Not In File(PLU or Cashier or Waiter)
*               TTL_LOCKED          - File Locked
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_SIZEOVER        - Max Size Over
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error(Dept or PLU or Cashier or Waiter)
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error(Execute PifAbort, Fin or Hourly)
*
** Description  This function reads the Total File.            R3.0
*
*============================================================================
*/
static SHORT  TtlTotalReadBody (VOID *pTotal, USHORT *pDataLen)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};   /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSREPORTHEAD *pReport = pTotal;
    SHORT  sRetvalue;

    switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLREGFIN:          /* Read Financial Total File */
        sRetvalue = TtlTreadFin(pTotal);
		*pDataLen = sizeof(TTLREGFIN);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
        }
        break;

    case CLASS_TTLHOURLY:          /* Read Hourly Total File */
        sRetvalue = TtlTreadHour(pTotal);
		*pDataLen = sizeof(TTLHOURLY);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
        }
        break;

   case CLASS_TTLDEPT:                /* Read Dept Total File */
        sRetvalue = TtlTreadDept(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, (USHORT)pReport->Dept.usDEPTNumber);
		*pDataLen = sizeof(TTLDEPT);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLPLU:              /* Read PLU Total File */
        sRetvalue = TtlTreadPLU(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
		*pDataLen = sizeof(TTLPLU);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
   case CLASS_TTLCPN:                /* Read Coupon Total File, R3.0 */
        sRetvalue = TtlTreadCpn(pTotal);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
		*pDataLen = sizeof(TTLCPN);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLCASHIER:          /* Read Cashier Total File */
        sRetvalue = TtlTreadCas(pTotal);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
		*pDataLen = sizeof(TTLCASHIER);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Read Service Time Total */
        sRetvalue = TtlTreadSerTime(pTotal);
		*pDataLen = sizeof(TTLSERTIME);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
            }
        }
        break;

    case CLASS_TTLINDFIN:           /* Read Individual Financial File */
        sRetvalue = TtlTreadIndFin(pTotal);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
		*pDataLen = sizeof(TTLREGFIN);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;
    /* ===== New Functions (Release 3.1) END ===== */

    default:
		*pDataLen = 0;
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
		PifLog(MODULE_TTL_TFL_MAJOR, pReport->Rpt.uchMajorClass);
        break;
    }
    return(sRetvalue);              /* Return */
}

SHORT  TtlTotalRead(VOID *pTotal, USHORT *pDataLen)
{
	SHORT  sRetvalue = 0;

	PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

	PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
	if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
		PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		return (TTL_LOCKED);        /* Return Locked */
	}

	sRetvalue = TtlTotalReadBody(pTotal, pDataLen);

    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		static UCHAR uchMajorClassLast = 0;
		static UCHAR uchMinorClassLast = 0;
		static SHORT sRetvalueLast = 0;

		TTLCSREPORTHEAD *pReport = pTotal;

		if (uchMajorClassLast != pReport->Rpt.uchMajorClass || uchMinorClassLast != pReport->Rpt.uchMinorClass || sRetvalueLast != sRetvalue) {
			char  xBuff[128];

			sprintf (xBuff, "==NOTE: TtlTotalRead() uchMajorClass %d  uchMinorClass %d  sRetValue %d", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue);
			NHPOS_NONASSERT_TEXT(xBuff)
			uchMajorClassLast = pReport->Rpt.uchMajorClass;
			uchMinorClassLast = pReport->Rpt.uchMinorClass;
			sRetvalueLast = sRetvalue;
		}
	}

	return sRetvalue;
}

SHORT  TtlTotalReadNoSemCheck(VOID *pTotal, USHORT *pDataLen)
{
	SHORT  sRetvalue = 0;

	sRetvalue = TtlTotalReadBody(pTotal, pDataLen);
	if (sRetvalue < 0) {
		static UCHAR uchMajorClassLast = 0;
		static UCHAR uchMinorClassLast = 0;
		static SHORT sRetvalueLast = 0;

		TTLCSREPORTHEAD *pReport = pTotal;

		if (uchMajorClassLast != pReport->Rpt.uchMajorClass || uchMinorClassLast != pReport->Rpt.uchMinorClass || sRetvalueLast != sRetvalue) {
			char  xBuff[128];

			sprintf(xBuff, "==NOTE: TtlTotalRead() uchMajorClass %d  uchMinorClass %d  sRetValue %d", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue);
			NHPOS_NONASSERT_TEXT(xBuff)
				uchMajorClassLast = pReport->Rpt.uchMajorClass;
			uchMinorClassLast = pReport->Rpt.uchMinorClass;
			sRetvalueLast = sRetvalue;
		}
	}

	return sRetvalue;
}

/*
*============================================================================
**Synopsis:     SHORT  TtlTotalIncrRead(VOID *pTotal)
*
*    Input:     VOID    *pTOTAL     - Major & Minor Class, Number    
*
*   Output:     VOID    *pTOTAL     - Read Total Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed the Reading File 
*               TTL_NOTINFILE       - Not In File(PLU or Cashier or Waiter)
*               TTL_LOCKED          - File Locked
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_SIZEOVER        - Max Size Over
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error(Dept or PLU or Cashier or Waiter)
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error(Execute PifAbort, Fin or Hourly)
*
** Description  This function reads the Total File incrementally.            R3.0
*				this function was added to support reading the Hourly Total file
*				incrementally via PCIF since the entire file is too large to
*				read at one time. The TtlTotalRead function is used to read the Hourly report
*				file from Supervisor mode and calls TtlTreadHour. This function modifies it to 
*				call TtlTreadIncrHour which support sending the Hourly total 
*				information in blocks Otherwise, the rest of the Major Class 
*				function calls are the same.- 7/28/2004 - cdh
*============================================================================
*/
SHORT  TtlTotalIncrRead(VOID *pTotal)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};    /* Temporary Work Buffer(1024 bytes),   Saratoga */
    TTLCSREPORTHEAD *pReport = pTotal;
     SHORT  sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return (TTL_LOCKED);        /* Return Locked */
    }

	switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLREGFIN:          /* Read Financial Total File */
        sRetvalue = TtlTreadFin(pTotal);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
        }
        break;

    case CLASS_TTLHOURLY:          /* Read Hourly Total File */
        sRetvalue = TtlTreadIncrHour(pTotal, (VOID *)uchTmpBuff);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
        }
        break;

   case CLASS_TTLDEPT:                /* Read Dept Total File */
        sRetvalue = TtlTreadDept(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, (USHORT)pReport->Dept.usDEPTNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLPLU:              /* Read PLU Total File */
        sRetvalue = TtlTreadPLU(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
   case CLASS_TTLCPN:                /* Read Coupon Total File, R3.0 */
        sRetvalue = TtlTreadCpn(pTotal);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLCASHIER:          /* Read Cashier Total File */
        sRetvalue = TtlTreadCas(pTotal);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Read Service Time Total */
        sRetvalue = TtlTreadSerTime(pTotal);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
            }
        }
        break;


    case CLASS_TTLINDFIN:           /* Read Individual Financial File */
        sRetvalue = TtlTreadIndFin(pTotal);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;
    /* ===== New Functions (Release 3.1) END ===== */

    default:
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlTotalIncrRead() uchMajorClass %d  uchMinorClass %d  sRetValue %d", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return(sRetvalue);              /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlTotalReset(VOID *pTotal, USHORT usMDCBit)
*
*    Input:     VOID    *pTOTAL     - Major & Minor Class, Number    
*               USHORT  usMDCBit    - Execution Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed the Reseting File 
*               TTL_NOTISSUED       - Disable reset Report Status
*               TTL_LOCKED          - File Locked
*               TTL_NOTCLASS        - Set Illigal Class No
*               TTL_NOTPTD          - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function resets the Total File.              R3.0
*
*============================================================================
*/                                     
SHORT  TtlTotalReset(VOID *pTotal, USHORT usMDCBit)
{
	TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};    /* Temporary Work Buffer(512 bytes) */
    TTLCSREPORTHEAD *pReport = pTotal;
    SHORT  sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return (TTL_LOCKED);        /* Return Locked */
    }

	PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_TOTALRESET_START);
	PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), pReport->Rpt.uchMajorClass);

    switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLEODRESET:         /* Reset EOD Total File */
        sRetvalue = TtlTresetEODPTD(usMDCBit, CLASS_TTLCURDAY, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_FINANCIAL, sRetvalue, 0);       /* Record Log,  Saratoga */
        break;

    case CLASS_TTLPTDRESET:         /* Reset PTD Total File */
        sRetvalue = TtlTresetEODPTD(usMDCBit, CLASS_TTLCURPTD, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_FINANCIAL, sRetvalue, 0);       /* Record Log,  Saratoga */
        break;

    case CLASS_TTLPLU:              /* Reset PLU Total File */
        sRetvalue = TtlTresetPLU(pReport->Rpt.uchMinorClass, pTotal);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
    case CLASS_TTLCPN:               /* Reset Coupon Total File, R3.0 */
        sRetvalue = TtlTresetCpn(pReport->Rpt.uchMinorClass, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
            }
        }
        break;
    
    case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
        sRetvalue = TtlTresetCas(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Reset Service Time File */
        sRetvalue = TtlTresetSerTime(pReport->Rpt.uchMinorClass, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_SERTIME, sRetvalue, pReport->RegFin.usTerminalNumber);          /* Record Log */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
            }
        }
        break;


    case CLASS_TTLINDFIN:           /* Reset Individual Financial File */
        sRetvalue = TtlTresetIndFin(usMDCBit, pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);          /* Record Log */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;
    /* ===== New Functions (Release 3.1) END ===== */
        
    default:
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlTotalReset() uchMajorClass %d  uchMinorClass %d  sRetValue %d  usMDCBit 0x%x", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue, usMDCBit);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return(sRetvalue);              /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlTotalDelete(VOID *pTotal)
*
*    Input:     VOID  *pTOTAL           - Major & Minor Class, Number    
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES      - Successed the Deletting File 
*               TTL_NOTINFILE   - Not In File
*               TTL_LOCKED      - File Locked
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function deletes the Total File.
*
*============================================================================
*/
SHORT  TtlTotalDelete(VOID *pTotal) 
{
    TTLCS_TMPBUF_WORKING  uchTmpBuff = {0};  /* Temporary Work Buffer(512 bytes) */
    TTLCSREPORTHEAD  *pReport = pTotal;
    SHORT            sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
	    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return (TTL_LOCKED);        /* Return Locked */
    }

    switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
        sRetvalue = TtlTDeleteCas(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLINDFIN:           /* Delete Individual Financial */
        sRetvalue = TtlTDeleteIndFin(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) END ===== */

    case CLASS_TTLPLU:              /* Reset PLU Total File */
        sRetvalue = TtlTDeletePLU(pReport->Rpt.uchMinorClass, pTotal);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, pReport->Rpt.uchMinorClass);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;

    default:
        sRetvalue = TTL_NOTCLASS; 
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlTotalDelete() uchMajorClass %d  uchMinorClass %d  sRetValue %d", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return(sRetvalue);              /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlIssuedSet(VOID *pTotal, UCHAR uchStat)
*
*    Input:     VOID  *pTotal           - Major & Minor Class, Number    
*               UCHAR uchStat           - Flag
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES      - Successed the setting Reset Enable Flag  
*               TTL_NOTINFILE   - Not In File(Cashier or Waiter)
*               TTL_LOCKED      - File Locked
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function updates the Rest Issued Flag.       R3.0
*
*============================================================================
*/
SHORT  TtlIssuedSet(VOID *pTotal, UCHAR uchStat)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};  /* Temporary Work Buffer(512 bytes) */
    TTLCSREPORTHEAD  *pReport = pTotal;
    SHORT            sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
   		PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return (TTL_LOCKED);        /* Return Locked */
    }

	switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLREGFIN:       /* Set Reset Enable flag Financial Total File */
        sRetvalue = TtlTIssuedFin(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_SETFLAG);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
        }
        break;

    case CLASS_TTLHOURLY:       /* Set Reset Enable flag Hourly Total File */
        sRetvalue = TtlTIssuedHour(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_SETFLAG);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
        }
        break;

    case CLASS_TTLDEPT:         /* Set Reset Enable flag Dept Total File */
        sRetvalue = TtlTIssuedDept(pTotal, uchTmpBuff, uchStat, TTL_SETFLAG);
        TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, (USHORT)pReport->Dept.usDEPTNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLPLU:          /* Set Reset Enable flag PLU Total File */
        sRetvalue = TtlTIssuedPLU(pTotal, uchTmpBuff, uchStat, TTL_SETFLAG);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
			// we pass on the error if there is a PLU totals file open error as this is a fundamental file.
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
     case CLASS_TTLCPN:               /* Set Reset Enable flag Coupon Total File, R3.0 */
        sRetvalue = TtlTIssuedCpn(pTotal, uchTmpBuff, uchStat, TTL_SETFLAG);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
			} else {
				// coupon file has not been opened or does not exist so just pretend things are ok.
				sRetvalue = TTL_SUCCESS;  // replace TTL_FILE_HANDLE_ERR error with TTL_SUCCESS
            }
        }
        break;
   
    case CLASS_TTLCASHIER:      /* Set Reset Enable flag Cashier Total File */
        sRetvalue = TtlTIssuedCas(pTotal, uchTmpBuff, uchStat, TTL_SETFLAG);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
			// we pass on the error if there is a cashier totals file open error as this is a fundamental file.
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    
    case CLASS_TTLSERVICE:      /* Set Reset Enable flag Service Time Total */
        sRetvalue = TtlTIssuedSerTime(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_SETFLAG);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
				TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
			} else {
				// service file has not been opened or does not exist so just pretend things are ok.
				sRetvalue = TTL_SUCCESS;  // replace TTL_FILE_HANDLE_ERR error with TTL_SUCCESS
            }
        }
        break;

    case CLASS_TTLINDFIN:       /* Set Reset Enable flag Individual Financial File */
        sRetvalue = TtlTIssuedIndFin(pTotal, uchTmpBuff, uchStat, TTL_SETFLAG);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
			// we pass on the error if there is a individual financial totals file open error as this is a fundamental file.
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) END ===== */

    default:
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlIssuedSet() uchMajorClass %d  uchMinorClass %d  sRetValue %d  uchStat 0x%x", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue, uchStat);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return(sRetvalue);              /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlIssuedReset(VOID *pTotal, UCHAR uchStat)
*
*    Input:     VOID  *pTotal           - Major & Minor Class, Number    
*               UCHAR uchStat           - Flag
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES      - Successed the setting Reset Enable Flag  
*               TTL_NOTINFILE   - Not In File(Cashier or Waiter)
*               TTL_LOCKED      - File Locked
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function updates the Rest Issued Flag.       R3.0
*
*============================================================================
*/
SHORT  TtlIssuedReset(VOID *pTotal, UCHAR uchStat)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};  /* Temporary Work Buffer(512 bytes) */
    TTLCSREPORTHEAD *pReport = pTotal;
    SHORT   sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
        return (TTL_LOCKED);        /* Return Locked */
    }

	switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLREGFIN:       /* Set Reset Enable flag Financial Total File */
        sRetvalue = TtlTIssuedFin(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_RESETFLAG);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
        }
        break;

    case CLASS_TTLHOURLY:       /* Set Reset Enable flag Hourly Total File */
        sRetvalue = TtlTIssuedHour(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_RESETFLAG);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
        }
        break;

    case CLASS_TTLDEPT:         /* Set Reset Enable flag Dept Total File */
        sRetvalue = TtlTIssuedDept(pTotal, uchTmpBuff, uchStat, TTL_RESETFLAG);
        TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, (USHORT)pReport->Dept.usDEPTNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLPLU:          /* Set Reset Enable flag PLU Total File */
        sRetvalue = TtlTIssuedPLU(pTotal, uchTmpBuff, uchStat, TTL_RESETFLAG);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
     case CLASS_TTLCPN:               /* Set Reset Enable flag Coupon Total File */
        sRetvalue = TtlTIssuedCpn(pTotal, uchTmpBuff, uchStat, TTL_RESETFLAG);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
			} else {
				// coupon file has not been opened or does not exist so just pretend things are ok.
				sRetvalue = TTL_SUCCESS;  // replace TTL_FILE_HANDLE_ERR error with TTL_SUCCESS
            }
        }
        break;
   
    case CLASS_TTLCASHIER:      /* Set Reset Enable flag Cashier Total File */
        sRetvalue = TtlTIssuedCas(pTotal, uchTmpBuff, uchStat, TTL_RESETFLAG);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Set Enable Flag Service Time File */
        sRetvalue = TtlTIssuedSerTime(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_RESETFLAG);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
			} else {
				// service file has not been opened or does not exist so just pretend things are ok.
				sRetvalue = TTL_SUCCESS;  // replace TTL_FILE_HANDLE_ERR error with TTL_SUCCESS
            }
        }
        break;


    case CLASS_TTLINDFIN:           /* Set Enable Flag Individual Financial File */
        sRetvalue = TtlTIssuedIndFin(pTotal, uchTmpBuff, uchStat, TTL_RESETFLAG);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;
    /* ===== New Functions (Release 3.1) END ===== */

    default:
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlIssuedReset() uchMajorClass %d  uchMinorClass %d  sRetValue %d  uchStat 0x%x", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue, uchStat);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return(sRetvalue);              /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlIssuedCheck(VOID *pTotal, UCHAR uchStat)
*
*    Input:     VOID  *pTotal           - Major & Minor Class, Number
*               UCHAR uchStat           - Flag
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_ISSUED      - Reset Enable Status  
**              TTL_NOTISSUED   - Reset Disable Status  
*               TTL_NOTINFILE   - Not In File(Cashier or Waiter)
*               TTL_LOCKED      - File Locked
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function Checks the issued flags status.        R3.0
*
*============================================================================
*/
SHORT  TtlIssuedCheck(VOID *pTotal, UCHAR uchStat) 
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};  /* Temporary Work Buffer(512 bytes) */
    TTLCSREPORTHEAD  *pReport = pTotal;
    SHORT            sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_ASSERT_TEXT((chTtlLock != TTL_LOCKED), "TtlIssuedCheck(): total file locked.");
        return (TTL_LOCKED);        /* Return Locked */
    }

    switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLREGFIN:           /* Set Reset Enable flag Financial Total File */
        sRetvalue = TtlTIssuedFin(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_CHECKFLAG);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLREGFIN sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
        }
        break;

    case CLASS_TTLHOURLY:           /* Set Reset Enable flag Hourly Total File */
        sRetvalue = TtlTIssuedHour(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_CHECKFLAG);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLHOURLY sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
        }
        break;

    case CLASS_TTLDEPT:             /* Set Reset Enable flag Dept Total File */
        sRetvalue = TtlTIssuedDept(pTotal, uchTmpBuff, uchStat, TTL_CHECKFLAG);
        TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, (USHORT)pReport->Dept.usDEPTNumber);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLDEPT sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLPLU:              /* Set Reset Enable flag PLU Total File */
        sRetvalue = TtlTIssuedPLU(pTotal, uchTmpBuff, uchStat, TTL_CHECKFLAG);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLPLU sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
     case CLASS_TTLCPN:               /* Set Reset Enable flag Coupon Total File, R3.0 */
        sRetvalue = TtlTIssuedCpn(pTotal, uchTmpBuff, uchStat, TTL_CHECKFLAG);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLCPN sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}

		if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
			} else {
				// coupon file has not been opened or does not exist so just pretend things are ok.
				sRetvalue = TTL_ISSUED;  // replace TTL_FILE_HANDLE_ERR error with TTL_ISSUED
			}
		}
        break;
   
    case CLASS_TTLCASHIER:          /* Set Reset Enable flag Cashier Total File */
        sRetvalue = TtlTIssuedCas(pTotal, uchTmpBuff, uchStat, TTL_CHECKFLAG);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLCASHIER sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Set Enable Flag Service Time File */
        sRetvalue = TtlTIssuedSerTime(pTotal, (VOID *)uchTmpBuff, uchStat, TTL_CHECKFLAG);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLSERVICE sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
			} else {
				// service file has not been opened or does not exist so just pretend things are ok.
				sRetvalue = TTL_ISSUED;  // replace TTL_FILE_HANDLE_ERR error with TTL_ISSUED
            }
        }
        break;

    case CLASS_TTLINDFIN:           /* Set Enable Flag Individual Financial File */
        sRetvalue = TtlTIssuedIndFin(pTotal, uchTmpBuff, uchStat, TTL_CHECKFLAG);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
		{
			char xBuff[128];
			sprintf (xBuff, "TtlIssuedCheck(): CLASS_TTLINDFIN sRetvalue = %d  uchStat 0x%x", sRetvalue, (ULONG)uchStat);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) END ===== */

    default:
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlIssuedCheck() uchMajorClass %d  uchMinorClass %d  sRetValue %d  uchStat 0x%x", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue, uchStat);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return (sRetvalue);             /* Return */
}

/*
*============================================================================
**Synopsis:     SHORT  TtlIssuedCheck(VOID *pTotal, UCHAR uchStat)
*
*    Input:     VOID  *pTotal           - Major & Minor Class, Number
*               UCHAR uchStat           - Flag
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      USHORT variable containing a bit mask indicating which totals
*               are not reset.
*
** Description:  This function performs a check on the specified Totals bucket
*                to determine if the various classes of Totals are indicating that
*                they are being reset or not. The purpose of this function is to
*                perform an audit on the various totals to determine if the last
*                End of Day or other total reset was completed successfully or not.
*============================================================================
*/
USHORT TtlIssuedCheckAllReset (UCHAR  uchMinorClass)
{
	USHORT  usTotalCheck = 0;
	USHORT  usTotalIndic = 0x0001;
	SHORT  sRet = 0;
	TTLCSREPORTHEAD  uTotals;

	{
		char  xBuff[128];
		sprintf (xBuff, ">>NOTE:  BEGIN TtlIssuedCheckAllReset() audit %d.", uchMinorClass);
		NHPOS_NONASSERT_NOTE (">>NOTE", xBuff);
	}
	uTotals.Rpt.uchMajorClass = CLASS_TTLREGFIN;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLHOURLY;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLDEPT;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLPLU;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLCASHIER;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	uTotals.Rpt.ulNumber = 0;                     // indicate to check all cashiers
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLCPN;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLINDFIN;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	uTotals.Rpt.ulNumber = 0;                     // indicate to check all terminals
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	uTotals.Rpt.uchMajorClass = CLASS_TTLSERVICE;
	uTotals.Rpt.uchMinorClass = uchMinorClass;
	sRet = TtlIssuedCheck(&uTotals, TTL_NOTRESET);
	if (sRet == TTL_NOT_ISSUED) {
		usTotalCheck |= usTotalIndic;
	}
	usTotalIndic <<= 1;

	{
		char  xBuff[128];
		sprintf (xBuff, ">>        END TtlIssuedCheckAllReset() audit 0x%x.", usTotalCheck);
		NHPOS_NONASSERT_NOTE (">>NOTE", xBuff);
	}
	return usTotalCheck;
}

/*
*============================================================================
**Synopsis:     SHORT  TtlTotalCheck(VOID *pTotal)
*
*    Input:     VOID  *pTotal           - Major & Minor Class, Number    
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCESS     - Total is 0  
**              TTL_NOTZERO     - Total is not 0  
*               TTL_NOTINFILE   - Not In File(Cashier or Waiter)
*               TTL_LOCKED      - File Locked
*               TTL_NOTCLASS    - Set Illigal Class No
*               TTL_NOTPTD      - Not Exist PTD
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function Checks the Total.                      R3.0
*
*============================================================================
*/
SHORT  TtlTotalCheck(VOID *pTotal) 
{
    TTLCS_TMPBUF_WORKING    uchTmpBuff = {0};      /* Temporary Work Buffer(512 bytes) */
    TTLCSREPORTHEAD  *pReport = pTotal;
    SHORT            sRetvalue;

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
		NHPOS_ASSERT_TEXT((chTtlLock != TTL_LOCKED), "TtlTotalCheck(): total file locked.");
        return (TTL_LOCKED);        /* Return Locked */
    }

    switch(pReport->Rpt.uchMajorClass) {
    case CLASS_TTLREGFIN:           /* Check Financial Total File */
        sRetvalue = TtlTTotalCheckFin(pTotal, (VOID *)uchTmpBuff);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_FINANCIAL, sRetvalue);
        }
        break;

    case CLASS_TTLHOURLY:           /* Check Hourly Total File */
        sRetvalue = TtlTTotalCheckHour(pTotal, (VOID *)uchTmpBuff);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            TtlAbort(MODULE_TTL_TFL_HOURLY, sRetvalue);
        }
        break;

    case CLASS_TTLDEPT:             /* Check Dept Total File */
        sRetvalue = TtlTTotalCheckDept(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_DEPT, sRetvalue, (USHORT)pReport->Dept.usDEPTNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_DEPT, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLPLU:              /* Check PLU Total File */
        sRetvalue = TtlTTotalCheckPLU(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_PLU, sRetvalue, 0);       /* Record Log,  Saratoga */
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_PLU, sRetvalue);
            }
        }
        break;
    
     case CLASS_TTLCPN:               /* Check Coupon Total File, R3.0 */
        sRetvalue = TtlTTotalCheckCpn(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_CPN, sRetvalue, pReport->Cpn.usCpnNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CPN, sRetvalue);
            }
        }
        break;
   
    case CLASS_TTLCASHIER:          /* Check Cashier Total File */
        sRetvalue = TtlTTotalCheckCas(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_CASHIER, sRetvalue, (USHORT)pReport->Cashier.ulCashierNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Check Service Time Total File */
        sRetvalue = TtlTTotalCheckSerTime(pTotal, (VOID *)uchTmpBuff);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_SERTIME, sRetvalue);
            }
        }
        break;

    case CLASS_TTLINDFIN:           /* Check Individual Financial Total File */
        sRetvalue = TtlTTotalCheckIndFin(pTotal, uchTmpBuff);
        TtlLog(MODULE_TTL_TFL_INDFIN, sRetvalue, pReport->RegFin.usTerminalNumber);
        if (sRetvalue <= TTL_FILE_OPEN_ERR) {
            if (sRetvalue != TTL_FILE_HANDLE_ERR) {
                TtlAbort(MODULE_TTL_TFL_INDFIN, sRetvalue);
            }
        }
        break;

    /* ===== New Functions (Release 3.1) END ===== */
 
    default:
        sRetvalue = TTL_NOTCLASS;
        TtlLog(MODULE_TTL_TFL, sRetvalue, 0);
        break;
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
	PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */
	if (sRetvalue < 0) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: TtlIssuedCheck() uchMajorClass %d  uchMinorClass %d  sRetValue %d", pReport->Rpt.uchMajorClass, pReport->Rpt.uchMinorClass, sRetvalue);
		NHPOS_NONASSERT_TEXT(xBuff)
	}
    return (sRetvalue);             /* Return */
}
/*
*============================================================================
**Synopsis:     SHORT  TtlLock(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES       - Success Setting Lock Status
*               TTL_LOCKED       - Already Locked
*
** Description  This function locks the Total File.
*
*============================================================================
*/
SHORT TtlLock(VOID)    
{
    SHORT   Retvalue;

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if(chTtlLock == TTL_LOCKED) {
        Retvalue = TTL_LOCKED;      /* Set Already Locked Status*/
    } else {
        chTtlLock = TTL_LOCKED;     /* Set Lock Status */
        Retvalue = TTL_SUCCESS;     /* Set Success Status */
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return (Retvalue);              /* Return */
}
/*
*============================================================================
**Synopsis:     VOID  TtlUnlock(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:
*
** Return:      Nothing 
*
** Description  This function unlocks the Total File.
*
*============================================================================
*/
VOID TtlUnlock(VOID)  
{
    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    chTtlLock = TTL_UNLOCKED;       /* Set Unlock Status */
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
}

/*
*============================================================================
**Synopsis:     SHORT  TtlReadWrtChk(VOID)
*
*    Input:     
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES       Success 
*               TTl_FAIL         Fail
*
** Description  This function check read & write point in the Update Total File.
*
*============================================================================
*/
SHORT  TtlReadWrtChk(VOID)  /* check read&write point in total update file */
{
    SHORT   sRetvalue;
 
    PifRequestSem(usTtlSemWrite);  // SHORT  TtlReadWrtChk(VOID)
                                    /* Request Semaphore for Update Total File Read & Write */
    if ((TtlUphdata.usReadPointer == TtlUphdata.usWritePointer) && ((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) == 0)) {
                                    /* Check Update Total File Empty */
        sRetvalue = TTL_SUCCESS;    /* return success */
    } else {
        sRetvalue = TTL_FAIL;       /* return Fail */
    }
    PifReleaseSem(usTtlSemWrite);  // SHORT  TtlReadWrtChk(VOID)
    return(sRetvalue);              /* return */
}

/*
*============================================================================
**Synopsis:     SHORT  TtlUpdateFileFH(USHORT usOffset, SHORT hsFileHandle, USHORT usLength)
*                                   
*    Input:     USHORT usOffset     - read start pointer
*               SHORT  hsFileHandle - File Handle of Writing Data File 
*               USHORT usLength     - Writing Length
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES          - Successed Writting Update Total File 
*               TTl_FAIL            - Failed Writting Update Total File
*               TTL_BUFFER_FULL     - Update Total File Full
*
** Description  This function updates the Update Total File(File Handle I/F Version).
*
*============================================================================
*/
SHORT  TtlUpdateFileFH(ULONG ulOffset, SHORT hsFileHandle, ULONG ulLength, UCHAR *puchDelayBalance)     
{
	static USHORT  usLastPifLogCode = 0;
	static UCHAR   auchTtlWrBuff[TTL_UPDATE_WORK_SIZE];   /* V3.3 work area, access sequenced by semaphore usTtlSemWrite  */
    ULONG       ulWritelen1, ulWritelen2;
    ULONG       ulLength1;
    ULONG       offulSeekPos;
    ULONG       ulFileMaxSize;
    ULONG       ulWritePointer;
	ULONG		ulUpdateReadPointer, ulUpdateCurReadPointer, ulUpdateWritePointer;
	SHORT		sUpdateHandle;
    SHORT       sRetvalue;
	USHORT      usConsNoTemp, usGuestNoTemp = 0, uchTermTemp;
	ULONG       ulCashierIDTemp, ulStoreregNoTemp;
	UCHAR		uchUpdateFlag, uchUpdateResetFlag;
	UCHAR		uchDBFlag = MAINT_DBOFF;

    if (ulLength == 0) {
        return (TTL_FAIL);          /* Return Fail */
    }

    if (hsTtlUpHandle < 0) {
        PifLog(MODULE_TTL_TFL_UPDATE, TTL_FILE_HANDLE_ERR);
        PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_UPDATE), (USHORT)__LINE__);
        TtlAbort(MODULE_TTL_TFL_UPDATE, TTL_FILE_HANDLE_ERR);
    }

    /* get file size */
    if ((ulFileMaxSize = TrnCalStoSize(RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR), FLEX_ITEMSTORAGE_ADR)) < TTL_UPDATE_FILESIZE) { /* 2172 */
        ulFileMaxSize = TTL_UPDATE_FILESIZE;
    }

	/*
		Update the TOTALUPD file. Request Semaphore for Temporary File Read & Write 
		Also governs access to work buffer auchTtlWrBuff.
	**/
    PifRequestSem(usTtlSemWrite);      // SHORT  TtlUpdateFileFH(ULONG ulOffset, SHORT hsFileHandle, ULONG ulLength, UCHAR *puchDelayBalance)

    PifRequestSem(usTtlSemRW);         /* Request Semaphore for lock state variables of Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {     /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);     /* Release Semaphore for Temporary File Read & Write */
        PifReleaseSem(usTtlSemWrite);  /* Release Semaphore for Total File Read & Write */
        PifLog(MODULE_TTL_TFL_UPDATE, LOG_EVENT_TTL_TOTALUPDATE_LOCK_TTL);
		NHPOS_ASSERT_TEXT((chTtlLock != TTL_LOCKED), "TtlUpdateFileFH(): total file locked.");
        return (TTL_LOCKED);           /* Return Locked */
    }

    if (chTtlUpdateLock == TTL_TUM_LOCK) {
        PifReleaseSem(usTtlSemRW);     /* Release Semaphore for Temporary File Read & Write */
        PifReleaseSem(usTtlSemWrite);  /* Release Semaphore for Temporary File Read & Write */
        PifLog(MODULE_TTL_TFL_UPDATE, LOG_EVENT_TTL_TOTALUPDATE_LOCK_TUM);
		NHPOS_ASSERT_TEXT((chTtlUpdateLock != TTL_TUM_LOCK), "TtlUpdateFileFH(): total update locked.");
        return (TTL_FAIL);             /* Return Fail */
    }

    PifReleaseSem(usTtlSemRW);         /* Release Semaphore for Total File Read & Write */

	offulSeekPos = ulOffset;

    TtlReadFileFH(offulSeekPos, auchTtlWrBuff, TTL_UPDATE_WORK_SIZE, hsFileHandle, &ulLength1);

	uchDBFlag = auchTtlWrBuff[0];  // same as TtlTranDBStatus in TTLTUMTRANQUAL; value MAINT_DBOFF, MAINT_DBON

	//we now do pointer assignment depending on the 
	//type of transaction it is.  if it is a delay balance
	//transaction we will be using a different file 
	//to store the information
	if(uchDBFlag == MAINT_DBON)
	{
		ulUpdateReadPointer = TtlUphDBdata.usReadPointer;
		ulUpdateCurReadPointer = TtlUphDBdata.usCurReadPointer;
		ulUpdateWritePointer = TtlUphDBdata.usWritePointer;
		sUpdateHandle = hsTtlUpHandleDB;
		uchUpdateFlag		= TtlUphDBdata.uchTtlFlag0;
		uchUpdateResetFlag  = TtlUphDBdata.uchResetFlag;
		ulFileMaxSize *= STD_TTLUPDDB_SIZE_MOD;
		*puchDelayBalance = MAINT_DBON;
		NHPOS_ASSERT_TEXT((sUpdateHandle >= 0), "**ERROR: TtlUpdateFileFH() - (uchDBFlag == MAINT_DBON) bad handle.");
	}else
	{
		ulUpdateReadPointer = TtlUphdata.usReadPointer;
		ulUpdateCurReadPointer = TtlUphdata.usCurReadPointer;
		ulUpdateWritePointer = TtlUphdata.usWritePointer;
		sUpdateHandle = hsTtlUpHandle;
		uchUpdateFlag		= TtlUphdata.uchTtlFlag0;
		uchUpdateResetFlag  = TtlUphDBdata.uchResetFlag;
		NHPOS_ASSERT_TEXT((sUpdateHandle >= 0), "**ERROR: TtlUpdateFileFH() - (uchDBFlag != MAINT_DBON) bad handle.");
	}

	if (RflLoggingRulesTransactions()) {
#pragma pack(push, 1)
		typedef struct {
			UCHAR				TtlTranDBStatus;
			USHORT              TtlTranVli;         /* Transaction VLI */
			TRANMODEQUAL        TtlTranModeQual;    /* Mode Qualifier */
			TRANCURQUAL         TtlTranCurQual;     /* Current Qualifier */
		} TTLTUMTRANQUAL;
#pragma pack(pop)
		TTLTUMTRANQUAL   *pTum = (TTLTUMTRANQUAL *)auchTtlWrBuff;
		char             transBuff[256];

		usConsNoTemp = pTum->TtlTranCurQual.usConsNo;
		ulStoreregNoTemp = pTum->TtlTranCurQual.ulStoreregNo;
		ulCashierIDTemp = pTum->TtlTranModeQual.ulCashierID;
		uchTermTemp = pTum->TtlTranModeQual.uchTerm;
		if (usConsNoTemp < 1) {
			usConsNoTemp = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
		}

		sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%03d|0x%x|0x%x|%08d|%08d|%08d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, 0x200, 0,
			uchTermTemp,  uchUpdateFlag, uchUpdateResetFlag, ulUpdateReadPointer, ulUpdateCurReadPointer, ulUpdateWritePointer);
		PifTransactionLog (0, transBuff, __FILE__, __LINE__);
	}

	if (ulUpdateReadPointer == ulUpdateWritePointer) { 
		if (uchUpdateFlag & TTL_UPBUFF_FULL) {
			USHORT  usSemReleased = 0;
			if ((uchUpdateFlag & TTL_UPBUFF_RUNNING) == 0)
			{
				PifReleaseSem(usTtlSemUpdate);  /* Release Semaphore for TUM Start */
				usSemReleased = 1;
			}
			PifReleaseSem(usTtlSemWrite);       /* Release Semaphore for Temporary File Read & Write */
			if (usLastPifLogCode != LOG_EVENT_TTL_UPD_BUFFER_FULL_01 || usSemReleased) {
				usLastPifLogCode = LOG_EVENT_TTL_UPD_BUFFER_FULL_01;
				PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_BUFFER_FULL_01);
				PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), usSemReleased);
				PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
			}
			return (TTL_BUFFER_FULL);    /* Return Buffer Full */
		}
	}

	// Check to see if the new total information will fit into the remaining
	// room in the file.  If not, then indicate a buffer full but do not set
	// the flag since the buffer is not actually full.  A smaller guest check
	// may fit though this one won't.
	if (ulUpdateReadPointer  <=  ulUpdateWritePointer)  {
		if ((ulUpdateWritePointer + ulLength) > (ulFileMaxSize - 1)) {
			ulWritelen2 = (ulUpdateWritePointer + ulLength) % ulFileMaxSize;
			if (ulWritelen2 > ulUpdateReadPointer) {
				USHORT  usSemReleased = 0;
				/* Set Buffer Full Flag */
//						TtlUphdata.uchTtlFlag0 |= TTL_UPBUFF_FULL; 
				if ((uchUpdateFlag & TTL_UPBUFF_RUNNING) == 0)
				{
					PifReleaseSem(usTtlSemUpdate);  /* Release Semaphore for TUM Start */
					usSemReleased = 1;
				}
				PifReleaseSem(usTtlSemWrite);       /* Release Semaphore for Temporary File Read & Write */
				if (usLastPifLogCode != LOG_EVENT_TTL_UPD_BUFFER_FULL_02 || usSemReleased) {
					usLastPifLogCode = LOG_EVENT_TTL_UPD_BUFFER_FULL_02;
					PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_BUFFER_FULL_02);
					PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), usSemReleased);
				}
				PifSleep (50);
				return (TTL_BUFFER_FULL);	/* Return Buffer Full */
			}
		}
	} else {
		if ((ulUpdateWritePointer + ulLength) > ulUpdateReadPointer) {
			USHORT  usSemReleased = 0;
            /* Set Buffer Full Flag */
//					TtlUphdata.uchTtlFlag0 |= TTL_UPBUFF_FULL; 
			if ((uchUpdateFlag & TTL_UPBUFF_RUNNING) == 0)
			{
				PifReleaseSem(usTtlSemUpdate);  /* Release Semaphore for TUM Start */
				usSemReleased = 1;
			}
			PifReleaseSem(usTtlSemWrite);       /* Release Semaphore for Temporary File Read & Write */
			if (usLastPifLogCode != LOG_EVENT_TTL_UPD_BUFFER_FULL_04 || usSemReleased) {
				usLastPifLogCode = LOG_EVENT_TTL_UPD_BUFFER_FULL_04;
				PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_BUFFER_FULL_04);
				PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), usSemReleased);
			}
			return (TTL_BUFFER_FULL);	/* Return Buffer FULL */
		}
	}

	if (usLastPifLogCode != 0) {
		NHPOS_ASSERT_TEXT((usLastPifLogCode == 0), "==NOTE: TtlUpdateFileFH() Clearing usLastPifLogCode.");
		PifLog (MODULE_TTL_TFL, 0);
		PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), usLastPifLogCode);
	}
	usLastPifLogCode = 0;

	/***
	 *    Now we will transfer the guest check information from the file containing it to the TOTALUPD file
	 *    which will be read by the TOTALS thread to actually do the update of the system totals.
	 *
	 *    The TOTALUPD file is a circular buffer so when we reach the end of it, we will need to circle back
	 *    around to the begining and continue writing.
	 ***/
	ulWritePointer = ulUpdateWritePointer;         //  TtlUphdata.usWritePointer, TtlUphDBdata.usWritePointer
	while ((offulSeekPos - ulOffset) < ulLength) {
		if (ulLength - (offulSeekPos - ulOffset) < ulLength1) {
			ulLength1 = ulLength - (offulSeekPos - ulOffset);
		}

		if (ulUpdateReadPointer <= ulWritePointer)
		{
			// if the write pointer is after the read pointer then we use the rest of the file
			// then wrap to the beginning if there is extra bytes that requires a wrap around
			if ((ulWritePointer + ulLength1) > (ulFileMaxSize - 1)) {
				ulWritelen1 = ulFileMaxSize - ulWritePointer;
				ulWritelen2 = (ulWritePointer + ulLength1) % ulFileMaxSize;
			} else {
				ulWritelen1 = ulLength1;
				ulWritelen2 = 0;
			}
		}
		else
		{
			// if the write pointer is before the read pointer then we only have the space between
			// the write pointer and the read pointer that we can use.
			ulWritelen1 = ulUpdateReadPointer - ulWritePointer;
			if (ulWritelen1 >= ulLength1) {
				ulWritelen1 = ulLength1;
			}
			else {
				PifReleaseSem(usTtlSemWrite);       /* Release Semaphore for Temporary File Read & Write */
				if (usLastPifLogCode != LOG_EVENT_TTL_UPD_BUFFER_FULL_01 + 20000) {
					PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_BUFFER_FULL_01);
					PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), (USHORT)ulLength1);
					PifLog (MODULE_DATA_VALUE(MODULE_TTL_TFL), (USHORT)ulWritelen1);
					PifLog (MODULE_LINE_NO(MODULE_TTL_TFL), (USHORT)__LINE__);
					usLastPifLogCode = LOG_EVENT_TTL_UPD_BUFFER_FULL_01 + 20000;
				}
				return (TTL_BUFFER_FULL);    /* Return Buffer Full */
			}
			ulWritelen2 = 0;
		}

		/* Write Update Data, V3.3 */
		sRetvalue = TtlWriteFile(sUpdateHandle, ulWritePointer, auchTtlWrBuff, ulWritelen1);
		if (sRetvalue != 0)
		{
			PifReleaseSem(usTtlSemWrite);                /* Release Semaphore for Temporary File Read & Write */
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_UPDATE), sRetvalue);
			PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_UPDATE), (USHORT)__LINE__);
			TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue);  /* Execute PifAbort */
		}

		if (ulWritelen2 != 0)
		{
			/* Handle if file wrap around to beginning of TOTALUPD file to write Update Data, V3.3 */
			sRetvalue = TtlWriteFile(sUpdateHandle, 0L, (auchTtlWrBuff + ulWritelen1), ulWritelen2);
			if (sRetvalue != 0) {
				PifReleaseSem(usTtlSemWrite);                   /* Release Semaphore for Temporary File Read & Write */
				PifLog(MODULE_ERROR_NO(MODULE_TTL_TFL_UPDATE), sRetvalue);
				PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_UPDATE), (USHORT)__LINE__);
				TtlAbort(MODULE_TTL_TFL_UPDATE, sRetvalue); 	/* Execute PifAbort */
			}
		}

		ulWritePointer = (ulWritePointer + ulWritelen1 + ulWritelen2) % ulFileMaxSize; 	/* Update TOTALUPD Write Pointer */

		offulSeekPos += ulLength1;

        TtlReadFileFH(offulSeekPos, auchTtlWrBuff, TTL_UPDATE_WORK_SIZE, hsFileHandle, &ulLength1);

		if (ulLength1 < 1) {
			NHPOS_ASSERT (ulLength1 > 0);
			break;
		}
    }

	if(uchDBFlag == MAINT_DBON)
	{
		ULONG		ulRem;

		//we do a calculation that will give us the percentage left
		//in the file to write to, so that we dont go over.
		ulRem =   ((ulFileMaxSize - ulWritePointer) * 100) / ulFileMaxSize;
		if (TtlUphDBdata.usReadPointer == ulWritePointer) {
			TtlUphDBdata.uchTtlFlag0 |= TTL_UPBUFF_FULL;      /* Set Buffer Full Flag */
		}

		TtlUphDBdata.usWritePointer = ulWritePointer;

		if (!(TtlUphDBdata.uchTtlFlag0 & TTL_UPDBBUFF_NEARFULL) && ((ulRem + EJ_ReadPrtCtl(SLIP_NEARFULL_ADR) ) < 100L))
		{
            TtlUphDBdata.uchTtlFlag0 |= TTL_UPDBBUFF_NEARFULL;
			CliParaSaveFarData();
			PifReleaseSem(usTtlSemWrite);   /* Release Semaphore for Temporary File Read & Write */
            return TTL_NEAR_FULL;
        }
	} else {
		if (TtlUphdata.usReadPointer == ulWritePointer)
		{
			TtlUphdata.uchTtlFlag0 |= TTL_UPBUFF_FULL;        /* Set Buffer Full Flag */
		}

		TtlUphdata.usWritePointer = ulWritePointer;

		if (TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL)
		{
			PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_TTL_UPBUFF_FULL);
		}
	}

	if ((uchUpdateFlag & TTL_UPBUFF_RUNNING) == 0)
	{
		PifReleaseSem(usTtlSemUpdate);  /* Release Semaphore for TUM Start */
	}

	CliParaSaveFarData();
	if (RflLoggingRulesTransactions()) {
		char transBuff[256];

		sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%03d|0x%x|0x%x|%08d|%08d|%08d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, 0x200, 0,
			uchTermTemp, uchUpdateFlag, uchUpdateResetFlag, ulUpdateReadPointer, ulUpdateCurReadPointer, ulUpdateWritePointer);
		PifTransactionLog (0, transBuff, __FILE__, __LINE__);
	}
	PifReleaseSem(usTtlSemWrite);   // SHORT  TtlUpdateFileFH(ULONG ulOffset, SHORT hsFileHandle, ULONG ulLength, UCHAR *puchDelayBalance)
	return(TTL_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: USHORT  TtlReadFileFH(ULONG ulOffset, VOID *pData,
*                                     USHORT usSize, SHORT hsFileHandle,
									  ULONG *pulActualBytesRead)
*                                                        
*       Input:  ULONG   ulOffset - Number of bytes from the biginning of the plu file.
*               VOID    *pData - Pointer to write data buffer.
*               USHORT  usSize - Number of bytes to be written.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  length of output data 
*
**  Description:
*           Read data from interfaced file handle. V3.3
*==========================================================================
*/

SHORT TtlReadFileFH(ULONG ulOffset, VOID FAR *pData,
                     ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead)
{
    ULONG   ulActPos;


    if (!ulSize) {          /* check writed size */
        return(TTL_NO_READ_SIZE);          /* if read nothing, return */
    }

    if ((PifSeekFile(hsFileHandle, ulOffset, &ulActPos)) != PIF_OK) {
		PifLog(MODULE_TTL_TFL_UPDATE, FAULT_ERROR_FILE_SEEK);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TFL_UPDATE), (USHORT)__LINE__);
        PifAbort(MODULE_TTL_TFL_UPDATE, FAULT_ERROR_FILE_SEEK); /* if error, then abort */
    }

	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hsFileHandle, pData, ulSize, pulActualBytesRead);
	if( *pulActualBytesRead != ulSize)
	{
		return (TTL_FAIL);
	}

    return(TTL_SUCCESS);

}

/*
*==========================================================================
**    Synopsis: VOID    TtlTtlUpdDeleteFile(VOID)
*                                              
*       Input:  Nothing
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                  
**  Description:
*           Delete update total file in case of loding loader B/D.
*==========================================================================
*/
VOID TtlTtlUpdDeleteFile(VOID)
{
    /* if any data is exist in total update file, deletion not performed */
    if (TtlUphdata.usReadPointer != TtlUphdata.usWritePointer) {
        return;
    }

    if ((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) != 0) {
        return;
    }

    if (hsTtlUpHandle != -1) {          /* file exist */
        PifCloseFile(hsTtlUpHandle);  hsTtlUpHandle = -1;
		PifCloseFile(hsTtlUpHandleDB);  hsTtlUpHandleDB = -1;
        PifDeleteFile(&szTtlUpFileName[0]);
		PifDeleteFile(&szTtlUpDBFileName[0]);
    }
}

/*
*============================================================================
**Synopsis:     SHORT  TtlCasTotalCheck(VOID)
*
*    Input:     Nothing  
*
*   Output:     Nothing
*    InOut:
*
** Return:      TTL_SUCCES      - Successed the non Deletting Record 
*               TTL_DELCASWAI   - Successed the Deletting Record
*               TTL_LOCKED      - File Locked
*               TTL_FAIL        - fail error
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description  This function deletes the Cashier Record. V3.3
*
*============================================================================
*/
SHORT  TtlCasTotalCheck(VOID) 
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};      /* Temporary Work Buffer(512 bytes) */
    ULONG           ausTmpCasPara[FLEX_CAS_MAX];      /* Parameter Index Read Buffer */
	TTLCSCASHEAD    Cashd = {0};      /* Read Buffer for Cashier File Header */
    SHORT           sNoCas;
    USHORT          usMaxNo;
    USHORT          i;           
    SHORT           sRetValue = TTL_SUCCESS;           

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		NHPOS_ASSERT_TEXT((chTtlLock != TTL_LOCKED), "TtlCasTotalCheck(): total file locked.");
        return (TTL_LOCKED);        /* Return Locked */
    }
    if (hsTtlCasHandle < 0) {       /* Check Cashier Total File Handle */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		PifLog(MODULE_TTL_TUM, LOG_EVENT_TTL_CAS_FILE_HANDLE);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)abs(hsTtlCasHandle));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        return (TTL_FILE_HANDLE_ERR);
    }
    if ((sNoCas = CasAllIdRead(sizeof(ausTmpCasPara), ausTmpCasPara)) <= 0) {
        PifReleaseSem(usTtlSemRW);          /* Release Semaphore for Total File Read & Write */
        return(TTL_SUCCESS);
    }

	/* --- Read Cashier File Header --- */
    if (TtlCasHeadRead(&Cashd) != 0) {   /* Read Cas File Header */
        PifReleaseSem(usTtlSemRW);          /* Release Semaphore for Total File Read & Write */
        return (TTL_FAIL);                  /* Rteurn Error Status */
    }

	// For each of the four different types of possible Cashier totals, perform a check on the totals.
	// There are two totals that may be kept, the Daily Totals and the Period To Date Totals, and
	// each of these has a Current and a Previous (Saved) bucket. So there is Daily Totals, Current and
	// Previous, and then Period To Date Totals, Current and Previous.
    for (i = 0; i < 4; i++) {
		USHORT          usNumberOfCashiers; /* Save Number of Cashier */
		USHORT          usIndOff;           
		ULONG           ulPositionInd;      /* Save Seek Position */
		ULONG           ulPositionEmp;      /* Save Seek Position */
		ULONG           ulPositionTtl;      /* Save Seek Position */

        /* --- Set Read Total Offset --- */
        switch(i) {
        case 0:                                   /* Current Daily File */
            if (Cashd.usCurDayCas == 0) {             /* Check Number of Current Daily Cashier */
                continue;
            }
            usNumberOfCashiers = Cashd.usCurDayCas;   /* Set Number of Current Daily Cashier */
            ulPositionInd = Cashd.ulCurDayIndexOff;   /* Set Currnet Daily Index Offset */
            ulPositionEmp = Cashd.ulCurDayEmptOff;    /* Set Current Daily Empty Offset */
            ulPositionTtl = Cashd.ulCurDayTotalOff;   /* Set Current Daily Total Offset */
            break;

        case 1:                                   /* Previous Daily File */
            if (Cashd.usPreDayCas == 0) {             /* Check Number of Previous Daily Cashier */
                continue;
            }
            usNumberOfCashiers = Cashd.usPreDayCas;    /* Set Number of Previous Daily Cashier */
            ulPositionInd = Cashd.ulPreDayIndexOff;    /* Set Previous Daily Index Offset */
            ulPositionEmp = Cashd.ulPreDayEmptOff;     /* Set Previous Daily Empty Offset */
            ulPositionTtl = Cashd.ulPreDayTotalOff;    /* Set Previous Daily Total Offset */
            break;

        case 2:                                    /* Current PTD File, V3.3 */
            if (Cashd.ulCurPTDIndexOff == 0L) {        /* Check Exist PTD  Total File */
                PifReleaseSem(usTtlSemRW);             /* Release Semaphore for Total File Read & Write */
                return (sRetValue);
            }
            if (Cashd.usCurPTDCas == 0) {              /* Check Number of Current PTD Cashier */
                continue;
            }
            usNumberOfCashiers = Cashd.usCurPTDCas;    /* Set Number of Current PTD Cashier */
            ulPositionInd = Cashd.ulCurPTDIndexOff;    /* Set Currnet PTD Index Offset */
            ulPositionEmp = Cashd.ulCurPTDEmptOff;     /* Set Current PTD Empty Offset */
            ulPositionTtl = Cashd.ulCurPTDTotalOff;    /* Set Current PTD Total Offset */
            break;

        case 3:                                   /* Previous PTD File, V3.3 */
            if (Cashd.ulPrePTDIndexOff == 0L) {        /* Check Exist PTD  Total File */
                PifReleaseSem(usTtlSemRW);             /* Release Semaphore for Total File Read & Write */
                return (sRetValue);
            }
            if (Cashd.usPrePTDCas == 0) {              /* Check Number of Previous PTD Cashier */
                continue;
            }
            usNumberOfCashiers = Cashd.usPrePTDCas;    /* Set Number of Previous PTD Cashier */
            ulPositionInd = Cashd.ulPrePTDIndexOff;    /* Set Previous PTD Index Offset */
            ulPositionEmp = Cashd.ulPrePTDEmptOff;     /* Set Previous PTD Empty Offset */
            ulPositionTtl = Cashd.ulPrePTDTotalOff;     /* Set Previous PTD Total Offset */
            break;

		default:
            PifReleaseSem(usTtlSemRW);             /* Release Semaphore for Total File Read & Write */
			NHPOS_ASSERT_TEXT(0, "==ERROR: TtlCasTotalCheck() state machine loop invalid count.");
            return (TTL_FAIL);
        }

        usMaxNo = Cashd.usMaxCas;

        /* --- Search Cashier Index Table, V3.3 --- */
        for (usIndOff = 0; usIndOff < usNumberOfCashiers; ) {
			ULONG			ulIndOffset;
			TTLCSCASINDX    CasInd = {0};

            CasInd.ulCasNo = ausTmpCasPara[usIndOff];
            sRetValue = TtlCasISearch(ulPositionInd, usNumberOfCashiers, &CasInd, uchTmpBuff, &ulIndOffset, TTL_SEARCH_EQU);
            if (sRetValue == TTL_SUCCESS) {
                usIndOff++;
                continue;
            } else if (sRetValue != TTL_NOTINFILE) {
                PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
                return (sRetValue);         /* Return Error Status */
            }

            if ((sRetValue = TtlCasIErase(ulPositionInd, usNumberOfCashiers, uchTmpBuff, ulIndOffset)) != 0) {
                                            /* Update Index Table */
                PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
                return (sRetValue);         /* Return Error Status */
            }
     
            /* --- Erase Waiter Empty Table --- */
            if (TtlCasEmptyRead(ulPositionEmp, uchTmpBuff) != 0) {
                                            /* Read Cashier Empty Table */
                PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
                return (TTL_FAIL);         /* Rteurn Error Status */
            }
            if (TtlEmptyTableUpdate((VOID *)uchTmpBuff, usMaxNo, CasInd.usBlockNo, TTL_EMPBIT_RESET) != 0) {
                                            /* Erase Cashier Empty Table */
                PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
                return (TTL_FAIL);          /* Rteurn Error Status */
            }
            if (TtlCasEmptyWrite(ulPositionEmp, uchTmpBuff) != 0) {
                                            /* Write Cashier Empty Table */
                PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
                return (TTL_FAIL);          /* Rteurn Error Status */
            }
     
            /* --- Clear Cashier Total ---- */
            memset(uchTmpBuff, 0, TTL_CASTTL_SIZE);     /* Clear Buffer */
            ulPositionTtl = ulPositionTtl + (TTL_CASTTL_SIZE * (ULONG)CasInd.usBlockNo);
            if (TtlWriteFile(hsTtlCasHandle, ulPositionTtl, uchTmpBuff, TTL_CASTTL_SIZE) != 0) {
                PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
                return (TTL_FAIL);
            }

            /* --- Set Read Total Offset --- */
            switch(i) {
            case 0:                         /* Current Daily File */
                Cashd.usCurDayCas = Cashd.usCurDayCas - 1;
                break;

            case 1:                         /* Previous Daily File */
                Cashd.usPreDayCas = Cashd.usPreDayCas - 1;
                break;

            case 2:                         /* Current PTD File, V3.3 */
                Cashd.usCurPTDCas = Cashd.usCurPTDCas - 1;
                break;

            case 3:                         /* Previous PTD File, V3.3 */
                Cashd.usPrePTDCas = Cashd.usPrePTDCas - 1;
                break;

			default:
				PifReleaseSem(usTtlSemRW);             /* Release Semaphore for Total File Read & Write */
				NHPOS_ASSERT_TEXT(0, "==ERROR: TtlCasTotalCheck() state machine loop invalid count.");
				return (TTL_FAIL);
            }

            /* --- Update Waiter File Header --- */
            if (TtlCasHeadWrite(&Cashd) != 0) {    /* Write Cashier File Header */
                PifReleaseSem(usTtlSemRW);         /* Release Semaphore for Total File Read & Write */
                return (TTL_FAIL);                 /* Rteurn Error Status */
            }
            usNumberOfCashiers--;
            sRetValue = TTL_DELCASWAI;
        }
    }
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(sRetValue);
}

/*
*==========================================================================
**    Synopsis: VOID    TtlShutDown(VOID)
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  Nothing
**  Description:
*           shutdown/suspend total update module 
*==========================================================================
*/
VOID TtlShutDown(VOID)
{
    TtlTUMShutDown();
}

/*
*==========================================================================
**    Synopsis: VOID    TtlResume(VOID)
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  Nothing
**  Description:
*           resume from suspend total update module 
*==========================================================================
*/
VOID TtlResume(VOID)
{
    TtlTUMResume();
}

/*
*==========================================================================
**    Synopsis: SHORT   TtlTenderAmountRead(TTLCASTENDER *pTender)
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  Nothing
**  Description:
*           resume from suspend total update module 
*==========================================================================
*/
SHORT   TtlTenderAmountRead(TTLCASTENDER *pTender)
{
    TTLCS_TMPBUF_WORKING  uchTmpBuff = {0};
    SHORT      sRetvalue, i;
	TTLCASHIER TtlCashier = {0};  /* 09/21/00 */

    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
    if (chTtlLock == TTL_LOCKED) {  /* Check File Lock Status */
        PifReleaseSem(usTtlSemRW);  /* Release Semaphore for Total File Read & Write */
		NHPOS_ASSERT_TEXT((chTtlLock != TTL_LOCKED), "TtlTenderAmountRead(): total file locked.");
        return (TTL_LOCKED);        /* Return Locked */
    }

    TtlCashier.uchMajorClass = CLASS_TTLCASHIER;
    TtlCashier.uchMinorClass = CLASS_TTLCURDAY;
    TtlCashier.ulCashierNumber = pTender->ulCashierNumber;

    sRetvalue = TtlTreadCas(&TtlCashier);
    if (sRetvalue <= TTL_FILE_OPEN_ERR) {
        if (sRetvalue != TTL_FILE_HANDLE_ERR) {
            TtlAbort(MODULE_TTL_TFL_CASHIER, sRetvalue);    /* return Not Used */
        }
    }

    if (sRetvalue == TTL_SUCCESS) {
        pTender->lCashTender = TtlCashier.CashTender.Total.lAmount;       /* Cash Tender, Saratoga */
        pTender->lCheckTender = TtlCashier.CheckTender.Total.lAmount;     /* Check Tender */
        pTender->lChargeTender = TtlCashier.ChargeTender.Total.lAmount;   /* Charge Tender */
		// copy tender and totals from received input into user's buffer
		// area.  Tenders are usually sized to STD_TENDER_MAX but we will
		// calculate the number of elements to ensure everything works properly.
        for (i = 0; i < sizeof(TtlCashier.MiscTender)/sizeof(TtlCashier.MiscTender[0]); i++) {
            pTender->lMiscTender[i] = TtlCashier.MiscTender[i].Total.lAmount;    /* Misc Tender 1 to 8 */
        }
        for (i = 0; i < sizeof(TtlCashier.ForeignTotal)/sizeof(TtlCashier.ForeignTotal[0]); i++) {
            pTender->lForeignTotal[i] = TtlCashier.ForeignTotal[i].Total.lAmount;  /* Foreign Total 1 to 8,    Saratoga */
        }
    } else if (sRetvalue == TTL_NOTINFILE) {
        pTender->lCashTender = 0;       /* Cash Tender, Saratoga */
        pTender->lCheckTender = 0;      /* Check Tender */
        pTender->lChargeTender = 0;     /* Charge Tender */
        for (i = 0; i < sizeof(pTender->lMiscTender)/sizeof(pTender->lMiscTender[0]); i++) {
            pTender->lMiscTender[i] = 0; /* Misc Tender 1 to 8 */
        }
        for (i = 0; i < sizeof(pTender->lForeignTotal)/sizeof(pTender->lForeignTotal[0]); i++) {
            pTender->lForeignTotal[i] = 0;  /* Foreign Total 1 to 8,    Saratoga */
        }

        sRetvalue = TTL_SUCCESS;
    }

    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
    return(sRetvalue);              /* Return */
}

/* ===== End of File (TOTALFL.C) ===== */
