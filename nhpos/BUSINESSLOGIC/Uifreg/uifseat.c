/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Seat# Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFSEAT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifSeat() : Reg Seat# Key
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jan-08-96:03.01.00:  hkato    : R3.1
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
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"
#include <transact.h>


/*----- Key Block -----*/
UIMENU FARCONST aChildRegSeat[] = {{UifSeatEntry, CID_SEATENTRY},
                                   {NULL, 0}};
UIMENU FARCONST aChildRegSeatPay[] = {{UifTotal, CID_TOTAL},
                                      {UifEC, CID_EC},
                                      {UifDiscount, CID_DISCOUNT},
                                      {UifChargeTips, CID_CHARGETIPS},    /* 5/22/96 */
                                      {UifPayment, CID_PAYMENT},
                                      {UifCancel, CID_CANCEL},
                                      {NULL, 0}};
UIMENU FARCONST aChildRegSeatType2[] = {{UifTotal, CID_TOTAL},
                                        {UifPayment, CID_PAYMENT},
                                        {UifEC, CID_EC},
                                        {UifDiscount, CID_DISCOUNT},
                                        {UifChargeTips, CID_CHARGETIPS},    /* 5/22/96 */
                                        {UifTransfer, CID_TRANSFER},
                                        {NULL, 0}};
UIMENU FARCONST aChildRegSeatPayTend[] = {{UifPayment, CID_PAYMENT},
                                          {NULL, 0}};
/*
*===========================================================================
** Synopsis:    SHORT   UifSeat(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Seat# Module.
*===========================================================================
*/
SHORT   UifSeat(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegSeat);
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_FINALIZE) {
            UieAccept();
            break;

        } else if (flUifRegStatus & UIFREG_SEATTRANS) {
            UieNextMenu(aChildRegSeatPay);
            break;

        } else if (flUifRegStatus & UIFREG_TYPE2_TRANS) {
            UieNextMenu(aChildRegSeatType2);
            break;

        } else if (flUifRegStatus & UIFREG_BASETRANS) {
            UieAccept();
            break;
        }

        switch (pData->SEL.usFunc) {
        case CID_SEATENTRY:
            UieAccept();
            break;

        case CID_TOTAL:
            UieNextMenu(aChildRegSeatPayTend);
            break;

        default:
            UieNextMenu(aChildRegSeatPayTend);
            break;
        }
        break;

    case UIM_REJECT:
        break;

    default:                                    /* not use */
        return(UifRegDefProc(pData));           /* default process */
    }
    return (SUCCESS);
}


/*----- Key -----*/
UISEQ FARCONST aszSeqRegSeatEntry[] = {FSC_SEAT, 0};

/*
*===========================================================================
** Synopsis:    SHORT   UifSeatEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Seat# Entry Module.
*===========================================================================
*/
SHORT   UifSeatEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegSeatEntry);
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_SEAT) {
			UIFREGMODIFIER  ItemData = {0};

			// basic data check for valid seat number. if no seat number was entered
			// then function ItemModSeatNo() will request that a seat number be entered.
			// there are several versions of seat FSC_SEAT which do different actions.
            if (!pData->SEL.INPUT.uchMinor || pData->SEL.INPUT.uchMinor > FSC_SEAT_MAX ||
				pData->SEL.INPUT.lData > NUM_SEAT ||
				(pData->SEL.INPUT.lData < 1 && pData->SEL.INPUT.uchLen != 0)) { //SR206
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_DIGIT1) { //SR206
				return(LDT_SEQERR_ADR);
            }

            /* 98.10.14 */
            if (flUifRegStatus & UIFREG_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }

			// set the minor class based on the extended FSC for the FSC_SEAT key.
			// at this time there are two extended FSC, one to actually do a seat
			// modifier and the other to display the items for the specified seat.
            ItemData.uchMajorClass = CLASS_UIFREGMODIFIER;
            ItemData.uchMinorClass = (UCHAR)(CLASS_UISEATNO1 - 1 + pData->SEL.INPUT.uchMinor);
            ItemData.lAmount = pData->SEL.INPUT.lData;
            sRetStatus = ItemModSeatNo(&ItemData);
            if (sRetStatus == UIF_CANCEL) {
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();
                break;
            }
            if (sRetStatus == UIF_SEAT_TRANS) {         /* Goto Next Key Seq */
                flUifRegStatus |= UIFREG_SEATTRANS;
                UieAccept();                            /* send accepted to parent */
                break;
            }
            if (sRetStatus == UIF_SEAT_TYPE2) {         /* Goto Next Key Seq */
                flUifRegStatus |= UIFREG_TYPE2_TRANS;
                UieAccept();                            /* send accepted to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
            break;
        }

        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
