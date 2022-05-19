/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintFlexMem Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATFLEX.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function assign the number of record & if each file has PTD or not. 
*
*           The provided function names are as follows: 
* 
*           MaintFlexMemRead()   : read & display the number of record & PTD flag status
*           MaintFlexMemEdit()   : check input data from UI & set self-work area
*           MaintFlexMemWrite()  : set the number of record and PTD status flag 
*           MaintFlexMemReport() : report the number of record and PTD status flag      
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-29-92: 00.00.01 : J.Ikeda   : initial                                   
* Jun-15-93: 01.00.12 : K.You     : Added EJ Feature.
* Jun-30-93: 01.00.12 : M.Yamamoto: Add ETK Feature.
* Apr-07-94: 00.00.04 : K.You     : Add Flex GC File Feature
*          :          :           : (Mod. MaintFlexMemEdit, MaintFlexMemWrite).
* Jan-12-95: 			 : hkato     : R3.0
* Aug-11-99: 03.05.00 : M.Teraki  : Remove WAITER_MODEL
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
#include <log.h>
#include <pif.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbbak.h>
#include <csstbpar.h>
#include <csstbstb.h>
#include <csop.h>
#include <cswai.h>
#include <csstbwai.h>
#include <cscas.h>
#include <csstbcas.h>
#include <csetk.h>
#include <csstbetk.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <appllog.h>
#include <ej.h>
#include <cskp.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintFlexMemRead( PARAFLEXMEM *pData )
*               
*     Input:    *pData         : pointer to structure for PARAFLEXMEM
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays the number of record & PTD flag status.
*===========================================================================
*/

SHORT MaintFlexMemRead( PARAFLEXMEM *pData )
{

    UCHAR  uchAddress;

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */
        uchAddress = ++MaintWork.FlexMem.uchAddress;

        /* check address */

        if (uchAddress > FLEX_ADR_MAX) {
            uchAddress = FLEX_DEPT_ADR;                     /* initialize address */
        }
    } else {                                                /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */

    if (uchAddress < FLEX_DEPT_ADR || FLEX_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);
    }
    MaintWork.FlexMem.uchMajorClass = pData->uchMajorClass; /* copy major class */
    MaintWork.FlexMem.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.FlexMem));                      /* call ParaFlexMemRead() */
    MaintWork.FlexMem.uchMinorClass = CLASS_PARAFLEXMEM_DISPRECORD;
    DispWrite(&(MaintWork.FlexMem));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintFlexMemEdit( PARAFLEXMEM *pData )
*               
*     Input:    *pData          : pointer to structure for PARAFLEXMEM
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : PTD do not exist
*               MAINT_PTD_EXIST : PTD exist
*               LDT_SEQERR_ADR  : sequence error
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function checks input data from UI & sets self-work area
*===========================================================================
*/

