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
* Title       : Print KDS IP Address Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSKDS_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupKdsIp() : form KDS IP Address Assignment
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-29-99: 00.00.01 : M.Ozawa   : initial                                   
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
#include <ecr.h>
#include <stdlib.h>
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupKdsIp( PARAKDSIP *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAKDSIP
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms KDS IP Address print format.
*===========================================================================
*/

VOID  PrtSupKdsIp( PARAKDSIP *pData )
{

    static const TCHAR FARCONST auchPrtSupKdsIpA[] = _T(" %2u / %03u.%03u.%03u.%03u");       
    static const TCHAR FARCONST auchPrtSupKdsIpB[] = _T(" %2u / %4u");       

    USHORT  usPrtType;


    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Print Line */

    if (pData->uchMajorClass == CLASS_MAINTHOSTSIP) {

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupKdsIpA,              /* format */
                  ( USHORT)(pData->uchAddress),     /* address */
                  pData->parakdsIPPort.uchIpAddress[0], /* ip address block */
				  pData->parakdsIPPort.uchIpAddress[1], /* ip address block */
				  pData->parakdsIPPort.uchIpAddress[2], /* ip address block */
				  pData->parakdsIPPort.uchIpAddress[3]); /* ip address block */
                  
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

            PmgPrintf(usPrtType,                            /* Printer Type */
                      auchPrtSupKdsIpA,              /* format */
                      ( USHORT)(pData->uchAddress),     /* address */
                      
					  pData->parakdsIPPort.usPort, /* ip address block */
					  pData->parakdsIPPort.usPort, /* ip address block */
					  pData->parakdsIPPort.usPort, /* ip address block */
					  pData->parakdsIPPort.usPort); /* ip address block */

            break;
        
        case KDS_PORT1_ADR:
        case KDS_PORT2_ADR:
        case KDS_PORT3_ADR:
        case KDS_PORT4_ADR:
		case KDS_PORT5_ADR:
        case KDS_PORT6_ADR:
        case KDS_PORT7_ADR:
        case KDS_PORT8_ADR:

            PmgPrintf(usPrtType,                        /* Printer Type */
                      auchPrtSupKdsIpB,					/* format */
                      ( USHORT)(pData->uchAddress),     /* address */
                      pData->parakdsIPPort.usPort);		/* port */
            break;

		case KDS_INT_PORT1:
		case KDS_INT_PORT2:
		case KDS_INT_PORT3:
		case KDS_INT_PORT4:
		case KDS_INT_PORT5:
		case KDS_INT_PORT6:
		case KDS_INT_PORT7:

            PmgPrintf(usPrtType,							/* Printer Type */
                      auchPrtSupKdsIpB,						/* format */
                      ( USHORT)(pData->uchAddress),			/* address */
                      pData->parakdsIPPort.usNHPOSPort);	/* interface port */
            break;
            
        default:
            break;
        }
    }

}

