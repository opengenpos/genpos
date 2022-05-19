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
* Title       : MaintSharedPrt Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATSHR.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               MaintSharedPrtRead()     : read & display TERMINAL NO. 
*               MaintSharedPrtWrite()    : set & print TERMINAL NO.  
*               MaintSharedPrtReport()   : print all TERMINAL NO.   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-23-93: 01.00.12   : J.IKEDA   : initial                                   
*          :            :                                    
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
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>

#include "maintram.h" 



/*
*===========================================================================================
** Synopsis:    SHORT MaintSharedPrtRead( PARASHAREDPRT *pData )
*               
*     Input:    *pData          : pointer to structure for PARASHAREDPRT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays TERMINAL NO.with SHARED/ALTERNATIVE PRINTER.
*===========================================================================================
*/

SHORT MaintSharedPrtRead( PARASHAREDPRT *pData )
{

    UCHAR uchAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.SharedPrt.uchAddress;

        /* check address */

        if (uchAddress > SHR_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < 1 || SHR_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.SharedPrt.uchMajorClass = pData->uchMajorClass;   /* copy major class */
    MaintWork.SharedPrt.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.SharedPrt));                        /* call ParaSharedPrtRead() */
    DispWrite(&(MaintWork.SharedPrt));
    return(SUCCESS);
}

/*
*===========================================================================================
** Synopsis:    SHORT MaintSharedPrtWrite( PARASHAREDPRT *pData )
*               
*     Input:    *pData          : pointer to structure for PARASHAREDPRT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_SEQERR_ADR   : error
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function sets & prints TERMINAL NO.with SHARED/ALTERNATIVE PRINTER.
*===========================================================================================
*/

SHORT MaintSharedPrtWrite( PARASHAREDPRT *pData )
{

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /* check range */

    if (STD_NUM_OF_TERMINALS < pData->uchTermNo) {
        return(LDT_KEYOVER_ADR);    /* wrong data */
    }

    MaintWork.SharedPrt.uchTermNo = pData->uchTermNo;

	if (MaintWork.SharedPrt.uchAddress >= SHR_COM_KP1_DEF_ADR && MaintWork.SharedPrt.uchAddress <= SHR_COM_KP8_DEF_ADR) {
		// comm port, address 41 thru 48, should be value of 0 thru 6.
		if (pData->uchTermNo > 6)       // should be PIF_LEN_PORT?
			return(LDT_KEYOVER_ADR);    /* wrong data */
	}

    /* control header item */
        
    MaintHeaderCtl(PG_SHR_PRT, RPT_PRG_ADR);

    /* set parameter */


    CliParaWrite(&(MaintWork.SharedPrt));       /* call ParaSharedPrtWrite() */

    /* set journal bit & receipt bit */

    MaintWork.SharedPrt.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    PrtPrintItem(NULL, &(MaintWork.SharedPrt));

    /* set address for display next address */

    /* check address */

    MaintWork.SharedPrt.uchAddress++;

    if (MaintWork.SharedPrt.uchAddress > SHR_ADR_MAX) {
        MaintWork.SharedPrt.uchAddress = SHR_TERM1_DEF_ADR; /* initialize address */
    }
    MaintSharedPrtRead(&(MaintWork.SharedPrt));
    return(SUCCESS);
}

/*
*===========================================================================================
** Synopsis:    VOID MaintSharedPrtReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all TERMINAL NO.with SHARED/ALTERNATIVE PRINTER.
*===========================================================================================
*/

VOID MaintSharedPrtReport( VOID )
{

    UCHAR           i;
    PARASHAREDPRT   ParaSharedPrt;


    /* control header item */
        
    MaintHeaderCtl(PG_SHR_PRT, RPT_PRG_ADR);

    ParaSharedPrt.uchMajorClass = CLASS_PARASHRPRT;
    ParaSharedPrt.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );


    /* set data at every address */

    for (i = 1; i <= SHR_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaSharedPrt.uchAddress = i;
        CliParaRead(&ParaSharedPrt);                            /* call ParaSharedPrtRead() */
        PrtPrintItem(NULL, &ParaSharedPrt);
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}

/*====== End of Source ======*/
