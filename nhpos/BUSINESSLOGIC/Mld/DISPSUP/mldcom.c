/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Entry Module for Supervisor Mode
* Category    : Multiline Display, NCR 2170 US Hospitality Application         
* Program Name: MLDCOM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               MldDispItem()           : Super Maint Display for MLD
*               UifACPGMLDDispCom()     : Super Prog Mode Display Common
*               UifACPGMLDClear()       : Super Prog Mode Clear Common
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
*    -  -95:00.00.01    :M.Waki     : Init
* Jul-20-95:03.00.01    :M.Ozawa    : Add Report Scrolling by One Page Feature.
* Aug-11-99:03.05.00    :M.Teraki   : Remove WAITER_MODEL
* Aug-16-99:03.05.00    :M.Teraki   : Merge STORE_RECALL_MODEL
*          :            :           :       /GUEST_CHECK_MODEL
* 
*** NCR2171 **
* Aug-26-99:01.00.00    :M.Teraki   : initial (for Win32)
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>
#include <ecr.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <csstbopr.h>
#include <report.h>
#include <prt.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>
#include <csstbfcc.h>
#include <csstbept.h>
#include <mldmenu.h>

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/*
*===========================================================================
** Format  : VOID   MldDispItem(VOID *pItem, USHORT usStatus);      
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT            usStatus
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from application user and display item for MLD.
*===========================================================================
*/
SHORT MldDispItem(VOID *pItem, USHORT usStatus)
{
    SHORT sReturn = MLD_SUCCESS;    /* aborted status in reporting */

    switch ( ((MLDITEMDATA *)pItem)->uchMajorClass ) {

    case CLASS_MAINTHEADER:
        MldRptSupHeader(( MAINTHEADER *)pItem);
        break;

    case CLASS_MAINTTRAILER:
        MldRptSupTrailer(( MAINTTRAILER *)pItem);
        break;

    case CLASS_MAINTERRORCODE:
        MldRptSupErrorCode(( MAINTERRORCODE *)pItem);
        break;

    case CLASS_MAINTTRANS:
        MldRptSupTrans(( MAINTTRANS *)pItem, usStatus);
        break;

    case CLASS_ITEMOTHER:               /* feed */
        sReturn = MldIRptItemOther(usStatus);
        break;

    case CLASS_RPTGUEST:
        sReturn = MldRptSupGCF(( RPTGUEST *)pItem);
        break;

    case CLASS_RPTCASHIER:
    case CLASS_RPTWAITER:
    case CLASS_RPTEMPLOYEENO:
        sReturn = MldRptSupCashWaitFile(pItem);
        break;

    case CLASS_RPTREGFIN:
        sReturn = MldRptSupRegFin(( RPTREGFIN *)pItem);
        break;                                      

    case CLASS_RPTHOURLY:
        sReturn = MldRptSupHourlyAct(( RPTHOURLY *)pItem);
        break;

    case CLASS_RPTDEPT:
        sReturn = MldRptSupDEPTFile(( RPTDEPT *)pItem, usStatus);
        break;

    case CLASS_RPTPLU:
        sReturn = MldRptSupPLUFile(( RPTPLU *)pItem, usStatus);
        break;

    case CLASS_RPTCPN:
        sReturn = MldRptSupCPNFile(( RPTCPN *)pItem, usStatus);
        break;                                    

    case CLASS_RPTEMPLOYEE:
        sReturn = MldRptSupETKFile(( RPTEMPLOYEE *)pItem);
        break;

    case CLASS_RPTEJ:
        sReturn = MldRptSupEJ(( RPTEJ *)pItem);
        break; 

    case CLASS_MAINTOPESTATUS:         // MldDispItem() - 
        sReturn = MldRptSupOpeStatus(pItem);
        break;                                   

    case CLASS_RPTCONFIGURATION:
        sReturn = MldRptSupConfig(( RPTEJ *)pItem);
        break; 

    case CLASS_RPTSERVICE:
        sReturn = MldRptSupServiceTime(( RPTSERVICE *)pItem);
        break;

    case CLASS_RPTPROGRAM:
        sReturn = MldRptSupProgRpt(( RPTPROGRAM *)pItem);
        break;

    /*======================================================================*\
                       F O R   T H E   M A I N T E N A N C E
    \*======================================================================*/
    case CLASS_PARACASHIERNO:                            /* A/C 20          */
        MldParaCASDisp((MAINTCASHIERIF *)pItem);
        break;

    case CLASS_PARAPLU:                                  /* A/C 63,64,68,82 */
    case CLASS_MAINTPLU:
        MldParaPLUDisp((PARAPLU *)pItem, usStatus);
        break;

    case CLASS_PARAPPI:                                 /* A/C 71           */
        MldParaPPI(( PARAPPI *)pItem, usStatus);
        break;

    case CLASS_PARADEPT:                                 /* A/C 114         */
        MldParaDEPTDisp((PARADEPT *)pItem);
        break;

    case CLASS_PARAMENUPLUTBL:                           /* A/C 116         */
        MldParaMenuPluDisp((PARAMENUPLUTBL *)pItem);
        break;

    case CLASS_MAINTTAXTBL1:                             /* A/C 124         */
    case CLASS_MAINTTAXTBL2:                             /* A/C 125         */
    case CLASS_MAINTTAXTBL3:                             /* A/C 126         */
        MldParaTaxDisp((MAINTTAXTBL *)pItem);
        break;

    case CLASS_PARAEMPLOYEENO:                           /* A/C 152         */
        MldParaEmpNoDisp((PARAEMPLOYEENO *)pItem,usStatus);
        break;

    case CLASS_MAINTETKFL:                              /* A/C 153          */
        MldMaintETKFl(( MAINTETKFL *)pItem, usStatus);
        break;

    case CLASS_PARAOEPTBL:                               /* A/C 160         */
        MldParaOEPDisp((PARAOEPTBL *)pItem);
        break;

    case CLASS_PARACPN:                                  /* A/C 161         */
        MldParaCPNDisp((PARACPN *)pItem, usStatus);
        break;
	case CLASS_RPTSTOREFORWARD:
		// MldStoreForwardDisp(pItem, usStatus);   // remove legacy Store and Forward for OpenGenPOS Rel 2.4.0  03/30/2021  Richard Chambers
		break;

    default:
        break;                         
    }

    return (sReturn);
}


