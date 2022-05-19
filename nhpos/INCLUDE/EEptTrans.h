/*EEptTrans.h

*/

#include <eept.h>

#ifdef _cplusplus
	extern "C" {
#endif

#if defined(_EEPT_DLL)
#define EEPTINTERFACE_DECLSPEC __declspec(dllexport)
#else
#define EEPTINTERFACE_DECLSPEC __declspec(dllimport)
#endif

#if defined(_PINPAD_DLL)
#define PINPAD_DECLSPEC __declspec(dllexport)
#else
#define PINPAD_DECLSPEC __declspec(dllimport)
#endif

#ifdef _cplusplus
	}
#endif

//EEptInterface ERROR CODES
#define EEPTINTERFACE_SUCCESS						0		//Success code
#define EEPTINTERFACE_FAILURE						1		//Failure code

//DSIServer Specific
#define EEPTINTERFACE_PASS_VERIFY					2000  //Password Verified
#define EEPTINTERFACE_QUE_FULL						2001  //Queue Full
#define EEPTINTERFACE_PASS_FAILED					2002  //Password Failed – Disconnecting
#define EEPTINTERFACE_SYS_GO_OFFLINE				2003  //System Going Offline
#define EEPTINTERFACE_DISCONNECTING_SOCKET			2004  //Disconnecting Socket
#define EEPTINTERFACE_MAX_CONNECTIONS				2006  //Refused ‘Max Connections’
#define EEPTINTERFACE_DUP_SERIALNO					2008  //Duplicate Serial Number Detected
#define EEPTINTERFACE_PASS_FAIL_CLISVR				2009  //Password Failed (Client / Server)
#define EEPTINTERFACE_PASS_FAIL_CHLNG_RESP			2010  //Password failed (Challenge / Response)
#define EEPTINTERFACE_INTERNAL_SVR_ERROR			2011  //Internal Server Error – Call Provider
//                                                  2012  //Merchant ID Not Found

//DSIClient Specific
#define	EEPTINTERFACE_GEN_FAIL						1001  //General Failure
#define	EEPTINTERFACE_INVALID_CMD_FMT				1003  //Invalid Command Format
#define	EEPTINTERFACE_INSUFF_FIELDS					1004  //Insufficient Fields
#define	EEPTINTERFACE_API_NOT_RECOGNIZE				1006  //Global API Not Initialized
#define	EEPTINTERFACE_TIMEOUT_RESPONSE				1007  //Timeout on Response
#define	EEPTINTERFACE_EMPTY_CMD_STR					1011  //Empty Command String
#define EEPTINTERFACE_BATCH_SIZE_LIMIT				1024  // Batch size limit (975 cards) is reached, settle batch must be done.
#define EEPTINTERFACE_SERVER_DATABASE_ERROR			1500  // Server reports "Database Error." possibly needed restart of server.
#define EEPTINTERFACE_SERVER_DATABASE_ERROR01		1501  // Server reports "Database Error." possibly needed restart of server.
#define EEPTINTERFACE_SERVER_DATABASE_ERROREND		1600  // Arbitrary "Database Error." end of range.

#define	EEPTINTERFACE_IN_PROCESS					3002  //In Process with server
#define	EEPTINTERFACE_SOCK_ERROR					3003  //Socket Error sending request.
#define	EEPTINTERFACE_SOCK_OPEN						3004  //Socket already open or in use
#define	EEPTINTERFACE_SOCK_CREATE_FAILED			3005  //Socket Creation Failed
#define	EEPTINTERFACE_SOCK_CONNECT_FAILED			3006  //Socket Connection Failed
#define	EEPTINTERFACE_CONNECT_LOST					3007  //Connection Lost
#define	EEPTINTERFACE_TCPIP_FAILED					3008  //TCP/IP Failed to Initialize
#define	EEPTINTERFACE_PASSWORD_FAILED				3009  //Control failed to find branded serial (password lookup failed)
#define	EEPTINTERFACE_TIMEOUT_SERVER_RESPONSE		3010  //Time Out waiting for server response
#define	EEPTINTERFACE_CONNECT_CANCEL				3011  //Connect Cancelled
#define	EEPTINTERFACE_128BIT_CRYPT_FAILED			3012  //128 bit CryptoAPI failed.
#define	EEPTINTERFACE_AUTH_STARTED					3014  //Threaded Auth Started Expect Response Event
#define	EEPTINTERFACE_EVENT_THREAD_FAILED			3017  //Failed to start Event Thread.
#define	EEPTINTERFACE_XML_PARSE_ERROR				3050  //XML Parse Error
#define	EEPTINTERFACE_CONNECTIONS_FAILED			3051  //All Connections Failed
#define	EEPTINTERFACE_LOGIN_FAILED					3052  //Server Login Failed
#define	EEPTINTERFACE_INITIALIZE_FAILED				3053  //Initialize Failed

