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
*              MaintKdsIpRead()   : read & display KDS IP DATA
*              MaintKdsIpWrite()  : set & print KDS IP DATA
*              MaintKdsIpReport() : print all KDS IP REPORT  
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
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintKdsIpRead( PARAKDSIP *pData )
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
SHORT MaintKdsIpRead( PARAKDSIP *pData )
{

    UCHAR uchAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.KdsIp.uchAddress;

        /* check address */

        if (uchAddress > KDS_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 

    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < 1 || KDS_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }

    MaintWork.KdsIp.uchMajorClass = pData->uchMajorClass; /* copy major class */
    MaintWork.KdsIp.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.KdsIp));                      /* call ParaKdsIpRead() */
	DispWrite(&(MaintWork.KdsIp));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintKdsIpWrite( PARAKDSIP *pData )
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

SHORT MaintKdsIpWrite( PARAKDSIP *pData )
{

    TCHAR aszIPData[MAINT_KDS_IP_LEN+1];
    USHORT i;
    
    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    memset(aszIPData, 0, sizeof(aszIPData));

    /*  check address */
	//ESMITH SR14
    switch (MaintWork.KdsIp.uchAddress) {
    case KDS_IP1_ADR:
    case KDS_IP2_ADR:
    case KDS_IP3_ADR:
    case KDS_IP4_ADR:
    case KDS_IP5_ADR:
    case KDS_IP6_ADR:
    case KDS_IP7_ADR:
    case KDS_IP8_ADR:
    
        /* check range */
        
        //SR 14 ESMITH
		//if (pData->ausIPAddress[0] == MAINT_KDS_IP_MAX) {
		if (pData->parakdsIPPort.uchIpAddress[0] == MAINT_KDS_IP_MAX) {
            for (i=0; i<MAINT_KDS_IP_NUM; i++) {
                _tcsncpy(aszIPData, &pData->aszInputData[i*MAINT_KDS_IP_LEN], MAINT_KDS_IP_LEN);
				//memcpy(aszIPData, &pData->aszInputData[i*MAINT_KDS_IP_LEN], MAINT_KDS_IP_LEN);

                //MaintWork.KdsIp.ausIPAddress[i] = _ttoi(aszIPData);
				MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i] = _ttoi(aszIPData);

                //if (MaintWork.KdsIp.ausIPAddress[i] > 255) {
				if (MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i] > 255) {
                    return(LDT_KEYOVER_ADR);                            /* wrong data */ 
                }
            }
        } else
        //if (pData->ausIPAddress[0] == 1) {
		if (pData->parakdsIPPort.uchIpAddress[0] == 1) {
            
            if (pData->aszInputData[0] == _T('0')) {
            
                /* 0 clear */
                for (i=0; i<MAINT_KDS_IP_NUM; i++) {
                    //MaintWork.KdsIp.ausIPAddress[i] = 0;
					MaintWork.KdsIp.parakdsIPPort.uchIpAddress[i] = 0;
                }
            } else {
                
                return(LDT_KEYOVER_ADR);                            /* wrong data */ 
            }
        } else {

            return(LDT_KEYOVER_ADR);                            /* wrong data */ 
        }
        
        break;

    case KDS_PORT1_ADR:
    case KDS_PORT2_ADR:
    case KDS_PORT3_ADR:
    case KDS_PORT4_ADR:
	case KDS_PORT5_ADR:
    case KDS_PORT6_ADR:
    case KDS_PORT7_ADR:
    case KDS_PORT8_ADR:

		//****************************
		// SR 14 ESMITH

		//MaintWork.KdsIp.uchAddress = (MaintWork.KdsIp.uchAddress/2);
		//MaintKdsIpRead(&(MaintWork.KdsIp));

        /* check range */
        
        //if (pData->ausIPAddress[0] > MAINT_KDS_PORT_MAX) {
		pData->parakdsIPPort.usPort = pData->parakdsIPPort.uchIpAddress[0];
		if (pData->parakdsIPPort.usPort > MAINT_KDS_PORT_MAX) {

            return(LDT_KEYOVER_ADR);                            /* wrong data */ 
        }
        
		_tcsncpy(aszIPData, &pData->aszInputData[0], MAINT_KDS_PORT_MAX);
        //memcpy(aszIPData, &pData->aszInputData[0], MAINT_KDS_PORT_MAX);
        
		MaintWork.KdsIp.parakdsIPPort.usPort = _ttoi(aszIPData);
		//MaintWork.KdsIp.ausIPAddress[0] = _ttoi(aszIPData);
        break;


    case KDS_INT_PORT1:
    case KDS_INT_PORT2:
    case KDS_INT_PORT3:
    case KDS_INT_PORT4:
	case KDS_INT_PORT5:
    case KDS_INT_PORT6:
    case KDS_INT_PORT7:
    case KDS_INT_PORT8:

		//****************************
		// SR 14 ESMITH

        /* check range */
        
        //if (pData->ausIPAddress[0] > MAINT_KDS_PORT_MAX) {
		pData->parakdsIPPort.usNHPOSPort = pData->parakdsIPPort.uchIpAddress[0];
		if (pData->parakdsIPPort.usNHPOSPort > MAINT_KDS_PORT_MAX) {

            return(LDT_KEYOVER_ADR);                            /* wrong data */ 
        }
        
		_tcsncpy(aszIPData, &pData->aszInputData[0], MAINT_KDS_PORT_MAX);
        
		MaintWork.KdsIp.parakdsIPPort.usNHPOSPort = _ttoi(aszIPData);
        break;

    default:                        /* SLIP_NEARFULL_ADR: */

        return(LDT_KEYOVER_ADR);                            /* wrong data */
        break;

    }
    CliParaWrite(&(MaintWork.KdsIp));                     /* call ParaSlipFeedCtlWrite() */

    /* control header item */
        
    MaintHeaderCtl(PG_KDS_IP, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.KdsIp.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.KdsIp));
    
    /* set address for display next address */

    /* check address */

	MaintWork.KdsIp.uchAddress++;

    if (MaintWork.KdsIp.uchAddress > KDS_ADR_MAX) {
        MaintWork.KdsIp.uchAddress = 1;                   /* initialize address */
    }
    MaintKdsIpRead(&(MaintWork.KdsIp));
    return(SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID MaintKdsIpReport( VOID )
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

VOID MaintKdsIpReport( VOID )
{


    UCHAR                i;
    PARAKDSIP      ParaKdsIp;


    /* control header item */
        
    MaintHeaderCtl(PG_KDS_IP, RPT_PRG_ADR);
    ParaKdsIp.uchMajorClass = CLASS_PARAKDSIP;

    /* set journal bit & receipt bit */

    ParaKdsIp.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= KDS_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaKdsIp.uchAddress = i;
        CliParaRead(&ParaKdsIp);                          /* call ParaKdsIpRead() */
        PrtPrintItem(NULL, &ParaKdsIp);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

/****** End of Source ******/    

