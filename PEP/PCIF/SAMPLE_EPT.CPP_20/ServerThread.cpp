
#include "StdAfx.h"
#include "ServerThread.h"

ServerThread::ServerThread (USHORT usLportL /* = CLI_PORT_CPM */, USHORT usFportL /* = ANYPORT */)
{
    fchNetStatus = 0;
    usLport = usLportL;
    usFport = usFportL;
	usNetHandle = 0;
	usNetTimer = 5000;
	ContinueComms = 1;
}

ServerThread::~ServerThread ()
{
	NetClose ();
}

void ServerThread::NetOpen(USHORT usLportL /* = CLI_PORT_CPM */, USHORT usFportL /* = ANYPORT */)
{
    XGHEADER        IPdata;

    fchNetStatus = 0;
	usNetHandle = 0;

    memset(&IPdata, 0, sizeof(XGHEADER));
	if (usLportL) {
		usLport = usLportL;
		usFport = usFportL;
	}

	IPdata.usLport = usLport;
	IPdata.usFport = usFport;

    usNetHandle = PifNetOpen(&IPdata);

	ASSERT(usNetHandle >= 0);

    if (0 <= usNetHandle) {
        fchNetStatus |= CLI_NET_OPEN;
        PifNetControl(usNetHandle, 
                      PIF_NET_SET_MODE, 
                      PIF_NET_NMODE + PIF_NET_TMODE + PIF_NET_SRVRMODE);
    }
}

SHORT ServerThread::NetSend(void *pvSndBuffer, USHORT usDataLen)
{
    SHORT   sError, sRetry=5;

    fchNetStatus |= CLI_NET_SEND;

    do {
		if (!ContinueComms)
			break;

		if (5 > sRetry) {
            PifSleep(100);
        }
        sError = PifNetSend(usNetHandle, pvSndBuffer, usDataLen);

    } while (( (PIF_ERROR_NET_WRITE == sError)   && (--sRetry > 0) ) ||
             ( (PIF_ERROR_NET_OFFLINE == sError) && (--sRetry > 0) ) );

    fchNetStatus &= ~CLI_NET_SEND;

	return sError;
}


/*
*===========================================================================
** Synopsis:    VOID    NetReceive(void *pvRecvBuffer, USHORT ulBufferLen);
*
** Return:      none.
*
** Description:
*   This function performs a receive on the network connection waiting for
*	a Charge Post request to come in for processing.  This function's sole
*	duty is to accept Charge Post requests and provide the incoming packet
*	to the caller.
*===========================================================================
*/
SHORT ServerThread::NetReceive(void *pvRecvBuffer, USHORT ulBufferLen)
{
    SHORT       sError;
    USHORT      usPrevErrorCo;

    PifNetControl(usNetHandle, PIF_NET_SET_TIME, usNetTimer);

    usPrevErrorCo = 0;
    do {
        fchNetStatus |= CLI_NET_RECEIVE;
        sError = PifNetReceive(usNetHandle, pvRecvBuffer, ulBufferLen);
        fchNetStatus &= ~CLI_NET_RECEIVE;

		if (!ContinueComms)
			break;

        if (0 > sError) {
            if (PIF_ERROR_NET_POWER_FAILURE == sError) {
                ;  // if net power failure, then we error out return PIF status
            } else if (PIF_ERROR_NET_TIMEOUT == sError) {
                ;  // if net timeout, then we error out return PIF status
            } else {
				// otherwise we retry for the specified number of times
                usPrevErrorCo ++;
                if (usPrevErrorCo > 3) {
                    sError = PIF_ERROR_NET_TIMEOUT;
                } else {
                    sError = PIF_ERROR_NET_ILLEGAL;
					PifSleep (500);
                }
            }
        }
    } while (PIF_ERROR_NET_ILLEGAL == sError);
	return sError;
}

SHORT ServerThread::NetReceiveBuf ()
{
	return NetReceive ((void *)&ServerRcvBuf, sizeof(ServerRcvBuf));
}

SHORT ServerThread::NetSendBuf ()
{
	return NetSend ((void *)&ServerSndBuf, ServerSndBufSize);
}

/*
*===========================================================================
** Synopsis:    VOID    NetClose(VOID);
*
** Return:      none.
*
** Description:
*   This function closes the network interface down so that ServerThread is
*	no longer handling Charge Post requests.
*===========================================================================
*/
void ServerThread::NetClose(void)
{
    if (fchNetStatus & CLI_NET_OPEN) {    /* V3.3 */
        PifNetClose(usNetHandle);
        fchNetStatus &= ~CLI_NET_OPEN;
    }                                               /* V3.3 */
}

/*

	The data in this struct is used in several ways.
		if ulMaxTotal is greater than 5000, usErrorCode is a notification not an error
		if ulMaxTotal is less than 5000, if usErrorCode is non-zero, its indicates an error condition
		usCounter is used for counting multiple messages such as when dealing with Guest Line.
 */
