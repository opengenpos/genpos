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
* Program Name: PrCFeedT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
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
* Jun-14-93 : 00.00.01 : R.Itoh     :                                    
* Jan-20-00 : 01.00.00 : hrkato     : Saratoga
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
#include <prt.h>
#include <shr.h>
#include <pmg.h>
#include <uie.h>
#include "prtcom.h"
#include "prtrin.h"
#include "prtshr.h"
#include "BlFWif.h"

/*
*===========================================================================
** Format  : VOID   PrtItemOther(TRANINFORMATION *pTran, ITEMOTHER *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMOTHER" .
*===========================================================================
*/
VOID   PrtItemOther(TRANINFORMATION *pTran, ITEMOTHER  *pItem)
{
    switch (pItem->uchMinorClass) {

    case CLASS_SLIPRELEASE:
        PrtSlpRel();
        break;

    case CLASS_INSIDEFEED:
    case CLASS_LP_INSIDEFEED:   /* Saratoga */
        PrtInFeed(pTran, pItem);
        break;

    case CLASS_OUTSIDEFEED:
        PrtOutFeed(pTran, pItem);
        break;

    case CLASS_RESETLOG:    /* V3.3 */
        PrtResetLog(pTran, pItem);
        break;

    case CLASS_POWERDOWNLOG:/* Saratoga */
        PrtPowerDownLog(pTran, pItem);
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

	BlFwBeginRemoval(2000);
	BlFwEndRemoval();
#if 0
    TCHAR  achWork[4];
    PARASLIPFEEDCTL  FeedCtl; 
    USHORT  usFeedLine;
    USHORT  usMaxLine;
    TCHAR    achEject[1];

#if defined (PRT_7158SLIP)
    /* -- get max slip line -- */
/*    memset(&FeedCtl, '\0', sizeof(PARASLIPFEEDCTL)); */
    FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;
    FeedCtl.uchAddress = SLIP_MAXLINE_ADR;   
    CliParaRead(&FeedCtl);    
    usMaxLine = (USHORT)FeedCtl.uchFeedCtl;

#if defined (USE_2170_ORIGINAL)
    /* -- decide the lines to feed back -- */
    usFeedLine = usPrtSlipPageLine % 100 + PRT_SP_BACKFEED;

    achWork[0] = ESC;       /* ESC: printer escape sequence */
    achWork[1] = _T('e');       /* back feed of 7158 */
    achWork[2] = (TCHAR)usFeedLine;
    achWork[3] = _T('\0');

    PrtPrint( PMG_PRT_SLIP, achWork, sizeof(achWork));

    achWork[0] = ESC;       /* ESC: printer escape sequence */
    achWork[1] = _T('c');
    achWork[2] = _T('0');
    achWork[3] = 0x02;      /* return to receipt */

    PmgPrint( PMG_PRT_RECEIPT, achWork, sizeof(achWork));
#else
    /* --- Fix a glich (03/30/2001)
        When using reverse feed command, slip paper should be
        covered on both leading & trailing edge sensor.
        If slip paper is on out of sensor, 7158 cannot respond
        printer status to application. This situation produces
        infinity loop of "PLS CHK PRINTER" error condition.
    --- */
    usFeedLine = usPrtSlipPageLine % 100;

    achWork[0] = ESC;       /* ESC: printer escape sequence */
    achWork[1] = _T('e');       /* back feed of 7158 */
    achWork[2] = (TCHAR)usFeedLine;
    achWork[3] = _T('\0');

    PmgPrint( PMG_PRT_SLIP, achWork, sizeof(achWork));

    /* --- Fix a glitch (03/30/2001)
        To exhost slip paper completely and change printing
        station to receipt, send "Eject Slip" command.
        Even if slip paper is out of sensor,  7158 will not be
        in error state. The reason why 7158 selects receipt station
        automatically after the command is received.
    --- */

    achEject[0] = 0x0c;

    PmgPrint( PMG_PRT_SLIP, achEject, sizeof(achEject));
#endif
#else

    achWork[0] = ESC;
    achWork[1] = _T('h');
    achWork[2] = 1;
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, sizeof(achWork));  /* set back */

    /* -- get max slip line -- */
/*    memset(&FeedCtl, '\0', sizeof(PARASLIPFEEDCTL)); */
    FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;
    FeedCtl.uchAddress = SLIP_MAXLINE_ADR;   
    CliParaRead(&FeedCtl);    
    usMaxLine = (USHORT)FeedCtl.uchFeedCtl;

    /* -- decide the lines to feed back -- */
    usFeedLine = usPrtSlipPageLine % 100 + PRT_SP_BACKFEED;
    PrtFeed(PMG_PRT_SLIP, usFeedLine);   /* back feed */

    achWork[0] = ESC;
    achWork[1] = _T('h');
    achWork[2] = _T('\0');
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, sizeof(achWork));  /* release back */

    achWork[0] = ESC;
    achWork[1] = _T('q');
    achWork[2] = _T('\0');
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, sizeof(achWork));  /* praten open */
    
#endif
#endif//0
}

