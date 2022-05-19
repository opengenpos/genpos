/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintMiscPara Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US GP Application Program
* Program Name: MATMISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Misc. Parameter.
*
*           The provided function names are as follows:
*
*              MaintMiscRead()   : read & display MISC. PARAMETER
*              MaintMiscWrite()  : set & print MISC. PARAMETER
*              MaintMiscReport() : print all MISC. PARAMETER
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Mar-08-93: 00.00.01 : M.Ozawa   : initial
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
#include "maint.h" 
#include "rfl.h" 
#include "display.h" 
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
#include "maintram.h"

/*
*===========================================================================
** Synopsis:    SHORT MaintMiscRead( PARAMISCPARA *pData )
*
*     Input:    *pData          : pointer to structure for PARAMISCPARA
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Misc. Parameter.
*===========================================================================
*/

SHORT MaintMiscRead( PARAMISCPARA *pData )
{
    UCHAR           uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.MiscPara.uchAddress;

        /* check address */
        if (uchAddress > MISC_ADR_MAX) {
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || MISC_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.MiscPara.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.MiscPara.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.MiscPara));                          /* call ParaMiscRead() */

	RflGetLead(MaintWork.MiscPara.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.MiscPara));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintMiscWrite( PARAMISCPARA *pData )
*
*     Input:    *pData             : pointer to structure for PARAMISCPARA
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints Misc. Parameter.
*===========================================================================
*/

SHORT MaintMiscWrite( PARAMISCPARA *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  check range */
    switch(MaintWork.MiscPara.uchAddress){
    case MISC_FOODDENOMI_ADR:                                   /* food stamp denomination */
        if (pData->ulMiscPara > MAINT_FOODDENOMI_MAX) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;

    case MISC_FOODLIMIT_ADR:                                    /* food stamp purchase limit */
    case MISC_TRANSCREDIT_ADR:                                  /* trans. credit limit */
    case MISC_CPNFACE2_ADR:                                     /* Max. Amt.of CPN(face)value to double (R2.0) */
    case MISC_CPNFACE3_ADR:                                     /* Max. Amt.of CPN(face)value to triple (R2.0) */
    case MISC_MAXCPNAMT_ADR:                                    /* Max. Credit Amt. allowed/transaction (R2.0) */
    case MISC_CASHLIMIT_ADR:									/* Cash Drawer Limit                    (R2.0) */
	case MISC_SF_SINGLE_ADR:                                    /* Store and Forward single transaction limit */
    case MISC_SF_CUMMUL_ADR:                                    /* Store and Forward cummulative transaction Limit */
        if (pData->ulMiscPara > 9999999) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
        break;
    }

    MaintWork.MiscPara.ulMiscPara = pData->ulMiscPara;
    CliParaWrite(&(MaintWork.MiscPara));                         /* call ParaMiscWrite() */

    /* control header item */
    MaintHeaderCtl(AC_MISC_PARA, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.MiscPara.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.MiscPara));

    MaintWork.MiscPara.uchAddress++;

    if (MaintWork.MiscPara.uchAddress > MISC_ADR_MAX) {
        MaintWork.MiscPara.uchAddress = 1;                       /* initialize address */
    }
    MaintMiscRead(&(MaintWork.MiscPara));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintMiscReport( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all Misc. Parameter.
*===========================================================================
*/
VOID MaintMiscReport( VOID )
{
    UCHAR            i;
	PARAMISCPARA     ParaMiscPara = { 0 };

    /* control header item */
    MaintHeaderCtl(AC_MISC_PARA, RPT_ACT_ADR);
    ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;

    /* set journal bit & receipt bit */
    ParaMiscPara.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= MISC_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaMiscPara.uchAddress = i;
        CliParaRead(&ParaMiscPara);                             /* call ParaMiscRead() */
        PrtPrintItem(NULL, &ParaMiscPara);
    }
    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

/* --- End of Source File --- */