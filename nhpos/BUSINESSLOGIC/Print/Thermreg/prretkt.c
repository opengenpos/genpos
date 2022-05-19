/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  misc                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrREtkT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtEtkIn() : 
*      PrtEtkIn_TH() : 
*      PrtEtkIn_EJ() : 
*      PrtEtkIn_SP() : 
*      PrtEtkOut() : 
*      PrtEtkOut_TH() : 
*      PrtEtkOut_EJ() : 
*      PrtEtkOut_SP() : 
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-30-93 : 01.00.12 :  R.Itoh    : initial                                   
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtEtkIn(), PrtEtkOut())
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-062 corr. (mod. PrtEtkIn(), PrtEtkOut())
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
#include<ecr.h>
#include <rfl.h>
#include<paraequ.h>
#include<para.h>
#include<csstbpar.h>
#include<regstrct.h>
#include<transact.h>
#include<pmg.h>
#include<prt.h>
#include"prtrin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID   PrtEtkIn(TRANINFORMATION  *pTran, ITEMMISC *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints ETK time in.
*===========================================================================
*/
VOID  PrtEtkIn(TRANINFORMATION  *pTran, ITEMMISC *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
    
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
            PrtEtkIn_SP(pTran, pItem);
        }
        return;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtEtkIn_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
        PrtEtkIn_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
        PrtEtkIn_EJ(pTran, pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtEtkIn_TH(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transactionn Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in .(thermal)
*===========================================================================
*/
VOID  PrtEtkIn_TH(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
	if (pItem->uchMinorClass == CLASS_SIGNIN) {
		return;
	}

    PrtTHHead(pTran);                      /* print header if necessary */  

    PrtTHEtkCode(pItem, PRT_WTIN);         /* print employee, job code */ 

	if (pItem->aszNumber[0]) {
		PrtPrint(PMG_PRT_RECEIPT, pItem->aszNumber, tcharlen(pItem->aszNumber));  // print Time-in method mnemonic if specified
	}

    PrtTHMnem(TRN_ETKIN_ADR, PRT_SINGLE);  /* ETK time-in line */

    PrtTHJobTimeIn(pTran, pItem);          /* ETK time-in time */
}

/*
*===========================================================================
** Format  : VOID PrtEtkIn_EJ(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information Address
*          : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in .(electric journal)
*===========================================================================
*/
VOID  PrtEtkIn_EJ(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
	USHORT  usAddress = TRN_ETKIN_ADR;

    PrtEJEtkCode(pItem, PRT_WTIN);         /* print employee, job code */ 

	if (pItem->aszNumber[0]) {
		PrtPrint(PMG_PRT_JOURNAL, pItem->aszNumber, tcharlen(pItem->aszNumber));  // print Time-in method mnemonic if specified
	}

	if (pItem->uchMinorClass == CLASS_SIGNIN) {
		usAddress = TRN_SIGNIN_ADR;
	}

    PrtEJMnem(usAddress, PRT_SINGLE);      /* ETK time-in or Operator sign-in line */

    PrtEJJobTimeIn(pTran, pItem, PRT_WTIN); /* ETK time-in time */
}

/*
*===========================================================================
** Format  : VOID PrtEtkIn_SP( TRANINFORMATION  *pTran, ITEMMISC *pItem );      
*
*   Input  : ITEMMISC  *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in .(slip)
*===========================================================================
*/
VOID PrtEtkIn_SP( TRANINFORMATION  *pTran, ITEMMISC  *pItem )
{
    TCHAR  aszSPPrintBuff[1][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* -- initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
                                                
    /* -- set time-in data -- */                                                
    usSlipLine += PrtSPEtkTimeIn(aszSPPrintBuff[usSlipLine], pTran, pItem);
                                                
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    /* -- slip release -- */
    PrtSlpRel();                            

}

/*
*===========================================================================
** Format  : VOID   PrtEtkOut(TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*            SHORT              sType      -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints ETK time out.
*===========================================================================
*/
VOID  PrtEtkOut(TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
    
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
            PrtEtkOut_SP(pTran, pItem, sType);
        }
        return;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtEtkOut_SP(pTran, pItem, sType);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
        PrtEtkOut_TH(pTran, pItem, sType);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
        PrtEtkOut_EJ(pTran, pItem, sType);
    }
}


/*
*===========================================================================
** Format  : VOID PrtEtkOut_TH(TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType);      
*
*   Input  : TRANINFORMATION  *pTran     -Transactionn Information address
*            ITEMMISC         *pItem     -Item Data address
*            SHORT              sType      -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-out .(thermal)
*===========================================================================
*/
VOID  PrtEtkOut_TH(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType)
{
	if (pItem->uchMinorClass == CLASS_SIGNOUT) {
		return;
	}

    PrtTHHead(pTran);                      /* print header if necessary */  
    
    PrtTHEtkCode(pItem, sType);            /* print employee, job code */ 

	if (pItem->aszNumber[0]) {
		PrtPrint(PMG_PRT_RECEIPT, pItem->aszNumber, tcharlen(pItem->aszNumber));  // print Time-out method mnemonic if specified
	}

    PrtTHEtkOut(TRN_ETKIN_ADR, TRN_ETKOUT_ADR); /* ETK time-out line */

    PrtTHJobTimeOut(pTran, pItem, sType);  /* ETK time-out time */
}

/*
*===========================================================================
** Format  : VOID PrtEtkOut_EJ(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*          : ITEMMISC         *pItem     -Item Data address
*            SHORT              sType      -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-out .(electric journal)
*===========================================================================
*/
VOID  PrtEtkOut_EJ(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType)
{
	USHORT  usAddress = TRN_ETKIN_ADR;

    PrtEJEtkCode(pItem, sType);            /* print employee, job code */ 

	if (pItem->aszNumber[0]) {
		PrtPrint(PMG_PRT_JOURNAL, pItem->aszNumber, tcharlen(pItem->aszNumber));  // print Time-out method mnemonic if specified
	}

	if (pItem->uchMinorClass == CLASS_SIGNOUT) {
		usAddress = TRN_SIGNOUT_ADR;
	}

    PrtEJMnem(usAddress, PRT_SINGLE);  /* ETK time-in line */

	if (pItem->uchMinorClass != CLASS_SIGNOUT) {
		PrtEJJobTimeIn(pTran, pItem, sType);   /* ETK time-in time */

		PrtEJMnem(TRN_ETKOUT_ADR, PRT_SINGLE); /* ETK time-out line */
	}

    PrtEJJobTimeOut(pTran, pItem);         /* ETK time-out time */
}

/*
*===========================================================================
** Format  : VOID PrtEtkOut_SP( TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType );      
*
*   Input  : ITEMMISC  *pItem     -Item Data address
*            SHORT              sType      -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-out .(slip)
*===========================================================================
*/
VOID PrtEtkOut_SP( TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType )
{
    TCHAR  aszSPPrintBuff[1][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* -- initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
                                                
    /* -- set time-in data -- */                                                
    usSlipLine += PrtSPEtkTimeOut(aszSPPrintBuff[usSlipLine], pTran, pItem, sType);
                                                
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    /* -- slip release -- */
    PrtSlpRel();                            

}
