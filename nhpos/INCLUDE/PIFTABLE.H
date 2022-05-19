/*hhhh
*****************************************************************************
**    
**  Title:      Header File for Portable Platform Interface
**
**  File Name:  PIFTABLE.H
**
**  Categoly:   Portable Platform Interface
**
**  Abstruct:   This header file provides symbolic constans and data types
**              to directly access the Platform Interface of the Portable
**              Platform Interface.
**              
**              When the users access the Platform Interface without the
**              Recovery Manager of the Platform Interface, the users must
**              include this header file. 
**              
**              The Recovery Manager must include this header file and define
**              "PIFRM".
**
**              A software sturcture of the Portable Platform Interface to
**              directly access to the Platform Interface is as follows:
**
**                  +-----------------+-------------------+
**                  |                 |                   |
**                  |                 |Applications and   |
**                  |  direct access  |System Applications|
**                  |        |        |                   |
**                  +  -  -  |  -  -  +-------------------+
**                  |        |        | Portable Platform |
**                  |        |        |     Interface     |
**                  |        |        +----------------+  |
**                  |        |        |                |  |
**                  |        |        |Recovery Manager|  |
**                  |        |        |                |  |
**                 ----++----V--------+---------------++---------------------
**                  |  ||   Function Pointers Table   ||  | or | Function  |
**                  |  ++-----------------------------++  |    | Pointers  |
**                  |  |                               |  |    | Table     |
**                  |  |       Platform Interface      |  |    +-----------+
**                  |  |                               |  |    |           |
**                  |  +-------------------------------+  |    | Simulator |
**                  |                                     |    |           |
**                  +-------------------------------------+    |           |
**                  |                                     |    |           |
**                  |          OS(XINU) or BIOS           |    |           |
**                  |                                     |    |           |
**
**
**              When the users include this header file, the user can
**              directly access the Platform Interface through the Function
**              Pointers Table.
**           
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date       Name           Description        
**  1.00      JUN-09-92  I.Shimizu      Initial release
**  00.00.01  AUG-17-92  I.Shimizu      Correspond to rev. G document
**            SEP-30-92  I.Shimizu      Add PifOpenMsr() and PifCloseMsr()
**  G0        FEB-20-95  O.Nakada       Correspond to hospitality release 3.0
**                                      functions
**
*****************************************************************************
hhhh*/


/******************
 *  Define Macro  *
 ******************/
/* These macros change each function of the Portable Platform Interface */
/* from the indirect access to the direct access.                       */
#if defined(PIFRM)
#else

#define PifAbort            ((PIFABORT)papPifFTable[ID_ABORT])
#define PifShutdown			((PIFSHUTDOWN)papPifFTable[ID_SHUTDOWN])
#define PifBeginThread      ((PIFBEGINTHREAD)papPifFTable[ID_BEGINTHREAD])
#define PifEndThread        ((PIFENDTHREAD)papPifFTable[ID_ENDTHREAD])
#define PifGetProcessId     ((PIFGETPROCESSID)papPifFTable[ID_GETPROCESSID])

#define PifCreateSem        ((PIFCREATESEM)papPifFTable[ID_CREATESEM])
#define PifDeleteSem        ((PIFDELETESEM)papPifFTable[ID_DELETESEM])
#define PifRequestSem       ((PIFREQUESTSEM)papPifFTable[ID_REQUESTSEM])
#define PifReleaseSem       ((PIFRELEASESEM)papPifFTable[ID_RELEASESEM])

/*  The message queue functions do not access the Function Pointers Table.
#define PifCreateQueue      ((PIFCREATEQUEUE)papPifFTable[ID_CREATEQUEUE])
#define PifDeleteQueue      ((PIFDELETEQUEUE)papPifFTable[ID_DELETEQUEUE])
#define PifWriteQueue       ((PIFWRITEQUEUE)papPifFTable[ID_WRITEQUEUE])
#define PifReadQueue        ((PIFREADQUEUE)papPifFTable[ID_READQUEUE])
*/

