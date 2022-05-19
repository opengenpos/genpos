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
* Title       : Thermal Print KDS IP Address Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSKDST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupKdsIp() : form KDS IP Address print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-29-99: 00.00.01 : M.Ozawa   : Initial                                   
* Dec-19-02:          :R.Chambers : SR 15 for Software Security
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
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupKdsIp( PARAKDSIP *pData )
*               
*     Input:    *pData      : pointer to structure for PARAKDSIP
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms KDS IP Address print format.
*
*                : KDS IP ASSIGNMENT
*
*===========================================================================
*/

VOID  PrtThrmSupKdsIp( PARAKDSIP *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupKdsIpA[] = _T("%18u  /  %03u.%03u.%03u.%03u");       
    static const TCHAR FARCONST auchPrtThrmSupKdsIpB[] = _T("%18u  /  %4u");       

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupKdsIpA[] = _T(" %2u / %03u.%03u.%03u.%03u");       
    static const TCHAR FARCONST auchPrtSupKdsIpB[] = _T(" %2u / %4u");       

    /* check print control */

    if (pData->uchMajorClass == CLASS_MAINTHOSTSIP) {
            
        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        
            PrtPrintf(PMG_PRT_RECEIPT,							/* printer type */
                      auchPrtThrmSupKdsIpA,						/* format */
                      ( USHORT)(pData->uchAddress),				/* address */
						pData->parakdsIPPort.uchIpAddress[0],	/* ip address block */
						pData->parakdsIPPort.uchIpAddress[1],	/* ip address block */
						pData->parakdsIPPort.uchIpAddress[2],	/* ip address block */
						pData->parakdsIPPort.uchIpAddress[3]);	/* ip address block */
      }

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
                  
            PrtPrintf(PMG_PRT_JOURNAL,							/* printer type */
                      auchPrtSupKdsIpA,							/* format */
                      ( USHORT)(pData->uchAddress),				/* address */
						pData->parakdsIPPort.uchIpAddress[0],	/* ip address block */
						pData->parakdsIPPort.uchIpAddress[1],	/* ip address block */
						pData->parakdsIPPort.uchIpAddress[2],	/* ip address block */
						pData->parakdsIPPort.uchIpAddress[3]);	/* ip address block */
        }
        
    } else {
    
        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

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

                PrtPrintf(PMG_PRT_RECEIPT,							/* printer type */
                          auchPrtThrmSupKdsIpA,						/* format */
                          ( USHORT)(pData->uchAddress),				/* address */
  							pData->parakdsIPPort.uchIpAddress[0],	/* ip address block */
							pData->parakdsIPPort.uchIpAddress[1],	/* ip address block */
							pData->parakdsIPPort.uchIpAddress[2],	/* ip address block */
							pData->parakdsIPPort.uchIpAddress[3]);  /* ip address block */
                break;
        
            case KDS_PORT1_ADR:
            case KDS_PORT2_ADR:
            case KDS_PORT3_ADR:
            case KDS_PORT4_ADR:
			case KDS_PORT5_ADR:
            case KDS_PORT6_ADR:
            case KDS_PORT7_ADR:
            case KDS_PORT8_ADR:

                PrtPrintf(PMG_PRT_RECEIPT,					/* printer type */
                          auchPrtThrmSupKdsIpB,             /* format */
                          ( USHORT)(pData->uchAddress),		/* address */
                          pData->parakdsIPPort.usPort);     /* port */
                break;

			case KDS_INT_PORT1:
			case KDS_INT_PORT2:
			case KDS_INT_PORT3:
			case KDS_INT_PORT4:
			case KDS_INT_PORT5:
			case KDS_INT_PORT6:
			case KDS_INT_PORT7:

                PrtPrintf(PMG_PRT_RECEIPT,						/* printer type */
                          auchPrtThrmSupKdsIpB,					/* format */
                          ( USHORT)(pData->uchAddress),			/* address */
                          pData->parakdsIPPort.usNHPOSPort);    /* interface port */

				break;
            
            default:
                break;
            }
        }
    
        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
    
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

                PrtPrintf(PMG_PRT_JOURNAL,								  /* printer type */
                          auchPrtSupKdsIpA,								  /* format */
                          ( USHORT)(pData->uchAddress),					  /* address */
						  pData->parakdsIPPort.uchIpAddress[0],           /* ip address block */
						  pData->parakdsIPPort.uchIpAddress[1],           /* ip address block */
						  pData->parakdsIPPort.uchIpAddress[2],           /* ip address block */
						  pData->parakdsIPPort.uchIpAddress[3]);          /* ip address block */

                break;
        
            case KDS_PORT1_ADR:
            case KDS_PORT2_ADR:
            case KDS_PORT3_ADR:
            case KDS_PORT4_ADR:
			case KDS_PORT5_ADR:
            case KDS_PORT6_ADR:
            case KDS_PORT7_ADR:
            case KDS_PORT8_ADR:
			
                PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                          auchPrtSupKdsIpB,					/* format */
                          ( USHORT)(pData->uchAddress),     /* address */
                          pData->parakdsIPPort.usPort);     /* port */
                break;

			case KDS_INT_PORT1:
			case KDS_INT_PORT2:
			case KDS_INT_PORT3:
			case KDS_INT_PORT4:
			case KDS_INT_PORT5:
			case KDS_INT_PORT6:
			case KDS_INT_PORT7:

                PrtPrintf(PMG_PRT_JOURNAL,						/* printer type */
                          auchPrtSupKdsIpB,						/* format */
                          ( USHORT)(pData->uchAddress),			/* address */
                          pData->parakdsIPPort.usNHPOSPort);    /* interface port */
				break;

            default:
                break;
            }
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupUnlockNo( PARAUNLOCKNO *pData )
*               
*     Input:    *pData      : pointer to structure for PARAUNLOCKNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Unlock code print format.
*
*                : Software Security
*
*===========================================================================
*/

VOID  PrtThrmSupUnlockNo( PARAUNLOCKNO *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupUnlockNo[] = _T("%15u  /  %24s\t%s");       
    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
    
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupUnlockNo,              /* format */
                  ( USHORT)(pData->uchAddress),     /* address */
                  pData->aszUnlockNo,
				  pData->aszMnemonic);          /* ip address block */
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
              
        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtThrmSupUnlockNo,              /* format */
                  ( USHORT)(pData->uchAddress),     /* address */
                  pData->aszUnlockNo,
				  pData->aszMnemonic);          /* ip address block */
    }
}

/***** End of Source *****/

