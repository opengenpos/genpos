/*
*===========================================================================
* Title       : Open Function
* Category    : PC Interface for STUB DLL, NCR 2170 PC Interface
* Program Name: PCIFOPEN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Large Model
* Options     : /c /Alnw /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Sep-07-92:00.00.01:O.Nakada   : Initial
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include "ecr.h"
#include "r20_pif.h"
#include "pifctl.h"
#include "pcif.h"
#include "pcifl.h"


/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/


/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
extern USHORT          husCliExeNet;       /* Semapho, ISP - APPLI */


/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT STUBENTRY PcifOpenEx(USHORT usFunc, VOID FAR *pData)
*     Input:    pData - Serial Port Information
*
** Return:      PCIF_SUCCESS - Successful
*
** Description:
*===========================================================================
*/
SHORT STUBENTRY PcifOpenEx(USHORT usFunc, VOID FAR *pData)
{
    SHORT   sStatus;

    PifRequestSem(husCliExeNet);

    if (usFunc == PCIF_FUNC_OPEN_LAN || usFunc == PCIF_FUNC_OPEN_LAN_PORT)
	{
        USHORT usPortNo = 0;

		if (usFunc == PCIF_FUNC_OPEN_LAN_PORT)
		{
			usPortNo = (USHORT)pData;
		}

        if ((sStatus = PifControl(PIF_NET_START_LAN_PORT, usPortNo)) >= 0) {    /* successful */
            sStatus = PCIF_SUCCESS;
        }
		else
		{
			switch (sStatus)
			{
				case PIF_ERROR_NET_CRITICAL:
					sStatus = PCIF_ERROR;
					break;
				case PIF_ERROR_NET_MEMORY:
					sStatus = PCIF_NET_MEMORY_ERR;
					break;
				case PIF_ERROR_NET_BAD_DATA:
					sStatus = PCIF_NET_SETUP_PARMS;
					break;
				case PIF_ERROR_NET_NOT_PROVIDED:
					sStatus = PCIF_NET_NO_NETORK;
					break;
				case PIF_ERROR_NET_ERRORS:
					sStatus = PCIF_NET_SOCK_ERR;
					break;
				case PIF_ERROR_NET_BIND_ERR:
					sStatus = PCIF_NET_BIND_ERR;
					break;
				default:
					sStatus = PCIF_ERROR;
					break;
			}
		}
    }
	else
	{
        sStatus = PCIF_ERROR;
    }

    PifReleaseSem(husCliExeNet);
    return sStatus;    

    pData = pData;
}
