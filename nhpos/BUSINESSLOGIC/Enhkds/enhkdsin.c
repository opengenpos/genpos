/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1996           **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Enhanced Kitchen Device System send/receive thread
* Category    : Enhanced Kitchen Device System Manager, NCR 2170 US HOSP Appl
* Program Name: EnhKdsIn.C
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               KdsSendProcess()
*               KdsRecvProcess()
*                -KdsNetOpen()
*                -KdsNetSend()
*                -KdsNetReceive()
*                -KdsNetClose()
*                -KdsErrorDisp()
*
*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Dec-21-99 : 01.00.00 : M.Ozawa      : initial
* Mar-19-15 : 02.02.01 : R.Chambers   : source cleanup and logs added.
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

/**
;=============================================================================
;+                  I N C L U D E     F I L E s                              +
;=============================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "rfl.h"
#include "enhkds.h"
#include "enhkdsin.h"
#include "display.h"
#include "appllog.h"

VOID DispRegOpDscrCntr(REGDISPMSG *pData, UCHAR uchDscSave);
/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/

static USHORT	fKDSMessageShown = 0;
static USHORT	fKdsBackup = 0;

/* -- semaphore handle -- */
extern USHORT usKdsSemRWSpool;         /* semaphore handle for access to spool buffer */
extern USHORT usKdsSemStubFunc;        /* semaphore handle for stub function */
extern USHORT usKdsSemSendSpool;         /* semaphore handle for access to send buffer */

/* -- spool buffer & pointer for access -- */
extern KDSIPADDR    aKdsIPAddr[KDS_NUM_CRT];
extern USHORT	fKDSBackupDown[KDS_NUM_CRT];
extern USHORT   KDSBackupInfo[KDS_NUM_CRT];
extern KDSRING  aKdsRing[KDS_NUM_CRT][KDS_SPOOL_SIZE];
extern KDSRING  KdsSendRing[KDS_SPOOL_SIZE];
extern USHORT   ausKdsWriteP[KDS_NUM_CRT];

extern UCHAR    fbKdsControl;            /* control flag */
extern USHORT   fKdsService;
/* extern ULONG    ulKdsMsgSeq; */

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/

