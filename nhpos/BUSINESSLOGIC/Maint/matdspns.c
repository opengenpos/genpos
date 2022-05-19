/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintDispensPara Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATDSPNS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*           MaintDispParaRead()   : read & display dispenser parameter
*           MaintDispParaEdit()   : check input data from UI & set self-work area
*           MaintDispParaWrite()  : set dispenser parameter
*           MaintDispParaReport() : report dispenser parameter
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Feb-02-96: 03.01.00 : M.Ozawa   : initial                                   
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
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <rfl.h> 
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbopr.h>
#include <csstbpar.h>

#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintDispParaRead( PARADISPPARA *pData )
*               
*     Input:    *pData         : pointer to structure for PARADISPPARA
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays the dispenser parameter
*===========================================================================
*/

SHORT MaintDispParaRead( PARADISPPARA *pData )
{
    USHORT  usAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */
        usAddress = ++MaintWork.DispPara.usAddress;
        /* check address */
        if (usAddress > DISPENS_ADR_MAX) {
            usAddress = 1;                                 /* initialize address */
        }
    } else {                                                /* with data */
        usAddress = pData->usAddress;
    }

    /* check address */
    if (usAddress < 1 || DISPENS_ADR_MAX < usAddress) {
        return(LDT_KEYOVER_ADR);
    }
    MaintWork.DispPara.uchMajorClass = pData->uchMajorClass; /* copy major class */
    MaintWork.DispPara.usAddress = usAddress;
    CliParaRead(&(MaintWork.DispPara));

	RflGetLead(MaintWork.DispPara.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.DispPara));

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintDispParaEdit( PARADISPPARA *pData )
*               
*     Input:    *pData          : pointer to structure for PARADISPPARA
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : adjective do not exist
*               MAINT_ADJECTIVE_EXIST : adjective exist
*               LDT_SEQERR_ADR  : sequence error
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function checks input data from UI & sets self-work area
*===========================================================================
*/

SHORT MaintDispParaEdit( PARADISPPARA *pData )
{


    /* check minor class */

    switch(pData->uchMinorClass) {
    case CLASS_PARADISPPARA_PLUNO:

        /* check status */

        if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */   
            return(LDT_SEQERR_ADR);                             /* sequence error */
        }

        /* distinguish address */

        if (MaintWork.DispPara.usAddress <= DISPENS_PLU256_ADR) {

            /* check PLU No. */

            if (pData->usPluNumber > MAX_PLU_NO) { 
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
        } else if (MaintWork.DispPara.usAddress == DISPENS_OFFSET_ADR) {

            /* check offset code */

            if (pData->usPluNumber > MAX_DISPPARA_OFFSET) { 
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }

        } else {

            /* check total key code */

            if (pData->usPluNumber > MAX_DISPPARA_TOTAL) { 
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
        }

        MaintWork.DispPara.usPluNumber = pData->usPluNumber;

        /* check address */

        if (MaintWork.DispPara.usAddress >= DISPENS_OFFSET_ADR) {
            return(SUCCESS);                                /* adjective do not exist */
        }

        MaintWork.DispPara.uchAdjective = 0;

        DispWrite(&(MaintWork.DispPara));

        return(MAINT_ADJECTIVE_EXIST);

    case CLASS_PARADISPPARA_ADJECTIVE:

        /* check status */

        if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */   
            ;
        } else {

            /* check Adjective range */

            if (pData->uchAdjective > FSC_ADJ_MAX) {
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            MaintWork.DispPara.uchAdjective = pData->uchAdjective;
        }

        return(SUCCESS);

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);                    /* wrong data */
    }
}

/*
*===========================================================================
** Synopsis:    VOID MaintDispParaWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets the dispenser parameter
*===========================================================================
*/

VOID MaintDispParaWrite( VOID )
{

    /* set data */
    CliParaWrite(&(MaintWork.DispPara));

    /* control header item */
    MaintHeaderCtl(AC_DISPENSER_PARA, RPT_ACT_ADR);

    /* print data */
    MaintWork.DispPara.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.DispPara));

    /* set address for display next address */
    MaintWork.DispPara.usAddress++;

    /* check address */
    if (MaintWork.DispPara.usAddress > DISPENS_ADR_MAX) {
        MaintWork.DispPara.usAddress = 1;       /* initialize address */
    }
    MaintDispParaRead(&(MaintWork.DispPara));      

}

/*
*==============================================================================
** Synopsis:    VOID MaintDispParaReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports the dispenser parameter
*==============================================================================
*/

VOID MaintDispParaReport( VOID )
{

    USHORT        i;
    PARADISPPARA  ParaDispPara;

    /* control header item */

    MaintHeaderCtl(AC_DISPENSER_PARA, RPT_ACT_ADR);

    /* set major class */

    ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;

    /* print data */

    ParaDispPara.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = 1; i <= DISPENS_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {              /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        /* get data & print */

        ParaDispPara.usAddress = i;
        CliParaRead(&ParaDispPara);
        PrtPrintItem(NULL, &ParaDispPara);
    }
    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

/****** end of source ******/
