/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display total key (Service Total)
* Category    : Multiline Display, NCR 2170 US Hospitarity Application 
* Program Name: MLDSEVT.C                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtServTotal() : displays total key (service total)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial 
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/

/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/

/*
*===========================================================================
** Format  : VOID MldServTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total)
*===========================================================================
*/
SHORT MldServTotal(ITEMTOTAL  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        return (MldDriveServTotal(pItem, usType, usScroll));

    } else {

        return (MldPrechkServTotal(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID MldPrechkServTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total)
*===========================================================================
*/
SHORT MldPrechkServTotal(ITEMTOTAL  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR       auchApproval[1];
    TCHAR       aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG        alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT      usScrollLine = 0;             /* number of lines to be printed */
    USHORT      i;
    TRANCURQUAL TranCurQual;
	TCHAR		tchAcctNo[NUM_NUMBER_EPT+1];

	memset(tchAcctNo, 0x00, sizeof(tchAcctNo));

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	_tcsncpy(tchAcctNo, pItem->aszNumber, NUM_NUMBER_EPT);
	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));

    TrnGetCurQual(&TranCurQual);
    if ((TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
        MldSetSpecialStatus(usScroll, MLD_REORDER_STATUS);
    }

    /* -- initialize the buffer -- */    
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

	//Display Pre-Auth Acct# and Date
	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		if (!ParaMDCCheck(MDC_PREAUTH_MASK_ADR,EVEN_MDC_BIT0)) {

			/* -- mask acct # if applicable -- */
			MldServMaskAcctDate(tchAcctNo, pItem);

			/* -- set mnemonic and number -- */
			usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], tchAcctNo, MLD_PRECHECK_SYSTEM);

			/* -- set exp.date -- */
			/* EPT offline & exp.date   */
			auchApproval[0] = '\0';
			usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, auchApproval, MLD_PRECHECK_SYSTEM);
		}
	} else {
		/* -- set mnemonic and number -- */
		usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], tchAcctNo, MLD_PRECHECK_SYSTEM);

		/* -- set exp.date -- */
		/* EPT offline & exp.date   */
		auchApproval[0] = '\0';
		usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, auchApproval, MLD_PRECHECK_SYSTEM);
	}

	memset (tchAcctNo, 0, sizeof(tchAcctNo));   // clear the memory area per the PABP recommendations

    /* -- set service total -- */
    if (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) {
        /* -- in case of drive through reorder -- */
        usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTtlAdr(pItem), pItem->lMI, MLD_PRECHECK_SYSTEM);
    } else {
        /* -- set service total -- */
        if (pItem->uchMinorClass != CLASS_TOTALPOSTCHECK) {
            usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTtlAdr(pItem), pItem->lService, MLD_PRECHECK_SYSTEM);
        }

        /* -- in case of reorder -- */
        if (MldCheckSpecialStatus(usScroll, MLD_REORDER_STATUS)) {
            /* -- set tax sum mnemonic and amount -- */    
            if (pItem->uchMinorClass != CLASS_TOTALPOSTCHECK) {
                if (pItem->lTax != 0L) {
                    usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TXSUM_ADR, pItem->lTax, MLD_PRECHECK_SYSTEM);
                }
            }

            /* -- set check sum mnemonic and amount -- */    
            usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_CKSUM_ADR, pItem->lMI, MLD_PRECHECK_SYSTEM);
        } else if (pItem->uchMinorClass == CLASS_TOTALPOSTCHECK) {
            /* in case of new check at post check system */
            /* -- set check sum mnemonic and amount -- */    
            usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_CKSUM_ADR, pItem->lMI, MLD_PRECHECK_SYSTEM);
        }
    }
/********
    usScrollLine += MldFolioPost(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszFolioNumber, pItem->aszPostTransNo, MLD_PRECHECK_SYSTEM);
*********/

    /* -- set response message text -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldServTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total)
*===========================================================================
*/
SHORT MldDriveServTotal(ITEMTOTAL  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR   auchApproval[1];
    TCHAR   aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;             /* number of lines to be printed */
    USHORT  i;

    /* -- initialize the buffer -- */    
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set mnemonic and number -- */
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);
    
    /* -- set exp.date -- */
    auchApproval[0] = '\0';
    usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, auchApproval, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set service total mnemonic and amount -- */    
    usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTtlAdr(pItem), pItem->lMI, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set response message text -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*==========================================================================
**  Synopsis:   VOID  MldServMaskAcctDate(TCHAR *pAcctNo, ITEMTOTAL *pItem)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    If Pre-Authorization system, check and mask acct and date
*                   See also function PrtServMaskAcctDate() which does the same
*                   operation for the print subsystem.
==========================================================================*/
VOID  MldServMaskAcctDate(TCHAR *pAcctNo, ITEMTOTAL *pItem)
{
	PARAMDC		MaskMDC;
	int			iShownChars = 0, iMaskedChars = 0, i;

	//Check if Pre-Authorization
	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		//Perform Mask
		if (!ParaMDCCheck(MDC_PREAUTH_MASK_ADR,EVEN_MDC_BIT1)) {
			//How many to mask
			MaskMDC.uchMajorClass = CLASS_PARAMDC;
			MaskMDC.usAddress = MDC_PREAUTH_MASK_ADR;
			ParaMDCRead(&MaskMDC);

			iShownChars = 0;
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				iShownChars = 2;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				iShownChars = 4;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				iShownChars = 6;
			}

			iMaskedChars = NUM_NUMBER_EPT - iShownChars;
			iMaskedChars = iMaskedChars ? iMaskedChars : 0;

			for(i=0; i < iMaskedChars; i++) {
				*pAcctNo = _T('X');
				pAcctNo++;
			}

			for(i=0; i < NUM_EXPIRA; i++) {
				pItem->auchExpiraDate[i] = _T('X');
			}

		}
	}

}
/***** End Of Source *****/

