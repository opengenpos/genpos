#if defined(POSSIBLE_DEAD_CODE)
// This file is not used and is a candidate for deletion.
/*
* ---------------------------------------------------------------------------
* Title         :   Send to Loader Board
* Category      :   Transfer, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   TRALSEND.C
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
#include <pif.h>

/* NCR2172 */
#include <paraequ.h>
#include <para.h>
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
** Synopsis:    short TransLoaderSend(HWND hDlg)
*               hDlg:   the handle of Dialog Box
*
** Return:      TRANS_LOADER_OK:    success
*               TRANS_LOADER_ABORT: aborted
*               TRANS_LOADER_FAIL:  failure
*
** Description: This function sends parameter to loader board
*===========================================================================
*/
short TransLoaderSend(HWND hDlg)
{
    SHORT       sPort, sFile;
    USHORT      usSize;
    LPSTR       lpGlobalMem;
    HANDLE      hGlobalMem;
    CHAR    FAR *pchBuff;
    CHAR        i = 0;
    WORD        wCount;
    CHAR        achDummy[] = TRANS_LOADER_DUMMY_DATA;

    if (Trans.fwTransLoader & TRANS_ACCESS_RPT) {
        if ( TransConvertToParaFile(hDlg) == FALSE) {
            TransMessage(hDlg,
                         MB_ICONEXCLAMATION,
                         MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL,
                         IDS_TRANS_NAME,
                         IDS_TRANS_ERR_CONVERT);
            return TRANS_LOADER_ABORT;
        }
    }

    /* open loader board    */
    if ((sPort = TransLoaderCommOpen(hDlg)) < 0) {
        return TRANS_LOADER_FAIL;
    }

    TransAbortCreateDlg(hDlg);
    TransAbortDlgMessage(hDlg, IDS_TRANS_LOADER_SAVE);

    /* set timeout value    */
    if (TransLoaderCommControl((USHORT)sPort) == FALSE) {
        return TRANS_LOADER_FAIL;
    }

    /* begin to send loader board   */
    if (TransLoaderSendUpload((USHORT)sPort) == FALSE) {
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

    /* send file name of loader board   */
    strcpy(lpGlobalMem, "BOB.DAT");
    if (TransLoaderSendWrite((USHORT)sPort,
                         lpGlobalMem,
                         TRANS_LOADER_COMM_BLOCK) == FALSE) {

        PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_FAIL;
    }

    /* get file size and send index */
    TransLoaderSendSetIndex();
    memcpy(lpGlobalMem, aTransLoaderIndex, sizeof(aTransLoaderIndex));
    if (TransLoaderSendWrite((USHORT)sPort,
                         lpGlobalMem,
                         TRANS_MEMORY_SIZE_LOADER) == FALSE) {

        PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_FAIL;
    }

    if (TransAbortQueryStatus() == TRUE) {    /* aborted  */

        PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_ABORT;
    }

    /* send application parameter to loader board   */
    TransAbortDlgMessage(hDlg, IDS_TRANS_LOADER_SAVEAPPL);

/* NCR2172 */
  /* for (pchBuff = (CHAR FAR *)ParaStart, wCount = 0; */
    for (pchBuff = (CHAR FAR *)(&Para), wCount = 0;
         aTransLoaderIndex[TRANS_LOADER_COMM_APPL].ulSize > 0;
         aTransLoaderIndex[TRANS_LOADER_COMM_APPL].ulSize -= TRANS_MEMORY_SIZE_LOADER) {

        TransAbortDlgCalcLoadMsg(hDlg, 
                                 aTransLoaderIndex[i].ulSize,
                                 wCount++);

        if(aTransLoaderIndex[TRANS_LOADER_COMM_APPL].ulSize < TRANS_MEMORY_SIZE_LOADER) {
            usSize = (USHORT)aTransLoaderIndex[TRANS_LOADER_COMM_APPL].ulSize;
            memset(lpGlobalMem, 0, TRANS_MEMORY_SIZE_LOADER);
        } else {
            usSize = TRANS_MEMORY_SIZE_LOADER;
        }

        if ( 0 < ( wCount - 1 )) {

            memcpy(lpGlobalMem, pchBuff, usSize);
            pchBuff += TRANS_MEMORY_SIZE_LOADER;

        } else {            /* send head of PARA data and dummy data */

            memcpy( lpGlobalMem, achDummy, TRANS_LOADER_DUMMY_SIZE );
            memcpy( lpGlobalMem + TRANS_LOADER_DUMMY_SIZE,
                    pchBuff,
                    usSize - TRANS_LOADER_DUMMY_SIZE );
            pchBuff += ( usSize - TRANS_LOADER_DUMMY_SIZE );
        }

        if (TransLoaderSendWrite((USHORT)sPort,
									  lpGlobalMem,
									  TRANS_MEMORY_SIZE_LOADER) == FALSE) {

            PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
            TransLoaderCommClose((USHORT)sPort, hGlobalMem);
            return TRANS_LOADER_FAIL;
        }

        if (usSize < TRANS_MEMORY_SIZE_LOADER)  break;  /* end  */
    }
    TransAbortDlgFinMessage(hDlg);

    /* send temporary file to loader board  */
    for (i = 1; (aTransLoaderIndex[i].szFileName[0] != '\0') && (i < TRANS_LOADER_COMM_NO_FILE); i++) {

        if (TransAbortQueryStatus() == TRUE) {    /* aborted  */

            PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
            TransLoaderCommClose((USHORT)sPort, hGlobalMem);
            return TRANS_LOADER_ABORT;
        }

        TransLoaderCommSetMessage(hDlg,
                              aTransLoaderIndex[i].szFileName,
                              TRANS_LOADER_COMM_SAVE);

        /* open temporary file  */
        if ((sFile = PifOpenFile(aTransLoaderIndex[i].szFileName, "or")) < 0) {

            PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
            TransLoaderCommClose((USHORT)sPort, hGlobalMem);
            return TRANS_LOADER_FAIL;
        }

        for (wCount = 0;;) {

            TransAbortDlgCalcLoadMsg(hDlg, 
                                     aTransLoaderIndex[i].ulSize,
                                     wCount++);

            memset(lpGlobalMem, 0, TRANS_MEMORY_SIZE_LOADER);

            /* read temporary file  */
            if((usSize = PifReadFile((USHORT)sFile,
                                     lpGlobalMem,
                                     TRANS_MEMORY_SIZE_LOADER)) == 0) {
                break;
            }

            /* send temparary file to loader board  */
            if (TransLoaderSendWrite((USHORT)sPort,
                                 lpGlobalMem,
                                 TRANS_MEMORY_SIZE_LOADER) == FALSE) {

                PifCloseFile((USHORT)sFile);
                PifControlCom((USHORT)sPort, PIF_YAM_CANCEL);
                TransLoaderCommClose((USHORT)sPort, hGlobalMem);
                return TRANS_LOADER_FAIL;
            }

            if (usSize < TRANS_MEMORY_SIZE_LOADER)    break;
        }

        /* close temporary file */
        PifCloseFile((USHORT)sFile);

        TransAbortDlgFinMessage(hDlg);
    }

    /* end to send to loader board  */
    memset(lpGlobalMem, 0, TRANS_LOADER_COMM_BLOCK);
    if (TransLoaderSendWrite((USHORT)sPort,
                         lpGlobalMem,
                         TRANS_LOADER_COMM_BLOCK) == FALSE) {

        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_FAIL;
    } else {
        TransLoaderCommClose((USHORT)sPort, hGlobalMem);
        return TRANS_LOADER_OK;
    }
}


/*
*===========================================================================
** Synopsis:    BOOL TransLoaderSendWrite(USHORT usPort, VOID FAR *pBuff,
*                                     USHORT usSize)
*               usPort: the handle of serial port
*               pBuff:  the pointer of buffer
*               usSize: the size of buffer
*				 
** Return:      TRUE:   success
*               FALSE:  failure 
*
** Description: This function sends to loader board.
*===========================================================================
*/
BOOL TransLoaderSendWrite(USHORT usPort, VOID FAR *pBuff, USHORT usSize)
{
    if (PifWriteCom(usPort, pBuff, usSize) != (SHORT)usSize) {
        return FALSE;
    }

    return TRUE;
}


/*
*===========================================================================
** Synopsis:    BOOL TransLoaderSendUpload(USHORT usPort)
*               usPort: the handle of serial port
*
** Return:      TRUE:   sucess 
*               FALSE:  failure
*
** Description: This function begins to load to loader board
*===========================================================================
*/
BOOL TransLoaderSendUpload(USHORT usPort)
{
    CHAR    szUpLoad[] = "UPLOAD\r";
    CHAR    szOk[]     = "\r\nOK\r\n";
    CHAR    achBuff[sizeof(szUpLoad)];

    if (PifControlCom(usPort,
                      PIF_YAM_WRITE,
                      (VOID FAR *)szUpLoad,
                      (USHORT)strlen(szUpLoad)) > 0) {

        if (PifControlCom(usPort,
                         PIF_YAM_READ,
                         (VOID FAR *)achBuff,
                         (USHORT)strlen(szUpLoad)) > 0) {

            if (memcmp(achBuff, szUpLoad, strlen(szUpLoad)) == 0) {

                if (PifControlCom(usPort,
                                  PIF_YAM_READ,
                                  (VOID FAR *)achBuff,
                                  (USHORT)strlen(szOk)) > 0) {

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


/*
*===========================================================================
** Synopsis:    VOID TransLoaderSendSetIndex(VOID)
*
** Return:      none 
*
** Description: This function sets file name and size to index
*===========================================================================
*/
VOID TransLoaderSendSetIndex(VOID)
{
    CHAR        i1, i2;
    SHORT       sFile;
    ULONG       ulSize;
    VOID    FAR *pStart;
/*** NCR2172
    VOID    FAR *pEnd;
 ***/

    memset(aTransLoaderIndex, 0, sizeof(aTransLoaderIndex));

    /* application parameter    */
    strcpy(aTransLoaderIndex[0].szFileName, pszTransName[TRANS_LOADER_COMM_APPL]);
/*** NCR2172
    pStart = (VOID FAR *)ParaStart;
    pEnd   = (VOID FAR *)ParaEnd;

    aTransLoaderIndex[0].ulSize = (ULONG)(FP_OFF(pEnd) - FP_OFF(pStart));
 ***/
    pStart = (VOID FAR *)(&Para);

    aTransLoaderIndex[0].ulSize = (ULONG)(sizeof(Para));

    /* --- add dummy area for loaderboard (R3.0) --- */

    aTransLoaderIndex[ 0 ].ulSize += ( ULONG )TRANS_LOADER_DUMMY_SIZE;

    for (i1 = i2 = 1; i1 < TRANS_LOADER_COMM_NO_FILE; i1++) {

        if (TransLoaderCommCheckFile(pszTransName[i1]) <= 0) {
            continue;
        }

        /* open temporary file  */
        if ((sFile = PifOpenFile(pszTransName[i1], "or")) < 0) {
            continue;
        }

        /* get temporary file size  */
        PifSeekFile((USHORT)sFile, 0x7fffffff, &ulSize);
        if (ulSize > 0L) {
            
            /* set temporary file name  */
            strcpy(aTransLoaderIndex[i2].szFileName, pszTransName[i1]);

            /* set temporary file size  */
            aTransLoaderIndex[i2++].ulSize = ulSize;
        }

        PifCloseFile((USHORT)sFile);

    }
}
        
/* ===== End of TRALSEND.C ===== */
#endif