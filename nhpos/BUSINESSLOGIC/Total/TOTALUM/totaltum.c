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
* Title       : TOTAL, Total Update Module
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: Totaltum.c
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
* --------------------------------------------------------------------------
* Abstract:
*   VOID  TtlTUMMain()          - TUM (Total Update Module) Main Function
*   SHORT TtlTUMUpRead()        - Read Update Total File Data for TUM
*   SHORT TtlTumUpdate()        - Affect Total
*   SHORT TtlCheckAffectClass() - Check Affection Data
*   VOID  TtlTumFileUpdate()    - Update Each Totals with Work Buffer
*   VOID  TtlTumReadTLWork()    - Read Each Totals into Work Buffer
*
* --------------------------------------------------------------------------
* Update Histories
*    Data   : Ver.Rev.:   Name    : Description
* May-06-92 : 00.00.01 : K.Terai    :
* Dec-03-93 : 02.00.03 : K.You      : add 4 tender total
* Apr-05-94 : 00.00.04 : K.You      : add flex GC featur (mod. TtlTUMMain, TtlTUMUpRead)
* Mar-03-95 : 03.00.00 : hkato      : R3.0
* Dec-26-95 : 03.01.00 : T.Nakahata : R3.1 Initial
*   Add Service Time Total (Daily/PTD) and Individual Terminal Financial (Daily)
*   Increase Regular Discount ( 2 => 6 )
*   Add Net Total at Register Financial Total
*   Add Some Discounts and Void Elements at Server Total
*   Add Post Receipt Count at Server/Cashier Total
*   TtlFinDayWork/TtlFinPTDWork => change FAR Data
* Feb-13-96 : 03.01.00 : M.Ozawa    : Enhanced to Bartender Operation
* Mar-21-96 : 03.01.01 : T.Nakahata : Cashier Total Offset USHORT to ULONG
* Aug-11-99 : 03.05.00 : K.Iwata    : R3.5 Remove WAITER_MODEL
* Aug-11-99 : 03.05.00 : K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Nov-30-99 : 01.00.00 : hrkato     : Saratoga
*
** GenPOS **
*
* Nov-09-15 : 02.02.01 : R.Chambers : In TtlTumUpdateDelayedBalance(), fixed PifSemRelease() always called.
* Aug-29-17 : 02.02.02 : R.Chambers : replaced PARAFLEXMEM with RflGetMaxRecordNumberByType(). using TTLCS_TMPBUF_WORKING.
* Aug-29-17 : 02.02.02 : R.Chambers : eliminate references to waiter id (ulWaiterID) TtlTumFileUpdate(), TtlTumReadTLWork().
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
#include <math.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32_WCE
#if defined (DEBUG) || (_DEBUG)
#include <winbase.h>
#endif
#endif

#include <ecr.h>
#include <pif.h>
#include <rfl.h>
#include <cscas.h>
#include <plu.h>
#include <csstbfcc.h>
#include <csttl.h>
#include <csstbttl.h>
#include <csop.h>
#include <ttl.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <maint.h>
#include "ttltum.h"
#include <appllog.h>
#include "nb.h"
#include "PluTtlD.h"

USHORT          TtlUpTotalFlag;         /* Total Update Flag */
TTLCSREGFIN     TtlFinDayWork;          /* Daily Financial Work Area */
TTLCSREGFIN     TtlFinPTDWork;          /* PTD Financial Work Area */
ULONG           ulTtlCasDayPosition;    /* Save Cashier Daily Position */
TTLCSCASTOTAL   TtlCasDayWork;          /* Daily Cashier Work Area */
ULONG           ulTtlCasPTDPosition;    /* Save Cashier PTD Position, V3.3 */
TTLCSCASTOTAL   TtlCasPTDWork;          /* PTD Cashier Work Area, V3.3 */
UCHAR           uchTtlCpnStatus;        /* Coupon File Status,     R3.0 */

TTLCPNDAYREADTOTAL  TtlCpnDayWork;      /* Daily Coupon Work Area, R3.0 */
TTLCPNPTDREADTOTAL  TtlCpnPTDWork;      /* PTD Coupon Work Area,   R3.0 */

UCHAR uchTtlUpdDelayBalanceFlag = FALSE;
/* ===== New Totals (Release 3.1) BEGIN ===== */
TTLCSREGFIN     TtlIndFinWork;          /* Individual Financial Work Area, R3.1 */
/* ===== New Totals (Release 3.1) END ===== */

PifSemHandle usTtlSemPowerDown = PIF_SEM_INVALID_HANDLE;      /* Semaphoe compete with powerdown proc. */
                               /* while having this semaphoe, powerdown proc */
                               /* will wait until this semaphoe released */

#if 0
static SHORT TtlTUMUpRead_Debug(VOID *pReadBuff, ULONG usReadPointer, ULONG usWritePointer, ULONG usLength, SHORT sFileHandle, char *aszFilePath, int iLineNo); 
#define TtlTUMUpRead(pBuff,ulReadPtr,ulWritePtr,ulLen,sFile) TtlTUMUpRead_Debug(pBuff,ulReadPtr,ulWritePtr,ulLen,sFile,__FILE__,__LINE__)
#else
static SHORT TtlTUMUpRead(VOID *pReadBuff, ULONG usReadPointer, ULONG usWritePointer, ULONG usLength, SHORT sFileHandle); 
#endif

/*
*===========================================================================
*   Extern Declarations
*===========================================================================
*/
/*
*============================================================================
**Synopsis:     VOID  TtlTUMMain(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function is Total Update Module Main function.
*
                This function is a thread which takes total update requests
                and updates the total files with the necessary values.

                Basically, this thread is a consumer of total update requests which are
                produced from other threads.

                This thread initializes and then does a PifRequestSem(usTtlSemUpdate)
                waiting for some other thread to put information into the TOTALUPD
                buffer file.  When the other thread puts information into that file,
                it will then do a release on the usTtlSemUpdate semaphore allowing the
                TtlTUMMain thread to run and update the totals.  See functions
                TtlUpdateFile() and TtlUpdateFileFH() in file totalfl.c.

                Communication is through the shared global variable, TtlUphdata, which
                is part of the UNINIRAM area.  This shared variable contains the current
                TOTALUPD file status allowing the file to be used as a communication mechanism
                between TtlTUMMain and producers of total information.  This global variable
                and the communication file itself are guarded by the semaphore usTtlSemWrite
                which coordinates both updates to the global variable and the file.

                NOTES:
				1. TtlUphdata.uchResetFlag usage
                    If we are in reset recovery mode then we need to determine if
                    we have processed all of the items that we had processed before
                    this terminal went down.  We will then turn off the reset
                    recovery mode flag.  This flag is used the PLU totals source
                    to determine if the totals for a particular PLU should be updated
                    or not.  The reason is that the PLU totals are kept in a database
                    using a real database engine so the PLU totals are updated as they
                    are processed.  The other totals such as financial totals or cashier
                    totals are accumulated in the working buffers and then saved to the
                    appropriate file when the transaction end is found.
*============================================================================
*/
VOID THREADENTRY TtlTUMMain(VOID)
{
    TTLCS_TMPBUF_WORKING uchTmpBuff;    // Temporary Buffer for item information
    TTLTUMTRANQUAL TtlTranQualBuff;     // Transaction Qualifier Save Buffer
    ULONG   ulLength;                   // Item Data Length
    UCHAR   uchNonCmpLength;            // Item Data NonCompress Length
    USHORT  usCmpLength;                // Item Data Compress Length
    SHORT   sAffect;                    // Affection Data Exist Value Save Area
    ULONG   uchAffectAbort;             // Affection Lost
    ULONG   ulTranEndPointer;           // Transaction End Pointer Save Area
    ULONG   ulFileMaxSize;
	SHORT   sTumReadStat;
#ifdef _WIN32_WCE
#if defined(_DEBUG) || defined(DEBUG)
#pragma message("***ENABLE LOGGING***")
    ULONG   ulStartTime, ulEndTime;
    TCHAR    szMsg[ 256 ];
#endif
#endif

	/*
		--- Initialize ---

		If, when we are starting up, the usReadPointer is not equal to the usWritePointer,
		there there are transactions in the TOTALUPD file which have not yet been
		processed by the Total Update thread, namely us.  Therefore, we will need to
		determine if we are starting up having already processed part of the next transaction
		or if we are starting at the beginning of the transaction.

		If TtlUphdata.usReadPointer == TtlUphdata.usCurReadPointer then this is a new
		transaction and we do not have to do any recovery.  If they are not equal, then
		we have to take a recovery action by processing the transaction until we reach
		the part we have not yet processed and then continue with out total update from there.
	 */
    usTtlSemPowerDown = PifCreateSem(1);    /* compete with powerdown proc. */

	PifLog(MODULE_TTL_TUM_UPDATE, LOG_EVENT_THREAD_START);
    PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_UPDATE), (USHORT)TtlUphdata.uchResetFlag);
    PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);

	// Check to see that the value for the semaphore handle is correct.
	// If invalid then we are a Satellite Terminal, function TtlInitial() was not called
	// and we are not doing total updates so we will just gracefully exit.
	if (usTtlSemWrite > 0x7fff) {
		NHPOS_NONASSERT_NOTE("==NOTE", "==NOTE: TtlTUMMain():  usTtlSemWrite handle invalid, stopping thread - Satellite?");
		PifLog(MODULE_TTL_TUM_UPDATE, LOG_EVENT_TTL_UPD_TTL_FILE_RSET);
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(__LINE__));
		return;
	}

    PifRequestSem(usTtlSemWrite); // Request Semaphore for Update Total File Read & Write

    uchAffectAbort = 0;

	{
		char  xBuff[128];

		sprintf (xBuff, "TtlTUMMain() start reset 0x%x flag 0x%x  read %d cur read %d write %d.", TtlUphdata.uchResetFlag, TtlUphdata.uchTtlFlag0, TtlUphdata.usReadPointer, TtlUphdata.usCurReadPointer, TtlUphdata.usWritePointer);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	// NOTE:  When using debugger to see the TtlUphdata struc you need to create
	// a watch on the variable Para.TtlUphdata since TtlUphdata is actually a
	// part of Pararam and #defines are used to allow use of just TtlUphdata.
	TtlUphdata.uchTtlFlag0 &= ~TTL_UPBUFF_RUNNING;    // ensure totaltum running flag is turned off
    if (!((TtlUphdata.usReadPointer == TtlUphdata.usWritePointer) && ((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) == 0)))
	{
		// If TtlUphdata.uchResetFlag then we are in reset mode  and the last
		// time we tried doing reset recovery it did not complete.  We will
		// assume that we have inconsistent data structure members.
		// Let's straighten them out as a part of initialization and recovery.
        if (TtlUphdata.uchResetFlag)
		{
			memset (&TtlUphdata, 0, sizeof(TtlUphdata));   // Reset everything
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_EVENT_TTL_UPD_TTL_FILE_RSET_CLR);
		}
        if ((TtlUphdata.usReadPointer != TtlUphdata.usCurReadPointer) && (TtlUphdata.uchResetFlag == 0))
		{
			char  xBuff[128];

			sprintf (xBuff, "==NOTE: TtlTUMMain() TtlUphdata.uchResetFlag set read %d cur read %d write %d.", TtlUphdata.usReadPointer, TtlUphdata.usCurReadPointer, TtlUphdata.usWritePointer);
            TtlUphdata.usSvCurReadPointer = TtlUphdata.usCurReadPointer;
            TtlUphdata.uchResetFlag = 1;   // Set Master Reset Flag
			PifLog(MODULE_TTL_TUM_UPDATE, LOG_EVENT_TTL_UPD_TTL_FILE_RSET);
			PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(__LINE__));
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
        }
        if (TtlUphdata.uchResetFlag || (TtlUphdata.usReadPointer != TtlUphdata.usWritePointer))
		{
			PifReleaseSem(usTtlSemUpdate);  // Release Semaphore for Update Module Start
		}
        TtlUphdata.usCurReadPointer = TtlUphdata.usReadPointer;  // Set Current Read Pointer
	} else {
		TtlUphdata.uchResetFlag = 0;   // Reset Master Reset Flag in case it is set and should not be
	}

    PifReleaseSem(usTtlSemWrite);   /* Release Semaphore for Total File Read & Write */

	SerTtlIssuedCheckAllReset ();   // At startup we audit the totals for consistency.

    /*
        A loop which runs forever handling total file updates.

        The first step is to wait on the usTtlSemUpdate semaphore until some other
        thread updates the communication file and the shared global file status variable
        and then releases the semaphore so that we can then do a total update.
     */
    for (;;){
        PifRequestSem(usTtlSemUpdate);  // Request Semaphore for Update Module Start
		TtlUphdata.uchTtlFlag0 |= TTL_UPBUFF_RUNNING;

        /* get update file size */
        if((ulFileMaxSize = TrnCalStoSize(RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR), FLEX_ITEMSTORAGE_ADR)) < TTL_UPDATE_FILESIZE) {  /* 2172 */
            ulFileMaxSize = TTL_UPDATE_FILESIZE;
        }

#ifdef _WIN32_WCE
#if defined(_DEBUG) || defined(DEBUG)
        PifGetTickCount( &ulStartTime );
#endif
#endif

        for (;;) {

			// --- Read Update Total File Data ---
			// See function TrnSendTotal() to see what information is written
			// into the TOTALUPD file.
            /*
                Disable power-down while we are doing a total file update.

                Request Semaphore for Update Total File Read & Write.
                This basically causes us to wait until we can get exclusive
                access to the file and the shared global variable.  We are going
                to read from the communication file and update the shared global
                variable so we want exclusive access to both.
             */
            PifRequestSem(usTtlSemPowerDown);   // disable power-down
            PifRequestSem(usTtlSemWrite);       // Request Semaphore for Update Total File Read & Write

			// Check to see if the TOTALUPD file is empty.  If so, nothing to do.
            if ( (TtlUphdata.usReadPointer == TtlUphdata.usWritePointer) && ((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) == 0) ) {
				break;
			}

			// If we are just beginning on this Total File Update, then usReadPointer is
			// equal to usCurReadPointer.  As we read the Total File, we will move the
			// usCurReadPointer until we have completed processing this transaction.
			// This implies that the following condition is met only when first starting to
			// process a transaction.
			// NOTE:  When using debugger to see the TtlUphdata struc you need to create
			// a watch on the variable Para.TtlUphdata since TtlUphdata is actually a
			// part of Pararam and #defines are used to allow use of just TtlUphdata.
            if (TtlUphdata.usReadPointer == TtlUphdata.usCurReadPointer)
			{
                /* Check Already Read Qualifier Data */
                /* Read Transaction Qualifier Data */
                if (TtlTUMUpRead((UCHAR *)&TtlTranQualBuff,
                                 TtlUphdata.usReadPointer,
                                 TtlUphdata.usWritePointer,
                                 sizeof(TTLTUMTRANQUAL), hsTtlUpHandle) != TTL_SUCCESS)
				{
                    break;
                }

				// Update the usCurReadPointer with our current position in the TOTALUPD
				// file.  We do not move the usReadPointer in case we have to do some
				// kind of recovery action after application is restarted that will
				// require that we restart the update process from the start of the
				// transaction.  Function TrnSendTotal () starts writing its content
				// beginning with an offset of sizeof(USHORT) so we take that into
				// account when updating the usCurReadPointer.
                TtlUphdata.usCurReadPointer = (TtlUphdata.usCurReadPointer + sizeof(USHORT) + sizeof(TTLTUMTRANQUAL)) % ulFileMaxSize;

				// the end pointer is our current file position plus the length of
				// the compressed variable length portion of the transaction that
				// follows the uncompressed known length part of the transaction data.
                ulTranEndPointer = (TtlUphdata.usReadPointer + TtlTranQualBuff.TtlTranVli) % ulFileMaxSize;

				// Initialize the working buffers for the totals calculation by
				// reading into the buffers the persistent totals information stored
				// in the various totals files.
				TtlTumReadTLWork(TtlTranQualBuff.TtlTranModeQual.ulCashierID,
								 TtlTranQualBuff.TtlTranModeQual.uchTerm,
								 TtlTranQualBuff.TtlTranModeQual.fsModeStatus,
								 &uchAffectAbort, uchTmpBuff);
				if (RflLoggingRulesTransactions()) {
					USHORT        usConsNoTemp, usGuestNoTemp = 0, uchTermTemp;
					ULONG         ulCashierIDTemp, ulStoreregNoTemp;
					char transBuff[256];

					usConsNoTemp = TtlTranQualBuff.TtlTranCurQual.usConsNo;
					ulStoreregNoTemp = TtlTranQualBuff.TtlTranCurQual.ulStoreregNo;
					ulCashierIDTemp = TtlTranQualBuff.TtlTranModeQual.ulCashierID;
					uchTermTemp = TtlTranQualBuff.TtlTranModeQual.uchTerm;
					sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%03d|0x%x|0x%x|%08d|%08d|%08d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, 0x100, 0,
						uchTermTemp, TtlUphdata.uchTtlFlag0, TtlUphdata.uchResetFlag, TtlUphdata.usReadPointer, TtlUphdata.usCurReadPointer, TtlUphdata.usWritePointer);
					PifTransactionLog (0, transBuff, __FILE__, __LINE__);
				}
            }

