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
* Title       : Reg Cursor Void Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFCVOID.C
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
*               UifCursorVoid() : Cursor Void Operation
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Mar-07-96:03.01.00:   M.Ozawa : initial 
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
#include <memory.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include <para.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCurVoid[] = {FSC_CURSOR_VOID, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT   UifCursorVoid( KEYMSG *pData )
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Cursor Void Operation functionality entry point - FSC_CURSOR_VOID
*===========================================================================
*/
SHORT   UifCursorVoid( KEYMSG *pData )
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence( aszSeqRegCurVoid );                       /* open cursor void key sequence */
        break;                                  
                                                            
    case UIM_INPUT:
        if ( pData->SEL.INPUT.uchMajor == FSC_CURSOR_VOID ) { /* cursor void ? */
			UIFREGOTHER     RegOther = {0};                   /* Other Transaction */
            sStatusSave = ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */
            if (flUifRegStatus & UIFREG_MODIFIER) {           /* modifier status */
            /* if(flUifRegStatus != 0) {  corrected at 07/22/98 */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchLen) {              /* exist input data ? */
                if (UifRegData.regsales.lQTY) {         /* input qty ? */
                    return(LDT_SEQERR_ADR);
                }

                if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {
                    return(sRetStatus);
                }
                RegOther.lAmount = (LONG)pData->SEL.INPUT.lData * 1000L;  /* set input amount to qty */
            } else {
                RegOther.lAmount = UifRegData.regsales.lQTY;
            }
            RegOther.uchMajorClass = CLASS_UIFREGOTHER;
			switch (pData->SEL.INPUT.uchMinor) {
				case 0:
					RegOther.uchMinorClass = CLASS_UICURSORVOID;         // FSC_CURSOR_VOID extended FSC 0
					if (pData->SEL.INPUT.ulStatusFlags & UIFREGMISC_STATUS_BALANCEDUE) {
						return(LDT_PROHBT_ADR);
					}
					break;
				case 1:
					RegOther.uchMinorClass = CLASS_UICURSORTRETURN1;     // FSC_CURSOR_VOID extended FSC 1
					break;
				case 2:
					RegOther.uchMinorClass = CLASS_UICURSORTRETURN2;     // FSC_CURSOR_VOID extended FSC 2
					break;
				case 3:
					RegOther.uchMinorClass = CLASS_UICURSORTRETURN3;     // FSC_CURSOR_VOID extended FSC 3
					break;
				default:
					NHPOS_ASSERT_TEXT((pData->SEL.INPUT.uchMinor == 0), "FSC_CURSOR_VOID: Invalid minor code.");
					return(LDT_PROHBT_ADR);
					break;
			}

			if (pData->SEL.INPUT.ulStatusFlags & UIFREGMISC_STATUS_BALANCEDUE) {
				RegOther.uchFsc = 1;
			} else {
				RegOther.uchFsc = 0;
			}
            sRetStatus = ItemOtherCursorVoid(&RegOther);
			//US Customs
			if(ParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){
				UIFREGTOTAL DummyTotal = {0};
				ITEMTOTAL	DummyItemTotal = {0};
				ItemOtherCurVoidRedisplay(&DummyTotal, &DummyItemTotal);
			}
			//End US Customs
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
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