struct _tagDataBase {
	UCHAR   auchRoomNumber[5];                  /* 54: room number          */
    UCHAR   auchGuestID[2];                     /* 59: guest ID             */
	ULONG   ulMaxTotal;                         // Maximum the guest can charge
	USHORT  usErrorCode;                        // error code to respond with if non-zero
	USHORT  usCounter;                          // misc counter used in various ways
} myCPM_Database [] = {
	{ 
		{ ' ', ' ', ' ', '4', '5'},
		{ '0', '1'},
		1000,
		0, 0
	},
	{ 
		{ ' ', ' ', ' ', '4', '5'},
		{ '0', '2'},
		500,
		0, 0
	},
	{ 
		{ ' ', ' ', '1', '4', '5'},
		{ '1', '1'},
		1000,
		0, 0
	},
	{ 
		{ ' ', ' ', '1', '4', '5'},
		{ '1', '2'},
		700,
		0, 0
	},
	{ 
		{ ' ', '1', '3', '0', '1'},
		{ '1', '2'},
		700,
		CPM_CP_NOALLOW_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '2'},
		{ '1', '2'},
		700,
		CPM_GC_OUT_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '3'},
		{ '1', '2'},
		700,
		CPM_WRONG_RM_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '4'},
		{ '1', '2'},
		700,
		CPM_WRONG_GID_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '5'},
		{ '1', '2'},
		700,
		CPM_VOID_NOALLOW_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '6'},
		{ '1', '2'},
		700,
		CPM_WRONG_ACNO_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '7'},
		{ '1', '2'},
		700,
		CPM_CP_ADVISE_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '8'},
		{ '1', '2'},
		700,
		CPM_WRONG_ACTYPE_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '0', '9'},
		{ '1', '2'},
		700,
		CPM_WRONG_SLD_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '1', '0'},
		{ '1', '2'},
		700,
		CPM_FLFUL_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '1', '1'},
		{ '1', '2'},
		700,
		CPM_NTINFL_ADR, 0
	},
	{ 
		{ ' ', '1', '3', '1', '2'},
		{ '1', '2'},
		700,
		CPM_WRONG_VOUCH_ADR, 0
	},
	{ 
		{ ' ', '1', '4', '0', '1'},
		{ '1', '2'},
		5001,
		CPM_FC2_OVERRIDE, 0
	},
	{ 
		{ ' ', '1', '4', '0', '2'},
		{ '1', '2'},
		5001,
		CPM_FC2_NEEDGLINE, 0
	},
	{ 
		{ ' ', '1', '4', '0', '3'},
		{ '1', '2'},
		5001,
		CPM_FC2_PARTTEND, 0
	},
	{ 
		{ 0, 0, 0, 0, 0},
		{ 0, 0},
		0,
		CPM_WRONG_RM_ADR, 0
	}
};

struct _tagFuncConv {
	UCHAR  ucFunc;
	CHAR  *ucName;
} myFuncConv [] = {
	{ 0, "Unknown Function"},
	{ CPM_FC2_ROOMCHG, "CPM_FC2_ROOMCHG - room charge func. code "},
	{ CPM_FC2_ACCTCHG, "CPM_FC2_ACCTCHG - account charge func code"},
	{ CPM_FC2_PRECREDITA, "CPM_FC2_PRECREDITA - pre-credit autho. func code"},
	{ CPM_FC2_CREDITA, "CPM_FC2_CREDITA - credit autho. func code"},
	{ CPM_FC2_ACCEPTED, "CPM_FC2_ACCEPTED - accepted"},
	{ CPM_FC2_REJECTED, "CPM_FC2_REJECTED - rejected"},
	{ CPM_FC2_OVERRIDE, "CPM_FC2_OVERRIDE - override required"},
	{ CPM_FC2_NEEDGLINE, "CPM_FC2_NEEDGLINE - need guest line number"},
	{ CPM_FC2_STOP_CHARG, "CPM_FC2_STOP_CHARG - stop Charge Post/Credit Auth."},
	{ CPM_FC2_STOP_POST, "CPM_FC2_STOP_POST - stop Posting"},
	{ CPM_FC2_START, "CPM_FC2_START - start Charge Post/Credit Auth"},
	{ CPM_FC2_PARTTEND, "CPM_FC2_PARTTEND - partial tender"},
	{ 0, NULL}
};


