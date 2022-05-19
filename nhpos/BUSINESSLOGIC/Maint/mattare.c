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
* Title       : MaintTare Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATTARE.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TARE.
*                                  
*           The provided function names are as follows: 
* 
*              MaintTareRead()   : read & display TARE
*              MaintTareWrite()  : set & print TARE
*              MaintTareReport() : print all TARE REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-24-92: 01.00.00 : J.Ikeda   : initial                                   
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
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h" 
#include "para.h"
#include "rfl.h" 
#include "maint.h" 
#include "display.h" 
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintTareRead( PARATARE *pData )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    *pData          : pointer to structure for PARATARE
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data 
*
** Description: This function reads & displays TARE.
*===========================================================================
*/

SHORT MaintTareRead( PARATARE *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.Tare.uchAddress;

        /* check address */
        if (uchAddress > TARE_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || TARE_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.Tare.uchMajorClass = pData->uchMajorClass;        /* copy major class */
    MaintWork.Tare.uchAddress = uchAddress;

    CliParaRead(&(MaintWork.Tare));                             /* call ParaTareRead() */

    /* display read data */
    /* check MDC & set minor class for display */
    if (!(CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0))) {      /* minimum unit is 0.01 */
        MaintWork.Tare.uchMinorClass = CLASS_PARATARE_001;
    } else {                                                    /* minimum unit is 0.001 or 0.005 */
        MaintWork.Tare.uchMinorClass = CLASS_PARATARE_0001;
    }

    /* set lead through */
	RflGetLead(MaintWork.Tare.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.Tare));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTareWrite( PARATARE *pData )
*               
*     Input:    *pData             : pointer to structure for PARATARE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints TARE.
*===========================================================================
*/

SHORT MaintTareWrite( PARATARE *pData )
{
    /* check MDC */
    if (!(CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0))) {      /* minimum unit is 0.01 */
        /* check range */
        if (pData->usWeight > MAINT_TARE001_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
    } else {                                                    /* minimum unit is 0.001 or 0.005 */
        /* check range */
        if (pData->usWeight > MAINT_TARE0001_MAX) {             
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
    
        /* check if minimum unit is 0.005 or 0.001 */
        if (!(CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT2))) {  /* minimum unit is 0.005 */

            /* check remainder */

            if (pData->usWeight % 5) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
        }
    }
    MaintWork.Tare.usWeight = pData->usWeight;

    /* major class, address has already set */
    CliParaWrite(&(MaintWork.Tare));                            /* call ParaTareWrite() */

    /* control header item */
    MaintHeaderCtl(AC_TARE_TABLE, RPT_ACT_ADR);

    /* set special mnemonics */
	RflGetSpecMnem(MaintWork.Tare.aszMnemonics, SPC_LB_ADR);

    /* set journal bit & receipt bit */
    MaintWork.Tare.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* major class, minor class, address has already set */

    PrtPrintItem(NULL, &(MaintWork.Tare));
    
    /* set address for display next address */
    /* check address */
    MaintWork.Tare.uchAddress++;

    if (MaintWork.Tare.uchAddress > TARE_ADR_MAX) {
        MaintWork.Tare.uchAddress = 1;                    /* initialize address */
    }

    MaintTareRead(&(MaintWork.Tare));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTareReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all TARE REPORT.
*===========================================================================
*/

VOID MaintTareReport( VOID )
{
    UCHAR         i;
	PARATARE      ParaTare = { 0 };

    /* control header item */
    MaintHeaderCtl(AC_TARE_TABLE, RPT_ACT_ADR);

    /* set major class */
    ParaTare.uchMajorClass = CLASS_PARATARE;

    /* check MDC & set minor class */
    if (!(CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0))) {      /* minimum unit is 0.01 */
        ParaTare.uchMinorClass = CLASS_PARATARE_001;
    } else {                                                    /* minimum unit is 0.001 or 0.005 */
        ParaTare.uchMinorClass = CLASS_PARATARE_0001;
    }

    /* set special mnemonics */
	RflGetSpecMnem(ParaTare.aszMnemonics, SPC_LB_ADR);

    /* set journal bit & receipt bit */
    ParaTare.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= TARE_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaTare.uchAddress = i;
        CliParaRead(&ParaTare);                                 /* call ParaTareRead() */
        PrtPrintItem(NULL, &ParaTare);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}
