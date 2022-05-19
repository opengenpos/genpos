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
* Title       : Multi Line Display For DEParmenT Module  ( SUPER MODE )
* Category    : MLD, NCR 2170 US Hospitality Application Program    
* Program Name: MLDCAS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls COUPON PARAMETER FILE.
*
*    The provided function names are as follows: 
* 
*        MldParaCASDisp()  : Display parameter in Department Files to MLD.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-95: 03.00.00 : T.Satoh   : initial                                   
* Nov-22-95: 03.01.00 : M.Ozawa   : add status9
* Agu-18-98: 03.03.00 : H.Iwata   :
* 
*** NCR2171 **
* Aug-26-99: 01.00.00 : M.Teraki  : initial (for Win32)
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
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <csttl.h>
#include <csstbttl.h>
#include <appllog.h>

#include <report.h>
#include <mld.h>
#include <mldsup.h> 
#include <mldmenu.h>

#include "maintram.h" 
#include "cscas.h"

/*
*===========================================================================
*   MLD Format of Department Maintenance Modeule
*===========================================================================
*/

/*------------------------------------------------------------------------*\
C A S H I E R   M a i n t e n a n c e
\*------------------------------------------------------------------------*/
/*
CONST UCHAR FARCONST aszMldAC20[][64] = {
"....*....1....*....2....*....3....*....4",
"Name",
"%-10s",
"%-10s TRAINEE",
""
};
*/
static CONST MLDMENU CONST aszMldAC20[] = {
	{ -1,_T("") },
	{ LCD_CAS_ADDR1_ADR, _T(" 1 %s\t%4s ") },
	{ LCD_CAS_ADDR2_ADR, _T(" 2 %s\t%4s ") },
	{ LCD_CAS_ADDR3_ADR, _T(" 3 %s\t%4s ") },
	{ LCD_CAS_ADDR4_ADR, _T(" 4 %s\t%4s ") },
	{ LCD_CAS_ADDR11_ADR, _T(" 5 %s\t%4s ") },
	{ LCD_CAS_ADDR5_ADR, _T(" 6 %s\t%4u ") },
	{ LCD_CAS_ADDR6_ADR, _T(" 7 %s\t%4u ") },
	{ LCD_CAS_ADDR7_ADR, _T(" 8 %s\t%1s ") },
	{ LCD_CAS_ADDR8_ADR, _T(" 9 %s\t%1s ") },
	{ LCD_CAS_ADDR9_ADR, _T("10 %s\t%2s ") },
	{ LCD_CAS_ADDR10_ADR, _T("11 %s\t%20s ") },
	{ 0,_T("") }
};
/***
CONST UCHAR FARCONST aszMldAC20[][64] = {
"....*....1....*....2....*....3....*....4",
" 1  Operator Status #1\t%4s",
" 2  Operator Status #2\t%4s",
" 3  Operator Status #3\t%4s",
" 4  Operator Status #4\t%4s",
" 5  Operator Status #5\t%4s",
" 6  Guest Check Number From\t%4u",
" 7  Guest Check Number To\t%4u",
" 8  Charge Tip Rate\t%1s",
" 9  Team Number \t%1s",
"10  Allowed Terminal No\t%2s",
"11  ACCII Character -%20s,
""
};
***/

/*
*===========================================================================
** Synopsis:    VOID MldParaCASDisp(MAINTCASHIERIF *pCAS)
*               
*     Input:    uchFieldAddr ( 0 : All, 1 : Individual )
*
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/

VOID MldParaCASDisp(MAINTCASHIERIF *pCas)
{
    TCHAR   aszStatus1[5][6] = { 0, 0, 0, 0, 0, 0}; /* Work Status strings      */
    TCHAR   aszStatus2[2][2] = { 0, 0 };            /* Work Status strings      */
    TCHAR   aszStatus3[4] = { 0, 0, 0, 0 };         /* Work Status strings      */
    TCHAR   aszMnemonics[PARA_CASHIER_LEN+1];
    SHORT   i;                                      /* Work counter             */
    MLDIF   MldIf;
    USHORT  usColumn , sReturn;

    /*
     * Initialize Display Format and Pre-Set character strings of Status
     */
