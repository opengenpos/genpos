/**
*===========================================================================
* Update Histories
*
* Data       Ver.      Name         Description
* Dec-06-99  01.00.00  T.Koyama     Initial
*===========================================================================
**/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/**
;========================================================================
;+                                                                      +
;+              L O C A L    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/
#define MAX_PRINT_CHARACTER 512
#define MAX_PRINTER			1

typedef struct _PMBBLK_FILE {
	TCHAR   *ptcFileName;
	FILE    *pFileHandle;
	USHORT   usIndex;
} PMGBLK_FILE, *PPMGBLK_FILE;

typedef struct _PMGBLK {
    LOAD_FUNCTION   Func;                   /* DLL Address */
	DWORD			dwDllLoaded;
	HANDLE			hHandle;
	PMGBLK_FILE     pmgFileInfo;
} PMGBLK, *PPMGBLK;

/**
;========================================================================
;+                                                                      +
;+              P R O T O T Y P E    D E C L A R A T I O N s            +
;+                                                                      +
;========================================================================
**/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/***    End of PMGLIB.H    ***/