#define PifSleep            ((PIFSLEEP)papPifFTable[ID_SLEEP])
#define PifGetDateTime      ((PIFGETDATETIME)papPifFTable[ID_GETDATETIME])
#define PifSetDateTime      ((PIFSETDATETIME)papPifFTable[ID_SETDATETIME])

#define PifOpenFile         ((PIFOPENFILE)papPifFTable[ID_OPENFILE])
#define PifReadFile         ((PIFREADFILE)papPifFTable[ID_READFILE])
#define PifWriteFile        ((PIFWRITEFILE)papPifFTable[ID_WRITEFILE])
#define PifSeekFile         ((PIFSEEKFILE)papPifFTable[ID_SEEKFILE])
#define PifCloseFile        ((PIFCLOSEFILE)papPifFTable[ID_CLOSEFILE])
#define PifDeleteFile       ((PIFDELETEFILE)papPifFTable[ID_DELETEFILE])

#define PifOpenCom          ((PIFOPENCOM)papPifFTable[ID_OPENCOM])
#define PifReadCom          ((PIFREADCOM)papPifFTable[ID_READCOM])
#define PifWriteCom         ((PIFWRITECOM)papPifFTable[ID_WRITECOM])
#define PifControlCom       ((PIFCONTROLCOM)papPifFTable[ID_CONTROLCOM])
#define PifCloseCom         ((PIFCLOSECOM)papPifFTable[ID_CLOSECOM])

#define PifNetOpen          ((PIFNETOPEN)papPifFTable[ID_NETOPEN])
#define PifNetSend          ((PIFNETSEND)papPifFTable[ID_NETSEND])
#define PifNetReceive       ((PIFNETRECEIVE)papPifFTable[ID_NETRECEIVE])
#define PifNetControl       ((PIFNETCONTROL)papPifFTable[ID_NETCONTROL])
#define PifNetClose         ((PIFNETCLOSE)papPifFTable[ID_NETCLOSE])

#define PifGetChar          ((PIFGETCHAR)papPifFTable[ID_GETCHAR])
#define PifControlKeyboard  ((PIFCONTROLKEYBOARD)papPifFTable[ID_CONTROLKEYBOARD])
#define PifGetKeyLock       ((PIFGETKEYLOCK)papPifFTable[ID_GETKEYLOCK])
#define PifGetWaiter        ((PIFGETWAITER)papPifFTable[ID_GETWAITER])

#define PifDisplayString    ((PIFDISPLAYSTRING)papPifFTable[ID_DISPLAYSTRING])
#define PifDisplayAttr      ((PIFDISPLAYATTR)papPifFTable[ID_DISPLAYATTR])
#define PifLightDescr       ((PIFLIGHTDESCR)papPifFTable[ID_LIGHTDESCR])

#define PifOpenDrawer       ((PIFOPENDRAWER)papPifFTable[ID_OPENDRAWER])
#define PifDrawerStatus     ((PIFDRAWERSTATUS)papPifFTable[ID_DRAWERSTATUS])

#define PifBeep             ((PIFBEEP)papPifFTable[ID_BEEP])
#define PifPlayStanza       ((PIFPLAYSTANZA)papPifFTable[ID_PLAYSTANZA])
#define PifToneVolume       ((PIFTONEVOLUME)papPifFTable[ID_TONEVOLUME])

#define PifSysConfig        ((PIFSYSCONFIG)papPifFTable[ID_SYSCONFIG])

#define PifDisable          ((PIFDISABLE)papPifFTable[ID_DISABLE])
#define PifRestore          ((PIFRESTORE)papPifFTable[ID_RESTORE])

#define PifLog              ((PIFLOG)papPifFTable[ID_LOG])

#define PifReadMsr          ((PIFREADMSR)papPifFTable[ID_READMSR])