SHORT ServerThread::ProcessRequestCPM(void)
{
	CPMRCRSPDATA *pResp;
	CPMRCREQDATA *pReqst;
	CHAR *pMsg1 = "This is a msg";
	CHAR *pMsg2 = "Error in Transaction";
	CHAR  cTempMsg[16];
	struct _tagFuncConv *pFunc = myFuncConv;
	struct _tagDataBase *pDB = myCPM_Database;

	pReqst = (CPMRCREQDATA *)ServerRcvBuf.auchData;

	pResp = (CPMRCRSPDATA *)ServerSndBuf.auchData;

	for ( ; pFunc->ucName; pFunc++) {
		if (pFunc->ucFunc == pReqst->auchFuncCode2[0])
			break;
	}

	if (!pFunc->ucName)
		pFunc = myFuncConv;


	fprintf (fpLogFile, "\nNew CPM Transaction In  %s\n   SrcTerm %c%c  DesTerm %c%c, SeqNo %c%c%c%c%c   CashierNo %c%c%c   ServerNo %c%c%c\n",
				pFunc->ucName,
				pReqst->auchSrcTerminal[0], pReqst->auchSrcTerminal[1],
				pReqst->auchDesTerminal[0], pReqst->auchDesTerminal[1],
				pReqst->auchDataSeqNo[0], pReqst->auchDataSeqNo[1], pReqst->auchDataSeqNo[2], pReqst->auchDataSeqNo[3], pReqst->auchDataSeqNo[4],
				pReqst->auchCashierNo[0], pReqst->auchCashierNo[1], pReqst->auchCashierNo[2],
				pReqst->auchServerNo[0], pReqst->auchServerNo[1], pReqst->auchServerNo[2]
				);

	fprintf (fpLogFile, "   TermNo %c  StoreNo %c%c%c%c  SystemNo %c%c%c%c   GuestCheckNo %c%c%c%c%c%c\n",
				pReqst->auchTerminalNo[0],
				pReqst->auchStoreNo[0], pReqst->auchStoreNo[1], pReqst->auchStoreNo[2], pReqst->auchStoreNo[3],
				pReqst->auchSystemNo[0], pReqst->auchSystemNo[1], pReqst->auchSystemNo[2], pReqst->auchSystemNo[3],
				pReqst->auchGCNumber[0], pReqst->auchGCNumber[1], pReqst->auchGCNumber[2], pReqst->auchGCNumber[3], pReqst->auchGCNumber[4], pReqst->auchGCNumber[5]
				);

	fprintf (fpLogFile, "   SLD No. %c%c%c%c   Guest Line No %c%c   RoomNo %c%c%c%c%c    GuestID %c%c\n",
				pReqst->auchSLDNumber[0], pReqst->auchSLDNumber[1],	pReqst->auchSLDNumber[2], pReqst->auchSLDNumber[3],
				pReqst->auchGuestLineNo[0], pReqst->auchGuestLineNo[1],
				pReqst->auchRoomNumber[0], pReqst->auchRoomNumber[1], pReqst->auchRoomNumber[2], pReqst->auchRoomNumber[3], pReqst->auchRoomNumber[4],
				pReqst->auchGuestID[0], pReqst->auchGuestID[1]
				);

	fprintf (fpLogFile, "   ChargeTip %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchChargeTip[0], pReqst->auchChargeTip[1], pReqst->auchChargeTip[2], pReqst->auchChargeTip[3], pReqst->auchChargeTip[4],
				pReqst->auchChargeTip[5], pReqst->auchChargeTip[6], pReqst->auchChargeTip[7], pReqst->auchChargeTip[8], pReqst->auchChargeTip[9]
				);

	fprintf (fpLogFile, "   ChargeTotl %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchChargeTtl[0], pReqst->auchChargeTtl[1], pReqst->auchChargeTtl[2], pReqst->auchChargeTtl[3], pReqst->auchChargeTtl[4],
				pReqst->auchChargeTtl[5], pReqst->auchChargeTtl[6], pReqst->auchChargeTtl[7], pReqst->auchChargeTtl[8], pReqst->auchChargeTtl[9]
				);

	fprintf (fpLogFile, "   TotalTax %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchTtlTax[0], pReqst->auchTtlTax[1], pReqst->auchTtlTax[2], pReqst->auchTtlTax[3], pReqst->auchTtlTax[4],
				pReqst->auchTtlTax[5], pReqst->auchTtlTax[6], pReqst->auchTtlTax[7], pReqst->auchTtlTax[8], pReqst->auchTtlTax[9]
				);

	fprintf (fpLogFile, "   DiscItemTotl_1 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchDiscItemTtl1[0], pReqst->auchDiscItemTtl1[1], pReqst->auchDiscItemTtl1[2], pReqst->auchDiscItemTtl1[3], pReqst->auchDiscItemTtl1[4],
				pReqst->auchDiscItemTtl1[5], pReqst->auchDiscItemTtl1[6], pReqst->auchDiscItemTtl1[7], pReqst->auchDiscItemTtl1[8], pReqst->auchDiscItemTtl1[9]
				);

	fprintf (fpLogFile, "   DiscItemTotl_2 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchDiscItemTtl2[0], pReqst->auchDiscItemTtl2[1], pReqst->auchDiscItemTtl2[2], pReqst->auchDiscItemTtl2[3], pReqst->auchDiscItemTtl2[4],
				pReqst->auchDiscItemTtl2[5], pReqst->auchDiscItemTtl2[6], pReqst->auchDiscItemTtl2[7], pReqst->auchDiscItemTtl2[8], pReqst->auchDiscItemTtl2[9]
				);

	fprintf (fpLogFile, "   MSR Data %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchMSRData[0], pReqst->auchMSRData[1], pReqst->auchMSRData[2], pReqst->auchMSRData[3], pReqst->auchMSRData[4],
				pReqst->auchMSRData[5], pReqst->auchMSRData[6], pReqst->auchMSRData[7], pReqst->auchMSRData[8], pReqst->auchMSRData[9],
				pReqst->auchMSRData[10], pReqst->auchMSRData[11], pReqst->auchMSRData[12], pReqst->auchMSRData[13], pReqst->auchMSRData[14],
				pReqst->auchMSRData[15], pReqst->auchMSRData[16], pReqst->auchMSRData[17], pReqst->auchMSRData[18], pReqst->auchMSRData[19],
				pReqst->auchMSRData[20], pReqst->auchMSRData[21], pReqst->auchMSRData[22], pReqst->auchMSRData[23], pReqst->auchMSRData[24],
				pReqst->auchMSRData[25], pReqst->auchMSRData[26], pReqst->auchMSRData[27], pReqst->auchMSRData[28], pReqst->auchMSRData[29],
				pReqst->auchMSRData[30], pReqst->auchMSRData[31], pReqst->auchMSRData[32], pReqst->auchMSRData[33], pReqst->auchMSRData[34],
				pReqst->auchMSRData[35], pReqst->auchMSRData[36], pReqst->auchMSRData[37], pReqst->auchMSRData[38], pReqst->auchMSRData[39]
				);

	fflush (fpLogFile);

	// Next we transfer the PcIF network layer port numbers into our Send Buffer.
	// The PcIF network layer will do the proper translation so that when we see
	// the received buffer, the header is set up os that usLport is our port
	// and usFport is the port on the host which sent us this message.
	ServerSndBuf.usLport = ServerRcvBuf.usLport;
	ServerSndBuf.usFport = ServerRcvBuf.usFport;

	// Transfer the IP address of the sending host to our destination IP in the Send Buffer
	memcpy (ServerSndBuf.auchFaddr, ServerRcvBuf.auchFaddr, sizeof(ServerSndBuf.auchFaddr));

	// Copy some of the fields from the request into the response.  These fields are used
	// by the requesting terminal to trace response with request.
	memcpy (pResp->auchSystemFunc, pReqst->auchSystemFunc, sizeof (pReqst->auchSystemFunc));
	memcpy (pResp->auchDataSeqNo, pReqst->auchDataSeqNo, sizeof (pReqst->auchDataSeqNo));
	memcpy (pResp->auchAcceptAmt, pReqst->auchChargeTtl + 4, sizeof (pResp->auchAcceptAmt));

	// The terminal software will check that the received response has certain
	// fields whose values are the same as what was sent in the request.
	// Lets make sure that those fields are set correctly.
	//		auchSrcTerminal - source terminal
	//		auchDesTerminal - destination terminal
	//		auchTerminalNo - terminal number
	memcpy (pResp->auchSrcTerminal, pReqst->auchSrcTerminal, sizeof (pReqst->auchSrcTerminal));
	memcpy (pResp->auchDesTerminal, pReqst->auchDesTerminal, sizeof (pReqst->auchDesTerminal));
	memcpy (pResp->auchTerminalNo, pReqst->auchTerminalNo, sizeof (pResp->auchTerminalNo));

    memcpy (pResp->auchPostTransNo, "123456", sizeof(pResp->auchPostTransNo));
    memset (pResp->auchMsgText, 0, sizeof(pResp->auchMsgText));
    memcpy (pResp->auchMsgText, pMsg1, strlen(pMsg1));

	// Indicate that this is the last message since this is the only message.
	// The last message indicator is used to indicate whether a Charge Post transaction
	// has multiple messages or just one.
    pResp->auchLastMsgInd[0] = CPM_LAST_MSG;
	
    pResp->auchEndofMsg[0] = 13;

	// Set the response function code to indicate whether the request has been
	// accepted, rejected, or what ever.

	pResp->auchFuncCode2[0] = CPM_FC2_REJECTED;
	sprintf (cTempMsg, "%2.2d", CPM_WRONG_RM_ADR);
	for ( ; pDB->auchRoomNumber[0]; pDB++) {
		if (memcmp (pDB->auchRoomNumber, pReqst->auchRoomNumber, sizeof(pDB->auchRoomNumber)) == 0) {
			if (pDB->ulMaxTotal > 5000) {     // indicates some kind of additional operation needed
				pResp->auchFuncCode2[0] = pDB->usErrorCode;
				pDB->usCounter++;
				if (pDB->usCounter > 1) {   // check to see if we have already received one of these
					pDB->usCounter = 0;
					pResp->auchFuncCode2[0] = CPM_FC2_ACCEPTED;
				}
			}
			else if (pDB->usErrorCode) {      // indicates to return an error code
				sprintf (cTempMsg, "%2.2d", pDB->usErrorCode);
			}
			else {                            // otherwise accept the operation
				pResp->auchFuncCode2[0] = CPM_FC2_ACCEPTED;
			}
			break;
		}
	}

	// Handle a partial tender by setting the amount of the partial tend correctly.
	// 
	if (pResp->auchFuncCode2[0] == CPM_FC2_PARTTEND) {
	}

	pResp->auchCRTRespMsg [0] = cTempMsg[0];
	pResp->auchCRTRespMsg [1] = cTempMsg[1];

	ServerSndBufSize = sizeof (CPMRCRSPDATA);

	return 0;
}


