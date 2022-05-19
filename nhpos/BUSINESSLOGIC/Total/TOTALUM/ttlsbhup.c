/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Update module Subroutine                         
* Category    : TOTAL, NCR 2170 US Hospitarity Application         
* Program Name: Ttlsbuph.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlHourupdate()   - Update Hourly Block
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:Kterai     :                                    
*          :        :           :                                    
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
#include <ecr.h>
#include <pif.h>
#include <csttl.h>
#include <ttl.h>

/*
*============================================================================
**Synopsis:		SHORT TtlHourupdate(HOURLY *pHourlyTotal, UCHAR uchOffset,
*                                  HOURLY *pTmpBuff)
*
*    Input:		UCHAR uchMinorClass     - Minor Class
*               UCHAR uchOffset         - Hourly Block Offset
*               VOID *pTmpBuff          - Pointer of Work Buffer
*
*   Output:		Nothing
*    InOut:     Nothing
*
** Return:		TTL_SUCCES          - Successed Updating Hourly Total File 
*				TTL_FILE_HANDLE_ERR	- Occured File Handle Error
*				TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*               TTL_NOTCLASS        - Set Illigal Class Code 
*
** Description  This function updates the Hourly Total File.
*
*============================================================================
*/
SHORT TtlHourupdate(HOURLY *pHourlyTotal, UCHAR uchOffset, HOURLY *pTmpBuff)
{
    SHORT   sRetvalue, i;

    if (uchOffset > 47) {   /* V3.3 */
        return (TTL_SUCCESS);
    }
/* --- Update Current Daily Hourly Total --- */
    if ((sRetvalue = TtlreadHour(CLASS_TTLCURDAY, uchOffset, 
                                 pTmpBuff)) != 0) {
                                    /* Read Daily Hourly Total */
        return (sRetvalue);         /* Return Error Status */
    }
    pTmpBuff->lHourlyTotal += pHourlyTotal->lHourlyTotal;
                                    /* Update Daily Hourly Total */
    pTmpBuff->lItemproductivityCount += pHourlyTotal->lItemproductivityCount;
                                    /* Update Daily Hourly Item Product Counter */
    pTmpBuff->sDayNoOfPerson += pHourlyTotal->sDayNoOfPerson;
                                    /* Update Daily Hourly No of Person Counter */
    /* V3.3 */
    for (i=0; i< TTL_BONUS_MAX; i++) {
        pTmpBuff->lBonus[i] += pHourlyTotal->lBonus[i];
    }
                                    /* Update Daily Hourly Bonus Total */
    if ((sRetvalue = TtlwriteHour(CLASS_TTLCURDAY, uchOffset, 
                                  pTmpBuff)) != 0) {
                                    /* Write Daily Hourly Total */
        return (sRetvalue);         /* Return Error Status */
    }


/* --- Update Current PTD Hourly Total --- */
    if ((sRetvalue = TtlreadHour(CLASS_TTLCURPTD, uchOffset, 
                                 pTmpBuff)) != 0) {
                                    /* Read PTD Hourly Total */
        return (sRetvalue);         /* Return Error Status */
    }
    pTmpBuff->lHourlyTotal += pHourlyTotal->lHourlyTotal;
                                    /* Update PTD Hourly Total */
    pTmpBuff->lItemproductivityCount += pHourlyTotal->lItemproductivityCount;
                                    /* Update PTD Hourly Item Product Counter */
    pTmpBuff->sDayNoOfPerson += pHourlyTotal->sDayNoOfPerson;
                                    /* Update PTD Hourly No of Person Counter */
    /* V3.3 */
	for (i=0; i<TTL_BONUS_MAX; i++) {
        pTmpBuff->lBonus[i] += pHourlyTotal->lBonus[i];
    }
                                    /* Update PTD Hourly Bonus Total */
    return (TtlwriteHour(CLASS_TTLCURPTD, uchOffset, pTmpBuff));
                                    /* Write PTD Hourly Total */
}