#if 0
			if(TtlUphdata.uchResetFlag == 1){
				TtlUphdata.usCurReadPointer = TtlUphdata.usSvCurReadPointer;
				TtlUphdata.uchResetFlag = 0;
			}

			/* We have finished processing this transaction the
			    cur position pointer is equal to the write pointer
			   lets make sure that we do not have an erronous file full
			   indicator.  Clear the Update Total File Full flag
			 */
			if(TtlUphdata.usCurReadPointer == TtlUphdata.usWritePointer){
                /* === Individual Financial Total (Release 3.1) BEGIN === */
                TtlTumFileUpdate(TtlTranQualBuff.TtlTranModeQual.ulCashierID,
                                 TtlTranQualBuff.TtlTranModeQual.uchTerm,
                                 &uchAffectAbort);
                /* === Individual Financial Total (Release 3.1) END === */

                /* Update Read Pointer indicating we have completed this transaction */
                TtlUphdata.usReadPointer = (TtlUphdata.usReadPointer + TtlTranQualBuff.TtlTranVli) % ulFileMaxSize;
				TtlUphdata.uchResetFlag = 0;

				if ((TtlUphdata.usReadPointer == TtlUphdata.usWritePointer) &&
                    (TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL)) {

                    TtlUphdata.uchTtlFlag0 &= ~TTL_UPBUFF_FULL;

					PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_TTL_UPBUFF_RESET);
                }
				break;
			}
#endif

			// Now that we update the totals for each of the items in the transaction.
			// The rest of this loop is executed as we read the Total File for the items
			// in the transaction and update the various totals based on the item information
			// in the transaction.

            // Read Transaction Item Data Length(Non compress)
            sTumReadStat = TtlTUMUpRead(&uchNonCmpLength, TtlUphdata.usCurReadPointer, TtlUphdata.usWritePointer, sizeof(UCHAR), hsTtlUpHandle);

			// catastrophic error so lets reset, issue a PifLog, and then
			// try again with next transaction after dropping this transaction.
			if ((sTumReadStat != TTL_SUCCESS) || (uchNonCmpLength < 1))
			{
				memset (&TtlUphdata, 0, sizeof(TtlUphdata));
				PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TOTALUM_UPDATETRANS);
				PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(sTumReadStat));
                break;
			}

           // Read Transaction Item Data Length (Compressed)
           sTumReadStat = TtlTUMUpRead(&usCmpLength, ((TtlUphdata.usCurReadPointer + uchNonCmpLength) % ulFileMaxSize),
                        TtlUphdata.usWritePointer, sizeof(USHORT), hsTtlUpHandle);
           if (sTumReadStat != TTL_SUCCESS)
		   {
				PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TOTALUM_UPDATETRANS);
				PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(sTumReadStat));
                break;
			}

			ulLength = uchNonCmpLength + usCmpLength;  // Calculate Item Data Length

			// Read Transaction Item Data (Compressed)
			if (TtlTUMUpRead(uchTmpBuff, TtlUphdata.usCurReadPointer, TtlUphdata.usWritePointer, ulLength, hsTtlUpHandle))
			{
                break;
            }

			// If we are in reset recovery mode then we need to determine if
			// we have processed all of the items that we had processed before
			// this terminal went down.  We will then turn off the reset
			// recovery mode flag.  This flag is used the PLU totals source
			// to determine if the totals for a particular PLU should be updated
			// or not.  The reason is that the PLU totals are kept in a database
			// using a real database engine so the PLU totals are updated as they
			// are processed.  The other totals such as financial totals or cashier
			// totals are accumulated in the working buffers and then saved to the
			// appropriate file when the transaction end is found.
			if(TtlUphdata.uchResetFlag == 1) {
				if (TtlUphdata.usCurReadPointer == TtlUphdata.usSvCurReadPointer)
					TtlUphdata.uchResetFlag = 0;
			}

            /*
                --- Update Total ----

                If this type of total transaction affects the totals, then
                we will update the totals using the information read in.

                We have to decompress the item information, then update the totals,
                then continue on with updating the global shared variable to reflect
                the new communication file status.
             */
            sAffect = TtlCheckAffectClass((TTLTUMTRANUPDATE *)&uchTmpBuff[0]);
#if 0
			if (RflLoggingRulesTransactions()) {
				USHORT        usConsNoTemp, usGuestNoTemp = 0, uchTermTemp;
				ULONG         ulCashierIDTemp, ulStoreregNoTemp;
				char transBuff[256];

				usConsNoTemp = TtlTranQualBuff.TtlTranCurQual.usConsNo;
				ulStoreregNoTemp = TtlTranQualBuff.TtlTranCurQual.ulStoreregNo;
				ulCashierIDTemp = TtlTranQualBuff.TtlTranModeQual.ulCashierID;
				uchTermTemp = TtlTranQualBuff.TtlTranModeQual.uchTerm;
				sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%03d|0x%x|0x%x|%08d|%08d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, 0x100, sAffect,
					uchTermTemp, TtlUphdata.uchTtlFlag0, TtlUphdata.uchResetFlag, TtlUphdata.usReadPointer, TtlUphdata.usWritePointer);
				PifTransactionLog (0, transBuff, __FILE__, __LINE__);
			}
#endif
            if (sAffect == TTL_AFFECT)       /* Check Affection Data Exists and update totals if it does */
			{
				TTLTUMTRANUPDATE TtlTranUpBuff;     // Transaction Item Save Buffer(For Decomp)
                             
                RflGetStorageItem(&TtlTranUpBuff, uchTmpBuff, RFL_WITHOUT_MNEM);                  /* Decomp Item Data */
                PifRequestSem(usTtlSemRW);                                                        // Request Semaphore for Total File Read & Write
                TtlTumUpdate(&TtlTranQualBuff, &TtlTranUpBuff, uchTmpBuff, &uchAffectAbort);  /* Updating Total */
                PifReleaseSem(usTtlSemRW);                                                        // Release Semaphore for Total File Read & Write

				// if this transaction item is either an Operator Close or if it is a tender of some kind then TRANSLOG it
				if (TtlTranUpBuff.TtlClass.uchMajorClass == CLASS_ITEMOPECLOSE || TtlTranUpBuff.TtlClass.uchMajorClass == CLASS_ITEMTENDER) {
					if (RflLoggingRulesTransactions()) {
						LONG          lRound = TtlTranUpBuff.TtlItemTender.lRound;
						LONG          lTenderAmount = TtlTranUpBuff.TtlItemTender.lTenderAmount;
						ULONG         ulCashierIDTemp, ulStoreregNoTemp;
						USHORT        usConsNoTemp, usGuestNoTemp = 0, uchTermTemp;
						char transBuff[256];

						usConsNoTemp = TtlTranQualBuff.TtlTranCurQual.usConsNo;
						ulStoreregNoTemp = TtlTranQualBuff.TtlTranCurQual.ulStoreregNo;
						ulCashierIDTemp = TtlTranQualBuff.TtlTranModeQual.ulCashierID;
						if (TtlTranUpBuff.TtlClass.uchMajorClass == CLASS_ITEMOPECLOSE) {
							lRound = lTenderAmount = 0;
							ulCashierIDTemp = TtlTranUpBuff.TtlItemClose.ulID;
						}
						uchTermTemp = TtlTranQualBuff.TtlTranModeQual.uchTerm;
						sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%08d|%08d|%03d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, TtlTranUpBuff.TtlClass.uchMajorClass, TtlTranUpBuff.TtlClass.uchMinorClass, lRound, lTenderAmount, TtlTranUpBuff.TtlClass.uchMajorClass);
						PifTransactionLog (0, transBuff, __FILE__, __LINE__);
					}
				}
            }

            // Update Current Read Pointer for this item in the transaction
			// and release the semaphore guarding TtlUphdata structure.
            TtlUphdata.usCurReadPointer = (TtlUphdata.usCurReadPointer + ulLength) % ulFileMaxSize;

            if (ulTranEndPointer == TtlUphdata.usCurReadPointer)
			{
                /* === Individual Financial Total (Release 3.1) BEGIN === */
                TtlTumFileUpdate(TtlTranQualBuff.TtlTranModeQual.ulCashierID,
                                 TtlTranQualBuff.TtlTranModeQual.uchTerm,
                                 &uchAffectAbort);
                /* === Individual Financial Total (Release 3.1) END === */

                /* Update Read Pointer indicating we have completed this transaction */
                TtlUphdata.usReadPointer = (TtlUphdata.usReadPointer + TtlTranQualBuff.TtlTranVli) % ulFileMaxSize;

                /* If we have processed everything in the TOTALUPD file then
				   lets make sure that we do not have an erronous file full
				   indicator.  Clear the Update Total File Full flag
				 */
                if (TtlUphdata.usReadPointer == TtlUphdata.usWritePointer) {
					if ((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) != 0) {
						TtlUphdata.uchTtlFlag0 &= ~TTL_UPBUFF_FULL;
						PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_UPD_TTL_UPBUFF_RESET);
					}
					break;
                }
            }

			//Looks like it should be writing to this area when it updates
			//the totals just in case of a power failure, should not
			//wait until sign-out to use the PifSaveFarData fucntion call. RLZ
			CliParaSaveFarData();

            PifReleaseSem(usTtlSemWrite);      // Release Semaphore for Update Total File Read & Write
            PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop
        }

		// lets make sure that the flags are turned off, especially the totaltum reset flag.
		// we have seen instances at VCS in which the totaltum reset flag was not turned off
		// properly resulting in PLU totals not being updated for several days.  The behavior
		// see was that financial totals were correct however PLU totals were all zero.  data
		// captured from the field indicated that the totaltum reset flag was still turned on
		// even when it should have been turned off after processing pending transaction data
		// that had not been processed before the GenPOS application terminated.
		TtlUphdata.uchTtlFlag0 &= ~TTL_UPBUFF_RUNNING;
		TtlUphdata.uchResetFlag = 0;           // no more to do so force the reset flag off in case it is still on.

		if (RflLoggingRulesTransactions()) {
			USHORT        usConsNoTemp, usGuestNoTemp = 0, uchTermTemp;
			ULONG         ulCashierIDTemp, ulStoreregNoTemp;
			char transBuff[256];

			usConsNoTemp = TtlTranQualBuff.TtlTranCurQual.usConsNo;
			ulStoreregNoTemp = TtlTranQualBuff.TtlTranCurQual.ulStoreregNo;
			ulCashierIDTemp = TtlTranQualBuff.TtlTranModeQual.ulCashierID;
			uchTermTemp = TtlTranQualBuff.TtlTranModeQual.uchTerm;
			sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%03d|0x%x|0x%x|%08d|%08d|%08d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, 0x100, 0,
				uchTermTemp, TtlUphdata.uchTtlFlag0, TtlUphdata.uchResetFlag, TtlUphdata.usReadPointer, TtlUphdata.usCurReadPointer, TtlUphdata.usWritePointer);
			PifTransactionLog (0, transBuff, __FILE__, __LINE__);
		}

		//Looks like it should be writing to this area when it updates
		//the totals just in case of a power failure, should not
		//wait until sign-out to use the PifSaveFarData fucntion call. RLZ
		CliParaSaveFarData();

        PifReleaseSem(usTtlSemWrite);      // Release Semaphore for Update Total File Read & Write
        PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop

#ifdef _WIN32_WCE
#if defined(_DEBUG) || defined(DEBUG)
        PifGetTickCount( &ulEndTime );
        wsprintf( szMsg, TEXT("TranUpdate Complete [%5lu]ms\r\n"), ulEndTime - ulStartTime );
        OutputDebugString( szMsg );
