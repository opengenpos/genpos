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
* Title       : DEPT/PLU Setting Module                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPPLU20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupDEPTPLU20()  : Display DEPT,PLU,CPN
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-11-92: 00.00.01 : K.You     : initial
* Mar-15-95: 03.00.00 : H.Ishida  : Add process of CLASS_PARACPN
* Jul-20-95: 03.00.01 : M.Ozawa   : Display Coupon Group No.
*          :          :           :                                    
*
** NCR2171 **                                         
* Aug-26-99: 01.00.00 : M.Teraki  :initial (for 2171)
* Jan-12-00: 01.00.00 : hrkato    :Saratoga
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
#include <stdlib.h>

/* #include <stdio.h> */
#include "ecr.h"
#include "uie.h"
/* #include <pif.h> */
/* #include <log.h> */
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include <appllog.h>
#include "display.h"
#include "disp20.h"

/*
*=======================================================================================
** Synopsis:    VOID DispSupDEPTPLU20( VOID *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARADEPT,PARAPLU  
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays DEPT,PLU,CPN
*=======================================================================================
*/

VOID DispSupDEPTPLU20( VOID *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupDEPTPLU20A[] = _T("%3u %s %2s\t%s\n%-18s\t%s");
    static const TCHAR  auchDispSupDEPTPLU20B[] = _T("%3u %s %2s\t%8s\n%-18s\t%s");

    CONST UCHAR  uchMajor = *(UCHAR *)(pData);         /* Get Major Class */
    CONST UCHAR  uchMinor = *(((UCHAR *)pData) + 1);   /* Get Minor Class */
    UCHAR       uchPageNumber = 0;
    UCHAR       uchBase = 10;
    UCHAR       fchCpnAdj  = 0;
    TCHAR       aszDspNull[1] = {0};
    TCHAR       aszBuffer1[7] = { 0 };                  // field address as text
	TCHAR       aszBuffer2[PLU_MAX_DIGIT + 1] = { 0 };  // PLU No., Dept. No., Coupon No. as text
    TCHAR       aszBuffer3[36] = { 0 };                 // this buffer used as _tcscat() target so make it big.
	TCHAR       aszBuffer4[PARA_PLU_LEN + 1] = { 0 };   // Lead Thru mnemonic or PLU mnemonic
    TCHAR       aszBuffer5[7] = { 0 };
    USHORT      usNoData;
    USHORT      usAdj = 0, usGroup = 0;
    ULONG       ulAmount = 0;
    MAINTDEPT   * const pMaintDEPT = pData;
    MAINTPLU    * const pMaintPLU = pData;
    MAINTCPN    * const pMaintCPN = pData;
        
    /* Distinguish Major Class */
    switch (uchMajor) {
    case CLASS_PARADEPT:                                        /* DEPT Case */
		_itot(pMaintDEPT->uchFieldAddr, aszBuffer1, 10);        /* Convert Address Number. to ASCII Data */
		_itot(pMaintDEPT->usDEPTNumber, aszBuffer2, 10);        /* Convert DEPT Number to ASCII Data */
		usNoData = pMaintDEPT->usACNumber;                      /* Set A/C Number */
        ulAmount = pMaintDEPT->ulInputData;                     /* Set Input Data */
        _tcsncpy(aszBuffer4, pMaintDEPT->aszMnemonics, PARA_LEADTHRU_LEN);    /* Set LeadThru Mnemonics */

		DispSupSaveData.MaintDept = *pMaintDEPT;                /* Save Data for Redisplay if needed. */
        
        if (uchMinor == CLASS_PARADEPT_STS) {                   /* Status Field Display Case */
            uchBase = 2;                
        }
        break;

    case CLASS_PARAPLU:                                         /* PLU Case */
		_itot(pMaintPLU->uchFieldAddr, aszBuffer1, 10);         /* Convert Address Number. to ASCII Data */
		RflConvertPLU(aszBuffer2, pMaintPLU->auchPLUNumber);    /* Set PLU Number,  Saratoga */
        //aszBuffer2[0] = _T('\0');                               /* RFC 01/30/2001 */
        
        if (PifSysConfig()->uchOperType == DISP_2X20) {
            aszBuffer2[0] = _T('\0');
        }
        
        usNoData = pMaintPLU->usACNumber;                       /* Set A/C Number */
        ulAmount = pMaintPLU->ulInputData;                      /* Set Input Data */
        uchPageNumber = pMaintPLU->uchPageNumber;               /* Set Page Number */
        _tcsncpy(aszBuffer4, pMaintPLU->aszMnemonics, PARA_PLU_LEN);  /* Set PLU Mnem, Saratoga */

		DispSupSaveData.MaintPlu = *pMaintPLU;               /* Save Data for Redisplay if needed. */

        /* Define Data Category by Minor Class */
        if (uchMinor == CLASS_PARAPLU_STS) {                   /* Status Field Display Case */
            uchBase = 2;                
        }
        break;

    case CLASS_PARACPN:                                         /* Combination Coupon Case */
		_itot(pMaintCPN->uchFieldAddr, aszBuffer1, 10);         /* Convert Address Number. to ASCII Data */
		_itot(pMaintCPN->usACNumber, aszBuffer2, 10);           /* Convert CPN Number to ASCII Data */
		usNoData = pMaintCPN->usACNumber;                       /* Set A/C Number */
        usAdj    = ( USHORT)(pMaintCPN->uchCPNAdj & 0x0F);      /* Set Adjective Code */
        usGroup  = ( USHORT)((pMaintCPN->uchCPNAdj & 0xF0) >> 4); /* Set Group Code */
        ulAmount = pMaintCPN->ulInputData;                      /* Set Input Data */
        _tcsncpy(aszBuffer4, pMaintCPN->aszMnemonics, PARA_LEADTHRU_LEN);  /* Set LeadThru Mnemonics */

		DispSupSaveData.MaintCpn = *pMaintCPN;          /* Save Data for Redisplay if needed. */
        
        if (uchMinor == CLASS_PARACPN_STS) {                    /* Status Field Display Case */
            uchBase = 2;                
        } else if (uchMinor == CLASS_PARACPN_ONEADJ ) {         /* Adjective Display */
            fchCpnAdj = 1;
        } else if (uchMinor == CLASS_PARACPN_GROUPCODE) {
            fchCpnAdj = 2;
        }
   
        break;

    default:
        PifLog(MODULE_DISPSUP_ID, LOG_EVENT_UI_DISPLAY_ERR_03);
		return;
        break;
    }

    if (fchCpnAdj == 1) {                                         /* display with adjective code */
			TCHAR       aszBuffer6[8] = { 0 };

			RflConvertPLU(aszBuffer3, pMaintCPN->auchCPNTarget);
            /*ltoa(ulAmount, aszBuffer3, uchBase);                / Convert Input Data to ASCII by Base Number */
            _tcscat(aszBuffer3, _T("-"));
            _itot(usAdj, aszBuffer6, 10);
            _tcscat(aszBuffer3, aszBuffer6);                      /* Save Data */  
    } else if (fchCpnAdj == 2) {                                  /* display terget plu group code */
            _itot(usGroup, aszBuffer3, 10);
    } else {
            _ltot(ulAmount, aszBuffer3, uchBase);                /* Convert Input Data to ASCII by Base Number */
    }

    if (uchMaintMenuPage) {                                        /* Page Number Defined */
        _itot(uchMaintMenuPage, aszBuffer5, 10);                    /* Convert Page Number to ASCLL Data */
    }

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                         /* Operator Display */
              0,                                /* Row */
              0,                                /* Column */
              auchDispSupDEPTPLU20A,            /* Format */
              usNoData,                         /* Super/Program number */
              aszBuffer2,                       /* DEPT(PLU) Number */
              aszBuffer1,                       /* Address */
              aszBuffer3,                       /* Amount */
              aszBuffer4,                       /* Lead Through Mnemonics */
              aszBuffer5);                      /* Page Number */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupDEPTPLU20B,    /* Format */
                      usNoData,                 /* Super/Program number */
                      aszBuffer2,               /* DEPT(PLU) Number */
                      aszBuffer1,               /* Address */
                      aszDspNull,               /* Null */
                      aszBuffer4,               /* Lead Through Mnemonics */    
                      aszBuffer5);              /* Page Number */
}

/**** End of File ****/