/*
*=============================================================================
** Format  : VOID THREADENTRY KdsSendProcess(USHORT usData)
*
*    Input : usData
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is sending spooled data to PC through Ethernet
*=============================================================================
*/
VOID THREADENTRY  KdsSendProcess(USHORT usSendPointer)
{
    KDSINF *pData = NULL;
    USHORT usSendP, usWriteP;
    SHORT  sReturn;
	TCHAR  versionNumber[40] = {0};
	CHAR   aucKdsVersionNumber[9];

	/* JMASON -- Get File version number*/
	PifGetVersion(versionNumber);	
	{
		CHAR aucVersionNumber[40];
		int j;

		// Transform the TCHAR array of characters into a CHAR array
		// of characters to make the following manipulation easier.
		// This is just a straight assign of array elements.
		for (j = 0; j < sizeof(aucVersionNumber); j++) {
			aucVersionNumber[j] = (CHAR)versionNumber[j];
		}

		// Initialize the version number space to all zero digits.
		// the space will look like 000000000 with Major, Minor, and Issue version
		// numbers being 2 digits and the build being 3 digits.
		memset (aucKdsVersionNumber, '0', sizeof(aucKdsVersionNumber));

		// Now we will parse the version string.  The version string should
		// look something like "02.00.05.23" or something similar.  There is
		// a trailing line feed character on the end, 0xa or octal 12.

		// Start the string tokeninzing with our transformed version number.
		// In all cases we are looking for either a dot (.) or line feed or
		// end of string character.
		do {  // parse the version number and if there is an error then bail out.
			CHAR  *pDelim = ".\012";
			CHAR  *pNextToken = 0;
			CHAR  *pChar = strtok_s (aucVersionNumber, ".\012", &pNextToken);
			if (pChar) {
				int i = strlen(pChar);
				if (i > 0) {
					aucKdsVersionNumber[1] = pChar[i-1];
					i--;
					if (i > 0)
						aucKdsVersionNumber[0] = pChar[i-1];
				}
			} else {
				break;
			}

			pChar = strtok_s (NULL, pDelim, &pNextToken);
			if (pChar) {
				int i = strlen(pChar);
				if (i > 0) {
					aucKdsVersionNumber[3] = pChar[i-1];
					i--;
					if (i > 0)
						aucKdsVersionNumber[2] = pChar[i-1];
				}
			} else {
				break;
			}

			pChar = strtok_s (NULL, pDelim, &pNextToken);
			if (pChar) {
				int i = strlen(pChar);
				if (i > 0) {
					aucKdsVersionNumber[5] = pChar[i-1];
					i--;
					if (i > 0)
						aucKdsVersionNumber[4] = pChar[i-1];
				}
			} else {
				break;
			}

			pChar = strtok_s (NULL, pDelim, &pNextToken);
			if (pChar) {
				int i = strlen(pChar);
				if (i > 0) {
					aucKdsVersionNumber[8] = pChar[i-1];
					i--;
					if (i > 0) {
						aucKdsVersionNumber[7] = pChar[i-1];
						i--;
						if (i > 0)
							aucKdsVersionNumber[6] = pChar[i-1];
					}
				}
			} else {
				break;
			}
		} while (0);
	}

	NHPOS_ASSERT(sizeof(aucKdsVersionNumber) == (&(pData->uchETX) - &(pData->auschMajorVersionNo[0])));

    usSendP = usSendPointer;	/* set ring buffer pointer, 12/21/01 */
	for (;;) {                                  /* forever */
		int jCrt;

        PifRequestSem(usKdsSemSendSpool);             /* request sem. for access to send process */
        if (fKdsService == FALSE) break; /* termination */
        PifRequestSem(usKdsSemRWSpool);             /* request sem. for access to spool buffer */

        pData = &KdsSendRing[usSendP].KdsInf;

		memcpy (&(pData->auschMajorVersionNo[0]), aucKdsVersionNumber, sizeof(aucKdsVersionNumber));

        for (jCrt = 0; jCrt < KDS_NUM_CRT; jCrt++) {
            sReturn = KdsSend((USHORT)(jCrt+1), pData);
            if (sReturn < 0) {  /* power down error */
//                PifReleaseSem(usKdsSemRWSpool);         /* release sem. for access buffer */
                break;
            }

            if (ausKdsWriteP[jCrt] >= KDS_SPOOL_SIZE) {
				// wrap around to the beginning again.
                ausKdsWriteP[jCrt] = 0;
            }
            usWriteP = ausKdsWriteP[jCrt];

			/* correct sequence no. setting, 12/21/01 */
            aKdsRing[jCrt][usWriteP].ulSeqNo = KdsSendRing[usSendP].ulSeqNo;
            /* aKdsRing[i][usWriteP].ulSeqNo = ulKdsMsgSeq; */
            memcpy (&aKdsRing[jCrt][usWriteP].KdsInf, pData, sizeof(KDSINF));  /* write data header to spool buffer */

            usWriteP++;  /* increment write pointer */
            ausKdsWriteP[jCrt] = usWriteP;
        }

        usSendP++;
        if (usSendP >= KDS_SPOOL_SIZE) {
            usSendP = 0;
        }

        PifReleaseSem(usKdsSemRWSpool);         /* release sem. for access buffer */
        if (fKdsService == FALSE) break; /* termination */
        if (sReturn < 0) {
            PifSleep(1000); /* power down error */
        }
    }

    PifEndThread();
}