/*
*===========================================================================
** Format  : VOID   PrtInFeed(TRANINFORMATION *pTran, ITEMOTHER *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction Information address
*    Input : ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none                  
*
** Synopsis: This function feeds specefied printer's paper.
*===========================================================================
*/
VOID PrtInFeed(TRANINFORMATION *pTran, ITEMOTHER  *pItem)
{
//    USHORT usRet;
//    UCHAR  fbPrtStatus;                         /* printer status */
    UCHAR   uchTrgPrt;
    USHORT  usPrevious;
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
            	PrtSlipPmgWait();                    /* wait spooler empty */

	            /* -- get printer status -- */
//    	        usRet = PmgGetStatus(PMG_PRT_SLIP, &fbPrtStatus);

        	//    if (usRet == PMG_SUCCESS) {

            	    /* -- form in? -- */
                	//if ((fbPrtStatus & PRT_STAT_FORM_IN) && (fbPrtStatus & PRT_STAT_ERROR)) {
                    	PrtFeed(PMG_PRT_SLIP, (USHORT)(pItem->lAmount));    /* feed */
	                    usPrtSlipPageLine = usPrtSlipPageLine / PRT_SLIP_MAX_LINES;        /* clear line area */
    	                usPrtSlipPageLine = usPrtSlipPageLine * PRT_SLIP_MAX_LINES;
        	            usPrtSlipPageLine += (USHORT)(pItem->lAmount);      /* add no. of feed lines */
            	        break;

                //	} else {
                    	/* allow power switch at error display */
	        //            PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    	                /* -- require form in -- */
        	 //           UieErrorMsg(LDT_SLPINS_ADR, UIE_WITHOUT);
            //	    }

	         //   } else {
    	            /* allow power switch at error display */
        	//        PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

            	    /* display "printer trouble"  and wait clear */
             //   	UieErrorMsg(LDT_PRTTBL_ADR, UIE_ABORT);
	         //   }
    	    }
        	PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
        }
    }

    if ( pItem->fsPrintStatus & PRT_RECEIPT ) {

        if (pItem->uchMinorClass == CLASS_LP_INSIDEFEED) {  /* Saratoga */
            if (usPrtSlipPageLine != 0) {                   /* Option/Comp Slip Print */
                return;
            }
        }

        /* -- check shared printer or not -- */
        if ( fbPrtAltrStatus & PRT_NEED_ALTR ) {
            uchTrgPrt = PRT_WITHALTR;
        } else {
            uchTrgPrt = PRT_WITHSHR;
        }

        if ( 0 != PrtCheckShr(uchTrgPrt) ) {

            fbPrtShrStatus |= PRT_SHARED_SYSTEM; 

            if ( fbPrtTHHead != PRT_HEADER_PRINTED ) {
                PrtShrInit(pTran);
                PrtFeed(PMG_PRT_RECEIPT, (USHORT)(pItem->lAmount)); /* feed */
                PrtShrEnd();
                return;
            }
        }            

        PrtFeed(PMG_PRT_RECEIPT, (USHORT)(pItem->lAmount)); /* feed */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtOutFeed(TRANINFORMATION *pTran, ITEMOTHER *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction Information address
*    Input : ITEMOTHER        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none                  
*
** Synopsis: This function feeds specefied printer.
*===========================================================================
*/
VOID  PrtOutFeed(TRANINFORMATION *pTran, ITEMOTHER  *pItem)
{

    if ( pItem->fsPrintStatus & PRT_RECEIPT )  {              /* receipt */

        PrtTHHead(pTran);                    /* print header */        

        PrtFeed(PMG_PRT_RECEIPT, (USHORT)(pItem->lAmount));   /* feed */

//        PrtFeed(PMG_PRT_RECEIPT, 3+1);        /* feed, for 7158, saratoga */

       	//we do this end here because the usual transaction end is called on a trailer message,
		//but because print demand will not have a trailer, we do it here. JHHJ
		PrtTransactionEnd(PRT_REGULARPRINTING,pTran->TranGCFQual.usGuestNo);

        fbPrtTHHead = 0;                    /* header not printed */

        PrtShrEnd();

        if ( !(fbPrtShrStatus & PRT_DEMAND) ) {
            /* -- reset shared/normal, aborted, alternation flag -- */
            fbPrtShrStatus = 0; 
            fbPrtAbortStatus = 0;
            fbPrtAltrStatus = 0;            
        }

    }
}

/*
*===========================================================================
** Format  : VOID  PrtResetLog(ITEMOTHER *pItem)
*
*   Input  : ITEMOTHER  *pItem
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints reset log on Journal.     V3.3 FVT#5
*===========================================================================
*/
VOID  PrtResetLog(TRANINFORMATION  *pTran, ITEMOTHER *pItem)
{
    PrtForceEJInit(pTran);                   /* initialize E/J print buffer */
	if (pItem->uchAction == UIF_UCHRESETLOG_LOGONLY) {
        PrtEJResetLog3(pItem);
	} else if (pItem->uchAction == UIF_UCHRESETLOG_TRAILERONLY) {
		PrtEJResetLog4(pItem);
    } else {
        PrtEJResetLog(pItem);
    }
    PrtEJSend();                        /* send to E/J module */
}

/*
*===========================================================================
** Format  : VOID   PrtPowerDownLog(TRANINFORMATION *pTran, ITEMOTHER *pItem)
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
VOID    PrtPowerDownLog(TRANINFORMATION *pTran, ITEMOTHER *pItem)
{
    PrtEJInit(pTran);
    if (pItem->uchAction == FSC_MINOR_POWER_DOWN || pItem->uchAction == FSC_MINOR_SUSPEND) {
        PrtEJPowerDownLog(pItem);
    }
    PrtEJSend();
}

/***** End of prcfeedt.c *****/