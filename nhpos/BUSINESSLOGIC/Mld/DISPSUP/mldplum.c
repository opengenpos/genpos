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
* Title       : Multi Line Display For MEnu Plu Module  ( SUPER MODE )
* Category    : MLD, NCR 2170 US Hospitality Application Program    
* Program Name: MLDPLUM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls Menu Plu FILE.
*
*    The provided function names are as follows: 
* 
*        MldParaMenuPluDisp()  : Display parameter in Menu PLU to MLD.
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

#include "maintram.h" 

/*
*===========================================================================
*   MLD Format of Menu Plu Maintenance Modeule
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID MldParaMenuPluDisp(PARAMENUPLUTBL *pMep)
*               
*     Input:    pMep->uchArrayAddr
*                   0   : Individual PLU Parameter (Parent PLU)
*                   1-5 : Individual PLU Parameter (Child  PLU)
*                   6   : All PLU Parameter
*
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/

VOID MldParaMenuPluDisp(PARAMENUPLUTBL *pMep)
{
	/*
	This was reformatted to take into consideration that the display
	screen width is 40 characters. Using the following formatting
	sequence will ensure that Plu Mnemonic names, Plu numbers, and
	Plu Type will line up correctly for all languages. ESMITH

	*/
	static CONST TCHAR FARCONST aszMldAC116[][64] = {
		_T("....*....1....*....2....*....3....*....4"),
		_T("%-32sPLU# ADJ"),
		/*                            "Name\tPLU# ADJ ", */
		_T("%-24s%14s %1u "), //_T("%-s\t%13s  %1u "),
							  /*                            "%-s\t%4u     %1u ", */
							  _T("%d/ %-21s%14s %1u "), //_T("%d/ %s\t%13s %1u "),
														/*                            "%d/ %s\t%4u   %1u ", */
														_T("")
	};

    SHORT   sRow;                               /* Display Line Number      */
    SHORT   i;                                  /* Work counter             */
	TCHAR   aszPluMnem[6][PARA_PLU_LEN + 1] = { 0 };  /* Work PLU Mnemonics       */
	TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1] = { 0 };
	TCHAR  aszDummy[PLU_MAX_DIGIT + 1] = { 0 };

    /*********************/
    /* Get PLU Mnemonics */
    /*********************/
    for(i=0; i<6; i++){
		PLUIF   PluIf = { 0 };                      /* PLU Communication buffer */

        if (_tcsncmp(pMep->SetPLU[i].aszPLUNumber, aszDummy, PLU_MAX_DIGIT) == 0) /* if Non-Effective PLU Number */
            continue;                           /* Try the Next             */
        _tcsncpy(PluIf.auchPluNo, pMep->SetPLU[i].aszPLUNumber, PLU_MAX_DIGIT);  /* Set PLU Number           */
        PluIf.uchPluAdj = 1;                    /* Set No. of Adj (1 FIXED) */

        /*
         * Call Individual PLU Parameter Read
         */
        if(CliOpPluIndRead(&PluIf,usMaintLockHnd) == OP_PERFORM){
            _tcsncpy(&(aszPluMnem[i][0]),PluIf.ParaPlu.auchPluName,PARA_PLU_LEN);
        }
    }

    /*****************************/
    /* Display All PLU Parameter */
    /*****************************/
    if(pMep->uchArrayAddr == 6){
		TCHAR aszMnemonics[PARA_TRANSMNEMO_LEN + 1] = { 0 };

        MldScrollClear(MLD_SCROLL_1);              /* Clear scroll display */

        /*
         * Display Item telop
         */
        /* get programmable mnemonics, V3.3 */
		RflGetTranMnem(aszMnemonics, TRN_MNEMONIC_ADR);
        MldPrintf(MLD_SCROLL_1, MLD_SCROLL_TOP+1, &(aszMldAC116[1][0]), aszMnemonics);

        /*
         * Display Parameter
         */
        RflConvertPLU(aszPLUNo, pMep->SetPLU[0].aszPLUNumber);
        MldPrintf(MLD_SCROLL_1, MLD_SCROLL_TOP+2, &(aszMldAC116[2][0]),
                            &(aszPluMnem[0][0]), aszPLUNo, (USHORT)(pMep->SetPLU[0].uchAdjective));

        for(i=3;i<=7;i++){
            RflConvertPLU(aszPLUNo, pMep->SetPLU[i-2].aszPLUNumber);
            MldPrintf(MLD_SCROLL_1, (SHORT)(MLD_SCROLL_TOP+i), &(aszMldAC116[3][0]),
                            i-2, &(aszPluMnem[i-2][0]), aszPLUNo, (USHORT)(pMep->SetPLU[i-2].uchAdjective));
        }
    }

    /**********************************************/
    /* Display Individual PLU Parameter           */
    /*    uchArrayAddr == 0      : Parent PLU     */
    /*    1 <= uchArrayAddr <= 5 : Child  PLU     */
    /**********************************************/
    else{
		TCHAR aszMnemonics[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		sRow = MLD_SCROLL_TOP + pMep->uchArrayAddr + 2; /* Calc Disp line  */

        switch(pMep->uchArrayAddr){
            case 0:                              /* Parent PLU             */
                RflConvertPLU(aszPLUNo, pMep->SetPLU[0].aszPLUNumber);
                MldPrintf(MLD_SCROLL_1, sRow, &(aszMldAC116[2][0]), &(aszPluMnem[0][0]), aszPLUNo, (USHORT)(pMep->SetPLU[0].uchAdjective));
                break;

            case 1:                              /* Child PLU1            */
            case 2:                              /* Child PLU2            */
            case 3:                              /* Child PLU3            */
            case 4:                              /* Child PLU4            */
            case 5:                              /* Child PLU5            */
                RflConvertPLU(aszPLUNo, pMep->SetPLU[pMep->uchArrayAddr].aszPLUNumber);
                MldPrintf(MLD_SCROLL_1, sRow, &(aszMldAC116[3][0]),
                       pMep->uchArrayAddr, &(aszPluMnem[pMep->uchArrayAddr][0]),
                       aszPLUNo, (USHORT)(pMep->SetPLU[pMep->uchArrayAddr].uchAdjective));
                break;

            case 10:                             /* Item Telop             */
                /* get programmable mnemonics, V3.3 */
				RflGetTranMnem(aszMnemonics, TRN_MNEMONIC_ADR);
				MldPrintf(MLD_SCROLL_1, MLD_SCROLL_TOP, &(aszMldAC116[1][0]), aszMnemonics);
                break;

            default :   break;
        }
    }

    return;
}
