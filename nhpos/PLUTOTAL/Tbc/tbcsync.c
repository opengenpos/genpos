/*************************************************************************************************
 * 
 *        *=*=*=*=*=*=*=*=*
 *        *  NCR 2172     *             AT&T GIS Corporation, E&M OISO
 *     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 2000-
 *    <|\/~               ~\/|>
 *   _/^\_                 _/^\_
 *
 *************************************************************************************************/
/*========================================================================*\
*   Title        : Totaldata (background/backup) copy module
*   Category     : Plutotal module, NCR2172 Saratoga application
*   Program Name : tbcsync.h
*  ------------------------------------------------------------------------
*   Compiler     : MS-VC++ Ver.6.00 by Microsoft Corp.
*   Options      : 
*  ------------------------------------------------------------------------
*   Abstract     :  TBC sync object manipulation and 
*                :   API <-> THREAD interface routines
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-30-00 : 00.00.01 :m.teraki     :Initial
\*=======================================================================*/

/* includes ------------------------------------------------------------------------------------ */
#include <windows.h>  /* needed for rename file */
#include <string.h>

#include "ecr.h"
#include "tbc.h"
#include "tbcin.h"

/* local typedefs ------------------------------------------------------------------------------ */

/* local macros -------------------------------------------------------------------------------- */

/* public symbols ------------------------------------------------------------------------------ */

/* local symbols ------------------------------------------------------------------------------- */
static LONG             g_lCopyState;  /* zero:idle, nonzero:copying */
                                       /* both api and thread updates this */
static LONG             g_lErrorOccured; /* zero:no error detected, nonzero:error has occured */
static CRITICAL_SECTION g_csCopyState; /* binary semaphoe guards g_lCopyState */
static HANDLE           g_evSync;      /* semaphoe used for syncronize api and thread */
static TBC_COPYSPEC    *g_tbcCopySpec; /* copy specification descriptor */
                                       /* (only one way from api to thread) */

/*************************************************************************************************
 * FUNCTION : TbcSyncInit
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : API calls this for issue request to copy thread
 * HISTORY  :
 *
 *************************************************************************************************/
VOID
TbcSyncInit(VOID)
{
    InitializeCriticalSection(&g_csCopyState);
    g_evSync = CreateEvent(NULL, FALSE, FALSE, NULL);

    /* initialize state flag */
    EnterCriticalSection(&g_csCopyState);
    g_lCopyState  = 0;

    /* initialize error state */
    g_lErrorOccured = 0;
    LeaveCriticalSection(&g_csCopyState);
}

/*************************************************************************************************
 * FUNCTION : TbcCopyRequest
 * INPUT    : tbcCopySpec : copy specification descriptor
 * OUTPUT   : NONE
 * RETURNS  : TBC_RSLT_ACC_CLEAR : request accepted.
 *          : TBC_RSLT_DNY_BUSY  : copy thread is working. (request refused)
 * DESC.    : API calls this to issue request to copy thread
 *************************************************************************************************/
DWORD
TbcCopyRequest(TBC_COPYSPEC *tbcCopySpec)
{
    EnterCriticalSection(&g_csCopyState);
    if (g_lCopyState != 0) {
		LeaveCriticalSection(&g_csCopyState);
        return TBC_RSLT_DNY_BUSY;
    }

    g_tbcCopySpec = tbcCopySpec;

    g_lCopyState = 1;
    LeaveCriticalSection(&g_csCopyState);

    SetEvent(g_evSync);

    return TBC_RSLT_ACC_CLEAR;
}

/*************************************************************************************************
 * FUNCTION : TbcGetThreadState
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : TBC_RSLT_COMPLETE : acceptable next request
 *          : TBC_RSLT_BUSY     : copy thread is working.
 *          : TBC_RSLT_ERROR    : any error has occured in copy thread
 * DESC.    : reads global variables and converts to TBC_RSLT_XXXX
 *************************************************************************************************/
