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
* Title       : TOTAL, Total Update Module Subroutine                         
* Category    : TOTAL, NCR 2170 US Hospitarity Application         
* Program Name: Ttlsbpup.c                                                      
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlDeptupdate()        - Update Dept
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. : Name       : Description
* May-06-92 : 00.00.01 : Kterai     :                                    
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
**Synopsis:		SHORT TtlDeptupdate(USHORT usStatus, USHORT usDeptNo,
*                                  LONG lAmount, 
*                                  LONG lCounter, VOID *pTmpBuff)
*
*    Input:		USHORT usStatus         - Affection Status
*               USHORT usDeptNo          - Dept Number
*               UCHAR uchAjectNo        - Ajective Number
*               LONG lAmount            - Update Amount
*               LONG lCounter           - Update Counter
*               VOID *pTmpBuff          - Pointer of Work Buffer
*
*   Output:		Nothing
*    InOut:     Nothing
*
** Return:		TTL_SUCCESS         - Dept Update Seccess  
*				TTL_FILE_HANDLE_ERR	- Occured File Handle Error
*				TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*				TTL_SIZEOVER	    - Max Size Over
*
** Description  This function affects Dept Total.
*
*============================================================================
*/
SHORT TtlDeptupdate(USHORT usStatus, USHORT usDeptNumber,
                    DCURRENCY lAmount, LONG lCounter, VOID *pTmpBuff)
{
    SHORT   sRetStat;
    ULONG   ulPositionMsc;
    ULONG   ulPositionTtl;
    SHORT   sRetvalue;

	if (usStatus & TTL_TUP_IGNORE) return (TTL_SUCCESS);

	{
	TTLDEPT  DeptTtl = {0};

	/* Current Daily Dept Update */
    DeptTtl.uchMajorClass = CLASS_TTLDEPT;    /* Set Dept Class */
    DeptTtl.uchMinorClass = CLASS_TTLCURDAY;  /* Set Current Daily Class */
    DeptTtl.usDEPTNumber = usDeptNumber;
    sRetStat = TtlreadDept(&DeptTtl, pTmpBuff, &ulPositionMsc, &ulPositionTtl, TTL_SEARCH_GT);    /* Read Current Dept Total Data */
	switch (sRetStat) {
    case TTL_SUCCESS:               /* Exist Dept Total File */
        break;

    case TTL_NOTINFILE:             /* Not Exist Dept File */
        if ((sRetvalue = TtlWriteFile(hsTtlDeptHandle, ulPositionTtl, &DeptTtl.DEPTTotal, sizeof(TOTAL))) != 0) { /* Clear the Dept Total Data */
            return (sRetvalue);     /* Return Error Status */
	    }
        break;
    default:
        /*return (sRetvalue);*/
        return (sRetStat);         /* Return Error Status *//* ### Mod (2171 for Win32) V1.0 */
    }
    DeptTtl.DEPTTotal.lAmount += lAmount;         /* Update Dept Total Amount */
    DeptTtl.DEPTTotal.lCounter += lCounter;       /* Update Dept Total Counter */ 
    if ((usStatus & (TTL_TUP_CREDT + TTL_TUP_HASH)) == 0) {   /* Check Credit Dept/PLU */
        DeptTtl.DEPTAllTotal.lAmount += lAmount;                  /* Update All Dept Total Amount */
        if (usStatus & TTL_TUP_SCALE) {
            if (lCounter < 0L) {
                DeptTtl.DEPTAllTotal.lCounter += -1L;             /* Update All Dept Total Counter */ 
            } else {
                DeptTtl.DEPTAllTotal.lCounter += 1L;              /* Update All Dept Total Counter */ 
            }
        } else {
            DeptTtl.DEPTAllTotal.lCounter += lCounter;            /* Update All Dept Total Counter */ 
        }
    }

	/* --- Write Dept Total --- */
    if ((sRetvalue = TtlTwriteDept(&DeptTtl, ulPositionMsc, ulPositionTtl)) != 0) {  /* Write Current Dept Total Data */
        return (sRetvalue);         /* Return Error Status */
    }
	}

	/* Current PTD Dept Update */
	{
	TTLDEPT  DeptTtl = { 0 };

    DeptTtl.uchMajorClass = CLASS_TTLDEPT;/* Set Dept Class */
    DeptTtl.uchMinorClass = CLASS_TTLCURPTD;/* Set Current PTD Class */
    DeptTtl.usDEPTNumber = usDeptNumber;
    sRetStat = TtlreadDept(&DeptTtl, pTmpBuff, &ulPositionMsc, &ulPositionTtl, TTL_SEARCH_GT);  /* Read Current Dept Total Data */
    switch (sRetStat) {
    case TTL_SUCCESS:               /* Exist Dept Total File */
        break;

    case TTL_NOTINFILE:             /* Not Exist Dept File */
        if ((sRetvalue = TtlWriteFile(hsTtlDeptHandle, ulPositionTtl, &DeptTtl.DEPTTotal, sizeof(TOTAL))) != 0) {  /* Clear the Dept Total Data */
            return (sRetvalue);     /* Return Error Status */
	    }
        break;
    case TTL_NOTPTD:
	    return (TTL_SUCCESS);       /* Return */
    default:
        /*return (sRetvalue);*/
        return (sRetStat);         /* Return Error Status *//* ### Mod (2171 for Win32) V1.0 */
    }
    DeptTtl.DEPTTotal.lAmount += lAmount;        /* Update Dept Total Amount */
    DeptTtl.DEPTTotal.lCounter += lCounter;      /* Update Dept Total Counter */ 

    if ((usStatus & (TTL_TUP_CREDT + TTL_TUP_HASH)) == 0) {   /* Check Credit Dept/Dept */
        DeptTtl.DEPTAllTotal.lAmount += lAmount;                  /* Update All Dept Total Amount */
        if (usStatus & TTL_TUP_SCALE) {
            if (lCounter < 0L) {
                DeptTtl.DEPTAllTotal.lCounter += -1L;             /* Update All Dept Total Counter */ 
            } else {
                DeptTtl.DEPTAllTotal.lCounter += 1L;              /* Update All Dept Total Counter */ 
            }
        } else {
            DeptTtl.DEPTAllTotal.lCounter += lCounter;            /* Update All Dept Total Counter */ 
        }
    }
    if ((sRetvalue = TtlTwriteDept(&DeptTtl, ulPositionMsc, ulPositionTtl)) != 0) {    /* Write Current Dept Total Data */
        return (sRetvalue);         /* Return Error Status */
    }
	}

	return (TTL_SUCCESS);	        /* Return Success */
}
