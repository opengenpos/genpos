/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Update module Subroutine
* Category    : TOTAL, NCR 2170 US Hospitarity Application
* Program Name: TtlTimUp.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*           SHORT TtlSerTimeupdate()    - Update Service Time Total File
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Nov-16-95:00.00.01:T.Nakahata : Initial at Release 3.1 Model
* Aug-11-99:03.05.00:K.Iwata    : R3.5 Remove WAITER_MODEL
* Aug-11-99:03.05.00:K.Iwata    : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
#include <tchar.h>
#include <math.h>
#include <ecr.h>
#include <pif.h>
#include <csttl.h>
#include <ttl.h>
#include <appllog.h>


/*
*============================================================================
**Synopsis:     SHORT TtlSerTimeupdate(TOTAL *pTotal,
*                                      UCHAR uchHourOffset,
*                                      UCHAR uchTimeBorder,
*                                      TOTAL *pTmpBuff)
*
*    Input:     TOTAL   *pTotal         - Address of Source Service Time Data
*               UCHAR   uchHourOffset   - Hourly Block Offset   (0 - 23)
*               UCHAR   uchTimeBorder   - Time Block Border     (0 -  2)
*               TOTAL   *pTmpBuff       - Address of Temporary Buffer
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCES          - Successed Updating Service Time Total
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function updates the Service Time Total File.
*
*============================================================================
*/
SHORT TtlSerTimeupdate(TOTAL *pTotal, UCHAR uchHourOffset, UCHAR uchTimeBorder,
                       TOTAL *pTmpBuff)
{
    SHORT   sRetvalue;

    if ((uchHourOffset > 47) || (uchTimeBorder > 2)) {  /* V3.3 */
        return (TTL_SUCCESS);
    }

    /* --- Update Current Daily Service Time Total --- */
    if ((sRetvalue = TtlreadSerTime(CLASS_TTLCURDAY,
                                    uchHourOffset,
                                    uchTimeBorder,
                                    pTmpBuff)) != 0) {  /* Read Daily Service Time */
		PifLog(MODULE_TTL_TUM_SERTIME, LOG_ERROR_TTL_CODE_04);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_SERTIME), (USHORT) abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_SERTIME), (USHORT) __LINE__);
        return (sRetvalue);         /* Return Error Status */
    }

    pTmpBuff->lAmount  += pTotal->lAmount;
    pTmpBuff->sCounter += pTotal->sCounter;

    if ((sRetvalue = TtlwriteSerTime(CLASS_TTLCURDAY,
                                     uchHourOffset,
                                     uchTimeBorder,
                                     pTmpBuff)) != 0) { /* Write Daily Service Time */
		PifLog(MODULE_TTL_TUM_SERTIME, LOG_ERROR_TTL_CODE_04);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_SERTIME), (USHORT) abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_SERTIME), (USHORT) __LINE__);
        return (sRetvalue);         /* Return Error Status */
    }


    /* --- Update Current PTD Service Time Total --- */
    if ((sRetvalue = TtlreadSerTime(CLASS_TTLCURPTD,
                                    uchHourOffset,
                                    uchTimeBorder,
                                    pTmpBuff)) != 0) {  /* Read PTD Service Time */
		PifLog(MODULE_TTL_TUM_SERTIME, LOG_ERROR_TTL_CODE_04);
		PifLog(MODULE_ERROR_NO(MODULE_TTL_TUM_SERTIME), (USHORT) abs(sRetvalue));
		PifLog(MODULE_LINE_NO(MODULE_TTL_TUM_SERTIME), (USHORT) __LINE__);
        return (sRetvalue);         /* Return Error Status */
    }

    pTmpBuff->lAmount  += pTotal->lAmount;
    pTmpBuff->sCounter += pTotal->sCounter;

    sRetvalue = TtlwriteSerTime(CLASS_TTLCURPTD,
                                uchHourOffset,
                                uchTimeBorder,
                                pTmpBuff);              /* Write PTD Hourly Total */

    return (sRetvalue);
}


/* ===== End of File (TtlTimup.c) ===== */
