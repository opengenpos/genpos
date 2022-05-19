/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,2170 US Hospital Application :
:   Program Name       : uifprog.h                                         :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-04 : 00.00.00     :                         :
:   Update Histories   : 93-06-23 : 01.00.12     : Adds PROG 50            :
:                      : 93-07-02 : 01.00.12     : Adds PROG 15            :
:                      : 93-11-11 : 03.00.01     : Adds PROG 54
:  ----------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00A by Microsoft Corp.                :
:   Memory Model       : Midium Model                                      :
:   Condition          :                                                   :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/
/**
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
**/

/*------------------------------------------------------------------------*\

            T Y P E D E F     S T R U C T U R Es

\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
    Define - Function Name
\*------------------------------------------------------------------------*/
               
#define CID_PGMODE              1               /* UifPGMode() */ 
#define CID_PGMODEIN            2               /* UifPGModeIn() */
#define CID_PGENTER             3               /* UifPGEnter() */ 
#define CID_PG1FUNCTION         4               /* UifPG1Function() */
#define CID_PG1ISSUERPT         5               /* UifPG1IssueRpt() */
#define CID_PG1ENTERADDR        6               /* UifPG1EnterAddr() */
#define CID_PG1ENTERDATA        7               /* UifPG1EnterData() */
#define CID_PG2FUNCTION         8               /* UifPG2Function() */
#define CID_PG2ISSUERPT         9               /* UifPG2IssueRpt() */
#define CID_PG2ENTERADDR        10              /* UifPG2EnterAddr1() */
#define CID_PG2ENTERDATA1       11              /* UifPG2EnterData1() */
#define CID_PG2ENTERDATA2       12              /* UifPG2EnterData2() */
#define CID_PG3FUNCTION         13              /* UifPG3Function() */
#define CID_PG3ISSUERPT         14              /* UifPG3IssueRpt() */
#define CID_PG3ENTERADDR        15              /* UifPG3EnterAddr() */
#define CID_PG3ENTERDATA1       16              /* UifPG3EnterData1() */
#define CID_PG3ENTERDATA2       17              /* UifPG3EnterData2() */
#define CID_PG6FUNCTION         18              /* UifPG6Function() */
#define CID_PG6ISSUERPT         19              /* UifPG6IssueRpt() */
#define CID_PG6ENTERADDR        20              /* UifPG6EnterAddr() */
#define CID_PG6ENTERDATA        21              /* UifPG6EnterData() */
#define CID_PG7FUNCTION         22              /* UifPG7Function() */
#define CID_PG7ISSUERPT         23              /* UifPG7IssueRpt() */
#define CID_PG7ENTERADDR        24              /* UifPG7EnterAddr() */
#define CID_PG7ENTERDATA        25              /* UifPG7EnterData() */
#define CID_PG8FUNCTION         26              /* UifPG8Function() */
#define CID_PG8ISSUERPT         27              /* UifPG8IssueRpt() */
#define CID_PG8ENTERADDR        28              /* UifPG8EnterAddr() */
#define CID_PG8ENTERDATA        29              /* UifPG8EnterData() */
#define CID_PG9FUNCTION         30              /* UifPG9Function() */
#define CID_PG9ISSUERPT         31              /* UifPG9IssueRpt() */
#define CID_PG9ENTERADDR        32              /* UifPG9EnterAddr() */
#define CID_PG9ENTERDATA        33              /* UifPG9EnterData() */
#define CID_PG10FUNCTION        34              /* UifPG10Function() */
#define CID_PG10ISSUERPT        35              /* UifPG10IssueRpt() */
#define CID_PG10ENTERADDR       36              /* UifPG10EnterAddr() */
#define CID_PG10ENTERDATA       37              /* UifPG10EnterData() */
#define CID_PG15FUNCTION        38              /* UifPG15Function() */
#define CID_PG15ISSUERPT        39              /* UifPG15IssueRpt() */
#define CID_PG15ENTERADDR       40              /* UifPG15EnterAddr() */
#define CID_PG15ENTERDATA       41              /* UifPG15EnterData() */
#define CID_PG17FUNCTION        42              /* UifPG17Function() */
#define CID_PG17ISSUERPT        43              /* UifPG17IssueRpt() */
#define CID_PG17ENTERADDR       44              /* UifPG17EnterAddr() */
#define CID_PG17ENTERDATA       45              /* UifPG17EnterData() */
#define CID_PG18FUNCTION        46              /* UifPG18Function() */
#define CID_PG18ISSUERPT        47              /* UifPG18IssueRpt() */
#define CID_PG18ENTERADDR       48              /* UifPG18EnterAddr() */
#define CID_PG18ENTERDATA       49              /* UifPG18EnterData() */
#define CID_PG20FUNCTION        50              /* UifPG20Function() */
#define CID_PG20ENTERALPHA      51              /* UifPG20EnterALPHA() */
#define CID_PG20ISSUERPT        52              /* UifPG20IssueRpt() */
#define CID_PG20ENTERADDR       53              /* UifPG20EnterAddr() */
#define CID_PG21FUNCTION        54              /* UifPG21Function() */
#define CID_PG21ENTERALPHA      55              /* UifPG21EnterALPHA() */
#define CID_PG21ISSUERPT        56              /* UifPG21IssueRpt() */
#define CID_PG21ENTERADDR       57              /* UifPG21EnterAddr() */
#define CID_PG22FUNCTION        58              /* UifPG22Function() */
#define CID_PG22ENTERALPHA      59              /* UifPG22EnterALPHA() */
#define CID_PG22ISSUERPT        60              /* UifPG22IssueRpt() */
#define CID_PG22ENTERADDR       61              /* UifPG22EnterAddr() */
#define CID_PG23FUNCTION        62              /* UifPG23Function() */
#define CID_PG23ENTERALPHA      63              /* UifPG23EnterALPHA() */
#define CID_PG23ISSUERPT        64              /* UifPG23IssueRpt() */
#define CID_PG23ENTERADDR       65              /* UifPG23EnterAddr() */
#define CID_PG39FUNCTION        66              /* UifPG39Function() */
#define CID_PG39ENTERALPHA      67              /* UifPG39EnterALPHA() */
#define CID_PG39ISSUERPT        68              /* UifPG39IssueRpt() */
#define CID_PG39ENTERADDR       69              /* UifPG39EnterAddr() */
#define CID_PG46FUNCTION        70              /* UifPG46Function() */
#define CID_PG46ENTERALPHA      71              /* UifPG46EnterALPHA() */
#define CID_PG46ISSUERPT        72              /* UifPG46IssueRpt() */
#define CID_PG46ENTERADDR       73              /* UifPG46EnterAddr() */
#define CID_PG47FUNCTION        74              /* UifPG47Function() */
#define CID_PG47ENTERALPHA      75              /* UifPG47EnterALPHA() */
#define CID_PG47ISSUERPT        76              /* UifPG47IssueRpt() */
#define CID_PG47ENTERADDR       77              /* UifPG47EnterAddr() */
#define CID_PG48FUNCTION        78              /* UifPG48Function() */
#define CID_PG48ENTERALPHA      79              /* UifPG48EnterALPHA() */
#define CID_PG48ISSUERPT        80              /* UifPG48IssueRpt() */
#define CID_PG48ENTERADDR       81              /* UifPG48EnterAddr() */
#define CID_PG49FUNCTION        82              /* UifPG49Function() */
#define CID_PG49ISSUERPT        83              /* UifPG49IssueRpt() */
#define CID_PG49ENTERADDR       84              /* UifPG49EnterAddr() */
#define CID_PG49ENTERDATA       85              /* UifPG49EnterData() */
#define CID_PG50FUNCTION        86              /* UifPG50Function() */
#define CID_PG50ISSUERPT        87              /* UifPG50IssueRpt() */
#define CID_PG50ENTERADDR       88              /* UifPG50EnterAddr() */
#define CID_PG50ENTERDATA       89              /* UifPG50EnterData() */
#define CID_PG57FUNCTION        90              /* UifPG57Function() */
#define CID_PG57ENTERALPHA      91              /* UifPG57EnterALPHA() */
#define CID_PG57ISSUERPT        92              /* UifPG57IssueRpt() */
#define CID_PG57ENTERADDR       93              /* UifPG57EnterAddr() */
#define CID_PG60FUNCTION        94              /* UifPG60Function() */
#define CID_PG60ISSUERPT        95              /* UifPG60IssueRpt() */
#define CID_PG60ENTERADDR       96              /* UifPG60EnterAddr() */
#define CID_PG60ENTERDATA       97              /* UifPG60EnterData() */
#define CID_PG61FUNCTION        98              /* UifPG61Function() */
#define CID_PG61ISSUERPT        99              /* UifPG61IssueRpt() */
#define CID_PG61ENTERADDR       100             /* UifPG61EnterAddr() */
#define CID_PG61ENTERFIELD      101             /* UifPG61EnterField() */
#define CID_PG61ENTERDATA       102             /* UifPG61EnterData() */
#define CID_PGCHANGEKB1         103             /* UifPGChangeKB1() */
#define CID_PGCHANGEKB2         104             /* UifPGChangeKB2() */
#define CID_PGSHIFT             105             /* UifPGShift() */
#define CID_PGEXIT              106             /* UifPGExit() */
#define CID_PG54FUNCTION        107             /* UifPG54Function */
#define CID_PG54ENTERALPHA      108             /* UifPG54EnterALPHA() */
#define CID_PG54ISSUERPT        109             /* UifPG54IssueRpt() */
#define CID_PG54ENTERADDR       110             /* UifPG54EnterAddr() */
#define CID_PG3ENTERPAGE        111             /* UifPG3EnterPage() */
#define CID_PG62FUNCTION        112             /* UifPG62Function() */
#define CID_PG62ISSUERPT        113             /* UifPG62IssueRpt() */
#define CID_PG62ENTERADDR       114             /* UifPG62EnterAddr() */
#define CID_PG62ENTERDATA       115             /* UifPG62EnterData() */
#define CID_PG65FUNCTION        116             /* UifPG65Function() */
#define CID_PG65ENTERALPHA      117             /* UifPG65EnterALPHA() */
#define CID_PG65ISSUERPT        118             /* UifPG65IssueRpt() */
#define CID_PG65ENTERADDR       119             /* UifPG65EnterAddr() */
#define CID_PG51FUNCTION        120             /* UifPG51Function(), 2172 */
#define CID_PG51ISSUERPT        121             /* UifPG51IssueRpt(), 2172 */
#define CID_PG51ENTERADDR       123             /* UifPG51EnterAddr(), 2172 */
#define CID_PG51ENTERDATA       124             /* UifPG51EnterData(), 2172 */
#define CID_DIAPGMNEM           125             /* Prog#3 PLU Mnem, Saratoga */
#define CID_PG99FUNCTION        126             /* UifPG99Function(), 2172 */
#define CID_PG99ISSUERPT        127             /* UifPG99IssueRpt(), 2172 */
#define CID_PG99ENTERADDR       128             /* UifPG99EnterAddr(), 2172 */
#define CID_PG99ENTERDATA       129             /* UifPG99EnterData(), 2172 */
#define CID_PG99GOTOREBOOT      130             /* UifPG99GoToReboot(), 2172 */