#define PifLoadFarData      ((PIFLOADFARDATA)papPifFTable[ID_LOADFARDATA])
#define PifSaveFarData      ((PIFSAVEFARDATA)papPifFTable[ID_SAVEFARDATA])

#define PifControl          ((PIFCONTROL)papPifFTable[ID_CONTROL])

#define PifRestart          ((PIFRESTART)papPifFTable[ID_RESTART])

#define PifTurnOffPower     ((PIFTURNOFFPOWER)papPifFTable[ID_TURNOFFPOWER])

#define PifOpenMsr          ((PIFOPENMSR)papPifFTable[ID_OPENMSR])
#define PifCloseMsr         ((PIFCLOSEMSR)papPifFTable[ID_CLOSEMSR])

#define PifVioStrAttr       ((PIFVIOSTRATTR)papPifFTable[ID_VIOSTRATTR])
#define PifVioStr           ((PIFVIOSTR)papPifFTable[ID_VIOSTR])
#define PifVioCurOnOff      ((PIFVIOCURONOFF)papPifFTable[ID_VIOCURONOFF])
#define PifVioCurType       ((PIFVIOCURTYPE)papPifFTable[ID_VIOCURTYPE])
#define PifVioGetCurPos     ((PIFVIOGETCURPOS)papPifFTable[ID_VIOGETCURPOS])
#define PifVioSetCurPos     ((PIFVIOSETCURPOS)papPifFTable[ID_VIOSETCURPOS])
#define PifVioScrollUp      ((PIFVIOSCROLLUP)papPifFTable[ID_VIOSCROLLUP])
#define PifVioScrollDown    ((PIFVIOSCROLLDOWN)papPifFTable[ID_VIOSCROLLDOWN])
#define PifVioMode          ((PIFVIOMODE)papPifFTable[ID_VIOMODE])
#define PifVioBackLight     ((PIFVIOBACKLIGHT)papPifFTable[ID_VIOBACKLIGHT])
#define PifVioActivePage    ((PIFVIOACTIVEPAGE)papPifFTable[ID_VIOACTIVEPAGE])
#define PifVioLoadCG        ((PIFVIOLOADCG)papPifFTable[ID_VIOLOADCG])

#endif


/*******************************
 *  Define Symbolic Constants  *
 *******************************/
