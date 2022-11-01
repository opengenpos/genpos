/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.3              ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Header file for user
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSPRGRPT.H
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date        Ver.Rev  : NAME        : Description
*   Jan-16-95 : 00.00.01 : T.Nakahata  : Initial
*   Sep-02-98 : 03.03.00 : A.Mitsui    : V3.3 Initial
*
*   Jan-24-00 : 01.00.00 : K.Yanagida  : Saratoga Initial
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

/* --- internal API function code --- */
#define PROGRPT_EOD             1       /* execute EOD function */
#define PROGRPT_PTD             2       /* execute PTD function */

#define PROGRPT_FILETYPE        1       /* retrieve total file type */
#define PROGRPT_DEVICE          2       /* retrieve device */
#define PROGRPT_LOOPKEY         3       /* retrieve loop key */

/* --- return code for user application (same as CASHIER MODULE) --- */
#define PROGRPT_SUCCESS         0
#define PROGRPT_FILE_NOT_FOUND  -1
#define PROGRPT_NOT_IN_FILE     -3
#define PROGRPT_LOCK            -5    
#define PROGRPT_NO_MAKE_FILE    -6 
#define PROGRPT_DELETE_FILE     -17
#define PROGRPT_COMERROR        -37

/* --- return code for internal --- */
#define PROGRPT_INVALID_PARAM   -40
#define PROGRPT_INVALID_FILE    -41
#define PROGRPT_INVALID_VERSION -42
#define PROGRPT_NO_READ_SIZE    -43
#define PROGRPT_DEVICE_FULL     -50
#define PROGRPT_END_OF_FILE     -51

/*
===========================================================================
    TYPEDEF
===========================================================================
*/
typedef struct {
    USHORT  usFileType;                             /* total file type */
    USHORT  usReportNo;                             /* report format no. */
} PROGRPT_EODPTDIF;

/*
===========================================================================
    EXTERNAL VARIABLE
===========================================================================
*/

/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*----------------------
    USER I/F
----------------------*/

VOID    ProgRptInitialize( VOID );
SHORT   ProgRptCreate( UCHAR uchNoOfBlock );
SHORT   ProgRptDelete();

/*********************
SHORT   ProgRptCheckAndCreate( UCHAR uchNoOfBlock );
SHORT   ProgRptCheckAndDelete( UCHAR uchNoOfBlock );


SHORT   ProgRptSendFile( VOID );
SHORT   ProgRptRcvFile( UCHAR  *puchRcvData,
                                 USHORT usRcvLen,
                                 UCHAR  *puchSndData,
                                 USHORT *pusSndLen);


SHORT   ProgRptGetMLDReportNo( UCHAR uchProgRptNo,
                                        UCHAR *puchMLDRptNo );


SHORT   ProgRptGetReportInfo( USHORT usInfoType,
                                       UCHAR  uchProgRptNo,
                                       USHORT *pusInfo );

#define ProgRptGetTtlFileType( uchProgRptNo, pusInfo ) \
        ProgRptGetReportInfo( PROGRPT_FILETYPE, uchProgRptNo, pusInfo )

#define ProgRptGetTtlFileDevice( uchProgRptNo, pusInfo ) \
        ProgRptGetReportInfo( PROGRPT_DEVICE, uchProgRptNo, pusInfo )

#define ProgRptGetTtlFileLoopKey( uchProgRptNo, pusInfo ) \
        ProgRptGetReportInfo( PROGRPT_LOOPKEY, uchProgRptNo, pusInfo )

SHORT   ProgRptGetEODPTDInfo( USHORT           usReportType,
                                       USHORT           *pusNoOfRpt,
                                       PROGRPT_EODPTDIF *paEODPTDRec );

#define ProgRptGetEODInfo( pusNoOfRpt, paEODRec ) \
        ProgRptGetEODPTDInfo( PROGRPT_EOD, pusNoOfRpt, paEODRec )

#define ProgRptGetPTDInfo( pusNoOfRpt, paEODRec ) \
        ProgRptGetEODPTDInfo( PROGRPT_PTD, pusNoOfRpt, paEODRec )


SHORT   ProgRptDeptRead( UCHAR  uchProgRptNo,
                                  UCHAR  uchDailyPTD,
                                  USHORT usDeptNo,
                                  UCHAR  uchMjrDeptNo );

SHORT   ProgRptPluRead( UCHAR uchProgRptNo,
                                 UCHAR uchDailyPTD,
                                 UCHAR  *auchPluNo,
                                 USHORT usDeptNo,
                                 UCHAR uchReportCode );

SHORT   ProgRptPluReset( UCHAR uchProgRptNo,
                                  UCHAR uchDailyPTD,
                                  UCHAR uchReportType );

SHORT   ProgRptCpnRead( UCHAR uchProgRptNo,
                                 UCHAR uchDailyPTD,
                                 UCHAR uchCouponNo );

SHORT   ProgRptCpnReset( UCHAR uchProgRptNo,
                                  UCHAR uchDailyPTD,
                                  UCHAR uchReportType );

SHORT   ProgRptCasRead( UCHAR  uchProgRptNo,
                                 UCHAR  uchDailyPTD,
                                 USHORT usCashierNo );

SHORT   ProgRptCasReset( UCHAR uchProgRptNo,
                                  UCHAR  uchDailyPTD,
                                  UCHAR uchReportType,
                                  USHORT usCashierNo );

SHORT   ProgRptWaiRead( UCHAR  uchProgRptNo,
                                 UCHAR  uchDailyPTD,
                                 USHORT usWaiterNo );

SHORT   ProgRptWaiReset( UCHAR uchProgRptNo,
                                  UCHAR uchDailyPTD,
                                  UCHAR uchReportType,
                                  USHORT usWaiterNo );

SHORT   ProgRptFinRead( UCHAR  uchProgRptNo,
                                 UCHAR  uchDailyPTD );

SHORT   ProgRptIndFinRead( UCHAR  uchProgRptNo,
                                    USHORT usTerminalNo );

SHORT   ProgRptIndFinReset( UCHAR uchProgRptNo,
                                     UCHAR uchReportType,
                                     USHORT usTerminalNo );

SHORT   ProgRptEODReset( UCHAR  uchReportType,
                                  PROGRPT_EODPTDIF *paEODRec,
                                  USHORT usNoOfRpt );

SHORT   ProgRptPTDReset( UCHAR  uchReportType,
                                  PROGRPT_EODPTDIF *paPTDRec,
                                  USHORT usNoOfRpt );

**************/

/* ===== End of File (CSPRGRPT.H) ===== */