#define	EEPTINTERFACE_TOO_SHORT						4001  //Global Response Length Error (Too Short)
#define	EEPTINTERFACE_PARSE_RESP_ERROR				4002  //Unable to Parse Response from Global (Indistinguishable)
#define	EEPTINTERFACE_GLOBAL_STRING_ERROR			4003  //Global String Error
#define	EEPTINTERFACE_WEAK_ENCRYPT_NOT_SUPPORT		4004  //Weak Encryption Request Not Supported
#define	EEPTINTERFACE_CLEAR_TEXT_NOT_SUPPORT		4005  //Clear Text Request Not Supported
#define	EEPTINTERFACE_UNREC_FORMAT					4010  //Unrecognized Request Format
#define	EEPTINTERFACE_DECRYPT_ERROR					4011  //Error Occurred While Decrypting Request
#define	EEPTINTERFACE_NO_REC						4014  //Record Not Found, usually means request info different from server info.
#define	EEPTINTERFACE_INVALID_CK_DIGIT				4017  //Invalid Check Digit
#define	EEPTINTERFACE_MERCID_MISSING				4018  //Merchant ID Missing
#define	EEPTINTERFACE_TSTREAM_MISSING				4019  //TStream Type Missing
#define	EEPTINTERFACE_ENCRYPT_RESPONSE_FAILED		4020  //Could Not Encrypt Response- Call Provider
#define	EEPTINTERFACE_DATACAP_ONLY_MAJOR_ACCEPTED	4040  //Only Major Credit Cards Accepted.
#define	EEPTINTERFACE_DATACAP_SERVER_ERROR			4050  //Unknown error on DataCap Server - check server logs for details

//                                                  4101  //No Test Cards on Live Merchant ID
//                                                  4102  //No Test Cards on Live Merchant ID

#define	EEPTINTERFACE_UNKOWN_ERROR					9999  //Unknown Error

