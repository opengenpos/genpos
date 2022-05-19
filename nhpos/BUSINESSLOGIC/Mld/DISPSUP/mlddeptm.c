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
* Program Name: MLDDEPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls COUPON PARAMETER FILE.
*
*    The provided function names are as follows: 
* 
*        MldParaDEPTDisp()  : Display parameter in Department Files to MLD.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-95: 03.00.00 : T.Satoh   : initial                                   
* Nov-22-95: 03.01.00 : M.Ozawa   : add status9
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
*   MLD Format of Department Maintenance Modeule
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID MldParaDEPTDisp(PARADEPT *pDept)
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

VOID MldParaDEPTDisp(PARADEPT *pDept)
{
    TCHAR   aszStatus[10][7];                    /* Work Status strings      */
    SHORT   sRow;                               /* Display Line Number      */
    SHORT   i;                                  /* Work counter             */
    MLDIF   MldIf;
    SHORT   usColumn, sReturn;

    /*
     * Initialize Display Format and Pre-Set character strings of Status
     */
//    for(i=0;i<9;i++){
//        memset(&(aszStatus[i][0]),0,7);
//    }

	memset(&aszStatus, 0x00, sizeof(aszStatus));
    /*************************************************/
    /* Convert DEPT Status Data to Binary ASCII Data */
    /*************************************************/
    PrtSupItoa((UCHAR)( pDept->auchDEPTStatus[0]&0x0F),    &(aszStatus[0][0]));
    PrtSupItoa((UCHAR)((pDept->auchDEPTStatus[0]&0xF0)>>4),&(aszStatus[1][0]));
    PrtSupItoa((UCHAR)( pDept->auchDEPTStatus[1]&0x0F),    &(aszStatus[2][0]));
    PrtSupItoa((UCHAR)((pDept->auchDEPTStatus[1]&0xF0)>>4),&(aszStatus[3][0]));
    PrtSupItoa((UCHAR)( pDept->auchDEPTStatus[2]&0x0F),    &(aszStatus[4][0]));
    PrtSupItoa((UCHAR)((pDept->auchDEPTStatus[2]&0xF0)>>4),&(aszStatus[5][0]));
    _itot(pDept->auchDEPTStatus[3]&0x0F,&(aszStatus[6][0]),10);
    PrtSupItoa((UCHAR)((pDept->auchDEPTStatus[3]&0xF0)>>4),&(aszStatus[7][0]));
    PrtSupItoa((UCHAR)( pDept->auchDEPTStatus[4]&0x0F),    &(aszStatus[8][0]));
    PrtSupItoa((UCHAR)((pDept->auchDEPTStatus[4]&0xF0)>>4),&(aszStatus[9][0]));

    /**************************/
    /* Display All Parameters */
    /**************************/
    if(pDept->uchFieldAddr == 0){
        MldScrollClear(MLD_SCROLL_1);              /* Clear scroll display */

        /*
         * Major Department Number
         */
        /* get programmable mnemonics, V3.3 */

        MldIf.usAddress = aszMldAC114[DPT_FIELD1_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }

        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-3;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD1_ADR-1, aszMldAC114[DPT_FIELD1_ADR].aszData, MldIf.aszMnemonics, (USHORT)(pDept->uchMajorDEPT));

        /*
         * Status 1-8
         */
        for(i=DPT_FIELD2_ADR;i<=DPT_FIELD9_ADR;i++){
            MldIf.usAddress = aszMldAC114[i].usAddress;
            sReturn = CliOpMldIndRead(&MldIf, 0);
            if (sReturn != OP_PERFORM){
                MldIf.aszMnemonics[0] = '\0';
            }
            /* set null at data display position */
            usColumn = MLD_MENU_COLUMN-3-5;
            if (usColumn < OP_MLD_MNEMONICS_LEN) {
                MldIf.aszMnemonics[usColumn] = '\0';
            }
            MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i-1), aszMldAC114[i].aszData, MldIf.aszMnemonics, &(aszStatus[i-2][0]));
        }

        /*
         * HALO
         */
        MldIf.usAddress = aszMldAC114[DPT_FIELD10_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-3;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD10_ADR-1, aszMldAC114[DPT_FIELD10_ADR].aszData, MldIf.aszMnemonics, (USHORT)(pDept->uchHALO));

        /*
         * Mnemonic 
         */
        MldIf.usAddress = aszMldAC114[DPT_FIELD11_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-14;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[MLD_MENU_COLUMN-3-14] = '\0';
        }
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD11_ADR-1, aszMldAC114[DPT_FIELD11_ADR].aszData, MldIf.aszMnemonics, pDept->aszMnemonic);

        /*
         * Print Priority
         */
        MldIf.usAddress = aszMldAC114[DPT_FIELD12_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-3;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD12_ADR-1, aszMldAC114[DPT_FIELD12_ADR].aszData, MldIf.aszMnemonics, pDept->uchPrintPriority);

        /*
         * Status 9
         */
        MldIf.usAddress = aszMldAC114[DPT_FIELD13_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-5;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD13_ADR-1, aszMldAC114[DPT_FIELD13_ADR].aszData, MldIf.aszMnemonics, &(aszStatus[8][0]));

        /*
         * Status 10
         */
        MldIf.usAddress = aszMldAC114[DPT_FIELD14_ADR].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }
        /* set null at data display position */
        usColumn = MLD_MENU_COLUMN-3-5;
        if (usColumn < OP_MLD_MNEMONICS_LEN) {
            MldIf.aszMnemonics[usColumn] = '\0';
        }
        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD14_ADR-1, aszMldAC114[DPT_FIELD14_ADR].aszData, MldIf.aszMnemonics, &(aszStatus[9][0]));

    }

    /********************************/
    /* Display Individual Parameter */
    /********************************/
    else{
        sRow = MLD_SCROLL_TOP + pDept->uchFieldAddr - 1; /* Calc Disp line  */

        /* get programmable mnemonics, V3.3 */

        MldIf.usAddress = aszMldAC114[pDept->uchFieldAddr].usAddress;
        sReturn = CliOpMldIndRead(&MldIf, 0);
        if (sReturn != OP_PERFORM){
            MldIf.aszMnemonics[0] = '\0';
        }

        switch(pDept->uchFieldAddr){
            case DPT_FIELD1_ADR:                        /* Major Dept No.   */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, (USHORT)(pDept->uchMajorDEPT));
                break;

            case DPT_FIELD2_ADR:                        /* Status 1         */
            case DPT_FIELD3_ADR:                        /* Status 2         */
            case DPT_FIELD4_ADR:                        /* Status 3         */
            case DPT_FIELD5_ADR:                        /* Status 4         */
            case DPT_FIELD6_ADR:                        /* Status 5         */
            case DPT_FIELD7_ADR:                        /* Status 6         */
            case DPT_FIELD8_ADR:                        /* Status 7         */
            case DPT_FIELD9_ADR:                        /* Status 8         */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-5;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, &(aszStatus[pDept->uchFieldAddr-2][0]));
                break;

            case DPT_FIELD10_ADR:                       /* HALO            */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, (USHORT)(pDept->uchHALO));
                break;

            case DPT_FIELD11_ADR:                       /* Mnemonic        */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-14;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, pDept->aszMnemonic);
                break;

            case DPT_FIELD12_ADR:                       /* Print Priority  */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-3;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,sRow, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, pDept->uchPrintPriority);
                break;

            case DPT_FIELD13_ADR:                       /* Status 13        */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-5;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD13_ADR-1, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, &(aszStatus[8][0]));
                break;

            case DPT_FIELD14_ADR:                       /* Status 13        */
                /* set null at data display position */
                usColumn = MLD_MENU_COLUMN-3-5;
                if (usColumn < OP_MLD_MNEMONICS_LEN) {
                    MldIf.aszMnemonics[usColumn] = '\0';
                }
                MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+DPT_FIELD14_ADR-1, aszMldAC114[pDept->uchFieldAddr].aszData, MldIf.aszMnemonics, &(aszStatus[9][0]));

            default :   break;
        }
    }

    return;
}
