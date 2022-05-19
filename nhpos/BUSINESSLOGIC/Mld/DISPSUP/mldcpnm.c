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
* Title       : Multi Line Display For CouPoN Module  ( SUPER MODE )
* Category    : MLD, NCR 2170 US Hospitality Application Program    
* Program Name: MLDCPN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls COUPON PARAMETER FILE.
*
*    The provided function names are as follows: 
* 
*        MldParaCPNDisp()  : Display parameter in Coupon Files to MLD.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-95: 03.00.00 : T.Satoh   : initial                                   
* Jul-20-95: 03.00.01 : M.Ozawa   : add coupon group code display
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
*   MLD Format of Coupon Maintenance Modeule
*===========================================================================
*/
static CONST MLDMENU FARCONST aszMldAC161[] = {
	{ -1,_T("") },
	{ LCD_CPN_ADDR1_ADR,_T(" 1 %s\t%4s ") },
	{ LCD_CPN_ADDR2_ADR,_T(" 2 %s\t%4s ") },
	{ LCD_CPN_ADDR3_ADR,_T(" 3 %s\t%4s ") },
	{ LCD_CPN_ADDR4_ADR,_T(" 4 %s\t%4s ") },
	{ LCD_CPN_ADDR5_ADR,_T(" 5 %s\t%7#l$ ") },
	{ LCD_CPN_ADDR6_ADR,_T(" 6 %s\t%12s ") },
	{ LCD_CPN_ADDR7_ADR,_T(" 7 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR8_ADR,_T(" 8 %s\t%1d ") },
	{ LCD_CPN_ADDR9_ADR,_T(" 9 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR10_ADR,_T("10 %s\t%1d ") },
	{ LCD_CPN_ADDR11_ADR,_T("11 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR12_ADR,_T("12 %s\t%1d ") },
	{ LCD_CPN_ADDR13_ADR,_T("13 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR14_ADR,_T("14 %s\t%1d ") },
	{ LCD_CPN_ADDR15_ADR,_T("15 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR16_ADR,_T("16 %s\t%1d ") },
	{ LCD_CPN_ADDR17_ADR,_T("17 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR18_ADR,_T("18 %s\t%1d ") },
	{ LCD_CPN_ADDR19_ADR,_T("19 %s\t%13s-%1d ") },
	{ LCD_CPN_ADDR20_ADR,_T("20 %s\t%1d ") },
	{ LCD_CPN_ADDR21_ADR,_T("21 %s\t%1d ") },
	{ LCD_CPN_ADDR22_ADR,_T("22 %s\t%ld ") },
	{ 0,_T("") }
};
/***
CONST UCHAR FARCONST aszMldAC161[][64] = {
"....*....1....*....2....*....3....*....4",
" 1 Tax4/Tax3/Tax2/Tax1\t%4s ",
" 2 Disc2/Disc1/Single/Not Use\t%4s ",
" 3 Targ4/Targ3/Targ2/Targ1\t%4s ",
" 4 Not Use/Targ7/Targ6/Targ5\t%4s ",
" 5 Amount\t%7#l$ ",
" 6 Coupon Name\t%12s ",
" 7 Target#1 DEPT/PLU-ADJ\t%4d-%1d ",
" 8 Target#1 Group Code\t%1d ",
" 9 Target#2 DEPT/PLU-ADJ\t%4d-%1d ",
"10 Target#2 Group Code\t%1d ",
"11 Target#3 DEPT/PLU-ADJ\t%4d-%1d ",
"12 Target#3 Group Code\t%1d ",
"13 Target#4 DEPT/PLU-ADJ\t%4d-%1d ",
"14 Target#4 Group Code\t%1d ",
"15 Target#5 DEPT/PLU-ADJ\t%4d-%1d ",
"16 Target#5 Group Code\t%1d ",
"17 Target#6 DEPT/PLU-ADJ\t%4d-%1d ",
"18 Target#6 Group Code\t%1d ",
"19 Target#7 DEPT/PLU-ADJ\t%4d-%1d ",
"20 Target#7 Group Code\t%1d ",
""
};
***/

/*
*===========================================================================
** Synopsis:    VOID MldParaCPNDisp(PARACPN *pCpn)
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

VOID MldParaCPNDisp(PARACPN *pCpn, USHORT usStatus)
{
    SHORT   sRow;                               /* Display Line Number      */
    TCHAR   aszStatus[4][5];                    /* Work Status1 strings     */
    SHORT   i;                                  /* Work counter             */
    SHORT   usTerAdr;
    MLDIF   MldIf;
    SHORT   usColumn, sReturn;
    TCHAR  aszPLUNo[PLU_MAX_DIGIT+1];

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));

    /*
     * Initialize Display Format and Pre-Set character strings of Status
     */
    /*memset(uchMldDispLine,'\0',41);*/
    PrtSupItoa((UCHAR)( pCpn->uchCPNStatus[0] & 0x0F),    &(aszStatus[0][0]));
    PrtSupItoa((UCHAR)((pCpn->uchCPNStatus[0] & 0xF0)>>4),&(aszStatus[1][0]));
    PrtSupItoa((UCHAR)( pCpn->uchCPNStatus[1] & 0x0F),    &(aszStatus[2][0]));
    PrtSupItoa((UCHAR)((pCpn->uchCPNStatus[1] & 0xF0)>>4),&(aszStatus[3][0]));

    /**************************/
    /* Display All Parameters */
    /**************************/
    if(pCpn->uchFieldAddr == 0){

		if (!usStatus) {
			MldScrollClear(MLD_SCROLL_1);              /* Clear scroll display */

			for(i=CPN_FIELD1_ADR;i<=CPN_FIELD4_ADR;i++){
				/* get programmable mnemonics, V3.3 */

				MldIf.usAddress = aszMldAC161[i].usAddress;
				sReturn = CliOpMldIndRead(&MldIf, 0);
				if (sReturn != OP_PERFORM){
					MldIf.aszMnemonics[0] = '\0';
				}
				/* set null at data display position */
				usColumn = MLD_MENU_COLUMN-3-5;
				if (usColumn < OP_MLD_MNEMONICS_LEN) {
					MldIf.aszMnemonics[usColumn] = '\0';
				}
				MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC161[i].aszData, MldIf.aszMnemonics, &(aszStatus[i-1][0]));
			}

			MldIf.usAddress = aszMldAC161[CPN_FIELD5_ADR].usAddress;
			sReturn = CliOpMldIndRead(&MldIf, 0);
			if (sReturn != OP_PERFORM){
				MldIf.aszMnemonics[0] = '\0';
			}
			/* set null at data display position */
			usColumn = MLD_MENU_COLUMN-3-14;
			if (usColumn < OP_MLD_MNEMONICS_LEN) {
				MldIf.aszMnemonics[usColumn] = '\0';
			}
			MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+CPN_FIELD5_ADR-1, aszMldAC161[CPN_FIELD5_ADR].aszData, MldIf.aszMnemonics, pCpn->ulCPNAmount);

			MldIf.usAddress = aszMldAC161[CPN_FIELD6_ADR].usAddress;
			sReturn = CliOpMldIndRead(&MldIf, 0);
			if (sReturn != OP_PERFORM){
				MldIf.aszMnemonics[0] = '\0';
			}
			/* set null at data display position */
			usColumn = MLD_MENU_COLUMN-3-12;
			if (usColumn < OP_MLD_MNEMONICS_LEN) {
				MldIf.aszMnemonics[usColumn] = '\0';
			}
			MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+CPN_FIELD6_ADR-1, aszMldAC161[CPN_FIELD6_ADR].aszData, MldIf.aszMnemonics, pCpn->aszMnemonic);

			for(i=CPN_FIELD7_ADR; i<CPN_FIELD20_ADR; i++){

				MldIf.usAddress = aszMldAC161[i].usAddress;
				sReturn = CliOpMldIndRead(&MldIf, 0);
				if (sReturn != OP_PERFORM){
					MldIf.aszMnemonics[0] = '\0';
				}
				/* set null at data display position */
				usColumn = MLD_MENU_COLUMN-3-7;
				if (usColumn < OP_MLD_MNEMONICS_LEN) {
					MldIf.aszMnemonics[usColumn] = '\0';
				}
				RflConvertPLU(aszPLUNo, &(pCpn->auchCPNTarget[(i-7)/2][0]));
				MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC161[i].aszData, MldIf.aszMnemonics, aszPLUNo, (USHORT)(pCpn->uchCPNAdj[(i-7)/2] & 0x0F));

				i++;
				MldIf.usAddress = aszMldAC161[i].usAddress;
				sReturn = CliOpMldIndRead(&MldIf, 0);
				if (sReturn != OP_PERFORM){
					MldIf.aszMnemonics[0] = '\0';
				}
				/* set null at data display position */
				usColumn = MLD_MENU_COLUMN-3-3;
				if (usColumn < OP_MLD_MNEMONICS_LEN) {
					MldIf.aszMnemonics[usColumn] = '\0';
				}
				MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC161[i].aszData, MldIf.aszMnemonics, (USHORT)((pCpn->uchCPNAdj[(i-7)/2] & 0xF0)>>4) );
			}
		} else
		{
			MldScrollClear(MLD_SCROLL_1);              /* Clear scroll display */

			MldIf.usAddress = aszMldAC161[CPN_FIELD21_ADR].usAddress;
					sReturn = CliOpMldIndRead(&MldIf, 0);
					if (sReturn != OP_PERFORM){
						MldIf.aszMnemonics[0] = '\0';
					}
					/* set null at data display position */
					usColumn = MLD_MENU_COLUMN-3-12;
					if (usColumn < OP_MLD_MNEMONICS_LEN) {
						MldIf.aszMnemonics[usColumn] = '\0';
					}
					MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP-1, aszMldAC161[CPN_FIELD21_ADR].aszData, MldIf.aszMnemonics,  pCpn->uchAutoCPNStatus);

			MldIf.usAddress = aszMldAC161[CPN_FIELD22_ADR].usAddress;
					sReturn = CliOpMldIndRead(&MldIf, 0);
					if (sReturn != OP_PERFORM){
						MldIf.aszMnemonics[0] = '\0';
					}
					/* set null at data display position */
					usColumn = MLD_MENU_COLUMN-3-12;
					if (usColumn <OP_MLD_MNEMONICS_LEN) {
						MldIf.aszMnemonics[usColumn] = '\0';
					}
					MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+2-1, aszMldAC161[CPN_FIELD22_ADR].aszData, MldIf.aszMnemonics, pCpn->usBonusIndex);
		}
    }

    /********************************/
    /* Display Individual Parameter */
    /********************************/
    else{
        sRow = MLD_SCROLL_TOP + pCpn->uchFieldAddr - 1; /* Calc Display line */

		if(pCpn->uchFieldAddr > 20)
		{
			sRow -= 20;
		}

        usTerAdr = (pCpn->uchFieldAddr-7)/2;

        MldIf.usAddress = aszMldAC161[pCpn->uchFieldAddr].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }

        switch(pCpn->uchFieldAddr){
            case CPN_FIELD1_ADR:
            case CPN_FIELD2_ADR:
            case CPN_FIELD3_ADR:
            case CPN_FIELD4_ADR:
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-5;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, &(aszStatus[pCpn->uchFieldAddr-1][0]));
                break;

            case CPN_FIELD5_ADR:
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-14;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, pCpn->ulCPNAmount);
                break;

            case CPN_FIELD6_ADR:
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-14;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, pCpn->aszMnemonic);
                break;

            case CPN_FIELD7_ADR:
            case CPN_FIELD9_ADR:
            case CPN_FIELD11_ADR:
            case CPN_FIELD13_ADR:
            case CPN_FIELD15_ADR:
            case CPN_FIELD17_ADR:
            case CPN_FIELD19_ADR:
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-7;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                RflConvertPLU(aszPLUNo, &(pCpn->auchCPNTarget[usTerAdr][0]));
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, aszPLUNo, (USHORT)(pCpn->uchCPNAdj[usTerAdr] & 0x0F));
                break;

            case CPN_FIELD8_ADR:
            case CPN_FIELD10_ADR:
            case CPN_FIELD12_ADR:
            case CPN_FIELD14_ADR:
            case CPN_FIELD16_ADR:
            case CPN_FIELD18_ADR:
            case CPN_FIELD20_ADR:
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-2;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, (USHORT)((pCpn->uchCPNAdj[usTerAdr] & 0xF0)>>4) );
                break;

			case CPN_FIELD21_ADR:  // Added to update CPN status
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-2;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,(sRow), aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, (USHORT)(pCpn->uchAutoCPNStatus) );
                break;

			case CPN_FIELD22_ADR:	// Added to update CPN Bonus Index
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-2;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,(sRow), aszMldAC161[pCpn->uchFieldAddr].aszData, MldIf.aszMnemonics, (USHORT)(pCpn->usBonusIndex) );
                break;

            default :   break;
        }
    }

    return;
}

/***** End of File ****/