/*

	The data in this struct is used in several ways.
		if ulMaxTotal is greater than 5000, usErrorCode is a notification not an error
		if ulMaxTotal is less than 5000, if usErrorCode is non-zero, its indicates an error condition
		usCounter is used for counting multiple messages such as when dealing with Guest Line.
 */
struct _tagDataBaseEPT {
	UCHAR   auchAcctNo[22];                  /* 54: Account number          */
    UCHAR   auchGuestID[2];                     /* 59: guest ID             */
	ULONG   ulMaxTotal;                         // Maximum the guest can charge
	USHORT  usErrorCode;                        // error code to respond with if non-zero
	USHORT  usCounter;                          // misc counter used in various ways
} myEPT_Database [] = {
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '5'},
		{ '0', '1'},
		1000,
		0, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '5'},
		{ '0', '2'},
		500,
		0, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '4', '5'},
		{ '1', '1'},
		1000,
		0, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '4', '5'},
		{ '1', '2'},
		700,
		0, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '1'},
		{ '1', '2'},
		700,
		CPM_CP_NOALLOW_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '2'},
		{ '1', '2'},
		700,
		CPM_GC_OUT_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '3'},
		{ '1', '2'},
		700,
		CPM_WRONG_RM_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '4'},
		{ '1', '2'},
		700,
		CPM_WRONG_GID_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '5'},
		{ '1', '2'},
		700,
		CPM_VOID_NOALLOW_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '6'},
		{ '1', '2'},
		700,
		CPM_WRONG_ACNO_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '7'},
		{ '1', '2'},
		700,
		CPM_CP_ADVISE_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '8'},
		{ '1', '2'},
		700,
		CPM_WRONG_ACTYPE_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '0', '9'},
		{ '1', '2'},
		700,
		CPM_WRONG_SLD_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '1', '0'},
		{ '1', '2'},
		700,
		CPM_FLFUL_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '1', '1'},
		{ '1', '2'},
		700,
		CPM_NTINFL_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '3', '1', '2'},
		{ '1', '2'},
		700,
		CPM_WRONG_VOUCH_ADR, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '4', '0', '1'},
		{ '1', '2'},
		5001,
		EPT_FC2_OVERRIDE, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '4', '0', '2'},
		{ '1', '2'},
		5001,
		EPT_FC2_NEEDGLINE, 0
	},
	{ 
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '4', '0', '3'},
		{ '1', '2'},
		5001,
		EPT_FC2_PARTTEND, 0
	},
	{ 
		{ 0, 0, 0, 0, 0},
		{ 0, 0},
		0,
		CPM_WRONG_RM_ADR, 0
	}
};