#endif
#endif
        if(uchAffectAbort)
		{
			PifLog (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
			PifLog (MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_WAI_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_WAI_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_PLU_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_PLU_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_DEP_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_DEP_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_CAS_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CAS_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_CPN_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CPN_ERROR);   /* Execute PifLog */
			TtlAbort (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
			break;

        }
#ifdef _WIN32_WCE
#if defined(_DEBUG) || defined(DEBUG)
        PifGetTickCount( &ulStartTime );
#endif
#endif
        TtlTUMTryToUpdateBackup();  /* Issue trigger to backup into persistent strage. */
                                    /* This trigger may ignored when copy thread is already working. */
#ifdef _WIN32_WCE
#if defined(_DEBUG) || defined(DEBUG)
        PifGetTickCount( &ulEndTime );
        wsprintf( szMsg, TEXT("\r\nTotalBackup Complete [%5lu]ms\r\n"), ulEndTime - ulStartTime );
        OutputDebugString( szMsg );
#endif
#endif
    }
}
/*
*============================================================================
**Synopsis:     SHORT  TtlTUMUpRead(UCHAR *pReadBuff, ULONG usReadPointer,
*                                   ULONG usWritePointer, ULONG usLength,
*                                   SHORT sFileHandle)
*
*    Input:     VOID  *pReadBuff        - Pointer of Reading Buffer from
*                                         Update Total File
*               ULONG  usReadPointer    - Current Raed Pointer
*               ULONG  usWritePointer   - Current Write Pointer
*               ULONG  usLength         - Reading Length
*               SHORT  sFileHandle      - handle for file used with TtlReadFile ()
*
*   Output:     VOID  *pReadBuff        - Update Total File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Reading Update Total File
*               TTl_FAIL            - Failed Reading Update Total File
*
** Description  This function reads the transaction data from the Update Total File
                into the user provided buffer area.  This function handles the possibility
                that the data written into the file resulted in a wrap around condition
                meaning that the last part of the data written to the file was written to
                the front of the file.  This is because the file is used as a circular buffer.
*
*============================================================================
*/
#if defined(TtlTUMUpRead)
static SHORT  TtlTUMUpRead_Special(VOID *pReadBuff, ULONG ulReadPointer, ULONG ulWritePointer, ULONG ulLength, SHORT sFileHandle);
static SHORT TtlTUMUpRead_Debug(VOID *pReadBuff, ULONG ulReadPointer, ULONG ulWritePointer, ULONG ulLength, SHORT sFileHandle, char *aszFilePath, int iLineNo)
{
	SHORT        sRet = TTL_SUCCESS;
	TTLCSUPHEAD  myTtlUphdata = TtlUphdata;

	sRet = TtlTUMUpRead_Special(pReadBuff, ulReadPointer, ulWritePointer, ulLength, sFileHandle);
	if (sRet != TTL_SUCCESS) {
		int   iLen = strlen (aszFilePath);
		char  xBuff[128];

		if (iLen > 30) iLen -= 30; else iLen = 0;

		sprintf (xBuff, "**WARNING: TtlTUMUpRead_Debug() sRet = %d, ulReadPointer = %d, ulWritePointer = %d, ulLength = %d %s %d", sRet, ulReadPointer, ulWritePointer, ulLength, aszFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT((sRet == TTL_SUCCESS), xBuff);
	}

	return sRet;
}

static SHORT  TtlTUMUpRead_Special(VOID *pReadBuff, ULONG ulReadPointer, ULONG ulWritePointer, ULONG ulLength, SHORT sFileHandle)
#else
static SHORT  TtlTUMUpRead(VOID *pReadBuff, ULONG ulReadPointer, ULONG ulWritePointer, ULONG ulLength, SHORT sFileHandle)
#endif
{
    ULONG  ulReadlen1 = 0;
    ULONG  ulReadlen2 = 0;
    SHORT  sRetvalue;
    ULONG  ulFileMaxSize = 0;

    if (ulLength == 0) {
        PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_FUNC_ILLIGAL);         /* 02/11/2001 */
        PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(TTL_BAD_LENGTH));
		return (TTL_SUCCESS);
    }

    /*
        The semantics of the usReadPoint and usWritePointer is that of the standard
        Reader/Consumer and the Writer/Producer.  If usReadPointer equals usWritePointer
        then either one of two conditions applies, the buffer is empty or the buffer is
        full.  To indicate which of the two conditions is the actual condition, we check
        to see if the uchTtlFlag0 indicates a buffer full condition or not.

        Since we are expecting something to be in the buffer, if usReadPointer equals
        usWritePointer and the buffer is empty, it is an error and we report a failure
        along with a PifLog.
     */
    if ((ulReadPointer == ulWritePointer) && ((TtlUphdata.uchTtlFlag0 & TTL_UPBUFF_FULL) == 0)) {
        PifLog(MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_UPDATEEMPTY);  /* Record Log */
        return (TTL_FAIL);          /* Return Fail */
    }

    /* get update file size */
    if ((ulFileMaxSize = TrnCalStoSize(RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR), FLEX_ITEMSTORAGE_ADR)) < TTL_UPDATE_FILESIZE) { /* 2172 */
        ulFileMaxSize = TTL_UPDATE_FILESIZE;
	}

    /*
        Now determine whether we have a wrap around condition we need to adjust
        for.  A wrap around is when the producer of total information has written
        to the update file and the information written was sufficiently long that
        the first part is written on the end of the file, from usReadPointer to the
        end of the file, and the remaining part is written on the front of the file,
        from offset 0 to usWritePointer.

        If the information was indeed split, then we need to do a read from the current
        usReadPointer to the end of the file for the first part, then do a read of the
        remaining data from the beginning of the file and then put the two pieces together
        in our buffer area.
     */

    ulReadlen1 = ulLength;
    ulReadlen2 = 0;
    if (ulReadPointer < ulWritePointer)  {
        if ((ulReadPointer + ulLength) > ulWritePointer) {
            PifLog (MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_UPDATEOVERRD);     /* 02/11/2001 */
            PifLog (MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(TTL_BAD_LENGTH));                 /* Execute PifAbort */
			return (TTL_BAD_LENGTH);          /* Return Fail */
        }
    } else {
        if ((ulReadPointer + ulLength) > (ulFileMaxSize - 1)) {           // subtract one because file offsets are zero based
            ulReadlen1 = ulFileMaxSize - ulReadPointer;
            ulReadlen2 = (ulReadPointer + ulLength) % ulFileMaxSize;
            if (ulReadlen2 > ulWritePointer) {
                PifLog (MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_OVERMAX);     /* 02/11/2001 */
                PifLog (MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(TTL_BAD_LENGTH));            /* Execute PifAbort */
				return (TTL_BAD_LENGTH);          /* Return Fail */
            }
        }
    }

    if ((sRetvalue = TtlReadFile(sFileHandle, ulReadPointer, pReadBuff, ulReadlen1)) != 0) {    /* Read Update Data */
        PifLog (MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_UPDATEOVERRD);         /* 02/11/2001 */
        PifLog (MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(sRetvalue));         /* 02/11/2001 */
        PifLog (MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);         /* 02/11/2001 */
        TtlAbort (MODULE_TTL_TUM_UPDATE, sRetvalue);                        /* Execute PifAbort */
        /* return  Not Used */
    }

    if (ulReadlen2 != 0) {
        if ((sRetvalue = TtlReadFile(sFileHandle, 0L, ((UCHAR *)pReadBuff + ulReadlen1), ulReadlen2)) != 0) {    /* Read Update Data */
            PifLog (MODULE_TTL_TUM_UPDATE, LOG_ERROR_TTL_UPDATEOVERRD);     /* 02/11/2001 */
			PifLog (MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(sRetvalue));         /* 02/11/2001 */
			PifLog (MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);         /* 02/11/2001 */
            TtlAbort (MODULE_TTL_TUM_UPDATE, sRetvalue);                    /* Execute PifAbort */
            /* return  Not Used */
        }
    }

    return(TTL_SUCCESS);            /* Return Success */
}
/*
*============================================================================
**Synopsis:     VOID  TtlTumUpdate(TTLTUNTRANQUAL *TtlTranQualBuff,
*                                  TTLTUMTRANUPDATE *TtlTranUpBuff,
*                                  VOID *pTmpBuff[0], UCHAR *puchAffectAbort)
*
*    Input:     TTLTUNTRANQUAL *pTtlTranQualBuff - Pointer of Transaction
*                                                  Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pinter of Transaction
*                                                  Item Data
*               VOID *pTmpBuff                   - Pointer of Work Buffer
*               UCHAR *puchAffectAbort           -
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Updateing Total File
*               TTl_FAIL            - Failed Updateing Total File
*
** Description  This function updates Total file.
*               This function assumes that the Total File read write semaphore has
*               already been requested and the thread has it.  See semaphore usTtlSemRW.
*============================================================================
*/
VOID TtlTumUpdate(TTLTUMTRANQUAL *pTtlTranQualBuff,
                  TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCS_TMPBUF_WORKING pTmpBuff,
                  ULONG *puchAffectAbort)
{
	static  UCHAR  uchUpdateResetFlag = 0;

    if (pTtlTranUpBuff->TtlClass.uchMajorClass == CLASS_ITEMOPECLOSE) {
		CASIF   Casif = {0};              /* Cashier Close Interface Area */

		Casif.ulCashierNo = pTtlTranUpBuff->TtlItemClose.ulID;                         /* Set Cashier/Waiter Number */
        Casif.usUniqueAddress = (USHORT)pTtlTranUpBuff->TtlItemClose.uchUniqueAdr;     /* Set Unique Address */
        switch (pTtlTranUpBuff->TtlItemClose.uchMinorClass) {
        case CLASS_CASHIEROUT:
        case CLASS_B_OUT:
        case CLASS_CASINTOUT:       /* R3.3 */
        case CLASS_CASINTB_OUT:     /* R3.3 */
        case CLASS_BARTENDEROUT:    /* V3.3 */
        case CLASS_CASHIERMISCOUT:  /* V3.3 */
            CasSignOut(&Casif);     /* Send Cashier/Waiter Close */
            break;
/*      default: Not Used */
        }

		// we have processed the operator close to update operator totals so lets
		// clear any indicators that may still exist.
        *puchAffectAbort = 0;
        chTtlUpdateLock = TTL_TUM_UNLOCK; /* UnLock Update Total File Write */
    } else {
		LONG    lError = 0;

        /*----- check REDUCE item 5/28/96 -----*/
        if (pTtlTranUpBuff->TtlClass.uchMajorClass == CLASS_ITEMSALES) {
            if (pTtlTranUpBuff->TtlItemSales.fbReduceStatus & REDUCE_ITEM) {
                return;
            }
        }
        if (pTtlTranUpBuff->TtlClass.uchMajorClass == CLASS_ITEMDISC) {
            if (pTtlTranUpBuff->TtlItemDisc.fbReduceStatus & REDUCE_ITEM) {
                return;
            }
        }
        if (pTtlTranUpBuff->TtlClass.uchMajorClass == CLASS_ITEMCOUPON) {
            if (pTtlTranUpBuff->TtlItemCoupon.fbReduceStatus & REDUCE_ITEM) {
                return;
            }
        }
                                    /* Check Operator close */
        TtlWUpdateFin(pTtlTranQualBuff, pTtlTranUpBuff);    /* Update Financial Total */
        TtlWUpdateCas(pTtlTranQualBuff, pTtlTranUpBuff);    /* Update Cashier Total */
        TtlWUpdateCpn(pTtlTranQualBuff, pTtlTranUpBuff);    /* Update Coupon Total, R3.0 */

        /* V3.3 */
        if (TtlWUpdateCasWai(pTtlTranQualBuff, pTtlTranUpBuff, pTmpBuff)) {    /* Update Cashier(Surrogate) Total */
            PifLog(MODULE_TTL_TUM_WAITER, LOG_ERROR_TTL_AFFECT);
            lError |= TTL_UPTOTAL_WAI_ERR;
        }
		if (TtlUphdata.uchResetFlag != uchUpdateResetFlag) {
			char  xBuff[128];
			sprintf (xBuff, "==NOTE:  TtlTumUpdate() - uchUpdateResetFlag 0x%x, TtlUphdata.uchResetFlag 0x%x", uchUpdateResetFlag, TtlUphdata.uchResetFlag);
			NHPOS_NONASSERT_TEXT(xBuff);
			uchUpdateResetFlag = TtlUphdata.uchResetFlag;
		}
        if (TtlUphdata.uchResetFlag == 0) {
            TtlTUpdateHour(pTtlTranQualBuff, pTtlTranUpBuff, pTmpBuff);        /* Update Hourly Total */

            /* ===== New Functions (Release 3.1) BEGIN ===== */
            TtlTUpdateSerTime(pTtlTranQualBuff, pTtlTranUpBuff, pTmpBuff);     /* Update Service Time Total, R3.1 */
            /* ===== New Functions (Release 3.1) END ===== */

            if (TtlTUpdatePLU(pTtlTranQualBuff, pTtlTranUpBuff, pTmpBuff)) {  /* Update PLU Total */
                PifLog(MODULE_TTL_TUM_PLU, LOG_ERROR_TTL_AFFECT);
                lError |= TTL_UPTOTAL_PLU_ERR;
            }

            /* 2172 */
            if (TtlTUpdateDept(pTtlTranQualBuff, pTtlTranUpBuff, pTmpBuff)) {  /* Update Dept Total */
                PifLog(MODULE_TTL_TUM_DEPT, LOG_ERROR_TTL_AFFECT);
                lError |= TTL_UPTOTAL_DEP_ERR;
            }
        }
        if (lError) {
			// Following use of semaphore commented out as it appears that under some
			// circumstances, this can cause a deadlock situation.  It appears that
			// under some circumstances, another thread can have a pending request
			// on the semaphore so that when this call is executed, it blocks
			// waiting on the semaphore though this thread already has it.
			// Richard Chambers, Apr-12-2008
//            PifRequestSem(usTtlSemWrite);  /* Request Semaphore for Update Total File Read & Write */
            *puchAffectAbort |= lError;
            chTtlUpdateLock = TTL_TUM_LOCK;  /* Lock Update Total File Write */
//            PifReleaseSem(usTtlSemWrite);  /* Release Semaphore for Update Total File Read & Write */
			PifLog(MODULE_TTL_TUM, LOG_ERROR_TTL_AFFECT);
			PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)lError);
			PifLog(MODULE_LINE_NO(MODULE_TTL_TUM), (USHORT)__LINE__);
        }
    }
}
/*
*============================================================================
**Synopsis:     SHORT TtlCheckAffectClass(TTLTUMTRANUPDATE *pTmpBuff)
*
*    Input:     TLTUMAFFECT *pTmpBuff - Pinter of Transaction
*                                       Item Data
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_AFFECT      - Affection Data
*               TTL_NOTAFFECT   - Not Affection Data
*
** Description  This function checks Class of Transaction Data for affection.
*                 R3.0
*============================================================================
*/
SHORT TtlCheckAffectClass(TTLTUMTRANUPDATE *pTmpBuff)
{
    switch(pTmpBuff->TtlCompClass.uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:       /* Transaction open */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_NEWCHECK:        /* Newcheck */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMSALES:           /* Sales */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_DEPT:            /* Dept */
        case CLASS_DEPTITEMDISC:    /* Dept W/Item Discount */
        case CLASS_DEPTMODDISC:     /* Dept W/Modifier Discount */
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMENU:         /* Set Menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPPLU:          /* OEP Menu,     R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMDISC:            /* Discount */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
        case CLASS_REGDISC1:        /* Regular Discount 1 */
        case CLASS_REGDISC2:        /* Regular Discount 2 */
        case CLASS_REGDISC3:        /* Regular Discount 3,  R3.1 */
        case CLASS_REGDISC4:        /* Regular Discount 4,  R3.1 */
        case CLASS_REGDISC5:        /* Regular Discount 5,  R3.1 */
        case CLASS_REGDISC6:        /* Regular Discount 6,  R3.1 */
        case CLASS_CHARGETIP:       /* Charge Tip */
        case CLASS_CHARGETIP2:      /* Charge Tip, V3.3 */
        case CLASS_CHARGETIP3:      /* Charge Tip, V3.3 */
        case CLASS_AUTOCHARGETIP:   /* Charge Tip, V3.3 */
        case CLASS_AUTOCHARGETIP2:  /* Charge Tip, V3.3 */
        case CLASS_AUTOCHARGETIP3:  /* Charge Tip, V3.3 */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMCOUPON:          /* Coupon,        R3.0 */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_COMCOUPON:       /* combination coupon */
        case CLASS_UPCCOUPON:       /* UPC coupon, saratoga */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMTOTAL:           /* Total key */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_TOTAL3:          /* Total #3 */
        case CLASS_TOTAL4:          /* Total #4 */
        case CLASS_TOTAL5:          /* Total #5 */
        case CLASS_TOTAL6:          /* Total #6 */
        case CLASS_TOTAL7:          /* Total #7 */
        case CLASS_TOTAL8:          /* Total #8 */
		case CLASS_TOTAL10:
		case CLASS_TOTAL11:
		case CLASS_TOTAL12:
		case CLASS_TOTAL13:
		case CLASS_TOTAL14:
		case CLASS_TOTAL15:
		case CLASS_TOTAL16:
		case CLASS_TOTAL17:
		case CLASS_TOTAL18:
		case CLASS_TOTAL19:
		case CLASS_TOTAL20:
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMTENDER:          /* Tender */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_TENDER1:         /* Tender #1 */
        case CLASS_TENDER2:         /* Tender #2 */
        case CLASS_TENDER3:         /* Tender #3 */
        case CLASS_TENDER4:         /* Tender #4 */
        case CLASS_TENDER5:         /* Tender #5 */
        case CLASS_TENDER6:         /* Tender #6 */
        case CLASS_TENDER7:         /* Tender #7 */
        case CLASS_TENDER8:         /* Tender #8 */
        case CLASS_TENDER9:         /* Tender #9 */
        case CLASS_TENDER10:        /* Tender #10 */
        case CLASS_TENDER11:        /* Tender #11 */
        case CLASS_TENDER12:         /* Tender #12 */
        case CLASS_TENDER13:         /* Tender #13 */
        case CLASS_TENDER14:         /* Tender #14 */
        case CLASS_TENDER15:         /* Tender #15 */
        case CLASS_TENDER16:         /* Tender #16 */
        case CLASS_TENDER17:         /* Tender #17 */
        case CLASS_TENDER18:         /* Tender #18 */
        case CLASS_TENDER19:         /* Tender #19 */
        case CLASS_TENDER20:        /* Tender #20 */
        case CLASS_FOREIGN1:        /* FC Tender #1 */
        case CLASS_FOREIGN2:        /* FC Tender #2 */
        case CLASS_FOREIGN3:        /* FC Tender #3,    Saratoga */
        case CLASS_FOREIGN4:        /* FC Tender #4,    Saratoga */
        case CLASS_FOREIGN5:        /* FC Tender #5,    Saratoga */
        case CLASS_FOREIGN6:        /* FC Tender #6,    Saratoga */
        case CLASS_FOREIGN7:        /* FC Tender #7,    Saratoga */
        case CLASS_FOREIGN8:        /* FC Tender #8,    Saratoga */
        case CLASS_TEND_FSCHANGE:   /* FS Change,       Saratoga */
        case CLASS_TEND_TIPS_RETURN:   /* FS Change,       Saratoga */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_NOSALE:          /* No Sale */
        case CLASS_PO:              /* Paid Out */
        case CLASS_RA:              /* Received on Account */
        case CLASS_TIPSPO:          /* Tips Paid Out */
        case CLASS_TIPSDECLARED:    /* Tips Declared */
        case CLASS_MONEY:           /* Money,   Saratoga */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMAFFECTION:       /* Affection */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_HOURLY:          /* Hourly Total */
        case CLASS_SERVICE:         /* Service Total */
        case CLASS_CANCELTOTAL:     /* Cancel Total */
        case CLASS_CASHIERCHECK:    /* Cashier Close Check */
        case CLASS_WAITERCHECK:     /* Waiter Close Check */
        case CLASS_ADDCHECKTOTAL:   /* Addcheck Total */
        case CLASS_MANADDCHECKTOTAL:/* Manual Addcheck Total */
        case CLASS_NEWWAITER:       /* Check Transfer New Waiter */
        case CLASS_OLDWAITER:       /* Check Transfer Old Waiter */
        case CLASS_CHARGETIPWAITER: /* Charge Tip Amount for Waiter */
        case CLASS_CHARGETIPWAITER2: /* Charge Tip Amount for Waiter, V3.3 */
        case CLASS_CHARGETIPWAITER3: /* Charge Tip Amount for Waiter, V3.3 */
        case CLASS_CASHIEROPENCHECK: /* Casher open check */
        case CLASS_TAXAFFECT:       /* Tax for Affection */
        /* ===== New Functions (Release 3.1) BEGIN ===== */
        case CLASS_SERVICETIME:     /* Service Time,    R3.1 */
        /* ===== New Functions (Release 3.1) END ===== */
        case CLASS_VATAFFECT:       /* VAT affect, V3.3 */
        case CLASS_SERVICE_VAT:     /* VATable Service Total, V3.3 */
        case CLASS_SERVICE_NON:     /* non VATable Service Total, V3.3 */
        case CLASS_LOANAFFECT:      /* Loan,    Saratoga */
        case CLASS_PICKAFFECT:      /* Pickup,  Saratoga */
        case CLASS_LOANCLOSE:       /* Loan,    Saratoga */
		case CLASS_PICKCLOSE:       /* Pickup,  Saratoga */
		case CLASS_EPT_ERROR_AFFECT:       /* Electronic Payment error to affect totals */
			break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    case CLASS_ITEMOPECLOSE:        /* Operator Close */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_CASHIEROUT:
        case CLASS_B_OUT:
        case CLASS_CASINTOUT:   /* R3.3 */
        case CLASS_CASINTB_OUT: /* R3.3 */
        case CLASS_WAITEROUT:
        /* ===== New Functions (Release 3.1) BEGIN ===== */
        case CLASS_BARTENDEROUT:    /* Bartender Sign Out,  R3.1 */
        /* ===== New Functions (Release 3.1) END ===== */
        case CLASS_CASHIERMISCOUT:  /* V3.3 */
            break;                  /* Execute Affection */

        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;

    /* === Add New Affection Class (Release 3.1) BEGIN === */
    case CLASS_ITEMOTHER:           /* Other Class */
        switch(pTmpBuff->TtlCompClass.uchMinorClass) {
        case CLASS_POSTRECEIPT:     /* Post Receipt */
        case CLASS_POSTRECPREBUF:   /* Post Receipt for Precheck */
        case CLASS_PARKING:         /* Parking Print */
            break;
        default:
            return (TTL_NOTAFFECT); /* Not Affect Class */
        }
        break;
    /* === Add New Affection Class (Release 3.1) END === */

    default:
        return (TTL_NOTAFFECT);     /* Not Affect Class */
    }
    return (TTL_AFFECT);            /* Affect Class */
}
/*
*============================================================================
**Synopsis:     VOID TtlTumFileUpdate(USHORT usCashierID,  USHORT usWaiterID,
*                                     UCHAR  uchTerminalNo,
*                                     UCHAR *puchAffectAbort)
*
*    Input:     USHORT usCashierID      - Cashier ID
*               USHORT usWaiterID       - Waiter ID
*               UCHAR  uchTerminalNo    - Terminal Unique No.
*               UCHAR *puchAffectAbort  - PifAbort Flag
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function cleared Total Update Work Area.     R3.0
*
*============================================================================
*/
VOID TtlTumFileUpdate(ULONG ulCashierID, USHORT usTerminalNo, ULONG *puchAffectAbort)
{
    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

    /* === Individual Terminal Financial (Release 3.1) BEGIN === */
    TtlTUpdateFin(usTerminalNo);   /* Update Financial Total File */
/*    TtlTUpdateFin();  */
    /* === Individual Terminal Financial (Release 3.1) END === */

    if (TtlTUpdateCas(ulCashierID, CLASS_TTLCURDAY)) {         /* Update Cashier Daily Total */
        PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_01);
        PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
        *puchAffectAbort |= TTL_UPTOTAL_CAS_ERR;
        chTtlUpdateLock = TTL_TUM_LOCK;              /* Lock Update Total File Write */
    }

    if (TtlTUpdateCas(ulCashierID, CLASS_TTLCURPTD)) {         /* Update Cashier PTD Total, V3.3 */
        PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_02);
        PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
        *puchAffectAbort |= TTL_UPTOTAL_CAS_ERR;
        chTtlUpdateLock = TTL_TUM_LOCK;              /* Lock Update Total File Write */
    }

    if (TtlTUpdateCpn()) {          /* Update Coupon Total */
        PifLog(MODULE_TTL_TUM_CPN, LOG_ERROR_TTL_CODE_01);
        PifLog(MODULE_TTL_TUM_CPN, LOG_ERROR_TTL_AFFECT);
        *puchAffectAbort |= TTL_UPTOTAL_CPN_ERR;
        chTtlUpdateLock = TTL_TUM_LOCK;              /* Lock Update Total File Write */
    }

    if (uchTtlCpnStatus & TTL_UPCPN_ERR) {
        PifLog(MODULE_TTL_TUM_CPN, LOG_ERROR_TTL_CODE_02);
        PifLog(MODULE_TTL_TUM_CPN, LOG_ERROR_TTL_AFFECT);
        *puchAffectAbort |= TTL_UPTOTAL_CPN_ERR;
        chTtlUpdateLock = TTL_TUM_LOCK;              /* Lock Update Total File Write */
    }

    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */

    return;
}
/*
*============================================================================
**Synopsis:     VOID TtlTumReadTLWork(USHORT usCashierID, USHORT usWaiterID,
*                                     UCHAR uchTerminalNo, USHORT fsModeStatus,
*                                     UCHAR *puchAffectAbort, VOID *pTmpBuff)
*
*    Input:     USHORT  usCashierID     -   Cashier ID
*               USHORT  usWaiterID      -   Waiter ID
*               UCHAR   uchTerminalNo   -   Terminal Unique No.
*               USHORT  fsModeStatus    -   Mode Qualifier Status
*               UCHAR   *puchAffectAbort-   PifAbort Flag
*               VOID    *pTmpBuff       -   Address of Temporary Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function reads total data to work buffer.
*               The totals functionality has several working buffers that are
*               used to process the totals data.  Some of these working buffers are:
*                   TtlCasDayWork - daily totals for the cashier
*                   TtlCasPTDWork - period to date totals for the cashier
*                   TtlFinDayWork - daily financial totals
*                   TtlFinPTDWork - period to date financial totals
*                   TtlIndFinWork - individual financial totals
*                   TtlCpnDayWork - daily totals for coupons/discounts
*                   TtlCpnPTDWork - period to date totals for coupons/discounts
*
*               This function reads the current totals information into the working
*               buffers so that we can then process the various items and other data
*               in a transaction.
*
*               As we are processing the transaction information and updating the
*               totals information in the working buffers, we indicate which totals
*               have been modified or updated through the use of the global
*               bit mask variable TtlUpTotalFlag which uses flags such as:
*                   TTL_UPTOTAL_FIN - financial totals have been modified
*                   TTL_UPTOTAL_CPN - coupon totals have been modified
*                   TTL_UPTOTAL_CAS - cashier totals have been modified
*============================================================================
*/
VOID TtlTumReadTLWork(ULONG ulCashierID,  USHORT usTerminalNo,
                      USHORT fsModeStatus,  ULONG *puchAffectAbort, TTLCS_TMPBUF_WORKING pTmpBuff)
{
    PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

    /* === Individual Financial Total (Release 3.1) BEGIN === */
    TtlTReadUpdateFin(usTerminalNo, pTmpBuff);
    /* TtlTReadUpdateFin(); */
    /* === Individual Financial Total (Release 3.1) END === */

    TtlTReadUpdateCpn();            /* R3.0 */
    if ((ulCashierID) || (fsModeStatus & MODEQUAL_BARTENDER)) {    /* R3.1 */
        /* V3.3 */
        if(TtlTReadUpdateCas(ulCashierID, pTmpBuff, CLASS_TTLCURDAY)) {
                                    /* Read Cashier Total */
			PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_03);
            PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
            *puchAffectAbort |= TTL_UPTOTAL_CAS_ERR;
            chTtlUpdateLock = TTL_TUM_LOCK;                  /* Lock Update Total File Write */
        }
        if(TtlTReadUpdateCas(ulCashierID, pTmpBuff, CLASS_TTLCURPTD)) {
                                    /* Read Cashier Total */
			PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_04);
            PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
            *puchAffectAbort |= TTL_UPTOTAL_CAS_ERR;
            chTtlUpdateLock = TTL_TUM_LOCK;                  /* Lock Update Total File Write */
        }
    } else {
        ulTtlCasDayPosition = 0UL;
        ulTtlCasPTDPosition = 0UL;
    }
    TtlUpTotalFlag = 0;             /* Clear Total Update Flag */
    PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */
}

