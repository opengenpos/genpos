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
* Program Name: DSPECHO2.C
* ----------------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*               DispRegSetOpKeep() : set descriptor for echo back
*               DispRegOpEchoBack() : make display format for echo back 
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 :   R.Itoh  : initial                                   
* Mar-27-95: 03.00.00 : M.Ozawa   : modify for LCD enhancement
*          :          :                                    
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
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <uie.h>
#include    <paraequ.h>
#include    <para.h>
#include    <maint.h>
#include    <display.h>
#include    <regstrct.h>
#include    <csstbpar.h>
#include    <rfl.h>
#include    "disp20.h"

static ULONG DispRegSetOpKeep(VOID);

/*
*===========================================================================
** Synopsis:    VOID DispRegOpEchoBack( REGDISPMSG *pData )      
*     Input:    nothing    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing  
*
** Description:     This function makes display format for echo back.  
*===========================================================================
*/
VOID DispRegOpEchoBack( REGDISPMSG *pData )
{
    static  const TCHAR FARCONST    format20_OP_1[] = _T("%20s\n                   %u");
    static  const TCHAR FARCONST    format20_OP_2[] = _T("\t%s\n%-20s");
    static  const TCHAR FARCONST    format20_OP_3[] = _T("%20s\n%-18s %u");
    static  const TCHAR FARCONST    format20_OPL_1[] = _T("%40s\n\t%u");
    static  const TCHAR FARCONST    format20_OPL_2[] = _T("\t%s\n%-40s");
    static  const TCHAR FARCONST    format20_OPL_3[] = _T("%40s\n%-18s\t%u");

    ULONG   ulKeepFlag = DispRegSetOpKeep();     /* ulKeepFlag = descriptor control data for echo back */ 
    TCHAR   aszNull[] = _T("");                        
                        
    switch(pData->fbSaveControl & DISP_SAVE_TYPE_MASK) {
    case DISP_SAVE_TYPE_5:
    case DISP_SAVE_TYPE_0:
    case DISP_SAVE_TYPE_1:
        //UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, 
        //    format20_OP_1, aszNull, (USHORT)uchDispCurrMenuPage);   /* RFC 01/30/2001 */
        
        if (PifSysConfig()->uchOperType == DISP_LCD) {
            UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OPL_1, aszNull, (USHORT)uchDispCurrMenuPage);
        } else {
            UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OP_1, aszNull, (USHORT)uchDispCurrMenuPage);
        }
        break;

    case DISP_SAVE_TYPE_2:
        //UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, 
        //    format20_OP_1, aszNull, (USHORT)uchDispCurrMenuPage);   /* RFC 01/30/2001 */
        
        if (PifSysConfig()->uchOperType == DISP_LCD) {
            UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OPL_1, aszNull, (USHORT)uchDispCurrMenuPage);
        } else {
            UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OP_1, aszNull, (USHORT)uchDispCurrMenuPage);
        }
        ulDispRegSaveDsc = ulDispRegOpBackStat;
        break;

    case DISP_SAVE_TYPE_3:
        /* V3.3 */
		{
			WCHAR  aszMnemonics[PARA_LEADTHRU_LEN + 1] = {0};

			//UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, 
			//    format20_OP_2, &ParaLeadThruWrk.aszMnemonics,
			//                            &DispRegSaveData.aszMnemonic);   /* RFC 01/30/2001 */
			RflGetLead (aszMnemonics, LDT_REGMSG_ADR);    /* Read  lead through mnemonic */
			if (PifSysConfig()->uchOperType == DISP_LCD) {
				UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OPL_2, aszMnemonics, &DispRegSaveData.aszMnemonic);
			} else {
				UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OP_2, aszMnemonics, &DispRegSaveData.aszMnemonic);
			}
			ulDispRegSaveDsc = ulDispRegOpBackStat;
		}
        break;
    
    case DISP_SAVE_TYPE_4:
        //UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL,
        //    format20_OP_3, aszNull, pData->aszMnemonic,
        //                            (USHORT)uchDispCurrMenuPage);   /* RFC 01/30/2001 */
        
        if (PifSysConfig()->uchOperType == DISP_LCD) {
            UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OPL_3, aszNull, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
        } else {
            UieEchoBackGround(ulKeepFlag, UIE_ATTR_NORMAL, format20_OP_3, aszNull, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
        }
        break;

/* #ifdef  DISP_REG_DEBUG
    default :
        PifAbort(MODULE_DISPREG_ID, FAULT_INVALID_ARGUMENTS);
        break;
#endif */
    }
}