struct _tagFuncConv myFuncConvEPT [] = {
	{ 0, "Unknown Function"},
	{ EPT_FC2_EPT, "EPT_FC2_EPT - do EPT "},
	{ EPT_FC2_ASK, "EPT_FC2_ASK - Ask"},
	{ CPM_FC2_PRECREDITA, "CPM_FC2_PRECREDITA - pre-credit autho. func code"},
	{ CPM_FC2_CREDITA, "CPM_FC2_CREDITA - credit autho. func code"},
	{ EPT_FC2_ACCEPTED, "EPT_FC2_ACCEPTED - accepted"},
	{ EPT_FC2_REJECTED, "EPT_FC2_REJECTED - rejected"},
	{ EPT_FC2_OVERRIDE, "EPT_FC2_OVERRIDE - override required"},
	{ EPT_FC2_NEEDGLINE, "EPT_FC2_NEEDGLINE - need guest line number"},
	{ EPT_FC2_STOP_CHARG, "EPT_FC2_STOP_CHARG - stop EPT"},
	{ EPT_FC2_STOP_POST, "EPT_FC2_STOP_POST - stop Posting"},
	{ EPT_FC2_START, "EPT_FC2_START - start EPT"},
	{ EPT_FC2_PARTTEND, "EPT_FC2_PARTTEND - partial tender"},
	{ 0, NULL}
};