/*
*=============================================================================
**  Synopsis:    VOID UifACPGMLDDispCom(UCHAR FAR *pForm) 
*
*       Input:  *pForm        : Pointer to String for Display Format
*      Output:  nothing
*
**  Return:     nothing 
*               
**  Description: MLD Display Common Proccess
*===============================================================================
*/
UCHAR  UifACPGMLDDispCom(CONST MLDMENU * * pForm) 
{
    static TCHAR FAR    aszMldDispSup[] = {_T("%s\t%s")};

    SHORT   i, sReturn;
    MLDIF   MldIf;
     
    MldScrollClear(MLD_SCROLL_1);   /* Scroll Display Clear */

    for (i=0; pForm[i]->usAddress != 0; i++){
        /* get programmable mnemonics, V3.3 */
        MldIf.usAddress = pForm[i]->usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }

        MldPrintf( MLD_SCROLL_1, (SHORT)(MLD_SCROLL_TOP+i), aszMldDispSup, MldIf.aszMnemonics, pForm[i]->aszData);    /* display one line */
    }

	if (pForm[i+1]->usAddress)
		return i+1;
	else
		return 0;
}

/*
*=============================================================================
**  Synopsis:    VOID UifACPGMLDClear(VOID) 
*
*       Input:  nothing
*      Output:  nothing
*
**  Return:     nothing 
*               
**  Description: MLD Clear
*===============================================================================
*/
VOID UifACPGMLDClear(VOID) 
{

    MldDisplayInit(MLD_SCROLL_1, 0);   /* Scroll Display Clear */

}

/**** End of File ****/
