/*
************************************************************************************
**                                                                                **
**        *=*=*=*=*=*=*=*=*                                                       **
**        *  NCR 2170     *             NCR Corporation, E&M OISO                 **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992                    **
**    <|\/~               ~\/|>                                                   **
**   _/^\_                 _/^\_                                                  **
**                                                                                **
************************************************************************************
*===================================================================================
* Title       : Display Module                         
* Category    : Display, NCR 2170 US Hospitality Application         
* Program Name: DSPDSCC2.C
* ----------------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               DispRegCuDscrCntr() : control descriptor ON/OFF
*               DispRegSetCuStat() : set current descriptor status
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 :  R.Itoh   : initial                                   
* Dec-12-95: 03.01.00 :  M.Ozawa  : enhanced to 2x20 customer display
*===================================================================================
*===================================================================================
* PVCS Entry
* ----------------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===================================================================================
*/
#include	<tchar.h>

#include <ecr.h>
#include <pif.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <display.h>
#include "disp20.h"


/*                                       
*===========================================================================
** Synopsis:    VOID DispRegSetCuStat(VOID);      
*     Input:    nothing    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UCHAR uchDispRegCuCurrStat    
*
** Description: This function sets current descriptor status to the buffer.  
*===========================================================================
*/
static UCHAR DispRegSetCuStat(VOID)       
{   
    UCHAR  uchDispRegCuCurrStat = 0;

    if (flDispRegDescrControl & TOTAL_10CU_CONTROL) {                 
        uchDispRegCuCurrStat |= TOTAL_10CU_STATUS;       
    }                                           /* Status Flag On */
    if (flDispRegDescrControl & CHANGE_10CU_CONTROL) {                 
        uchDispRegCuCurrStat |= CHANGE_10CU_STATUS;       
    }                                           /* Status Flag On */
    if (flDispRegDescrControl & MINUS_10CU_CONTROL) {                 
        uchDispRegCuCurrStat |= MINUS_10CU_STATUS;       
    }                                           /* Status Flag On */

    return (uchDispRegCuCurrStat);
}

/*
*===========================================================================
** Synopsis:    VOID DispRegCuDscrCntr( REGDISPMSG *pData );   
*     Input:    *pData      - Pointer of the Display Data Buffer
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This function compares current descriptor status with old    
*               status and turns the descriptors on/off according to the   
*               status change. These indicators are the ones displayed on
*               the Customer Display when in Register Mode.
*
*               See also DispRegOpDscrCntr() for the Operator Display.
*===========================================================================
*/
VOID DispRegCuDscrCntr( REGDISPMSG *pData )   
{
    UCHAR  uchCurrSave;
    UCHAR  uchWork;
 
    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (pData->uchArrow) {                         /* not turn on/off at 2x20 scroll */
        return;
    }
    uchCurrSave = DispRegSetCuStat();              /* uchCurrSave = current descriptor status */
        
    uchWork = uchCurrSave ^ uchDispRegCuOldStat;   /* uchWork = status change data */
        
    /*  10N10D */
    if (PifSysConfig()->uchCustType == DISP_10N10D) {
        if (uchWork & TOTAL_10CU_STATUS) {          
            if (uchCurrSave & TOTAL_10CU_STATUS) {
                UieDescriptor(UIE_CUST, _10CU_TOTAL, UIE_DESC_ON);
            } else {
                UieDescriptor(UIE_CUST, _10CU_TOTAL, UIE_DESC_OFF);
            }
        }
        if (uchWork & CHANGE_10CU_STATUS) {          
            if (uchCurrSave & CHANGE_10CU_STATUS) {
                UieDescriptor(UIE_CUST, _10CU_CHANGE, UIE_DESC_ON);
            } else {
                UieDescriptor(UIE_CUST, _10CU_CHANGE, UIE_DESC_OFF);
            }
        }
        if (uchWork & MINUS_10CU_STATUS) {          
            if (uchCurrSave & MINUS_10CU_STATUS) {
                UieDescriptor(UIE_CUST, _10CU_MINUS, UIE_DESC_ON);
            } else {
                UieDescriptor(UIE_CUST, _10CU_MINUS, UIE_DESC_OFF);
            }
        }
    /* 2x20 */
    } else if ((PifSysConfig()->uchCustType == DISP_2X20) ||
               (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
        if (uchWork & TOTAL_10CU_STATUS) {          
            if (uchCurrSave & TOTAL_10CU_STATUS) {
                UieDescriptor(UIE_CUST, _20CU_TOTAL, UIE_DESC_ON);
            } else {
                UieDescriptor(UIE_CUST, _20CU_TOTAL, UIE_DESC_OFF);
            }
        }
        if (uchWork & CHANGE_10CU_STATUS) {          
            if (uchCurrSave & CHANGE_10CU_STATUS) {
                UieDescriptor(UIE_CUST, _20CU_CHANGE, UIE_DESC_ON);
            } else {
                UieDescriptor(UIE_CUST, _20CU_CHANGE, UIE_DESC_OFF);
            }
        }
        if (uchWork & MINUS_10CU_STATUS) {          
            if (uchCurrSave & MINUS_10CU_STATUS) {
                UieDescriptor(UIE_CUST, _20CU_MINUS, UIE_DESC_ON);
            } else {
                UieDescriptor(UIE_CUST, _20CU_MINUS, UIE_DESC_OFF);
            }
        }
    }

    uchDispRegCuOldStat = uchCurrSave;    
                                /* save current status as old data */    
}
