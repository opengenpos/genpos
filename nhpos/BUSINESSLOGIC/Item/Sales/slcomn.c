/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Dept./ PLU sales common process                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: ITSLCOMN.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesCommon() : Item sales common process   
*           ItemSalesComnAutoScale()
*           ItemSalesComnManWeight()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-16-92: 00.00.01  : K.Maeda   : initial                                   
* Jun- 5-92: 00.00.01  : K.Maeda   : 2nd code inspeciton held on Jun. 3rd.                                   
* Dec- 4-92: 01.00.00  : K.Maeda   : Add scale interface feature for GCA.                                   
*          :           :           :                         
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

#include	<tchar.h>
#include    <memory.h>
#include    <string.h>
#include    <stdlib.h>
#include    <math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <rfl.h>
#include    <uie.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    "itmlocal.h"
#include	"uireg.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesComnAutoScale(UIFREGSALES *pUifRegSales,
*                                          ITEMSALES *pItemSales,
*                                          ITEMMODLOCAL *pItemModLocal)
*
*    Input: *pUifRegSales
*           *pItemSales
*           *pItemModLocal
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS      : Function Performed succesfully
*         LDT_SEQERR_ADR   : Function aborted with Illegal operation sequense error
*         LDT_SCALEERR_ADR : 
*
** Description: Scalable item sales with auto sacle operation.
*                 
*===========================================================================
*/

#define REQ_TO_READ_SCALE  1

static SHORT   ItemSalesComnAutoScale(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, ITEMMODSCALE  *pModLocalScaleData)
{
	UCHAR    uchWork;
	SHORT    sReturnStatus;
	SHORT    sStatus = 0;

    if (pUifRegSales->fbModifier & VOID_MODIFIER) {
        return(LDT_SEQERR_ADR);
    }

    /* prohibit auto scale operation     */
    /* under preselect void transaction. */
    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) { 
        return(LDT_SEQERR_ADR);
    }

    /*--- GET MDC FOR SCALE ---*/
    /* get MDC address 29 to determine flags for MOD_READ_SCALE_AT_TARE, etc. */
    uchWork = CliParaMDCCheck(MDC_SCALE_ADR, (ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3));  /* get MDC address 29 */
    uchWork &= 0x0f;                              /* mask upper 4 bits */
    pModLocalScaleData->fbTareStatus |= uchWork;      /* set MDC address 29 for MOD_READ_SCALE_AT_TARE, etc. */

    if (pModLocalScaleData->fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
        if (!(pModLocalScaleData->fbTareStatus & MOD_READ_SCALE_AT_TARE)) {  /* read scale at dept/PLU */
            sStatus = REQ_TO_READ_SCALE;
        } else {
            sStatus &= ~REQ_TO_READ_SCALE;
        }
    } else {                                /* no tare key operation */
        if (pModLocalScaleData->fbTareStatus & MOD_READ_SCALE_AT_TARE) {     /* read scale at scale/tare key */
            return(LDT_SEQERR_ADR);
        }
        sStatus = REQ_TO_READ_SCALE;
    }

    if (sStatus == REQ_TO_READ_SCALE) {
        /*--- READ AUTO SCALE ---*/
        sReturnStatus = ItemCommonGetScaleData(pModLocalScaleData);
        if (sReturnStatus != ITM_SUCCESS) {
			return(sReturnStatus);
        }
    }

    if (pModLocalScaleData->lWeightScaled == 0L) {
        return(LDT_SCALEERR_ADR);                           /* no weght entry for scalable item */
    }

    pItemSales->fsPrintStatus &= ~PRT_MANUAL_WEIGHT;            /* auto scale entry */
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesComnManWeight(UIFREGSALES *pUifRegSales,
*                                          ITEMSALES *pItemSales,
*                                          ITEMMODLOCAL *pItemModLocal)
*
*    Input: *pItemSales
*           *pItemModLocal
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: Scalable item sales with manual weight entry opereation.
*
*===========================================================================
*/

#define SLCOMN_VOID_OR_MODDISC    0x01                /* item void or pre. void r mod. disc */           

