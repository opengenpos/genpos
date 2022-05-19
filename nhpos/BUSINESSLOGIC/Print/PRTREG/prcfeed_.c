/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  feed
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrCFeed_.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*      PrtItemOther() : print items specified class "CLASS_ITEMOTHER"
*      PrtSlpRel()    : slip release
*      PrtInFeed()    : feed inside transaction
*      PrtOutFeed()   : feed outside transaction
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* May-15-92 : 00.00.01 : K.Nakajima :
* Jan-19-00 : 01.00.00 : hrkato     : Saratoga
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

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
/* #include <log.h> */
#include <prt.h>
#include <rfl.h>
#include <pmg.h>
#include <uie.h>
#include "prtrin.h"

/*
*===========================================================================
** Format  : VOID   PrtItemOther(ITEMOTHER *pItem);
*
*    Input : ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMOTHER" .
*===========================================================================
*/
VOID   PrtItemOther(ITEMOTHER  *pItem)
{
    switch (pItem->uchMinorClass) {
    case CLASS_SLIPRELEASE:
        PrtSlpRel();
        break;

    case CLASS_INSIDEFEED:
    case CLASS_LP_INSIDEFEED:   /* Saratoga */
        PrtInFeed(pItem);
        break;

    case CLASS_OUTSIDEFEED:
        PrtOutFeed(pItem);
        break;

    case CLASS_RESETLOG:    /* V3.3 */
        PrtResetLog(pItem);
        break;

    case CLASS_POWERDOWNLOG:    /* Saratoga */
        PrtPowerDownLog(pItem);
        break;

    default:
        break;
    }
}


/*
*===========================================================================
** Format  : VOID   PrtSlpRel(VOID);
*
*    Input : ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function makes slip release.
*===========================================================================
*/
VOID PrtSlpRel(VOID)
{
	TCHAR  achWork[8] = {0};
    USHORT  usFeedLine;

    achWork[0] = ESC;
    achWork[1] = _T('h');
    achWork[2] = 1;
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, (USHORT)_tcslen(achWork));  /* set back */

    /* -- decide the lines to feed back -- */
    usFeedLine = usPrtSlipPageLine % PRT_SLIP_MAX_LINES + PRT_SP_BACKFEED;
    PrtFeed(PMG_PRT_SLIP, usFeedLine);   /* back feed */

    achWork[0] = ESC;
    achWork[1] = _T('h');
    achWork[2] = _T('\0');
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, (USHORT)_tcslen(achWork));  /* release back */

    achWork[0] = ESC;
    achWork[1] = _T('q');
    achWork[2] = _T('\0');
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, (USHORT)_tcslen(achWork));  /* praten open */
}

