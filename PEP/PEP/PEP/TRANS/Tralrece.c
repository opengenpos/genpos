#if defined(POSSIBLE_DEAD_CODE)
// This file is not used and is a candidate for deletion.
/*
* ---------------------------------------------------------------------------
* Title         :   Receive from Loader Board
* Category      :   Transfer, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   TRALRECE.C
* Copyright (C) :   1995, AT&T GIS Corp. E&M-OISO, All rights reserved.
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
* Sep-20-95 : 01.00.03 : T.Nakahata : add dummy area at head of loader board.
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
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pif.h>

/* NCR2172 */
#include <pararam.h>

#include "file.h"
#include "trans.h"
#include "transl.h"
#include "traload.h"
#include "tralcomm.h"

/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/
/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    short TransLoaderReceive(HWND hDlg)
*               hDlg:   the handle of Dialog Box
*
** Return:      TRANS_LOADER_OK:    success
*               TRANS_LOADER_ABORT: aborted
*               TRANS_LOADER_FAIL:  failure
*
** Description: This function receives parameter from loader board
*===========================================================================
*/
short TransLoaderReceive(HWND hDlg)
{
    SHORT       sPort, sFile, sReturn;
    ULONG      usSize;
    LPSTR       lpGlobalMem;
    HANDLE      hGlobalMem;
    CHAR    FAR *pchBuff;
    CHAR        i;
    WORD        wCount;
    FLEXMEM     PrevFlexMem[TRANS_FLEX_ADR_MAX];
    FLEXMEM     NewFlexMem[TRANS_FLEX_ADR_MAX];
    SHORT       sFlexRead;
   
	/* open loader board    */
    if ((sPort = TransLoaderCommOpen(hDlg)) < 0) {
        return TRANS_LOADER_FAIL;
    }

    TransAbortCreateDlg(hDlg);
    TransAbortDlgMessage(hDlg, IDS_TRANS_LOADER_LOAD);

    /* set timeout value    */
    if (TransLoaderCommControl((USHORT)sPort) == FALSE) {
        return TRANS_LOADER_FAIL;
    }

    /* begin to receve loader board   */
    if (TransLoaderReceiveDonwload((USHORT)sPort) == FALSE) {
        return TRANS_LOADER_FAIL;
    }

    /* allocate and lock global memory  */
    if ((hGlobalMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                  TRANS_MEMORY_SIZE_LOADER)) == NULL) {

        PifCloseCom((USHORT)sPort);
        return TRANS_LOADER_FAIL;
    }
    if ((lpGlobalMem = GlobalLock(hGlobalMem)) == NULL) {
        GlobalFree(hGlobalMem);
        PifCloseCom((USHORT)sPort);
        return TRANS_LOADER_FAIL;
    }

    /* recveive file name of loader board   */
    if (TransLoaderReceiveRead((USHORT)sPort,
                        lpGlobalMem,
                        TRANS_LOADER_COMM_BLOCK) == FALSE) {

        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_FAIL;
    }

    /* get file size and send index */
    if (TransLoaderReceiveRead((USHORT)sPort,
                        lpGlobalMem,
                        TRANS_MEMORY_SIZE_LOADER) == FALSE) {

        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_FAIL;
    }

    memcpy(aTransLoaderIndex, lpGlobalMem, sizeof(aTransLoaderIndex));

    for (i = 0; aTransLoaderIndex[i].szFileName[0] != 0 && i < TRANS_LOADER_COMM_MAX_FILE; i++) {

        if (TransAbortQueryStatus() == TRUE) {    /* aborted  */

            PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
            TransLoaderCommClose((USHORT)sPort, hGlobalMem);
            return TRANS_LOADER_ABORT;
        }
        
        /* check file name  */
        if ((sReturn = TransLoaderCommCheckFile(aTransLoaderIndex[i].szFileName)) < 0) {
                                        /* invalid file (ex: total files)   */

            TransAbortDlgMessage(hDlg, IDS_TRANS_LOADER_SEEK);
            TransAbortDlgCalcLoadMsg(hDlg, TRANS_MEMORY_SIZE_LOADER, 0);

            /* read loader board    */
            for (; aTransLoaderIndex[i].ulSize > 0L;
                 aTransLoaderIndex[i].ulSize -= TRANS_MEMORY_SIZE_LOADER) {

                if (TransLoaderReceiveRead((USHORT)sPort,
                                    lpGlobalMem,
                                    TRANS_MEMORY_SIZE_LOADER) == FALSE) {

                    TransLoaderCommClose((USHORT)sPort, hGlobalMem);
                    return TRANS_LOADER_FAIL;
                }

                if (aTransLoaderIndex[i].ulSize < TRANS_MEMORY_SIZE_LOADER)   break;
            }
        } else if (sReturn == TRANS_LOADER_COMM_APPL) {  /* application parameter*/

            TransAbortDlgMessage(hDlg, IDS_TRANS_LOADER_LOADAPPL);

            /* ----- read previous flex memory data ----- */
            ParaAllRead(CLASS_PARAFLEXMEM,
                        (UCHAR *)PrevFlexMem,
                        sizeof(PrevFlexMem),
                        0,
                        (USHORT *)&sFlexRead);
/* NCR2172 */
          /* for (pchBuff = (CHAR FAR *)ParaStart, wCount = 0; */
            for (pchBuff = (CHAR FAR *)(&Para), wCount = 0;
                 aTransLoaderIndex[i].ulSize > 0L;
                 aTransLoaderIndex[i].ulSize -= TRANS_MEMORY_SIZE_LOADER) {

                TransAbortDlgCalcLoadMsg(hDlg, 
                                         aTransLoaderIndex[i].ulSize,
                                         wCount++);

                /* read application parameter from loader board */
                if (TransLoaderReceiveRead((USHORT)sPort,
                                    lpGlobalMem,
                                    TRANS_MEMORY_SIZE_LOADER) == FALSE) {

                    TransLoaderCommClose((USHORT)sPort, hGlobalMem);
                    return TRANS_LOADER_FAIL;
                }

                if (aTransLoaderIndex[i].ulSize < TRANS_MEMORY_SIZE_LOADER) {
                    usSize = (USHORT)aTransLoaderIndex[i].ulSize;
                } else {
                    usSize = TRANS_MEMORY_SIZE_LOADER;
                }

                if ( 0 < ( wCount - 1 )) {

                    memcpy(pchBuff, lpGlobalMem, usSize);
                    pchBuff += usSize;

                } else {            /* skip dummy data */

                    memcpy( pchBuff,
                            lpGlobalMem + TRANS_LOADER_DUMMY_SIZE,
                            usSize - TRANS_LOADER_DUMMY_SIZE );
                    pchBuff += ( usSize - TRANS_LOADER_DUMMY_SIZE );
                }

                if (aTransLoaderIndex[i].ulSize < TRANS_MEMORY_SIZE_LOADER)   break;
            }

            /* ----- read new flex memory data ----- */
            ParaAllRead(CLASS_PARAFLEXMEM,
                        (UCHAR *)NewFlexMem,
                        sizeof(NewFlexMem),
                        0,
                        (USHORT *)&sFlexRead);

            /* ----- check and create each files ----- */
            TransCreateFiles(NewFlexMem, PrevFlexMem);

            TransAbortDlgFinMessage(hDlg);

        } else {                        /* temporary files  */

            TransLoaderCommSetMessage(hDlg,
                                  aTransLoaderIndex[i].szFileName,
                                  TRANS_LOADER_COMM_LOAD);

            /* delete old temporary file    */
            PifDeleteFile(aTransLoaderIndex[i].szFileName);

            /* open temporary file  */
            if ((sFile = PifOpenFile(aTransLoaderIndex[i].szFileName, "nw")) < 0) {
                TransLoaderCommClose((USHORT)sPort, hGlobalMem);
                return TRANS_LOADER_FAIL;
            }

/* NCR2172 */
           /* for (pchBuff = (CHAR FAR *)ParaStart, wCount = 0; */
            for (pchBuff = (CHAR FAR *)(&Para), wCount = 0;
                 aTransLoaderIndex[i].ulSize > 0L;
                 aTransLoaderIndex[i].ulSize -= TRANS_MEMORY_SIZE_LOADER) {

                TransAbortDlgCalcLoadMsg(hDlg, 
                                         aTransLoaderIndex[i].ulSize,
                                         wCount++);

                /* read temporary file from loader board    */
                if (TransLoaderReceiveRead((USHORT)sPort,
                                    lpGlobalMem,
                                    TRANS_MEMORY_SIZE_LOADER) == FALSE) {

                    PifCloseFile((USHORT)sFile);
                    TransLoaderCommClose((USHORT)sPort, hGlobalMem);
                    return TRANS_LOADER_FAIL;
                }

                if (aTransLoaderIndex[i].ulSize < TRANS_MEMORY_SIZE_LOADER) {
                    usSize = (USHORT)aTransLoaderIndex[i].ulSize;
                } else {
                    usSize = TRANS_MEMORY_SIZE_LOADER;
                }

                PifWriteFile((USHORT)sFile, lpGlobalMem, usSize);

                if (aTransLoaderIndex[i].ulSize < TRANS_MEMORY_SIZE_LOADER) {
                    break;
                }
            }
            PifCloseFile((USHORT)sFile);
            TransAbortDlgFinMessage(hDlg);
        }
    }

    for (;;) {                          /* end  */
        if ((sReturn = PifReadCom((USHORT)sPort,
                                  lpGlobalMem,
                                  TRANS_MEMORY_SIZE_LOADER)) < 0) {

            TransLoaderCommClose((USHORT)sPort, hGlobalMem);
            return TRANS_LOADER_FAIL;

        } else if (sReturn == TRANS_LOADER_COMM_BLOCK) {

            TransLoaderCommClose((USHORT)sPort, hGlobalMem);

            /* ----- read new flex memory data ----- */
            ParaAllRead(CLASS_PARAFLEXMEM,
                        (UCHAR *)NewFlexMem,
                        sizeof(NewFlexMem),
                        0,
                        (USHORT *)&sFlexRead);

            if ((Trans.fwTransLoader & TRANS_ACCESS_RPT)
            /*NCR2172*/
            /*    && (NewFlexMem[FLEX_PROGRPT_ADR - 1].usRecordNumber != 0)) {*/
                && (NewFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber != 0L)) {
                if ( TransConvertToIniFile(TransAbort.hDlg) == FALSE) {
                     TransMessage(TransAbort.hDlg,
                                  MB_ICONEXCLAMATION,
                                  MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                                  IDS_TRANS_NAME,
                                  IDS_TRANS_ERR_CONVERT);
                    return TRANS_LOADER_ABORT;
                }
            }
            return TRANS_LOADER_OK;

        } else {
            continue;
        }
    }
}

