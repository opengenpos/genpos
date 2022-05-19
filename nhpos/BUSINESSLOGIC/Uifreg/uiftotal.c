/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Total Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFTOTAL.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifTotal() : Reg Total
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Nov-09-95:03.01.00:hkato      : R3.1
* Jul-28-98:03.03.00:hrkato     : V3.3 ("99" Service Total: Whole tax print 
*                               :  with service total at split check system.)
* Nov-07-15:02.02.01:R.Chambers : allow splitting of a guest check that does not have seats.
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

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"
#include "transact.h" //SR 353 necessary for using TrnInformation Structure
#include "trans.h"    //SR 353 necessary for using TrnInformation Structure
#include "BlFWif.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegTotal[] = {FSC_TOTAL, FSC_SPLIT, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifTotal(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Total Module
*===========================================================================
*/
SHORT UifTotal(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;
	USHORT i, numSeats = 0, numSeatsPaid = 0; //SR 353

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegTotal);        /* open total key sequence */
        break;

    case UIM_INPUT:

		NHPOS_ASSERT_ARRAYSIZE(UifRegData.regtotal.aszNumber, NUM_NUMBER); //JHHJ NHPOS ASSERT

        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if (pData->SEL.INPUT.uchMajor == FSC_TOTAL) {   /* total ? */
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_TOTAL_MAX)) { /* check total # */
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen > 2) {          /* V3.3 */
                return(LDT_SEQERR_ADR);                 
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                return(LDT_SEQERR_ADR);                 
            }
            memcpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, sizeof(aszUifRegNumber));  /* set number */
            /* memcpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, NUM_NUMBER);  / set number */
            UifRegData.regtotal.uchMajorClass =  CLASS_UIFREGTOTAL;  /* set total class */
            UifRegData.regtotal.uchMinorClass = pData->SEL.INPUT.uchMinor;
            if (pData->SEL.INPUT.uchLen != 0) {
                UifRegData.regtotal.uchSeatNo = (UCHAR)pData->SEL.INPUT.lData;
                UifRegData.regtotal.fbInputStatus |= INPUT_0_ONLY;
            }
            sRetStatus = ItemTotal(&UifRegData.regtotal);   /* Total Modele */
            UifRegWorkClear();                              /* clear work area */
            switch (sRetStatus) {
            case UIF_CANCEL:                            /* cancel */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;

            case UIF_SUCCESS:                           /* success */
                UieAccept();                            /* send accepted to parent */
                break;

            case LDT_TAKETL_ADR:                        /* buffer full */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);

            case UIF_FINALIZE:                          /* finalize */
                flUifRegStatus |= UIFREG_FINALIZE;      /* set finalize status */
                UieAccept();                            /* send accepted to parent */
				UifRegClearDisplay(0);					/* clear work area */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                break;

            case UIF_CAS_SIGNOUT:                       /* cashier sign out */
                flUifRegStatus |= (UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE);   /* set cashier sign out and finalize status */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                            /* send accepted to parent */
                break;

            case UIF_WAI_SIGNOUT:                       /* waiter sign out */
                flUifRegStatus |= (UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE);    /* set waiter sign out and finalize status */
                UieAccept();                            /* send accepted to parent */
                break;

            case UIF_SEAT_TRANS:                            /* End of Seat Trans,  R3.1 */
                flUifRegStatus |= UIFREG_BASETRANS;         /* Goto Base Trans Seq */
                flUifRegStatus &= ~(UIFREG_SEATTRANS | UIFREG_TYPE2_TRANS);
                UieAccept();
                break;

            default:                                        
                return(sRetStatus);
            }
            break;

        } else if (pData->SEL.INPUT.uchMajor == FSC_SPLIT) {
            if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                return(LDT_SEQERR_ADR);                 
            }


			//Begin SR 353 Work. We will count the number of seats that have been assigned.
			//We then count the number of seats that have been paid.  By subtracting those
			//values, we will be able to tell how many checks are left.  If there is only 
			//one left, we will return a prohibit error, because the user should not be 
			//allowed to split a single check. JHHJ 3-30-05.
			for(i = 0; i < NUM_SEAT; i++)
			{
				if(TrnInformation.TranGCFQual.auchUseSeat[i] != 0)
				{
					numSeats++;
				}
				if(TrnInformation.TranGCFQual.auchFinSeat[i] != 0)
				{
					numSeatsPaid++;
				}
			}

			// we will allow the splitting of a guest check that was not created with any seats
			// to provide the convenience of being able to split a check without having had to
			// previously assign items to seats.
			//    Richard Chambers Nov-07-2015
			if(numSeats && (numSeats - numSeatsPaid) <= 1)
			{
				return(LDT_PROHBT_ADR);
			}

            memcpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, sizeof(aszUifRegNumber));  /* set number */
            /* memcpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, NUM_NUMBER); */
            UifRegData.regtotal.uchMajorClass = CLASS_UIFREGTOTAL;
            UifRegData.regtotal.uchMinorClass = CLASS_UISPLIT;
            sRetStatus = ItemTotal(&UifRegData.regtotal);
            UifRegWorkClear();                              /* clear work area */
            switch (sRetStatus) {
            case UIF_CANCEL:                            /* cancel */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;

            case UIF_SUCCESS:                           /* success */
                UieAccept();                            /* send accepted to parent */
                break;

            case LDT_TAKETL_ADR:                        /* buffer full */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);

            default:                                        
                return(sRetStatus);
            }
            break;
        }

        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/