SHORT TtlTumUpdateDelayedBalance(VOID)
{
    TTLCS_TMPBUF_WORKING uchTmpBuff = {0};    // Temporary Buffer
    TTLTUMTRANQUAL TtlTranQualBuff = {0};     // Transaction Qualifier Save Buffer
    TTLTUMTRANUPDATE TtlTranUpBuff = {0};     // Transaction Item Save Buffer(For Decomp)
#ifdef _WIN32_WCE
#if defined(_DEBUG) || defined(DEBUG)
#pragma message("***ENABLE LOGGING***")
    ULONG    ulStartTime, ulEndTime;
    TCHAR    szMsg[ 256 ];
#endif
#endif

	//we will want to stop for a second and tell the other terminals to turn the
	//flag off so that we dont keep added to this file while we are trying to clear it out.
	NbWriteMessage(NB_MESOFFSET3, NB_REQSETDBFLAG);
	PifSleep(2500);
	//NbResetMessage(NB_MESOFFSET3, NB_RSTSETDBFLAG);

	PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_TTLUPD_START);

	if(TtlUphDBdata.usWritePointer)
	{
		ULONG        ulLength;                   // Item Data Length
		ULONG        uchAffectAbort = 0;         // Affection Lost
		ULONG        ulTranEndPointer = 0;       // Transaction End Pointer Save Area
		ULONG        ulFileMaxSize = 0;

		if((ulFileMaxSize = TrnCalStoSize(RflGetMaxRecordNumberByType(FLEX_ITEMSTORAGE_ADR), FLEX_ITEMSTORAGE_ADR)) < TTL_UPDATE_FILESIZE) {  /* 2172 */
			ulFileMaxSize = TTL_UPDATE_FILESIZE;

		}
		ulFileMaxSize *= STD_TTLUPDDB_SIZE_MOD;

		// --- Read Update Total File Data ---
		/*
			Disable power-down while we are doing a total file update.

			Request Semaphore for Update Total File Read & Write.
			This basically causes us to wait until we can get exclusive
			access to the file and the shared global variable.  We are going
			to read from the communication file and update the shared global
			variable so we want exclusive access to both.
		 */
		PifRequestSem(usTtlSemPowerDown);   // disable power-down
		PifRequestSem(usTtlSemWrite);	    // Request Semaphore for Update Total File Read & Write

		uchTtlUpdDelayBalanceFlag = TRUE;

		while(TtlUphDBdata.usCurReadPointer <= TtlUphDBdata.usWritePointer)
		{
			SHORT        sAffect = 0;                // Affection Data Exist Value Save Area
			USHORT       usCmpLength = 0;            // Item Data Compress Length
			UCHAR        uchNonCmpLength = 0;        // Item Data NonCompress Length

			if (TtlUphDBdata.usReadPointer == TtlUphDBdata.usCurReadPointer) {
				SHORT  sRetStatus = TtlTUMUpRead(&TtlTranQualBuff, TtlUphDBdata.usReadPointer, TtlUphDBdata.usWritePointer, sizeof(TTLTUMTRANQUAL), hsTtlUpHandleDB);
				/* Check Already Read Qualifier Data */
				/* Read Transaction Qualifier Data */
				if (sRetStatus != TTL_SUCCESS) {
					uchTtlUpdDelayBalanceFlag = FALSE;

					PifReleaseSem(usTtlSemWrite);     // Release Semaphore for Total File Read & Write
					PifReleaseSem(usTtlSemPowerDown); // enable power-down and jump to top of loop
					PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_TRANQUALREAD_FAIL);   /* Execute PifLog */
					PifLog (MODULE_ERROR_NO(MODULE_TTL_TUM_UPDATE), (USHORT)abs(sRetStatus));   /* Execute PifLog */
					return TTL_FAIL;
				}

				/* Update Current Read Pointer */
				TtlUphDBdata.usCurReadPointer = (TtlUphDBdata.usCurReadPointer + (sizeof(TTLTUMTRANQUAL) + sizeof(USHORT))) % ulFileMaxSize;
				ulTranEndPointer = (TtlUphDBdata.usReadPointer + TtlTranQualBuff.TtlTranVli) % ulFileMaxSize;

				/* === Individual Financial Total (Release 3.1) BEGIN === */
				TtlTumReadTLWork(TtlTranQualBuff.TtlTranModeQual.ulCashierID,
								 TtlTranQualBuff.TtlTranModeQual.uchTerm,
								 TtlTranQualBuff.TtlTranModeQual.fsModeStatus,
								 &uchAffectAbort, uchTmpBuff);
				/* === Individual Financial Total (Release 3.1) END === */
			}

			if (TtlTUMUpRead(&uchNonCmpLength, TtlUphDBdata.usCurReadPointer,
							 TtlUphDBdata.usWritePointer, sizeof(UCHAR), hsTtlUpHandleDB) != TTL_SUCCESS) {
									/* Read Transaction Item Data Length(Non compress) */
				uchTtlUpdDelayBalanceFlag = FALSE;

				PifReleaseSem(usTtlSemWrite);      // Release Semaphore for Total File Read & Write
				PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop

				return TTL_FAIL;
			}

		   if (TtlTUMUpRead((UCHAR *)&usCmpLength,
					 ((TtlUphDBdata.usCurReadPointer + uchNonCmpLength) % ulFileMaxSize),
							 TtlUphDBdata.usWritePointer, sizeof(USHORT), hsTtlUpHandleDB) != TTL_SUCCESS) { /* ### Mod (2171 for Win32) V1.0 */
									/* Read Transaction Item Data Length(Compress) */
			    uchTtlUpdDelayBalanceFlag = FALSE;

				PifReleaseSem(usTtlSemWrite);    	// Release Semaphore for Total File Read & Write
				PifReleaseSem(usTtlSemPowerDown); 	// enable power-down and jump to top of loop

				return TTL_FAIL;
			}

			ulLength = uchNonCmpLength + usCmpLength; 	// Set Item Data Length
			if (TtlTUMUpRead(uchTmpBuff, TtlUphDBdata.usCurReadPointer,
							 TtlUphDBdata.usWritePointer, ulLength, hsTtlUpHandleDB) != TTL_SUCCESS) {
									/* Read Transaction Item Data(Compress) */
				uchTtlUpdDelayBalanceFlag = FALSE;

				PifReleaseSem(usTtlSemWrite);      // Release Semaphore for Total File Read & Write
				PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_ITEMREAD_FAIL);   /* Execute PifLog */

				return TTL_FAIL;
			}

			/* Set Affection Data Exist Value */
			/* Update Current Read Pointer */
			sAffect = TtlCheckAffectClass((TTLTUMTRANUPDATE *)&uchTmpBuff[0]);
			TtlUphDBdata.usCurReadPointer = (TtlUphDBdata.usCurReadPointer + ulLength) % ulFileMaxSize;
  
			/*
				--- Update Total ----

				If this type of total transaction affects the totals, then
				we will update the totals using the information read in.

				We have to decompress the item information, then update the totals,
				then continue on with updating the global shared variable to reflect
				the new communication file status.
			 */
			if (sAffect == TTL_AFFECT) {    /* Check Affection Data Exists and if so, update totals */
				RflGetStorageItem((VOID *)&TtlTranUpBuff, uchTmpBuff, RFL_WITHOUT_MNEM);        /* Decomp Item Data */
				PifRequestSem(usTtlSemRW); 	                    // Request Semaphore for Total File Read & Write
				TtlTumUpdate(&TtlTranQualBuff, &TtlTranUpBuff, uchTmpBuff, &uchAffectAbort);    /* Updating Total */
				PifReleaseSem(usTtlSemRW); 	                    // Release Semaphore for Total File Read & Write
			}

			/* Request Semaphore for Update Total File Read & Write */
			if (TtlUphDBdata.uchResetFlag) {
				if (TtlUphDBdata.usCurReadPointer == TtlUphDBdata.usSvCurReadPointer) {
					TtlUphDBdata.uchResetFlag = 0;
				}
			}
			if (ulTranEndPointer == TtlUphDBdata.usCurReadPointer) {
									/* Check Transaction End */
				if ((TtlUphDBdata.usReadPointer == TtlUphDBdata.usWritePointer) &&
					(TtlUphDBdata.uchTtlFlag0 & TTL_UPBUFF_FULL)) {       /* Check Previous Update Total File Full */
					TtlUphDBdata.uchTtlFlag0 &= ~TTL_UPBUFF_FULL;         /* Reset Update Total File Full Flag */
				}

				/* === Individual Financial Total (Release 3.1) BEGIN === */
				TtlTumFileUpdate(TtlTranQualBuff.TtlTranModeQual.ulCashierID,
								 TtlTranQualBuff.TtlTranModeQual.uchTerm,
								 &uchAffectAbort);
				/* === Individual Financial Total (Release 3.1) END === */

				TtlUphDBdata.usReadPointer = (TtlUphDBdata.usReadPointer + TtlTranQualBuff.TtlTranVli) % ulFileMaxSize;
									/* Update Read Pointer */
				if(TtlUphDBdata.usReadPointer == TtlUphDBdata.usWritePointer)
				{
					break;
				}

			}
		}

		// Clear the management information for the total db update file
		// because we are done with it transfering information from TOTALDB
		// into the total files.  JHHJ
		memset(&TtlUphDBdata, 0x00, sizeof(TtlUphDBdata));

		uchTtlUpdDelayBalanceFlag = FALSE;

		PifReleaseSem(usTtlSemWrite); 	   // Release Semaphore for Update Total File Read & Write
		PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop

		if(uchAffectAbort)
		{
			PifLog (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
			PifLog (MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_WAI_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_WAI_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_PLU_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_PLU_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_DEP_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_DEP_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_CAS_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CAS_ERROR);   /* Execute PifLog */
			if (uchAffectAbort & TTL_UPTOTAL_CPN_ERR)
				PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CPN_ERROR);   /* Execute PifLog */
			PifLog (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
		}
	}

	PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_TTLUPD_FINISH);   /* Execute PifLog */

	return TTL_SUCCESS;
}