#define	EEPTINTERFACE_INV_TRANTYPE					100201  //Invalid Transaction Type
#define	EEPTINTERFACE_INV_OPERID					100202  //Invalid Operator ID
#define	EEPTINTERFACE_INV_MEMO						100203  //Invalid Memo
#define	EEPTINTERFACE_INV_ACCTNO					100204  //Invalid Account Number
#define	EEPTINTERFACE_INV_EXPDATE					100205  //Invalid Expiration Date
#define	EEPTINTERFACE_INV_AUTHCODE					100206  //Invalid Authorization Code
#define	EEPTINTERFACE_INV_REFNO						100207  //Invalid Reference Number
#define	EEPTINTERFACE_INV_AUTH_AMT					100208  //Invalid Authorization Amount
#define	EEPTINTERFACE_INV_CASHBACK_AMT				100209  //Invalid Cash Back Amount
#define	EEPTINTERFACE_INV_GRAT_AMT					100210  //Invalid Gratuity Amount
#define	EEPTINTERFACE_INV_PURCHASE_AMT				100211  //Invalid Purchase Amount
#define	EEPTINTERFACE_INV_MSR						100212  //Invalid Magnetic Stripe Data
#define	EEPTINTERFACE_INV_PIN						100213  //Invalid PIN Block Data
#define	EEPTINTERFACE_INV_DERVKEY					100214  //Invalid Derived Key Data
#define	EEPTINTERFACE_INV_STATE						100215  //Invalid State Code
#define	EEPTINTERFACE_INV_DOB						100216  //Invalid Date of Birth
#define	EEPTINTERFACE_INV_CKTYPE					100217  //Invalid Check Type
#define	EEPTINTERFACE_INV_ROUTINGNO					100218  //Invalid Routing Number
#define	EEPTINTERFACE_INV_TRANCODE					100219  //Invalid TranCode
#define	EEPTINTERFACE_INV_MERCID					100220  //Invalid Merchant ID
#define	EEPTINTERFACE_INV_TSTREAM					100221  //Invalid TStream Type
#define	EEPTINTERFACE_INV_BATCHNO					100222  //Invalid Batch Number
#define	EEPTINTERFACE_INV_BATCHCOUNT				100223  //Invalid Batch Item Count
#define	EEPTINTERFACE_INV_MICR						100224  //Invalid MICR Input Type
#define	EEPTINTERFACE_INV_DL						100225  //Invalid Driver’s License
#define	EEPTINTERFACE_INV_SEQNO						100226  //Invalid Sequence Number
#define	EEPTINTERFACE_INV_PASS_DATA					100227  //Invalid Pass Data
#define	EEPTINTERFACE_INV_CARDTYPE					100228  //Invalid Card Type
#define EEPTINTERFACE_INV_INVALIDFIELD              100231  //Invalid Field - Record Number
#define	EEPTINTERFACE_INV_LOOKUPNAMEDATA			100245  //Invalid Request Data Specified such as empty fields
#define	EEPTINTERFACE_INV_LOOKUPNUMBERDATA			100246  //Invalid Request Data Specified such as empty fields
#define	EEPTINTERFACE_INV_MAXIMUM_VALUE				100999  //Maximum value for Invalid type error codes allowing more flexible filter.

//Verifone 1000 PinPad Specific
#define PINPAD_SUCCESS								0 //Operation Successful.
#define PINPAD_FAILURE								1 //General Communication Failure.
#define PINPAD_INVALID_PORT							2 //Invalid Com Port Value(Out of Range).
#define PINPAD_CANNOT_INIT_PORT						3 //Cannot Open Com Port.
#define PINPAD_CANNOT_SET_HANDSHAKE					4 //Cannot Set Line Signal HandShake on Port.
#define PINPAD_FAILED_TO_SET_PROMPT					5 //Failed To Set Prompt
#define PINPAD_WORKING_KEY_LENGTH					6 //Invalid Working Key Length.
#define PINPAD_BAD_KEYTYPE							7 //Key Type is Invalid or could not be set.(0)Master/Session (1)Master Only (2)DUKPT
#define PINPAD_DISK_WRITE_FAILED					8 //Disk Write Failed!
#define PINPAD_IDLE_PROMPT_LENGTH					9 //The Prompt you tried to set is of improper length.
#define PINPAD_INVALID_ACCOUNT_NO					10 //The Account Number is Invalid.
#define PINPAD_AMOUNT_ZERO							11 //An amount of zero(0) is invalid.
#define PINPAD_AMOUNT_INVALID						12 //The amount is invalid.
#define PINPAD_NOT_SETUP							13 //Control cannot find Settings. Call the setup method again.
#define PINPAD_CANCELLED							14 //Cancelled.
#define PINPAD_NO_MASTER_KEY						15 //Master Key Not Available.
#define PINPAD_NO_DUKPT_SERNO						16 //No DUKPT Serial Number in PinPad or no response to DUKPT Pin Test..
#define PINPAD_CLEAR_KEY							17 //Clear Key was Hit
#define PINPAD_MESSAGE_TOO_LONG						18 //Message too long. PinPad messages frames are 16 characters or less.
#define PINPAD_NOT_INITIALIZED						19 //Initialization failed or was never called.
#define PINPAD_IN_PROCESS							20 //Ver2000X is not finished processing your last request.
#define PINPAD_CLEAR_SENT							21 //A Clear Command request was sent to the pinpad while it was waiting for user input.
#define PINPAD_TIME_OUT								22 //A TimeOut occurred while waiting for user input.
#define PINPAD_CANNOT_ENABLE_CARD_READER			23 //Card Reader failed to enable/disable per request.
#define PINPAD_TIME_OUT_ON_CARD						24 //A Time Out occurred while waiting for a card to be slid through the reader.
#define PINPAD_TIME_OUT_ON_PIN						25 //A Time Out occurred while waiting for a pin to be entered.
#define PINPAD_KEYTYPE_NOT_MASTER_SESSION			26 //An attempt to set a new working was made and the key management was other than Master/Session
#define PINPAD_MASTER_KEY_INDEX_OUT_OF_RANGE		27 //Master Key Index must be 0-9 inclusive.
#define PINPAD_IN_MANUAL_SETUP						28 //Setup In Progress. Please Wait for completion.

