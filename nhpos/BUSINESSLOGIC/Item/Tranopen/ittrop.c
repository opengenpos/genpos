/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP.C (main/common)
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
:   ItemTransOpen()         ;   transaction open module main
:   ItemTransGCFNo()        ;   get GCF No   
:   ItemTransDispDef()      ;   display cashier/waiter mnemonic
:   ItemTransGetLocal()     ;   get transaction open local data
:   ItemTransPutLocal()     ;   put transaction open local data
:
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date    : Ve.Rev.  : Name        : Description
: 05-28-92  : 00.00.01 : hkato       : initial                                   
: 08-27-93  : 01.00.13 : kyou        : bug fixed E-3 (mod. ItemTransGCFNo)
: 11-10-95  : 03.01.00 : hkato       : R3.1
:  7-31-98  : 03.03.00 : hrkato      : V3.3 (Reduce NC/RO/AC Key)
:  8-11-99  : 03.05.00 :   K.Iwata   : R3.5 (remove WAITER_MODEL)
:  8-13-99  : 03.05.00 :   K.Iwata   : merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
:
:* NCR2171 **
:  9-06-99  : 01.00.00 :   M.Teraki  : initial (for 2171)
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
#include    "log.h"
#include    "appllog.h"
#include    "uireg.h"
#include    "pif.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "rfl.h"
#include    "display.h"
#include    "item.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "csstbgcf.h"
#include    "csgcs.h"
#include    "mld.h"
#include    "itmlocal.h"

ITEMTRANSLOCAL  ItemTransLocal;


/*==========================================================================
**  Synopsis:   SHORT   ItemTransGetLocal( ITEMTRANSLOCAL *pData )      
**  Synopsis:   SHORT   ItemTransPutLocal( ITEMTRANSLOCAL *pData )      
*
*   Input:      ITEMTRANSLOCAL  *pData           
*   Output:     ITEMTRANSLOCAL  *pData  
*   InOut:      none     
*
*   Return:     
*
*   Description:    get transaction open local data
*   Description:    put transaction local local data
==========================================================================*/
CONST volatile  ITEMTRANSLOCAL * CONST ItemTransLocalPtr = &ItemTransLocal;

ITEMTRANSLOCAL *ItemTransGetLocalPointer (VOID)
{
	return &ItemTransLocal;
}

VOID    ItemTransGetLocal( ITEMTRANSLOCAL *pTransLocal )
{
    *pTransLocal = ItemTransLocal;
}