/*
*===========================================================================
** Synopsis:    BOOL TransLoaderReceiveRead(USHORT usPort, VOID FAR *pBuff,
*                                    USHORT usSize)
*               usPort: the handle of serial port
*               pBuff:  the pointer of buffer
*               usSize: the size of buffer
*
** Return:      TRUE:   success
*               FALSE:  failure 
*
** Description: This function receives from loader board.
*===========================================================================
*/
BOOL TransLoaderReceiveRead(USHORT usPort, VOID FAR *pBuff, USHORT usSize)
{
    if (PifReadCom(usPort, pBuff, usSize) != (SHORT)usSize) {
        return FALSE;
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    BOOL TransLoaderReceiveDonwload(USHORT usPort)
*               usPort: the handle of serial port
*
** Return:      TRUE:   sucess 
*               FALSE:  failure
*
** Description: This function begins to load down to loader board
*===========================================================================
*/
BOOL TransLoaderReceiveDonwload(USHORT usPort)
{
    CHAR    szDownLoad[] = "DOWNLOAD\r";
    CHAR    szOk[]       = "\r\nOK\r\n";
    CHAR    achBuff[sizeof(szDownLoad)];

    if (PifControlCom(usPort,
                      PIF_YAM_WRITE,
                      (VOID FAR *)szDownLoad,
                      (USHORT)strlen(szDownLoad)) > 0) {

        if (PifControlCom(usPort,
                         PIF_YAM_READ,
                         (VOID FAR *)achBuff,
                         (USHORT)strlen(szDownLoad)) > 0) {

            if (memcmp(achBuff, szDownLoad, strlen(szDownLoad)) == 0) {

                if (PifControlCom(usPort,
                                 PIF_YAM_READ,
                                 (VOID FAR *)achBuff,
                                 (USHORT)strlen(szOk)) >  0) {

                    if (memcmp(achBuff, szOk, strlen(szOk)) == 0) {
                        return TRUE;
                    }
                }
            }
        }
   }

   PifCloseCom(usPort);
   return FALSE;
}   

/* ===== End of TRALRECV.C ===== */
#endif