/*
*=============================================================================
** Format  : SHORT KdsSend(USHORT usKds, KDSINF *pData)
*
*    Input : usData
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is sending spooled data to PC through Ethernet
*=============================================================================
*/
SHORT  KdsSend(USHORT usKds, KDSINF *pData)
{
    KDSINF KdsInf;
	SHORT sRet;

    if (aKdsIPAddr[usKds-1].usPortNo > 0) {
		fKDSBackupDown[usKds-1] = FALSE;

        memcpy(&KdsInf, pData, sizeof(KDSINF));

        KdsItoA(aKdsIPAddr[usKds-1].usPortNo, KdsInf.auchPort, KDS_PORT_LEN);
        /* KdsItoA(usKds, KdsInf.aszRouting, KDS_ROUTE_LEN); */ /* by FVT */
        KdsInf.uchCRC = KdsCalcCRC((UCHAR *)&KdsInf, sizeof(KDSINF)-1);

		sRet = KdsNetSend(usKds, &KdsInf);
        if ( sRet != PIF_OK) {
			KdsErrorDisp();
        } else {
			PifSleep(100);

			if(!fKDSBackupDown[usKds-1])
			{
				if((KDSBackupInfo[usKds-1] >= 0) && (KDSBackupInfo[usKds-1] <= 9))
				{
					if(!fKDSBackupDown[KDSBackupInfo[usKds-1]])
					{
						/*The following ADJ_CNTRL was not being used, so we use this one
						to display the KDS message on the display*/
						flDispRegDescrControl &= ~ADJ_CNTRL;
						flDispRegKeepControl &= ~ADJ_CNTRL;

						//We set the class so that we can send the dummy data,
						//and trigger the discriptor to show KDS
						// The following update display descriptors request did not exist so as part
						// of eliminating use of regDispMsg with DispWrite(&regDispMsg); we have added
						// this line while commenting it out. It would seem logical that this statement
						// to refresh the displayed descriptors is needed but it wasn't there.
						//    Richard Chambers, Jul-17-2020
//						RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
					}
				} else
				{
					/*The following ADJ_CNTRL was not being used, so we use this one
					to display the KDS message on the display*/
					flDispRegDescrControl &= ~ADJ_CNTRL;
					flDispRegKeepControl &= ~ADJ_CNTRL;

					//We set the class so that we can send the dummy data,
					//and trigger the discriptor to show KDS
					RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);

					fKDSBackupDown[usKds -1] = TRUE;
				}

			}

            fbKdsControl &= ~KDS_ALREADY_DISP;           /* set disp error flag */
        }
    }
    return (KDS_PERFORM);
}

