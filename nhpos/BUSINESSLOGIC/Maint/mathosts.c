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
* Title       : MaintSlipFeddCtl Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATKDSIP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report KDS IP DATA.
*                                  
*           The provided function names are as follows: 
* 
*              MaintHostsIpRead()   : read & display KDS IP DATA
*              MaintHostsIpWrite()  : set & print KDS IP DATA
*              MaintHostsIpReport() : print all KDS IP REPORT  
*              MaintHostsIpReboot() : 
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-29-99: 00.00.01 : M.Ozawa   : initial                                   
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
#include <stdlib.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include <rfl.h>
#include <csttl.h>
#include <ttl.h>
#include "maintram.h" 

static TCHAR   uchMaintHostName[PIF_LEN_HOST_NAME + 1];

/*
*===========================================================================
** Synopsis:    SHORT MaintHostsIpRead( PARAKDSIP *pData )
*               
*     Input:    *pData          : pointer to structure for PARAKDSIP
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays KDS IP DATA.
*===========================================================================
*/
SHORT MaintHostsIpRead( PARAKDSIP *pData )
{

    USHORT     len, i;
	TCHAR      uchHosts[PIF_LEN_HOST_NAME + 1] = { 0 };
	UCHAR      uchIPAddress[PIF_LEN_IP+1];
    UCHAR      uchAddress = 1;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.KdsIp.uchAddress;

        /* check address */

        if (uchAddress > PIF_NET_MAX_IP) { 
            uchAddress = 1;                                     /* initialize address */
        } 

    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < 1 || PIF_NET_MAX_IP < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.KdsIp.uchMajorClass = pData->uchMajorClass; /* copy major class */
    MaintWork.KdsIp.uchAddress = uchAddress;

	// Get this terminal's host name.  The host name should be in the standard format which contains a
	// text name followed by a dash and then a number.  For example "NCRPOS-1".  We find where the
	// dash is as we are going to replace the number with the address of the P99 entry (1 - 16) which
	// the user is currently modifying.  This implies that all terminals in a cluster should have the
	// same terminal text name before the dash.
    PifGetLocalHostName(uchMaintHostName);

    len = tcharlen(uchMaintHostName);
    for (i=0; i< len; i++) {
        if (uchMaintHostName[i] == _T('-')) {
            uchMaintHostName[i+1] = '\0';
            break;
        }
    }

#if defined(_DEBUG)
#pragma message("  WARNING:  Overriding terminal name adding dash if terminal name not in NCRPOS-n format.")
    if (i==len) {

        /*
			not set as "NCRPOS-n" format.  So we will force it by adding - to end of name.
			Down below we then add the P99 address to the end using RflSPrintf so that we will
			then be able to allow things to flow through as usual.
		*/
        uchMaintHostName[i] = _T('-');
        uchMaintHostName[i+1] = '\0';
    }
#else
    if (i==len)
        return(LDT_EQUIPPROVERROR_ADR);     // Terminal name is incorrect format.  Setup/Provisioning error
#endif

	// modify the terminal text name by adding the P99 address to the end of it after the dash.
	// we will be using this for our registry key to obtain any terminal's IP address later.
    RflSPrintf(uchHosts, TCHARSIZEOF(uchHosts), _T("%s%d"), &uchMaintHostName[0], uchAddress);

    PifGetHostsAddress(uchHosts, uchIPAddress);
    
    for (i=0; i<PIF_LEN_IP; i++) {        
		// SR 14 ESMITH change in PARAKDSIP struct to handle more KDS devices
		//              since terminal host settings with P99 use same struct
		//              modifications here as well.
		MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i] = uchIPAddress[i];
    }

    DispWrite(&(MaintWork.KdsIp));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintHostsIpWrite( PARAKDSIP *pData )
*               
*     Input:    *pData             : pointer to structure for PARASLIPFEEDCTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints SLIP FEED CONTROL.
*===========================================================================
*/

