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
* Title       : Check transaction qualifier for discount.
* Category    : Dicount Item Module, NCR 2170 US Hsopitality Model Application
* Program Name: DISCQUAL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: ItemDiscCheckQual() : Discount sequence check function main
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver. Rev. :   Name    : Description
* Jun-20-92: 00.00.01  : K.Maeda   : initial
* Nov-09-95: 03.01.00  : hkato     : R3.1
* Aug-05-98: 03.03.00  : hrkato    : V3.3
* Aug-08-13: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
*
** NCR2171 **
* Sep-06-99: 01.00.00  : M.Teraki  : initial (for 2171)
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

#include    <stdlib.h>
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
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCheckQual(UIFREGDISC *pUifRegDisc)
*
*    Input: UIFREGDISC *pUifRegDisc
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*         Returned status from ItemDiscCheckGCFStat()
*
** Description: Check relation between GCF qualifier status and discount
*               operation.
*===========================================================================
*/
SHORT   ItemDiscCheckQual(CONST UIFREGDISC *pUifRegDisc)
{
    SHORT           sReturnStatus;

    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
		/* cashier transaction  ? */
        sReturnStatus = ITM_SUCCESS;
    } else {
        sReturnStatus = ItemDiscCheckGCFStat();
    }

    if (sReturnStatus == ITM_SUCCESS) {
		TRANGCFQUAL     *GCFQualRcvBuff;

        if (TrnSplCheckSplit() == TRN_SPL_SEAT
            || TrnSplCheckSplit() == TRN_SPL_MULTI
            || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
            TrnGetGCFQualSpl(&GCFQualRcvBuff, TRN_TYPE_SPLITA);
        } else {
            TrnGetGCFQual(&GCFQualRcvBuff);
        }

        if (pUifRegDisc->fbModifier & VOID_MODIFIER) {
			// do this only if this is a void action in which something is being removed from
			// the transaction.  if this is an exchange and items have been added then
			// we do not do this step if this is not a void.
            switch(pUifRegDisc->uchMinorClass) {
            case CLASS_UIREGDISC1:
                if(!(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_REGULAR1)) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC2:
                if(!(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_REGULAR2)) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC3:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR3)) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC4:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR4)) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC5:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR5)) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC6:            /* R3.1 */
                if(!(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR6)) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UICHARGETIP:
            case CLASS_UICHARGETIP2:            /* V3.3 */
            case CLASS_UICHARGETIP3:
            case CLASS_UIAUTOCHARGETIP:
				//Allow void charge tips as first item in a check
				// added MDC check to bypass prohibit error when voiding charge tips if no charge tip applied to the check
				// for GSU SR 6 by cwunn
				//if MDC Addrss 23A allows charge tip voids with 0 itemizer,
				//   skip the GCFStatus = chargetip check, otherwise prohibit operation

				if(!CliParaMDCCheckField(MDC_CHRGTIP1_ADR, MDC_PARAM_BIT_A)) {
	                if(!(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_CHARGETIP)) {
		                sReturnStatus = LDT_PROHBT_ADR;
			        }
				}
                /* V3.3 */
                if(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_AUTOCHARGETIP) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				//Allow void charge tips as first item in a check
				// added MDC check to bypass new check/reorder key sequence blocking
				// for GSU SR 6 by cwunn
				if(!CliParaMDCCheck(MDC_CHRGTIP1_ADR, ODD_MDC_BIT3)) {
	                /* for new key sequence, V3.3 */
					if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) ||
						((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_REORDER)) {
						sReturnStatus = LDT_SEQERR_ADR;
					}
				}
                break;

            default:
                break;
            }

        } else {
            switch(pUifRegDisc->uchMinorClass) {
            case CLASS_UIREGDISC1:
                if(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_REGULAR1) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC2:
                if(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_REGULAR2) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC3:            /* R3.1 */
                if(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR3) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC4:            /* R3.1 */
                if(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR4) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC5:            /* R3.1 */
                if(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR5) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UIREGDISC6:            /* R3.1 */
                if(GCFQualRcvBuff->fsGCFStatus2 & GCFQUAL_REGULAR6) {
                    sReturnStatus = LDT_PROHBT_ADR;
                }

				// If the check for any other discount option is turned on then
				// if any other discount has been used, then we do a prohibit.
				if (CliParaMDCCheckField(MDC_MONEY1_ADR, MDC_PARAM_BIT_A)) {
					USHORT  usGcfStatus2 = (GCFQUAL_REGULAR3 | GCFQUAL_REGULAR4 | GCFQUAL_REGULAR5 | GCFQUAL_REGULAR6);

					if (GCFQualRcvBuff->fsGCFStatus2 & usGcfStatus2) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
					if (GCFQualRcvBuff->fsGCFStatus & (GCFQUAL_REGULAR1 | GCFQUAL_REGULAR2)) {
						sReturnStatus = LDT_PROHBT_ADR;
					}
				}
                break;

            case CLASS_UICHARGETIP:
            case CLASS_UICHARGETIP2:            /* V3.3 */
            case CLASS_UICHARGETIP3:
            case CLASS_UIAUTOCHARGETIP:
	                if(GCFQualRcvBuff->fsGCFStatus & GCFQUAL_CHARGETIP) {
		                sReturnStatus = LDT_PROHBT_ADR;
			        }
				//Allow charge tips as first item in a check
				// added MDC check to bypass new check/reorder key sequence blocking
				// for GSU SR 6 by cwunn
				if(!CliParaMDCCheck(MDC_CHRGTIP1_ADR, ODD_MDC_BIT3)) {
					/* for new key sequence, V3.3 */
					if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) ||
						((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_REORDER)) {
						sReturnStatus = LDT_SEQERR_ADR;
					}
				}
                break;

            default:
                break;
            }
        }
    }

    return(sReturnStatus);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCheckQualSpl(UIFREGDISC *pUifDisc, ITEMDISC *pDisc)
