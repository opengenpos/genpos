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
* Title       : TOTAL, Total Function Library Subroutine                         
* Category    : TOTAL, NCR 2170 US Hospitarity Application         
* Program Name: Ttlumsub.c                                                      
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:K.Terai    :                                    
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : removed __DEL_2172 code for PLU totals.
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
#include <paraequ.h>
#include <csttl.h>
#include <ttl.h>    

/*
*============================================================================
**Synopsis:     SHORT TtlTwriteDept(TTLDEPT *pTotal, ULONG ulPositionMsc,
*                                  ULONG ulPositionTtl)
*
*    Input:     TTLDEPT *pTotal          - Major & Minor Class, Dept Number
*               ULONG  ulPositionMsc    - Target Misc Table Position
*               ULONG  ulPositionTtl    - Target Total Position
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed the writeing Dept Total File 
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function writes the Dept Total File.
*
*============================================================================
*/
SHORT TtlTwriteDept(TTLDEPT *pTotal, ULONG ulPositionMsc, ULONG ulPositionTtl)
{
    TOTAL   DeptTotal;
    SHORT    sRetvalue;

    if (hsTtlDeptHandle < 0) {         /* Check Dept Total File Handle */
        return (TTL_FILE_HANDLE_ERR);  /* Return File Handle Error for hsTtlDeptHandle */
    }

	/* --- Write Dept Total --- */
    DeptTotal.lAmount = pTotal->DEPTTotal.lAmount;
    DeptTotal.sCounter = (DSCOUNTER)pTotal->DEPTTotal.lCounter;
    if ((sRetvalue = TtlWriteFile(hsTtlDeptHandle, ulPositionTtl, &DeptTotal, sizeof(TOTAL))) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

	/* --- Write Dept Gross(All) Total --- */
    if ((sRetvalue = TtlDeptGTotalWrite(ulPositionMsc, &pTotal->DEPTAllTotal)) != 0) {
        return (sRetvalue);         /* Return Error Status */
    }

    return (TTL_SUCCESS);           /* Return Success */
}

