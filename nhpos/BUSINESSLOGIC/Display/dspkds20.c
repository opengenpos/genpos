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
* Title       : KDS Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPKDS20.C
*
* Also contains some of the software security display code for Release 1.4.
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupKdsIp20()       : Display KDS IP Address
*                                                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-29-99: 00.00.01 : M.Ozawa   : initial                                   
* Dec-19-02:          :R.Chambers : SR 15 for Software Security
* Jan-22-04:		  : E.Smith	  : SR 14
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
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h>
#include <para.h>
#include <maint.h>
/* #include <appllog.h> */
#include <display.h>
#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupKdsIp20( PARAKDSIP *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAKDSIP
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays KDS IP Address
*===========================================================================
*/

VOID DispSupKdsIp20( PARAKDSIP *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupKdsIp20A[] = _T("%2u %2u\t%03u.%03u.%03u.%03u\n\t%s");
    static const TCHAR  auchDispSupKdsIp20B[] = _T("%2u %2u\t%4u\n\t%s");
    static const TCHAR  auchDispSupKdsIp20C[] = _T("%2u %2u\t%15s\n\t%s");

    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.KdsIp = *pData;      /* Save Data for Redisplay if needed. */

    if (pData->uchMajorClass == CLASS_MAINTHOSTSIP) {
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupKdsIp20A,              /* Format */
                  PG_HOSTS_IP,                        /* Program Number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  pData->parakdsIPPort.uchIpAddress[0], // SR 14 ESMITH
				  pData->parakdsIPPort.uchIpAddress[1], // SR 14 ESMITH
				  pData->parakdsIPPort.uchIpAddress[2], // SR 14 ESMITH
				  pData->parakdsIPPort.uchIpAddress[3], // SR 14 ESMITH

				  //pData->ausIPAddress[0], /* Block 1 */
                  //pData->ausIPAddress[1], /* Block 2 */
                  //pData->ausIPAddress[2], /* Block 3 */
                  //pData->ausIPAddress[3], /* Block 4 */
                 aszDspNull);                      /* Null */
                 
        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                          UIE_ATTR_NORMAL,              /* Attribute */
                          auchDispSupKdsIp20C,         /* Format */
                          PG_HOSTS_IP,                    /* Program Number */
                          ( USHORT)pData->uchAddress,   /* Address */
                          aszDspNull,                   /* Null */
                          aszDspNull);                  /* Null */    
    
    } else {
        
		//ESMITH SR14
        switch (pData->uchAddress) {
        case KDS_IP1_ADR:
        case KDS_IP2_ADR:
        case KDS_IP3_ADR:
        case KDS_IP4_ADR:
        case KDS_IP5_ADR:
        case KDS_IP6_ADR:
        case KDS_IP7_ADR:
        case KDS_IP8_ADR:

            /* Display ForeGround */
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupKdsIp20A,              /* Format */
                      PG_KDS_IP,                        /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */                      
					  pData->parakdsIPPort.uchIpAddress[0], // SR 14 ESMITH
					  pData->parakdsIPPort.uchIpAddress[1], // SR 14 ESMITH
					  pData->parakdsIPPort.uchIpAddress[2], // SR 14 ESMITH
					  pData->parakdsIPPort.uchIpAddress[3], // SR 14 ESMITH

					  //pData->ausIPAddress[0], /* Block 1 */
                      //pData->ausIPAddress[1], /* Block 2 */
                      //pData->ausIPAddress[2], /* Block 3 */
                      //pData->ausIPAddress[3], /* Block 4 */
                     aszDspNull);                      /* Null */
            break;

        case KDS_PORT1_ADR:
        case KDS_PORT2_ADR:
        case KDS_PORT3_ADR:
        case KDS_PORT4_ADR:
		case KDS_PORT5_ADR:
        case KDS_PORT6_ADR:
        case KDS_PORT7_ADR:
        case KDS_PORT8_ADR:

            /* Display ForeGround */
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupKdsIp20B,              /* Format */
                      PG_KDS_IP,                        /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */
                      pData->parakdsIPPort.usPort,		/* port no *?
					  //pData->ausIPAddress[0], /* port no */
                     aszDspNull);                      /* Null */
            break;

        case KDS_INT_PORT1:
        case KDS_INT_PORT2:
        case KDS_INT_PORT3:
        case KDS_INT_PORT4:
		case KDS_INT_PORT5:
        case KDS_INT_PORT6:
        case KDS_INT_PORT7:
        case KDS_INT_PORT8:

            /* Display ForeGround */
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupKdsIp20B,              /* Format */
                      PG_KDS_IP,                        /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */
                      pData->parakdsIPPort.usNHPOSPort,		/* port no */
                     aszDspNull);                      /* Null */
            break;


        default:
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
            break;
        }


        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                          UIE_ATTR_NORMAL,              /* Attribute */
                          auchDispSupKdsIp20C,         /* Format */
                          PG_KDS_IP,                    /* Program Number */
                          ( USHORT)pData->uchAddress,   /* Address */
                          aszDspNull,                   /* Null */
                          aszDspNull);                  /* Null */    
    }
}


/*
*===========================================================================
** Synopsis:    VOID DispSupUnlockNo( PARAUNLOCKNO *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAUNLOCKNO
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays software security information
*===========================================================================
*/

VOID DispSupUnlockNo( PARAUNLOCKNO *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispProgUnlock[] = _T("%2u %2u\t%15s\n%-s\t%s");
    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.UnlockNo = *pData;      /* Save Data for Redisplay if needed */

        /* Display ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispProgUnlock,              /* Format */
                  PG_UNLOCK_NO,                        /* Program Number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  pData->aszUnlockNo,				// Terminal serial, software serial, or unlock number
				  pData->aszExpireVal,				// Expire Value (in days)
                  pData->aszMnemonic);                      /* szMnemonic */
                 
        /* Display BackGround */
        UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                          UIE_ATTR_NORMAL,              /* Attribute */
                          auchDispProgUnlock,         /* Format */
                          PG_UNLOCK_NO,                    /* Program Number */
                          ( USHORT)pData->uchAddress,   /* Address */
                          aszDspNull,                   /* Null */
                          aszDspNull,                   /* Null */
                          aszDspNull);                  /* Null */    
    
}
