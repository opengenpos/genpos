/*
* ---------------------------------------------------------------------------
* Title         :   Header File of Common Routines for Loader Board Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRALCOMM.H
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 01.00.00 : H.Ishida   : Add R3.0
* Sep-25-95 : 01.00.03 : T.Nakahata : increase no of file (16->32)
* Feb-07-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Oct-15-98 : 03.03.00 : O.Nakada   : Add V3.3
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


/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define TRANS_LOADER_COMM_APPL          0   /* application parameter        */
#define TRANS_LOADER_COMM_DEPT          1   /* dept file                    */
#define TRANS_LOADER_COMM_PLU           2   /* PLU file                     */
#define TRANS_LOADER_COMM_WAI           3   /* server file                  */
#define TRANS_LOADER_COMM_CAS           4   /* cashier file                 */
#define TRANS_LOADER_COMM_ETK           5   /* ETK file                     */
#define TRANS_LOADER_COMM_CPN           6   /* Combination Coupon file      */
#define TRANS_LOADER_COMM_CSTR          7   /* Control String file          */
#define TRANS_LOADER_COMM_RPT           8   /* Control String file          */
#define TRANS_LOADER_COMM_PPI           9   /* Control String file          */
#define TRANS_LOADER_COMM_MLD          10   /* MLD file, V3.3               */

#define TRANS_LOADER_COMM_NO_FILE      11   /* number of file in PEP        */
#define TRANS_LOADER_COMM_MAX_FILE     63   /* number of file in loader     */
#define TRANS_LOADER_COMM_RESERVED      1   /* number of file for reserve   */

#define TRANS_LOADER_COMM_SAVE          0   /* saving                       */
#define TRANS_LOADER_COMM_LOAD          1   /* loading                      */

#define TRANS_LOADER_COMM_BLOCK       128   /* initial block size           */
#define TRANS_LOADER_COMM_TIMEOUT   10000   /* timeout value                */

#define TRANS_LOADER_DUMMY_SIZE        16   /* dummy area size (R3.0)       */
#define TRANS_LOADER_DUMMY_DATA "!UN_INITIALIZE!\0xff"
            /* dummy data for loaderboad. ==> see "APPL\SRC\astart.asm" */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#define TRANS_LOADER_LEN_FILENAME   9   /* length of file name included NULL*/
typedef struct {
    CHAR    szFileName[TRANS_LOADER_LEN_FILENAME + 1];
    ULONG   ulSize;
    SHORT   sReserved;
} TRALOADERIDX;

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern TRALOADERIDX aTransLoaderIndex[TRANS_LOADER_COMM_MAX_FILE + TRANS_LOADER_COMM_RESERVED];
extern CHAR         *pszTransName[TRANS_LOADER_COMM_NO_FILE];

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ===== TRALCOMM.C ===== */
SHORT   TransLoaderCommOpen(HWND hDlg);
VOID    TransLoaderCommClose(USHORT usPort, HANDLE hMem);
BOOL    TransLoaderCommControl(USHORT usPort);
SHORT   TransLoaderCommCheckFile(CHAR FAR *pszFileName);
VOID    TransLoaderCommSetMessage(HWND hDlg, CHAR FAR *pszFileName, CHAR chFlag);

/* ===== TRALSEND.C ===== */
#if defined(POSSIBLE_DEAD_CODE)
SHORT   TransLoaderSend(HWND hwnd);
#endif
BOOL    TransLoaderSendWrite(USHORT usPort, VOID FAR *pBuff, USHORT usSize);
BOOL    TransLoaderSendUpload(USHORT usPort);
VOID    TransLoaderSendSetIndex(VOID);

/* ===== TRALRECE.C ===== */
SHORT   TransLoaderReceive(HWND hwnd);
BOOL    TransLoaderReceiveRead(USHORT usPort, VOID FAR *pBuff, USHORT usSize);
BOOL    TransLoaderReceiveDonwload(USHORT usPort);


/* ===== End of TRALOAD.H ===== */