VOID    ItemTransPutLocal( ITEMTRANSLOCAL *pTransLocal )
{
    ItemTransLocal = *pTransLocal;
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransOpen( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    transaction open module main which handles specific open
*                   or retrieve Guest Check key presses in Store Recall,
*                   Post Guest Check, and the Pre Guest Check system types.
*
*                   However it is also possible to start a Guest Check by merely
*                   entering the first item of a transaction which will automatically
*                   set up the transaction environment.
*                   See function ItemCommonTransOpen() which is called when a
*                   Guest Check is started by just entering an item.
==========================================================================*/

SHORT    ItemTransOpen( UIFREGTRANSOPEN *UifRegTransOpen )
{       
	SHORT  sRetStatus = LDT_SEQERR_ADR;

    switch( UifRegTransOpen->uchMinorClass ) {  

    case    CLASS_UINEWCHECK:                                       /* newcheck */  
        sRetStatus = ItemTransOpenNC( UifRegTransOpen );    
		break;

    case    CLASS_UIREORDER:                                        /* reorder */
        sRetStatus = ItemTransOpenRO( UifRegTransOpen );
		break;

	case	CLASS_UIRETURNCHECK:
		break;

    case    CLASS_UIADDCHECK1:                                      /* addcheck 1 */
    case    CLASS_UIADDCHECK2:                                      /* addcheck 2 */
    case    CLASS_UIADDCHECK3:                                      /* addcheck 3 */
    case    CLASS_UICASINTRECALL:                                   /* cashier interrupt recall, R3.3 */
    case    CLASS_UIRECALL:                                         /* recall */
        sRetStatus = ItemTransOpenAC( UifRegTransOpen );
		break;

    case    CLASS_UINEWKEYSEQ:                                      /* New guest check sequence V3.3 */
    case    CLASS_UINEWKEYSEQGC:                                    /* V3.3 */
        sRetStatus = ItemTransOpenNewKeySeq(UifRegTransOpen);
		break;

    default : 
        return( LDT_SEQERR_ADR );                                   /* sequence error */                 
    } 

	// indicate that we should not allow a sign-out at this time.
	// this will prevent the operator from accidently pressing the
	// wrong key that will cause them to sign out with a transaction
	// still pending causing a PifAbort() when they try to sign back
	// in and causing problems with the guest check file as well.
	// When the check is tendered, the status bits will be cleared.
	if (sRetStatus == ITM_SUCCESS)
		ItemCommonPutStatus (COMMON_PROHIBIT_SIGNOUT);

	return sRetStatus;
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTransGCFNo( ULONG *usGuestNo )
*
*   Input:      ULONG   *ulGuestNo
*   Output:     ULONG   *ulGuestNo
*   InOut:      none
*
*   Return:     
*
*   Description:    get GCF No
==========================================================================*/

SHORT   ItemTransGCFNo(ULONG *ulGuestNo)
{
	UIFDIADATA  WorkUI = {0};

    ItemTransDispGCFNo(TRN_GCNO_ADR);

    if (UifDiaGuestNo(&WorkUI) != UIF_SUCCESS) {
        return(UIF_CANCEL);
    }       

    *ulGuestNo = WorkUI.ulData;

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTransDispGCFNo(VOID)
*
*   Input:      ULONG   *ulGuestNo
*   Output:     ULONG   *ulGuestNo
*   InOut:      none
*   Return:     
*
*   Description:    Display "Enter GCF No."                          V3.3
*==========================================================================
*/
VOID    ItemTransDispGCFNo(USHORT usAddress)
{
	REGDISPMSG      Disp = {0};

    flDispRegDescrControl &= ~TAXEXMPT_CNTRL;
    flDispRegKeepControl &= ~TAXEXMPT_CNTRL;
    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_GCNO;
    RflGetTranMnem (Disp.aszMnemonic, usAddress);
    Disp.fbSaveControl = 4;
    DispWrite(&Disp);

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;
}

/*==========================================================================
**   Synopsis:  VOID    ItemTransDispDef( VOID )   
*
*   Input:      none
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    display default mnemonic(cashier/waiter)
==========================================================================*/

VOID    ItemTransDispDef( VOID )   
{
    TRANMODEQUAL    WorkMode;
	REGDISPMSG      DispMsg = {0};

    TrnGetModeQual( &WorkMode );                                    /* get mode qualifier */
    DispMsg.uchMinorClass = CLASS_UICASHIERIN;
    DispMsg.uchMajorClass = CLASS_UIFREGOPEN;  
    _tcsncpy( DispMsg.aszMnemonic, WorkMode.aszCashierMnem, NUM_OPERATOR );
                                                                    
    flDispRegDescrControl &= ~( COMPLSRY_CNTRL | VOID_CNTRL );      /* compulsory control */ 
    flDispRegKeepControl &= ~VOID_CNTRL;
    
    DispMsg.fbSaveControl = 1;                                      /* save for redisplay */
    DispWrite( &DispMsg );                                          /* display check total */

    if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT1 ) ) {       /* sign-out by mode key */
        UieModeChange( UIE_ENABLE );                                /* mode changeable */    
    }
}             



/*==========================================================================
**   Synopsis:  VOID    ItemTransCalSec(ITEMAFFECTION *ItemAffection)
*
*   Input:      
*   Output:     
*   InOut:      none
*
*   Return:     
*
*   Description:    Calculate Service Time(Seconds).
*                       ItemAffection->uchMajorClass = CLASS_ITEMAFFECTION;
*                       ItemAffection->uchMinorClass = CLASS_SERVICETIME;
==========================================================================*/

VOID    ItemTransCalSec(ITEMAFFECTION *ItemAffection)
{
    ULONG   ulStartSec, ulEndSec;
    SHORT   i;
    USHORT  ausBorder[SERVTIME_ADR_MAX];
    PARASERVICETIME     WorkServ;
    DATE_TIME           WorkDate;

    /*----- Get Service Time Parameter -----*/
    WorkServ.uchMajorClass = CLASS_PARASERVICETIME;
    WorkServ.uchAddress = SERV_TIME1_ADR;
    for (i = 0; i < SERVTIME_ADR_MAX; i++) {
        CliParaRead(&WorkServ);
        ausBorder[i] = WorkServ.usBorder;
        WorkServ.uchAddress++;
    }

    /*----- Illegal Parameter -----*/
    if (ausBorder[1]) {
        if (ausBorder[0] >= ausBorder[1]) {
            ausBorder[1] = 0;
        }
    }

    /*----- Get Current Time -----*/
    PifGetDateTime(&WorkDate);

    /*----- Calculate Service Time -----*/
    ulStartSec = (TranGCFQualPtr->auchCheckOpen[0] * 3600UL 
                + TranGCFQualPtr->auchCheckOpen[1] * 60UL
                + TranGCFQualPtr->auchCheckOpen[2]);

    ulEndSec =   (WorkDate.usHour * 3600UL
                + WorkDate.usMin * 60UL
                + WorkDate.usSec);

    if (TranGCFQualPtr->auchCheckOpen[0] > (UCHAR)WorkDate.usHour) {  /* over 12:00AM */
        ulEndSec += 24 * 3600UL;
    }

    ulEndSec -= ulStartSec;

    ItemAffection->lAmount = (LONG)ulEndSec;
    ItemAffection->sItemCounter = 1;
    ItemAffection->uchOffset = TranGCFQualPtr->uchHourlyOffset;

    /*----- Decide Service Time Offset -----*/
    if (ausBorder[1] && (ulEndSec > (ULONG)ausBorder[1])) {
        ItemAffection->ulId = 2;
    } else if (ausBorder[0] && (ulEndSec > (ULONG)ausBorder[0])) {
        ItemAffection->ulId = 1;
    } else {
        ItemAffection->ulId = 0;
    }
}




/*==========================================================================
**   Synopsis:  SHORT   ItemTransSeatNo(ITEMTRANSOPEN *ItemTransOpen)
*
*   Input:      ITEMTRANSOPEN *ItemTransOpen
*   Output:     ITEMTRANSOPEN   *ItemTransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    Get Seat# for SplitCheck,         R3.1, V3.3
==========================================================================*/

SHORT   ItemTransSeatNo(ITEMTRANSOPEN *ItemTransOpen, SHORT sType)
{
    SHORT           sStatus;
	REGDISPMSG      DispMsg = {0};
    UIFDIADATA      WorkUI = {0};

    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_SEATNO;
    RflGetSpecMnem (DispMsg.aszMnemonic, SPC_SEAT_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = 4;
    DispWrite(&DispMsg);

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;
     
    sStatus = UifDiaSeatNo(&WorkUI);
    if (sType == 0) {
        if (sStatus != UIF_SUCCESS) {
            ItemTransDispDef();
            return(UIF_CANCEL);
        }
        ItemTransOpen->uchSeatNo = (UCHAR)WorkUI.ulData;
    } else {
        if (sStatus == UIFREG_ABORT) {
            ItemTransDispDef();
            return(UIF_CANCEL);
        } else if (sStatus != UIF_SUCCESS) {
            ItemTransDispDef();
            return(LDT_SEQERR_ADR);
        }
        ItemTransOpen->uchSeatNo = (UCHAR)WorkUI.ulData;
        if (WorkUI.aszMnemonics[0] == '0') {
            ItemTransOpen->uchSeatNo = '0';
        }
    }

    return(ITM_SUCCESS);
}



/*
*==========================================================================
**   Synopsis:  SHORT   ItemTransNoPerson(ITEMTRANSOPEN *TransOpen)
*
*   Input:      ITEMTRANSOPEN   *TransOpen
*   Output:     ITEMTRANSOPEN   *TransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    Get No of Person,                       V3.3
*==========================================================================
*/
SHORT   ItemTransNoPerson(ITEMTRANSOPEN *TransOpen)
{
    if (CliParaMDCCheck(MDC_PSN_ADR, EVEN_MDC_BIT0)) {
		REGDISPMSG      DispMsg = {0};
		UIFDIADATA      WorkUI = {0};

        DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
        DispMsg.uchMinorClass = CLASS_REGDISP_PERSON;
        RflGetLead (DispMsg.aszMnemonic, LDT_NOOFPRSN_ADR);

        flDispRegDescrControl |= COMPLSRY_CNTRL;
        flDispRegKeepControl |= COMPLSRY_CNTRL;
        DispMsg.fbSaveControl = 4;
        
        DispWrite(&DispMsg);
         
        flDispRegDescrControl &= ~COMPLSRY_CNTRL;
        flDispRegKeepControl &= ~COMPLSRY_CNTRL;

        if (UifDiaNoPerson(&WorkUI) != UIF_SUCCESS) {
            ItemTransDispDef();
            return(UIF_CANCEL);
        }
        
        TransOpen->usNoPerson = (USHORT)WorkUI.ulData;
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**   Synopsis:  SHORT   ItemTransTableNo(ITEMTRANSOPEN *TransOpen)
*
*   Input:      ITEMTRANSOPEN   *TransOpen
*   Output:     ITEMTRANSOPEN   *TransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    Get Table No,                               V3.3
*==========================================================================
*/
SHORT   ItemTransTableNo(ITEMTRANSOPEN *TransOpen)
{
    if (CliParaMDCCheck(MDC_RECEIPT_ADR, ODD_MDC_BIT2)) {
		REGDISPMSG      DispMsg = {0};
		UIFDIADATA      WorkUI = {0};

		RflGetLead (DispMsg.aszMnemonic, LDT_TBLNO_ADR);
        DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
        DispMsg.uchMinorClass = CLASS_REGDISP_TABLENO;

        flDispRegDescrControl |= COMPLSRY_CNTRL;
        flDispRegKeepControl |= COMPLSRY_CNTRL;
        DispMsg.fbSaveControl = 4;

        DispWrite(&DispMsg);

        flDispRegDescrControl &= ~COMPLSRY_CNTRL;
        flDispRegKeepControl &= ~COMPLSRY_CNTRL;

        if (UifDiaTableNo(&WorkUI) != UIF_SUCCESS) {
            ItemTransDispDef();
            return(UIF_CANCEL);
        }
        
        TransOpen->usTableNo = (USHORT)WorkUI.ulData;
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTransNCCommon(ITEMTRANSOPEN *TransOpen)
*
*   Input:      ITEMTRANSOPEN   *TransOpen
*   Output:     ITEMTRANSOPEN   *TransOpen 
*   InOut:      none
*
*   Return:     
*
*   Description:    Newcheck Common Routine,                    V3.3
*==========================================================================
*/
VOID    ItemTransNCCommon(ITEMTRANSOPEN *TransOpen)
{
    TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();

    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );           /* inhibit post receipt, R3.0 */

    ItemTransNCPara();                                              /* set current qualifier */
    ItemPromotion( PRT_RECEIPT, TYPE_STORAGE );                         /* print promotion header */
    ItemHeader( TYPE_STORAGE );                                     /* p-void, training header */
    ItemTransNewCheck( TransOpen );
    TrnOpen( TransOpen );

    /*--- LCD Initialize,  R3.0 ---*/
    ItemCommonMldInit();

    /*--- Display GCF/Order#, R3.0 ---*/
    MldScrollWrite(TransOpen, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(TransOpen);

    /* send to enhanced kds, 2172 */
    ItemSendKds(TransOpen, 0);

    pWorkCur->fsCurStatus |= CURQUAL_NEWCHECK;                        /* newcheck operation */

    if ( pWorkCur->fsCurStatus & CURQUAL_PVOID ) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_PVOID;                      /* p-void GCF */
    }    
    if ( pWorkCur->fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN) ) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_TRETURN;                    /* transaction return */
    }    
    if (pWorkCur->fsCurStatus & CURQUAL_WIC) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_WIC;                        /* WIC, Saratoga */
    }    
    if (pWorkCur->fsCurStatus & CURQUAL_TRAINING ) {
        pWorkGCF->fsGCFStatus |= GCFQUAL_TRAINING;                   /* training transaction */
    }
    if ( !TranModeQualPtr->ulWaiterID ) {   
        pWorkGCF->fsGCFStatus |= GCFQUAL_NEWCHEK_CASHIER;            /* newcheck is cashier */
    }
    if (TransOpen->uchSeatNo != 0) {
        pWorkGCF->fsGCFStatus2 |= GCFQUAL_USESEAT;
    }

    ItemSalesGetLocalPointer()->fbSalesStatus |= SALES_ITEMIZE;     /* itemize state */
    ItemSalesCalcInitCom();                                         /* ppi initialize r3.1 2172 */

    if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) {
        pWorkCur->fsCurStatus |= ( CURQUAL_TABLECHANGE | CURQUAL_PERSONCHANGE	);
    }                

    UieModeChange(UIE_DISABLE);                                     /* mode unchangable */
}

/****** End of Source ******/