SHORT MaintHostsIpWrite( PARAKDSIP *pData )
{

	TCHAR aszIPData[MAINT_KDS_IP_LEN + 1] = { 0 };
    USHORT i;
    USHORT fDelete = FALSE;
    TCHAR uchHosts[PIF_LEN_HOST_NAME + 1] = { 0 };
    UCHAR uchIPAddress[PIF_LEN_IP+1];
	PARATERMINALINFO TermInfo = { 0 };
    
    /* check status */
    if (uchMaintHostName[0] == 0) {
        return(LDT_PROHBT_ADR);                            /* wrong data */ 
    }

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

	/*** TAR178231
    if (pData->uchAddress > PIF_NET_MAX_IP) {

        return(LDT_KEYOVER_ADR);                            / wrong data / 
    } ***/

    /* check range */
    
	//  We now check to see if the number of character digits provided by the user
	//  is equal to the number of digits which we expect.  The caller is required to put
	//  the number of digits entered by the user into parakdsIPPort.uchIpAddress[0].
	//  If the user has entered the correct number of total digits, we convert the
	//  IP address entered into the IP octet values.  We take groups of 3 digits to
	//  convert for each octet as in the first group of 3 digits goes into the first octet, etc.
	//  We also do a data check to ensure that the 3 digits are within the legal range.
	if (pData->parakdsIPPort.uchIpAddress[0] == MAINT_KDS_IP_MAX) {
		USHORT  usTempIP;

        for (i=0; i<PIF_LEN_IP; i++) {
            _tcsncpy(aszIPData, &pData->aszInputData[i*MAINT_KDS_IP_LEN], MAINT_KDS_IP_LEN);
            //MaintWork.KdsIp.ausIPAddress[i] = atoi(aszIPData);
			usTempIP = _ttoi(aszIPData);
			if (usTempIP > 255) {
                return(LDT_KEYOVER_ADR);                            /* wrong data */
			}
			MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i] = (UCHAR) (usTempIP & 0xff);
        }
    } else if (pData->parakdsIPPort.uchIpAddress[0] == 1) {
        
        if (pData->aszInputData[0] == _T('0')) {
            
            /* If the user entered just a singe digit 0, we will clear the current IP address */
            for (i=0; i<PIF_LEN_IP; i++) {
				MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i] = 0;
                fDelete = TRUE;
            }
        } else {
            
            return(LDT_KEYOVER_ADR);                            /* wrong data */ 
        }
    } else {
        
         return(LDT_KEYOVER_ADR);                            /* wrong data */ 
    }
        
    
    RflSPrintf(uchHosts, TCHARSIZEOF(uchHosts), _T("%s%d"), &uchMaintHostName[0], MaintWork.KdsIp.uchAddress);
    for (i=0; i<PIF_LEN_IP; i++) {
		uchIPAddress[i] = MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i];
    }
    
    if (fDelete) {
        
        PifRemoveHostsAddress(uchHosts);
        
    } else {
        
        PifSetHostsAddress(uchHosts, uchIPAddress);
    }
    
    /* control header item */
        
    MaintHeaderCtl(PG_HOSTS_IP, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.KdsIp.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.KdsIp));
    
    /* set address for display next address */

    /* check address */

    MaintWork.KdsIp.uchAddress++;

    if (MaintWork.KdsIp.uchAddress > PIF_NET_MAX_IP) {
        MaintWork.KdsIp.uchAddress = 1;                   /* initialize address */
    }
    MaintHostsIpRead(&(MaintWork.KdsIp));

	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = (UCHAR)(MaintWork.KdsIp.uchAddress-1);
	ParaTerminalInfoParaRead (&TermInfo);

	// setting TermInfo IP fields - CSMALL 11/16/05
	for(i=0; i<PIF_LEN_IP; i++) {
		USHORT usTempIP;

        _tcsncpy(aszIPData, &pData->aszInputData[i*MAINT_KDS_IP_LEN], MAINT_KDS_IP_LEN);
		usTempIP = _ttoi(aszIPData);

		if (usTempIP > 255) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
		}

		TermInfo.TerminalInfo.ucIpAddress[i] = (UCHAR)usTempIP;
	}

	ParaWrite(&TermInfo);

    return(SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID MaintHostsIpReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all KDS IP REPORT.
*===========================================================================
*/

VOID MaintHostsIpReport( VOID )
{
    UCHAR          i, j;
    PARAKDSIP      ParaKdsIp;
    TCHAR          uchHosts[PIF_LEN_HOST_NAME + 1] = { 0 };
    UCHAR          uchIPAddress[PIF_LEN_IP+1];

    /* control header item */
        
    MaintHeaderCtl(PG_HOSTS_IP, RPT_PRG_ADR);
    ParaKdsIp.uchMajorClass = CLASS_MAINTHOSTSIP;

    /* set journal bit & receipt bit */

    ParaKdsIp.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= PIF_NET_MAX_IP; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaKdsIp.uchAddress = i;
        RflSPrintf(uchHosts, TCHARSIZEOF(uchHosts), _T("%s%d"), uchMaintHostName, i);

        PifGetHostsAddress(uchHosts, uchIPAddress);
    
        for (j=0; j<PIF_LEN_IP; j++) {
            //SR 14 ESMITH
			//ParaKdsIp.ausIPAddress[j] = (USHORT)uchIPAddress[j];
			ParaKdsIp.parakdsIPPort.uchIpAddress[j] = uchIPAddress[j];
        
        }
        PrtPrintItem(NULL, &ParaKdsIp);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

/*
*===========================================================================
** Synopsis:    VOID MaintHostsIpReboot( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: 
*===========================================================================
*/

VOID MaintHostsIpReboot( VOID )
{

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_POWERDOWN, DISP_SAVE_TYPE_0);

    TtlShutDown();   /* request shutdown to total update module */

    PifSleep(1000);

    PifRestart(TRUE);

}

/****** End of Source ******/    