static SHORT   ItemSalesComnManWeight(ITEMSALES *pItemSales, ITEMMODSCALE  *pModLocalScaleData)
{
	UIE_SCALE      ScaleDataBuff = {0};              /* scale data receive buffer */
	SHORT          sReturnStatus;
	SHORT          sScaleReadStatus;
	UCHAR          uchWork;

    if (pModLocalScaleData->fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
        return(LDT_TARENONSCALABLE_ADR);             /* tare key for non scalable item */
    }

    /*--- READ SCALE ---*/
    sScaleReadStatus = ItemCommonReadScale(&ScaleDataBuff);
    if (sScaleReadStatus < 0) {             /* scale error */
        switch(sScaleReadStatus) {
        case UIE_SCALE_NOT_PROVIDE:     /* ItemCommonReadScale() error return so allow manual weight entry */
        case UIE_SCALE_TIMEOUT:         /* ItemCommonReadScale() error return so allow manual weight entry */
        case UIE_SCALE_ERROR:           /* ItemCommonReadScale() error return so allow manual weight entry */
            break;                      /* allow manual weight entry */

        default:
            return(LDT_SCALEERR_ADR);       
            break;  /* not use */
        }
    } else {
        if (ScaleDataBuff.ulData != 0L) {
            return(LDT_SCALEERR_ADR);       /* error if commodity is on the scale */
        }
    }

    /*--- GET MDC FOR SCALE ---*/
    /* get MDC address 29 to determine flags for MOD_READ_SCALE_AT_TARE, etc. */
    uchWork = CliParaMDCCheck(MDC_SCALE_ADR, (ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3));  /* get MDC address 29 */
    uchWork &= 0x0f;                                /* mask upper 4 bits */
    pModLocalScaleData->fbTareStatus |= uchWork;   /* set MDC address 29 for MOD_READ_SCALE_AT_TARE, etc.  */

    sReturnStatus = ItemCommonCheckScaleDiv(pModLocalScaleData, pModLocalScaleData->lWeightScaled);
                                            /* check weight */
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);
    }
 
    if ((pItemSales->uchRndType & ITM_TYPE_WEIGHT) == 0) {  /* selling on weight */
        pItemSales->fsPrintStatus |= PRT_MANUAL_WEIGHT;         /* status for print */
    }
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCommon(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales);
*
*    Input: *pUifRegSales
*           *pItemSales
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This function executs following process.
*               - Validates QTY data entered   
*               - Process modified discount function. 
*===========================================================================
*/
SHORT   ItemSalesCommon(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
	ITEMMODSCALE  * CONST pModLocalScaleData = ItemModGetLocalScaleDataPtr();
    SHORT         sReturnStatus;
	
    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        if (pItemSales->lQTY == 0L) {       /* QTY wasn't entered ? */
            if (pUifRegSales->uchMinorClass == CLASS_UIMODDISC) { 
                return(LDT_SEQERR_ADR);     /* tare key for modified disc. item */
            }
		//BEGIN ADDITION OF TARE INCLUDED WITH PLU
			if(pItemSales->usTareInformation & PLU_REQ_TARE)
			{
				if(!(pModLocalScaleData->fbTareStatus & MOD_TARE_KEY_DEPRESSED))
				{
					UIFDIADATA    UifDiaRtn = { 0 };
					SHORT         sReturn;

					/* execute job number entry */

					/* --- execute Display Module --- */
					RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_TAREENTER, DISP_SAVE_TYPE_0);
					if ((sReturn = UifDiaTare(&UifDiaRtn)) != UIF_SUCCESS) {
						/* check job code */
						return sReturn;
					} else if (sReturn == UIFREG_ABORT) {
						return(UIF_CANCEL);
					}
				}
			} else if( pItemSales->usTareInformation & PLU_REQ_TARE_MASK)
			{
				UIFREGMODIFIER  regmodifier = {0};
				SHORT           sStatus = 0, sRetStatus;

				regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;       /* set class */
				regmodifier.uchMinorClass = CLASS_UITARE;
				regmodifier.lAmount = (pItemSales->usTareInformation & PLU_REQ_TARE_MASK);           /* set input tare */
				if ( ( sRetStatus = ItemModTare( &regmodifier ) ) != SUCCESS ) {
					sStatus =2;
				}
			}
		//END ADDITION OF TARE INCLUSION OF PLU

            sReturnStatus = ItemSalesComnAutoScale(pUifRegSales, pItemSales, pModLocalScaleData);
            if (sReturnStatus != ITM_SUCCESS) {
                return(sReturnStatus);
            }

            pItemSales->lQTY = pModLocalScaleData->lWeightScaled;
        } else {
            if ((ULONG)(pItemSales->lQTY) > 999999L) { /* TAR 105486 */
                return(LDT_KEYOVER_ADR);         /* return prohibit error */
            }

            pModLocalScaleData->lWeightScaled = pItemSales->lQTY;
            sReturnStatus = ItemSalesComnManWeight(pItemSales, pModLocalScaleData);
            if (sReturnStatus != ITM_SUCCESS) {
                return(sReturnStatus);
            }
            
            if (pUifRegSales->uchMinorClass == CLASS_UIMODDISC) { 
                /*----- SEQ CHECK SUB-ROUTINE -----*/
                sReturnStatus = ItemSalesModDiscCheck(pUifRegSales, pItemSales);
                if (sReturnStatus != ITM_SUCCESS) {
                    return(sReturnStatus);
                }
            }
        }

        /*--- CLEAR TARE DATA ---*/
        memset(pModLocalScaleData, 0, sizeof(*pModLocalScaleData));
    } else {
        if (pModLocalScaleData->fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
            return(LDT_TARENONSCALABLE_ADR);         /* tare key for non scalable item */
        }

        if (pItemSales->lQTY == 0L) {       /* QTY wasn't entered ? */
            pItemSales->lQTY = 1000L;       /* set QTY = 1 data */
        } else {
            if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
                return(LDT_SEQERR_ADR);     /* QTY entry for condiment */
            }  
            if ((pItemSales->lQTY % 1000L) > 0) { 
                                            /* Does QTY have decimal data? */
                return(LDT_PROHBT_ADR);     /* return prohibit error */
            }
            if ((ULONG)pItemSales->lQTY > 999000L) { /* TAR105486 */
                return(LDT_KEYOVER_ADR);         /* return prohibit error */
            }
        }

        if (pUifRegSales->uchMinorClass == CLASS_UIMODDISC) { 
            /*----- SEQ CHECK SUB-ROUTINE -----*/
            sReturnStatus = ItemSalesModDiscCheck(pUifRegSales, pItemSales);
            if (sReturnStatus != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }
    }

    return(ITM_SUCCESS);
}
/****** End of Source ******/