/* function ID */
/* Each direct access uses these ID indicating one of the pointer to */
/* a function in the Function Pointers Table.                        */
/* The original function ID are defined in LOG.H as the module ID. */
#define ID_ABORT           FAULT_AT_PIFABORT           - FAULT_AT_PIF_CLASS
#define ID_BEGINTHREAD     FAULT_AT_PIFBEGINTHREAD     - FAULT_AT_PIF_CLASS
#define ID_ENDTHREAD       FAULT_AT_PIFENDTHREAD       - FAULT_AT_PIF_CLASS
#define ID_GETPROCESSID    FAULT_AT_PIFGETPROCESSID    - FAULT_AT_PIF_CLASS
#define ID_CREATESEM       FAULT_AT_PIFCREATESEM       - FAULT_AT_PIF_CLASS
#define ID_DELETESEM       FAULT_AT_PIFDELETESEM       - FAULT_AT_PIF_CLASS
#define ID_REQUESTSEM      FAULT_AT_PIFREQUESTSEM      - FAULT_AT_PIF_CLASS
#define ID_RELEASESEM      FAULT_AT_PIFRELEASESEM      - FAULT_AT_PIF_CLASS
#define ID_SLEEP           FAULT_AT_PIFSLEEP           - FAULT_AT_PIF_CLASS
#define ID_GETDATETIME     FAULT_AT_PIFGETDATETIME     - FAULT_AT_PIF_CLASS
#define ID_SETDATETIME     FAULT_AT_PIFSETDATETIME     - FAULT_AT_PIF_CLASS
#define ID_OPENFILE        FAULT_AT_PIFOPENFILE        - FAULT_AT_PIF_CLASS
#define ID_READFILE        FAULT_AT_PIFREADFILE        - FAULT_AT_PIF_CLASS
#define ID_WRITEFILE       FAULT_AT_PIFWRITEFILE       - FAULT_AT_PIF_CLASS
#define ID_SEEKFILE        FAULT_AT_PIFSEEKFILE        - FAULT_AT_PIF_CLASS
#define ID_CLOSEFILE       FAULT_AT_PIFCLOSEFILE       - FAULT_AT_PIF_CLASS
#define ID_DELETEFILE      FAULT_AT_PIFDELETEFILE      - FAULT_AT_PIF_CLASS
#define ID_OPENCOM         FAULT_AT_PIFOPENCOM         - FAULT_AT_PIF_CLASS
#define ID_READCOM         FAULT_AT_PIFREADCOM         - FAULT_AT_PIF_CLASS
#define ID_WRITECOM        FAULT_AT_PIFWRITECOM        - FAULT_AT_PIF_CLASS
#define ID_CONTROLCOM      FAULT_AT_PIFCONTROLCOM      - FAULT_AT_PIF_CLASS
#define ID_CLOSECOM        FAULT_AT_PIFCLOSECOM        - FAULT_AT_PIF_CLASS
#define ID_NETOPEN         FAULT_AT_PIFNETOPEN         - FAULT_AT_PIF_CLASS
#define ID_NETSEND         FAULT_AT_PIFNETSEND         - FAULT_AT_PIF_CLASS
#define ID_NETRECEIVE      FAULT_AT_PIFNETRECEIVE      - FAULT_AT_PIF_CLASS
#define ID_NETCONTROL      FAULT_AT_PIFNETCONTROL      - FAULT_AT_PIF_CLASS
#define ID_NETCLOSE        FAULT_AT_PIFNETCLOSE        - FAULT_AT_PIF_CLASS
#define ID_GETCHAR         FAULT_AT_PIFGETCHAR         - FAULT_AT_PIF_CLASS
#define ID_CONTROLKEYBOARD FAULT_AT_PIFCONTROLKEYBOARD - FAULT_AT_PIF_CLASS
#define ID_GETKEYLOCK      FAULT_AT_PIFGETKEYLOCK      - FAULT_AT_PIF_CLASS
#define ID_GETWAITER       FAULT_AT_PIFGETWAITER       - FAULT_AT_PIF_CLASS
#define ID_DISPLAYSTRING   FAULT_AT_PIFDISPLAYSTRING   - FAULT_AT_PIF_CLASS
#define ID_DISPLAYATTR     FAULT_AT_PIFDISPLAYATTR     - FAULT_AT_PIF_CLASS
#define ID_LIGHTDESCR      FAULT_AT_PIFLIGHTDESCR      - FAULT_AT_PIF_CLASS
#define ID_OPENDRAWER      FAULT_AT_PIFOPENDRAWER      - FAULT_AT_PIF_CLASS
#define ID_DRAWERSTATUS    FAULT_AT_PIFDRAWERSTATUS    - FAULT_AT_PIF_CLASS
#define ID_BEEP            FAULT_AT_PIFBEEP            - FAULT_AT_PIF_CLASS
#define ID_PLAYSTANZA      FAULT_AT_PIFPLAYSTANZA      - FAULT_AT_PIF_CLASS
#define ID_TONEVOLUME      FAULT_AT_PIFTONEVOLUME      - FAULT_AT_PIF_CLASS
#define ID_SYSCONFIG       FAULT_AT_PIFSYSCONFIG       - FAULT_AT_PIF_CLASS
#define ID_DISABLE         FAULT_AT_PIFDISABLE         - FAULT_AT_PIF_CLASS
#define ID_RESTORE         FAULT_AT_PIFRESTORE         - FAULT_AT_PIF_CLASS
#define ID_LOG             FAULT_AT_PIFLOG             - FAULT_AT_PIF_CLASS
#define ID_READMSR         FAULT_AT_PIFREADMSR         - FAULT_AT_PIF_CLASS
#define ID_LOADFARDATA     FAULT_AT_PIFLOADFARDATA     - FAULT_AT_PIF_CLASS
#define ID_SAVEFARDATA     FAULT_AT_PIFSAVEFARDATA     - FAULT_AT_PIF_CLASS
#define ID_CONTROL         FAULT_AT_PIFCONTROL         - FAULT_AT_PIF_CLASS
#define ID_RESTART         FAULT_AT_PIFRESTART         - FAULT_AT_PIF_CLASS
#define ID_TURNOFFPOWER    FAULT_AT_PIFTURNOFFPOWER    - FAULT_AT_PIF_CLASS
#define ID_OPENMSR         FAULT_AT_PIFOPENMSR         - FAULT_AT_PIF_CLASS
#define ID_CLOSEMSR        FAULT_AT_PIFCLOSEMSR        - FAULT_AT_PIF_CLASS
#define ID_VIOSTRATTR      FAULT_AT_PIFVIOSTRATTR      - FAULT_AT_PIF_CLASS
#define ID_VIOSTR          FAULT_AT_PIFVIOSTR          - FAULT_AT_PIF_CLASS
#define ID_VIOCURONOFF     FAULT_AT_PIFVIOCURONOFF     - FAULT_AT_PIF_CLASS
#define ID_VIOCURTYPE      FAULT_AT_PIFVIOCURTYPE      - FAULT_AT_PIF_CLASS
#define ID_VIOGETCURPOS    FAULT_AT_PIFVIOGETCURPOS    - FAULT_AT_PIF_CLASS
#define ID_VIOSETCURPOS    FAULT_AT_PIFVIOSETCURPOS    - FAULT_AT_PIF_CLASS
#define ID_VIOSCROLLUP     FAULT_AT_PIFVIOSCROLLUP     - FAULT_AT_PIF_CLASS
#define ID_VIOSCROLLDOWN   FAULT_AT_PIFVIOSCROLLDOWN   - FAULT_AT_PIF_CLASS
#define ID_VIOMODE         FAULT_AT_PIFVIOMODE         - FAULT_AT_PIF_CLASS
#define ID_VIOBACKLIGHT    FAULT_AT_PIFVIOBACKLIGHT    - FAULT_AT_PIF_CLASS
#define ID_VIOACTIVEPAGE   FAULT_AT_PIFVIOACTIVEPAGE   - FAULT_AT_PIF_CLASS
#define ID_VIOLOADCG       FAULT_AT_PIFVIOLOADCG       - FAULT_AT_PIF_CLASS

        
/* This area is reserved
#define ID_CREATEQUEUE     FAULT_AT_PIFCREATEQUEUE     - FAULT_AT_PIF_CLASS
#define ID_DELETEQUEUE     FAULT_AT_PIFDELETEQUEUE     - FAULT_AT_PIF_CLASS
#define ID_WRITEQUEUE      FAULT_AT_PIFWRITEQUEUE      - FAULT_AT_PIF_CLASS
#define ID_READQUEUE       FAULT_AT_PIFREADQUEUE       - FAULT_AT_PIF_CLASS
*/


