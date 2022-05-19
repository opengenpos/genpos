/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Enhanced EPT Error Conversion Routines
*   Category           : Enhanced EPT module,NCR2170 GP R2.0
*   Program Name       : EEPTERR.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*                   EEPTConvertError();

*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Feb-19-97 : 02.00.00 : T.Yatuhasi   : Initial
* Dec-07-99 : 01.00.00 : hrkato       : Saratoga
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include	<math.h>

#include    "ecr.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "eept.h"
#include    "appllog.h"

/*
*===========================================================================
** Synopsis:    USHORT     EEPTConvertError(SHORT sError);
*     Input:    sError
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   EEPTConvertError(SHORT sError)
{
    USHORT  usLeadthruNo;

	if (sError != EEPT_RET_SUCCESS) {
		PifLog (MODULE_EEPT, LOG_EVENT_STB_CONVERTERROR);
		PifLog (MODULE_EEPT, (USHORT)(abs(sError)));
	}

    switch ( sError ) {
    case EEPT_RET_SUCCESS:                      /* Success              */
        usLeadthruNo = 0;
        break;

    case EEPT_RET_LENGTH:                       /* receive length error */
        usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
        break;

    case  EEPT_RET_OFFLINE:                     /* EEPT & PMS is offline */
    case  EEPT_RET_TIMEOUT:                     /* time out error       */
        usLeadthruNo = LDT_EPT_OFFLINE_ADR;     /* 81 * EPT offline     */
        break;

    case  EEPT_RET_FAIL:                        /* other communication  */
        usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
        break;

    case  EEPT_RET_BUSY:                        /* EPT is busy, AKA CPM_RET_BUSY   */
        usLeadthruNo = LDT_CPM_BUSY_ADR;        /* ?? * EPT busy        */
        break;

    case  EEPT_RET_PORT_NONE:                   /* PMS is not provide   */
        usLeadthruNo = LDT_PROHBT_ADR;          /* 10 * Prohibit Operation */
        break;

    default:
        usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
        break;
    }

    return (usLeadthruNo);
}

/*====== End of Source ======*/