/*  for(i=0;i<9;i++){
        memset(&(aszStatus[i][0]),0,7);
    }
*/
    /*************************************************/
    /* Convert CAS Status Data to Binary ASCII Data */
    /*************************************************/
    PrtSupItoa(( UCHAR)(pCas->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & 0x0F), aszStatus1[0]);
    PrtSupItoa(( UCHAR)((pCas->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & 0xF0) >> 4), aszStatus1[1]);
    PrtSupItoa(( UCHAR)(pCas->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & 0x0F), aszStatus1[2]);
    PrtSupItoa(( UCHAR)((pCas->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & 0xF0) >> 4), aszStatus1[3]);
	PrtSupItoa(( UCHAR)(pCas->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_2] & 0x0F), aszStatus1[4]);
    _itot((UCHAR)( pCas->CashierIf.uchChgTipRate), aszStatus2[0], 10);
    _itot((UCHAR)( pCas->CashierIf.uchTeamNo), aszStatus2[1], 10);
    _itot((UCHAR)( pCas->CashierIf.uchTerminal), aszStatus3, 10);

    /**************************/
    /* Display All Parameters */
    /**************************/
    if(pCas->uchFieldAddr == 0){
		MldScrollClear(MLD_SCROLL_1);              /* Clear scroll display */
 
        /*
         * Status 1-5
         */
        for(i=CAS_FIELD1_ADR; i<=CAS_FIELD5_ADR; i++){
            MldIf.usAddress = aszMldAC20[i].usAddress;
            sReturn = CliOpMldIndRead(&MldIf,0);
            /* set null at data display position */
            usColumn = MLD_MENU_COLUMN-3-12;
            if (usColumn < OP_MLD_MNEMONICS_LEN) {
                MldIf.aszMnemonics[usColumn] = '\0';
            }

            MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC20[i].aszData, MldIf.aszMnemonics, aszStatus1[i-1]);
        }

        /*
         * Guest Check Start Number
         */

        MldIf.usAddress = aszMldAC20[CAS_FIELD6_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf,0);
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-12;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }

         MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC20[CAS_FIELD6_ADR].aszData, MldIf.aszMnemonics, pCas->CashierIf.usGstCheckStartNo);
 
        /*
         *  Guest Check End Number
         */

        MldIf.usAddress = aszMldAC20[CAS_FIELD7_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf,0);
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-12;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }
        
        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i), aszMldAC20[CAS_FIELD7_ADR].aszData, MldIf.aszMnemonics, pCas->CashierIf.usGstCheckEndNo);

        /*
         *  Charge Tip Rate
         */

        MldIf.usAddress = aszMldAC20[CAS_FIELD8_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf,0);
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-12;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }

        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i+1), aszMldAC20[CAS_FIELD8_ADR].aszData, MldIf.aszMnemonics, aszStatus2[0]);

        /*
         *  Team Number
         */

        MldIf.usAddress = aszMldAC20[CAS_FIELD9_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf,0);
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-12;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }

        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i+2), aszMldAC20[CAS_FIELD9_ADR].aszData, MldIf.aszMnemonics, aszStatus2[1]);

        /*
         *  Terminal Number
         */

        MldIf.usAddress = aszMldAC20[CAS_FIELD10_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf,0);
        /* set null at data display position */
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[MLD_MENU_COLUMN-3-5] = '\0';
        }

        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i+3), aszMldAC20[CAS_FIELD10_ADR].aszData, MldIf.aszMnemonics, aszStatus3);

        /*
         * Mnemonic 
         */
        MldIf.usAddress = aszMldAC20[CAS_FIELD11_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf,0);
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-12;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }

        memset(&aszMnemonics, 0, sizeof(aszMnemonics));
        _tcsncpy(aszMnemonics, pCas->CashierIf.auchCashierName, PARA_CASHIER_LEN);
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP + CAS_FIELD11_ADR-1, aszMldAC20[CAS_FIELD11_ADR].aszData, MldIf.aszMnemonics, &aszMnemonics);


    }
}