SHORT ServerThread::ProcessRequestEPT(void)
{
	EEPTRSPDATA *pResp;
	EEPTREQDATA *pReqst;
	WCHAR *pMsg1 = L"This is a msg";
	WCHAR *pMsg2 = L"Error in Transaction";
	CHAR  cTempMsg[16];
	struct _tagFuncConv *pFunc = myFuncConv;
	struct _tagDataBaseEPT *pDB = myEPT_Database;

	pReqst = (EEPTREQDATA *)ServerRcvBuf.auchData;

	pResp = (EEPTRSPDATA *)ServerSndBuf.auchData;

	for ( ; pFunc->ucName; pFunc++) {
		if (pFunc->ucFunc == pReqst->auchFuncCode)
			break;
	}

	if (!pFunc->ucName)
		pFunc = myFuncConv;


	fprintf (fpLogFile, "\nNew EPT Transaction In  %s\n   SrcTerm %c%c  DesTerm %c%c, SeqNo %c%c%c%c%c   CashierNo %c%c%c   ServerNo %c%c%c\n",
				pFunc->ucName,
				pReqst->auchSrcTerminal[0], pReqst->auchSrcTerminal[1],
				pReqst->auchDesTerminal[0], pReqst->auchDesTerminal[1],
				pReqst->auchDataSeqNo[0], pReqst->auchDataSeqNo[1], pReqst->auchDataSeqNo[2], pReqst->auchDataSeqNo[3], pReqst->auchDataSeqNo[4],
				pReqst->auchCashierNo[0], pReqst->auchCashierNo[1], pReqst->auchCashierNo[2]
				);

	fprintf (fpLogFile, "   TermNo %c  StoreNo %c%c%c%c  SystemNo %c%c%c%c   GuestCheckNo %c%c%c%c%c%c\n",
				pReqst->auchTerminalNo,
				pReqst->auchStoreNo[0], pReqst->auchStoreNo[1], pReqst->auchStoreNo[2], pReqst->auchStoreNo[3],
				pReqst->auchSystemNo[0], pReqst->auchSystemNo[1], pReqst->auchSystemNo[2], pReqst->auchSystemNo[3],
				pReqst->auchGuest[0], pReqst->auchGuest[1], pReqst->auchGuest[2], pReqst->auchGuest[3], pReqst->auchGuest[4], pReqst->auchGuest[5]
				);

	fprintf (fpLogFile, "   SLD No. %c%c%c%c   Guest Line No %c%c   Hotel ID %c%c\n",
				pReqst->auchSLDNumber[0], pReqst->auchSLDNumber[1],	pReqst->auchSLDNumber[2], pReqst->auchSLDNumber[3],
				pReqst->auchGuestLineNo[0], pReqst->auchGuestLineNo[1],
				pReqst->auchHotelID[0], pReqst->auchHotelID[1]
				);

	fprintf (fpLogFile, "   ChargeTip %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchChargeTip[0], pReqst->auchChargeTip[1], pReqst->auchChargeTip[2], pReqst->auchChargeTip[3], pReqst->auchChargeTip[4],
				pReqst->auchChargeTip[5], pReqst->auchChargeTip[6], pReqst->auchChargeTip[7], pReqst->auchChargeTip[8], pReqst->auchChargeTip[9]
				);

	fprintf (fpLogFile, "   ChargeTotl %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchChargeTtl[0], pReqst->auchChargeTtl[1], pReqst->auchChargeTtl[2], pReqst->auchChargeTtl[3], pReqst->auchChargeTtl[4],
				pReqst->auchChargeTtl[5], pReqst->auchChargeTtl[6], pReqst->auchChargeTtl[7], pReqst->auchChargeTtl[8], pReqst->auchChargeTtl[9]
				);

	fprintf (fpLogFile, "   TotalTax %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchTtlTax[0], pReqst->auchTtlTax[1], pReqst->auchTtlTax[2], pReqst->auchTtlTax[3], pReqst->auchTtlTax[4],
				pReqst->auchTtlTax[5], pReqst->auchTtlTax[6], pReqst->auchTtlTax[7], pReqst->auchTtlTax[8], pReqst->auchTtlTax[9]
				);

	fprintf (fpLogFile, "   DiscItemTotl_1 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchDiscItemTtl1[0], pReqst->auchDiscItemTtl1[1], pReqst->auchDiscItemTtl1[2], pReqst->auchDiscItemTtl1[3], pReqst->auchDiscItemTtl1[4],
				pReqst->auchDiscItemTtl1[5], pReqst->auchDiscItemTtl1[6], pReqst->auchDiscItemTtl1[7], pReqst->auchDiscItemTtl1[8], pReqst->auchDiscItemTtl1[9]
				);

	fprintf (fpLogFile, "   DiscItemTotl_2 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchDiscItemTtl2[0], pReqst->auchDiscItemTtl2[1], pReqst->auchDiscItemTtl2[2], pReqst->auchDiscItemTtl2[3], pReqst->auchDiscItemTtl2[4],
				pReqst->auchDiscItemTtl2[5], pReqst->auchDiscItemTtl2[6], pReqst->auchDiscItemTtl2[7], pReqst->auchDiscItemTtl2[8], pReqst->auchDiscItemTtl2[9]
				);

	fprintf (fpLogFile, "   BonusTotal #1 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus1[0], pReqst->auchBonus1[1], pReqst->auchBonus1[2], pReqst->auchBonus1[3], pReqst->auchBonus1[4],
				pReqst->auchBonus1[5], pReqst->auchBonus1[6], pReqst->auchBonus1[7], pReqst->auchBonus1[8], pReqst->auchBonus1[9]
				);

	fprintf (fpLogFile, "   BonusTotal #2 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus2[0], pReqst->auchBonus2[1], pReqst->auchBonus2[2], pReqst->auchBonus2[3], pReqst->auchBonus2[4],
				pReqst->auchBonus2[5], pReqst->auchBonus2[6], pReqst->auchBonus2[7], pReqst->auchBonus2[8], pReqst->auchBonus2[9]
				);

	fprintf (fpLogFile, "   BonusTotal #3 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus3[0], pReqst->auchBonus3[1], pReqst->auchBonus3[2], pReqst->auchBonus3[3], pReqst->auchBonus3[4],
				pReqst->auchBonus3[5], pReqst->auchBonus3[6], pReqst->auchBonus3[7], pReqst->auchBonus3[8], pReqst->auchBonus3[9]
				);

	fprintf (fpLogFile, "   BonusTotal #4 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus4[0], pReqst->auchBonus4[1], pReqst->auchBonus4[2], pReqst->auchBonus4[3], pReqst->auchBonus4[4],
				pReqst->auchBonus4[5], pReqst->auchBonus4[6], pReqst->auchBonus4[7], pReqst->auchBonus4[8], pReqst->auchBonus4[9]
				);

	fprintf (fpLogFile, "   BonusTotal #5 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus5[0], pReqst->auchBonus5[1], pReqst->auchBonus5[2], pReqst->auchBonus5[3], pReqst->auchBonus5[4],
				pReqst->auchBonus5[5], pReqst->auchBonus5[6], pReqst->auchBonus5[7], pReqst->auchBonus5[8], pReqst->auchBonus5[9]
				);

	fprintf (fpLogFile, "   BonusTotal #6 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus6[0], pReqst->auchBonus6[1], pReqst->auchBonus6[2], pReqst->auchBonus6[3], pReqst->auchBonus6[4],
				pReqst->auchBonus6[5], pReqst->auchBonus6[6], pReqst->auchBonus6[7], pReqst->auchBonus6[8], pReqst->auchBonus6[9]
				);

	fprintf (fpLogFile, "   BonusTotal #7 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus7[0], pReqst->auchBonus7[1], pReqst->auchBonus7[2], pReqst->auchBonus7[3], pReqst->auchBonus7[4],
				pReqst->auchBonus7[5], pReqst->auchBonus7[6], pReqst->auchBonus7[7], pReqst->auchBonus7[8], pReqst->auchBonus7[9]
				);

	fprintf (fpLogFile, "   BonusTotal #8 %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchBonus8[0], pReqst->auchBonus8[1], pReqst->auchBonus8[2], pReqst->auchBonus8[3], pReqst->auchBonus8[4],
				pReqst->auchBonus8[5], pReqst->auchBonus8[6], pReqst->auchBonus8[7], pReqst->auchBonus8[8], pReqst->auchBonus8[9]
				);

	fprintf (fpLogFile, "   Food Stamp %c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchFoodStamp[0], pReqst->auchFoodStamp[1], pReqst->auchFoodStamp[2], pReqst->auchFoodStamp[3], pReqst->auchFoodStamp[4],
				pReqst->auchFoodStamp[5], pReqst->auchFoodStamp[6], pReqst->auchFoodStamp[7], pReqst->auchFoodStamp[8], pReqst->auchFoodStamp[9]
				);

	fprintf (fpLogFile, "   MSR Data %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
				pReqst->auchMSRData[0], pReqst->auchMSRData[1], pReqst->auchMSRData[2], pReqst->auchMSRData[3], pReqst->auchMSRData[4],
				pReqst->auchMSRData[5], pReqst->auchMSRData[6], pReqst->auchMSRData[7], pReqst->auchMSRData[8], pReqst->auchMSRData[9],
				pReqst->auchMSRData[10], pReqst->auchMSRData[11], pReqst->auchMSRData[12], pReqst->auchMSRData[13], pReqst->auchMSRData[14],
				pReqst->auchMSRData[15], pReqst->auchMSRData[16], pReqst->auchMSRData[17], pReqst->auchMSRData[18], pReqst->auchMSRData[19],
				pReqst->auchMSRData[20], pReqst->auchMSRData[21], pReqst->auchMSRData[22], pReqst->auchMSRData[23], pReqst->auchMSRData[24],
				pReqst->auchMSRData[25], pReqst->auchMSRData[26], pReqst->auchMSRData[27], pReqst->auchMSRData[28], pReqst->auchMSRData[29],
				pReqst->auchMSRData[30], pReqst->auchMSRData[31], pReqst->auchMSRData[32], pReqst->auchMSRData[33], pReqst->auchMSRData[34],
				pReqst->auchMSRData[35], pReqst->auchMSRData[36], pReqst->auchMSRData[37], pReqst->auchMSRData[38], pReqst->auchMSRData[39]
				);

	fflush (fpLogFile);

	// Next we transfer the PcIF network layer port numbers into our Send Buffer.
	// The PcIF network layer will do the proper translation so that when we see
	// the received buffer, the header is set up os that usLport is our port
	// and usFport is the port on the host which sent us this message.
	ServerSndBuf.usLport = ServerRcvBuf.usLport;
	ServerSndBuf.usFport = ServerRcvBuf.usFport;

	// Transfer the IP address of the sending host to our destination IP in the Send Buffer
	memcpy (ServerSndBuf.auchFaddr, ServerRcvBuf.auchFaddr, sizeof(ServerSndBuf.auchFaddr));

	// Copy some of the fields from the request into the response.  These fields are used
	// by the requesting terminal to trace response with request.
//	memcpy (pResp->auchSystemFunc, pReqst->auchSystemFunc, sizeof (pReqst->auchSystemFunc));
	memcpy (pResp->auchDataSeqNo, pReqst->auchDataSeqNo, sizeof (pReqst->auchDataSeqNo));
	memcpy (pResp->auchPartTender, pReqst->auchChargeTtl, sizeof (pResp->auchPartTender));

	// The terminal software will check that the received response has certain
	// fields whose values are the same as what was sent in the request.
	// Lets make sure that those fields are set correctly.
	//		auchSrcTerminal - source terminal
	//		auchDesTerminal - destination terminal
	//		auchTerminalNo - terminal number
	memcpy (pResp->auchSrcTerminal, pReqst->auchSrcTerminal, sizeof (pReqst->auchSrcTerminal));
	memcpy (pResp->auchDesTerminal, pReqst->auchDesTerminal, sizeof (pReqst->auchDesTerminal));
	pResp->auchTerminalNo = pReqst->auchTerminalNo;

    memcpy (pResp->auchPostTransNo, "123456", sizeof(pResp->auchPostTransNo));
    memset (pResp->auchMsgText, 0, sizeof(pResp->auchMsgText));
    memcpy (pResp->auchMsgText, pMsg1, wcslen(pMsg1) * sizeof(WCHAR));

	// Indicate that this is the last message since this is the only message.
	// The last message indicator is used to indicate whether a Charge Post transaction
	// has multiple messages or just one.
    pResp->auchLastMsgID = CPM_LAST_MSG;
	
    pResp->uchSTX = EPT_STX;
    pResp->uchETX = EPT_ETX;

	// Set the response function code to indicate whether the request has been
	// accepted, rejected, or what ever.

	pResp->auchStatus = EPT_FC2_REJECTED;
	sprintf (cTempMsg, "%2.2d", CPM_WRONG_RM_ADR);
	for ( ; pDB->auchAcctNo[0]; pDB++) {
		if (memcmp (pDB->auchAcctNo, pReqst->auchAcctNo, sizeof(pDB->auchAcctNo)) == 0) {
			if (pDB->ulMaxTotal > 5000) {     // indicates some kind of additional operation needed
//				pResp->auchFuncCode2[0] = pDB->usErrorCode;
				pDB->usCounter++;
				if (pDB->usCounter > 1) {   // check to see if we have already received one of these
					pDB->usCounter = 0;
					pResp->auchStatus = EPT_FC2_ACCEPTED;
				}
			}
			else if (pDB->usErrorCode) {      // indicates to return an error code
				sprintf (cTempMsg, "%2.2d", pDB->usErrorCode);
			}
			else {                            // otherwise accept the operation
				pResp->auchStatus = EPT_FC2_ACCEPTED;
			}
			break;
		}
	}

	pResp->auchRspMsgNo [0] = cTempMsg[0];
	pResp->auchRspMsgNo [1] = cTempMsg[1];

	ServerSndBufSize = sizeof (EEPTRSPDATA);

	return 0;
}