*
*    Input: UIFREGDISC *pUifRegDisc
*   Output: None
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*         Returned status from ItemDiscCheckGCFStat()
*
** Description: Check relation between GCF qualifier status and discount.
*===========================================================================
*/
SHORT   ItemDiscCheckQualSpl(CONST UIFREGDISC *pUifDisc, CONST ITEMDISC *pDisc)
{
    SHORT           sStatus = ITM_SUCCESS;

    if (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) {
        sStatus = ItemDiscCheckGCFStat();
    } else {
        sStatus = ITM_SUCCESS;
    }

    if (sStatus == ITM_SUCCESS) {
		UCHAR           uchMaskSeatDisc = GCFQUAL_REG1_SEAT;  // used with auchSeatDisc[] to check discounts

        if (pUifDisc->fbModifier & VOID_MODIFIER) {
			if (0 == pDisc->uchSeatNo) {
                return(ItemDiscCheckQual(pUifDisc));
			} else if ( (pDisc->uchSeatNo > 0) && (pDisc->uchSeatNo <= NUM_SEAT)) {
				if (!(TranGCFQualPtr->auchSeatDisc[pDisc->uchSeatNo - 1] & (uchMaskSeatDisc << (pUifDisc->uchMinorClass - CLASS_UIREGDISC1)))) {
					return(LDT_PROHBT_ADR);
				}
				return(ITM_SUCCESS);
			} else if ('0' == pDisc->uchSeatNo) {
                return(LDT_PROHBT_ADR);
			} else if ('B' == pDisc->uchSeatNo) {
				if (!(TranGCFQualPtr->auchSeatDisc[NUM_SEAT] & (uchMaskSeatDisc << (pUifDisc->uchMinorClass - CLASS_UIREGDISC1)))) {
					return(LDT_PROHBT_ADR);
				}
				return(ITM_SUCCESS);
			} else {
                return(ITM_SUCCESS);
			}
        } else {
			if (0 == pDisc->uchSeatNo) {
                return(ItemDiscCheckQual(pUifDisc));
			} else if ( (pDisc->uchSeatNo > 0) && (pDisc->uchSeatNo <= NUM_SEAT)) {
                if (TranGCFQualPtr->auchSeatDisc[pDisc->uchSeatNo - 1] & (uchMaskSeatDisc << (pUifDisc->uchMinorClass - CLASS_UIREGDISC1))) {
					return(LDT_PROHBT_ADR);
				}
				return(ITM_SUCCESS);
			} else if ('0' == pDisc->uchSeatNo) {
				return(ITM_SUCCESS);
			} else if ('B' == pDisc->uchSeatNo) {
                if (TranGCFQualPtr->auchSeatDisc[NUM_SEAT] & (uchMaskSeatDisc << (pUifDisc->uchMinorClass - CLASS_UIREGDISC1))) {
					return(LDT_PROHBT_ADR);
				}
				return(ITM_SUCCESS);
			} else {
                return(ITM_SUCCESS);
			}
		}
    }
    return sStatus;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCheckGCFStat(VOID)
*
*    Input: VOID
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_PROHBT_ADR : Function aborted with prohibited operation error
*
** Description: Check relation between new check and reorder transaction.
*
*===========================================================================
*/
SHORT   ItemDiscCheckGCFStat(VOID)
{
    SHORT           sReturnStatus = ITM_SUCCESS;

    if(TranGCFQualPtr->fsGCFStatus & GCFQUAL_PVOID) {
                                            /* new check is p-void */
       if (!(TranCurQualPtr->fsCurStatus & CURQUAL_PVOID)) {
                                            /* re-order is normal */
            sReturnStatus = LDT_PROHBT_ADR;
       }
    } else {                                /* new check is normal */
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) {
                                            /* re-order is p-void */
            sReturnStatus = LDT_PROHBT_ADR;
        }
    }

    if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) {
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_TRETURN) == 0) {
            sReturnStatus = (LDT_PROHBT_ADR);
        }
    } else {
        if (TranCurQualPtr->fsCurStatus & CURQUAL_TRETURN) {
            sReturnStatus = (LDT_PROHBT_ADR);
        }
    }
    if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING)) {
        if (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
            sReturnStatus = (LDT_PROHBT_ADR);
        }
    } else {
        if (!(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING)) {
            sReturnStatus = (LDT_PROHBT_ADR);
        }
    }    
    return(sReturnStatus);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemDiscCheckNormalAndSeat(ITEMDISC *pDisc)