//Verifone Everest PinPad Specific
#define VERIFONE_EVEREST_FAIL						5000	//General Failure
#define VERIFONE_EVEREST_TIMEOUT					5001	//Timeout
#define VERIFONE_EVEREST_POS_CANCELLED				5002	//Cancelled at POS
#define VERIFONE_EVEREST_USER_CANCELLED				5003	//Cancelled at PinPad
#define VERIFONE_EVEREST_XML_PARSEERROR				5004	//Failed To Parse XML
#define VERIFONE_EVEREST_INVALID_PORT				5005	//Invalid Com Port Value(Out of Range).
#define VERIFONE_EVEREST_INVALID_COMMAND			5006	//Invalid Command.
#define VERIFONE_EVEREST_PORT_INIT_FAILED			5007	//Could Not Access Com Port
#define VERIFONE_EVEREST_IN_PROCESS					5008	//In Process!
#define VERIFONE_EVEREST_INVALID_TIMEOUT			5009	//Invalid Timeout Value (5-60)
#define VERIFONE_EVEREST_INVALID_AMOUNT				5010	//Invalid Amount
#define VERIFONE_EVEREST_INVALID_ACCOUNT			5011	//Invalid Account
#define VERIFONE_EVEREST_UNEXPECTED_RESPONSE		5012	//Pertinent Error Information
#define VERIFONE_EVEREST_UNKNOWN					9999	//Unknown Error

//DSIEncryptX specific
#define EEPTINTERFACE_ENCRYPT_UNCREATED				6000  //DSI Encryption control for Store and Forward not created

#define EEPTINTERFACE_LOG_REQUEST					1		//Request Type log entry
#define EEPTINTERFACE_LOG_RESPONSE					2		//Response Type log entry
#define EEPTINTERFACE_LOG_ERROR_TEXT				3		//Response Type log entry

#define EEPTINTERFACE_GETPIN						1		//Response will be from get pin command
#define EEPTINTERFACE_INIT							2		//Response will be from init command
#define EEPTINTERFACE_RESETPINPAD					3		//Response will be from reset command
#define EEPTINTERFACE_GETMAGSTRIPE					4		//Response will be from get magstripe command
#define EEPTINTERFACE_SERVERIPCONFIG				5		//Response will be from serveripconfig command
#define EEPTINTERFACE_PINGSTOREDSERVER				6		//Response will be from ping to server command
#define EEPTINTERFACE_PINGSERVER					7		//Response will be from ping server command
#define EEPTINTERFACE_IP_FROM_HOST					8		//Response will be from ip from host command
#define EEPTINTERFACE_BUILD_EXECUTE					9		//Response will be from build execute command
#define EEPTINTERFACE_SET_BAUD_RATE					10		//Response will be from set baud rate command
#define EEPTINTERFACE_ENCRYPT_XML					11		//Response will be from encrypt XML command

#define XEPT_LOG_MAX		1024*3000						//Max Size of log file before move to begin

#define EEPTINTERFACE_MASK_ACCTNUM		0x00000001
#define EEPTINTERFACE_MASK_EXPNUM		0x00000002
#define EEPTINTERFACE_MASK_MASK0		0x00000010
#define EEPTINTERFACE_MASK_MASK2		0x00000020
#define EEPTINTERFACE_MASK_MASK4		0x00000040
#define EEPTINTERFACE_MASK_MASK6		0x00000080