SHORT MaintFlexMemEdit( PARAFLEXMEM *pData )
{

    PARAFLEXMEM ParaFlex;


    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */   
        return(LDT_SEQERR_ADR);                             /* sequence error */
    }

    /* check minor class */

    switch(pData->uchMinorClass) {
    case CLASS_PARAFLEXMEM_RECORD:

        /* distinguish address */

        switch(MaintWork.FlexMem.uchAddress) {
        case FLEX_DEPT_ADR:

            /* check the number of DEPT */

            if (pData->ulRecordNumber > FLEX_DEPT_MAX) {    /* FLEX_DEPT_MAX=50 */
            /* if (pData->usRecordNumber > FLEX_DEPT_MAX) {    / FLEX_DEPT_MAX=50 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_PLU_ADR:

            /* check the number of PLU */

            if (pData->ulRecordNumber > FLEX_PLU_MAX) {     /* FLEX_PLU_MAX=1000 */
            /* if (pData->usRecordNumber > FLEX_PLU_MAX) {     / FLEX_PLU_MAX=1000 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_WT_ADR:

            /* check the number of waiter */

            if (pData->ulRecordNumber > FLEX_WT_MAX) {      /* FLEX_WT_MAX=100 */
            /* if (pData->usRecordNumber > FLEX_WT_MAX) {      / FLEX_WT_MAX=100 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_CAS_ADR:

            /* check the number of cashier */

            if (pData->ulRecordNumber > FLEX_CAS_MAX) {     /* FLEX_CAS_MAX=16 */
            /* if (pData->usRecordNumber > FLEX_CAS_MAX) {     / FLEX_CAS_MAX=16 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            /* V3.3 */
            /* MaintWork.FlexMem.uchPTDFlag = 0; */
            break;

        case FLEX_ETK_ADR:

            /* check the number of Block E/J */

            if (pData->ulRecordNumber > FLEX_ETK_MAX) {     /* FLEX_ETK_MAX=200 */
            /* if (pData->usRecordNumber > FLEX_ETK_MAX) {     / FLEX_ETK_MAX=200 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_EJ_ADR:

            /* check the number of Block E/J */

            if (pData->ulRecordNumber > FLEX_EJ_MAX) {      /* FLEX_EJ_MAX=500 */
            /* if (pData->usRecordNumber > FLEX_EJ_MAX) {      / FLEX_EJ_MAX=500 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_ITEMSTORAGE_ADR:
        case FLEX_CONSSTORAGE_ADR:

            /* check the number of item for item/cons. buffer */
                                                                    
            if ((pData->ulRecordNumber < FLEX_ITEMSTORAGE_MIN) ||   /* FLEX_ITEMSTORAGE_MIN=30 */
                (pData->ulRecordNumber > FLEX_ITEMSTORAGE_MAX)) {   /* FLEX_ITEMSTORAGE_MAX=200 */
            /* if ((pData->usRecordNumber < FLEX_ITEMSTORAGE_MIN) ||   / FLEX_ITEMSTORAGE_MIN=30 /
                (pData->usRecordNumber > FLEX_ITEMSTORAGE_MAX)) {   / FLEX_ITEMSTORAGE_MAX=200 */
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }

            if (MaintWork.FlexMem.uchAddress == FLEX_CONSSTORAGE_ADR) {
                ParaFlex.uchMajorClass = CLASS_PARAFLEXMEM;
                ParaFlex.uchAddress = FLEX_ITEMSTORAGE_ADR;
                CliParaRead(&ParaFlex);
                if (pData->ulRecordNumber < ParaFlex.ulRecordNumber) {
                /* if (pData->usRecordNumber < ParaFlex.usRecordNumber) { */
                    return(LDT_PROHBT_ADR);                     /* wrong data */
                }
            }
            MaintWork.FlexMem.uchPTDFlag = 0;
            break;

        case FLEX_GC_ADR:

            /* check the number of Guest Check */

            if (pData->ulRecordNumber > FLEX_GC_MAX) {      /* FLEX_GC_MAX=300 */
            /* if (pData->usRecordNumber > FLEX_GC_MAX) {      / FLEX_GC_MAX=300 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_CPN_ADR:

            /* check the number of Coupon */

            if (pData->ulRecordNumber > FLEX_CPN_MAX) {    /* FLEX_CPN_MAX=100 */
            /* if (pData->usRecordNumber > FLEX_CPN_MAX) {    / FLEX_CPN_MAX=100 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
            break;

        case FLEX_CSTRING_ADR:

            /* check the size of Control String */

            if (pData->ulRecordNumber > FLEX_CSTRING_MAX) {    /* FLEX_CSTRING_MAX=4000 */
            /* if (pData->usRecordNumber > FLEX_CSTRING_MAX) {    / FLEX_CSTRING_MAX=4000 */
                return(LDT_KEYOVER_ADR);                       /* wrong data */
            }
            break;

        case FLEX_PROGRPT_ADR:

            /* check the size of programable report */

            if (pData->ulRecordNumber > FLEX_PROGRPT_MAX) {    /* FLEX_PROGRPT_MAX=64 */
            /* if (pData->usRecordNumber > FLEX_PROGRPT_MAX) {    / FLEX_PROGRPT_MAX=64 */
                return(LDT_KEYOVER_ADR);                       /* wrong data */
            }
            break;

        case FLEX_PPI_ADR:

            /* check number of PPI table */

            if (pData->ulRecordNumber > FLEX_PPI_MAX) {        /* FLEX_PPI_MAX=300 ESMITH*/
            /* if (pData->usRecordNumber > FLEX_PPI_MAX) {        / FLEX_PPI_MAX=100 */
                return(LDT_KEYOVER_ADR);                       /* wrong data */
            }
            break;


        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_KEYOVER_ADR);                    /* wrong data */

        }
        MaintWork.FlexMem.ulRecordNumber = pData->ulRecordNumber;
        /* MaintWork.FlexMem.usRecordNumber = pData->usRecordNumber; */

        /* check address */

        /* if ((MaintWork.FlexMem.uchAddress == FLEX_CAS_ADR) V3.3 */
        if ((MaintWork.FlexMem.uchAddress == FLEX_ETK_ADR) 
            || (MaintWork.FlexMem.uchAddress == FLEX_ITEMSTORAGE_ADR) 
            || (MaintWork.FlexMem.uchAddress == FLEX_CONSSTORAGE_ADR)
            || (MaintWork.FlexMem.uchAddress == FLEX_CSTRING_ADR)
            || (MaintWork.FlexMem.uchAddress == FLEX_PROGRPT_ADR)
            || (MaintWork.FlexMem.uchAddress == FLEX_PPI_ADR)) {
            return(SUCCESS);                                /* PTD do not exist */
        }
        MaintWork.FlexMem.uchMinorClass = CLASS_PARAFLEXMEM_DISPFLAG;
        DispWrite(&(MaintWork.FlexMem));

        return(MAINT_PTD_EXIST);                            /* PTD exist */

    case CLASS_PARAFLEXMEM_PTD:

        /* check PTD range */

        if (MaintWork.FlexMem.uchAddress == FLEX_GC_ADR) {
            if (pData->uchPTDFlag > 3) {							/* R3.0 */
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
        } else {
            if (pData->uchPTDFlag > 1) {
                return(LDT_KEYOVER_ADR);                    /* wrong data */
            }
        }
        MaintWork.FlexMem.uchPTDFlag = pData->uchPTDFlag;
        return(SUCCESS);

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);                    /* wrong data */
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintFlexMemWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS : create file & write data
*                       : do not create file
*                       : do not create file & print error code
*
** Description: This function sets the number of record and PTD status flag
*===========================================================================
*/

SHORT MaintFlexMemWrite( VOID )
{
    UCHAR           uchSaveAddr;
    SHORT           sReturn = 0;
    SHORT           sCnvErr; 
    ULONG           ulRec, ulConsRec;
    /* USHORT          usRec, usConsRec; */
    MAINTERRORCODE  MaintErrorCode;
    PARAFLEXMEM     ParaFlexMem, WorkPara;

    /* control header item */
    MaintHeaderCtl(PG_FLX_MEM, RPT_PRG_ADR);

    /* print data */
    MaintWork.FlexMem.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.FlexMem));

    /* Execute Disconnect Master /Backup Master */
    SerChangeInqStat();

    /* save address */
    uchSaveAddr = MaintWork.FlexMem.uchAddress;

    do {
        /* create file */
        if ((sReturn = CliCreateFile(MaintWork.FlexMem.uchAddress, 
                                     MaintWork.FlexMem.ulRecordNumber, 
                                     /* MaintWork.FlexMem.usRecordNumber, */
                                     MaintWork.FlexMem.uchPTDFlag)) != OP_PERFORM) {
            
            if (!(((MaintWork.FlexMem.uchAddress == FLEX_ITEMSTORAGE_ADR) ||
                (MaintWork.FlexMem.uchAddress == FLEX_CONSSTORAGE_ADR)) &&
                (sReturn == LDT_PROHBT_ADR))) {

                CliParaRead(&(MaintWork.FlexMem));                  /* call ParaFlexMemRead() */

                /* Execute Disconnect Master /Backup Master */
                SerChangeInqStat();

                /* re-create file in old data */
                if (CliCreateFile(MaintWork.FlexMem.uchAddress, 
                              MaintWork.FlexMem.ulRecordNumber, 
                              /* MaintWork.FlexMem.usRecordNumber, */
                              MaintWork.FlexMem.uchPTDFlag) != OP_PERFORM) {
                    PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_CODE_01);
                    PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTCREATE_FILE);
                }

                /*----- Re-Create Item Storage, R3.0 -----*/
                if (MaintWork.FlexMem.uchAddress == FLEX_CONSSTORAGE_ADR) {
                    WorkPara.uchMajorClass = CLASS_PARAFLEXMEM;   
                    WorkPara.uchAddress = FLEX_ITEMSTORAGE_ADR;
                    CliParaRead(&WorkPara);
                    if (WorkPara.ulRecordNumber <= MaintWork.FlexMem.ulRecordNumber) {
                        ulRec = WorkPara.ulRecordNumber;
                        ulConsRec = MaintWork.FlexMem.ulRecordNumber;
                        MaintWork.FlexMem.ulRecordNumber = ulRec;
                    } else {
                        ulConsRec = ulRec = MaintWork.FlexMem.ulRecordNumber;
                    }
                    if (CliCreateFile(FLEX_ITEMSTORAGE_ADR,
                              ulRec,
                              MaintWork.FlexMem.uchPTDFlag) != OP_PERFORM) {
						PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_CODE_02);
                        PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTCREATE_FILE);
                    }                    /* if (WorkPara.usRecordNumber <= MaintWork.FlexMem.usRecordNumber) { 
                        usRec = WorkPara.usRecordNumber;
                        usConsRec = MaintWork.FlexMem.usRecordNumber;
                        MaintWork.FlexMem.usRecordNumber = usRec;
                    } else {
                        usConsRec = usRec = MaintWork.FlexMem.usRecordNumber;
                    }
                    if (CliCreateFile(FLEX_ITEMSTORAGE_ADR,
                              usRec,
                              MaintWork.FlexMem.uchPTDFlag) != OP_PERFORM) {
                        PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTCREATE_FILE);
                    } */
                    MaintWork.FlexMem.uchAddress = (UCHAR)FLEX_ITEMSTORAGE_ADR;
                    CliParaWrite(&MaintWork.FlexMem);
                    if (uchSaveAddr == FLEX_CONSSTORAGE_ADR) {
                        MaintWork.FlexMem.uchAddress++;
                        MaintWork.FlexMem.ulRecordNumber = ulConsRec;
                        /* MaintWork.FlexMem.usRecordNumber = usConsRec; */
                    }
                }
            }

            /* convert error code */
            switch (MaintWork.FlexMem.uchAddress) {
            case FLEX_DEPT_ADR:
            case FLEX_PLU_ADR:
            case FLEX_CPN_ADR:
            case FLEX_PPI_ADR:
                sCnvErr = OpConvertError(sReturn);
                break;

            case FLEX_WT_ADR:
                sCnvErr = LDT_PROHBT_ADR;
                break;

            case FLEX_CAS_ADR:
                sCnvErr = CasConvertError(sReturn);
                break;

            case FLEX_ETK_ADR:
                sCnvErr = EtkConvertError(sReturn);
                break;

            case FLEX_EJ_ADR:
                sCnvErr = EJConvertError(sReturn);
                break;

            case FLEX_ITEMSTORAGE_ADR:
            case FLEX_CONSSTORAGE_ADR:
            case FLEX_CSTRING_ADR:
            case FLEX_PROGRPT_ADR:
                sCnvErr = LDT_FLFUL_ADR;              /* R3.0 */
                break;

            default:
                sCnvErr = GusConvertError(sReturn);
                break;

            }

            /* print error code */
            MaintErrorCode.uchMajorClass = CLASS_MAINTERRORCODE;
            MaintErrorCode.sErrorCode = sCnvErr;                                  /* File Full */
            MaintErrorCode.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
            PrtPrintItem(NULL, &MaintErrorCode);
            MaintWork.FlexMem.uchMinorClass = CLASS_PARAFLEXMEM_DISPRECORD;
            DispWrite(&(MaintWork.FlexMem));
            return(sCnvErr);
        }

        /* set data */
        CliParaWrite(&(MaintWork.FlexMem));

        /* set address for display next address */
        MaintWork.FlexMem.uchAddress++;

        /* check numer of cons. storage item */
        ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;   
        ParaFlexMem.uchAddress = FLEX_CONSSTORAGE_ADR;
        CliParaRead(&ParaFlexMem);                      
    }
    while ((MaintWork.FlexMem.uchAddress == FLEX_CONSSTORAGE_ADR) &&
           (MaintWork.FlexMem.ulRecordNumber > ParaFlexMem.ulRecordNumber));
           /* (MaintWork.FlexMem.usRecordNumber > ParaFlexMem.usRecordNumber)); */

    /* check address */
    if (uchSaveAddr == FLEX_ITEMSTORAGE_ADR) {
        MaintWork.FlexMem.uchAddress = FLEX_CONSSTORAGE_ADR;
    }
    if (MaintWork.FlexMem.uchAddress > FLEX_ADR_MAX) {
        MaintWork.FlexMem.uchAddress = FLEX_DEPT_ADR;       /* initialize address */
    }
    MaintFlexMemRead(&(MaintWork.FlexMem));      
    return(SUCCESS);
}

/*
*==============================================================================
** Synopsis:    VOID MaintFlexMemReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports the number of record and PTD status flag
*==============================================================================
*/

VOID MaintFlexMemReport( VOID )
{

    UCHAR        i;
    PARAFLEXMEM  ParaFlexMem;

    /* control header item */

    MaintHeaderCtl(PG_FLX_MEM, RPT_PRG_ADR);

    /* set major class */

    ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;

    /* print data */

    ParaFlexMem.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */

    for (i = FLEX_DEPT_ADR; i <= FLEX_ADR_MAX; i++) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {              /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        /* get data & print */

        ParaFlexMem.uchAddress = i;
        CliParaRead(&ParaFlexMem);                          /* call ParaFlexMemRead() */
        PrtPrintItem(NULL, &ParaFlexMem);
    }
    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}

/****** end of source ******/
