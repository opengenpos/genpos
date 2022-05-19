/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Add check head
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRAdd1T.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*    PrtAddChk1()    : print addcheck
*    PrtAddChk1_TH() : print addcheck (thermal)
*    PrtAddChk1_EJ() : print addcheck (electric journal)
*    PrtAddChk1_SP() : print addcheck (slip)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 00.00.01 :  R.Itoh    :
* Mar-06-95 : 03.00.00 : T.Nakahata : Modify for Unique Transaction Number
* Jul-20-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Nov-24-95 : 03.01.00 : M.Ozawa    : print out multiple seat no
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>


/**------- 2170 local------**/
#include <ecr.h>
#include <pmg.h>
#include <paraequ.h>
/* #include <para.h> */
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include "prtrin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID  PrtAddChk1(TRANINFORMATION  *pTran,
*                              ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints add check, CLASS_ADDCHECK.
*            This function is one of several functions that are used to print
*            for a CLASS_ITEMTRANSOPEN message when printing a receipt. The
*            particular function will depend on several criteria such as:
*              - system type (Post Guest Check, Store Recall, etc.)
*              - key press used to start a new guest check or transaction
*
*            See function PrtItemTransOpen() for the handling of a CLASS_ITEMTRANSOPEN
*            message and how the minor code is used to determine which of the
*            functions to use.
*===========================================================================
*/
VOID   PrtAddChk1(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem)
{
    SHORT   sWidthType;

    if ( (pTran->TranCurQual.flPrintStatus & (CURQUAL_POSTCHECK | CURQUAL_STORE_RECALL) )
        && (pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL) ) {
        return;     
    }

    /* --- determine current system uses unique trans. no. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }
    
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtAddChk1_SP(pTran, sWidthType);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
        PrtAddChk1_TH(pTran, sWidthType);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
        PrtAddChk1_EJ(pTran, sWidthType);
    } else  if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtAddChk1_EJ(pTran, sWidthType);
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtAddChk1_TH(TRANINFORMATION  *pTran, SHORT sWidthType);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            SHORT  sWidthType      -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in add check. (thermal)
*===========================================================================
*/
VOID  PrtAddChk1_TH(TRANINFORMATION  *pTran, SHORT sWidthType)
{

    USHORT usTable = 0;
    USHORT usNoPer = 0;
                                              

    if (pTran->TranCurQual.fsCurStatus & CURQUAL_PERSONCHANGE) {

        usNoPer = pTran->TranGCFQual.usNoPerson;

    }

    /* --- determine print type is single width or double width --- */

    if ( sWidthType == PRT_SINGLE ) {   /* single width print */

        /* --- store table no., if guest moved other table --- */

        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TABLECHANGE ) {

            usTable = pTran->TranGCFQual.usTableNo;
        }

    } else {                            /* double width print */

        /* --- store unique transaction number --- */

        usTable = pTran->TranGCFQual.usTableNo;
    }


    PrtTHHead(pTran);                      /* print header if necessary */

    PrtTHCustomerName( pTran->TranGCFQual.aszName );
    PrtTHTblPerGC(usTable,
                  usNoPer,pTran->TranGCFQual.usGuestNo,
                  pTran->TranGCFQual.uchCdv,
                  0,
                  sWidthType);
                               /* Table No., No. of person, guest check no mnemo */

}

/*
*===========================================================================
** Format  : VOID  PrtAddChk1_EJ(TRANINFORMATION  *pTran, SHORT sWidthType);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            SHORT  sWidthType      -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in add check. (electric journal)
*===========================================================================
*/
VOID  PrtAddChk1_EJ(TRANINFORMATION  *pTran, SHORT sWidthType )
{
    USHORT usTable = 0;
    USHORT usNoPer = 0;
                                              

    if (pTran->TranCurQual.fsCurStatus & CURQUAL_PERSONCHANGE) {

        usNoPer = pTran->TranGCFQual.usNoPerson;

    }

    /* --- determine print type is single width or double width --- */

    if ( sWidthType == PRT_SINGLE ) {   /* single width print */

        /* --- store table no., if guest moved other table --- */

        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TABLECHANGE ) {

            usTable = pTran->TranGCFQual.usTableNo;
        }

    } else {                            /* double width print */

        /* --- store unique transaction number --- */

        usTable = pTran->TranGCFQual.usTableNo;
    }

    PrtEJCustomerName( pTran->TranGCFQual.aszName );
    PrtEJTblPerson(usTable, usNoPer, 0, sWidthType);   /* Table no.,  no. of person */
                                               /* guest check no mnemo */
    PrtEJGuest(pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv);

}

/*
*===========================================================================
** Format  : VOID  PrtAddChk1_SP(TRANINFORMATION  *pTran, SHORT sWidthType);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            SHORT  sWidthType      -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints dept sales. (slip)
*===========================================================================
*/
VOID  PrtAddChk1_SP(TRANINFORMATION  *pTran, SHORT sWidthType)
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1];   /* print data save area */
    USHORT usTable = 0;
    USHORT usNoPer = 0;
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;

    /* -- initialize the area -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set table No., No. of person mnemonic and amount -- */
    if (pTran->TranCurQual.fsCurStatus & CURQUAL_PERSONCHANGE) {
        usNoPer = pTran->TranGCFQual.usNoPerson;
    }

    /* --- determine print type is single width or double width --- */

    if ( sWidthType == PRT_SINGLE ) {   /* single width print */

        /* --- store table no., if guest moved other table --- */

        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TABLECHANGE ) {

            usTable = pTran->TranGCFQual.usTableNo;
        }

    } else {                            /* double width print */

        /* --- store unique transaction number --- */

        usTable = pTran->TranGCFQual.usTableNo;
    }

    /* -- set table No. and No. of person -- */
    usSlipLine += PrtSPCustomerName( aszSPPrintBuff[ 0 ],
                                     pTran->TranGCFQual.aszName );
    usSlipLine += PrtSPTblPerson( aszSPPrintBuff[ usSlipLine ],
                                  usTable, usNoPer, sWidthType);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
 
    /* if just after header printed, not print table No., No. of person */  
    if ( usSaveLine != 0 ) {

        if ( usNoPer || usTable ) {

/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 1 ], PRT_SPCOLUMN ); */
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 1 ], PRT_SPCOLUMN );

        } else {

/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 0 ], PRT_SPCOLUMN );*/
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 0 ], PRT_SPCOLUMN );
        }

    } else {

        /* -- print all data in the buffer -- */ 

        for ( i = 0; i < usSlipLine; i++ ) {

/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );*/
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;    
}

/* ===== End of File ( PrRAdd1T.C ) ===== */