DWORD
TbcGetThreadState(VOID)
{
    DWORD dwRet = TBC_RSLT_COMPLETE;

    /* if copy thread is working override thread-state */
    if (g_lCopyState != 0) {
        dwRet = TBC_RSLT_BUSY;
    }

    /* at last, check if error has occured or not. if error has occured, override thread-state */
    if (g_lErrorOccured != 0) {
        dwRet = TBC_RSLT_ERROR;
    }

    return dwRet;
}

/*************************************************************************************************
 * FUNCTION : TbcWaitCopyRequest
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : copy specification descriptor
 * DESC.    : Wait caller's thread until copy request issued from other thread.
 *************************************************************************************************/
TBC_COPYSPEC *
TbcWaitCopyRequest(VOID)
{
    /* protect global(shared) variable */
    WaitForSingleObject(g_evSync, INFINITE);

    return g_tbcCopySpec;
}

/*************************************************************************************************
 * FUNCTION : TbcCopyComplete
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : The copy thread must call this when copy completed.
 *************************************************************************************************/
VOID
TbcCopyComplete(VOID)
{
    EnterCriticalSection(&g_csCopyState);
    TbcDeInitCopySpec(g_tbcCopySpec);
    g_tbcCopySpec = NULL;

    g_lCopyState = 0;
    LeaveCriticalSection(&g_csCopyState);
}

/*************************************************************************************************
 * FUNCTION : TbcCopyFail
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : The copy thread must call this when copy failed.
 *************************************************************************************************/
VOID
TbcCopyFail(VOID)
{
    EnterCriticalSection(&g_csCopyState);
    TbcDeInitCopySpec(g_tbcCopySpec);
    g_tbcCopySpec = NULL;

    g_lCopyState = 0;
    g_lErrorOccured = 1;
    LeaveCriticalSection(&g_csCopyState);
}

/*************************************************************************************************
 * FUNCTION : TbcDeInitCopySpec
 * INPUT    : tbcCopySpec : copy specification descriptor
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : free all memory block associated to copy specification descriptor
 *************************************************************************************************/
VOID
TbcDeInitCopySpec(TBC_COPYSPEC *tbcCopySpec)
{
    DWORD dwIdx;
    TBC_PATHSPEC *tbcPathSpec;

    if (tbcCopySpec == NULL) {      /* nothing to free */
        return;
    }

    if (tbcCopySpec->tbcPathSpec != TEXT('\0')) {

        tbcPathSpec = tbcCopySpec->tbcPathSpec;

        for (dwIdx = 0; dwIdx < tbcCopySpec->dwCount; dwIdx++, tbcPathSpec++) {
            if (tbcPathSpec == NULL) {
                continue;
            }
            if (tbcPathSpec->szSrcPath != TEXT('\0')) {
                free(tbcPathSpec->szSrcPath);
            }
            if (tbcPathSpec->szSrcName != TEXT('\0')) {
                free(tbcPathSpec->szSrcName);
            }
            if (tbcPathSpec->szDstName != TEXT('\0')) {
                free(tbcPathSpec->szDstName);
            }
            if (tbcPathSpec->szTmpName != TEXT('\0')) {
                free(tbcPathSpec->szTmpName);
            }
            if (tbcPathSpec->lpBuf != NULL) {
                free(tbcPathSpec->lpBuf);
            }
        }

        if (tbcCopySpec->tbcPathSpec != NULL) {
            /* path spec is allocated at once even if dwCount is 10 */
            free(tbcCopySpec->tbcPathSpec);
        }
    }

    if (tbcCopySpec->szDstPath != TEXT('\0')) {
        free(tbcCopySpec->szDstPath);
    }

    if (tbcCopySpec != TEXT('\0')) {
        free(tbcCopySpec);
    }
}
/*************************************************************************************************
 * end of file
 *************************************************************************************************/