/*
*============================================================================
**Synopsis:     VOID  TtlTUMMain(VOID)
*
*    Input:     Nothing
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function is Total Update Module Main function.
*
                This function is a thread which takes total update requests
                and updates the total files with the necessary values.

                Basically, this thread is a consumer of total update requests which are
                produced from other threads.

                This thread initializes and then does a PifRequestSem(usTtlSemUpdate)
                waiting for some other thread to put information into the TOTALUPD
                buffer file.  When the other thread puts information into that file,
                it will then do a release on the usTtlSemUpdate semaphore allowing the
                TtlTUMMain thread to run and update the totals.  See functions
                TtlUpdateFile() and TtlUpdateFileFH() in file totalfl.c.

                Communication is through the shared global variable, TtlUphdata, which
                is part of the UNINIRAM area.  This shared variable contains the current
                TOTALUPD file status allowing the file to be used as a communication mechanism
                between TtlTUMMain and producers of total information.  This global variable
                and the communication file itself are guarded by the semaphore usTtlSemWrite
                which coordinates both updates to the global variable and the file.

                NOTES:
				1. TtlUphdata.uchResetFlag usage
                    If we are in reset recovery mode then we need to determine if
                    we have processed all of the items that we had processed before
                    this terminal went down.  We will then turn off the reset
                    recovery mode flag.  This flag is used the PLU totals source
                    to determine if the totals for a particular PLU should be updated
                    or not.  The reason is that the PLU totals are kept in a database
                    using a real database engine so the PLU totals are updated as they
                    are processed.  The other totals such as financial totals or cashier
                    totals are accumulated in the working buffers and then saved to the
                    appropriate file when the transaction end is found.
*============================================================================
*/
static union {
	TTLCSREGFIN         TtlFinDayWork;    /* Daily Financial Work Area */
	TTLCPNDAYREADTOTAL  TtlCpnDayWork;    /* Daily Coupon Work Area, R3.0 */
	TTLCSCASTOTAL       TtlCasDayWork;    /* Daily Cashier Work Area */
	TTLCSDEPTMISC       TtlDeptDayMisc;      /* Misc. Departmental Work Area  */
	TTLSERTIME          TtlService;
	TTLHOURLY           TtlHourly;
	TTLCSDEPTINDX       TtlDepartments[250];
} area1;

static union {
	TTLCSREGFIN         TtlFinPTDWork;     /* PTD Financial Work Area */
	TTLCPNPTDREADTOTAL  TtlCpnPTDWork;     /* PTD Coupon Work Area,   R3.0 */
	TTLCSCASTOTAL       TtlCasPTDWork;     /* PTD Cashier Work Area, V3.3 */
	TTLCSDEPTMISC       TtlDeptPTDMisc;      /* Misc. Departmental Work Area  */
	TTLSERTIME          TtlService;
	TTLHOURLY           TtlHourly;
	TTLCSDEPTINDX       TtlDepartmentsJoin[250];
} area2;

static union {
	TTLCSREGFIN         TtlIndFinWork;       /* Individual Financial Work Area, R3.1 */
	TTLDEPT             TtlDepartments[250];
} area3;

static TOTAL  * TtlAddTtlTotal (TOTAL *pTtlTotalDest, TOTAL *pTtlTotalSrc)
{
	pTtlTotalDest->lAmount  += pTtlTotalSrc->lAmount;
	pTtlTotalDest->sCounter += pTtlTotalSrc->sCounter;

	return pTtlTotalDest;
}

static LTOTAL  * TtlAddTtlTotal_l (LTOTAL *pTtlTotalDest, LTOTAL *pTtlTotalSrc)
{
	pTtlTotalDest->lAmount  += pTtlTotalSrc->lAmount;
	pTtlTotalDest->lCounter += pTtlTotalSrc->lCounter;

	return pTtlTotalDest;
}

static  TAXABLE  * TtlAddTtlTaxable (TAXABLE *pTtlTaxableDest, TAXABLE *pTtlTaxbleSrc)
{
	pTtlTaxableDest->mlTaxableAmount  += pTtlTaxbleSrc->mlTaxableAmount;    /* Taxable Amount */
	pTtlTaxableDest->lTaxAmount       += pTtlTaxbleSrc->lTaxAmount;    /* Tax Amount */
	pTtlTaxableDest->lTaxExempt       += pTtlTaxbleSrc->lTaxExempt;    /* Tax Exempt */
	pTtlTaxableDest->lFSTaxExempt     += pTtlTaxbleSrc->lFSTaxExempt;  /* FoodStamp Tax Exempt */

	return pTtlTaxableDest;
}

static  TEND  * TtlAddTtlTend (TEND *pTtlTendDest, TEND *pTtlTendSrc)
{
    TtlAddTtlTotal(&(pTtlTendDest->OnlineTotal), &(pTtlTendSrc->OnlineTotal));      /* Online To/Co         */
    TtlAddTtlTotal(&(pTtlTendDest->OfflineTotal), &(pTtlTendSrc->OfflineTotal));    /* Offline To/Co        */

	return pTtlTendDest;
}


static  TENDHT  * TtlAddTtlTendH (TENDHT *pTtlTendHDest, TENDHT *pTtlTendHSrc)
{
    TtlAddTtlTotal(&(pTtlTendHDest->Total), &(pTtlTendHSrc->Total));      /* Online To/Co         */
    pTtlTendHDest->lHandTotal += pTtlTendHSrc->lHandTotal;

	return pTtlTendHDest;
}

#define CALC_NO_ARRAY_ITEMS(x)  (sizeof(x)/sizeof((x)[0]))

static TTLCSREGFIN * TtlAddTtlFin (TTLCSREGFIN *pTtlFinDest, TTLCSREGFIN *pTtlFinSrc)
{
	int   iLoop;

#if 0
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */
#endif

//    D13DIGITS   mlCGGTotal;         /* Current Gross Group Total */
	pTtlFinDest->mlCGGTotal  += pTtlFinSrc->mlCGGTotal;    /* Current Gross Group Total */

	pTtlFinDest->lNetTotal  += pTtlFinSrc->lNetTotal;    /* Net Total,   Release 3.1 */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->Taxable); iLoop++) {
		TtlAddTtlTaxable(pTtlFinDest->Taxable + iLoop, pTtlFinSrc->Taxable + iLoop);       /* Taxable Total 1 to 4, R3.0 */
	}

//    D13DIGITS   NonTaxable;         /* Non taxable Total */
	pTtlFinDest->NonTaxable  += pTtlFinSrc->NonTaxable;    /* Non taxable Total */

	pTtlFinDest->lTGGTotal  += pTtlFinSrc->lTGGTotal;    /* Training Gross Group Total */
    TtlAddTtlTotal(&(pTtlFinDest->DeclaredTips), &(pTtlFinSrc->DeclaredTips));       /* Declared Tips */
	pTtlFinDest->lDGGtotal  += pTtlFinSrc->lDGGtotal;    /* Daily Gross Group Total */

    TtlAddTtlTotal(&(pTtlFinDest->PlusVoid), &(pTtlFinSrc->PlusVoid));       /* Plus Void */
    TtlAddTtlTotal(&(pTtlFinDest->PreselectVoid), &(pTtlFinSrc->PreselectVoid));       /* Preselect Void */
    TtlAddTtlTotal(&(pTtlFinDest->ConsCoupon), &(pTtlFinSrc->ConsCoupon));       /* Consolidated Coupon */
    TtlAddTtlTotal(&(pTtlFinDest->ItemDisc), &(pTtlFinSrc->ItemDisc));       /* Item Discount */
    TtlAddTtlTotal(&(pTtlFinDest->ModiDisc), &(pTtlFinSrc->ModiDisc));       /* Modified Item Discount */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->RegDisc); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->RegDisc + iLoop, pTtlFinSrc->RegDisc + iLoop);       /* Modified Item Discount */
	}

    TtlAddTtlTotal(&(pTtlFinDest->Coupon), &(pTtlFinSrc->Coupon));       /* Combination Coupon, R3.0 */
    TtlAddTtlTotal(&(pTtlFinDest->PaidOut), &(pTtlFinSrc->PaidOut));     /* Paid Out */
    TtlAddTtlTotal(&(pTtlFinDest->RecvAcount), &(pTtlFinSrc->RecvAcount));   /* Received on Acount */
    TtlAddTtlTotal(&(pTtlFinDest->TipsPaid), &(pTtlFinSrc->TipsPaid));   /* Tips Paid Out */
    TtlAddTtlTotal(&(pTtlFinDest->FoodStampCredit), &(pTtlFinSrc->FoodStampCredit));   /* Food Stamp Credit */

    TtlAddTtlTend(&(pTtlFinDest->CashTender), &(pTtlFinSrc->CashTender));   /* Cash Tender          change EPT*/
    TtlAddTtlTend(&(pTtlFinDest->CheckTender), &(pTtlFinSrc->CheckTender)); /* Check Tender         change EPT*/
    TtlAddTtlTend(&(pTtlFinDest->ChargeTender), &(pTtlFinSrc->ChargeTender));  /* Charge Tender        change EPT*/

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->MiscTender); iLoop++) {
		TtlAddTtlTend(pTtlFinDest->MiscTender + iLoop, pTtlFinSrc->MiscTender + iLoop);   /* Misc Tender 1 to 8   change EPT*/
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->ForeignTotal); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->ForeignTotal + iLoop, pTtlFinSrc->ForeignTotal + iLoop);    /* Foreign Total 1 to 8,    Saratoga */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->ServiceTotal); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->ServiceTotal + iLoop, pTtlFinSrc->ServiceTotal + iLoop);    /* Service Total 3 to 8, 10-20 JHHJ */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->AddCheckTotal); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->AddCheckTotal + iLoop, pTtlFinSrc->AddCheckTotal + iLoop);    /* Add Check Total 1 to 3 */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->ChargeTips); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->ChargeTips + iLoop, pTtlFinSrc->ChargeTips + iLoop);    /* Charge Tips, V3.3 */
	}

	TtlAddTtlTotal(&(pTtlFinDest->Loan), &(pTtlFinSrc->Loan));               /* Loan,    Saratoga */
	TtlAddTtlTotal(&(pTtlFinDest->Pickup), &(pTtlFinSrc->Pickup));           /* Pickup,  Saratoga */
	TtlAddTtlTotal(&(pTtlFinDest->TransCancel), &(pTtlFinSrc->TransCancel)); /* Transaction Cancel */
	TtlAddTtlTotal(&(pTtlFinDest->MiscVoid), &(pTtlFinSrc->MiscVoid));       /* Misc Void */
	TtlAddTtlTotal(&(pTtlFinDest->Audaction), &(pTtlFinSrc->Audaction));     /* Audaction */

	pTtlFinDest->sNoSaleCount  += pTtlFinSrc->sNoSaleCount;      /* No Sale Counter */
	pTtlFinDest->lItemProductivityCount  += pTtlFinSrc->lItemProductivityCount;  /* Item Productivity Counter */
	pTtlFinDest->sNoOfPerson  += pTtlFinSrc->sNoOfPerson;        /* Number of Person Counter */
	pTtlFinDest->sNoOfChkOpen  += pTtlFinSrc->sNoOfChkOpen;      /* Number of Checks Opened */
	pTtlFinDest->sNoOfChkClose  += pTtlFinSrc->sNoOfChkClose;    /* Number of Checks Closed */
	pTtlFinDest->sCustomerCount  += pTtlFinSrc->sCustomerCount;  /* Customer Counter */

	TtlAddTtlTotal(&(pTtlFinDest->HashDepartment), &(pTtlFinSrc->HashDepartment));  /* Hash Department */
	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->Bonus); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->Bonus + iLoop, pTtlFinSrc->Bonus + iLoop);                /* Bonus Total 1 to 8 */
	}

	pTtlFinDest->lConsTax  += pTtlFinSrc->lConsTax;  /* Consolidation tax */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlFinDest->aUPCCoupon); iLoop++) {
		TtlAddTtlTotal(pTtlFinDest->aUPCCoupon + iLoop, pTtlFinSrc->aUPCCoupon + iLoop);       /* Coupon (Single/Double/Triple), saratoga */
	}

	return pTtlFinDest;
}


