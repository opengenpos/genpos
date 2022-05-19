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
* Title       : Multi Line Display For PLU Module  ( SUPER MODE )
* Category    : MLD, NCR 2170 US Hospitality Application Program    
* Program Name: MLDPLU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls PLU PARAMETER FILE.
*
*    The provided function names are as follows: 
* 
*        MldParaPLUDisp()  : Display parameter in PLU Files to MLD.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-95: 03.00.00 : T.Satoh   : initial                                   
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

/*
*===========================================================================
*   MLD Format of PLU Maintenance Modeule
*===========================================================================
*/

static CONST MLDMENU FARCONST aszMldAC63[] = {
	{ -1,_T("") },
	{ LCD_PLU_ADDR1_ADR, _T("%2s %s\t%2d ") },
	{ LCD_PLU_ADDR2_ADR, _T("%2s %s\t%2d ") },
	{ LCD_PLU_ADDR3_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR4_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR5_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR6_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR7_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR8_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR9_ADR, _T("%2s %s\t%s ") },
	{ LCD_PLU_ADDR10_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR11_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR12_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR13_ADR, _T("%2s %s\t%20s ") },
	{ LCD_PLU_ADDR14_ADR, _T("%2s %s\t%2d ") },
	{ LCD_PLU_ADDR15_ADR, _T("%2s %s\t%2d ") },
	{ LCD_PLU_ADDR16_ADR, _T("%2s %s\t%2d ") },
	{ LCD_PLU_ADDR17_ADR, _T("%2s %s\t%3d ") },
	{ LCD_PLU_ADDR18_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR19_ADR, _T("%2s %s\t%04s ") },
	{ LCD_PLU_ADDR20_ADR, _T("%2s %s\t%4d ") },
	{ LCD_PLU_ADDR21_ADR, _T("%2s %s\t%1d ") },
	{ LCD_PLU_ADDR22_ADR, _T("%2s %s\t%03d ") },
	{ LCD_PLU_ADDR23_ADR, _T("%2s %s\t%02d ") },
	{ LCD_PLU_ADDR_24_ADR, _T("%2s %s\t%7#l$ ") },
	{ LCD_PLU_ADDR_25_ADR, _T("%2s %s\t%7#l$ ") },
	{ LCD_PLU_ADDR_26_ADR, _T("%2s %s\t%7#l$ ") },
	{ LCD_PLU_ADDR_27_ADR, _T("%2s %s\t%7#l$ ") },
	{ LCD_PLU_ADDR_28_ADR, _T("%2s %s\t%7#l$ ") },
	{ 0,_T("") }
};
/***
CONST UCHAR FARCONST aszMldAC63[][64] = {
"....*....1....*....2....*....3....*....4",
"%2s Department Number\t%2d ",
"%2s Report Code\t%2d ",
"%2s Prt Mod/Spv Int/Special/Minus\t%04s ",
"%2s Sng Dbl/Issu Rec/Val/Rec Cond\t%04s ",
"%2s Tax4/Tax3/Tax2/Tax1\t%04s ",
"%2s Disc2/Disc1/Not Use/Not Use\t%04s ",
"%2s DEPT Ctl/Not Use/Cond./Scale\t%04s ",
"%2s Rmt4/Rmt3/Rmt2/Rmt1\t%04s ",
"%2s Bonus Total Index\t%s ",
"%2s Adj4/Adj3/Adj2/Adj1\t%04s ",
"%2s Var1/Adjective(000-101)\t%04s ",
"%2s Var5/Var4/Var3/Var2\t%04s ",
"%2s PLU Name\t%12s ",
"%2s Order Entry Prompt Table#\t%2d ",
"%2s Order Entry Prompt Group#\t%2d ",
"%2s Print Priority\t%2d ",
"%2s PPI Code\t%3d ",
"%2s Rmt8/Rmt7/Rmt6/Rmt5\t%04s ",
"%2s Adjective#1 Price\t%7#l$ ",
"%2s Adjective#2 Price\t%7#l$ ",
"%2s Adjective#3 Price\t%7#l$ ",
"%2s Adjective#4 Price\t%7#l$ ",
"%2s Adjective#5 Price\t%7#l$ ",
""
};
***/
static CONST MLDMENU FARCONST aszMldAC68[] = {
	{ -1,_T("") },
	{ LCD_PLU_TYPE1_ADR,_T("%s\t1 ") },
	{ LCD_PLU_TYPE2_ADR,_T("%s\t2 ") },
	{ LCD_PLU_TYPE3_ADR,_T("%s\t3 ") },
	{ 0,_T("") }
};
/***
CONST UCHAR FARCONST aszMldAC68[][64] = {
"....*....1....*....2....*....3....*....4",
"Open PLU\t1 ",
"Non Adjective PLU\t2 ",
"Adjective PLU\t3 ",
""
};
***/

