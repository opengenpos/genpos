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
* Title       : Price Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDPRCET.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaPriceEntry() : Reg Price Entry
*               UifDiaTareEntry()  : Reg Tare Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name     : Description
* Nov-09-99 :          : M.Ozawa    : initial                                   
* Jul-16-15 : 02.02.01 : R.Chambers : allow additional keys for Price Entry and Tare Entry
* Jul-29-15 : 02.02.01 : R.Chambers : new data entry UifDiaDeptEntry() used by UifDiaDept()
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
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"
#include <rfl.h>
#include <string.h>
#include <item.h>
                                                                                

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaPrice(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Register Mode Price Entry Module to ask for the entry of a price
*               for an item or for a department.  In some places we allow for the
*               quick identification of whether this is a department entry or an
*               item price entry by looking to see what key was pressed.
*===========================================================================
*/
SHORT UifDiaPriceEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:            // treat P1 key is same as Price Enter key (FSC_PRICE_ENTER) for UifDiaPriceEntry() to reduce number of buttons
        case FSC_AC:            // treat the AC Key same as Price Enter key (FSC_PRICE_ENTER) for UifDiaPriceEntry() to reduce number of buttons
        case FSC_NUM_TYPE:      // treat the #/Type Key same as Price Enter key (FSC_PRICE_ENTER) for UifDiaPriceEntry() to reduce number of buttons
			pData->SEL.INPUT.uchMajor = FSC_PRICE_ENTER;
        case FSC_PRICE_ENTER:
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
			UifRegDiaWork.lAmount = pData->SEL.INPUT.lData;   /* set input data */
            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.uchLen;
            return(UIF_DIA_SUCCESS);
            
        /* --- Dept No. key --- */
        case FSC_DEPT:
            /* --- check department number entered is within range --- */
            sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT4);
            if (sRetStatus != SUCCESS) {
                /* --- return lead thru messsage number --- */
                return(sRetStatus);
            }

            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);

        case FSC_KEYED_DEPT:
            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.uchLen;

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);
        
        case FSC_MENU_SHIFT:                        /* menu shift */
            /* --- check indexed data --- */
            sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT3);
            if (sRetStatus != SUCCESS) {

                /* --- return lead thru messsage number --- */
                return(sRetStatus);
            }
            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);

        case FSC_D_MENU_SHIFT:                      /* direct menu shift */
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }

            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);

		case FSC_CLEAR:                 /* clear key ? */
			// FSC_CLEAR may give Sequence Error on first press while Error Correct does not
			// so lets turn the FSC_CLEAR into FSC_EC so that clearing the last data entry
			// allowing a different entry will work should Clear Key be pressed.
			pData->SEL.INPUT.uchMajor = FSC_EC;    // Clear to Error Correct to handle properly
		case FSC_EC:                    /* error correct key */
			UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
			UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			return(UIF_DIA_SUCCESS);

        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaNoPersonEntry() allowing consolidation of button types
        case FSC_CANCEL:
			UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
			UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(UIF_DIA_SUCCESS);
            }
			memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
			memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
			UifRegDiaWork.auchFsc[0] = FSC_ERROR;
			//SR 143 HALO Override cwunn Reset status variable for RflHALO
			RflSetHALOStatus(0x00);
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
			memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
			memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
			//SR 143 HALO Override cwunn Reset status variable for RflHALO
			RflSetHALOStatus(0x00);
            return(UIFREG_ABORT);

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifRegDefProc(pData));           /* reg default process */

		/*6/17/2004 - removed - CDH
		DO not need to process for @/For key - if this is pressed when doing price entry, it should be a sequence error

		//SR 143 @/For Key cwunn handle price entry after depression of @/FOR key
		/*case FSC_FOR:
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
           /* UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
			UifRegDiaWork.ulFor = pData->SEL.INPUT.lData;
			UifDiaPrice(&UifRegDiaWork);
            return(UIF_DIA_SUCCESS);*/	

        default:                                        
            return(LDT_SEQERR_ADR);
        }

    case UIM_REDISPLAY:
    case UIM_DRAWER:
        return(UieDefProc(pData));                  /* engine default process */

    case UIM_INIT:
    case UIM_CANCEL:
        break;

    default:                                        /* not use */
        return(LDT_SEQERR_ADR);
    }
    return (SUCCESS);
}