/*
*=============================================================================
** Format  : VOID THREADENTRY KdsRecvProcess(USHORT usData)
*
*    Input : usData
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is receiving re-send from PC through Ethernet
*=============================================================================
*/
VOID THREADENTRY  KdsRecvProcess(USHORT usData)
{
    USHORT     usKds = usData; /* save KDS No to own thread area */

    for (;;) {                                  /* forever */
		ULONG      ulSeqNo;
		KDSRESINF  KdsResInf;
		SHORT      sReturn;
		USHORT     fsFound;
		USHORT     usRecvKds, i;
		UCHAR      uchCRC;
        sReturn = KdsNetReceive(usKds, &KdsResInf, &usRecvKds);
		if((sReturn == PIF_OK))
		{
			continue;
		}
        if (sReturn < 0) {  /* power down error */
            KdsNetClose(usKds);
            if (fKdsService == FALSE) break; /* termination */

            do {
                PifSleep(1000);
            } while (KdsNetOpen(usKds) == FALSE);   /* reopen */

			flDispRegDescrControl &= ~ADJ_CNTRL;
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
			continue;
        }

        if (sReturn != sizeof(KdsResInf)) {
			/* if received size is wrong, send error message, 12/21/01 */
			KDSINF     KdsInf = { 0 };

			KdsInf.uchSTX = (UCHAR)KDS_STX;
            KdsInf.uchMsgType = '1';
		    KdsInf.auchMsgIndType[0] = '1';
			KdsInf.auchMsgIndType[1] = '9';
			KdsInf.auchMsgIndType[2] = '9';
			KdsInf.auchMsgIndType[3] = '7';	/* received size is wrong *.

            /* copy sub message information of request message */
            /* memcpy(KdsInf.auchSubMsgInfo, KdsResInf.auchSubMsgInfo,KDS_SMI_LEN); */
            /* memcpy(&KdsInf.auchMsgSeqNo[0], &KdsResInf.auchMsgSeqNo[0], KDS_MSN_LEN); */
		    KdsItoA(KdsGetTerminalNo(), &KdsInf.auchTerminal[0], KDS_TERM_LEN);
            KdsItoA(aKdsIPAddr[usRecvKds-1].usPortNo, KdsInf.auchPort, KDS_PORT_LEN);
		    memset(&KdsInf.aszLineData[0], 0, sizeof(KdsInf.aszLineData));

			KdsInf.uchETX = (UCHAR)KDS_ETX;
            KdsInf.uchCRC = KdsCalcCRC((UCHAR *)&KdsInf, sizeof(KDSINF)-1);

			KdsNetSend(usRecvKds, &KdsInf);
            continue;
        }

        uchCRC = KdsCalcCRC((UCHAR *)&KdsResInf, sizeof(KDSRESINF)-1);
        if (uchCRC != KdsResInf.uchCRC)  {
			/* if CRC is wrong, send error message, 12/21/01 */
			//RPH WIDE need to properly initialize KDSINF
			KDSINF     KdsInf = { 0 };

			KdsInf.uchSTX = (UCHAR)KDS_STX;
            KdsInf.uchMsgType = _T('1');
		    KdsInf.auchMsgIndType[0] = _T('1');
			KdsInf.auchMsgIndType[1] = _T('9');
			KdsInf.auchMsgIndType[2] = _T('9');
			KdsInf.auchMsgIndType[3] = _T('8');	/* CRC is wrong */

            /* copy sub message information of request message */
            memcpy(KdsInf.auchSubMsgInfo, KdsResInf.auchSubMsgInfo,KDS_SMI_LEN);
            memcpy(&KdsInf.auchMsgSeqNo[0], &KdsResInf.auchMsgSeqNo[0], KDS_MSN_LEN);
		    KdsItoA(KdsGetTerminalNo(), &KdsInf.auchTerminal[0], KDS_TERM_LEN);
            KdsItoA(aKdsIPAddr[usRecvKds-1].usPortNo, KdsInf.auchPort, KDS_PORT_LEN);
		    memset(&KdsInf.aszLineData[0], 0, sizeof(KdsInf.aszLineData));

			KdsInf.uchETX = (UCHAR)KDS_ETX;
            KdsInf.uchCRC = KdsCalcCRC((UCHAR *)&KdsInf, sizeof(KDSINF)-1);

			KdsNetSend(usRecvKds, &KdsInf);
			continue;
		}

        /* read from ring buffer */
        KdsAtoL(&ulSeqNo, KdsResInf.auchMsgSeqNo, KDS_MSN_LEN);

        PifRequestSem(usKdsSemRWSpool);             /* request sem. for access to spool buffer */

		fsFound = FALSE;	/* */
        for (i = 0; i < KDS_SPOOL_SIZE; i++) {
            /* re-send KDS data to receivecd IP address */
            if (ulSeqNo && (ulSeqNo == aKdsRing[usRecvKds-1][i].ulSeqNo)) {
				KDSINF     KdsInf = aKdsRing[usRecvKds - 1][i].KdsInf;

                KdsInf.uchMsgType = _T('1');
                /* copy sub message information of request message */
                memcpy(KdsInf.auchSubMsgInfo, KdsResInf.auchSubMsgInfo,KDS_SMI_LEN);
				/* do not mask the previous sequence no. by FVT comment, 11/20/01 */
                /* memset(&KdsInf.auchMsgSeqNo[0], '0', KDS_MSN_LEN); */
                KdsItoA(aKdsIPAddr[usRecvKds-1].usPortNo, KdsInf.auchPort, KDS_PORT_LEN);
                /* KdsItoA(usRecvKds, KdsInf.aszRouting, KDS_ROUTE_LEN); */ /* by FVT */

                KdsInf.uchCRC = KdsCalcCRC((UCHAR *)&KdsInf, sizeof(KDSINF)-1);

                KdsNetSend(usRecvKds, &KdsInf);

				fsFound = TRUE;	/* */
				break;
            }
        }

		if (fsFound == FALSE) {
			/* if resquested message is not found, send not found message, 12/21/01 */
			//RPH WIDE need to properly initialize KDSINF
			KDSINF     KdsInf = { 0 };

			KdsInf.uchSTX = (UCHAR)KDS_STX;
            KdsInf.uchMsgType = _T('1');
		    KdsInf.auchMsgIndType[0] = _T('1');
			KdsInf.auchMsgIndType[1] = _T('9');
			KdsInf.auchMsgIndType[2] = _T('9');
			KdsInf.auchMsgIndType[3] = _T('9');	/* sequence no. is not found */

            /* copy sub message information of request message */
            memcpy(KdsInf.auchSubMsgInfo, KdsResInf.auchSubMsgInfo,KDS_SMI_LEN);
            memcpy(&KdsInf.auchMsgSeqNo[0], &KdsResInf.auchMsgSeqNo[0], KDS_MSN_LEN);
		    KdsItoA(KdsGetTerminalNo(), &KdsInf.auchTerminal[0], KDS_TERM_LEN);
            KdsItoA(aKdsIPAddr[usRecvKds-1].usPortNo, KdsInf.auchPort, KDS_PORT_LEN);
		    memset(&KdsInf.aszLineData[0], 0, sizeof(KdsInf.aszLineData));

			KdsInf.uchETX = (UCHAR)KDS_ETX;
            KdsInf.uchCRC = KdsCalcCRC((UCHAR *)&KdsInf, sizeof(KDSINF)-1);

			KdsNetSend(usRecvKds, &KdsInf);
		}

        PifReleaseSem(usKdsSemRWSpool);         /* release sem. for access buffer */
    }

    PifEndThread();
}