/*
*===========================================================================
** Format  : VOID   PrtInFeed(ITEMOTHER *pItem);
*
*    Input : ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function feeds specefied printer's paper.
*===========================================================================
*/
VOID PrtInFeed(ITEMOTHER  *pItem)
{
//    USHORT usRet;
//    UCHAR  fbPrtStatus;                         /* printer status */
    USHORT usPrevious;
    USHORT  usColumn = 0;
    USHORT  fbStatus;

    if ( pItem->fsPrintStatus & PRT_SLIP ) {

		/* if slip printer is not prepaired, not call slip function, 09/11/01 */
    	if (PmgPrtConfig(PMG_PRT_SLIP, &usColumn, &fbStatus) != PMG_SUCCESS) {

        	usColumn = 0;   /* assume as thermal printer by default */
	    }
    	if (usColumn) {

	        /* allow power switch at error display */
    	    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

        	for (;;) {

            	PrtSlipPmgWait();                   /* wait spooler empty */

	            /* -- get printer status -- */
//    	        usRet = PmgGetStatus(PMG_PRT_SLIP, &fbPrtStatus);

  //      	    if (usRet == PMG_SUCCESS) {

 //           	    /* -- form in? -- */
 //               	if ((fbPrtStatus & PRT_STAT_FORM_IN) && (fbPrtStatus & PRT_STAT_ERROR)) {

                    	PrtFeed(PMG_PRT_SLIP, (USHORT)(pItem->lAmount));    /* feed */

	                    usPrtSlipPageLine = usPrtSlipPageLine / PRT_SLIP_MAX_LINES;        /* clear line area */
    	                usPrtSlipPageLine = usPrtSlipPageLine * PRT_SLIP_MAX_LINES;

        	            usPrtSlipPageLine += (USHORT)(pItem->lAmount);      /* add no. of feed lines */

            	        break;
//                	} else {

                    	/* allow power switch at error display */
	                    //PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
    	                /* -- require form in -- */
        	            //UieErrorMsg(LDT_SLPINS_ADR, UIE_WITHOUT);
            	    //}

	            //} else {

    	            /* allow power switch at error display */
        	        //PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
            	    /* display "printer trouble"  and wait clear */
                	//UieErrorMsg(LDT_PRTTBL_ADR, UIE_ABORT);
            	//}
            }

	        PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
	    }
    }

    if ( pItem->fsPrintStatus & PRT_RECEIPT ) {

        PrtFeed(PMG_PRT_RECEIPT, (USHORT)(pItem->lAmount)); /* feed */
    }

    if ( pItem->fsPrintStatus & PRT_JOURNAL ) {

        PrtFeed(PMG_PRT_JOURNAL, (USHORT)(pItem->lAmount)); /* feed */
    }

}

/*
*===========================================================================
** Format  : VOID  PrtOutFeed(ITEMOTHER *pItem);
*
*    Input : ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function feeds specefied printer.
*===========================================================================
*/
VOID  PrtOutFeed(ITEMOTHER  *pItem)
{
    PrtSetRJColumn();                           /* set printer's columns */

    if ( pItem->fsPrintStatus & PRT_RECEIPT )  {              /* receipt */
        PrtFeed(PMG_PRT_RECEIPT, (USHORT)(pItem->lAmount));   /* feed */

//        if ( pItem->fsPrintStatus & PRT_HEADER24_PRINT ) {
		/* check electro or 24char print */
		if ( CliParaMDCCheck( MDC_SLIP_ADR, EVEN_MDC_BIT3) != 0 ) {
			/* -- Electro not provided -- */
            PrtFeed(PMG_PRT_RECEIPT, 6);        /* feed */
            PrtRJHead();                        /* send header  */
//            PrtFeed(PMG_PRT_RECEIPT, 2);        /* feed */
            PrtCut();                           /* cut */
        } else {
			/* --  Electro provided -- */
            PrtElec();                          /* send electro */
            PrtFeed(PMG_PRT_RECEIPT, 12);       /* 10 line feed */
            PrtCut();                           /* cut */
        }
    }

    if ( pItem->fsPrintStatus & PRT_JOURNAL ) {        /* journal */
        PrtFeed(PMG_PRT_JOURNAL, (USHORT)(pItem->lAmount));   /* feed */
    }
}

/*===========================================================================
** Format  : VOID  PrtResetLog(ITEMOTHER *pItem)
*
*   Input  : TEMPRINT  *pItem
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints reset log on Journal.     V3.3 FVT#5
*===========================================================================
*/
VOID  PrtResetLog(ITEMOTHER *pItem)
{
    if (pItem->uchAction == UIF_UCHRESETLOG_LOGONLY) {
        PrtRJResetLog3(pItem);
	} else if(pItem->uchAction == UIF_UCHRESETLOG_TRAILERONLY) {
        PrtRJResetLog4(pItem);
    } else {
        PrtRJResetLog(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID   PrtPowerDownLog(ITEMOTHER *pItem)
*
*   Input  : TEMPRINT  *pItem
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints power down log on Journal.    Saratoga
*===========================================================================
*/
VOID    PrtPowerDownLog(ITEMOTHER *pItem)
{
    if (pItem->uchAction == FSC_MINOR_POWER_DOWN || pItem->uchAction == FSC_MINOR_SUSPEND) {
        PrtRJPowerDownLog(pItem);
    }
}

/*** End of File */