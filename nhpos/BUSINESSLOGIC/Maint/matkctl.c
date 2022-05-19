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
* Title       : Total Key Control Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATKCTL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                          
*              MaintTtlKeyCtlRead()     : Read and Display Total Key Control
*              MaintTtlKeyCtlWrite()    : Set and Print Total Key Control
*              MaintTtlKeyCtlReport()   : Print All Total Key Control   
*                                             
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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
#include <ecr.h>
#include <uie.h>
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
/* #include <appllog.h> */

#include "maintram.h" 

/**
==============================================================================
;    Static Area Declaration                          
=============================================================================
**/

UCHAR uchMaintByteTtlKeyCtl;                /* Total Key Control Bit Save Area */

/*
*===========================================================================
** Synopsis:    SHORT MaintTtlKeyCtlRead( PARATTLKEYCTL *pData )
*               
*     Input:    *pData          : pointer to structure for PARATTLKEYCTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : Successful
*               LDT_KEYOVER_ADR  : Wrong Data
*
** Description: This Function Reads and Displays Total Key Control.
*===========================================================================
*/
SHORT MaintTtlKeyCtlRead( PARATTLKEYCTL *pData )
{

    UCHAR uchAddress;
    UCHAR uchField;


    /* Distingush Minor Class */

    switch (pData->uchMinorClass) {
    case CLASS_PARATTLKEYCTL_ALL:
        uchAddress = pData->uchAddress;
        uchField = pData->uchField;
        break;

    case CLASS_PARATTLKEYCTL_ADR:
        uchField = 1;                                               /* Initialize Field Address */

        /* Check W/ Amount */

        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
            uchAddress = ++MaintWork.TtlKeyCtl.uchAddress;
     
            /* Check Address */

            if (uchAddress > TLCT_ADR_MAX) {                        /* Over Limit */
                uchAddress = 1;                                     /* Initialize Address */
            }
        } else {                                                    /* With data */
            uchAddress = pData->uchAddress;
        }
        break;

    case CLASS_PARATTLKEYCTL_FLD:
        uchAddress = MaintWork.TtlKeyCtl.uchAddress;

        /* Check W/ Amount */

        if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
            uchField = ++MaintWork.TtlKeyCtl.uchField;

            /* Check Field */

            if (uchField > TLCT_FLD_MAX) {                         /* Over Limit */
                uchField = 1;                                      /* Initialize Address */
                uchAddress = ++MaintWork.TtlKeyCtl.uchAddress;

                /* Check Address */

                if (uchAddress > TLCT_ADR_MAX) {                    /* Over Limit */
                    uchAddress = 1;                                 /* Initialize Address */
                }
            }
        } else {                                                    /* W/ Amount */
            uchField = pData->uchField;
        }
        break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);                                
    }

    /* Check Address and Field */

    if (uchAddress < 1 || TLCT_ADR_MAX < uchAddress) {          /* Out of Range */
        return(LDT_KEYOVER_ADR);                                
    }
    if (uchField < 1 || TLCT_FLD_MAX < uchField) {              /* Out of Range */
        return(LDT_KEYOVER_ADR);                                
    }

    /* Execute Read and Display Function */

    MaintWork.TtlKeyCtl.uchMajorClass = pData->uchMajorClass;   
    MaintWork.TtlKeyCtl.uchAddress = uchAddress;
    MaintWork.TtlKeyCtl.uchField = uchField;
    CliParaRead(&(MaintWork.TtlKeyCtl));

    /* Save Total Key Control Data to Own Save Area */

    uchMaintByteTtlKeyCtl = MaintWork.TtlKeyCtl.uchTtlKeyMDCData;

    /* Check Odd/Even Number */

    if (!(MaintWork.TtlKeyCtl.uchField & 0x0001)) {             /* Even Case */
        MaintWork.TtlKeyCtl.uchTtlKeyMDCData >>= 4;
    } else {                                                    /* Odd Case */
        MaintWork.TtlKeyCtl.uchTtlKeyMDCData &= 0x0F;
    }
    DispWrite(&(MaintWork.TtlKeyCtl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTtlKeyCtlWrite( PARATTLKEYCTL *pData )
*               
*     Input:    *pData             : pointer to structure for PARATTLKEYCTL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : Successful
*               LDT_SEQERR_ADR     : Sequense Error
*               LDT_KEYOVER_ADR    : Wrong Data
*
** Description: This function Sets & Prints Total Key Control.
*===========================================================================
*/

SHORT MaintTtlKeyCtlWrite( PARATTLKEYCTL *pData )
{

    UCHAR   uchTtlBitData;


    /* Check W/ Amount */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                            /* W/o Amount */
        return(LDT_SEQERR_ADR);                                 
    }

    /* Check Total Key Control Data */

    if (MaintChkBinary(pData->aszTtlKeyMDCString, 4) == LDT_KEYOVER_ADR) {  /* Wrong Data */
        return(LDT_KEYOVER_ADR);                            
    }

    /* Convert String Data into Bit Pattern */

    uchTtlBitData = MaintAtoStatus(pData->aszTtlKeyMDCString);

    /* Set Total Key Control Data */

    if (!(MaintWork.TtlKeyCtl.uchField & 0x0001)) {                         /* Even Field */
        uchMaintByteTtlKeyCtl &= 0x0F;
        uchMaintByteTtlKeyCtl |= uchTtlBitData << 4;
    } else {                                                                /* Odd Field */
        uchMaintByteTtlKeyCtl &= 0xF0;
        uchMaintByteTtlKeyCtl |= uchTtlBitData;
    }
    MaintWork.TtlKeyCtl.uchTtlKeyMDCData = uchMaintByteTtlKeyCtl;
    CliParaWrite(&(MaintWork.TtlKeyCtl));                         

    /* Control Header Item */
        
    MaintHeaderCtl(PG_TTLKEY_CTL, RPT_PRG_ADR);

    /* Set Journal Bit & Receipt Bit */

    MaintWork.TtlKeyCtl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    MaintWork.TtlKeyCtl.uchTtlKeyMDCData = uchTtlBitData;
    PrtPrintItem(NULL, &(MaintWork.TtlKeyCtl));
    
    /* Set Address for Display Next Address */

    if ((++MaintWork.TtlKeyCtl.uchField) > TLCT_FLD_MAX) {          /* Over Limit */
        MaintWork.TtlKeyCtl.uchField = 1;                           /* Initialize Field */

        if ((++MaintWork.TtlKeyCtl.uchAddress) > TLCT_ADR_MAX) {    /* Over Limit */
            MaintWork.TtlKeyCtl.uchAddress = 1;                     /* Initialize Address */
        }
    }

    MaintWork.TtlKeyCtl.uchMinorClass = CLASS_PARATTLKEYCTL_FLD;   
    MaintTtlKeyCtlRead(&(MaintWork.TtlKeyCtl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTtlKeyCtlReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This Function Prints All Total Key Control.
*===========================================================================
*/

VOID MaintTtlKeyCtlReport( VOID )
{

    UCHAR           i;
    UCHAR           j;     
    PARATTLKEYCTL   ParaTtlKeyCtl;


    /* Control Header Item */
        
    MaintHeaderCtl(PG_TTLKEY_CTL, RPT_PRG_ADR);

    ParaTtlKeyCtl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );   
    ParaTtlKeyCtl.uchMajorClass = CLASS_PARATTLKEYCTL;

    /* Print Data for Every Address */

    for (i = 1; i <= TLCT_ADR_MAX; i++) {

        /* Check Abort Key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* Depress Abort Key */
            MaintMakeAbortKey();
            break;
        }
        ParaTtlKeyCtl.uchAddress = i;

        for (j = 1; j <= TLCT_FLD_MAX; j++) {
            ParaTtlKeyCtl.uchField = j;

            /* Print Odd Field Data */

            CliParaRead(&(ParaTtlKeyCtl));                           
            ParaTtlKeyCtl.uchTtlKeyMDCData &= 0x0F;
            PrtPrintItem(NULL, &ParaTtlKeyCtl);

            /* Print Even Field Data */

            ParaTtlKeyCtl.uchField = ++j;                           
            CliParaRead(&(ParaTtlKeyCtl));                           
            ParaTtlKeyCtl.uchTtlKeyMDCData &= 0xF0;
            ParaTtlKeyCtl.uchTtlKeyMDCData >>= 4;
            PrtPrintItem(NULL, &ParaTtlKeyCtl);
        }
    }

    /* Make Trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

    
    
    
    
    