#define CID_PG95FUNCTION        131             /* UifPG95Function(), 2172 */
#define CID_PG95ISSUERPT        132             /* UifPG95IssueRpt(), 2172 */
#define CID_PG95ENTERADDR       133             /* UifPG99EnterAddr(), 2172 */
#define CID_PG95ENTERDATA       134             /* UifPG95EnterData(), 2172 */

#define CID_PG67FUNCTION        135              /* UifPG67Function() */
#define CID_PG67ISSUERPT        136              /* UifPG67IssueRpt() */
#define CID_PG67ENTERADDR       137              /* UifPG67EnterAddr() */
#define CID_PG67ENTERDATA       138              /* UifPG67EnterData() */

/*
*---------------------------------------------------------------------------
*   Dialog Return Status
*---------------------------------------------------------------------------
*/
#define UIPG_DIA_SUCCESS        -1              /* dialog success */
#define UIPG_ABORT              -2              /* cancel dialog */

/*
*===========================================================================
*   Dialog Interface                                        Saratoga
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {                                /* Dialog output */
    UCHAR auchFsc[2];                           /* FSC */
    ULONG ulData;                               /* input Data */
    TCHAR aszMnemonics[19 + 1];                 /* alpha input data */
} UIFPGDIADATA;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*------------------------------------------------------------------------*\
                                 
             P R O T O T Y P E     D E C L A R A T I O N s
                                 