/*
*===========================================================================
** Synopsis:    ULONG DispRegSetOpKeep(VOID)      
*     Input:    nothing    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ULONG flDispRegKeepControl  
*
** Description:     This function sets descriptor for echo back.  
*===========================================================================
*/
static ULONG DispRegSetOpKeep(VOID)
{
    ULONG  ulDispRegOpKeepStat = UIE_KEEP;

    ulDispRegOpBackStat = 0;

#if 0
    /* Because this descriptor is controlled by the ISP module, */
    /* the display module cannot be changed. HPUS V3.3          */

    if (flDispRegKeepControl & COMM_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC00_ON;       
        ulDispRegOpBackStat |= COMM_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC00_OFF;      
    }                              
#endif

    if (flDispRegKeepControl & NEW_STORE_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC04_ON;       
        ulDispRegOpBackStat |= NEW_STORE_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC04_OFF;      
    }                              

    if (flDispRegKeepControl & TRAIN_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC05_ON;       
        ulDispRegOpBackStat |= TRAIN_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC05_OFF;      
    }                              

    if (flDispRegKeepControl & P_BALANCE_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC06_ON;       
        ulDispRegOpBackStat |= P_BALANCE_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC06_OFF;      
    }                              

    if (flDispRegKeepControl & TXBLE_TRANS_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC07_ON;       
        ulDispRegOpBackStat |= TXBLE_TRANS_20_STATUS;
    } else {                             
        ulDispRegOpKeepStat |= UIE_DESC07_OFF;      
    }                              

    if (flDispRegKeepControl & PAYMENT_RECALL_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC08_ON;       
        ulDispRegOpBackStat |= PAYMENT_RECALL_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC08_OFF;      
    }                              

    if (flDispRegKeepControl & MINUS_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC09_ON;       
        ulDispRegOpBackStat |= MINUS_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC09_OFF;      
    }                              

    if (flDispRegKeepControl & VOID_DELETE_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC10_ON;       
        ulDispRegOpBackStat |= DELETE_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC10_OFF;      
    }                              
                                           
    if (flDispRegKeepControl & CRED_ALPHA_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC11_ON;       
        ulDispRegOpBackStat |= CRED_ALPHA_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC11_OFF;      
    }                              

    if (flDispRegKeepControl & T_VOID_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC12_ON;       
        ulDispRegOpBackStat |= T_VOID_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC12_OFF;      
    }                              

    if (flDispRegKeepControl & RECEIPT_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC13_ON;      
        ulDispRegOpBackStat |= RECEIPT_20_STATUS;
    } else {        
        ulDispRegOpKeepStat |= UIE_DESC13_OFF;     
    }                              

    if (flDispRegKeepControl & TOTAL_B_DUE_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC14_ON;       
        ulDispRegOpBackStat |= TOTAL_B_DUE_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC14_OFF;      
    }                              

    if (flDispRegKeepControl & CHANGE_20_CONTROL) {                 
        ulDispRegOpKeepStat |= UIE_DESC15_ON;       
        ulDispRegOpBackStat |= CHANGE_20_STATUS;
    } else {
        ulDispRegOpKeepStat |= UIE_DESC15_OFF;      
    }                              
    return (ulDispRegOpKeepStat);    
}    