//Store and Forward Specific see CDsiClient::ForwardStoredTransactions()
#define EEPT_END_OF_FILE	   (-1)
#define EEPT_ERROR_BLOCKSIZE   (-2)
#define EEPT_ERROR_EQUIP_PROV  (-3)    // same as LDT_EQUIPPROVERROR_ADR, hardware not setup
#define EEPT_ERROR_SW_PROV     (-4)    // same as LDT_EQUIPPROVERROR_ADR, software not provisioned such as no payment client
#define EEPT_ERROR_FILE_READ   (-5)    // indicates problem reading the file.

#define EEPT_FILE_NOT_EMPTY	1

#define EEPT_ENCRYPT_BUF_SIZE  2048    // 2KB of TCHARs so it is sized at 4096 bytes for the buffer size for encrypted data


//Key Management Types
#define KEY_MANAGEMENT_MSESSION						0
#define KEY_MANAGEMENT_MONLY						1
#define KEY_MANAGEMENT_DUKPT						2


#ifdef __cplusplus
	#define EEPTINTERFACE_EXTERN_C extern "C"
#else
	#define EEPTINTERFACE_EXTERN_C
#endif

EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC VOID	XEEptSetOposValues(TCHAR *pPinPadEncryptedPin, TCHAR *m_PinPadKeySerialNumber, TCHAR *PinPadTrack2);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptCheckServerPing(VOID);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC BOOL	XEEptOpen(TCHAR* tchRegStr);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC VOID	XEEptClose();
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptBuildXMLAndExecuteTrans(EEPTREQDATA * pXEPTReqData, EEPTRSPDATA * pXEPTRespData);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC ULONG XEEptCheckAndRecoverFile(SHORT sTypeDsi);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT	XEEptForwardStoreForward(USHORT usTender, EEPTRSPDATA *resData,ULONG *ulMaskData, ITMSTOREFORWARD *ForwardEJ);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptForwardStoreForwardDecrypt(TCHAR *AcctNumberEncrypted, TCHAR *AccountNo, TCHAR *ExpireDate);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptGenerateStoredResponseMessage(VOID* resData, ULONG *pulMaskData, VOID* reqData, ULONG ulLastStoredConsecNo);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptToggleStoreForward(SHORT onOff);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC LONG  XEEptDeleteFailedSF(LONG lUieAc102_SubMenu, EEPT_BLOCK_HEADER* Totals);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptBuildFreedomPayAndExecuteTrans(VOID* pXEPTReqData, VOID* pEEptSpool);

EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC BOOL	 XEEptCheckPinPad(VOID);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC USHORT XEEptGetPinPadType();
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC USHORT XEEptGetInterfaceType();

EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptCollectStripeAndPin(TCHAR* strAmount, SHORT sCardTimeOut, SHORT sPinTimeOut, BOOL bScreen, UCHAR uchDecimal);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptCollectPin(TCHAR* strAmount, TCHAR* strAcctNo, SHORT sPinTimeOut, BOOL bScreen, UCHAR uchDecimal);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptCollectStripe(SHORT sCardTimeOut, BOOL bScreen);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC VOID	XEEptGetLastStripe(TCHAR* tchMagStripe);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC VOID	XEEptGetLastPin(TCHAR* tchPinBlock);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptResetPinPad();
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT XEEptCancelRequest();
EEPTINTERFACE_EXTERN_C PINPAD_DECLSPEC		  VOID	DllInitDyn();

EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC LONG   XEEptGetNextBlockIterateAll(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC LONG   XEEptGetNextBlockIterate(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC LONG   XEEptGetSpecifiedBlockHeader(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC LONG   XEEptUpdateSpecifiedBlock(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT  XEEptGetTenderNo(short sTypeDsi, EEPT_BLOCK_HEADER *blockHeader);
EEPTINTERFACE_EXTERN_C EEPTINTERFACE_DECLSPEC SHORT  XEEptUnjoinClear();