\*------------------------------------------------------------------------*/
            
SHORT   UifPGMode(KEYMSG *pKeyMsg);
SHORT   UifPGModeIn(KEYMSG *pKeyMsg);
SHORT   UifPGEnter(KEYMSG *pKeyMsg);
SHORT   UifPG1Function(KEYMSG *pKeyMsg);
SHORT   UifPG1IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG1EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG1EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG2Function(KEYMSG *pKeyMsg);
SHORT   UifPG2IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG2EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG2EnterData1(KEYMSG *pKeyMsg);
SHORT   UifPG2EnterData2(KEYMSG *pKeyMsg);
SHORT   UifPG3Function(KEYMSG *pKeyMsg);
SHORT   UifPG3IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG3EnterPage(KEYMSG *pKeyMsg);
SHORT   UifPG3EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG3EnterData1(KEYMSG *pKeyMsg);
SHORT   UifPG3EnterData2(KEYMSG *pKeyMsg);
SHORT   UifPGDiaMnem(UIFPGDIADATA *pData);          /* Saratoga */
SHORT   UifPGDiaMnemEntry(KEYMSG *pData);           /* Saratoga */
SHORT   UifPGDiaClear(VOID);                        /* Saratoga */

SHORT   UifPG6Function(KEYMSG *pKeyMsg);
SHORT   UifPG6IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG6EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG6EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG7Function(KEYMSG *pKeyMsg);
SHORT   UifPG7IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG7EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG7EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG8Function(KEYMSG *pKeyMsg);
SHORT   UifPG8IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG8EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG8EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG9Function(KEYMSG *pKeyMsg);
SHORT   UifPG9IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG9EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG9EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG10Function(KEYMSG *pKeyMsg);
SHORT   UifPG10IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG10EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG10EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG15Function(KEYMSG *pKeyMsg);
SHORT   UifPG15IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG15EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG15EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG17Function(KEYMSG *pKeyMsg);
SHORT   UifPG17IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG17EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG17EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG18Function(KEYMSG *pKeyMsg);
SHORT   UifPG18IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG18EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG18EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG20Function(KEYMSG *pKeyMsg);
SHORT   UifPG20EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG20IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG20EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG21Function(KEYMSG *pKeyMsg);
SHORT   UifPG21EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG21IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG21EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG22Function(KEYMSG *pKeyMsg);
SHORT   UifPG22EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG22IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG22EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG23Function(KEYMSG *pKeyMsg);
SHORT   UifPG23EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG23IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG23EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG39Function(KEYMSG *pKeyMsg);
SHORT   UifPG39EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG39IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG39EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG46Function(KEYMSG *pKeyMsg);
SHORT   UifPG46EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG46IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG46EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG47Function(KEYMSG *pKeyMsg);
SHORT   UifPG47EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG47IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG47EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG48Function(KEYMSG *pKeyMsg);
SHORT   UifPG48EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG48IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG48EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG49Function(KEYMSG *pKeyMsg);
SHORT   UifPG49IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG49EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG49EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG50Function(KEYMSG *pKeyMsg);
SHORT   UifPG50IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG50EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG50EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG54Function(KEYMSG *pKeyMsg);
SHORT   UifPG54EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG54IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG54EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG57Function(KEYMSG *pKeyMsg);
SHORT   UifPG57EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG57IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG57EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG60Function(KEYMSG *pKeyMsg);
SHORT   UifPG60IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG60EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG60EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG61Function(KEYMSG *pKeyMsg);
SHORT   UifPG61IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG61EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG61EnterField(KEYMSG *pKeyMsg);
SHORT   UifPG61EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG62Function(KEYMSG *pKeyMsg);
SHORT   UifPG62IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG62EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG62EnterData(KEYMSG *pKeyMsg);
SHORT   UifPG65Function(KEYMSG *pKeyMsg);
SHORT   UifPG65EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifPG65IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG65EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPGDefProc(KEYMSG *pKeyMsg);
SHORT   UifPGChangeKB1(KEYMSG *pKeyMsg);
SHORT   UifPGChangeKB2(KEYMSG *pKeyMsg);
SHORT   UifPGShift(KEYMSG *pKeyMsg);
SHORT   UifPGExit(KEYMSG *pKeyMsg);

SHORT   UifPG51Function(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG51IssueRpt(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG51EnterAddr(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG51EnterData(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG99Function(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG99IssueRpt(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG99EnterAddr(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG99EnterData(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG99GoToReboot(KEYMSG *pKeyMsg); /* 2172 */

SHORT   UifPG95Function(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG95IssueRpt(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG95EnterAddr(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifPG95EnterData(KEYMSG *pKeyMsg); /* 2172 */

SHORT   UifPG67Function(KEYMSG *pKeyMsg);
SHORT   UifPG67IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifPG67EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifPG67EnterData(KEYMSG *pKeyMsg);

/* --- End of Header --- */
