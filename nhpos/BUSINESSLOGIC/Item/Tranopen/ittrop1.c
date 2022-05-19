/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP1.C (newcheck)                                       
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
:  Abstract:
:   ItemTransOpenNC()       ;   newcheck
:   ItemTransNCCheck()      ;   check newcheck operation
:   ItemTransNCKey()        ;   newcheck key sequence
:   ItemTransNewCheck()     ;   generate transaction open data
:   ItemTransNCPara()       ;   set current qualifier
:
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date    : Ve.Rev.  : Name        : Description
:  2-14-95  : 03.00.00 : hkato       : R3.0
: 11-10-95  : 03.01.00 : hkato       : R3.1
:  7-31-98  : 03.03.00 : hrkato      : V3.3 (New Key Sequence)
:  8-13-99  : 03.05.00 : K.Iwata     : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "uireg.h"
#include    "pif.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csstbgcf.h"
#include    "csgcs.h"
#include    "rfl.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "itmlocal.h"
#include	"cscas.h"


/*==========================================================================
**   Synopsis:  SHORT   ItemTransOpenNC( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    newcheck operation triggered by pressing FSC_NEW_CHK
==========================================================================*/
        
SHORT   ItemTransOpenNC(UIFREGTRANSOPEN *UifRegTransOpen)
{
    SHORT   sStatus;    

    if ( RflGetSystemType () == FLEX_STORE_RECALL ) {               /* store/recall system */
        return( LDT_PROHBT_ADR );                                   /* Prohibit Operation */
    }

    if ( ItemModLocalPtr->fsTaxable ) {                             /* check taxable modifier key entry */
		/* sequence error if tax modifier key has been used */
        return( LDT_SEQERR_ADR );
    }
    if ( UifRegTransOpen->fbModifierTransOpen ) {
		/* sequence error if Void key has been used */
        return( LDT_SEQERR_ADR );
    }

    /* prohibit guest check open, V3.3 */
    if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_GUEST_CHECK_OPEN) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier setting CAS_NOT_GUEST_CHECK_OPEN prohibits.");
        return(LDT_PROHBT_ADR);
    }

    if ((sStatus = ItemTransNCKey(UifRegTransOpen)) != ITM_SUCCESS) {
        return(sStatus);
    }

    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransNCKey( UIFREGTOTAL *UifRegTransOpen,
*                           ItemTransOpen *ItemTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen, ITEMTRANSOPEN *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    key sequence of newcheck operation
==========================================================================*/

SHORT   ItemTransNCKey(UIFREGTRANSOPEN *UifRegTransOpen)
{
    SHORT           sStatus;
    USHORT          usStatus;
    USHORT          usGuestNo = (USHORT)UifRegTransOpen->ulAmountTransOpen;
	REGDISPMSG      DispMsg = {0};
	ITEMTRANSOPEN   ItemTransOpen = {0};


	if (! CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT0)) {
		// if manual guest check number then get the guest check number and
		// validate it before asking for table number, etc.
		ULONG   ulGuestNo;

        if ( ( sStatus = ItemTransGCFNo( &ulGuestNo ) ) != ITM_SUCCESS ) {
            ItemTransDispDef();   
            return( sStatus );            
        }
        if ( ulGuestNo > GCF_MAX_CHK_NUMBER ) {                     /* max check */
            return( LDT_KEYOVER_ADR );                              /* over digit error */
        }

        usGuestNo = ( USHORT )ulGuestNo;
        if ( TranModeQualPtr->usFromTo[0] || TranModeQualPtr->usFromTo[1] ) {
            if ( TranModeQualPtr->usFromTo[0] > usGuestNo || TranModeQualPtr->usFromTo[1] < usGuestNo ) {
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  Cashier GC range prohibits GC # entered.");
                return( LDT_PROHBT_ADR );                           /* prohibit */
            }
        }
	}

    if ( UifRegTransOpen->usNoPerson == 0 ) {
		// if number of person is not specified then ask for it 
		ItemTransOpen.usNoPerson = 1;                               /* default */
        if ((sStatus = ItemTransNoPerson(&ItemTransOpen)) != ITM_SUCCESS) {  /* V3.3 */
            return(sStatus);
        }
    } else {                                                        /* "exist No of person" */
        ItemTransOpen.usNoPerson = UifRegTransOpen->usNoPerson;    /* No of person */    
    }

	ItemTransOpen.usTableNo = 0;                               /* default */
    if ((sStatus = ItemTransTableNo(&ItemTransOpen)) != ITM_SUCCESS) {   /* V3.3 */
        return(sStatus);
    }

    /*----- Seat Number Entry for SplitCheck,   R3.1 -----*/
	ItemTransOpen.uchSeatNo = 0;
    if (RflGetSystemType () == FLEX_POST_CHK && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
        if (CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT1)) {
            if ((sStatus = ItemTransSeatNo(&ItemTransOpen, 0)) != ITM_SUCCESS) {
                return(sStatus);
            }
        }
    }

    if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT0)) {             /* auto GCF, R3.1 */
        if ( ( sStatus = CliGusGetAutoNo( &usGuestNo ) ) != GCF_SUCCESS ) {        
            usStatus = GusConvertError( sStatus );
            return( ( SHORT )usStatus );            
        }
    } else {                                                        /* manual GCF */
        if ( ( sStatus = CliGusManAssignNo( usGuestNo ) ) != GCF_SUCCESS ) {
            usStatus = GusConvertError( sStatus );
            return( ( SHORT )usStatus );            
        }
    }

	ItemTransOpen.usGuestNo = usGuestNo;                       /* GCF No */

    DispMsg.uchMajorClass = UifRegTransOpen->uchMajorClass;
    DispMsg.uchMinorClass = CLASS_UINEWCHECK;
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);
    flDispRegDescrControl |= NEWCHK_CNTRL;                          /* compulsory control */ 
    flDispRegKeepControl |= NEWCHK_CNTRL;                           /* compulsory control */ 
    DispMsg.fbSaveControl = 0;                                      /* save for redisplay */
    DispWrite( &DispMsg );                                          /* display check total */

    ItemTransNCCommon(&ItemTransOpen);                      /* V3.3 */
    return( ITM_SUCCESS );
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransNewCheck( ItemTransOpen *ItemTransOpen )   
*
*   Input:      ITEMTRANSOPEN *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    generate transaction open data
==========================================================================*/

