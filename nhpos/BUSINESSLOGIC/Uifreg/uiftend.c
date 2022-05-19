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
* Title       : Reg Tender Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFTEND.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifTender() : Reg Tender
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Jan-08-96:03.01.00:hkato      : R3.1
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
#include "rfl.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"
#include "para.h"
#include "transact.h"
#include "BlFWif.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegTender[] = {FSC_TENDER,
                                    FSC_PRESET_AMT,
                                    FSC_OFFTEND_MODIF,
									FSC_CURSOR_VOID,
									// FSC_CANCEL,    AMT-2436,  Amtrak requested that Cancel not be allowed.
                                    0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifTender(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg Tender Module
*===========================================================================
*/
SHORT UifTender(KEYMSG *pData)
{
    SHORT           sRetStatus;
    SHORT           sStatusSave;
	ULONG			ulStatus[2];

	NHPOS_ASSERT_ARRAYSIZE(UifRegData.regtender.aszNumber, NUM_NUMBER); //JHHJ NHPOS ASSERT

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegTender);       /* register key sequence */
        break;
                                                
    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
#if 0
		// remove handling the cancel key if there is a balance due
		// this allows a partial tender with credit and then cancel resulting in
		// financials and back office being incorrect.
		// AMT-2436
        if (pData->SEL.INPUT.uchMajor == FSC_CANCEL) {   /* off line modifier */
			if (pData->SEL.INPUT.uchLen != 0) {         /* exist input data ? */
				return(LDT_SEQERR_ADR);
			}
			if (pData->SEL.INPUT.uchMinor != 0) {
				// allow more than one type of Cancel Key so that we do not
				// provide a way for Cashier to inadvertently cancel a transaction.
				// requested by Amtrak who is putting a Cancel Key button on their
				// layout to only be used to Cancel a credit tender.
				return(LDT_SEQERR_ADR);
			}
			if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
				return(sRetStatus);
			}
			UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
			UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
			sRetStatus = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
            UifRegData.regtender.fbModifier &= ~OFFCPTEND_MODIF;  /* set off line modifier status */
			UifRegWorkClear();                          /* clear work area */
			flUifRegStatus &= ~(UIFREG_PARTIAL | UIFREG_MODIFIER);   /* clear partial and modifier status if set */
            UieCancel();                                /* send reject to parent */
			return (SUCCESS);
		}