static TTLCSCASTOTAL * TtlAddTtlCas (TTLCSCASTOTAL *pTtlCasDest, TTLCSCASTOTAL *pTtlCasSrc)
{
	int   iLoop;

#if 0
    UCHAR       uchResetStatus;     /* Reset Report Status */
    DATE        FromDate;           /* Preiod From */
    DATE        ToDate;             /* Preiod To */

#endif

	pTtlCasDest->lDGGtotal  += pTtlCasSrc->lDGGtotal;  /* Daily Gross Group Total */

	TtlAddTtlTotal(&(pTtlCasDest->PlusVoid), &(pTtlCasSrc->PlusVoid));            /* Plus Void */
	TtlAddTtlTotal(&(pTtlCasDest->PreselectVoid), &(pTtlCasSrc->PreselectVoid));  /* Preselect Void */
	TtlAddTtlTotal(&(pTtlCasDest->ConsCoupon), &(pTtlCasSrc->ConsCoupon));        /* Consolidated Coupon */
	TtlAddTtlTotal(&(pTtlCasDest->ItemDisc), &(pTtlCasSrc->ItemDisc));            /* Item Discount */
	TtlAddTtlTotal(&(pTtlCasDest->ModiDisc), &(pTtlCasSrc->ModiDisc));            /* Modified Item Discount */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->RegDisc); iLoop++) {
		TtlAddTtlTotal(pTtlCasDest->RegDisc + iLoop, pTtlCasSrc->RegDisc + iLoop);    /* Regular Discount 1 to 6, Release 3.1 */
	}

	TtlAddTtlTotal(&(pTtlCasDest->Coupon), &(pTtlCasSrc->Coupon));                /* Combination Coupon, R3.0 */
	TtlAddTtlTotal(&(pTtlCasDest->PaidOut), &(pTtlCasSrc->PaidOut));              /* Paid Out */
	TtlAddTtlTotal(&(pTtlCasDest->RecvAcount), &(pTtlCasSrc->RecvAcount));        /* Received on Acount */
	TtlAddTtlTotal(&(pTtlCasDest->TipsPaid), &(pTtlCasSrc->TipsPaid));            /* Tips Paid Out */
	TtlAddTtlTotal(&(pTtlCasDest->FoodStampCredit), &(pTtlCasSrc->FoodStampCredit));   /* Food Stamp Credit */

    TtlAddTtlTendH(&(pTtlCasDest->CashTender), &(pTtlCasSrc->CashTender));       /* Cash Tender */
    TtlAddTtlTendH(&(pTtlCasDest->CheckTender), &(pTtlCasSrc->CheckTender));     /* Check Tender */
    TtlAddTtlTendH(&(pTtlCasDest->ChargeTender), &(pTtlCasSrc->ChargeTender));   /* Charge Tender */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->MiscTender); iLoop++) {
		TtlAddTtlTendH(pTtlCasDest->MiscTender + iLoop, pTtlCasSrc->MiscTender + iLoop);       /* Misc Tender 1 to 8 */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->ForeignTotal); iLoop++) {
		TtlAddTtlTendH(pTtlCasDest->ForeignTotal + iLoop, pTtlCasSrc->ForeignTotal + iLoop);   /* Foreign Total 1 to 8,    Saratoga */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->ServiceTotal); iLoop++) {
		TtlAddTtlTotal(pTtlCasDest->ServiceTotal + iLoop, pTtlCasSrc->ServiceTotal + iLoop);    /* Service Total 3 to 8, 10-20 JHHJ */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->AddCheckTotal); iLoop++) {
		TtlAddTtlTotal(pTtlCasDest->AddCheckTotal + iLoop, pTtlCasSrc->AddCheckTotal + iLoop);    /* Add Check Total 1 to 3 */
	}

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->ChargeTips); iLoop++) {
		TtlAddTtlTotal(pTtlCasDest->ChargeTips + iLoop, pTtlCasSrc->ChargeTips + iLoop);    /* Charge Tips */
	}

	TtlAddTtlTotal(&(pTtlCasDest->Loan), &(pTtlCasSrc->Loan));                /* Loan,    Saratoga */
	TtlAddTtlTotal(&(pTtlCasDest->Pickup), &(pTtlCasSrc->Pickup));            /* Pickup,  Saratoga */
	TtlAddTtlTotal(&(pTtlCasDest->TransCancel), &(pTtlCasSrc->TransCancel));  /* Transaction Cancel */
	TtlAddTtlTotal(&(pTtlCasDest->MiscVoid), &(pTtlCasSrc->MiscVoid));        /* Misc Void */
	TtlAddTtlTotal(&(pTtlCasDest->Audaction), &(pTtlCasSrc->Audaction));      /* Audaction */

	pTtlCasDest->sNoSaleCount    += pTtlCasSrc->sNoSaleCount;    /* No Sale Counter */
	pTtlCasDest->lItemProductivityCount  += pTtlCasSrc->lItemProductivityCount;  /* Item Productivity Counter */
	pTtlCasDest->sNoOfPerson     += pTtlCasSrc->sNoOfPerson;     /* Number of Person Counter */
	pTtlCasDest->sNoOfChkOpen    += pTtlCasSrc->sNoOfChkOpen;    /* Number of Checks Opened */
	pTtlCasDest->sNoOfChkClose   += pTtlCasSrc->sNoOfChkClose;   /* Number of Checks Closed */
	pTtlCasDest->sCustomerCount  += pTtlCasSrc->sCustomerCount;  /* Customer Counter */

	TtlAddTtlTotal(&(pTtlCasDest->HashDepartment), &(pTtlCasSrc->HashDepartment));    /* Hash Department */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->Bonus); iLoop++) {
		TtlAddTtlTotal(pTtlCasDest->Bonus + iLoop, pTtlCasSrc->Bonus + iLoop);    /* Bonus Total 1 to 8 */
	}

	pTtlCasDest->lConsTax  += pTtlCasSrc->lConsTax;  /* Consolidation tax */
	pTtlCasDest->usPostRecCo  += pTtlCasSrc->usPostRecCo;  /* post receipt counter, R3,1 */
	pTtlCasDest->lVATServiceTotal  += pTtlCasSrc->lVATServiceTotal;  /* Service Total of VAT, V3.3*/

	TtlAddTtlTotal(&(pTtlCasDest->CheckTransTo), &(pTtlCasSrc->CheckTransTo));       /* Checks Transferred to, V3.3 */
	TtlAddTtlTotal(&(pTtlCasDest->CheckTransFrom), &(pTtlCasSrc->CheckTransFrom));   /* Checks Transferred from, V3.3 */
	TtlAddTtlTotal(&(pTtlCasDest->DeclaredTips), &(pTtlCasSrc->DeclaredTips));       /* Declared Tips, V3.3 */

	for (iLoop = 0; iLoop < CALC_NO_ARRAY_ITEMS(pTtlCasDest->aUPCCoupon); iLoop++) {
		TtlAddTtlTotal(pTtlCasDest->aUPCCoupon + iLoop, pTtlCasSrc->aUPCCoupon + iLoop);    /* Coupon (Single/Double/Triple), saratoga */
	}

	return pTtlCasDest;
}