/*
*=============================================================================
** Format  : VOID   KdsErrorDisp(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
VOID   KdsErrorDisp(VOID)
{
    if ( fbKdsControl & KDS_ALREADY_DISP ) {    /* already display error ? */
        return ;
    }

    UieErrorMsg(LDT_DFLERR_ADR, UIE_WITHOUT);

    fbKdsControl |= KDS_ALREADY_DISP;           /* set disp error flag */

}

/*
*=============================================================================
** Format  : VOID   KdsNetOpen(USHORT *pfsHostOpen)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is open port and set status.
*=============================================================================
*/
SHORT KdsNetOpen(USHORT usKds)
{
    SYSCONFIG CONST *SysCon = PifSysConfig();
    XGHEADER        IPData;
    USHORT          i, usKDSNo;

	//SR 236 JHHJ
	switch(usKds)
	{
	case (KDS_IP1_ADR):
		usKDSNo = 0;
		break;
	case (KDS_IP2_ADR):
		usKDSNo = 1;
		break;
	case KDS_IP3_ADR:
		usKDSNo = 2;
		break;
	case KDS_IP4_ADR:
		usKDSNo = 3;
		break;
	case KDS_IP5_ADR:
		usKDSNo = 4;
		break;
	case KDS_IP6_ADR:
		usKDSNo = 5;
		break;
	case KDS_IP7_ADR:
		usKDSNo = 6;
		break;
	case KDS_IP8_ADR:
		usKDSNo = 7;
		break;
	default:
		usKDSNo = usKds;
		NHPOS_ASSERT_TEXT(!"==ERROR", "==ERROR: KdsNetOpen invalid KDS number.");
        return FALSE;
		break;
	}
	//END SR 236

    memcpy(IPData.auchFaddr, aKdsIPAddr[usKDSNo].ausIPAddr, sizeof(aKdsIPAddr[usKDSNo].ausIPAddr)); //SR14 ESMITH

    IPData.usLport = aKdsIPAddr[usKDSNo].usPortNo;
    IPData.usFport = aKdsIPAddr[usKDSNo].usFPortNo; //SR14 ESMITH, RJC allow ROD/KDS to run on terminal
	if (IPData.usLport == 0 || IPData.usFport == 0) {
		char  xBuff[128];
		sprintf (xBuff, "==PROVISION: KDS number %d data invalid Lport %d, Fport %d.", usKds, IPData.usLport, IPData.usFport);
		NHPOS_NONASSERT_NOTE("==PROVISION", xBuff);
        return FALSE;
	}

    for (i = usKDSNo; i > 0; i--) {
        if (IPData.usLport == aKdsIPAddr[i-1].usPortNo) {
            /* UDP/IP port has already opened */
			fKdsBackup = TRUE;
			KDSBackupInfo[i] = (i-1);

            aKdsIPAddr[usKDSNo].sHandle = aKdsIPAddr[i-1].sHandle;
            return FALSE;
        }
    }
    aKdsIPAddr[usKDSNo].sHandle = PifNetOpenEx(&IPData, PIF_NET_DIRECTPORTMODE);
    if ((SHORT)aKdsIPAddr[usKDSNo].sHandle >= 0) {
		char  xBuff[128];
		sprintf (xBuff, "==STATE: KDS number %d Lport %d, Fport %d opened.", usKDSNo, IPData.usLport, IPData.usFport);
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
        return TRUE;
    } else {
		char  xBuff[128];
		sprintf (xBuff, "==ERROR: KDS number %d Lport %d, Fport %d open failed %d.", usKDSNo, IPData.usLport, IPData.usFport, aKdsIPAddr[usKDSNo].sHandle);
		NHPOS_ASSERT_TEXT(!"==ERROR", xBuff);
		if((!fKdsBackup) && (aKdsIPAddr[usKDSNo].sHandle != -20))
		{
			KdsErrorDisp();
		}
        return FALSE;
    }
}

