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
* Title       : MaintPCIF Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPCIF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write password.
*
*           The provided function names are as follows: 
* 
*              MaintPCIFRead()       : read & display PC password
*              MaintPCIFWrite()      : read & display PC password
*              MaintPCIFReport()     : report & print PC password report 
*              MaintPCIFGetCommand() : get Command for Modem
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Nov-30-92 : 00.00.01 : J.Ikeda   : initial                                   
* Dec-16-92 : 01.00.00 : J.Ikeda   : repair Address range ( '0' is not available data )
* Aug-24-93 : 01.00.13 : J.IKeda   : Add Address 2 Function
*
** NCR2171 **
* Aug-26-99 : 01.00.00 : M.Teraki  : initial (for 2171)
* Aug-05-21 : 02.04.01 : R.Chamber : made MaintPCIFGetCommand() static
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
#include <pif.h>
#include <rfl.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

static VOID  MaintPCIFGetCommand(WCHAR *paszCommand);

/*
*===========================================================================
** Synopsis:    SHORT MaintPCIFRead( PARAPCIF *pData )
*               
*     Input:    *pData          : ponter to structure for PARAPCIF
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays PC password.
*===========================================================================
*/

SHORT MaintPCIFRead( PARAPCIF *pData )
{

    UCHAR  uchAddress;

    /* initialize */

/*    memset(&MaintWork.PCIF.aszMnemonics, '\0', PARA_PCIF_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.PCIF.uchAddress;

        /* check address */

        if (uchAddress > PCIF_ADR_MAX) {                      
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || PCIF_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.PCIF.uchMajorClass = pData->uchMajorClass;        /* copy major class */
    MaintWork.PCIF.uchAddress = uchAddress;

    /* check address */

    if (MaintWork.PCIF.uchAddress == PCIF_PASSWORD_ADR) {
        CliParaRead(&(MaintWork.PCIF));                             /* call ParaPCIFRead() */ 
    } else {    /* PCIF_COMMAND_ADR */
        MaintPCIFGetCommand(MaintWork.PCIF.aszMnemonics);
    }
    DispWrite(&(MaintWork.PCIF));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPCIFWrite( PARAPCIF *pData )
*               
*     Input:    *pData         : pointer to structure for PARAPCIF
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays PC I/F.
*===========================================================================
*/

SHORT MaintPCIFWrite( PARAPCIF *pData )
{

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /* distinguish address */
    switch(MaintWork.PCIF.uchAddress) {
    case  PCIF_PASSWORD_ADR:

        /* check string length */
        if (PARA_PASSWORD_LEN < _tcslen(pData->aszMnemonics)) {
            return(LDT_KEYOVER_ADR);
        }
        _tcsncpy(MaintWork.PCIF.aszMnemonics, pData->aszMnemonics, PARA_PCIF_LEN);
        CliParaWrite(&(MaintWork.PCIF));                            /* call ParaPCIFWrite() */
        break;

    case  PCIF_COMMAND_ADR: 
		{
			TCHAR aszBuffer[PARA_PCIF_LEN + 2 + 1] = { 0 };  // room needed for \r \n as well as end of string terminator.

			_tcsncpy(aszBuffer, pData->aszMnemonics, PARA_PCIF_LEN);
			_tcscat(aszBuffer, _T("\r\n"));
			PifSetModemStrings(aszBuffer);  /* saratoga */
			_tcsncpy(PifSysConfig()->pModemStrings, aszBuffer, PARA_PCIF_LEN+1); 
			_tcsncpy(MaintWork.PCIF.aszMnemonics, pData->aszMnemonics, PARA_PCIF_LEN);
		}
        break;

    default:
        break;                 
    }

    /* control header item */
    MaintHeaderCtl(PG_PCIF, RPT_PRG_ADR);

    MaintWork.PCIF.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.PCIF));

    /* set address for Display next address */
    MaintWork.PCIF.uchAddress++;

    /* check address */
    if (MaintWork.PCIF.uchAddress > PCIF_ADR_MAX) {
        MaintWork.PCIF.uchAddress = 1;                          /* initialize address */
    }
    MaintPCIFRead(&(MaintWork.PCIF));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPCIFReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints PC I/F report.
*===========================================================================
*/

VOID MaintPCIFReport( VOID )
{
	PARAPCIF  ParaPCIF = { 0 };

    /* control header */
    MaintHeaderCtl(PG_PCIF, RPT_PRG_ADR);

    /* set major class */
    /* set journal bit & receipt bit */
    ParaPCIF.uchMajorClass = CLASS_PARAPCIF;
    ParaPCIF.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (UCHAR i = 1; i <= PCIF_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaPCIF.uchAddress = i;

        if (i == PCIF_PASSWORD_ADR) {
            CliParaRead(&ParaPCIF);                                 /* call ParaPCIFRead() */
        } else {
            MaintPCIFGetCommand(ParaPCIF.aszMnemonics);
        }
        PrtPrintItem(NULL, &ParaPCIF);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}

/*
*===========================================================================
** Synopsis:    VOID MaintPCIFGetCommand(UCHAR *paszCommand)
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    aszCommand
*
** Return:      Nothing
*
** Description: This function gets Command for Modem from SysConfig.
*===========================================================================
*/

static VOID MaintPCIFGetCommand(TCHAR *paszCommand) 
{
	TCHAR  aszBuffer[PARA_PCIF_LEN + 1] = { 0 };
    USHORT usStrLen;

    memset(paszCommand, '\0', ((PARA_PCIF_LEN + 1) * sizeof(TCHAR)));
	_tcsncpy(aszBuffer, PifSysConfig()->pModemStrings, PARA_PCIF_LEN);

    /* check string length */
	// This will clear off the new line and return characters at the end
	// of the string, but only if the string length is greater than 2. ('\r\n')
    if ((usStrLen = tcharlen(aszBuffer)) > MAINT_CRLF_LEN) {
        _tcsncpy(paszCommand, aszBuffer, usStrLen - MAINT_CRLF_LEN);  
    }
}	
