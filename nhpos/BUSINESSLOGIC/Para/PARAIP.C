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
* Title       : Parameter ParaKdsIp Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAIP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaLeadThruRead()  : reads LEADTHROUGH of appointed address
*               ParaLeadThruWrite() : sets LEADTHROUGH in appointed address
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
* 2171 for Win32
* Aug-26-99:  01.00.00 : K.Iwata  : V1.0 Initial
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
;=============================================================================
**/
#include	<tchar.h>
#include<string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>
#include <log.h>
#include <appllog.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaKdsIpRead( PARAKDSIP *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_LEADTHRU_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads LEADTHROUGH of appointed address.
;===============================================================================
**/
VOID ParaKdsIpRead( PARAKDSIP *pData )
{

    int    i = 0;
	
	switch(pData->uchAddress)
	{
		case KDS_IP1_ADR:
		case KDS_PORT1_ADR:
		case KDS_INT_PORT1:

				i = 0;
			break;

		case KDS_IP2_ADR:
		case KDS_PORT2_ADR:
		case KDS_INT_PORT2:

				i = 1;
			break;

		case KDS_IP3_ADR:
		case KDS_PORT3_ADR:
		case KDS_INT_PORT3:

				i=2;
			break;

		case KDS_IP4_ADR:
		case KDS_PORT4_ADR:
		case KDS_INT_PORT4:

				i=3;
			break;

		case KDS_IP5_ADR:
		case KDS_PORT5_ADR:
		case KDS_INT_PORT5:

				i=4;
			break;

		case KDS_IP6_ADR:
		case KDS_PORT6_ADR:
		case KDS_INT_PORT6:

				i=5;
			break;

		case KDS_IP7_ADR:
		case KDS_PORT7_ADR:
		case KDS_INT_PORT7:

				i=6;
			break;

		case KDS_IP8_ADR:
		case KDS_PORT8_ADR:
		case KDS_INT_PORT8:

				i=7;
			break;

		default:
				i = 0;
				PifLog (MODULE_KPS, LOG_EVENT_KPS_KDS_READ_PARMADDR);
			break;
	}
	
	pData->parakdsIPPort = Para_KdsIp[i];
}                    

/**
;=============================================================================
;    Synopsis:  VOID ParaKdsIpWrite( PARAKDSIP *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_LEADTHRU_LEN+1]
;      output:  Nothing
;      
;      Return:  Nothing
;
; Descruption:  This function sets LEADTHROUGH in appointed address.
;===============================================================================
**/

VOID ParaKdsIpWrite( PARAKDSIP *pData )
{

    int    i = 0;

	switch((UCHAR)(pData->uchAddress))
	{
		case KDS_IP1_ADR:
		case KDS_PORT1_ADR:
		case KDS_INT_PORT1:

				i = 0;
			break;

		case KDS_IP2_ADR:
		case KDS_PORT2_ADR:
		case KDS_INT_PORT2:

				i = 1;
			break;

		case KDS_IP3_ADR:
		case KDS_PORT3_ADR:
		case KDS_INT_PORT3:

				i=2;
			break;

		case KDS_IP4_ADR:
		case KDS_PORT4_ADR:
		case KDS_INT_PORT4:

				i=3;
			break;

		case KDS_IP5_ADR:
		case KDS_PORT5_ADR:
		case KDS_INT_PORT5:

				i=4;
			break;

		case KDS_IP6_ADR:
		case KDS_PORT6_ADR:
		case KDS_INT_PORT6:

				i=5;
			break;

		case KDS_IP7_ADR:
		case KDS_PORT7_ADR:
		case KDS_INT_PORT7:

				i=6;
			break;

		case KDS_IP8_ADR:
		case KDS_PORT8_ADR:
		case KDS_INT_PORT8:

				i=7;
			break;

		default:
				i = 0;
				PifLog (MODULE_KPS, LOG_EVENT_KPS_KDS_WRITE_PARMADDR);
			break;
	}

	Para_KdsIp[i] = pData->parakdsIPPort; /* SR 14 ESMITH */
}