/*
*===========================================================================
** Synopsis: VOID MldParaPLUDisp(PARAPLU *pPlu)
*               
*     Input: pPlu->usACNumber:    AC_PLU_MAINT     (A/C 63)
*                                 AC_PLU_PRICE_CHG (A/C 64)
*                                 AC_PLU_ADDDEL    (A/C 68)
*                                 AC_PLU_MNEMO_CHG (A/C 82)
*
*            pPlu->uchMinorClass: CLASS_PARAPLU_ADD_READ
*                                 Minor Class is only used to display menu
*                                 of selecting PLU Type in A/C Number 68.
*                                 If A/C Number is not AC_PLU_ADDDEL(A/C 68),
*                                 then specification of Minor Class is ignored.
*
*            pPlu->uchFieldAddr ( 0:All Parameter  1:Individual Parameter )
*
*    Output: Nothing 
*     InOut: Nothing
*
** Return:   Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/

VOID MldParaPLUDisp(PARAPLU *pPlu, USHORT usStatus)
{
	ULONG   ulPrice[5] = { 0 };                         /* Pre-Set PRICE * 5       */
    TCHAR   aszStatus[12][5] = { 0 };                   /* Status * 10             */
    UCHAR   uchFormIdx;                         /* Index to Format Table   */
    TCHAR   uchItemNo[8] = { 0 };                       /* Work Item Number        */
    UCHAR   uchPriceAddr;                       /* Field Address for Price */
    USHORT  sRow;                               /* Work Display Line No.   */
    USHORT  i,j;                                /* Work Counter            */
    MLDIF   MldIf;
    USHORT  usColumn, sReturn;


    /************************************************/
    /* Convert PLU status data to binary ASCII data */
    /************************************************/
    PrtSupItoa((UCHAR) (pPlu->auchPLUStatus[0]&0x0F),    &(aszStatus[0][0]));
    PrtSupItoa((UCHAR)((pPlu->auchPLUStatus[0]&0xF0)>>4),&(aszStatus[1][0]));
    PrtSupItoa((UCHAR) (pPlu->auchPLUStatus[1]&0x0F),    &(aszStatus[2][0]));
    PrtSupItoa((UCHAR)((pPlu->auchPLUStatus[1]&0xF0)>>4),&(aszStatus[3][0]));
    PrtSupItoa((UCHAR) (pPlu->auchPLUStatus[2]&0x0F),    &(aszStatus[4][0]));
    PrtSupItoa((UCHAR)((pPlu->auchPLUStatus[2]&0xF0)>>4),&(aszStatus[5][0]));
    _itot(pPlu->auchPLUStatus[3]&0x0F,&(aszStatus[6][0]),10);
    PrtSupItoa((UCHAR)((pPlu->auchPLUStatus[3]&0xF0)>>4),&(aszStatus[7][0]));
    PrtSupItoa((UCHAR) (pPlu->auchPLUStatus[4]&0x0F),    &(aszStatus[8][0]));
    PrtSupItoa((UCHAR)((pPlu->auchPLUStatus[4]&0xF0)>>4),&(aszStatus[9][0]));
    PrtSupItoa((UCHAR) (pPlu->auchPLUStatus[5]&0x0F),    &(aszStatus[10][0]));
    PrtSupItoa((UCHAR)((pPlu->auchPLUStatus[5]&0xF0)>>4),&(aszStatus[11][0]));

    /*************************************************************/
    /* Convert Adjective Price(3byte) data to ULONG data for ALL */
    /*************************************************************/
    if(pPlu->uchFieldAddr == 0){
        switch (pPlu->uchMinorClass) {
            case CLASS_PARAPLU_ONEADJ:             /* Adjective Price PLU  */
                RflConv3bto4b(&(ulPrice[1]),&(pPlu->auchPresetPrice[1][0]));
                RflConv3bto4b(&(ulPrice[2]),&(pPlu->auchPresetPrice[2][0]));
                RflConv3bto4b(&(ulPrice[3]),&(pPlu->auchPresetPrice[3][0]));
                RflConv3bto4b(&(ulPrice[4]),&(pPlu->auchPresetPrice[4][0]));

            case CLASS_PARAPLU_NONADJ:             /* One Price PLU        */
                RflConv3bto4b(&(ulPrice[0]),&(pPlu->auchPresetPrice[0][0]));

            default : break;                       /* OPEN PLU or Else     */
        }
    }
    /********************************************************************/
    /* Convert Adjective Price(3byte) data to ULONG data for Individual */
    /********************************************************************/
    else{
        uchPriceAddr = pPlu->uchFieldAddr;        /* Set Price Address     */
        if(pPlu->usACNumber == AC_PLU_PRICE_CHG){
            uchPriceAddr += PLU_FIELD23_ADR;      /* Adjusting for A/C 64  */
        }

        switch(uchPriceAddr){
            case PLU_FIELD28_ADR:                  /* Preset Price 5       */
                RflConv3bto4b(&(ulPrice[4]),&(pPlu->auchPresetPrice[4][0]));
                break;

            case PLU_FIELD27_ADR:                  /* Preset Price 4       */
                RflConv3bto4b(&(ulPrice[3]),&(pPlu->auchPresetPrice[3][0]));
                break;

            case PLU_FIELD26_ADR:                  /* Preset Price 3       */
                RflConv3bto4b(&(ulPrice[2]),&(pPlu->auchPresetPrice[2][0]));
                break;

            case PLU_FIELD25_ADR:                  /* Preset Price 2       */
                RflConv3bto4b(&(ulPrice[1]),&(pPlu->auchPresetPrice[1][0]));
                break;

            case PLU_FIELD24_ADR:                  /* Preset Price 1       */
                RflConv3bto4b(&(ulPrice[0]),&(pPlu->auchPresetPrice[0][0]));
                break;

            default : break;
        }
    }

    /*********************************/
    /* Display All Parameters to MLD */
    /*********************************/
    if(pPlu->uchFieldAddr == 0){
        MldScrollClear(MLD_SCROLL_1);              /* Clear scroll display */

        /*
         * Branch by A/C Number
         */
        switch(pPlu->usACNumber){
            case AC_PLU_ADDDEL:
                /*********************************************/
                /*      Display Menu of PLU Type to MLD      */
                /*********************************************/
                if(pPlu->uchMinorClass == CLASS_PARAPLU_ADD_READ){
                    for(i=1;i<=3;i++){
                        /* get programmable mnemonics, V3.3 */
                        MldIf.usAddress = aszMldAC68[i].usAddress;
                        sReturn = CliOpMldIndRead(&MldIf, 0);
                        if (sReturn != OP_PERFORM){
                            MldIf.aszMnemonics[0] = '\0';
                        }

                        MldPrintf(MLD_SCROLL_1, (SHORT)(MLD_SCROLL_TOP+i),aszMldAC68[i].aszData, MldIf.aszMnemonics);
                    }
                    break;
                }
                /*********************************************/
                /* Else Case is Through Next Display Pattern */
                /*********************************************/

            case AC_PLU_MAINT:
                /* ---- display second page display at address22/23 ---- */
                if (usStatus) {
                /* if ((usStatus) && (pPlu->uchMinorClass == CLASS_PARAPLU_ONEADJ)) { */

                        /* get programmable mnemonics, V3.3 */
                        MldIf.usAddress = aszMldAC63[PLU_FIELD22_ADR].usAddress;
                        sReturn = CliOpMldIndRead(&MldIf, 0);
                        if (sReturn != OP_PERFORM){
                            MldIf.aszMnemonics[0] = '\0';
                        }

                        /* set null at data display position */
                        usColumn = MLD_MENU_COLUMN-3-12;
                        if (usColumn < OP_MLD_MNEMONICS_LEN) {
                            MldIf.aszMnemonics[usColumn] = '\0';
                        }

                        _itot(PLU_FIELD22_ADR,uchItemNo,10);
                        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP), aszMldAC63[PLU_FIELD22_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usFamilyCode));
                                    
                        MldIf.usAddress = aszMldAC63[PLU_FIELD23_ADR].usAddress;
                        sReturn = CliOpMldIndRead(&MldIf, 0);
                        if (sReturn != OP_PERFORM){
                            MldIf.aszMnemonics[0] = '\0';
                        }

                        /* set null at data display position */
                        usColumn = MLD_MENU_COLUMN-3-12;
                        if (usColumn < OP_MLD_MNEMONICS_LEN) {
                            MldIf.aszMnemonics[usColumn] = '\0';
                        }

                        _itot(PLU_FIELD23_ADR,uchItemNo,10);
                        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+1), aszMldAC63[PLU_FIELD23_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchPriceMultiple));
                                    
                    for(i=PLU_FIELD24_ADR;i<=PLU_FIELD28_ADR;i++){ /*Adj Price1-5*/

                        if (pPlu->uchMinorClass == CLASS_PARAPLU_OPENPLU) { /* Saratoga */
                            break;
                        }

                        /* get programmable mnemonics, V3.3 */
                        MldIf.usAddress = aszMldAC63[i].usAddress;
                        sReturn = CliOpMldIndRead(&MldIf, 0);
                        if (sReturn != OP_PERFORM){
                            MldIf.aszMnemonics[0] = '\0';
                        }

                        /* set null at data display position */
                        usColumn = MLD_MENU_COLUMN-3-12;
                        if (usColumn < OP_MLD_MNEMONICS_LEN) {
                            MldIf.aszMnemonics[usColumn] = '\0';
                        }

                        _itot(i,uchItemNo,10);
                        MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-PLU_FIELD22_ADR), aszMldAC63[i].aszData, uchItemNo, MldIf.aszMnemonics, (DCURRENCY)(LONG)ulPrice[i-PLU_FIELD24_ADR]);
                                    
                        if (pPlu->uchMinorClass != CLASS_PARAPLU_ONEADJ) break;
                    }
                    return;
                }

                /* get programmable mnemonics, V3.3 */

                MldIf.usAddress = aszMldAC63[PLU_FIELD1_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD1_ADR,uchItemNo,10);         /* Item Number    */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD1_ADR-1, aszMldAC63[PLU_FIELD1_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usDEPTNumber));

                MldIf.usAddress = aszMldAC63[PLU_FIELD2_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD2_ADR,uchItemNo,10);         /* Report Code    */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD2_ADR-1, aszMldAC63[PLU_FIELD2_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchReportCode));

                for(i=PLU_FIELD3_ADR;i<=PLU_FIELD12_ADR;i++){  /*Status 1-10*/

                    MldIf.usAddress = aszMldAC63[i].usAddress;
                    sReturn = CliOpMldIndRead(&MldIf, 0);
                    if (sReturn != OP_PERFORM){
                        MldIf.aszMnemonics[0] = '\0';
                    }
                    /* set null at data display position */
                    usColumn = MLD_MENU_COLUMN-3-5;
                    if (usColumn < OP_MLD_MNEMONICS_LEN) {
                        MldIf.aszMnemonics[usColumn] = '\0';
                    }
                    _itot(i,uchItemNo,10);
					// if field9, MldPrintf from below
					// ugly temporary fix for bonus total prob
					if (i == PLU_FIELD9_ADR){
						// We have gone from a bit map allowing any combination of eight bonus totals to be assigned to a PLU
						// to allowing only a single bonus total to be assigned to a PLU but there are 100 possible bonus totals now.
						// See other places where PLU_FIELD9_ADR is used or referenced including in comments.
						//    Richard Chambers, Mar-03-2020
						//MldPrintf(MLD_SCROLL_1, (SHORT)(MLD_SCROLL_TOP + i - 1), aszMldAC63[15].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->auchPLUStatus[3]));
						MldPrintf(MLD_SCROLL_1, (SHORT)(MLD_SCROLL_TOP + i - 1), aszMldAC63[15].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usBonusIndex));
						continue;
					}
                    MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC63[i].aszData, uchItemNo, MldIf.aszMnemonics, &(aszStatus[i-3][0]));
                }

                MldIf.usAddress = aszMldAC63[PLU_FIELD13_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-14;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD13_ADR,uchItemNo,10);        /* PLU Name      */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD13_ADR-1, aszMldAC63[PLU_FIELD13_ADR].aszData, uchItemNo, MldIf.aszMnemonics, pPlu->aszMnemonics);

                MldIf.usAddress = aszMldAC63[PLU_FIELD14_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD14_ADR,uchItemNo,10);        /* OEP Table No. */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD14_ADR-1, aszMldAC63[PLU_FIELD14_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchTableNumber));

                MldIf.usAddress = aszMldAC63[PLU_FIELD15_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD15_ADR,uchItemNo,10);        /* OEP Group No. */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD15_ADR-1, aszMldAC63[PLU_FIELD15_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchGroupNumber));

                MldIf.usAddress = aszMldAC63[PLU_FIELD16_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD16_ADR,uchItemNo,10);         /* Print Priority*/
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD16_ADR-1, aszMldAC63[PLU_FIELD16_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchPrintPriority));

                MldIf.usAddress = aszMldAC63[PLU_FIELD17_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD17_ADR,uchItemNo,10);         /* PPI Code      */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD17_ADR-1, aszMldAC63[PLU_FIELD17_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchPPICode));

                MldIf.usAddress = aszMldAC63[PLU_FIELD18_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-5;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD18_ADR,uchItemNo,10);
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD18_ADR-1, aszMldAC63[PLU_FIELD18_ADR].aszData, uchItemNo, MldIf.aszMnemonics, &(aszStatus[10][0]));

                MldIf.usAddress = aszMldAC63[PLU_FIELD19_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-5;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD19_ADR,uchItemNo,10);
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD19_ADR-1, aszMldAC63[PLU_FIELD19_ADR].aszData, uchItemNo, MldIf.aszMnemonics, &(aszStatus[11][0]));

                MldIf.usAddress = aszMldAC63[PLU_FIELD20_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD20_ADR,uchItemNo,10);         /* Link No      */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD20_ADR-1, aszMldAC63[PLU_FIELD20_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usLinkNumber));

                MldIf.usAddress = aszMldAC63[PLU_FIELD21_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD21_ADR,uchItemNo,10);         /* PPI Code      */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+PLU_FIELD21_ADR-1, aszMldAC63[PLU_FIELD21_ADR].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchSalesRestriction));

                break;

            case AC_PLU_PRICE_CHG:
                for(i=PLU_FIELD24_ADR,j=0;i<=PLU_FIELD28_ADR;i++,j++){
                    /* Open Price PLU  */
                    if(pPlu->uchPLUType == PLU_OPEN)    break;

                    MldIf.usAddress = aszMldAC63[i].usAddress;
                    sReturn = CliOpMldIndRead(&MldIf, 0);
                    if (sReturn != OP_PERFORM){
                        MldIf.aszMnemonics[0] = '\0';
                    }
                    /* set null at data display position */
                    usColumn = MLD_MENU_COLUMN-3-12;
                    if (usColumn < OP_MLD_MNEMONICS_LEN) {
                        MldIf.aszMnemonics[usColumn] = '\0';
                    }
                    _itot(j+1,uchItemNo,10);
                    MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+j), aszMldAC63[i].aszData, uchItemNo, MldIf.aszMnemonics, (DCURRENCY)(LONG)ulPrice[j]);

                    /* One Price PLU  */
                    if(pPlu->uchPLUType == PLU_NONADJ)  break;
                }
                break;

            case AC_PLU_MNEMO_CHG:
                MldIf.usAddress = aszMldAC63[PLU_FIELD13_ADR].usAddress;
                sReturn = CliOpMldIndRead(&MldIf, 0);
                if (sReturn != OP_PERFORM){
                    MldIf.aszMnemonics[0] = '\0';
                }
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-14;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                _itot(PLU_FIELD1_ADR,uchItemNo,10);         /* PLU Name      */
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP, aszMldAC63[PLU_FIELD13_ADR].aszData, "  ", MldIf.aszMnemonics, pPlu->aszMnemonics);

                break;

            /*default : return(LDT_xxxxxxx_ADR);*/
        }
    }

    /**********************************************/
    /* Display Individual Field Parameters to MLD */
    /**********************************************/
    else{
        _itot(pPlu->uchFieldAddr,uchItemNo,10);           /* Item Number    */
        if (pPlu->uchFieldAddr > PLU_FIELD21_ADR) {     /* display nexe page */
            /* Disp Line No */
            sRow = MLD_SCROLL_TOP + pPlu->uchFieldAddr - PLU_FIELD22_ADR;
        } else {
            sRow = MLD_SCROLL_TOP + pPlu->uchFieldAddr - 1;  /* Disp Line No.  */
        }

        /* Calc Form-Tbl Idx */
        uchFormIdx = pPlu->uchFieldAddr;       /* Calc Form-Tbl Idx */
        if (pPlu->usACNumber == AC_PLU_PRICE_CHG) {
            uchFormIdx = (UCHAR)(pPlu->uchFieldAddr+PLU_FIELD24_ADR-1);
        } else 
        if (pPlu->usACNumber == AC_PLU_MNEMO_CHG) {
            uchFormIdx = PLU_FIELD13_ADR;
        }
        /* get programmable mnemonics, V3.3 */

        MldIf.usAddress = aszMldAC63[uchFormIdx].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }

        /*
         * Branch by A/C Number
         */
        switch(pPlu->usACNumber){
            case AC_PLU_ADDDEL:
            case AC_PLU_MAINT:
                /*
                 * Branch by Field Address
                 */
                switch(pPlu->uchFieldAddr){
                   case PLU_FIELD1_ADR:                /* Dept number       */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usDEPTNumber));
                       break;

                   case PLU_FIELD2_ADR:                /* Report Code       */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow,
                                    aszMldAC63[uchFormIdx].aszData,
                                    uchItemNo,
                                    MldIf.aszMnemonics,
                                    (USHORT)(pPlu->uchReportCode));
                       break;

                   case PLU_FIELD3_ADR:                /* Status 1          */
                   case PLU_FIELD4_ADR:                /* Status 2          */
                   case PLU_FIELD5_ADR:                /* Status 3          */
                   case PLU_FIELD6_ADR:                /* Status 4          */
                   case PLU_FIELD7_ADR:                /* Status 5          */
                   case PLU_FIELD8_ADR:                /* Status 6          */
                   case PLU_FIELD10_ADR:               /* Status 8          */
                   case PLU_FIELD11_ADR:               /* Status 9          */
                   case PLU_FIELD12_ADR:               /* Status 10         */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-5;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, &(aszStatus[pPlu->uchFieldAddr-3][0]));
                       break;
				   
				   case PLU_FIELD9_ADR:                /* Bonus Total Index */

					   /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-5;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
					   // Hard coding aszMldAC63[15].aszData for now to fix
					   // problem with display of Bonus Total Index. Will
					   // get aszMldAC63[9].aszData set up correctly when I
					   // have some down time. -JV
                       //MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[15].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->auchPLUStatus[3]));
                                    //(USHORT)(aszStatus[pPlu->uchFieldAddr-3][0]));
									// conversion of binary to ascii at beginning
									// of this function seems to be causing the
									// problem with AC 63 Bonus Total. Will get this
									// cleaned up in the future. For now, ignoring
									// the converted ascii data works.

						// We have gone from a bit map allowing any combination of eight bonus totals to be assigned to a PLU
						// to allowing only a single bonus total to be assigned to a PLU but there are 100 possible bonus totals now.
						// See other places where PLU_FIELD9_ADR is used or referenced including in comments.
						//    Richard Chambers, Mar-03-2020
					   MldPrintf(MLD_SCROLL_1, sRow, aszMldAC63[15].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usBonusIndex));
					   break;

                   case PLU_FIELD13_ADR:               /* PLU Name          */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-14;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, pPlu->aszMnemonics);
                       break;

                   case PLU_FIELD14_ADR:               /* OEP Table Number */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchTableNumber));
                       break;

                   case PLU_FIELD15_ADR:               /* OEP Group Number */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchGroupNumber));
                       break;

                   case PLU_FIELD16_ADR:               /* Print Priority    */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchPrintPriority));
                       break;

                   case PLU_FIELD17_ADR:               /* PPI Code          */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchPPICode));
                       break;

                   case PLU_FIELD18_ADR:               /* Status 11         */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-5;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, &(aszStatus[10][0]));
                       break;

                   case PLU_FIELD19_ADR:               /* Status 12         */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-5;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, &(aszStatus[11][0]));
                       break;

                   case PLU_FIELD20_ADR:               /* Link No          */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usLinkNumber));
                       break;

                   case PLU_FIELD21_ADR:               /* Sales Restriction          */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchSalesRestriction));
                       break;

                   case PLU_FIELD22_ADR:               /* Family Code          */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->usFamilyCode));
                       break;

                   case PLU_FIELD23_ADR:               /* Price Multiple          */

                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-3;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                       }
                       MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, (USHORT)(pPlu->uchPriceMultiple));
                       break;

                   case PLU_FIELD24_ADR:               /* Preset Price 1   */
                   case PLU_FIELD25_ADR:               /* Preset Price 2   */
                   case PLU_FIELD26_ADR:               /* Preset Price 3   */
                   case PLU_FIELD27_ADR:               /* Preset Price 4   */
                   case PLU_FIELD28_ADR:               /* Preset Price 5   */
                       /* set null at data display position */
                       MldIf.aszMnemonics[MLD_MENU_COLUMN-3-11] = '\0';

                       MldPrintf(MLD_SCROLL_1,sRow,
                                    aszMldAC63[uchFormIdx].aszData,
                                    uchItemNo,
                                    MldIf.aszMnemonics,
                                    ulPrice[pPlu->uchFieldAddr-24]);
                       break;
                   /* default : break; */
                }

                break;

            case AC_PLU_PRICE_CHG:
                /*
                 * Branch by Field Address
                 */
                switch(pPlu->uchFieldAddr){
                    case PLU_FIELD1_ADR:                /* Preset Price 1   */
                    case PLU_FIELD2_ADR:                /* Preset Price 2   */
                    case PLU_FIELD3_ADR:                /* Preset Price 3   */
                    case PLU_FIELD4_ADR:                /* Preset Price 4   */
                    case PLU_FIELD5_ADR:                /* Preset Price 5   */
                       /* set null at data display position */
                       usColumn = MLD_MENU_COLUMN-3-12;
                       if (usColumn < OP_MLD_MNEMONICS_LEN) {
                           MldIf.aszMnemonics[usColumn] = '\0';
                        }
                        MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, uchItemNo, MldIf.aszMnemonics, ulPrice[pPlu->uchFieldAddr-1]);
                        break;

                    /* default : break; */
                }

                break;

            case AC_PLU_MNEMO_CHG:
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC63[uchFormIdx].aszData, "  ", MldIf.aszMnemonics, pPlu->aszMnemonics);
                break;

            /*default : return(LDT_xxxxxxx_ADR);*/
        }
    }

   return;
}

/* --- End of Source File --- */