/************************
 *  Declare Data Types  *
 ************************/
/* data types of each function to access the pointer table of function table */
typedef VOID   (PIFENTRY *PIFABORT)(USHORT usFaultModule, USHORT usFaultCode);
typedef VOID   (PIFENTRY *PIFSHUTDOWN)(VOID);
typedef USHORT (PIFENTRY *PIFBEGINTHREAD)
               (VOID (THREADENTRY *pStart)(VOID),
                VOID FAR *pStack, USHORT usStackSize, USHORT usPriority,
                CONST CHAR FAR *pszThreadName, ...);
typedef VOID   (PIFENTRY *PIFENDTHREAD)(VOID);
typedef USHORT CONST FAR * (PIFENTRY *PIFGETPROCESSID)(VOID);

typedef USHORT (PIFENTRY *PIFCREATESEM)(USHORT usCount);
typedef VOID   (PIFENTRY *PIFDELETESEM)(USHORT usSem);
typedef VOID   (PIFENTRY *PIFREQUESTSEM)(USHORT usSem);
typedef VOID   (PIFENTRY *PIFRELEASESEM)(USHORT usSem);

/*  The message queue functions do not access the pointer table. 
typedef USHORT (PIFENTRY *PIFCREATEQUEUE)(USHORT usCount);
typedef VOID   (PIFENTRY *PIFDELETEQUEUE)(USHORT usQueue);
typedef VOID   (PIFENTRY *PIFWRITEQUEUE)(USHORT usQueue);
typedef VOID * (PIFENTRY *PIFREADQUEUE)(USHORT usQueue);
*/

