
typedef struct {
	ULONG  ulEventId;
	int    iDialogId;
	TCHAR  *tcsTitle;
	TCHAR  *tcsText;
	int    iNoOptions;
	TCHAR  *tcsOptions[5];
	USHORT usStateCount;             // count used to drive a state machine engine
} CONNENGINE_PRINTSPOOLER_DIALOG;


SHORT ConnEngineSendTransactionFH (BOOL bRecordEj, SHORT sGcfStatus, GCNUM  usGCNumber, SHORT   hsFileHandle, ULONG   ulStartPoint, ULONG ulSize);

SHORT ConnEngineSendButtonPress (TCHAR *tcsWindowName, TCHAR *tcsButtonName, TCHAR *tcsActionType, int iExtendedFsc, TCHAR *tcsArgument);

SHORT ConnEngineSendRequestTransactionStatus (int iIndicator, LONG *plErrorCode);

SHORT ConnEngineSendRequestTransaction (TCHAR *tcsActionType, TCHAR  *uchUniqueIdentifier);

SHORT ConnEngineSendDataQueryResult (TCHAR *tcsActionType, TCHAR  *uchDataQueryResult);

SHORT ConnEngineSendCashierActionWithAddon (TCHAR *tcsActionType, TCHAR *tcsEmployeeId, TCHAR *tcsAddOnString);

SHORT ConnEngineSendEmployeeDataChange (TCHAR *tcsActionType, TCHAR *tcsSapId, TCHAR *tcsStatus, ULONG ulStatusFlags);

SHORT ConnEngineSendEmployeeDataField (TCHAR *tcsActionType, VOID  *EtkDetailsArray, int iCount);

SHORT ConnEngineSendCashierSignInSignOut (TCHAR *tcsActionType, TCHAR *tcsSapId);

SHORT ConnEngineSendCashierActionWithAmount (TCHAR *tcsActionType, TCHAR *tcsEmployeeId, TCHAR *tcsReference, DCURRENCY lAmount, USHORT usVoidIndic);

SHORT ConnEngineSendCashierActionWithError (TCHAR *tcsActionType, TCHAR *tcsReference, ULONG ulErrorCode, TCHAR *tcsErrorString);

SHORT ConnEngineInitCriticalRegion (USHORT usConnEngineObjectEchoElectronicJournalFlag);

SHORT ConnEngineObjectEchoElectronicJournalSetting (USHORT usConnEngineObjectEchoElectronicJournalFlag);

SHORT ConnEngineSendResponseWithErrorLocal (TCHAR *tcsCommandType, TCHAR *tcsPrefix, TCHAR *tcsActionType, TCHAR *tcsReference, ULONG ulErrorCode, TCHAR *tcsErrorString);
SHORT ConnEngineSendResponseWithError (TCHAR *tcsCommandType, TCHAR *tcsActionType, TCHAR *tcsReference, ULONG ulErrorCode, TCHAR *tcsErrorString);

SHORT ConnEngineEjEntryNext (ULONG ulFilePosHigh, ULONG ulFilePosLow);
SHORT  ConnEngineEjEntryFirst (VOID);
SHORT ConnEngineEjEntryClear (ULONG ulFilePosHigh, ULONG ulFilePosLow);

SHORT ConnEngineSendStoreAndForwardAction (TCHAR *tcsActionType, ITEMTENDER *ItemTender, ITMSTOREFORWARD *pdata, VOID* resData);
SHORT ConnEngineSendPreauthCaptureAction (TCHAR *tcsActionType, ITEMTENDER *pItemTender, ITEMTENDERPREAUTH *pdata, VOID* resData);
SHORT ConnEngineSendStateChange (TCHAR *tcsActionType, TCHAR *tcsService, ULONG ulEventId, CONNENGINE_PRINTSPOOLER_DIALOG  *pEventDialog);

#define CONNENGINE_ACTIONTYPE_RECEIVEACCOUNT   _T("receive-account")
#define	CONNENGINE_ACTIONTYPE_PAIDOUT          _T("paid-out")
#define CONNENGINE_ACTIONTYPE_LOAN             _T("loan")
#define CONNENGINE_ACTIONTYPE_PICKUP           _T("pick-up")
#define CONNENGINE_ACTIONTYPE_SIGNIN           _T("sign-in")
#define CONNENGINE_ACTIONTYPE_SIGNOUT          _T("sign-out")
#define CONNENGINE_ACTIONTYPE_CANCELTRAN       _T("cancel-tran")
#define CONNENGINE_ACTIONTYPE_NOSALE           _T("no-sale")
#define CONNENGINE_ACTIONTYPE_EVENT            _T("event")
#define CONNENGINE_ACTIONTYPE_TIMEIN           _T("time-in")
#define CONNENGINE_ACTIONTYPE_TIMEOUT          _T("time-out")

#define CONNENGINE_SERVICE_PRINTSPOOLER        _T("printspooler")

