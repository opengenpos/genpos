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
* Title       : Print  Trailer                
* Category    : Print, NCR 2170  US Hospitarity Application
* Program Name: PrRTril_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTrail1()       : print normal trailer
*               PrtTrail1_RJ()    : print trailer to receipt
*               PrtTrail1_SP()    : print trailer to slip
*               PrtDoubleHeader() : print double receipt's trailer 
*               PrtTrail2()       : print ticket trailer
*               PrtTrail3()       : print only header
*               PrtSoftProm()     : promotional message for softcheck
*               PrtChkPaid()      : print check paid
*               PrtChkPaid_SP()   : print check paid to slip
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-12-92 : 00.00.01 : K.Nakajima :                                    
* Aug-23-93 : 01.00.13 :  R.Itoh    : add PrtSoftProm()                                    
* Apr-04-94 : 00.00.04 : Yoshiko.Jim: add EPT feature in GPUS
* Apr-28-94 : 02.05.00 : K.You      : bug fixed E-75 (mod. PrtSoftProm)
* Mar-17-94 : 02.05.00 : K.You      : bug fixed S-16 (mod. PrtSoftProm)
* Jul-25-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Sep-04-95 : 03.00.05 : T.Nakahata : bug fixed.
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
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
============================================================================
+                        I N C L U D E   F I L E s
============================================================================
*/
/**------- M S  - C  ------**/
#include	<tchar.h>
#include <string.h>
#include <memory.h>

/**------- 2170 local------**/
#include <ecr.h>
/* #include <pif.h> */
/* #include <rfl.h> */
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include "prtrin.h"
#include "prrcolm_.h"

