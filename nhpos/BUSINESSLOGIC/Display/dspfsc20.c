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
*==========================================================================
* Title       : FSC for Regular KB( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPFSC20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupFSC20()  : Display FSC
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-12-92: 00.00.01 : K.You     : initial                                   
* Nov-28-95: 03.01.00 : M.Ozawa   : display plu no at plu key entry
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

#include "ecr.h"
#include "uie.h"
/* #include <pif.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "fsc.h"
#include "rfl.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupFSC20( PARAFSC *pData )
*               
*     Input:    *Data           : Pointer to Structure for PARAFSC
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function displays FSC data
*===========================================================================
*/

VOID DispSupFSC20( PARAFSC *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupFSC20A[] = _T("%3u  %2u  %3u\t%2u-%4u\n\t%s");
    static const TCHAR  auchDispSupFSC20B[] = _T("%3u  %2u  %3u\t%6s\n\t%s");

    static const TCHAR  auchDispSupFSC20C[] = _T("%3u  %2u  %3u\t%2u-\n\t%c %s");

    TCHAR  aszDspNull[1] = {0};
	TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1] = { 0 };

    DispSupSaveData.FSC = *pData;     /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    if (pData->uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
		TCHAR  uchEversion = _T(' ');
        /* 13 digit PLU No. */

        RflConvertPLU(aszPLUNo, pData->PluNo.aszPLUNumber);

        if (pData->PluNo.uchModStat & MOD_STAT_EVERSION) {
            uchEversion = _T('E');
        } else  {
            uchEversion = _T(' ');
        }

        UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupFSC20C,                    /* Format */
              PG_FSC,                               /* Super/Program number */
              ( USHORT)pData->uchPageNo,            /* Page */
              ( USHORT)pData->uchAddress,           /* Address */
              ( USHORT)pData->uchMajorFSCData,      /* Major FSC Data */
              uchEversion,
              aszPLUNo);                       /* Minor FSC Data */

        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupFSC20B,            /* Format */
                      PG_FSC,                       /* Super/Program number */
                      ( USHORT)pData->uchPageNo,    /* Page */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      aszDspNull);                  /* Null */

    } else {
		USHORT usMinorFSCData = pData->uchMinorFSCData;

        /* saratoga */
        if (pData->uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_DEPT) {
            usMinorFSCData = pData->usDeptNo;
        } else {
            usMinorFSCData = (USHORT)pData->uchMinorFSCData;
        }


        UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupFSC20A,                    /* Format */
              PG_FSC,                               /* Super/Program number */
              ( USHORT)pData->uchPageNo,            /* Page */
              ( USHORT)pData->uchAddress,           /* Address */
              ( USHORT)pData->uchMajorFSCData,      /* Major FSC Data */
              usMinorFSCData,                       /* Minor FSC Data */
              aszDspNull);                          /* Null */    

        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupFSC20B,            /* Format */
                      PG_FSC,                       /* Super/Program number */
                      ( USHORT)pData->uchPageNo,    /* Page */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      aszDspNull);                  /* Null */    
    }
}
