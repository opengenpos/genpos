/*
*===========================================================================
* Title       : Modem Dis-Connect Function
* Category    : PC Interface for STUB DLL, NCR 2170 PC Interface
* Program Name: PCIFCON.C
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
* Jul-01-93:01.00.00:O.Nakada   : Add hung-up AT command.
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
#include <string.h>
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
** Synopsis:    SHORT STUBENTRY PcifDisconnect(VOID)
*
** Return:      PCIF_SUCCESS - Successful
*
** Description:
*===========================================================================
*/
SHORT STUBENTRY PcifDisconnect(VOID)
{
    static UCHAR auchPcifEscape[] = "+++";
    static UCHAR auchPcifAt[]     = "AT";
    static UCHAR auchPcifHangup[] = "H0\r\n";
    PCIF_CONNECT Connect;
    SHORT   sStatus;

    PifRequestSem(husCliExeNet);

    /* ----- AT command ----- */
    PifSleep(PCIF_AT_GUARD_TIME);           /* guard time */

    Connect.pData    = auchPcifEscape;      /* escape command */
    Connect.usLength = (USHORT)strlen((CHAR *)auchPcifEscape);
    PifControl(PIF_ASY_WRITE, &Connect);

    PifSleep(PCIF_AT_GUARD_TIME);           /* guard time */

    Connect.pData    = auchPcifAt;          /* hang-up command */
    Connect.usLength = (USHORT)strlen((CHAR *)auchPcifAt);
    PifControl(PIF_ASY_WRITE, &Connect);
    PifSleep(PCIF_AT_DELAY);
    Connect.pData    = auchPcifHangup;
    Connect.usLength = (USHORT)strlen((CHAR *)auchPcifHangup);
    PifControl(PIF_ASY_WRITE, &Connect);

    /* ----- DTR control ----- */
    if (PifControl(PIF_ASY_DTR_OFF, NULL) == 1) {   /* successful */
        sStatus = PCIF_RESULT_NO_CARRIER;
    } else {                                        /* error */
        sStatus = PCIF_RESULT_ERROR;
    }

    PifReleaseSem(husCliExeNet);
    return sStatus;    
}



/*===== End of Source File ========*/
