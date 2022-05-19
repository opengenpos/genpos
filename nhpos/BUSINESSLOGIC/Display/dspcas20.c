/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*==========================================================================
* Title       : Cashier No. Assignment Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 Int'l Hospitality Application Program        
* Program Name: DSPCAS20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*			    DispSupCashierNo20()    : Display Cashier No. Assignment
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-29-92: 00.00.01 : K.You     : Initial                                   
* Aug-18-98: 03.03.00 : H.Iwata   :                                    
*** GenPOS **
* Oct-02-17 : 02.02.02  : R.Chambers  : clean up based on inspection.
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
#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "cscas.h"
#include "maint.h"
#include "display.h"

#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupCashierNo20( MAINTCASHIERIF *pData )
*               
*     Input:    *pData          : Pointer to Structure for MAINTCASHIERIF
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function displays Cashier No. Assignment Data
*===========================================================================
*/

VOID DispSupCashierNo20( MAINTCASHIERIF *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupCashierNo20A[] = _T("%3u %3s %s\t%s\n%-20s");
    static const TCHAR  auchDispSupCashierNo20B[] = _T("%3u %3s %s\t%8s\n%-20s");
    static const TCHAR  auchDispSupCashierNo20C[] = _T("%3u %3s %s %4s\t%d\n%-20s");
    static const TCHAR  auchDispSupCashierNo20D[] = _T("%3u %3s %s %4s\t%6s\n%-20s");

    TCHAR       aszDspNull[1] = { 0 };
    TCHAR       aszBuffer1[8] = { 0 };
    TCHAR       aszBuffer2[12] = { 0 };
    TCHAR       aszBuffer3[8] = { 0 };
	TCHAR       aszBuffer4[PARA_CASHIER_LEN+1] = {0};

    USHORT      usGCNumber;

    NHPOS_ASSERT(sizeof(aszBuffer4) >= sizeof(pData->CashierIf.auchCashierName));

    DispSupSaveData.MaintCashierIf = *pData;    /* Save Data for Redisplay if needed. */

    _itot(pData->uchFieldAddr, aszBuffer1, 10);                          /* Convert Field Data to ASCII Data */
    _itot(pData->CashierIf.ulCashierNo, aszBuffer2, 10);                 /* Convert Cashier No. to ASCII Data */
    usGCNumber = pData->CashierIf.usGstCheckStartNo;

    switch(pData->uchMinorClass){
    case CLASS_PARACASHIERNO_STS:
        _itot(pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0], aszBuffer3, 2);       /* Convert Status Data to ASCII by Base Number */
        break;

    case CLASS_PARACASHIERNO_DATA2:
    case CLASS_PARACASHIERNO_DATA4:                                     /* Display Start GC No. */
	case CLASS_PARACASHIERNO_DATA5:
        _itot(pData->CashierIf.usGstCheckStartNo, aszBuffer3, 10);       /* Convert GC Start No to ASCII by Base Number */
        break;

    case CLASS_PARACASHIERNO_DATA6:                                     /* Display Start GC No. */
        _itot(pData->CashierIf.usGstCheckStartNo, aszBuffer3, 10);       /* Convert GC Start No to ASCII by Base Number */
        break;

    case CLASS_PARACASHIERNO_DATA7:                                     /* Display End GC No. */
        _itot(pData->CashierIf.usGstCheckEndNo, aszBuffer3, 10);         /* Convert GC End No to ASCII by Base Number */
        break;

    case CLASS_PARACASHIERNO_DATA8:                                     /* Display Charge Tip Rate */
        _itot((UCHAR)pData->CashierIf.uchChgTipRate, aszBuffer3, 10);           /* Convert Charge Tip Rate to ASCII by Base Number */
        break;

    case CLASS_PARACASHIERNO_DATA9:                                     /* Display  Team Number */
        _itot((UCHAR)pData->CashierIf.uchTeamNo, aszBuffer3, 10);               /* Convert Team Number to ASCII by Base Number */
        break;

    case CLASS_PARACASHIERNO_DATA10:                                     /* Display Terminal */
        _itot((UCHAR)pData->CashierIf.uchTerminal, aszBuffer3, 10);             /* Convert Allowed Terminal Number ASCII by Base Number */
        break;
    }

    if(pData->uchMinorClass != CLASS_PARACASHIERNO_STS) {
        if (pData->uchMinorClass == CLASS_PARACASHIERNO_MNEMO) {
            _itot(pData->uchFieldAddr, aszBuffer1, 10);                  /* Keep Field Data of Entry Start GC No. */
            usGCNumber = pData->CashierIf.usGstCheckEndNo;
        }
    }

    if (pData->uchMinorClass == CLASS_PARACASHIERNO_MNEMO) {
        _tcsncpy(aszBuffer4, pData->CashierIf.auchCashierName, PARA_CASHIER_LEN);    /* Set Cashier Name */
    } else {
        _tcsncpy(aszBuffer4, pData->aszMnemonics, PARA_LEADTHRU_LEN); /* Set Lead Through Mnemonics */
    }

    if ((pData->uchMinorClass == CLASS_PARACASHIERNO_DATA3) || (pData->uchMinorClass == CLASS_PARACASHIERNO_DATA4)) {

        /* Display A/C 9 ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupCashierNo20C,          /* Format */
                  ( USHORT)AC_RANGE_GCNO,           /* Action Number */
                  aszBuffer2,                       /* Cashier Number */
                  aszDspNull,                       /* Address */
                  aszBuffer3,                       /* Start GC Number */
                  usGCNumber,                       /* Start/End GC Number */
                  aszBuffer4);                      /* Lead Through Mnemonics */

        /* Display A/C 9 BackGround */
        UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                          UIE_ATTR_NORMAL,          /* Attribute */
                          auchDispSupCashierNo20D,  /* Format */
                          ( USHORT)AC_RANGE_GCNO,   /* Action Number */
                          aszBuffer2,               /* Cashier Number */
                          aszDspNull,               /* Address */
                          aszBuffer3,               /* Start GC Number */
                          aszDspNull,               /* Null */
                          aszBuffer4);              /* Lead Through Mnemonics */    

    } else if ((pData->uchMinorClass == CLASS_PARACASHIERNO_DATA1) || (pData->uchMinorClass == CLASS_PARACASHIERNO_DATA2)) {

        /* Display A/C 20 ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupCashierNo20C,          /* Format */
                  ( USHORT)AC_CASH_ASSIN,           /* Action Number */
                  aszBuffer2,                       /* Cashier Number */
                  aszBuffer1,                       /* Address */
                  aszBuffer3,                       /* Start GC Number */
                  usGCNumber,                       /* Start/End GC Number */
                  aszBuffer4);                      /* Lead Through Mnemonics */

        /* Display A/C 20 BackGround */
        UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                          UIE_ATTR_NORMAL,          /* Attribute */
                          auchDispSupCashierNo20D,  /* Format */
                          ( USHORT)AC_CASH_ASSIN,    /* Action Number */
                          aszBuffer2,               /* Cashier Number */
                          aszBuffer1,               /* Address */
                          aszBuffer3,               /* Start GC Number */
                          aszDspNull,               /* Null */
                          aszBuffer4);              /* Lead Through Mnemonics */    
    } else {
        /* Display A/C 20 ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupCashierNo20A,          /* Format */
                  ( USHORT)AC_CASH_ASSIN,           /* Action Number */
                  aszBuffer2,                       /* Cashier Number */
                  aszBuffer1,                       /* Address */
                  aszBuffer3,                       /* Amount */
                  aszBuffer4);                      /* Lead Through Mnemonics */

        /* Display A/C 20 BackGround */
        UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                          UIE_ATTR_NORMAL,          /* Attribute */
                          auchDispSupCashierNo20B,  /* Format */
                          ( USHORT)AC_CASH_ASSIN,   /* Action Number */
                          aszBuffer2,               /* Cashier Number */
                          aszBuffer1,               /* Address */
                          aszDspNull,               /* Null */
                          aszBuffer4);              /* Lead Through Mnemonics */    
    }
}
