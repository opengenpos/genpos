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
* Program Name: PrREtk_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtEtkIn() : 
*      PrtEtkIn_RJ() : 
*      PrtEtkIn_VL() : 
*      PrtEtkIn_SP() : 
*      PrtEtkOut() : 
*      PrtEtkOut_RJ() : 
*      PrtEtkOut_VL() : 
*      PrtEtkOut_SP() : 
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jul-06-93 : 01.00.12 : R.Itoh     : initial                                   
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtEtkIn(), PrtEtkOut())
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-062 corr. (mod. PrtEtkIn(), PrtEtkOut())
* Apr-28-94 : 02.05.00 : Yoshiko.Jim: E-062 corr. (mod. PrtEtkIn())
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
#include<paraequ.h>
#include<para.h>
#include<csstbpar.h>
#include<regstrct.h>
#include<transact.h>
#include<prt.h>
#include<pmg.h>
#include"prtrin.h"
#include"prrcolm_.h"

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
            PrtEtkIn_SP(pTran, pItem);              /* E-062 corr. '94 4/28 */
        } else {
            PrtEtkIn_VL(pTran, pItem);
        }
        return ;
    }
    
    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtEtkIn_SP(pTran, pItem);
    }

    if ((fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {  /* receipt, journal print */
        PrtEtkIn_RJ(pTran, pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtEtkIn_RJ(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transactionn Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in .(receipt/journal)
*===========================================================================
*/
VOID  PrtEtkIn_RJ(TRANINFORMATION *pTran, ITEMMISC *pItem)
{

    PrtRJEtkCode(pItem, PRT_WTIN);         /* print employee, job code */ 

    PrtRJMnem(TRN_ETKIN_ADR, PRT_SINGLE);  /* ETK time-in line */

    PrtRJJobTimeIn(pTran, pItem, PRT_WTIN);  /* ETK time-in time */
}

/*
*===========================================================================
** Format  : VOID PrtEtkIn_SP( TRANINFORMATION  *pTran, ITEMMISC *pItem );      
*
*   Input  : TRANINFORMATION  *pTran    -Transaction Information address
*            ITEMMISC  *pItem           -Item Data address
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
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    /* -- slip release -- */
    PrtSlpRel();                            
}

/*
*===========================================================================
** Format  : VOID PrtEtkIn_VL( TRANINFORMATION  *pTran, ITEMMISC *pItem );      
*
*   Input  : TRANINFORMATION  *pTran    -Transaction Information address
*            ITEMMISC  *pItem           -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in .(validation)
*===========================================================================
*/
VOID   PrtEtkIn_VL(TRANINFORMATION *pTran, ITEMMISC *pItem)
{

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    PrtVLEtkTimeIn(pTran, pItem);               /* send ETK time-in line */

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */

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
        } else {
            PrtEtkOut_VL(pTran, pItem, sType);
        }
        return ;
    }
    
    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtEtkOut_SP(pTran, pItem, sType);
    }

    if ( (fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {    /* receipt, journal print */
        PrtEtkOut_RJ(pTran, pItem, sType);
    }

}


/*
*===========================================================================
** Format  : VOID PrtEtkOut_RJ(TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType);      
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
** Synopsis: This function prints ETK time-out .(receipt/journal)
*===========================================================================
*/
VOID  PrtEtkOut_RJ(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType)
{
    PrtRJEtkCode(pItem, sType);            /* print employee, job code */ 

    PrtRJMnem(TRN_ETKIN_ADR, PRT_SINGLE);  /* ETK time-in line */

    PrtRJJobTimeIn(pTran, pItem, sType);   /* ETK time-in time */

    PrtRJMnem(TRN_ETKOUT_ADR, PRT_SINGLE); /* ETK time-out line */

    PrtRJJobTimeOut(pTran, pItem);         /* ETK time-out time */
}


/*
*===========================================================================
** Format  : VOID PrtEtkOut_SP( TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType );      
*
*   Input  : TRANINFORMATION    *pTran     -Transaction information address
*            ITEMMISC           *pItem     -Item Data address
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
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    /* -- slip release -- */
    PrtSlpRel();                            
}

/*
*===========================================================================
** Format  : VOID PrtEtkOut_VL( TRANINFORMATION  *pTran, ITEMMISC *pItem, SHORT sType );      
*
*   Input  : TRANINFORMATION  *pTran    -Transaction Information address
*            ITEMMISC  *pItem           -Item Data address
*            SHORT      sType           -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in .(validation)
*===========================================================================
*/
VOID   PrtEtkOut_VL(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType)
{
    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    PrtVLEtkTimeOut(pTran, pItem, sType);       /* send ETK time-out line */

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
}