VOID THREADENTRY TtlTUMJoinDisconnectedSatellite(VOID)
{
    TTLCS_TMPBUF_WORKING   uchTmpBuff = {0};      // Temporary Buffer for item information
    ULONG   uchAffectAbort = 0;             // Affection Lost

    /*
        A loop which runs forever handling total file updates.

        The first step is to wait on the usTtlSemUpdate semaphore until some other
        thread updates the communication file and the shared global file status variable
        and then releases the semaphore so that we can then do a total update.
     */

	// if the handle to the queue has not be initialized then we are just
	// a standard Satellite terminal and so we should not be doing this
	// thread as this is Master Terminal functionality only.
	if (usTtlQueueJoinUpdate > 0xf000)
		return;

    for (;;) {
		USHORT  *pusTerminalPosition = NULL;

        pusTerminalPosition = PifReadQueue(usTtlQueueJoinUpdate);  // Read the queue Semaphore for Update Module Start
		if (pusTerminalPosition == NULL) {
			PifSleep (1000);
			continue;
		}

		PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_TOTALUM_JOINMSG_START);
		PifLog (MODULE_TTL_TUM_UPDATE, *pusTerminalPosition);

        /*
            Disable power-down while we are doing a total file update.

            Request Semaphore for Update Total File Read & Write.
            This basically causes us to wait until we can get exclusive
            access to the file and the shared global variable.  We are going
            to read from the communication file and update the shared global
            variable so we want exclusive access to both.
         */
        PifRequestSem(usTtlSemPowerDown);   // disable power-down
        PifRequestSem(usTtlSemWrite);       // Request Semaphore for Update Total File Read & Write

		{
			USHORT  uchTerminalNo = *pusTerminalPosition;
			int     iLoop;
			SHORT   hsTtlHandleSave = -1;
			SHORT   hsTtlHandleSave1 = -1;
			SHORT   hsTtlHandleSave2 = -1;
			SHORT   hsTtlHandleJoin = -1;
			SHORT   hsTtlHandleJoin1 = -1;
			SHORT   hsTtlHandleJoin2 = -1;
			USHORT  szTtlBaseFileNameJoin = 0;
			USHORT  szTtlCasFileNameJoin = 1;
			USHORT  szTtlCpnFileNameJoin = 2;
			USHORT  szTtlDeptFileNameJoin = 3;
			USHORT  szTtlIndFileNameJoin = 5;
			USHORT  szTtlSerTimeNameJoin = 6;
			USHORT  szTtlPluDayFileNameJoin = 7;
			USHORT  szTtlPluPTDFileNameJoin = 8;
			TCHAR   szTotalLocalFileName[16];
			TCHAR *(TotalLocalFileNameList []) = {
				_T("ZATALBAS"),   /* Base Total File Name, szTtlBaseFileName */
				_T("ZATALCAS"),   /* Cashier Total File Name, szTtlCasFileName */
				_T("ZATALCPN"),   /* Coupon Total File Name, R3.0, szTtlCpnFileName */
				_T("ZATALDEP"),   /* Dept Total File Name, szTtlDeptFileName */
				_T("ZATALETK"),   /* Employee Time Keeping Total File Name */
				_T("ZATALIND"),   /* Individual Financial, R3.1, szTtlIndFileName */
				_T("ZATALTIM"),   /* Serive Time, R3.1, szTtlSerTimeName */
				_T("ZATALPLD"),   /* PLU Totals interim file, CLASS_TTLCURDAY */
				_T("ZATALPLP"),   /* PLU Totals interim file, CLASS_TTLCURPTD */
				NULL };

			// Now that we update the totals for each of the items in the transaction.
			// The rest of this loop is executed as we read the Total File for the items
			// in the transaction and update the various totals based on the item information
			// in the transaction.

			// The first totals we will update will be the totals for AC 23 and AC223
			// this are the basic system totals as well as the terminal specific totals.
			// The Disconnected Satellite has been operating as Terminal # 1, a standalone Master Terminal
			// however in the cluster it has been assigned some other terminal number.
			// So we take tht totals from the Disconnected Satellite as Terminal #1 and update
			// the Master Terminals totals specifying the terminal number that has been assigned to
			// the Disconnected Satellite while it is operatined as a Joined terminal and standard Satellite.

			if (uchTerminalNo > PIF_NET_MAX_IP)
				uchTerminalNo = 0;

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlBaseFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			hsTtlHandleJoin1 = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlIndFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			hsTtlHandleJoin2 = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);

			if (hsTtlHandleJoin1 >= 0) {
				extern ULONG   ulTtlIndFinPosition;       /* Individual Financial Position , R3.1 */
				ULONG          ulTtlIndFinPositionSave;   /* Individual Financial Position , R3.1 */
				UCHAR          ClassTypes[] = {CLASS_TTLCURDAY, CLASS_TTLCURPTD};

				PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

				// Save the current file handle and pointer state for these totals
				hsTtlHandleSave1 = hsTtlBaseHandle;
				hsTtlHandleSave2 = hsTtlIndFinHandle;

				TtlTReadUpdateFin(uchTerminalNo, uchTmpBuff);

				// Save the financial totals state for now.
				area1.TtlFinDayWork = TtlFinDayWork;
				area2.TtlFinPTDWork = TtlFinPTDWork;
				area3.TtlIndFinWork = TtlIndFinWork;

				ulTtlIndFinPositionSave = ulTtlIndFinPosition;

				hsTtlBaseHandle = hsTtlHandleJoin1;
				hsTtlIndFinHandle = hsTtlHandleJoin2;

				// Read the totals file from the Disconnected Satellite that has been Joined
				TtlTReadUpdateFin(uchTerminalNo, uchTmpBuff);

				TtlUpTotalFlag |= TTL_UPTOTAL_FIN;   /* Set Financial Updated Flag */

				//  Add the totals information from the Disconnected Satellite with
				//  the totals from this Master terminal
				TtlAddTtlFin (&TtlFinDayWork, &area1.TtlFinDayWork);
				TtlAddTtlFin (&TtlFinPTDWork, &area2.TtlFinPTDWork);
				TtlAddTtlFin (&TtlIndFinWork, &area3.TtlIndFinWork);

				// Put the saved totals context from the Master Terminal totals read
				// back into the global variables.  Then update the Master Terminal
				// financial totals.
				hsTtlBaseHandle   = hsTtlHandleSave1;
				hsTtlIndFinHandle = hsTtlHandleSave2;
				ulTtlIndFinPosition = ulTtlIndFinPositionSave;

				TtlTUpdateFin(uchTerminalNo);

				TtlUpTotalFlag &= ~TTL_UPTOTAL_FIN;   /* Unset Financial Updated Flag */

				// -----------------------------------------------------------------
				// Now do the hourly totals.  First we do the CLASS_TTLCURDAY then CLASS_TTLCURPTD
				// and update our hourly totals file.
				for (iLoop = 0; iLoop < 2; iLoop++) {
					int   jLoop, kLoop;

					memset(&area1.TtlHourly, 0, sizeof(area1.TtlHourly));
					memset(&area2.TtlHourly, 0, sizeof(area2.TtlHourly));

					area2.TtlHourly.uchMajorClass = area1.TtlHourly.uchMajorClass = CLASS_TTLHOURLY;
					area2.TtlHourly.uchMinorClass = area1.TtlHourly.uchMinorClass = ClassTypes[iLoop];

					TtlTreadHour (&area1.TtlHourly);

					hsTtlBaseHandle = hsTtlHandleJoin1;

					TtlTreadHour (&area2.TtlHourly);

					for (jLoop = 0; jLoop < 48; jLoop++) {
						area1.TtlHourly.Total[jLoop].lHourlyTotal += area2.TtlHourly.Total[jLoop].lHourlyTotal;                       /* Hourly Total */
						area1.TtlHourly.Total[jLoop].lItemproductivityCount += area2.TtlHourly.Total[jLoop].lItemproductivityCount;   /* IP Counter */
						area1.TtlHourly.Total[jLoop].sDayNoOfPerson += area2.TtlHourly.Total[jLoop].sDayNoOfPerson;                   /* Number of Person Counter */
						for (kLoop = 0; kLoop < STD_NO_BONUS_TTL; kLoop++) {
							area1.TtlHourly.Total[jLoop].lBonus[kLoop] += area2.TtlHourly.Total[jLoop].lBonus[kLoop];
						}
					}

					hsTtlBaseHandle = hsTtlHandleSave1;
					TtlTwriteHour(&area1.TtlHourly);
				}

				PifReleaseSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

				PifCloseFile (hsTtlHandleJoin1);  hsTtlHandleJoin1 = -1;
				PifCloseFile (hsTtlHandleJoin2);  hsTtlHandleJoin2 = -1;
			}

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlCasFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			hsTtlHandleJoin = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);
			if (hsTtlHandleJoin >= 0) {
				extern ULONG   ulTtlCasDayPosition;    /* Individual Financial Position , R3.1 */
				extern ULONG   ulTtlCasPTDPosition;    /* Individual Financial Position , R3.1 */
				ULONG          ulTtlCasDayPositionSave   ;    /* Individual Financial Position , R3.1 */
				ULONG          ulTtlCasPTDPositionSave   ;    /* Individual Financial Position , R3.1 */
				ULONG          ulCashierID;
				TTLCSCASHEAD   Cashd;

				PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

				hsTtlHandleSave = hsTtlCasHandle;

				if (TtlCasHeadRead(&Cashd) == 0) { /* Read Cashier File Header */
					SHORT         sRetStatus = TTL_SUCCESS;
					ULONG         ulPositionInd = 0, ulActualPosition, ulActualBytesRead;
					TTLCSCASINDX  CashierIndx;
					ULONG         ulCashierNumber;

					TtlUpTotalFlag |= TTL_UPTOTAL_CAS;  // Set financial flag

					for (ulCashierID = 0; ulCashierID < Cashd.usMaxCas; ulCashierID++) {

						hsTtlCasHandle = hsTtlHandleJoin;

						ulPositionInd = Cashd.ulCurDayIndexOff + ulCashierID * sizeof(TTLCSCASINDX);

						if (PifSeekFile(hsTtlCasHandle, ulPositionInd, &ulActualPosition) < 0) {  /* Seek Pointer of Index Table */
							continue;
						}

						//Change made in PifReadFile, see piffile.c for new usage
						PifReadFile(hsTtlCasHandle, &CashierIndx, sizeof(TTLCSCASINDX), &ulActualBytesRead);
						ulCashierNumber = CashierIndx.ulCasNo;

						memset (&TtlCasDayWork, 0, sizeof(TtlCasDayWork));
						memset (&TtlCasPTDWork, 0, sizeof(TtlCasPTDWork));
						ulTtlCasDayPosition = 0;
						ulTtlCasPTDPosition = 0;

						hsTtlCasHandle = hsTtlHandleSave;

						sRetStatus = TtlTReadUpdateCas(ulCashierNumber, uchTmpBuff, CLASS_TTLCURDAY);
						if(sRetStatus) {
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_03);
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), (USHORT)ulCashierNumber);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), chTtlUpdateLock);
							PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_CASHIER), (USHORT)__LINE__);
							continue;
						}

						sRetStatus = TtlTReadUpdateCas(ulCashierNumber, uchTmpBuff, CLASS_TTLCURPTD);
						if(sRetStatus) {
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_04);
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), (USHORT)ulCashierNumber);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), chTtlUpdateLock);
							PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_CASHIER), (USHORT)__LINE__);
							continue;
						}


						ulTtlCasDayPositionSave = ulTtlCasDayPosition;
						ulTtlCasPTDPositionSave = ulTtlCasPTDPosition;
						// Save the financial totals state for now.
						area1.TtlCasDayWork = TtlCasDayWork;
						area2.TtlCasPTDWork = TtlCasPTDWork;

						hsTtlCasHandle = hsTtlHandleJoin;

						sRetStatus = TtlTReadUpdateCas(ulCashierNumber, uchTmpBuff, CLASS_TTLCURDAY);
						if(sRetStatus) {
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_03);
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), (USHORT)ulCashierNumber);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), chTtlUpdateLock);
							PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_CASHIER), (USHORT)__LINE__);
							continue;
						}

						sRetStatus = TtlTReadUpdateCas(ulCashierNumber, uchTmpBuff, CLASS_TTLCURPTD);
						if(sRetStatus) {
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_CODE_04);
							PifLog(MODULE_TTL_TUM_CASHIER, LOG_ERROR_TTL_AFFECT);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), (USHORT)ulCashierNumber);
							PifLog(MODULE_DATA_VALUE(MODULE_TTL_TUM_CASHIER), chTtlUpdateLock);
							PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_CASHIER), (USHORT)__LINE__);
							continue;
						}

						TtlAddTtlCas(&TtlCasDayWork, &area1.TtlCasDayWork);
						TtlAddTtlCas(&TtlCasPTDWork, &area2.TtlCasPTDWork);

						hsTtlCasHandle = hsTtlHandleSave;
						ulTtlCasDayPosition = ulTtlCasDayPositionSave;
						ulTtlCasPTDPosition = ulTtlCasPTDPositionSave;

						if (TtlTUpdateCas(ulCashierNumber, CLASS_TTLCURDAY) != TTL_SUCCESS) {
							NHPOS_ASSERT_TEXT(0, "TtlTUpdateCas(usCashierNumber, CLASS_TTLCURDAY)  failed.");
						}

						if (TtlTUpdateCas(ulCashierNumber, CLASS_TTLCURPTD) != TTL_SUCCESS) {
							NHPOS_ASSERT_TEXT(0, "TtlTUpdateCas(usCashierNumber, CLASS_TTLCURPTD)  failed.");
						}
					}
					TtlUpTotalFlag &= ~TTL_UPTOTAL_CAS;  // Unset financial flag
				}

				hsTtlCasHandle = hsTtlHandleSave;

				PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;

				PifReleaseSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */
			}

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlCpnFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			if (hsTtlCpnHandle >= 0)
				hsTtlHandleJoin = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);

			if (hsTtlHandleJoin >= 0) {
				PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

				TtlTReadUpdateCpn();            /* R3.0 */
				area1.TtlCpnDayWork = TtlCpnDayWork;
				area2.TtlCpnPTDWork = TtlCpnPTDWork;

				hsTtlHandleSave = hsTtlCpnHandle;
				hsTtlCpnHandle = hsTtlHandleJoin;
				TtlTReadUpdateCpn();            /* R3.0 */

				//  Need to also handle TTLCSCPNMISC struct
#pragma message("------------------------------------    TtlTUMJoinDisconnectedSatellite() incomplete  \z  -----------------------")
				for (iLoop = 0; iLoop < FLEX_CPN_MAX; iLoop++) {
					TtlAddTtlTotal(TtlCpnDayWork.CpnDayTotal + iLoop, area1.TtlCpnDayWork.CpnDayTotal + iLoop);
					TtlAddTtlTotal(TtlCpnPTDWork.CpnPTDTotal + iLoop, area2.TtlCpnPTDWork.CpnPTDTotal + iLoop);
				}

				TtlUpTotalFlag |= TTL_UPTOTAL_CPN;   /* Set Financial Updated Flag */

				hsTtlCpnHandle = hsTtlHandleSave;
				TtlTUpdateCpn();
				TtlUpTotalFlag &= ~TTL_UPTOTAL_CPN;   /* Unset Financial Updated Flag */

				PifReleaseSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

				PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
			}

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlDeptFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			if (hsTtlDeptHandle >= 0)
				hsTtlHandleJoin = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);

			if (hsTtlHandleJoin >= 0) {
				ULONG            ulPosition[4], ulPositionIndex[4];
				TTLCSDEPTHEAD    Depthd = {0};
				ULONG            ulPositionJoin[4], ulPositionJoinIndex[4];
				TTLCSDEPTHEAD    DepthdJoin = {0};
				SHORT            sReturn2;
				UCHAR            ClassTypes[] = {CLASS_TTLCURDAY, CLASS_TTLSAVDAY, CLASS_TTLCURPTD, CLASS_TTLSAVPTD};

				PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

				hsTtlHandleSave = hsTtlDeptHandle;

				if (TtlDeptHeadRead(&Depthd) == 0) {          /* Read Department File Header */
					hsTtlDeptHandle = hsTtlHandleJoin;

					if (TtlDeptHeadRead(&DepthdJoin) < 0) {  /* Read Department File Header from transferred file */
						NHPOS_ASSERT_TEXT(0, "THREADENTRY TtlTUMJoinDisconnectedSatellite(): TtlDeptHeadRead(&DepthdJoin) < 0 FAILED.");
					}
					else {
						ulPosition[0] = Depthd.ulCurDayMiscOff;
						ulPosition[1] = Depthd.ulPreDayMiscOff;
						ulPosition[2] = Depthd.ulCurPTDMiscOff;
						ulPosition[3] = Depthd.ulPrePTDMiscOff;
						ulPositionJoin[0] = DepthdJoin.ulCurDayMiscOff;
						ulPositionJoin[1] = DepthdJoin.ulPreDayMiscOff;
						ulPositionJoin[2] = DepthdJoin.ulCurPTDMiscOff;
						ulPositionJoin[3] = DepthdJoin.ulPrePTDMiscOff;

						ulPositionIndex[0] = Depthd.ulCurDayIndexOff;
						ulPositionIndex[1] = Depthd.ulPreDayIndexOff;
						ulPositionIndex[2] = Depthd.ulCurPTDIndexOff;
						ulPositionIndex[3] = Depthd.ulPrePTDIndexOff;
						ulPositionJoinIndex[0] = DepthdJoin.ulCurDayIndexOff;
						ulPositionJoinIndex[1] = DepthdJoin.ulPreDayIndexOff;
						ulPositionJoinIndex[2] = DepthdJoin.ulCurPTDIndexOff;
						ulPositionJoinIndex[3] = DepthdJoin.ulPrePTDIndexOff;

						hsTtlDeptHandle = hsTtlHandleSave;         // Set the file handle to the standard department handle
						for (iLoop = 0; iLoop < 4; iLoop++) {
							TTLCSDEPTMISC    TtlDeptMisc = {0};
							TTLCSDEPTMISC    TtlDeptMiscJoin = {0};
							USHORT    usDEPTNumber;
							ULONG     ulActualPosition = 0, ulActualBytesRead = 0;
							USHORT    usArea1, usArea2;

							TtlDeptMiscRead(ulPosition[iLoop], &TtlDeptMisc);

							hsTtlDeptHandle = hsTtlHandleJoin;
							if (ulPositionJoin[iLoop]) {
								TtlDeptMiscRead(ulPositionJoin[iLoop], &TtlDeptMiscJoin);
							}

							TtlAddTtlTotal_l(&(TtlDeptMisc.DeptAllTotal), &(TtlDeptMiscJoin.DeptAllTotal));

							hsTtlDeptHandle = hsTtlHandleSave;

							if (TtlDeptMiscWrite(ulPosition[iLoop], &TtlDeptMisc) != 0) {
								NHPOS_ASSERT_TEXT(0, "TtlDeptMiscWrite(ulPosition[iLoop], &TtlDeptMisc) != 0");
							}

							memset (area1.TtlDepartments, 0, sizeof(area1.TtlDepartments));
							memset (area2.TtlDepartmentsJoin, 0, sizeof(area2.TtlDepartmentsJoin));

							if (PifSeekFile(hsTtlHandleSave, ulPositionIndex[iLoop], &ulActualPosition) >= 0) {
								for (usDEPTNumber = 0; usDEPTNumber < Depthd.usMaxDept; usDEPTNumber++) {
									PifReadFile(hsTtlHandleSave, area1.TtlDepartments + usDEPTNumber, sizeof(TTLCSDEPTINDX), &ulActualBytesRead);
									if (ulActualBytesRead < sizeof(TTLCSDEPTINDX))
										break;
								}
							}

							if (PifSeekFile(hsTtlHandleJoin, ulPositionJoinIndex[iLoop], &ulActualPosition) >= 0) {
								for (usDEPTNumber = 0; usDEPTNumber < DepthdJoin.usMaxDept; usDEPTNumber++) {
									PifReadFile(hsTtlHandleJoin, area2.TtlDepartmentsJoin + usDEPTNumber, sizeof(TTLCSDEPTINDX), &ulActualBytesRead);
									if (ulActualBytesRead < sizeof(TTLCSDEPTINDX))
										break;
								}
							}

							// Read in the department totals from the save file of the Joining Terminal.
							memset (area3.TtlDepartments, 0, sizeof(area3.TtlDepartments));
							hsTtlDeptHandle = hsTtlHandleJoin;
							for (usDEPTNumber = 0; usDEPTNumber < DepthdJoin.usMaxDept; usDEPTNumber++) {
								TTLDEPT   TtlDept = {0};
								TtlDept.uchMajorClass = CLASS_TTLDEPT;                              /* Set Major for Ttl */
								TtlDept.uchMinorClass = ClassTypes[iLoop];                          /* Set Major for Ttl */
								TtlDept.usDEPTNumber = area2.TtlDepartmentsJoin[usDEPTNumber].usDeptNo;
								if (area2.TtlDepartmentsJoin[usDEPTNumber].usDeptNo == 0)
									break;

								sReturn2 = TtlreadDept(&TtlDept, uchTmpBuff, ulPosition + 2, ulPosition, TTL_SEARCH_EQU);
								if (sReturn2 == TTL_SUCCESS) {  /* 09/26/00 */
									area3.TtlDepartments[usDEPTNumber] = TtlDept;         /* Add to SubTotal          */
								}   
							}

							// Now merge the departmental totals from the Joining Terminal with our departmental totals
							hsTtlDeptHandle = hsTtlHandleSave;
							usArea1 = usArea2 = 0;
							while (usArea1 < 250 && usArea2 < 250) {
								USHORT   usDeptTemp;
								LTOTAL   DEPTTotal;          /* Department Total */
								TTLDEPT  TtlDept = {0};

								TtlDept.uchMajorClass = CLASS_TTLDEPT;                              /* Set Major for Ttl */
								TtlDept.uchMinorClass = ClassTypes[iLoop];                          /* Set Major for Ttl */

								usDeptTemp = 0;

								if (area2.TtlDepartmentsJoin[usArea2].usDeptNo && area1.TtlDepartments[usArea1].usDeptNo) {
									if (area1.TtlDepartments[usArea1].usDeptNo > area2.TtlDepartmentsJoin[usArea2].usDeptNo) {
										// if department from Joined Terminal is lower then use that department number
										// this probably means that the Joined Terminal has used a department that the Master did not.
										usDeptTemp = area2.TtlDepartmentsJoin[usArea2].usDeptNo;
										DEPTTotal = area3.TtlDepartments[usArea2].DEPTTotal;
										usArea2++;
									}
									else if (area1.TtlDepartments[usArea1].usDeptNo < area2.TtlDepartmentsJoin[usArea2].usDeptNo) {
										// if department from Master Terminal is lower then use that department number
										// this probably means that the Master Terminal has used a department that the Joined Terminal did not.
										usDeptTemp = area1.TtlDepartments[usArea1].usDeptNo;
										memset (&DEPTTotal, 0, sizeof(DEPTTotal));
										usArea1++;
									}
									else {
										// if department from both is equal then just use the department number from Master.
										// this means that both terminals have used this department number.
										usDeptTemp = area1.TtlDepartments[usArea1].usDeptNo;
										DEPTTotal = area3.TtlDepartments[usArea2].DEPTTotal;
										usArea2++;
										usArea1++;
									}
								}
								else if (area1.TtlDepartments[usArea1].usDeptNo) {
										usDeptTemp = area1.TtlDepartments[usArea1].usDeptNo;
										memset (&DEPTTotal, 0, sizeof(DEPTTotal));
										usArea1++;
								}
								else if (area2.TtlDepartmentsJoin[usArea2].usDeptNo) {
										usDeptTemp = area2.TtlDepartmentsJoin[usArea2].usDeptNo;
										DEPTTotal = area3.TtlDepartments[usArea2].DEPTTotal;
										usArea2++;
								}
								else {
									break;
								}

								TtlDept.usDEPTNumber = usDeptTemp;
								sReturn2 = TtlreadDept(&TtlDept, uchTmpBuff, ulPosition + 2, ulPosition, TTL_SEARCH_GT);
								if (sReturn2 == TTL_SUCCESS) {  /* 09/26/00 */
									TtlAddTtlTotal_l(&(DEPTTotal), &(TtlDept.DEPTTotal));
									if (TtlWriteFile(hsTtlDeptHandle, ulPosition[0], &(DEPTTotal), sizeof(TOTAL)) != 0) {
									}
								}   
							}
						}  // for (iLoop = 0; iLoop < 4; iLoop++)
					}
				}  // if (TtlDeptHeadRead(&Depthd) == 0)
				else {
					NHPOS_ASSERT_TEXT(0, "THREADENTRY TtlTUMJoinDisconnectedSatellite(): TtlDeptHeadRead(&Depthd) == 0 FAILED.");
				}

				hsTtlDeptHandle = hsTtlHandleSave;         // Set the file handle to the standard department handle

				PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */

				PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
			}
		
			// The Service Time measurements are only collected with a Store/Recall system.
			// Other types of systems such as Post Guest Check do not collect Service Time Measurements.
			// See function CliCheckCreateFile() for the FLEX_GC_ADR type of file where it can be
			// seen that if the type is not FLEX_STORE_RECALL then delete the Service Time totals file.
			// If the Service Times are not being measured then the file handle will be less than zero.
			if (hsTtlSerTimeHandle >= 0) {
				_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlSerTimeNameJoin]);
				szTotalLocalFileName[1] += uchTerminalNo;
				hsTtlHandleJoin = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);
				if (hsTtlHandleJoin >= 0) {
					int              jLoop;
					UCHAR            ClassTypes[] = {CLASS_TTLCURDAY, CLASS_TTLCURPTD};
					ULONG            ulPosition = 0;

					PifRequestSem(usTtlSemRW);      /* Request Semaphore for Total File Read & Write */

					hsTtlHandleSave = hsTtlSerTimeHandle;
					for (iLoop = 0; iLoop < 2; iLoop++) {
						area1.TtlService.uchMajorClass = area2.TtlService.uchMajorClass = CLASS_TTLSERVICE;
						area1.TtlService.uchMinorClass = area2.TtlService.uchMinorClass = ClassTypes[iLoop];

						TtlTreadSerTime(&area1.TtlService);

						hsTtlSerTimeHandle = hsTtlHandleJoin;

						TtlTreadSerTime(&area2.TtlService);

						for (jLoop = 0; jLoop < 48; jLoop++) {
							TtlAddTtlTotal (area1.TtlService.ServiceTime[jLoop] + 0, area2.TtlService.ServiceTime[jLoop] + 0);
							TtlAddTtlTotal (area1.TtlService.ServiceTime[jLoop] + 1, area2.TtlService.ServiceTime[jLoop] + 1);
							TtlAddTtlTotal (area1.TtlService.ServiceTime[jLoop] + 2, area2.TtlService.ServiceTime[jLoop] + 2);
						}

						hsTtlSerTimeHandle = hsTtlHandleSave;
						if (TtlGetSerTimeFilePosition(area1.TtlService.uchMinorClass, &ulPosition) == 0) {
							ulPosition += TTL_STAT_SIZE;
							TtlWriteFile(hsTtlSerTimeHandle, ulPosition, area1.TtlService.ServiceTime, sizeof(area1.TtlService.ServiceTime));    /* Write Service Time Total */
						}
					}

					PifReleaseSem(usTtlSemRW);      /* Release Semaphore for Total File Read & Write */

					PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
				}
			}

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlPluDayFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			hsTtlHandleJoin = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);
			if (hsTtlHandleJoin >= 0) {
				PLUTOTAL_REC  TotalRecordJoin;
				ULONG         ulActualBytesRead = 1;
				USHORT        usStatus = 0;                  /* Buffer for Affection Status */

				while (ulActualBytesRead) {
					PifReadFile (hsTtlHandleJoin, &TotalRecordJoin, sizeof(PLUTOTAL_REC), &ulActualBytesRead);
					if (ulActualBytesRead == sizeof(PLUTOTAL_REC)) {
						TTLPLU TotalRecord;

						memset (&TotalRecord, 0, sizeof(TotalRecord));
						TotalRecord.uchMajorClass = CLASS_TTLPLU;         /* Major Class Data definition */
						TotalRecord.uchMinorClass = CLASS_TTLCURDAY;      /* Minor Class Data definition */
						_tcsncpy (TotalRecord.auchPLUNumber, TotalRecordJoin.auchPluNumber, STD_PLU_NUMBER_LEN);
						TotalRecord.uchAdjectNo = TotalRecordJoin.sAdjIdx;

						TtlTotalIncrRead (&TotalRecord);

						TotalRecord.PLUTotal.lAmount += TotalRecordJoin.lTotal;
						TotalRecord.PLUTotal.lCounter += TotalRecordJoin.lCounter;

						TtlPLU_AddTotal (CLASS_TTLCURDAY,
                             TotalRecordJoin.auchPluNumber,
                             TotalRecordJoin.sAdjIdx,
                             TotalRecordJoin.lTotal,
                             TotalRecordJoin.lCounter,
                             uchTmpBuff);
					}
				}
				PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
			}

			_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [szTtlPluPTDFileNameJoin]);
			szTotalLocalFileName[1] += uchTerminalNo;
			hsTtlHandleJoin = PifOpenFile(szTotalLocalFileName, auchOLD_FILE_READ_WRITE_FLASH);
			if (hsTtlHandleJoin >= 0) {
				PLUTOTAL_REC  TotalRecordJoin;
				ULONG         ulActualBytesRead = 1;
				USHORT        usStatus = 0;                  /* Buffer for Affection Status */

				while (ulActualBytesRead) {
					PifReadFile (hsTtlHandleJoin, &TotalRecordJoin, sizeof(PLUTOTAL_REC), &ulActualBytesRead);
					if (ulActualBytesRead == sizeof(PLUTOTAL_REC)) {
						TTLPLU TotalRecord = {0};

						TotalRecord.uchMajorClass = CLASS_TTLPLU;         /* Major Class Data definition */
						TotalRecord.uchMinorClass = CLASS_TTLCURPTD;      /* Minor Class Data definition */
						_tcsncpy (TotalRecord.auchPLUNumber, TotalRecordJoin.auchPluNumber, STD_PLU_NUMBER_LEN);
						TotalRecord.uchAdjectNo = TotalRecordJoin.sAdjIdx;

						TtlTotalIncrRead (&TotalRecord);

						TotalRecord.PLUTotal.lAmount += TotalRecordJoin.lTotal;
						TotalRecord.PLUTotal.lCounter += TotalRecordJoin.lCounter;

						TtlPLU_AddTotal (PLUTOTAL_ID_PTD_CUR,
                             TotalRecordJoin.auchPluNumber,
                             TotalRecordJoin.sAdjIdx,
                             TotalRecordJoin.lTotal,
                             TotalRecordJoin.lCounter,
                             uchTmpBuff);
					}
				}
				PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
			}

			// Now delete the temporary files that were transferred over
			// so as to clean up the disk space.
			for (iLoop = 0; TotalLocalFileNameList [iLoop]; iLoop++) {
				_tcscpy (szTotalLocalFileName, TotalLocalFileNameList [iLoop]);
				szTotalLocalFileName[1] += uchTerminalNo;
				PifDeleteFile (szTotalLocalFileName);
			}
        }

        PifReleaseSem(usTtlSemWrite);      // Release Semaphore for Update Total File Read & Write
        PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop

		PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_TOTALUM_JOINMSG_FINI);
    }
}