/*
*=============================================================================
** Format  : VOID   KdsNetSend(USHORT usKds, KDSINF *pKdsInf)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
SHORT KdsNetSend(USHORT usKds, KDSINF *pKdsInf)
{
    XGRAMEX   xGram;
    SHORT     sHandle;
	SHORT     sRetStatus;

    sHandle = aKdsIPAddr[usKds-1].sHandle;
    if (sHandle < 0) { /* power down */
        return KDS_FAIL;
    }

    xGram.auchFaddr[0] = (UCHAR)aKdsIPAddr[usKds-1].ausIPAddr[0];
    xGram.auchFaddr[1] = (UCHAR)aKdsIPAddr[usKds-1].ausIPAddr[1];
    xGram.auchFaddr[2] = (UCHAR)aKdsIPAddr[usKds-1].ausIPAddr[2];
    xGram.auchFaddr[3] = (UCHAR)aKdsIPAddr[usKds-1].ausIPAddr[3];
    xGram.usLport = aKdsIPAddr[usKds-1].usPortNo;
    xGram.usFport = aKdsIPAddr[usKds-1].usFPortNo; //SR14 ESMITH, RJC allow ROD/KDS to run on terminal
    memcpy(xGram.auchData, pKdsInf, sizeof(KDSINF));

	sRetStatus = PifNetSend(sHandle, &xGram, sizeof(XGHEADER)+sizeof(KDSINF));
	return sRetStatus;
}