SHORT ServerThread::ProcessRequest(void)
{
	switch (myThreadType) {
		case ServerTypeCPM:
			return ProcessRequestCPM ();
			break;
		case ServerTypeEPT:
			return ProcessRequestEPT ();
			break;
		default:
			break;
	}
	return 0;
}

/*
	ThreadFunc (ServerThread *pServerThreadClass)

	The ThreadFunc is the server thread which opens the network connections and
	acts as the actual server process thread.

	The argument is a pointer to a ServerThread class through which ThreadFunc
	will then do its network I/O and other services.

	The basic architecture is to accept Charge Post Manager data packets from
	a NeighborhoodPOS terminal, process the Charge Post request, and then send
	back to the terminal the results.
 */

static DWORD WINAPI ThreadFunc( LPVOID lpParam ) 
{
	ServerThread  *myData = (ServerThread *) lpParam;
	int  nStatus = 0;
	int sStatus;

	ASSERT (myData);


    sStatus = PifControl(PIF_NET_START_LAN, NULL);

	myData->NetOpen ();
	for (int i = 0; myData->ContinueComms ; i++) {
		nStatus = myData->NetReceiveBuf ();
		if (! myData->ContinueComms)
			break;
		if (nStatus > 24 ) {
			myData->ProcessRequest ();
			if (! myData->ContinueComms)
				break;
			myData->NetSendBuf ();
		}
	}
	myData->NetClose ();

	sStatus = PifControl(PIF_NET_STOP_LAN, NULL);	


    return 0; 
} 