VOID    ItemTransNewCheck( ITEMTRANSOPEN *ItemTransOpen )   
{
    ItemTransOpen->uchMajorClass = CLASS_ITEMTRANSOPEN;
    ItemTransOpen->uchMinorClass = CLASS_NEWCHECK;
    ItemTransOpen->ulCashierID = TranModeQualPtr->ulCashierID;
    ItemTransOpen->ulWaiterID = TranModeQualPtr->ulWaiterID;

    if ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT1 ) ) {         /* CDV */
        ItemTransOpen->uchCdv = RflMakeCD( ItemTransOpen->usGuestNo );
    }

//	{
//		DATE_TIME   WorkDate;

//		PifGetDateTime(&WorkDate);                                      /* get TOD */
//		ItemTransOpen->auchCheckOpen[0] = (UCHAR)WorkDate.usHour;       /* hour */
//		ItemTransOpen->auchCheckOpen[1] = (UCHAR)WorkDate.usMin;        /* minute */
//		ItemTransOpen->auchCheckOpen[2] = (UCHAR)WorkDate.usSec;        /* second, R3.1 */

//		ItemTransOpen->auchCheckHashKey[0] = ( UCHAR )(WorkDate.usMonth + WorkDate.usSec + '+'); /* month mangled */
//		ItemTransOpen->auchCheckHashKey[1] = ( UCHAR )(WorkDate.usMDay + 'P');  /* day mangled */
//		ItemTransOpen->auchCheckHashKey[2] = ( UCHAR )(WorkDate.usHour + WorkDate.usSec + '!');  /* hour mangled */
//		ItemTransOpen->auchCheckHashKey[3] = ( UCHAR )(WorkDate.usMin + 'A');   /* minute mangled */
//		ItemTransOpen->auchCheckHashKey[4] = ( UCHAR )(WorkDate.usSec + '0');   /* Second mangled */
//		ItemTransOpen->auchCheckHashKey[5] = ( UCHAR )0;                      /* end of string */
//	}

	ItemTransOpen->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

//    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {      /* R3.0 */
//        ItemTransOpen->usTableNo = ItemCommonUniqueTransNo();
//    }

    ItemTransOpen->uchTeamNo = TranModeQualPtr->uchCasTeamNo;         /* save team no. V3.3 */
	ItemTransOpen->uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;
}



/*==========================================================================
**   Synopsis:  VOID    ItemTransNCPara( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    set transaction open data
==========================================================================*/

VOID    ItemTransNCPara( VOID )   
{
    ItemCurPrintStatus();                                           /* set current print system */
    ItemCountCons();                                                /* consecutive No */
}

/****** End of Source ******/