/*
*=============================================================================
** Format  : VOID   KdsNetReceive(USHORT usKds, KDSINF *pKdsInf)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
SHORT KdsNetReceive(USHORT usKds, KDSRESINF *pKdsResInf, USHORT *pusKds)
/* SHORT KdsNetReceive(USHORT usKds, KDSINF *pKdsInf, USHORT *pusKds) */
{
    SHORT      sHandle;
    SHORT      sReturn;
    XGRAMEX    xGram;

    sHandle = aKdsIPAddr[usKds-1].sHandle;
    sReturn = PifNetReceive(sHandle, &xGram, sizeof(XGHEADER)+sizeof(KDSRESINF));

	if (sReturn < 0) {
		PifLog (MODULE_KPS, LOG_EVENT_KPS_KDS_NET_RECEIVE);
		PifLog (MODULE_ERROR_NO(MODULE_KPS), (USHORT)abs(sReturn));
	}

    if (sReturn == (sizeof(XGHEADER) + sizeof(KDSRESINF))) {
		USHORT     i;
        memcpy(pKdsResInf, &xGram.auchData, sizeof(KDSRESINF));
        for (i = 0; i < KDS_NUM_CRT; i++) {
            /* check source IP address */
            if ((xGram.auchFaddr[0] == (UCHAR)aKdsIPAddr[i].ausIPAddr[0]) &&
                (xGram.auchFaddr[1] == (UCHAR)aKdsIPAddr[i].ausIPAddr[1]) &&
                (xGram.auchFaddr[2] == (UCHAR)aKdsIPAddr[i].ausIPAddr[2]) &&
                (xGram.auchFaddr[3] == (UCHAR)aKdsIPAddr[i].ausIPAddr[3])) {

                if (xGram.usLport == aKdsIPAddr[i].usPortNo) {
                    /* set source CRT No */
                    *pusKds = i+1;
                    break;
                }
            }
        }
        if (i == KDS_NUM_CRT) {
            *pusKds = usKds;
        }

        sReturn -= sizeof(XGHEADER);
	//Begin SR236 Fix
    }
	else if ( (sReturn == PIF_ERROR_NET_ERRORS))
	{
		USHORT     i;

        for (i = 0; i < KDS_NUM_CRT; i++) {
            /* check source IP address */
            if ((xGram.auchFaddr[0] == (UCHAR)aKdsIPAddr[i].ausIPAddr[0]) &&
                (xGram.auchFaddr[1] == (UCHAR)aKdsIPAddr[i].ausIPAddr[1]) &&
                (xGram.auchFaddr[2] == (UCHAR)aKdsIPAddr[i].ausIPAddr[2]) &&
                (xGram.auchFaddr[3] == (UCHAR)aKdsIPAddr[i].ausIPAddr[3])) {
                    /* set source CRT No */
                    *pusKds = i+1;
                    break;
            }
        }
        if (i == KDS_NUM_CRT) {
            *pusKds = usKds;
        }

		/*The following ADJ_CNTRL was not being used, so we use this one
		to display the KDS message on the display*/
		flDispRegDescrControl |= ADJ_CNTRL;
		flDispRegKeepControl |= ADJ_CNTRL;

		//We set the class so that we can send the dummy data,
		//and trigger the discriptor to show KDS
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);

		fKDSBackupDown[*pusKds - 1] = TRUE;

		/*If this flag is false, we havent shown the user the Pop up error
		  and we need to do this only once, if we dont show do this,
		  the error box will show up evertime it receives the error message*/
		if(!fKDSMessageShown)
		{
			UieErrorMsg(LDT_KDS_BACKUP_DOWN, UIE_WITHOUT);
			fKDSMessageShown = TRUE;
		}

		return PIF_OK;
	}
	//End SR236 Fix
    return sReturn;
}

/*
*=============================================================================
** Format  : VOID   KdsNetClose(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis:
*=============================================================================
*/
VOID KdsNetClose(USHORT usKds)
{	
	if (aKdsIPAddr[usKds-1].sHandle >= 0) {
		PifNetClose(aKdsIPAddr[usKds-1].sHandle);
	}
	
	aKdsIPAddr[usKds-1].sHandle = -1;
}

/* ---------- End of EnhKdsIn.c ---------- */