void ServerThread::StartServerCPM (USHORT usLportL /* = 0 */, USHORT usFportL /* = 0 */)
{
    fchNetStatus = 0;
	usNetHandle = 0;

	if (usLportL) {
		usLport = usLportL;
		usFport = usFportL;
		usNetTimer = 1000;
	}

	fpLogFile = fopen ("ChargePostLog.txt", "w+");
	myThreadType = ServerTypeCPM;

	hThread = CreateThread( 
		NULL,						// no security attributes 
		0,							// use default stack size  
		ThreadFunc,					// thread function 
		this,						// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
 
	// Check the return value for success. 
 
	ASSERT (hThread != NULL);

//	cdTrans.Create (IDD_DIALOG_TRAN_DISPLAY, NULL);

	SetDialogData ();
}

void ServerThread::StartServerEPT (USHORT usLportL /* = 0 */, USHORT usFportL /* = 0 */)
{
    fchNetStatus = 0;
	usNetHandle = 0;

	if (usLportL) {
		usLport = usLportL;
		usFport = usFportL;
		usNetTimer = 1000;
	}

	fpLogFile = fopen ("ElectronicPaymentLog.txt", "w+");

	myThreadType = ServerTypeEPT;

	hThread = CreateThread( 
		NULL,						// no security attributes 
		0,							// use default stack size  
		ThreadFunc,					// thread function 
		this,						// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
 
	// Check the return value for success. 
 
	ASSERT (hThread != NULL);

//	cdTrans.Create (IDD_DIALOG_TRAN_DISPLAY, NULL);

	SetDialogData ();
}


void ServerThread::StopServer (void)
{
	fclose (fpLogFile);
	ContinueComms = 0;
}

int ServerThread::SetDialogData (void)
{
	CPMRCRSPDATA tempResp;

	return 0;

	CPMRCRSPDATA * pResp = (CPMRCRSPDATA *)ServerSndBuf.auchData;
	memcpy (&tempResp, pResp, sizeof(CPMRCRSPDATA));

	cdTrans.SetDialogData (&tempResp);

	return 0;
}