/*===========================================================================
** Synopsis:    SHORT UifDiaDeptEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Register Mode Dept Number Entry Module to ask for the entry
*               of adepartment number.
*===========================================================================
*/
SHORT UifDiaDeptEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:            // treat P1 key is same as Dept Number Enter key (FSC_DEPT) for UifDiaDeptEntry() to reduce number of buttons
        case FSC_AC:            // treat the AC Key same as Dept Number Enter key (FSC_DEPT) for UifDiaDeptEntry() to reduce number of buttons
        case FSC_NUM_TYPE:      // treat the #/Type Key same as Dept Number Enter key (FSC_DEPT) for UifDiaDeptEntry() to reduce number of buttons
			pData->SEL.INPUT.uchMajor = FSC_DEPT;
        case FSC_DEPT:
            /* --- check department number entered is within range --- */
            sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT4);
            if (sRetStatus != SUCCESS) {
                /* --- return lead thru messsage number --- */
                return(sRetStatus);
            }

            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);

        case FSC_KEYED_DEPT:
            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.uchLen;

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);
        
        case FSC_MENU_SHIFT:                        /* menu shift */
            /* --- check indexed data --- */
            sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT3);
            if (sRetStatus != SUCCESS) {

                /* --- return lead thru messsage number --- */
                return(sRetStatus);
            }
            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);

        case FSC_D_MENU_SHIFT:                      /* direct menu shift */
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }

            /* --- setup dialog i/f work block --- */
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   

            /*--- return dialog successful status --- */
            return(UIF_DIA_SUCCESS);
        
		case FSC_CLEAR:                 /* clear key ? */
			// FSC_CLEAR may give Sequence Error on first press while Error Correct does not
			// so lets turn the FSC_CLEAR into FSC_EC so that clearing the last data entry
			// allowing a different entry will work should Clear Key be pressed.
			pData->SEL.INPUT.uchMajor = FSC_EC;    // Clear to Error Correct to handle properly
		case FSC_EC:                    /* error correct key */
			UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
			UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			return(UIF_DIA_SUCCESS);

		case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaNoPersonEntry() allowing consolidation of button types
        case FSC_CANCEL:
			UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
			UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
				return(UIF_DIA_SUCCESS);
			}
			memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
			memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
			UifRegDiaWork.auchFsc[0] = FSC_ERROR;
			//SR 143 HALO Override cwunn Reset status variable for RflHALO
			RflSetHALOStatus(0x00);
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
			memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
			memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
			//SR 143 HALO Override cwunn Reset status variable for RflHALO
			RflSetHALOStatus(0x00);
            return(UIFREG_ABORT);

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifRegDefProc(pData));           /* reg default process */

		/*6/17/2004 - removed - CDH
		DO not need to process for @/For key - if this is pressed when doing price entry, it should be a sequence error

		//SR 143 @/For Key cwunn handle price entry after depression of @/FOR key
		/*case FSC_FOR:
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
           /* UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
			UifRegDiaWork.ulFor = pData->SEL.INPUT.lData;
			UifDiaPrice(&UifRegDiaWork);
            return(UIF_DIA_SUCCESS);*/	

        default:                                        
            return(LDT_SEQERR_ADR);
        }

    case UIM_REDISPLAY:
    case UIM_DRAWER:
        return(UieDefProc(pData));                  /* engine default process */

    case UIM_INIT:
    case UIM_CANCEL:
        break;

    default:                                        /* not use */
        return(LDT_SEQERR_ADR);
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaPrice(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Register Tare (scale) Entry Module
*===========================================================================
*/
SHORT UifDiaTareEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;
	UIFREGMODIFIER  regmodifier;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:            // treat P1 key is same as Tare Enter key (FSC_TARE) for UifDiaPriceEntry() to reduce number of buttons
        case FSC_AC:            // treat the AC Key same as Tare Enter key (FSC_TARE) for UifDiaPriceEntry() to reduce number of buttons
        case FSC_NUM_TYPE:      // treat the #/Type Key same as Tare Enter key (FSC_TARE) for UifDiaPriceEntry() to reduce number of buttons
			pData->SEL.INPUT.uchMajor = FSC_TARE;
		case FSC_TARE:
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {    /* check item void or number */
                return(sRetStatus);                     
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT2)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (UifRegData.regsales.lQTY != 0L) {                   /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {            /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.lData == 0L && pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR); 
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            memset( &regmodifier, 0, sizeof( UIFREGMODIFIER ) );
            regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;       /* set class */
            regmodifier.uchMinorClass = CLASS_UITARE;
            regmodifier.lAmount = pData->SEL.INPUT.lData;           /* set input tare */
            if ( ( sRetStatus = ItemModTare( &regmodifier ) ) != UIF_SUCCESS ) {
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_MODIFIER;                      /* set modifier status */
            return(SUCCESS);                                        /* GCA */
       
		case FSC_CLEAR:                 /* clear key ? */
			// FSC_CLEAR may give Sequence Error on first press while Error Correct does not
			// so lets turn the FSC_CLEAR into FSC_EC so that clearing the last data entry
			// allowing a different entry will work should Clear Key be pressed.
			pData->SEL.INPUT.uchMajor = FSC_EC;    // Clear to Error Correct to handle properly
		case FSC_EC:                    /* error correct key */
			UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
			UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			return(UIF_DIA_SUCCESS);

		case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaNoPersonEntry() allowing consolidation of button types
		case FSC_CANCEL:
			UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
			UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
				return(UIF_DIA_SUCCESS);
			}
			memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
			memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
			UifRegDiaWork.auchFsc[0] = FSC_ERROR;
			//SR 143 HALO Override cwunn Reset status variable for RflHALO
			RflSetHALOStatus(0x00);
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
			memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
			memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
			//SR 143 HALO Override cwunn Reset status variable for RflHALO
			RflSetHALOStatus(0x00);
            return(UIFREG_ABORT);

        default:                                        
            return(LDT_SEQERR_ADR);
        }

    case UIM_REDISPLAY:
    case UIM_DRAWER:
        return(UieDefProc(pData));                  /* engine default process */

    case UIM_INIT:
    case UIM_CANCEL:
        break;

    default:                                        /* not use */
        return(LDT_SEQERR_ADR);
    }
    return (SUCCESS);
}