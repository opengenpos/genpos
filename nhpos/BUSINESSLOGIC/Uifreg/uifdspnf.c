/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Beverage Dispenser Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFDSPNF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*          UifDispenserFirst() : Beverage Dispenser First Entry Sales Module
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Mar-01-96:03.01.00:   M.Ozawa : initial                                   
*          :        :           : 
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
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegDispFirst[] = {FSC_BEVERAGE, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT   UifDispenserFirst( KEYMSG *pData )
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Beverage Dispenser First Entry Sales Module
*===========================================================================
*/
SHORT   UifDispenserFirst( KEYMSG *pData )
{
    USHORT  usPLUNo;
    UCHAR   uchAdjective;
    SHORT   sRetStatus;
    TCHAR   aszPLUNo[NUM_PLU_SCAN_LEN];

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence( aszSeqRegDispFirst );                       /* open dispenser key sequence */
        ItemOtherDifEnableStatus(0);        /* R3.1 for Beverage Dispenser */
        break;                                  
                                                            
    case UIM_INPUT:
        if ( pData->SEL.INPUT.uchMajor == FSC_BEVERAGE ) {         /* beverage dispenser ? */
            switch(ItemOtherCheckDispPara((VOID *)pData->SEL.INPUT.DEV.auchOther,
                                            &usPLUNo, &uchAdjective)) {
            case CLASS_ITEMSALES:

                _itot(usPLUNo, aszPLUNo, 10);
                UifRegData.regsales.uchMajorClass = CLASS_UIFREGSALES;  /* set salea major class */
                UifRegData.regsales.uchMinorClass = CLASS_UIDISPENSER;  /* set plu minor class */
                _tcsncpy(UifRegData.regsales.aszPLUNo, aszPLUNo, _tcslen(aszPLUNo));   /* set PLU# */
                //memcpy(UifRegData.regsales.aszPLUNo, aszPLUNo, strlen(aszPLUNo));   /* set PLU# */
                /* UifRegData.regsales.usPLUNo = usPLUNo;              / set PLU# */
                UifRegData.regsales.uchAdjective = uchAdjective;    /* set adjectove # */
                UifRegData.regsales.lQTY = 1000L;                   /* assume as 1 QTY */
                UifRegData.regsales.fbModifier &= ~VOID_MODIFIER;   /* reset item void bit */
                UifRegData.regsales.fbModifier2 |= VELO_MODIFIER;   /* no c/d option */
                _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);  /* set number saratoga */
                //memcpy(UifRegData.regsales.aszNumber, aszUifRegNumber, sizeof(aszUifRegNumber));  /* set number saratoga */
                /* memcpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);  / set number */
                sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */
                break;

            default:
                sRetStatus = UIF_CANCEL;
            }

            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            ItemOtherDifClearBuffer();                  /* clear saved buffer for retry */
            UieAccept();                                /* send accepted to parent */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
