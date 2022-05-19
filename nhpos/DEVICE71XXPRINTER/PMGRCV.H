/**
;========================================================================
;+                                                                      +
;+              L O C A L    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/
#define PMG_JNL_END     10          /* counter to permit printing       */
#define PMG_SLIP_END    2           /* counter to permit printing       */


/**
;========================================================================
;+                                                                      +
;+              P R O T O T Y P E    D E C L A R A T I O N s            +
;+                                                                      +
;========================================================================
**/
VOID    PmgReceiveRJ(VOID);    /* receive thread for R&J       */
VOID    PmgReceiveS(VOID);     /* receive thread for Slip      */
VOID    RcvChkResponce( USHORT , PRT_RESPONCE *);
VOID    RcvDelSpoolData( USHORT , PRT_RESPONCE *);
USHORT  RcvGetSpoolData( USHORT , SPOOLDAT **);
USHORT  RcvChkSpoolData( VOID );
USHORT  RcvCheckSeqNo( USHORT , UCHAR );
USHORT  RcvWarningNearEnd( USHORT );
VOID    RcvSeqErrRecovery( USHORT , UCHAR );
VOID    RcvCommRecovery( USHORT );
VOID    RcvErrorRecovery( USHORT );
VOID    RcvPowerRecovery( USHORT );
USHORT  RcvSpoolRecovery( USHORT, USHORT* );
USHORT  RcvRecvPrtPoint( USHORT, USHORT* );
SHORT   RcvErrorPrint( USHORT , USHORT , USHORT );