*
*    Input:
*   Output:
*    InOut:
*   Return: ITM_SUCCESS    : Function Performed succesfully
*           LDT_PROHBT_ADR : Function aborted with prohibited operation error
*
** Description: Check relation between normal R/D and seat R/D.         V3.3
*===========================================================================
*/
SHORT   ItemDiscCheckNormalAndSeat(CONST ITEMDISC *pDisc)
{
    USHORT      usRD = 0;
    SHORT       i;

    for (i = 0; i <= NUM_SEAT; i++) {
        usRD |= TranGCFQualPtr->auchSeatDisc[i];
    }
    switch (pDisc->uchSeatNo) {
    case    0:
        if (usRD != 0) {
            return(LDT_PROHBT_ADR);
        }
        break;

    case    1:
    case    2:
    case    3:
    case    4:
    case    5:
    case    6:
    case    7:
    case    8:
    case    9:
    case    'B':
        if ((TranGCFQualPtr->fsGCFStatus & GCFQUAL_REGULAR1) ||
            (TranGCFQualPtr->fsGCFStatus & GCFQUAL_REGULAR2) ||
            (TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_REGULAR3) ||
            (TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_REGULAR4) ||
            (TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_REGULAR5) ||
            (TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_REGULAR6)) {
            return(LDT_PROHBT_ADR);
        }
        break;

    default:
        break;
    }

    return(ITM_SUCCESS);
}

/****** End of Source ******/