/**
;========================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtTrail1(TRANINFORMATION  *pTran,
*                              ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Trailer & Header.
*===========================================================================
*/
VOID  PrtTrail1(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    SHORT   fsPortSave;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {      /* slip print */

/*        if ( ! ((pTran->TranGCFQual.fsGCFStatus & GCFQUAL_DRIVE_THROUGH) &&
               ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK)
                == CURQUAL_CASHIER))) {*/
        if ( !((( pTran->TranCurQual.auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_SERVICE1 )||
               (( pTran->TranCurQual.auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_SERVICE2 ))  /* except service total */
            || (auchPrtSeatCtl[0])) {  /* seat no. consolidation print */

             /* not print trailer line when new check/ reorder */
            if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER ||           /* not Guest Check operation */
                (pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK ||          /* not new check or reorder operation */
                (auchPrtSeatCtl[0]) ||  /* seat no. consolidation print */
                pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL ||
                (CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT1))) {   /* 8-2-2000  TAR# 148299 */
            
                if ( !( pTran->TranCurQual.fsCurStatus & CURQUAL_MULTICHECK ) ) {
                    PrtTrail1_SP(pTran, pItem);
                    usPrtSlipPageLine++;                    /* K */
                }
            }
        }
    }

    if ( (fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) { /* receipt, journal print */
        PrtTrail1_RJ(pTran, pItem->ulStoreregNo);
    }
    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) && ( fsPrtStatus & PRT_SERV_JOURNAL )) {
        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        PrtTrail1_RJ( pTran, pItem->ulStoreregNo );

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID   PrtTrail1_RJ(TRANINFORMATION  *pTran,
*                                ULONG   ulStRegNo);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ULONG         ulStRegNo     -Store Reg number
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer & Header.(normal)
*===========================================================================
*/
VOID  PrtTrail1_RJ(TRANINFORMATION  *pTran, ULONG   ulStRegNo)
{
    
    if ( fsPrtPrintPort & PRT_RECEIPT ) {
        PrtRFeed(1);                            /* 1 line feed(Receipt) */
        /* --  Electro provided ? -- */
        if ( !(pTran->TranCurQual.flPrintStatus & CURQUAL_NO_ELECTRO) ) {
            PrtElec();                          /* send electro */
            /* -- send trailer  1st line -- */
            if (usPrtRJColumn == 24) {
                PrtRJTrail1_24(PrtChkPort(), pTran, ulStRegNo);
            } else {
                PrtRJTrail1_21(PrtChkPort(), pTran, ulStRegNo);
            }

            PrtRJTrail2(PrtChkPort(), pTran);   /* send 2nd line */
            PrtFeed(PMG_PRT_RECEIPT, 10);       /* feed */
            PrtCut();                           /* cut */
        } else {
			/* -- Electro not provided -- */    
            if (usPrtRJColumn == 24) {
                PrtRJTrail1_24(PrtChkPort(), pTran, ulStRegNo);
            } else {
                PrtRJTrail1_21(PrtChkPort(), pTran, ulStRegNo);
            }
            PrtRJTrail2(PrtChkPort(), pTran);   /* send 2nd line */
            PrtFeed(PMG_PRT_RECEIPT, 4);        /* feed */
            PrtRJHead();                        /* send header  */
//            PrtFeed(PMG_PRT_RECEIPT, 2);        /* feed */
            PrtCut();                           /* cut */
        }
    } else {
        /* -- jounal requested? -- */
        if ( fsPrtPrintPort & PRT_JOURNAL ) {     
            if (usPrtRJColumn == 24) {
                PrtRJTrail1_24(PMG_PRT_JOURNAL, pTran, ulStRegNo);
            } else {
                PrtRJTrail1_21(PMG_PRT_JOURNAL, pTran, ulStRegNo);
            }
            PrtRJTrail2(PMG_PRT_JOURNAL, pTran); 
        }
    }
}

/*
*===========================================================================
** Format  : VOID   PrtTrail1_SP(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer.(normal)
*===========================================================================
*/
VOID PrtTrail1_SP(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    TCHAR  aszSPPrintBuff[10][PRT_SPCOLUMN + 1];    /* print data save area */
    USHORT  usSlipLine = 0;                         /* lines to be printed  */
    USHORT  i;
    ULONG   ulStoreRegSave;
    
    /* -- initialize the buffer -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- feed & print EPT logo -- */
    if ( pItem->uchMinorClass == CLASS_EPT_TRAILER ) {
        PrtTrailEPT_SP(aszSPPrintBuff[0]);              /* EPT trailer logo */
        usSlipLine += 5;
    }

    if ( pItem->uchMinorClass == CLASS_RETURNS_TRAILER ) {
        PrtTrailEPT_SP(aszSPPrintBuff[0]);              /* EPT trailer logo */
        usSlipLine += 5;
    }

    /* -- set trailer data -- */
    ulStoreRegSave = pTran->TranCurQual.ulStoreregNo;
    pTran->TranCurQual.ulStoreregNo = pItem->ulStoreregNo;
    if (usSlipLine == 0) {
        usSlipLine += PrtSPTrail1(aszSPPrintBuff[0], pTran, usSlipLine);
    } else {
        usSlipLine += PrtSPTrail1(aszSPPrintBuff[usSlipLine], pTran, usSlipLine);
    }
    pTran->TranCurQual.ulStoreregNo = ulStoreRegSave;

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }
    usPrtSlipPageLine += usSlipLine;        /* update current line No. */

    /* -- slip release -- */
    PrtSlpRel();                            
}

/*
*===========================================================================
** Format  : VOID   PrtDoubleHeader(TRANINFORMATION  *pTran);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer & Header.(double header)
*===========================================================================
*/
VOID PrtDoubleHeader(TRANINFORMATION  *pTran)
{
    PrtFeed(PMG_PRT_RECEIPT, 1);                    /* 1 line feed */

    /* --  Electro provided ? -- */
    if ( !(pTran->TranCurQual.flPrintStatus & CURQUAL_NO_ELECTRO) ) {
        PrtElec();                                  /* electro */
        if (usPrtRJColumn == 24) {
            PrtRJTickTrail_24(PrtChkPort(), pTran);
        } else {
            PrtRJTickTrail_21(PrtChkPort(), pTran);
        }
        PrtRJTrail2(PMG_PRT_RECEIPT, pTran);        /* 2nd line */
        PrtFeed(PMG_PRT_RECEIPT, 10);               /* 10 line feed */
        PrtCut();                                   /* cut */
    } else {
		/* -- Electro not provided -- */    
        if (usPrtRJColumn == 24) {
            PrtRJTickTrail_24(PrtChkPort(), pTran);
        } else {
            PrtRJTickTrail_21(PrtChkPort(), pTran);
        }
        PrtRJTrail2(PrtChkPort(), pTran);           /* 2nd line */
        PrtFeed(PMG_PRT_RECEIPT, 4);                /* feed */
        PrtRJHead();                                /* header  */
//        PrtFeed(PMG_PRT_RECEIPT, 2);                /* feed */
        PrtCut();                                   /* cut */
    }

    PrtRJDoubleHead();                              /* double header mnemo */
}

/*
*===========================================================================
** Format  : VOID   PrtTrail2(TRANINFORMATION  *pTran);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints ticket trailer on receipt & journal.
*===========================================================================
*/
VOID PrtTrail2(TRANINFORMATION  *pTran)
{
    PrtFeed(PMG_PRT_RECEIPT, 1);                /* 1 line feed */

    /* --  Electro provided ? -- */
    if ( !(pTran->TranCurQual.flPrintStatus & CURQUAL_NO_ELECTRO) ) {
        PrtElec();                              /* electro */
        if (usPrtRJColumn == 24) {
            PrtRJTickTrail_24(PrtChkPort(), pTran);
        } else {
            PrtRJTickTrail_21(PrtChkPort(), pTran);
        }
        PrtRJTrail2(PMG_PRT_RECEIPT, pTran);    /* 2nd line */
        PrtFeed(PMG_PRT_RECEIPT, 10);           /* 10 line feed */
        PrtCut();                               /* cut */
    } else {
		/* -- Electro not provided -- */    
        if (usPrtRJColumn == 24) {
            PrtRJTickTrail_24(PMG_PRT_RECEIPT, pTran);
        } else {
            PrtRJTickTrail_21(PMG_PRT_RECEIPT, pTran);
        }
        PrtRJTrail2(PMG_PRT_RECEIPT, pTran);    /* 2nd line */
        PrtFeed(PMG_PRT_RECEIPT, 4);            /* feed */
        PrtRJHead();                            /* header  */
//        PrtFeed(PMG_PRT_RECEIPT, 2);            /* feed */
        PrtCut();                               /* cut */

    }
}
/*
*===========================================================================
** Format  : VOID   PrtTrail3(TRANINFORMATION  *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints only Header on receipt & journal.
*            This function is called after "service total print".     
*            "service total print" includes own trailer.      
*===========================================================================
*/
VOID PrtTrail3(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
    PrtTrail1_RJ(pTran, pItem->ulStoreregNo);
}

/*
*===========================================================================
** Format  : VOID   PrtTrayTrail(TRANINFORMATION  *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints  trailer and header on receipt.
*            when printed on slip, this function does not print.
*===========================================================================
*/
VOID PrtTrayTrail(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( !(fsPrtPrintPort & PRT_SLIP) ) {       /* slip printing ?  */

        PrtTrail1_RJ(pTran, pItem->ulStoreregNo);
    }
}

/*
*===========================================================================
** Format  : VOID   PrtSoftProm(ITEMPRINT *pItem);      
*
*   Input  : ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints promotional message on receipt in soft check system.
*===========================================================================
*/
VOID PrtSoftProm(ITEMPRINT *pItem)
{   
    UCHAR   uchI, uchTimes=0;
    PARASOFTCHK  ST;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);   

    if ( fsPrtPrintPort & PRT_RECEIPT ) {                   /* thermal print    */
        ST.uchMajorClass = CLASS_PARASOFTCHK;
        if (pItem->uchMinorClass == CLASS_RETURNS_TRAILER) {
            uchI = SOFTCHK_RTN1_ADR;                    /* Returns logo message */
		} else if (pItem->uchMinorClass == CLASS_EPT_TRAILER) {
            uchI = SOFTCHK_EPT1_ADR;                    /* EPT logo message */
        } else {
            uchI = SOFTCHK_MSG1_ADR;                    /* Soft Check msg   */
        }
        for ( uchI=uchI; uchTimes < 5; uchI++ ) {       /* E-047 corr.4/20  */
            ST.uchAddress = uchI;
            CliParaRead(&ST);
            if (uchI < SOFTCHK_EPT1_ADR) {              /* soft check only  */
                if ( PRT_FULL_SP == PrtChkFullSP(ST.aszMnemonics) ) {
                    return;                             /* not print        */
                }

                PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed      */
            } else if (ST.aszMnemonics[0] == '\0') {
                ST.aszMnemonics[0] = _T(' ');                        
            }
            if ( usPrtRJColumn == PRT_24CHAR ) {
                ST.aszMnemonics[24] = _T('\0');                        
            
            } else {            
                ST.aszMnemonics[21] = _T('\0');                        
            
            }
            if ((usPrtRJColumn == PRT_21CHAR) && (ST.aszMnemonics[20] == 0x12)) {
                ST.aszMnemonics[20] = _T('\0');
            }

            PmgPrint(PMG_PRT_RECEIPT, ST.aszMnemonics, (USHORT)_tcslen(ST.aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */
            uchTimes++;                                 /* E-047 corr. 4/20 */
        }        
    }   
}
        
/*
*===========================================================================
** Format  : VOID   PrtChkPaid(ITEMPRINT *pItem);
*
*   Input  : ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Trailer & Header.
*===========================================================================
*/
VOID PrtChkPaid(ITEMPRINT  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtChkPaid_SP();
    }

}

/*
*===========================================================================
** Format  : VOID   PrtChkPaid_SP(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Trailer & Header.
*===========================================================================
*/
VOID PrtChkPaid_SP(VOID)
{
    TCHAR  aszSPPrintBuff[PRT_SPCOLUMN + 1];    /* print data save area */
    
    /* -- initialize the buffer -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set check paid data -- */
    PrtSPChkPaid(aszSPPrintBuff, TRN_CKPD_ADR);

    /* -- print all data in the buffer -- */ 
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);       

    /* -- slip release -- */
    PrtSlpRel();                            
}
