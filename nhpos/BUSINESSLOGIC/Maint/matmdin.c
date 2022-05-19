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
* Title       : MaintModeIn Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATMDIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls mode-in function.
*
*      The provided function names are as follows:  
*
*        SHORT MaintModeInPrg() : controls mode-in function at program mode
*        SHORT MaintModeInSup() : controls mode-in function at supervisor mode 
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-92: 00.00.01 : J.Ikeda   : initial                                   
* Dec-08-95: 03.01.00 : M.Ozawa   : Enhanced to MSR Sign-In
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
#include <ecr.h>
/* #include <uie.h> */
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <display.h>
#include <csstbpar.h>
#include "maintram.h"

/*
*===========================================================================
** Synopsis:    SHORT MaintModeInPrg( MAINTMODEIN *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: security number is exist
*               LDT_SEQERR_ADR: sequence error 
*               LDT_KEYOVER_ADR: wrong data
*
** Description: This function controls mode-in function at program mode.
*===========================================================================
*/                                         

SHORT MaintModeInPrg( MAINTMODEIN *pData )
{
    PARASECURITYNO  ParaSecurityNo;

    /* check if data is or not */
    if (!(pData->uchStatus & MAINT_WITHOUT_DATA)) {  /* with amount */
        ParaSecurityNo.uchMajorClass = CLASS_PARASECURITYNO;
        ParaSecurityNo.uchAddress = SEC_NO_ADR;
        CliParaRead(&ParaSecurityNo);

        MaintWork.MaintModeIn = *pData;    /* copy structure */

        /* check security number */
        if (( USHORT)(MaintWork.MaintModeIn.ulModeInNumber) == ParaSecurityNo.usSecurityNumber) {
            MaintIncreSpcCo(SPCO_PRG_ADR);          /* increase program counter */
            return(SUCCESS);
        } else {  
            return(LDT_KEYOVER_ADR);            /* wrong data */
        }
    } else { /* in case of without data */
        return(LDT_SEQERR_ADR);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintModeInSup( MAINTMODEIN *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: supervisor number is exist
*               LDT_SEQERR_ADR: sequence error
*               LDT_KEYOVER_ADR: wrong data
*
** Description: This function controls mode-in function at supervisor mode.
*===========================================================================
*/                                         

SHORT MaintModeInSup( MAINTMODEIN *pData )
{
    UCHAR           uchSecret;
	PARASUPLEVEL    ParaSupLevel;

    /* check if data is or not */
    if (!(pData->uchStatus & MAINT_WITHOUT_DATA)) {
        MaintWork.MaintModeIn = *pData;      /* copy structure */

        /* ---- MSR entry check ---- */
        if (pData->uchStatus & MAINT_MSR_DATA) {
            if (MaintWork.MaintModeIn.ulModeInNumber < 1000L) {
                MaintWork.MaintModeIn.ulModeInNumber *= 100L;   /* dummy secret no. */
            }
        }
        ParaSupLevel.uchMajorClass = CLASS_PARASUPLEVEL;
        ParaSupLevel.uchAddress = 0;
        ParaSupLevel.usSuperNumber = MAINT_SUP_GET_SUPER_NUMBER(MaintWork.MaintModeIn.ulModeInNumber);
    
        /* check if supervisor number is or not */
        if ((ParaSupLevel.usSuperNumber < MAINT_SUP_LEVEL_NUMBER_MIN) || (ParaSupLevel.usSuperNumber > MAINT_SUP_LEVEL_NUMBER_MAX)) {		//Changed min and max values from 801 and 899 to 101 and 999
			NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: LDT_INVALID_SUP_NUM - Supervisor Id is out of range.");
            return(LDT_INVALID_SUP_NUM);                          /* wrong data */
        }
        if (CliParaSupLevelRead(&ParaSupLevel) == SUCCESS) {  /* read data */
            /* check if secret code is or not */
            uchSecret = ( UCHAR)MAINT_SUP_GET_SECRET_NUMBER(MaintWork.MaintModeIn.ulModeInNumber);

            if (pData->uchStatus & MAINT_MSR_DATA) {    /* msr entry */
                if (uchSecret) {
                    /* check if secret number is exist */ 
                    if (ParaSupLevel.uchSecret != 0) {
                        /* check secret code */
                        if (ParaSupLevel.uchSecret != uchSecret) {
                            return(LDT_KEYOVER_ADR);     /* wrong data */
                        } 
                    } else { /* secret code is not set */
                        ParaSupLevel.uchSecret = uchSecret;  /* address has already set */
                        CliParaWrite(&ParaSupLevel);
                    }
                }
            } else {                                /* keyboard entry */
                /* check if secret code is '0' */
                if (uchSecret == 0) {
                    return(LDT_KEYOVER_ADR);
                }

                /* check if secret number is exist */ 
                if (ParaSupLevel.uchSecret != 0) {
                    /* check secret code */
                    if (ParaSupLevel.uchSecret != uchSecret) {
                        return(LDT_INVALID_SUP_NUM);     /* wrong data */
                    } 
                } else { /* secret code is not set */
                    ParaSupLevel.uchSecret = uchSecret;  /* address has already set */
                    CliParaWrite(&ParaSupLevel);
                }
            }

            usMaintSupNumberInternal = ParaSupLevel.usSuperNumber;

            /* set supervisor level */
    	    ParaSupLevel.uchAddress = SPNO_LEV0_ADR;
			ParaSupLevel.usSuperNumber = 0;	 /* initialize supervisor number */                    

    	    /* read supervisor level 0 */
			CliParaSupLevelRead(&ParaSupLevel);

            if (usMaintSupNumberInternal <= ParaSupLevel.usSuperNumber) {
                usMaintSupLevel = 0;
                return(SUCCESS);
            }

    	    /* read supervisor level 1 */
			ParaSupLevel.uchAddress = SPNO_LEV1_ADR;
			ParaSupLevel.usSuperNumber = 0;  /*	initialize supervisor number */

			/* read supervisor level 1 */
			CliParaSupLevelRead(&ParaSupLevel);

            if (usMaintSupNumberInternal <= ParaSupLevel.usSuperNumber) {
                usMaintSupLevel = 1;
            } else { 
                usMaintSupLevel = 2;
            }
        } else {
            return(LDT_INVALID_SUP_NUM);             /* wrong data */
        }
    } else { /* without data */
        /* check if supervisor no. entry is required or not */
        if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT0)) {    /* supervisor no. entry is not required */
            usMaintSupNumberInternal = 800;
            usMaintSupLevel = 0;
        } else { /* supervisor no. entry is required */
            return(LDT_SEQERR_ADR);
        }
    }

    return(SUCCESS);
}