static TCHAR FARCONST    szTtlStoreForwardName[] = _T("TOTALSTF");   /* V3.3 */

SHORT  TtlWriteNextStoreForwardTtlMessage (TTLMESSAGEUNION  *pTotalMessage)
{
	TTLMESSAGEUNIONHEADER TotalHeader;
	SHORT                 hsTtlHandleJoin = -1;
	ULONG                 ulNextItemWrite = 0;
	SHORT                 sRetStatus = SUCCESS;
	ULONG                 ulActualBytesRead;
	ULONG                 ulActualPosition;
	SHORT                 sRetryLoop = 5;

	do {
		hsTtlHandleJoin = PifOpenFile(szTtlStoreForwardName, auchOLD_FILE_READ_WRITE_FLASH_EX);
		if (hsTtlHandleJoin < 0)
			PifSleep (50);
	} while (hsTtlHandleJoin < 0 && sRetryLoop > 0);

	if (hsTtlHandleJoin >= 0) {
		PifSeekFile(hsTtlHandleJoin, 0, &ulActualPosition);
		PifReadFile (hsTtlHandleJoin, &TotalHeader, sizeof(TTLMESSAGEUNIONHEADER), &ulActualBytesRead);

		ulNextItemWrite = TotalHeader.ulNextItemWrite;

		TotalHeader.ulNextItemWrite += sizeof(TTLMESSAGEUNION);
		if (TotalHeader.ulNextItemWrite >= TotalHeader.ulFileSize) {
			TotalHeader.ulNextItemWrite = sizeof(TTLMESSAGEUNIONHEADER);
		}

		if (TotalHeader.ulNextItemRead != TotalHeader.ulNextItemWrite) {
			PifSeekFile(hsTtlHandleJoin, ulNextItemWrite, &ulActualPosition);
			PifWriteFile (hsTtlHandleJoin, pTotalMessage, sizeof(TTLMESSAGEUNION));

			PifSeekFile(hsTtlHandleJoin, 0, &ulActualPosition);
			PifWriteFile (hsTtlHandleJoin, &TotalHeader, sizeof(TTLMESSAGEUNIONHEADER));
		}
		else {
			sRetStatus = OP_FILE_FULL;
		}

		PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
	}

	return sRetStatus;
}

SHORT  TtlReadNextStoreForwardTtlMessage (TTLMESSAGEUNION  *pTotalMessage)
{
	TTLMESSAGEUNIONHEADER TotalHeader;
	SHORT                 hsTtlHandleJoin = -1;
	ULONG                 ulNextItemWrite = 0;
	SHORT                 sRetStatus = SUCCESS;
	ULONG                 ulActualBytesRead;
	ULONG                 ulActualPosition;
	SHORT                 sRetryLoop = 5;

	do {
		hsTtlHandleJoin = PifOpenFile(szTtlStoreForwardName, auchOLD_FILE_READ_WRITE_FLASH_EX);
		if (hsTtlHandleJoin < 0)
			PifSleep (50);
	} while (hsTtlHandleJoin < 0 && sRetryLoop > 0);

	if (hsTtlHandleJoin >= 0) {
		PifSeekFile(hsTtlHandleJoin, 0, &ulActualPosition);
		PifReadFile (hsTtlHandleJoin, &TotalHeader, sizeof(TTLMESSAGEUNIONHEADER), &ulActualBytesRead);

		if (TotalHeader.ulNextItemRead != TotalHeader.ulNextItemWrite) {
			PifSeekFile(hsTtlHandleJoin, TotalHeader.ulNextItemRead, &ulActualPosition);
			PifReadFile (hsTtlHandleJoin, pTotalMessage, sizeof(TTLMESSAGEUNION), &ulActualBytesRead);
			TotalHeader.ulNextItemRead += ulActualBytesRead;

			if (TotalHeader.ulNextItemRead >= TotalHeader.ulFileSize) {
				TotalHeader.ulNextItemRead = sizeof(TTLMESSAGEUNIONHEADER);
			}
			PifSeekFile(hsTtlHandleJoin, 0, &ulActualPosition);
			PifWriteFile (hsTtlHandleJoin, &TotalHeader, sizeof(TTLMESSAGEUNIONHEADER));
		}
		else {
			sRetStatus = -1;
		}

		PifCloseFile (hsTtlHandleJoin);  hsTtlHandleJoin = -1;
	}

	return sRetStatus;
}


VOID THREADENTRY TtlTUMUpdateTotalsMsg(VOID)
{
    /*
        A loop which runs forever handling total file updates.

        The first step is to wait on the usTtlSemUpdate semaphore until some other
        thread updates the communication file and the shared global file status variable
        and then releases the semaphore so that we can then do a total update.
     */

	// if the handle to the queue has not be initialized then we are just
	// a standard Satellite terminal and so we should not be doing this
	// thread as this is Master Terminal functionality only.
	if (usTtlQueueUpdateTotalsMsg > 0xf000)
		return;

    for (;;) {
		USHORT                *pusTerminalPosition = NULL;
		TTLCS_TMPBUF_WORKING  uchTmpBuff = {0};                       // Temporary Buffer for item information
		TTLMESSAGEUNION       TotalMessage;

        pusTerminalPosition = PifReadQueue(usTtlQueueUpdateTotalsMsg);  // Read the queue Semaphore for Update Module Start
		if (pusTerminalPosition == NULL) {
			PifSleep (1000);
			continue;
		}

		if (TtlReadNextStoreForwardTtlMessage (&TotalMessage) == SUCCESS) {
			/*
				Disable power-down while we are doing a total file update.

				Request Semaphore for Update Total File Read & Write.
				This basically causes us to wait until we can get exclusive
				access to the file and the shared global variable.  We are going
				to read from the communication file and update the shared global
				variable so we want exclusive access to both.
			 */
			PifRequestSem(usTtlSemPowerDown);   // disable power-down
			PifRequestSem(usTtlSemWrite);       // Request Semaphore for Update Total File Read & Write

			if (TotalMessage.usMessageType == TTLMESSAGEUNION_STOREFORWARD &&
				TotalMessage.u.StoreForward.ulCashierId > 0 &&
				TotalMessage.u.StoreForward.usTerminalId > 0)
			{
				ULONG   uchAffectAbort = 0;             // Affection Lost
				int     iLoop = 0;

				// Initialize the working buffers for the totals calculation by
				// reading into the buffers the persistent totals information stored
				// in the various totals files.
				TtlTumReadTLWork(TotalMessage.u.StoreForward.ulCashierId, TotalMessage.u.StoreForward.usTerminalId, 0,
								 &uchAffectAbort, uchTmpBuff);

				NHPOS_ASSERT_TEXT(uchAffectAbort == 0, "uchAffectAbort error from TtlTumReadTLWork()");

				if(uchAffectAbort)
				{
					PifLog (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
					PifLog (MODULE_DATA_VALUE(MODULE_TTL_TUM_UPDATE), (USHORT)TotalMessage.u.StoreForward.ulCashierId);   /* Execute PifLog */
					PifLog (MODULE_DATA_VALUE(MODULE_TTL_TUM_UPDATE), TotalMessage.u.StoreForward.usTerminalId);   /* Execute PifLog */
					if (uchAffectAbort & TTL_UPTOTAL_WAI_ERR)
						PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_WAI_ERROR);   /* Execute PifLog */
					if (uchAffectAbort & TTL_UPTOTAL_PLU_ERR)
						PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_PLU_ERROR);   /* Execute PifLog */
					if (uchAffectAbort & TTL_UPTOTAL_DEP_ERR)
						PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_DEP_ERROR);   /* Execute PifLog */
					if (uchAffectAbort & TTL_UPTOTAL_CAS_ERR)
						PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CAS_ERROR);   /* Execute PifLog */
					if (uchAffectAbort & TTL_UPTOTAL_CPN_ERR)
						PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CPN_ERROR);   /* Execute PifLog */
					PifLog (MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);   /* Execute PifLog */
//					TtlAbort (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
				}
				else {
					//Set the Total Update Flags
					TtlUpTotalFlag |= TTL_UPTOTAL_FIN;   /* Set Financial Updated Flag */
					TtlUpTotalFlag |= TTL_UPTOTAL_CAS;   // Set financial flag

					// update the cashier totals for both day and the period to date.
					// update the cluster financial totals for both day and period to date.
					// update the individual terminal financials.
					// Put the various kinds of Non-Void totals into bonus totals 6, 7, and 8.
					for (iLoop = TTLMESSAGEUNION_TOTALTYPE_APPROVED; iLoop <= TTLMESSAGEUNION_TOTALTYPE_DELETE; iLoop++) {
						TtlAddTtlTotal(TtlCasDayWork.Bonus + 5 + iLoop, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);    /* Bonus Total 1 to 8 */

						TtlAddTtlTotal(TtlCasPTDWork.Bonus + 5 + iLoop, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);    /* Bonus Total 1 to 8 */

						TtlAddTtlTotal(TtlFinDayWork.Bonus + 5 + iLoop, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);                /* Bonus Total 1 to 8 */
						TtlAddTtlTotal(TtlFinPTDWork.Bonus + 5 + iLoop, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);                /* Bonus Total 1 to 8 */
						TtlAddTtlTotal(TtlIndFinWork.Bonus + 5 + iLoop, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);                /* Bonus Total 1 to 8 */
					}

					// Put the various kinds of Void totals into bonus totals 3, 4, and 5.
					for (iLoop = TTLMESSAGEUNION_TOTALTYPE_VOID_APRVD; iLoop <= TTLMESSAGEUNION_TOTALTYPE_VOID_DELETE; iLoop++) {
						int iLoopShift = iLoop - TTLMESSAGEUNION_TOTALTYPE_VOID_APRVD;
						TtlAddTtlTotal(TtlCasDayWork.Bonus + 2 + iLoopShift, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);    /* Bonus Total 1 to 8 */

						TtlAddTtlTotal(TtlCasPTDWork.Bonus + 2 + iLoopShift, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);    /* Bonus Total 1 to 8 */

						TtlAddTtlTotal(TtlFinDayWork.Bonus + 2 + iLoopShift, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);                /* Bonus Total 1 to 8 */
						TtlAddTtlTotal(TtlFinPTDWork.Bonus + 2 + iLoopShift, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);                /* Bonus Total 1 to 8 */
						TtlAddTtlTotal(TtlIndFinWork.Bonus + 2 + iLoopShift, TotalMessage.u.StoreForward.StoreForwardTtl + iLoop);                /* Bonus Total 1 to 8 */
					}

					TtlUpFinNetTotal(&TtlFinDayWork, (TotalMessage.u.StoreForward.StoreForwardTtl[TTLMESSAGEUNION_TOTALTYPE_DECLINE].lAmount * (-1)));    /* Affect Net Total, R3.1 */    

					// Update the total files with the updated information from the
					// totals message from Store and Forward.
					TtlTumFileUpdate(TotalMessage.u.StoreForward.ulCashierId, TotalMessage.u.StoreForward.usTerminalId, &uchAffectAbort);

					NHPOS_ASSERT_TEXT(uchAffectAbort == 0, "uchAffectAbort error from TtlTumFileUpdate()");

					if(uchAffectAbort)
					{
						PifLog (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
						PifLog (MODULE_DATA_VALUE(MODULE_TTL_TUM_UPDATE), (USHORT)TotalMessage.u.StoreForward.ulCashierId);   /* Execute PifLog */
						PifLog (MODULE_DATA_VALUE(MODULE_TTL_TUM_UPDATE), TotalMessage.u.StoreForward.usTerminalId);   /* Execute PifLog */
						if (uchAffectAbort & TTL_UPTOTAL_WAI_ERR)
							PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_WAI_ERROR);   /* Execute PifLog */
						if (uchAffectAbort & TTL_UPTOTAL_PLU_ERR)
							PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_PLU_ERROR);   /* Execute PifLog */
						if (uchAffectAbort & TTL_UPTOTAL_DEP_ERR)
							PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_DEP_ERROR);   /* Execute PifLog */
						if (uchAffectAbort & TTL_UPTOTAL_CAS_ERR)
							PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CAS_ERROR);   /* Execute PifLog */
						if (uchAffectAbort & TTL_UPTOTAL_CPN_ERR)
							PifLog (MODULE_TTL_TUM_UPDATE, LOG_EVENT_DB_UPD_CPN_ERROR);   /* Execute PifLog */
						PifLog (MODULE_LINE_NO(MODULE_TTL_TUM_UPDATE), (USHORT)__LINE__);   /* Execute PifLog */
//						TtlAbort (MODULE_TTL_TUM_UPDATE, TTL_AFFECTION_ERR);   /* Execute PifLog */
					}

					TtlUpTotalFlag &= ~TTL_UPTOTAL_FIN;   /* Unset Financial Updated Flag */
					TtlUpTotalFlag &= ~TTL_UPTOTAL_CAS;   // Unset financial flag
				}
			}

			PifReleaseSem(usTtlSemWrite);      // Release Semaphore for Update Total File Read & Write
			PifReleaseSem(usTtlSemPowerDown);  // enable power-down and jump to top of loop
		}
    }
}

/* ===== End of File (TotalTUM.C) ===== */