#endif
        if (pData->SEL.INPUT.uchMajor == FSC_OFFTEND_MODIF) {   /* off line modifier */

            /* check input condition */

            if ((UifRegData.regtender.fbModifier & OFFCPTEND_MODIF) ||
                (pData->SEL.INPUT.lData)) {
                return(LDT_SEQERR_ADR);
            }

            UifRegData.regtender.fbModifier |= OFFCPTEND_MODIF;   /* set off line modifier status */
            flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */

            UieKeepMessage();                       /* E-003 corr. '94 4/18 */
            return(ItemModOffTend());      
        }

        if (pData->SEL.INPUT.uchMajor == FSC_CURSOR_VOID) {   /* off line modifier */
			if (flUifRegStatus & UIFREG_PARTIAL) {
				pData->SEL.INPUT.ulStatusFlags |= UIFREGMISC_STATUS_BALANCEDUE;
			} else {
				flUifRegStatus |= UIFREG_CURSORRETURN;
			}
			return UifCursorVoid(pData);
		}

        if (pData->SEL.INPUT.uchMajor == FSC_TENDER) {   /* tender ? */
			/* temporary hardcoded 11, change to MAINT_TEND_MAX when implementing full
			tender database change JHHJ 3-16-04*/
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_TEND_MAX)) { /* check tender # */
                return(LDT_PROHBT_ADR);
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                if (!pData->SEL.INPUT.lData) {
                    return(LDT_SEQERR_ADR);
                }
                UifRegData.regtender.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }

			NHPOS_ASSERT_ARRAYSIZE(UifRegData.regtender.aszNumber, NUM_NUMBER); 
			
            _tcsncpy(UifRegData.regtender.aszNumber, aszUifRegNumber, NUM_NUMBER);  /* set number */
			RflEncryptByteString ((UCHAR *)&(UifRegData.regtender.aszNumber[0]), sizeof(UifRegData.regtender.aszNumber));
            UifRegData.regtender.uchMajorClass = CLASS_UIFREGTENDER;    /* set tender class */
            UifRegData.regtender.uchMinorClass = pData->SEL.INPUT.uchMinor;
            UifRegData.regtender.lTenderAmount = pData->SEL.INPUT.lData;  /* set input amount */
        } else if (pData->SEL.INPUT.uchMajor == FSC_PRESET_AMT) {
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_PREAMT_MAX)) {   /* check tender # */
                return(LDT_PROHBT_ADR);                                                         
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regtender.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            _tcsncpy(UifRegData.regtender.aszNumber, aszUifRegNumber, NUM_NUMBER);  /* set number */
			RflEncryptByteString ((UCHAR *)&(UifRegData.regtender.aszNumber[0]), sizeof(UifRegData.regtender.aszNumber));
            UifRegData.regtender.uchMajorClass = CLASS_UIFREGPAMTTENDER;        /* set major class to indicate preset tender, FSC_PRESET_AMT */
            UifRegData.regtender.uchMinorClass = pData->SEL.INPUT.uchMinor;     // minor class is the preset number for P15 table lookup for now.
            UifRegData.regtender.lTenderAmount = pData->SEL.INPUT.lData;        /* set input amount */
        }

        if (pData->SEL.INPUT.uchMajor == FSC_TENDER || pData->SEL.INPUT.uchMajor == FSC_PRESET_AMT) {
			if(uchIsNoCheckOn == 1) // flag that tells us that #/TYPE was previous button pressed,
			{
				switch(pData->SEL.INPUT.uchMinor)
				{//only allow Check Tender button to be pressed next. SR 18 JHHJ
				case CLASS_UITENDER8:												//If the tender pressed is 8,	
					if(!ParaMDCCheck(MDC_TEND84_ADR, EVEN_MDC_BIT2)){	//check to see if it is assigned to Check no Purchase
						return(LDT_SEQERR_ADR);				//return Sequence error  SR 18 JHHJ 8-21-03
					}
					break;
				case CLASS_UITENDER9:													//If the tender pressed is 9
					if(!ParaMDCCheck(MDC_TEND94_ADR, EVEN_MDC_BIT2)){	//check to see if it is assigned to Check No Purchase
						return(LDT_SEQERR_ADR);				//return Sequence error SR 18 JHHJ 8-21-03
					}
					break;
				case CLASS_UITENDER10:											//If the tender pressed is 10	
					if(!ParaMDCCheck(MDC_TEND104_ADR,EVEN_MDC_BIT2)){	//check to see if it is assigned to Check No Purchase
						return(LDT_SEQERR_ADR);				//return Sequence error SR 18 JHHJ 8-21-03
					}
					break;
				default:
					return(LDT_SEQERR_ADR);
				}
			}
			BlFwIfDrawerStatus(ulStatus);
			// Now perform the tender.  We allow the changing of the tender type
			// as part of executing a specific tender.  This would typically happen when
			// a card swipe or account number entry is prompted and the operator elects to
			// change the tender type to some other type such as Cash instead.
			do {
				memset (UifRegData.regtender.aszNumber, 0, sizeof(UifRegData.regtender.aszNumber));
				if (uchIsNoCheckOn) {
					// copy the data from a #/Type used with Check No Purchase, MDC_CHK_NO_PURCH,
					// into the tender data so that it will appear on receipt and Electronic Journal.
					// this is not the same as VCS Ask Check No Purchase. see ItemCheckNoPurchase()
					_tcsncpy(UifRegData.regtender.aszNumber, aszUifRegNumber, STD_NUMBER_LEN);
				}
				sRetStatus = ItemTenderEntry(&UifRegData.regtender); /* Tender Modele */
			} while (sRetStatus == UIF_NEW_FSC);

			{
				USHORT usLength = 0;
				FSCTBL myFsc;

				PifSleep (500);
				UiePeekRingBuf(&myFsc, 0, &usLength);
				while (myFsc.uchMajor == FSC_MSR) {
					UieReadRingBuf(&myFsc, 0, &usLength);
					PifSleep (50);
					UiePeekRingBuf(&myFsc, 0, &usLength);
				}
			}

			if(uchIsNoCheckOn == 1)  //we are in #/Type, so we are Check No Purchase, but after ItemTender, flag no longer needed,
			{						//so reset all flags that we used to default values SR 18 JHHJ 8-21-03
				uchIsNoCheckOn = 0;
				fsPrtCompul = 0;
				fsPrtNoPrtMask = 0;
			}

            if (sRetStatus == UIF_FS2NORMAL) {              /* Saratoga */
				// the purpose of the following is difficult to understand. It has been
				// in the source code base since the beginning with NHPOS 1.4 back in 2002
				// when NCR donated the source code to Georgia Southern University.
				//
				// this automatically entering an additional total key press if foodstamp tender is
				// triggered by the return value of function ItemTendFSNonFoodPart() indicating that
				// the transaction has both foodstampable and non-foodstampably items in the transaction
				// and a partial or split tender with the food stamp tender was done. there is still a
				// balance due.
				// 
				// this total minor class is evaluated as a total key type of ITM_TYPE_CHECK_FS_TOTAL
				// which ends up calling function ItemTotalCFS().
				//    Richard Chambers, Mar-18-2018
				UIFREGTOTAL     UifTotal = {0};

				UifTotal.uchMajorClass = CLASS_UIFREGTOTAL;
                UifTotal.uchMinorClass = CLASS_UITOTAL2_FS;
                sRetStatus = ItemTotal(&UifTotal);
            }
			UifRegCheckDrawerLimit(1);						//SR 155 Cash Drawer Limit
            UifRegWorkClear();

            switch (sRetStatus) {
            case UIF_CANCEL:                                /* cancel */
                UifRegData.regtender.fbModifier &= ~OFFCPTEND_MODIF;  /* set off line modifier status */
                flUifRegStatus &= ~UIFREG_MODIFIER;         /* set modifier status */
                ItemOtherClear();                           /* E-068 */       
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                                /* send reject to parent */
                break;

            case UIF_SUCCESS:                               /* success */
                flUifRegStatus &= ~UIFREG_BUFFERFULL;       /* reset buffer full status */
				ItemTenderClearChargeAutoTips();            // reset the charge tips indicator if set allowing multiple charge tips
                UieAccept();                                /* send accepted to parent */
                break;

            case LDT_TAKETL_ADR:                            /* buffer full */
                flUifRegStatus |= UIFREG_BUFFERFULL;        /* set buffer full status */
                UieAccept();                                /* send accepted to parent */
                return(sRetStatus);

            case UIF_FINALIZE:                              /* finalize */
                flUifRegStatus |= UIFREG_FINALIZE;          /* set finalize status */
                UieAccept();                                /* send accepted to parent */
				UifRegClearDisplay(0);						/* clear work area */
				BlFwIfDefaultUserScreen();					/* call function to bring up default window(s) */
                break;

            case UIF_CAS_SIGNOUT:                           /* cashier sign out */
                /* set cashier sign out and finalize status */

                flUifRegStatus |= (UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE);
                UieAccept();                                /* send accepted to parent */
                break;

            case UIF_WAI_SIGNOUT:                           /* waiter sign out */
                /* set waiter sign out and finalize status */

                flUifRegStatus |= (UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE);
                UieAccept();                                /* send accepted to parent */
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
        }
        /* break */                                     /* not use */

    case UIM_CANCEL:                                    /* clear key case   */
        UifRegData.regtender.fbModifier &= ~OFFCPTEND_MODIF;
		if( uchIsNoCheckOn == 1)
		{
			uchIsNoCheckOn = 0;
		}
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
