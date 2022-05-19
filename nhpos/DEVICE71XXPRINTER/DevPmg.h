/**
*===========================================================================
* Update Histories
*
* Data       Ver.      Name         Description
* Dec-03-99  1.0       T.Koyama     Initial
*
*===========================================================================
**/

#include "DeviceIOPrinter.h"

/**
;========================================================================
;+                                                                      +
;+              P R O T O T Y P E    D E C L A R A T I O N s            +
;+                                                                      +
;========================================================================
**/

VOID    DevPmgInitialize( VOID );
VOID    DevPmgFinalize( VOID );
BOOL    DevPmgOpen( VOID );
VOID    DevPmgClose( VOID );
USHORT  DevPmgPrint( USHORT, UCHAR *, USHORT);
VOID    DevPmgWait( VOID );
USHORT  DevPmgBeginValidation( USHORT );
USHORT  DevPmgEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  DevPmgBeginSmallValidation( USHORT );
USHORT  DevPmgEndSmallValidation( USHORT );
USHORT  DevPmgFeed( USHORT, USHORT );
USHORT  DevPmgBeginImportant( USHORT );
USHORT  DevPmgEndImportant( USHORT );
USHORT  DevPmgGetStatus( USHORT, UCHAR * );
USHORT  DevPmgPrtConfig(USHORT, USHORT *, UCHAR *);
#ifdef  TPOS
USHORT  DevPmgFontLoad( VOID );
#else
USHORT  DevPmgInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  DevPmgFont( USHORT, USHORT );
#endif
VOID    DevPmgSetValWaitCount( USHORT );
VOID    DevPmgSetCallBack( PCALL_BACK );

HANDLE  DevGetPrtHandle();
BOOL    DevCheckPrtHandle(SHORT handle);

/***    End of PMG.H    ***/