typedef VOID   (PIFENTRY *PIFSLEEP)(USHORT usMsec);
typedef VOID   (PIFENTRY *PIFGETDATETIME)(DATE_TIME FAR *pDateTime);
typedef VOID   (PIFENTRY *PIFSETDATETIME)(CONST DATE_TIME FAR *pDateTime);

typedef SHORT  (PIFENTRY *PIFOPENFILE)
               (CONST CHAR FAR *pszFileName, CONST CHAR FAR *pszMode);
typedef SHORT  (PIFENTRY *PIFREADFILE)
               (USHORT usFile, VOID FAR *pBuffer, ULONG ulBytes, ULONG *pulBytesRead);
typedef VOID   (PIFENTRY *PIFWRITEFILE)
               (USHORT usFile, CONST VOID FAR *pBuffer, ULONG ulBytes);
typedef SHORT  (PIFENTRY *PIFSEEKFILE)(USHORT usFile, ULONG ulPosition,
                                       ULONG FAR *pulActualPosition);
typedef VOID   (PIFENTRY *PIFCLOSEFILE)(USHORT usFile);
typedef VOID   (PIFENTRY *PIFDELETEFILE)(CONST CHAR FAR *pszFileName);

typedef SHORT  (PIFENTRY *PIFOPENCOM)
               (USHORT usPortId, CONST PROTOCOL FAR *pProtocol);
typedef SHORT  (PIFENTRY *PIFREADCOM)
               (USHORT usPort, VOID FAR *pBuffer, USHORT usBytes);
typedef SHORT  (PIFENTRY *PIFWRITECOM)
               (USHORT usPort, CONST VOID FAR *pBuffer, USHORT usBytes);
typedef SHORT  (PIFENTRY *PIFCONTROLCOM)
               (USHORT usPort, USHORT usFunc, ...);
typedef VOID   (PIFENTRY *PIFCLOSECOM)(USHORT usPort);

typedef SHORT  (PIFENTRY *PIFNETOPEN)(CONST XGHEADER FAR *pHeader);
typedef SHORT  (PIFENTRY *PIFNETSEND)
               (USHORT usNet, CONST VOID FAR *pBuffer, USHORT usBytes);
typedef SHORT  (PIFENTRY *PIFNETRECEIVE)
               (USHORT usNet, VOID FAR *pBuffer, USHORT usBytes);
typedef SHORT  (PIFENTRY *PIFNETCONTROL)
               (USHORT usNet, USHORT usFunc, ...);
typedef VOID   (PIFENTRY *PIFNETCLOSE)(USHORT usNet);

typedef VOID   (PIFENTRY *PIFGETCHAR)(CHARDATA FAR *pInputChar);
typedef VOID   (PIFENTRY *PIFCONTROLKEYBOARD)(USHORT usFunc);
typedef UCHAR  (PIFENTRY *PIFGETKEYLOCK)(VOID);
typedef USHORT (PIFENTRY *PIFGETWAITER)(VOID);

typedef VOID   (PIFENTRY *PIFDISPLAYSTRING)
               (USHORT usDispId, USHORT usRow, USHORT usColumn,
                CONST UCHAR FAR *puchString, USHORT usChars, UCHAR uchAttr);
