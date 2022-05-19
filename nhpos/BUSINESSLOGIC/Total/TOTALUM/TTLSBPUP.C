
#if defined(POSSIBLE_DEAD_CODE)
// Aug-29-17 : 02.02.02 : R.Chambers : removed __DEL_2172 code for PLU totals.

/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Update Module Subroutine                         
* Category    : TOTAL, NCR 2170 US Hospitarity Application         
* Program Name: Ttlsbpup.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlPLUupdate()        - Update PLU
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. : Name       : Description
* May-06-92 : 00.00.01 : Kterai     :                                    
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-14-99 : 01.00.00 : hrkato     : Saratoga
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : removed __DEL_2172 code for PLU totals.
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
#include	<tchar.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
#include <pif.h>
#include <rfl.h>       
#include <paraequ.h>
#include <csttl.h>
#include <ttl.h>  
#include <regstrct.h>
#include <transact.h>
#include "ttltum.h"

/*
*============================================================================
**Synopsis:		SHORT TtlPLUupdate(USHORT usStatus, UCHAR *puchPLUNo,
*                                  UCHAR uchAjectNo, LONG lAmount, 
*                                  LONG lCounter, VOID *pTmpBuff)
*
*    Input:		USHORT usStatus         - Affection Status
*               USHORT usPLUNo          - PLU Number
*               UCHAR uchAjectNo        - Ajective Number
*               LONG lAmount            - Update Amount
*               LONG lCounter           - Update Counter
*               VOID *pTmpBuff          - Pointer of Work Buffer
*
*   Output:		Nothing
*    InOut:     Nothing
*
** Return:		TTL_SUCCESS         - PLU Update Seccess  
*				TTL_FILE_HANDLE_ERR	- Occured File Handle Error
*				TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*				TTL_SIZEOVER	    - Max Size Over
*
** Description  This function affects PLU Total.                Saratoga
*============================================================================
*/
#ifdef	__DEL_2172	/* 12.29.99 */
SHORT TtlPLUupdate(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo,
                    LONG lAmount, LONG lCounter, VOID *pTmpBuff)
{
    TTLPLU  PLUTtl;
    SHORT   sRetStat;
    ULONG   ulPositionMsc;
    ULONG   ulPositionTtl;
    SHORT   sRetvalue;

/* Current Daily PLU Update */
    memset(&PLUTtl, 0, sizeof(TTLPLU)); /* Clear work */
    PLUTtl.uchMajorClass = CLASS_TTLPLU;/* Set PLU Class */
    PLUTtl.uchMinorClass = CLASS_TTLCURDAY;/* Set Current Daily Class */
    _tcsncpy(PLUTtl.auchPLUNumber, puchPLUNo, OP_PLU_LEN);
    //memcpy(PLUTtl.auchPLUNumber, puchPLUNo, sizeof(PLUTtl.auchPLUNumber));  /* Saratoga */
    if (uchAjectNo) {
        PLUTtl.uchAdjectNo = (UCHAR)(((uchAjectNo - 1) % 5) + 1);
    } else {
        PLUTtl.uchAdjectNo = 1;
    }
    sRetStat = TtlreadPLU(&PLUTtl, pTmpBuff, &ulPositionMsc, &ulPositionTtl, TTL_SEARCH_GT);
                                    /* Read Current PLU Total Data */
    switch (sRetStat) {
    case TTL_SUCCESS:               /* Exist PLU Total File */
        break;

    case TTL_NOTINFILE:             /* Not Exist PLU File */
        if ((sRetvalue = TtlWriteFile(hsTtlPLUHandle, ulPositionTtl,
                                      (VOID *)&PLUTtl.PLUTotal, 
                                      sizeof(TOTAL))) != 0) {
                                    /* Clear the PLU Total Data */
            return (sRetvalue);     /* Return Error Status */
	    }
        break;
    default:
        /*return (sRetvalue);*/
        return (sRetStat);         /* Return Error Status *//* ### Mod (2171 for Win32) V1.0 */
    }
    PLUTtl.PLUTotal.lAmount += lAmount; 
                                    /* Update PLU Total Amount */
    PLUTtl.PLUTotal.lCounter += lCounter;
                                    /* Update PLU Total Counter */ 
    if ((usStatus & (TTL_TUP_CREDT + TTL_TUP_HASH)) == 0) {
                                    /* Check Credit PLU/PLU */
        PLUTtl.PLUAllTotal.lAmount += lAmount; 
                                    /* Update All PLU Total Amount */
        if (usStatus & TTL_TUP_SCALE) {
            if (lCounter < 0L) {
                PLUTtl.PLUAllTotal.lCounter += -1L;
                                    /* Update All PLU Total Counter */ 
            } else {
                PLUTtl.PLUAllTotal.lCounter += 1L;
                                    /* Update All PLU Total Counter */ 
            }
        } else {
            PLUTtl.PLUAllTotal.lCounter += lCounter;
                                    /* Update All PLU Total Counter */ 
        }
    }
/* --- Write PLU Total --- */

    if ((sRetvalue = TtlTwritePLU(&PLUTtl, ulPositionMsc, 
                                  ulPositionTtl)) != 0) {
                                    /* Read Current PLU Total Data */
        return (sRetvalue);         /* Return Error Status */
    }

/* Current PTD PLU Update */
    memset(&PLUTtl, 0, sizeof(TTLPLU)); /* Clear work */
    PLUTtl.uchMajorClass = CLASS_TTLPLU;/* Set PLU Class */
    PLUTtl.uchMinorClass = CLASS_TTLCURPTD;/* Set Current PTD Class */
    _tcsncpy(PLUTtl.auchPLUNumber, puchPLUNo, OP_PLU_LEN);
    //memcpy(PLUTtl.auchPLUNumber, puchPLUNo, sizeof(PLUTtl.auchPLUNumber));  /* Saratoga */
    if (uchAjectNo) {
        PLUTtl.uchAdjectNo = (UCHAR)(((uchAjectNo - 1) % 5) + 1);
    } else {
        PLUTtl.uchAdjectNo = 1;
    }
    sRetStat = TtlreadPLU(&PLUTtl, pTmpBuff, &ulPositionMsc, &ulPositionTtl, TTL_SEARCH_GT);
                                    /* Read Current PLU Total Data */
    switch (sRetStat) {
    case TTL_SUCCESS:               /* Exist PLU Total File */
        break;

    case TTL_NOTINFILE:             /* Not Exist PLU File */
        if ((sRetvalue = TtlWriteFile(hsTtlPLUHandle, ulPositionTtl,
                                      (VOID *)&PLUTtl.PLUTotal, 
                                      sizeof(TOTAL))) != 0) {
                                    /* Clear the PLU Total Data */
            return (sRetvalue);     /* Return Error Status */
	    }
        break;
    case TTL_NOTPTD:
	    return (TTL_SUCCESS);       /* Return */
    default:
        /*return (sRetvalue);*/
        return (sRetStat);         /* Return Error Status *//* ### Mod (2171 for Win32) V1.0 */
    }
    PLUTtl.PLUTotal.lAmount += lAmount; 
                                    /* Update PLU Total Amount */
    PLUTtl.PLUTotal.lCounter += lCounter;
                                    /* Update PLU Total Counter */ 

    if ((usStatus & (TTL_TUP_CREDT + TTL_TUP_HASH)) == 0) {
                                    /* Check Credit PLU/PLU */
        PLUTtl.PLUAllTotal.lAmount += lAmount; 
                                    /* Update All PLU Total Amount */
        if (usStatus & TTL_TUP_SCALE) {
            if (lCounter < 0L) {
                PLUTtl.PLUAllTotal.lCounter += -1L;
                                    /* Update All PLU Total Counter */ 
            } else {
                PLUTtl.PLUAllTotal.lCounter += 1L;
                                    /* Update All PLU Total Counter */ 
            }
        } else {
            PLUTtl.PLUAllTotal.lCounter += lCounter;
                                    /* Update All PLU Total Counter */ 
        }
    }
    if ((sRetvalue = TtlTwritePLU(&PLUTtl, ulPositionMsc,
                                  ulPositionTtl)) != 0) {
                                    /* Write Current PLU Total Data */
        return (sRetvalue);         /* Return Error Status */
    }

	return (TTL_SUCCESS);	        /* Return Success */
}
#endif	/* __DEL_2172 */

/* --- End of Source File --- */
#endif