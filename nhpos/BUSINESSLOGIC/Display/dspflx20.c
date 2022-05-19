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
* Title       : Flexible Memory Assignment Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPFLX20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupFlexMem20()  : Display Flexible Memory Assignment
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-25-92: 00.00.01 : K.You     : initial                                   
*          :          :           :                                    
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
#include "display.h"

#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupFlexMem20( PARAFLEXMEM *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAFLEXMEM
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function displays Flexible Memory Assignment Data
*===========================================================================
*/

VOID DispSupFlexMem20( PARAFLEXMEM *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupFlexMem20A[] = _T("%3u  %3u\t%6lu\n\t%s");
    static const TCHAR  auchDispSupFlexMem20B[] = _T("%3u  %3u\t%10s\n\t%s");
    static const TCHAR  auchDispSupFlexMem20C[] = _T("%3u  %3u %6lu\t%1u\n\t%s");
    static const TCHAR  auchDispSupFlexMem20D[] = _T("%3u  %3u %6lu\t%5s\n\t%s");/* RFC 01/30/01 */
    /* static const UCHAR FARCONST auchDispSupFlexMem20D[] = "%3u  %3u %6lu\t%7s\n\t%s"; */

    TCHAR   aszDspNull[1] = {0};
    
    DispSupSaveData.FlexMem = *pData;      /* Save Data for Redisplay if needed. */

    /* Select Display Data */
    if (pData->uchMinorClass == CLASS_PARAFLEXMEM_DISPRECORD) {         /* Set Number of Record Case */
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupFlexMem20A,            /* Format */
                  PG_FLX_MEM,                       /* Program number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  pData->ulRecordNumber,            /* Number of Record */
                  aszDspNull);                      /* Null */

        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                 /* Descriptor Control */
                          UIE_ATTR_NORMAL,          /* Attribute */
                          auchDispSupFlexMem20B,    /* Format */
                          PG_FLX_MEM,               /* Program number */
                          ( USHORT)pData->uchAddress, /* Address */
                          aszDspNull,               /* Null */
                          aszDspNull);              /* Null */    
    } else {
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupFlexMem20C,            /* Format */
                  PG_FLX_MEM,                       /* Program number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  pData->ulRecordNumber,            /* Number of Record */
                  ( USHORT)(pData->uchPTDFlag),     /* PTD Flag Info. */
                  aszDspNull);                      /* Null */

        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                 /* Descriptor Control */
                          UIE_ATTR_NORMAL,          /* Attribute */
                          auchDispSupFlexMem20D,    /* Format */
                          PG_FLX_MEM,               /* Program number */
                          ( USHORT)pData->uchAddress, /* Address */
                          pData->ulRecordNumber,    /* Number of Record */
                          aszDspNull,               /* Null */
                          aszDspNull);              /* Null */    
    }

}