typedef VOID   (PIFENTRY *PIFDISPLAYATTR)
               (USHORT usDispId, USHORT usRow, USHORT usColumn,
                CONST UCHAR FAR *puchString, USHORT usChars);
typedef VOID   (PIFENTRY *PIFLIGHTDESCR)
               (USHORT usDispId, USHORT usDescrId, UCHAR uchAttr);

typedef USHORT (PIFENTRY *PIFOPENDRAWER)(USHORT usDrawerId);
typedef USHORT (PIFENTRY *PIFDRAWERSTATUS)(USHORT usDrawerId);

typedef VOID   (PIFENTRY *PIFBEEP)(USHORT usMsec);
typedef VOID   (PIFENTRY *PIFPLAYSTANZA)(CONST STANZA FAR *pStanza);
typedef VOID   (PIFENTRY *PIFTONEVOLUME)(USHORT usVolume);

typedef CONST SYSCONFIG FAR * (PIFENTRY *PIFSYSCONFIG)(VOID);

typedef VOID   (PIFENTRY *PIFLOG)(USHORT usModuleId, USHORT usExceptionCode);

typedef SHORT  (PIFENTRY *PIFREADMSR)(MSR_BUFFER FAR *pMsrBuffer);

typedef VOID   (PIFENTRY *PIFLOADFARDATA)(USHORT usSel);
typedef VOID   (PIFENTRY *PIFSAVEFARDATA)(VOID);

typedef SHORT  (PIFENTRY *PIFCONTROL)(USHORT usFunc, VOID FAR *pArgList);

typedef VOID   (PIFENTRY *PIFRESTART)(USHORT usLoadMode);

typedef USHORT (PIFENTRY *PIFTURNOFFPOWER)(VOID);
   
typedef SHORT  (PIFENTRY *PIFOPENMSR)(VOID);
typedef VOID   (PIFENTRY *PIFCLOSEMSR)(VOID);


typedef VOID   (PIFENTRY *PIFVIOSTRATTR)
               (USHORT usRow, USHORT usColumn,
                CONST UCHAR FAR *puchString, USHORT usChars, USHORT usPage);
typedef VOID   (PIFENTRY *PIFVIOSTR)
               (USHORT usRow, USHORT usColumn,
                CONST UCHAR FAR *puchString, USHORT usChars,
                UCHAR  uchAttr, USHORT usPage);
typedef VOID   (PIFENTRY *PIFVIOCURONOFF)(USHORT usControl);
typedef VOID   (PIFENTRY *PIFVIOCURTYPE)(UCHAR uchTop, UCHAR uchBottom);
typedef VOID   (PIFENTRY *PIFVIOGETCURPOS)
               (USHORT FAR *pusRow, USHORT FAR *pusColumn, USHORT usPage);
typedef VOID   (PIFENTRY *PIFVIOSETCURPOS)
               (USHORT usRow, USHORT usColumn, USHORT usPage);
typedef VOID   (PIFENTRY *PIFVIOSCROLLUP)
               (USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr);
typedef VOID   (PIFENTRY *PIFVIOSCROLLDOWN)
               (USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr);
typedef VOID   (PIFENTRY *PIFVIOMODE)(USHORT usMode);
typedef SHORT  (PIFENTRY *PIFVIOBACKLIGHT)(USHORT usControl);
typedef VOID   (PIFENTRY *PIFVIOACTIVEPAGE)(USHORT usPage);
typedef VOID   (PIFENTRY *PIFVIOLOADCG)(UCHAR FAR *puchFonts, UCHAR uchBytes);

/*********************************************
 *  External Static Variables                *
 *********************************************/
extern PIFDEF CONST FAR * NEAR papPifFTable;/* pointer to the Function */
                                            /* Pointers Table          */


/**********************
 *  End of PIFTABLE.H *
 **********************/
