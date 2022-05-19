/**
;========================================================================
;+                                                                      +
;+              L O C A L    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/

/*************************************************/
/**         communication to printer            **/
/*************************************************/


/**
;========================================================================
;+                                                                      +
;+              P R O T O T Y P E    D E C L A R A T I O N s            +
;+                                                                      +
;========================================================================
**/
VOID    PmgSendRJ( VOID );
VOID    PmgSendS( VOID );
VOID    SndPrintFunctionRJ( UCHAR , UCHAR , SPOOLDAT *, SPOOLDAT *);
VOID    SndPrintFunctionS( SPOOLDAT *);
VOID    SndSpecialFunction( USHORT, SPOOLDAT *);
SHORT   SndPrintRJ( USHORT , SPOOLDAT *);
SHORT   SndBeginValidation( USHORT );
VOID    SndEndValidation( USHORT );
VOID    SndBeginImportant( USHORT );
VOID    SndEndImportant( USHORT );
SHORT   SndFontType( USHORT, UCHAR, SPOOLDAT *);
UCHAR   SndGetSpoolData( USHORT, SPOOLDAT **);
USHORT  SndChkSpoolDataID( USHORT, UCHAR );
VOID    SndNextSpoolPtr( USHORT , UCHAR );
SHORT   PmgSendData( UCHAR, UCHAR, SPOOLDAT *, SPOOLDAT *);
SHORT   SndOpenPlaten( USHORT, USHORT );
SHORT   SndClosePlaten( USHORT );
VOID    SndCommError( USHORT );
USHORT  SndChkEsc(SPOOLDAT *);
VOID    SndPrtTally(USHORT, SPOOLDAT *);
